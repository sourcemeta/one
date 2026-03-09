#include <sourcemeta/one/build.h>

#include <sourcemeta/core/jsonschema.h>

#include <algorithm> // std::ranges::none_of
#include <cassert>   // assert
#include <chrono>    // std::chrono::nanoseconds, std::chrono::duration_cast
#include <cstdint>   // std::int64_t
#include <fstream>   // std::ofstream, std::ifstream

#include <mutex>       // std::unique_lock
#include <string>      // std::string
#include <string_view> // std::string_view

static constexpr std::string_view DEPENDENCIES_FILE{"deps.txt"};

using mark_type = sourcemeta::one::Build::mark_type;
using Entry = sourcemeta::one::Build::Entry;

static auto mark_locked(const std::unordered_map<std::string, Entry> &entries,
                        const std::filesystem::path &path)
    -> std::optional<mark_type> {
  assert(path.is_absolute());
  const auto match{entries.find(path.native())};
  if (match != entries.end() && match->second.file_mark.has_value()) {
    return match->second.file_mark;
  }

  try {
    return std::filesystem::last_write_time(path);
  } catch (const std::filesystem::filesystem_error &) {
    return std::nullopt;
  }
}

namespace sourcemeta::one {

Build::Build(const std::filesystem::path &output_root)
    : root{(static_cast<void>(std::filesystem::create_directories(output_root)),
            std::filesystem::canonical(output_root))},
      root_string{this->root.string()} {
  const auto dependencies_path{this->root / DEPENDENCIES_FILE};
  if (!std::filesystem::exists(dependencies_path)) {
    // First run or crash recovery: scan directory for orphaned files
    for (const auto &entry :
         std::filesystem::recursive_directory_iterator(this->root)) {
      auto &map_entry{this->entries_[entry.path().native()]};
      map_entry.tracked = false;
      map_entry.is_directory = entry.is_directory();
    }

    return;
  }

  try {
    std::ifstream stream{dependencies_path};
    if (!stream.is_open()) {
      return;
    }

    std::string contents{std::istreambuf_iterator<char>(stream),
                         std::istreambuf_iterator<char>()};

    std::string current_key;
    std::vector<std::filesystem::path> current_static_dependencies;
    std::vector<std::filesystem::path> current_dynamic_dependencies;
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
            auto &previous_entry{this->entries_[current_key]};
            previous_entry.static_dependencies =
                std::move(current_static_dependencies);
            previous_entry.dynamic_dependencies =
                std::move(current_dynamic_dependencies);
            current_static_dependencies = {};
            current_dynamic_dependencies = {};
          }

          current_key = value;
          break;
        case 's':
          current_static_dependencies.emplace_back(std::string{value});
          break;
        case 'd':
          current_dynamic_dependencies.emplace_back(std::string{value});
          break;
        case 'e':
          this->entries_[std::string{value}];
          break;
        case 'm': {
          const auto space{value.find(' ')};
          if (space != std::string_view::npos) {
            const auto path_part{value.substr(0, space)};
            const auto nanoseconds_part{value.substr(space + 1)};
            const std::chrono::nanoseconds nanoseconds{
                std::stoll(std::string{nanoseconds_part})};
            const auto mark_value{mark_type{
                std::chrono::duration_cast<mark_type::duration>(nanoseconds)}};
            this->entries_[std::string{path_part}].file_mark = mark_value;
          }

          break;
        }
        default:
          break;
      }

      position = newline + 1;
    }

    if (!current_key.empty()) {
      auto &last_entry{this->entries_[current_key]};
      last_entry.static_dependencies = std::move(current_static_dependencies);
      last_entry.dynamic_dependencies = std::move(current_dynamic_dependencies);
    }
    this->has_previous_run = true;
  } catch (...) {
    this->entries_.clear();
  }
}

auto Build::has_dependencies(const std::filesystem::path &path) const -> bool {
  assert(path.is_absolute());
  std::shared_lock lock{this->mutex};
  const auto match{this->entries_.find(path.native())};
  return match != this->entries_.end() &&
         (!match->second.static_dependencies.empty() ||
          !match->second.dynamic_dependencies.empty());
}

