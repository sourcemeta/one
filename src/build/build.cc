#include <sourcemeta/one/build.h>

#include "build_state.h"

#include <sourcemeta/core/jsonschema.h>

#include <algorithm> // std::ranges::none_of
#include <cassert>   // assert
#include <fstream>   // std::ofstream
#include <string>    // std::string

using mark_type = sourcemeta::one::Build::mark_type;
using Entry = sourcemeta::one::Build::Entry;

namespace sourcemeta::one {

Build::Build(const std::filesystem::path &output_root)
    : root{(static_cast<void>(std::filesystem::create_directories(output_root)),
            std::filesystem::canonical(output_root))},
      root_string{this->root.string()} {
  const auto state_path{this->root / STATE_FILENAME};
  if (!std::filesystem::exists(state_path)) {
    for (const auto &entry :
         std::filesystem::recursive_directory_iterator(this->root)) {
      auto &map_entry{this->entries_[entry.path().native()]};
      map_entry.tracked = false;
      map_entry.is_directory = entry.is_directory();
    }

    return;
  }

  try {
    this->has_previous_run = load_state(state_path, this->entries_);
  } catch (...) {
    this->entries_.clear();
  }
}

auto Build::find_entry(const std::string &key) const -> const Entry * {
  if (this->has_writes_.load(std::memory_order_relaxed)) {
    std::lock_guard<std::mutex> lock{this->write_mutex_};
    const auto match{this->writes_.find(key)};
    if (match != this->writes_.end()) {
      return &match->second;
    }
  }

  const auto match{this->entries_.find(key)};
  if (match != this->entries_.end()) {
    return &match->second;
  }

  return nullptr;
}

auto Build::find_mark(const std::filesystem::path &path) const
    -> std::optional<mark_type> {
  assert(path.is_absolute());
  const auto *entry{this->find_entry(path.native())};
  if (entry != nullptr && entry->file_mark.has_value()) {
    return entry->file_mark;
  }

  try {
    return std::filesystem::last_write_time(path);
  } catch (const std::filesystem::filesystem_error &) {
    return std::nullopt;
  }
}

auto Build::has_dependencies(const std::filesystem::path &path) const -> bool {
  assert(path.is_absolute());
  const auto *entry{this->find_entry(path.native())};
  return entry != nullptr && (!entry->static_dependencies.empty() ||
                              !entry->dynamic_dependencies.empty());
}

auto Build::merge_writes() -> void {
  if (!this->has_writes_.load(std::memory_order_relaxed)) {
    return;
  }

  std::lock_guard<std::mutex> lock{this->write_mutex_};
  for (auto &[key, write_entry] : this->writes_) {
    auto &target{this->entries_[key]};
    target.file_mark = write_entry.file_mark;
    target.static_dependencies = std::move(write_entry.static_dependencies);
    target.dynamic_dependencies = std::move(write_entry.dynamic_dependencies);
    target.tracked.store(write_entry.tracked.load(std::memory_order_relaxed),
                         std::memory_order_relaxed);
    target.is_directory.store(
        write_entry.is_directory.load(std::memory_order_relaxed),
        std::memory_order_relaxed);
  }

  this->writes_.clear();
  this->has_writes_.store(false, std::memory_order_relaxed);
}

auto Build::entries() -> const std::unordered_map<std::string, Entry> & {
  this->merge_writes();
  return this->entries_;
}

auto Build::finish() -> void {
  this->merge_writes();
  const auto state_path{this->root / STATE_FILENAME};
  save_state(state_path, this->entries_);
  this->track(state_path);

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

  const auto check_mtime{
      [this, &entry](const std::filesystem::path &dependency_path) {
        const auto dependency_mark{this->find_mark(dependency_path)};
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
  const auto file_mark{std::filesystem::file_time_type::clock::now()};
  std::lock_guard<std::mutex> lock{this->write_mutex_};
  auto &entry{this->writes_[destination.native()]};
  entry.file_mark = file_mark;
  entry.static_dependencies = std::move(static_dependencies);
  entry.dynamic_dependencies = std::move(dynamic_dependencies);
  entry.tracked.store(true, std::memory_order_relaxed);
  this->has_writes_.store(true, std::memory_order_relaxed);

  // Track parent directories for new entries (already holding write_mutex_)
  const auto &path_string{destination.native()};
  auto parent_key{path_string};
  while (true) {
    const auto slash{parent_key.rfind('/')};
    if (slash == std::string::npos || slash < this->root_string.size()) {
      break;
    }

    parent_key = parent_key.substr(0, slash);
    const auto parent_in_entries{this->entries_.find(parent_key)};
    if (parent_in_entries != this->entries_.end()) {
      if (parent_in_entries->second.tracked.load(std::memory_order_relaxed)) {
        break;
      }

      parent_in_entries->second.tracked.store(true, std::memory_order_relaxed);
      parent_in_entries->second.is_directory.store(true,
                                                   std::memory_order_relaxed);
    } else {
      auto &write_parent{this->writes_[parent_key]};
      if (write_parent.tracked.load(std::memory_order_relaxed)) {
        break;
      }

      write_parent.tracked.store(true, std::memory_order_relaxed);
      write_parent.is_directory.store(true, std::memory_order_relaxed);
    }
  }
}

auto Build::refresh(const std::filesystem::path &path) -> void {
  const auto value{std::filesystem::file_time_type::clock::now()};
  const auto &path_string{path.native()};
  const auto match{this->entries_.find(path_string)};
  if (match != this->entries_.end()) {
    match->second.file_mark = value;
    return;
  }

  std::lock_guard<std::mutex> lock{this->write_mutex_};
  this->writes_[path_string].file_mark = value;
  this->has_writes_.store(true, std::memory_order_relaxed);
}

auto Build::track_unsafe(const std::filesystem::path &path) -> void {
  assert(path.is_absolute());
  const auto &path_string{path.native()};
  const auto destination_match{this->entries_.find(path_string)};
  if (destination_match != this->entries_.end()) {
    destination_match->second.tracked.store(true, std::memory_order_relaxed);
  }

  auto parent_key{path_string};
  while (true) {
    const auto slash{parent_key.rfind('/')};
    if (slash == std::string::npos || slash < this->root_string.size()) {
      break;
    }

    parent_key = parent_key.substr(0, slash);
    const auto parent_match{this->entries_.find(parent_key)};
    if (parent_match == this->entries_.end()) {
      break;
    }

    if (parent_match->second.tracked.load(std::memory_order_relaxed)) {
      break;
    }

    parent_match->second.tracked.store(true, std::memory_order_relaxed);
    parent_match->second.is_directory.store(true, std::memory_order_relaxed);
  }
}

auto Build::track(const std::filesystem::path &path) -> void {
  assert(path.is_absolute());
  const auto &path_string{path.native()};

  const auto match{this->entries_.find(path_string)};
  if (match != this->entries_.end()) {
    match->second.tracked.store(true, std::memory_order_relaxed);
  } else {
    std::lock_guard<std::mutex> lock{this->write_mutex_};
    this->writes_[path_string].tracked.store(true, std::memory_order_relaxed);
    this->has_writes_.store(true, std::memory_order_relaxed);
  }

  auto parent_key{path_string};
  while (true) {
    const auto slash{parent_key.rfind('/')};
    if (slash == std::string::npos || slash < this->root_string.size()) {
      break;
    }

    parent_key = parent_key.substr(0, slash);
    const auto parent_in_entries{this->entries_.find(parent_key)};
    if (parent_in_entries != this->entries_.end()) {
      if (parent_in_entries->second.tracked.load(std::memory_order_relaxed)) {
        break;
      }

      parent_in_entries->second.tracked.store(true, std::memory_order_relaxed);
      parent_in_entries->second.is_directory.store(true,
                                                   std::memory_order_relaxed);
    } else {
      std::lock_guard<std::mutex> lock{this->write_mutex_};
      auto &write_parent{this->writes_[parent_key]};
      if (write_parent.tracked.load(std::memory_order_relaxed)) {
        break;
      }

      write_parent.tracked.store(true, std::memory_order_relaxed);
      write_parent.is_directory.store(true, std::memory_order_relaxed);
      this->has_writes_.store(true, std::memory_order_relaxed);
    }
  }
}

auto Build::is_untracked_file(const std::filesystem::path &path) const -> bool {
  const auto *entry{this->find_entry(path.native())};
  return entry == nullptr || !entry->tracked.load(std::memory_order_relaxed);
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
