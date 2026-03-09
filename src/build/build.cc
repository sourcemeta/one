#include <sourcemeta/one/build.h>

#include <sourcemeta/core/io.h>
#include <sourcemeta/core/jsonschema.h>

#include <algorithm> // std::ranges::none_of
#include <cassert>   // assert
#include <chrono>    // std::chrono::nanoseconds, std::chrono::duration_cast
#include <cstdint>   // std::int64_t, std::uint32_t, std::uint8_t
#include <cstring>   // std::memcpy
#include <fstream>   // std::ofstream

#include <mutex>  // std::unique_lock
#include <string> // std::string

static constexpr std::string_view DEPENDENCIES_FILE{"state.bin"};
static constexpr std::uint32_t DEPS_MAGIC{0x44455053};
static constexpr std::uint32_t DEPS_VERSION{1};
static constexpr std::uint8_t FLAG_HAS_DEPENDENCIES{0x01};
static constexpr std::uint8_t FLAG_HAS_MARK{0x02};

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

static auto append_uint32(std::string &buffer, const std::uint32_t value)
    -> void {
  buffer.append(reinterpret_cast<const char *>(&value), sizeof(value));
}

static auto append_int64(std::string &buffer, const std::int64_t value)
    -> void {
  buffer.append(reinterpret_cast<const char *>(&value), sizeof(value));
}

static auto append_string(std::string &buffer, const std::string &value)
    -> void {
  append_uint32(buffer, static_cast<std::uint32_t>(value.size()));
  buffer.append(value);
}

static auto read_uint32(const std::uint8_t *data, std::size_t &offset)
    -> std::uint32_t {
  std::uint32_t value;
  std::memcpy(&value, data + offset, sizeof(value));
  offset += sizeof(value);
  return value;
}

static auto read_int64(const std::uint8_t *data, std::size_t &offset)
    -> std::int64_t {
  std::int64_t value;
  std::memcpy(&value, data + offset, sizeof(value));
  offset += sizeof(value);
  return value;
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
    const sourcemeta::core::FileView view{dependencies_path};
    const auto *data{view.as<std::uint8_t>()};
    const auto file_size{view.size()};

    if (file_size < 12) {
      return;
    }

    std::size_t offset{0};
    const auto magic{read_uint32(data, offset)};
    if (magic != DEPS_MAGIC) {
      return;
    }

    const auto version{read_uint32(data, offset)};
    if (version != DEPS_VERSION) {
      return;
    }

    const auto entry_count{read_uint32(data, offset)};
    for (std::uint32_t index = 0; index < entry_count; ++index) {
      const auto path_length{read_uint32(data, offset)};
      std::string entry_path{reinterpret_cast<const char *>(data + offset),
                             path_length};
      offset += path_length;

      const auto flags{data[offset++]};
      auto &map_entry{this->entries_[std::move(entry_path)]};

      if ((flags & FLAG_HAS_DEPENDENCIES) != 0) {
        const auto static_count{read_uint32(data, offset)};
        map_entry.static_dependencies.reserve(static_count);
        for (std::uint32_t static_index = 0; static_index < static_count;
             ++static_index) {
          const auto dep_length{read_uint32(data, offset)};
          map_entry.static_dependencies.emplace_back(std::string{
              reinterpret_cast<const char *>(data + offset), dep_length});
          offset += dep_length;
        }

        const auto dynamic_count{read_uint32(data, offset)};
        map_entry.dynamic_dependencies.reserve(dynamic_count);
        for (std::uint32_t dynamic_index = 0; dynamic_index < dynamic_count;
             ++dynamic_index) {
          const auto dep_length{read_uint32(data, offset)};
          map_entry.dynamic_dependencies.emplace_back(std::string{
              reinterpret_cast<const char *>(data + offset), dep_length});
          offset += dep_length;
        }
      }

      if ((flags & FLAG_HAS_MARK) != 0) {
        const auto nanoseconds{read_int64(data, offset)};
        map_entry.file_mark =
            mark_type{std::chrono::duration_cast<mark_type::duration>(
                std::chrono::nanoseconds{nanoseconds})};
      }
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

  std::string buffer;
  // Reserve header space
  buffer.resize(12);
  std::memcpy(buffer.data(), &DEPS_MAGIC, sizeof(DEPS_MAGIC));
  std::memcpy(buffer.data() + 4, &DEPS_VERSION, sizeof(DEPS_VERSION));

  std::uint32_t count{0};
  std::shared_lock lock{this->mutex};
  for (const auto &entry : this->entries_) {
    if (!entry.second.tracked) {
      continue;
    }

    ++count;
    append_string(buffer, entry.first);

    const bool has_dependencies{!entry.second.static_dependencies.empty() ||
                                !entry.second.dynamic_dependencies.empty()};
    const bool has_mark{entry.second.file_mark.has_value()};

    std::uint8_t flags{0};
    if (has_dependencies) {
      flags |= FLAG_HAS_DEPENDENCIES;
    }
    if (has_mark) {
      flags |= FLAG_HAS_MARK;
    }

    buffer.push_back(static_cast<char>(flags));

    if (has_dependencies) {
      append_uint32(buffer, static_cast<std::uint32_t>(
                                entry.second.static_dependencies.size()));
      for (const auto &dependency : entry.second.static_dependencies) {
        append_string(buffer, dependency.native());
      }

      append_uint32(buffer, static_cast<std::uint32_t>(
                                entry.second.dynamic_dependencies.size()));
      for (const auto &dependency : entry.second.dynamic_dependencies) {
        append_string(buffer, dependency.native());
      }
    }

    if (has_mark) {
      const auto nanoseconds{
          std::chrono::duration_cast<std::chrono::nanoseconds>(
              entry.second.file_mark.value().time_since_epoch())
              .count()};
      append_int64(buffer, static_cast<std::int64_t>(nanoseconds));
    }
  }

  std::memcpy(buffer.data() + 8, &count, sizeof(count));

  std::ofstream stream{dependencies_path, std::ios::binary};
  assert(!stream.fail());
  stream.write(buffer.data(), static_cast<std::streamsize>(buffer.size()));
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
