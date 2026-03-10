#include <sourcemeta/one/build.h>

#include "build_state.h"

#include <sourcemeta/core/jsonschema.h>

#include <algorithm> // std::ranges::none_of
#include <cassert>   // assert
#include <fstream>   // std::ofstream

#include <mutex>  // std::unique_lock
#include <string> // std::string

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
  const auto state_path{this->root / state::FILENAME};
  if (!std::filesystem::exists(state_path)) {
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
    this->has_previous_run = state::load(state_path, this->entries_);
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
  const auto state_path{this->root / state::FILENAME};

  {
    std::shared_lock lock{this->mutex};
    state::save(state_path, this->entries_);
  }

  this->track(state_path);

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
