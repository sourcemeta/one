#include <sourcemeta/one/build_adapter_filesystem.h>

#include <sourcemeta/core/io.h>

#include <cassert>     // assert
#include <chrono>      // std::chrono::nanoseconds, std::chrono::duration_cast
#include <cstdint>     // std::int64_t
#include <fstream>     // std::ofstream, std::ifstream
#include <mutex>       // std::unique_lock
#include <string>      // std::string
#include <string_view> // std::string_view

static constexpr std::string_view DEPENDENCIES_FILE{"deps.txt"};

namespace sourcemeta::one {

BuildAdapterFilesystem::BuildAdapterFilesystem(
    const std::filesystem::path &output_root)
    : root{std::filesystem::canonical(output_root)} {
  const auto deps_path{this->root / DEPENDENCIES_FILE};
  if (!std::filesystem::exists(deps_path)) {
    return;
  }

  try {
    std::ifstream stream{deps_path};
    if (!stream.is_open()) {
      return;
    }

    std::string contents{std::istreambuf_iterator<char>(stream),
                         std::istreambuf_iterator<char>()};

    std::string current_key;
    BuildDependencies<node_type> current_deps;
    std::size_t position{0};

    while (position < contents.size()) {
      auto newline{contents.find('\n', position)};
      if (newline == std::string::npos) {
        newline = contents.size();
      }

      if (newline <= position + 2 || contents[position + 1] != ' ') {
        position = newline + 1;
        continue;
      }

      const char tag{contents[position]};
      const std::string_view value{contents.data() + position + 2,
                                   newline - position - 2};

      switch (tag) {
        case 't':
          if (!current_key.empty()) {
            this->dependencies_map.insert_or_assign(current_key,
                                                    std::move(current_deps));
            current_deps = {};
          }

          current_key = value;
          break;
        case 's':
          current_deps.emplace_back(
              BuildDependencyKind::Static,
              (this->root / std::string{value}).lexically_normal());
          break;
        case 'd':
          current_deps.emplace_back(
              BuildDependencyKind::Dynamic,
              (this->root / std::string{value}).lexically_normal());
          break;
        case 'm': {
          const auto space{value.find(' ')};
          if (space != std::string_view::npos) {
            const auto path_part{value.substr(0, space)};
            const auto ns_part{value.substr(space + 1)};
            const std::chrono::nanoseconds nanoseconds{
                std::stoll(std::string{ns_part})};
            const auto mark_value{mark_type{
                std::chrono::duration_cast<mark_type::duration>(nanoseconds)}};
            this->marks.insert_or_assign(
                (this->root / std::string{path_part}).lexically_normal(),
                mark_value);
          }

          break;
        }
        default:
          break;
      }

      position = newline + 1;
    }

    if (!current_key.empty()) {
      this->dependencies_map.insert_or_assign(current_key,
                                              std::move(current_deps));
    }
    this->has_previous_run = true;
  } catch (...) {
    this->dependencies_map.clear();
    this->marks.clear();
  }
}

auto BuildAdapterFilesystem::read_dependencies(const node_type &path) const
    -> std::optional<BuildDependencies<node_type>> {
  assert(path.is_absolute());
  const auto key{path.lexically_relative(this->root).string()};
  std::shared_lock lock{this->dependencies_mutex};
  const auto match{this->dependencies_map.find(key)};
  if (match == this->dependencies_map.end() || match->second.empty()) {
    return std::nullopt;
  }

  return match->second;
}

auto BuildAdapterFilesystem::write_dependencies(
    const node_type &path, const BuildDependencies<node_type> &dependencies)
    -> void {
  assert(path.is_absolute());
  assert(std::filesystem::exists(path));
  sourcemeta::core::flush(path);
  this->refresh(path);
  const auto key{path.lexically_relative(this->root).string()};
  std::unique_lock lock{this->dependencies_mutex};
  this->dependencies_map.insert_or_assign(key, dependencies);
}

auto BuildAdapterFilesystem::flush_dependencies(
    const std::function<bool(const node_type &)> &filter) -> void {
  const auto deps_path{this->root / DEPENDENCIES_FILE};
  std::ofstream stream{deps_path};
  assert(!stream.fail());

  for (const auto &entry : this->dependencies_map) {
    if (!filter(this->root / entry.first)) {
      continue;
    }

    stream << "t " << entry.first << '\n';
    for (const auto &dependency : entry.second) {
      const char kind_char{
          dependency.first == BuildDependencyKind::Dynamic ? 'd' : 's'};
      const auto relative{dependency.second.lexically_relative(this->root)};
      if (!relative.empty() && *relative.begin() != "..") {
        stream << kind_char << ' ' << relative.string() << '\n';
      } else {
        stream << kind_char << ' ' << dependency.second.string() << '\n';
      }
    }
  }

  for (const auto &entry : this->marks) {
    const auto relative{entry.first.lexically_relative(this->root)};
    if (!relative.empty() && *relative.begin() != "..") {
      const auto nanoseconds{
          std::chrono::duration_cast<std::chrono::nanoseconds>(
              entry.second.time_since_epoch())
              .count()};
      stream << "m " << relative.string() << ' '
             << static_cast<std::int64_t>(nanoseconds) << '\n';
    }
  }

  stream.flush();
  stream.close();
}

auto BuildAdapterFilesystem::refresh(const node_type &path) -> void {
  // We prefer our own computed marks so that we don't have to rely
  // too much on file-system specific non-sense
  const auto value{std::filesystem::file_time_type::clock::now()};
  // Because builds are typically ran in parallel
  std::unique_lock lock{this->mutex};
  this->marks.insert_or_assign(path, value);
}

auto BuildAdapterFilesystem::mark(const node_type &path)
    -> std::optional<mark_type> {
  assert(path.is_absolute());

  {
    // Because a read operation while a write operation is taking place is
    // undefined behaviour
    std::shared_lock lock{this->mutex};
    const auto match{this->marks.find(path)};
    if (match != this->marks.end()) {
      return match->second;
    }
  }

  // Output files should always have their marks cached
  // Only input files  or new output files are not
  assert(!this->has_previous_run ||
         !path.string().starts_with(this->root.string()) ||
         !std::filesystem::exists(path));

  try {
    const auto value{std::filesystem::last_write_time(path)};
    // Cache for the rest of this run since input files don't change
    std::unique_lock lock{this->mutex};
    this->marks.emplace(path, value);
    return value;
  } catch (const std::filesystem::filesystem_error &) {
    return std::nullopt;
  }
}

auto BuildAdapterFilesystem::is_newer_than(const mark_type left,
                                           const mark_type right) const
    -> bool {
  return left > right;
}

} // namespace sourcemeta::one
