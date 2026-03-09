#include <sourcemeta/one/build.h>

#include <sourcemeta/core/jsonschema.h>

#include <cassert>     // assert
#include <chrono>      // std::chrono::nanoseconds, std::chrono::duration_cast
#include <cstdint>     // std::int64_t
#include <fstream>     // std::ofstream, std::ifstream
#include <mutex>       // std::unique_lock
#include <string>      // std::string
#include <string_view> // std::string_view

static constexpr std::string_view DEPENDENCIES_FILE{"deps.txt"};

namespace sourcemeta::one {

auto Build::dependency(std::filesystem::path node)
    -> std::pair<DependencyKind, std::filesystem::path> {
  return {DependencyKind::Static, std::move(node)};
}

auto Build::key(const std::filesystem::path &path) const -> std::string {
  return path.lexically_relative(this->root).string();
}

Build::Build(const std::filesystem::path &output_root)
    : root{(static_cast<void>(std::filesystem::create_directories(output_root)),
            std::filesystem::canonical(output_root))} {
  for (const auto &entry :
       std::filesystem::recursive_directory_iterator(this->root)) {
    this->entries[this->key(entry.path())].tracked = false;
  }

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
    Build::Dependencies current_deps;
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
            this->entries[current_key].dependencies = std::move(current_deps);
            current_deps = {};
          }

          current_key = value;
          break;
        case 's':
          current_deps.emplace_back(
              Build::DependencyKind::Static,
              (this->root / std::string{value}).lexically_normal());
          break;
        case 'd':
          current_deps.emplace_back(
              Build::DependencyKind::Dynamic,
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
            this->entries[std::string{path_part}].file_mark = mark_value;
          }

          break;
        }
        default:
          break;
      }

      position = newline + 1;
    }

    if (!current_key.empty()) {
      this->entries[current_key].dependencies = std::move(current_deps);
    }
    this->has_previous_run = true;
  } catch (...) {
    this->entries.clear();
  }
}

auto Build::has_dependencies(const std::filesystem::path &path) const -> bool {
  assert(path.is_absolute());
  const auto entry_key{this->key(path)};
  std::shared_lock lock{this->mutex};
  const auto match{this->entries.find(entry_key)};
  return match != this->entries.end() && !match->second.dependencies.empty();
}

auto Build::finish() -> void {
  const auto deps_path{this->root / DEPENDENCIES_FILE};
  std::ofstream stream{deps_path};
  assert(!stream.fail());

  std::shared_lock lock{this->mutex};
  for (const auto &entry : this->entries) {
    if (!entry.second.tracked) {
      continue;
    }

    if (!entry.second.dependencies.empty()) {
      stream << "t " << entry.first << '\n';
      for (const auto &dependency : entry.second.dependencies) {
        const char kind_char{
            dependency.first == Build::DependencyKind::Dynamic ? 'd' : 's'};
        const auto relative{dependency.second.lexically_relative(this->root)};
        if (!relative.empty() && *relative.begin() != "..") {
          stream << kind_char << ' ' << relative.string() << '\n';
        } else {
          stream << kind_char << ' ' << dependency.second.string() << '\n';
        }
      }
    }

    if (entry.second.file_mark.has_value()) {
      // Only persist marks for files inside the output directory
      if (!entry.first.empty() && entry.first.front() != '.') {
        const auto nanoseconds{
            std::chrono::duration_cast<std::chrono::nanoseconds>(
                entry.second.file_mark.value().time_since_epoch())
                .count()};
        stream << "m " << entry.first << ' '
               << static_cast<std::int64_t>(nanoseconds) << '\n';
      }
    }
  }

  stream.flush();
  stream.close();
  lock.unlock();
  this->track(deps_path);

  // Remove untracked files inside the output directory
  std::shared_lock read_lock{this->mutex};
  for (const auto &entry : this->entries) {
    if (!entry.second.tracked && !entry.first.empty() &&
        entry.first.front() != '.') {
      const auto absolute_path{(this->root / entry.first).lexically_normal()};
      std::filesystem::remove_all(absolute_path);
    }
  }
}

auto Build::refresh(const std::filesystem::path &path) -> void {
  const auto value{std::filesystem::file_time_type::clock::now()};
  const auto entry_key{this->key(path)};
  std::unique_lock lock{this->mutex};
  this->entries[entry_key].file_mark = value;
}

auto Build::mark(const std::filesystem::path &path)
    -> std::optional<mark_type> {
  assert(path.is_absolute());
  const auto entry_key{this->key(path)};

  {
    std::shared_lock lock{this->mutex};
    const auto match{this->entries.find(entry_key)};
    if (match != this->entries.end() && match->second.file_mark.has_value()) {
      return match->second.file_mark;
    }
  }

  // Output files should always have their marks cached
  // Only input files or new output files are not
  const auto relative{path.lexically_relative(this->root)};
  assert(!this->has_previous_run || relative.empty() ||
         *relative.begin() == ".." || !std::filesystem::exists(path));

  try {
    const auto value{std::filesystem::last_write_time(path)};
    std::unique_lock lock{this->mutex};
    this->entries[entry_key].file_mark = value;
    return value;
  } catch (const std::filesystem::filesystem_error &) {
    return std::nullopt;
  }
}

auto Build::mark_locked(const std::filesystem::path &path) const
    -> std::optional<mark_type> {
  assert(path.is_absolute());
  const auto entry_key{this->key(path)};
  const auto match{this->entries.find(entry_key)};
  if (match != this->entries.end() && match->second.file_mark.has_value()) {
    return match->second.file_mark;
  }

  // For the locked variant used in dispatch cache-hit checks,
  // if the mark isn't cached, fall back to stat
  try {
    return std::filesystem::last_write_time(path);
  } catch (const std::filesystem::filesystem_error &) {
    return std::nullopt;
  }
}

auto Build::track(const std::filesystem::path &path) -> void {
  assert(path.is_absolute());
  const auto entry_key{this->key(path)};
  std::unique_lock lock{this->mutex};
  auto &entry{this->entries[entry_key]};
  assert(!entry.tracked);
  entry.tracked = true;
  for (auto current = path.parent_path();
       !current.empty() && current != this->root;
       current = current.parent_path()) {
    this->entries[this->key(current)].tracked = true;
  }
}

auto Build::is_untracked_file(const std::filesystem::path &path) const -> bool {
  const auto entry_key{this->key(path)};
  std::shared_lock lock{this->mutex};
  const auto match{this->entries.find(entry_key)};
  return match == this->entries.cend() || !match->second.tracked;
}

auto Build::output_write_json(const std::filesystem::path &path,
                              const sourcemeta::core::JSON &document) -> void {
  assert(path.is_absolute());
  std::filesystem::create_directories(path.parent_path());
  std::ofstream stream{path};
  assert(!stream.fail());
  sourcemeta::core::stringify(document, stream);
  this->track(path);
}

auto Build::write_json_if_different(const std::filesystem::path &path,
                                    const sourcemeta::core::JSON &document)
    -> void {
  if (std::filesystem::exists(path)) {
    const auto current{sourcemeta::core::read_json(path)};
    if (current != document) {
      this->output_write_json(path, document);
      this->refresh(path);
    } else {
      this->track(path);
    }
  } else {
    this->output_write_json(path, document);
    this->refresh(path);
  }
}

} // namespace sourcemeta::one
