#include <sourcemeta/one/build_state.h>

#include <sourcemeta/core/io.h>

#include <cassert> // assert
#include <chrono>  // std::chrono::nanoseconds, std::chrono::duration_cast
#include <cstdint> // std::int64_t, std::uint32_t, std::uint8_t
#include <cstring> // std::memcpy
#include <fstream> // std::ofstream
#include <string>  // std::string

namespace {

constexpr std::uint32_t STATE_MAGIC{0x44455053};
constexpr std::uint32_t STATE_VERSION{1};
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

BuildState::BuildState() = default;
BuildState::~BuildState() = default;
BuildState::BuildState(BuildState &&) noexcept = default;
auto BuildState::operator=(BuildState &&) noexcept -> BuildState & = default;

auto BuildState::load(const std::filesystem::path &path) -> void {
  if (!std::filesystem::exists(path)) {
    return;
  }

  try {
    this->view = std::make_unique<sourcemeta::core::FileView>(path);
    this->view_data = this->view->as<std::uint8_t>();
    const auto file_size{this->view->size()};

    if (file_size < 12) {
      this->view.reset();
      this->view_data = nullptr;
      return;
    }

    std::size_t offset{0};
    if (read_uint32(this->view_data, offset) != STATE_MAGIC) {
      this->view.reset();
      this->view_data = nullptr;
      return;
    }

    if (read_uint32(this->view_data, offset) != STATE_VERSION) {
      this->view.reset();
      this->view_data = nullptr;
      return;
    }

    const auto count{read_uint32(this->view_data, offset)};
    this->mmap_index.reserve(count);

    for (std::uint32_t index = 0; index < count; ++index) {
      const auto path_length{read_uint32(this->view_data, offset)};
      std::string_view entry_key{
          reinterpret_cast<const char *>(this->view_data + offset),
          path_length};
      offset += path_length;

      const auto data_offset{offset};
      const auto flags{this->view_data[offset++]};

      if ((flags & STATE_FLAG_HAS_DEPENDENCIES) != 0) {
        const auto dependency_count{read_uint32(this->view_data, offset)};
        for (std::uint32_t dependency_index = 0;
             dependency_index < dependency_count; ++dependency_index) {
          const auto dependency_length{read_uint32(this->view_data, offset)};
          offset += dependency_length;
        }
      }

      const auto nanoseconds{read_int64(this->view_data, offset)};
      const auto file_mark{
          mark_type{std::chrono::duration_cast<mark_type::duration>(
              std::chrono::nanoseconds{nanoseconds})}};

      const auto data_length{offset - data_offset};
      this->mmap_index.emplace(entry_key,
                               MmapEntryInfo{.file_mark = file_mark,
                                             .data_offset = data_offset,
                                             .data_length = data_length});
    }

    this->entry_count = this->mmap_index.size();
  } catch (...) {
    this->mmap_index.clear();
    this->view.reset();
    this->view_data = nullptr;
    this->entry_count = 0;
    throw;
  }
}

auto BuildState::contains(const std::string &key) const -> bool {
  if (this->overlay.contains(key)) {
    return true;
  }

  if (this->deleted.contains(key)) {
    return false;
  }

  return this->mmap_index.contains(key);
}

auto BuildState::entry(const std::string &key) const -> const Entry * {
  const auto overlay_match{this->overlay.find(key)};
  if (overlay_match != this->overlay.end()) {
    return &overlay_match->second;
  }

  if (this->deleted.contains(key)) {
    return nullptr;
  }

  const auto mmap_match{this->mmap_index.find(key)};
  if (mmap_match == this->mmap_index.end()) {
    return nullptr;
  }

  return &this->parse_mmap_entry(mmap_match->first, mmap_match->second);
}

auto BuildState::is_stale(
    const std::string &key,
    const std::filesystem::file_time_type source_mtime) const -> bool {
  const auto overlay_match{this->overlay.find(key)};
  if (overlay_match != this->overlay.end()) {
    return source_mtime > overlay_match->second.file_mark;
  }

  if (this->deleted.contains(key)) {
    return true;
  }

  const auto mmap_match{this->mmap_index.find(key)};
  if (mmap_match == this->mmap_index.end()) {
    return true;
  }

  return source_mtime > mmap_match->second.file_mark;
}

auto BuildState::commit(const std::filesystem::path &path,
                        std::vector<std::filesystem::path> dependencies)
    -> void {
  const auto &key{path.native()};
  const auto was_live{
      this->overlay.contains(key) ||
      (this->mmap_index.contains(key) && !this->deleted.contains(key))};

  auto &result{this->overlay[key]};
  result.file_mark = std::filesystem::file_time_type::clock::now();
  result.dependencies = std::move(dependencies);
  this->deleted.erase(key);

  if (!was_live) {
    this->entry_count++;
  }

  this->dirty = true;
  this->keys_stale = true;
}

auto BuildState::forget(const std::string &key) -> void {
  const auto child_prefix{key + "/"};
  auto matches{[&](std::string_view candidate) -> bool {
    return candidate == key || candidate.starts_with(child_prefix);
  }};

  std::unordered_set<std::string, TransparentHash, TransparentEqual>
      removed_from_overlay;
  for (auto iterator = this->overlay.begin();
       iterator != this->overlay.end();) {
    if (matches(iterator->first)) {
      removed_from_overlay.insert(iterator->first);
      iterator = this->overlay.erase(iterator);
      this->entry_count--;
    } else {
      ++iterator;
    }
  }

  for (const auto &[mmap_key, info] : this->mmap_index) {
    if (matches(mmap_key) && !this->deleted.contains(mmap_key)) {
      this->deleted.emplace(mmap_key);
      this->lazy_cache.erase(std::string{mmap_key});
      if (!removed_from_overlay.contains(mmap_key)) {
        this->entry_count--;
      }
    }
  }

  this->dirty = true;
  this->keys_stale = true;
}

auto BuildState::emplace(const std::filesystem::path &path, Entry entry)
    -> void {
  const auto &key{path.native()};
  const auto was_live{
      this->overlay.contains(key) ||
      (this->mmap_index.contains(key) && !this->deleted.contains(key))};

  this->overlay[key] = std::move(entry);
  this->deleted.erase(key);

  if (!was_live) {
    this->entry_count++;
  }

  this->dirty = true;
  this->keys_stale = true;
}

auto BuildState::parse_mmap_entry(std::string_view key,
                                  const MmapEntryInfo &info) const
    -> const Entry & {
  const auto cache_match{this->lazy_cache.find(key)};
  if (cache_match != this->lazy_cache.end()) {
    return cache_match->second;
  }

  auto &cached{this->lazy_cache[std::string{key}]};
  cached.file_mark = info.file_mark;

  auto offset{info.data_offset};
  const auto flags{this->view_data[offset++]};
  if ((flags & STATE_FLAG_HAS_DEPENDENCIES) != 0) {
    const auto dependency_count{read_uint32(this->view_data, offset)};
    cached.dependencies.reserve(dependency_count);
    for (std::uint32_t dependency_index = 0;
         dependency_index < dependency_count; ++dependency_index) {
      const auto dependency_length{read_uint32(this->view_data, offset)};
      cached.dependencies.emplace_back(
          std::string{reinterpret_cast<const char *>(this->view_data + offset),
                      dependency_length});
      offset += dependency_length;
    }
  }

  return cached;
}

auto BuildState::save(const std::filesystem::path &path) const -> void {
  if (!this->dirty && this->view) {
    return;
  }

  std::string buffer;
  buffer.resize(12);
  std::memcpy(buffer.data(), &STATE_MAGIC, sizeof(STATE_MAGIC));
  std::memcpy(buffer.data() + 4, &STATE_VERSION, sizeof(STATE_VERSION));

  std::uint32_t count{0};

  for (const auto &[key, info] : this->mmap_index) {
    if (this->deleted.contains(key)) {
      continue;
    }

    if (this->overlay.contains(key)) {
      continue;
    }

    count += 1;
    append_uint32(buffer, static_cast<std::uint32_t>(key.size()));
    buffer.append(key);
    buffer.append(
        reinterpret_cast<const char *>(this->view_data + info.data_offset),
        info.data_length);
  }

  for (const auto &[entry_path, entry] : this->overlay) {
    count += 1;

    append_string(buffer, entry_path);

    const bool has_dependencies{!entry.dependencies.empty()};
    std::uint8_t flags{0};
    if (has_dependencies) {
      flags |= STATE_FLAG_HAS_DEPENDENCIES;
    }

    buffer.push_back(static_cast<char>(flags));

    if (has_dependencies) {
      append_uint32(buffer,
                    static_cast<std::uint32_t>(entry.dependencies.size()));
      for (const auto &dependency : entry.dependencies) {
        append_string(buffer, dependency.native());
      }
    }

    const auto nanoseconds{std::chrono::duration_cast<std::chrono::nanoseconds>(
                               entry.file_mark.time_since_epoch())
                               .count()};
    append_int64(buffer, static_cast<std::int64_t>(nanoseconds));
  }

  std::memcpy(buffer.data() + 8, &count, sizeof(count));

  std::ofstream stream{path, std::ios::binary};
  assert(!stream.fail());
  stream.write(buffer.data(), static_cast<std::streamsize>(buffer.size()));
}

} // namespace sourcemeta::one
