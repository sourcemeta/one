#include <sourcemeta/one/build.h>

#include <sourcemeta/core/io.h>

#include <cassert> // assert
#include <chrono>  // std::chrono::nanoseconds, std::chrono::duration_cast
#include <cstdint> // std::int64_t, std::uint32_t, std::uint8_t
#include <cstring> // std::memcpy
#include <fstream> // std::ofstream
#include <string>  // std::string

namespace {

constexpr std::uint32_t STATE_MAGIC{0x44455053};
constexpr std::uint32_t STATE_VERSION{2};
constexpr std::uint8_t STATE_FLAG_HAS_DEPENDENCIES{0x01};

auto read_uint32(const std::uint8_t *data, std::size_t &offset)
    -> std::uint32_t {
  std::uint32_t value;
  std::memcpy(&value, data + offset, sizeof(value));
  offset += sizeof(value);
  return value;
}

auto read_int64(const std::uint8_t *data, std::size_t &offset) -> std::int64_t {
  std::int64_t value;
  std::memcpy(&value, data + offset, sizeof(value));
  offset += sizeof(value);
  return value;
}

auto append_uint32(std::string &buffer, const std::uint32_t value) -> void {
  buffer.append(reinterpret_cast<const char *>(&value), sizeof(value));
}

auto append_int64(std::string &buffer, const std::int64_t value) -> void {
  buffer.append(reinterpret_cast<const char *>(&value), sizeof(value));
}

auto append_string(std::string &buffer, const std::string &value) -> void {
  append_uint32(buffer, static_cast<std::uint32_t>(value.size()));
  buffer.append(value);
}

} // anonymous namespace

namespace sourcemeta::one {

using mark_type = std::filesystem::file_time_type;

auto load_state(const std::filesystem::path &path, BuildEntries &entries)
    -> bool {
  const sourcemeta::core::FileView view{path};
  const auto *data{view.as<std::uint8_t>()};
  const auto file_size{view.size()};

  if (file_size < 12) {
    return false;
  }

  std::size_t offset{0};
  if (read_uint32(data, offset) != STATE_MAGIC) {
    return false;
  }

  if (read_uint32(data, offset) != STATE_VERSION) {
    return false;
  }

  const auto entry_count{read_uint32(data, offset)};
  for (std::uint32_t index = 0; index < entry_count; ++index) {
    const auto path_length{read_uint32(data, offset)};
    std::string entry_path{reinterpret_cast<const char *>(data + offset),
                           path_length};
    offset += path_length;

    const auto flags{data[offset++]};
    auto &map_entry{entries[std::move(entry_path)]};

    if ((flags & STATE_FLAG_HAS_DEPENDENCIES) != 0) {
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

    const auto nanoseconds{read_int64(data, offset)};
    map_entry.file_mark =
        mark_type{std::chrono::duration_cast<mark_type::duration>(
            std::chrono::nanoseconds{nanoseconds})};
  }

  return true;
}

auto save_state(const std::filesystem::path &path, const BuildEntries &entries)
    -> void {
  std::string buffer;
  buffer.resize(12);
  std::memcpy(buffer.data(), &STATE_MAGIC, sizeof(STATE_MAGIC));
  std::memcpy(buffer.data() + 4, &STATE_VERSION, sizeof(STATE_VERSION));

  std::uint32_t count{0};
  for (const auto &entry : entries) {
    count += 1;
    append_string(buffer, entry.first);

    const bool has_dependencies{!entry.second.static_dependencies.empty() ||
                                !entry.second.dynamic_dependencies.empty()};
    std::uint8_t flags{0};
    if (has_dependencies) {
      flags |= STATE_FLAG_HAS_DEPENDENCIES;
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

    const auto nanoseconds{std::chrono::duration_cast<std::chrono::nanoseconds>(
                               entry.second.file_mark.time_since_epoch())
                               .count()};
    append_int64(buffer, static_cast<std::int64_t>(nanoseconds));
  }

  std::memcpy(buffer.data() + 8, &count, sizeof(count));

  std::ofstream stream{path, std::ios::binary};
  assert(!stream.fail());
  stream.write(buffer.data(), static_cast<std::streamsize>(buffer.size()));
}

} // namespace sourcemeta::one