auto Build::finish() -> void {
  const auto dependencies_path{this->root / DEPENDENCIES_FILE};
  std::ofstream stream{dependencies_path};
  assert(!stream.fail());

  std::shared_lock lock{this->mutex};
  for (const auto &entry : this->entries_) {
    if (!entry.second.tracked) {
      continue;
    }

    if (!entry.second.static_dependencies.empty() ||
        !entry.second.dynamic_dependencies.empty()) {
      stream << "t " << entry.first << '\n';

      for (const auto &dependency : entry.second.static_dependencies) {
        stream << "s " << dependency.native() << '\n';
      }

      for (const auto &dependency : entry.second.dynamic_dependencies) {
        stream << "d " << dependency.native() << '\n';
      }
    }

    if (entry.second.file_mark.has_value()) {
      const auto nanoseconds{
          std::chrono::duration_cast<std::chrono::nanoseconds>(
              entry.second.file_mark.value().time_since_epoch())
              .count()};
      stream << "m " << entry.first << ' '
             << static_cast<std::int64_t>(nanoseconds) << '\n';
    } else if (entry.second.static_dependencies.empty() &&
               entry.second.dynamic_dependencies.empty()) {
      stream << "e " << entry.first << '\n';
    }
  }

  stream.flush();
  stream.close();
  lock.unlock();
  this->track(dependencies_path);

  // Remove untracked files inside the output directory
  std::shared_lock read_lock{this->mutex};
  for (const auto &entry : this->entries_) {
    if (!entry.second.tracked &&
        entry.first.size() > this->root_string.size() &&
        entry.first.starts_with(this->root_string)) {
      std::filesystem::remove_all(entry.first);
    }
  }
}

auto Build::dispatch_is_cached(const Entry &entry,
                               const bool static_dependencies_match) const
    -> bool {
  if (!static_dependencies_match) {
    return false;
  }

  const auto check_mtime{[this, &entry](
                             const std::filesystem::path &dependency_path) {
    const auto dependency_mark{mark_locked(this->entries_, dependency_path)};
    return !dependency_mark.has_value() ||
           dependency_mark.value() > entry.file_mark.value();
  }};

  return std::ranges::none_of(entry.static_dependencies, check_mtime) &&
         std::ranges::none_of(entry.dynamic_dependencies, check_mtime);
}

auto Build::dispatch_commit(
    const std::filesystem::path &destination,
    std::vector<std::filesystem::path> &&static_dependencies,
    std::vector<std::filesystem::path> &&dynamic_dependencies) -> void {
  assert(destination.is_absolute());
  assert(std::filesystem::exists(destination));
  this->refresh(destination);
  this->track(destination);
  std::unique_lock lock{this->mutex};
  auto &entry{this->entries_[destination.native()]};
  entry.static_dependencies = std::move(static_dependencies);
  entry.dynamic_dependencies = std::move(dynamic_dependencies);
}

auto Build::refresh(const std::filesystem::path &path) -> void {
  const auto value{std::filesystem::file_time_type::clock::now()};
  std::unique_lock lock{this->mutex};
  this->entries_[path.native()].file_mark = value;
}

auto Build::track(const std::filesystem::path &path) -> void {
  assert(path.is_absolute());
  const auto &path_string{path.native()};
  std::unique_lock lock{this->mutex};
  this->entries_[path_string].tracked = true;
  auto parent_key{path_string};
  while (true) {
    const auto slash{parent_key.rfind('/')};
    if (slash == std::string::npos || slash < this->root_string.size()) {
      break;
    }

    parent_key = parent_key.substr(0, slash);
    auto &parent_entry{this->entries_[parent_key]};
    if (parent_entry.tracked) {
      break;
    }

    parent_entry.tracked = true;
    parent_entry.is_directory = true;
  }
}

auto Build::is_untracked_file(const std::filesystem::path &path) const -> bool {
  std::shared_lock lock{this->mutex};
  const auto match{this->entries_.find(path.native())};
  return match == this->entries_.cend() || !match->second.tracked;
}

auto Build::write_json_if_different(const std::filesystem::path &path,
                                    const sourcemeta::core::JSON &document)
    -> void {
  if (std::filesystem::exists(path)) {
    const auto current{sourcemeta::core::read_json(path)};
    if (current == document) {
      this->track(path);
      return;
    }
  }

  assert(path.is_absolute());
  std::filesystem::create_directories(path.parent_path());
  std::ofstream stream{path};
  assert(!stream.fail());
  sourcemeta::core::stringify(document, stream);
  this->track(path);
  this->refresh(path);
}

} // namespace sourcemeta::one
