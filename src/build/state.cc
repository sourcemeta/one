#include <sourcemeta/one/build_state.h>

#include <sourcemeta/core/io.h>

#include <cassert> // assert
#include <chrono>  // std::chrono::nanoseconds, std::chrono::duration_cast
#include <cstdint> // std::int64_t, std::uint16_t, std::uint32_t, std::uint64_t
#include <cstring> // std::memcpy, std::memcmp
#include <fstream> // std::ofstream
#include <string>  // std::string
#include <vector>  // std::vector

namespace {

constexpr std::uint32_t STATE_MAGIC{0x44455053};
constexpr std::uint32_t STATE_VERSION{2};

// File layout:
//   [Header: 24 bytes]
//   [Hash table: capacity * 32-byte slots]
//   [String pool: variable]
constexpr std::size_t HEADER_SIZE{24};

// Slot layout (32 bytes, open-addressing with linear probing):
//   0:  hash         u64
//   8:  key_offset   u32  (into string pool)
//   12: key_length   u32
//   16: timestamp    i64  (nanoseconds)
//   24: deps_offset  u32  (into string pool)
//   28: deps_count   u16
//   30: occupied     u8
//   31: padding      u8
constexpr std::size_t SLOT_SIZE{32};
constexpr std::size_t SLOT_HASH{0};
constexpr std::size_t SLOT_KEY_OFFSET{8};
constexpr std::size_t SLOT_KEY_LENGTH{12};
constexpr std::size_t SLOT_TIMESTAMP{16};
constexpr std::size_t SLOT_DEPS_OFFSET{24};
constexpr std::size_t SLOT_DEPS_COUNT{28};
constexpr std::size_t SLOT_OCCUPIED{30};

template <typename T>
auto read_field(const std::uint8_t *base, std::size_t offset) -> T {
  T value;
  std::memcpy(&value, base + offset, sizeof(T));
  return value;
}

auto fnv1a(const char *data, std::size_t length) -> std::uint64_t {
  std::uint64_t hash{14695981039346656037ULL};
  for (std::size_t index = 0; index < length; ++index) {
    hash ^= static_cast<std::uint64_t>(static_cast<std::uint8_t>(data[index]));
    hash *= 1099511628211ULL;
  }

  return hash;
}

auto slot_key(const std::uint8_t *slot, const std::uint8_t *pool)
    -> std::string_view {
  const auto key_offset{read_field<std::uint32_t>(slot, SLOT_KEY_OFFSET)};
  const auto key_length{read_field<std::uint32_t>(slot, SLOT_KEY_LENGTH)};
  return {reinterpret_cast<const char *>(pool + key_offset), key_length};
}

} // anonymous namespace

namespace sourcemeta::one {

using mark_type = std::filesystem::file_time_type;

auto BuildState::load(const std::filesystem::path &path) -> void {
  if (!std::filesystem::exists(path)) {
    return;
  }

  try {
    this->loaded_path = path;
    this->view = std::make_unique<sourcemeta::core::FileView>(path);
    this->view_data = this->view->as<std::uint8_t>();
    const auto file_size{this->view->size()};

    if (file_size < HEADER_SIZE) {
      this->table_capacity = 0;
      this->table_slots = nullptr;
      this->string_pool = nullptr;
      this->view.reset();
      this->view_data = nullptr;
      this->entry_count = 0;
      return;
    }

    if (read_field<std::uint32_t>(this->view_data, 0) != STATE_MAGIC ||
        read_field<std::uint32_t>(this->view_data, 4) != STATE_VERSION) {
      this->table_capacity = 0;
      this->table_slots = nullptr;
      this->string_pool = nullptr;
      this->view.reset();
      this->view_data = nullptr;
      this->entry_count = 0;
      return;
    }

    this->table_capacity = read_field<std::uint32_t>(this->view_data, 8);
    this->entry_count = read_field<std::uint32_t>(this->view_data, 12);
    this->table_slots = this->view_data + HEADER_SIZE;
    this->string_pool =
        this->table_slots +
        static_cast<std::size_t>(this->table_capacity) * SLOT_SIZE;
  } catch (...) {
    this->table_capacity = 0;
    this->table_slots = nullptr;
    this->string_pool = nullptr;
    this->view.reset();
    this->view_data = nullptr;
    this->entry_count = 0;
    throw;
  }
}

auto BuildState::probe_slot(std::string_view key) const
    -> const std::uint8_t * {
  if (this->table_capacity == 0) {
    return nullptr;
  }

  const auto hash{fnv1a(key.data(), key.size())};
  auto index{static_cast<std::uint32_t>(hash & (this->table_capacity - 1))};

  for (std::uint32_t probe = 0; probe < this->table_capacity; ++probe) {
    const auto *slot{this->table_slots + index * SLOT_SIZE};
    if (slot[SLOT_OCCUPIED] == 0) {
      return nullptr;
    }

    if (read_field<std::uint64_t>(slot, SLOT_HASH) == hash) {
      const auto key_length{read_field<std::uint32_t>(slot, SLOT_KEY_LENGTH)};
      if (key_length == key.size()) {
        const auto key_offset{read_field<std::uint32_t>(slot, SLOT_KEY_OFFSET)};
        if (std::memcmp(this->string_pool + key_offset, key.data(),
                        key.size()) == 0) {
          return slot;
        }
      }
    }

    index = (index + 1) & (this->table_capacity - 1);
  }

  return nullptr;
}

auto BuildState::parse_slot_entry(const std::uint8_t *slot) const
    -> const Entry & {
  const auto key{slot_key(slot, this->string_pool)};

  const auto cache_match{this->lazy_cache.find(key)};
  if (cache_match != this->lazy_cache.end()) {
    return cache_match->second;
  }

  auto &cached{this->lazy_cache[std::string{key}]};

  const auto nanoseconds{read_field<std::int64_t>(slot, SLOT_TIMESTAMP)};
  cached.file_mark = mark_type{std::chrono::duration_cast<mark_type::duration>(
      std::chrono::nanoseconds{nanoseconds})};

  const auto deps_count{read_field<std::uint16_t>(slot, SLOT_DEPS_COUNT)};
  if (deps_count > 0) {
    cached.dependencies.reserve(deps_count);
    auto offset{static_cast<std::size_t>(
        read_field<std::uint32_t>(slot, SLOT_DEPS_OFFSET))};
    for (std::uint16_t dep_index = 0; dep_index < deps_count; ++dep_index) {
      const auto dep_length{
          read_field<std::uint32_t>(this->string_pool, offset)};
      offset += sizeof(std::uint32_t);
      cached.dependencies.emplace_back(std::string{
          reinterpret_cast<const char *>(this->string_pool + offset),
          dep_length});
      offset += dep_length;
    }
  }

  return cached;
}

auto BuildState::contains(const std::string &key) const -> bool {
  if (this->overlay.contains(key)) {
    return true;
  }

  if (this->deleted.contains(key)) {
    return false;
  }

  return this->probe_slot(key) != nullptr;
}

auto BuildState::entry(const std::string &key) const -> const Entry * {
  const auto overlay_match{this->overlay.find(key)};
  if (overlay_match != this->overlay.end()) {
    return &overlay_match->second;
  }

  if (this->deleted.contains(key)) {
    return nullptr;
  }

  const auto *slot{this->probe_slot(key)};
  if (slot == nullptr) {
    return nullptr;
  }

  return &this->parse_slot_entry(slot);
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

  const auto *slot{this->probe_slot(key)};
  if (slot == nullptr) {
    return true;
  }

  const auto nanoseconds{read_field<std::int64_t>(slot, SLOT_TIMESTAMP)};
  const auto file_mark{
      mark_type{std::chrono::duration_cast<mark_type::duration>(
          std::chrono::nanoseconds{nanoseconds})}};
  return source_mtime > file_mark;
}

auto BuildState::commit(const std::filesystem::path &path,
                        std::vector<std::filesystem::path> dependencies)
    -> void {
  const auto &key{path.native()};
  const auto was_live{
      this->overlay.contains(key) ||
      (this->probe_slot(key) != nullptr && !this->deleted.contains(key))};

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

  for (std::uint32_t slot_index = 0; slot_index < this->table_capacity;
       ++slot_index) {
    const auto *slot{this->table_slots + slot_index * SLOT_SIZE};
    if (slot[SLOT_OCCUPIED] == 0) {
      continue;
    }

    const auto key_sv{slot_key(slot, this->string_pool)};
    if (matches(key_sv) && !this->deleted.contains(key_sv)) {
      this->deleted.emplace(key_sv);
      this->lazy_cache.erase(std::string{key_sv});
      if (!removed_from_overlay.contains(key_sv)) {
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
      (this->probe_slot(key) != nullptr && !this->deleted.contains(key))};

  this->overlay[key] = std::move(entry);
  this->deleted.erase(key);

  if (!was_live) {
    this->entry_count++;
  }

  this->dirty = true;
  this->keys_stale = true;
}

auto BuildState::keys() const -> const std::vector<std::string_view> & {
  if (!this->keys_stale) {
    return this->cached_keys;
  }

  this->cached_keys.clear();
  this->cached_keys.reserve(this->entry_count);

  for (const auto &[key, value] : this->overlay) {
    this->cached_keys.push_back(key);
  }

  for (std::uint32_t slot_index = 0; slot_index < this->table_capacity;
       ++slot_index) {
    const auto *slot{this->table_slots + slot_index * SLOT_SIZE};
    if (slot[SLOT_OCCUPIED] == 0) {
      continue;
    }

    const auto key{slot_key(slot, this->string_pool)};
    if (!this->deleted.contains(key) && !this->overlay.contains(key)) {
      this->cached_keys.push_back(key);
    }
  }

  this->keys_stale = false;
  return this->cached_keys;
}

auto BuildState::save(const std::filesystem::path &path) const -> void {
  if (!this->dirty && this->view && path == this->loaded_path) {
    return;
  }

  const auto count{static_cast<std::uint32_t>(this->entry_count)};

  std::uint32_t capacity{16};
  while (capacity < count * 2) {
    capacity <<= 1;
  }

  std::string pool;
  pool.reserve(static_cast<std::size_t>(count) * 100);
  std::vector<std::uint8_t> slots(
      static_cast<std::size_t>(capacity) * SLOT_SIZE, 0);

  auto write_slot{[&](std::string_view entry_key, std::int64_t timestamp,
                      std::uint32_t deps_pool_offset,
                      std::uint16_t deps_count) {
    const auto hash{fnv1a(entry_key.data(), entry_key.size())};
    auto index{static_cast<std::uint32_t>(hash & (capacity - 1))};
    while (slots[index * SLOT_SIZE + SLOT_OCCUPIED] != 0) {
      index = (index + 1) & (capacity - 1);
    }

    auto *slot{slots.data() + index * SLOT_SIZE};
    const auto key_offset{static_cast<std::uint32_t>(pool.size())};
    pool.append(entry_key);
    const auto key_length{static_cast<std::uint32_t>(entry_key.size())};

    std::memcpy(slot + SLOT_HASH, &hash, sizeof(hash));
    std::memcpy(slot + SLOT_KEY_OFFSET, &key_offset, sizeof(key_offset));
    std::memcpy(slot + SLOT_KEY_LENGTH, &key_length, sizeof(key_length));
    std::memcpy(slot + SLOT_TIMESTAMP, &timestamp, sizeof(timestamp));
    std::memcpy(slot + SLOT_DEPS_OFFSET, &deps_pool_offset,
                sizeof(deps_pool_offset));
    std::memcpy(slot + SLOT_DEPS_COUNT, &deps_count, sizeof(deps_count));
    slot[SLOT_OCCUPIED] = 1;
  }};

  // Re-insert live on-disk entries (not deleted, not shadowed by overlay)
  for (std::uint32_t slot_index = 0; slot_index < this->table_capacity;
       ++slot_index) {
    const auto *old_slot{this->table_slots + slot_index * SLOT_SIZE};
    if (old_slot[SLOT_OCCUPIED] == 0) {
      continue;
    }

    const auto key{slot_key(old_slot, this->string_pool)};
    if (this->deleted.contains(key) || this->overlay.contains(key)) {
      continue;
    }

    const auto timestamp{read_field<std::int64_t>(old_slot, SLOT_TIMESTAMP)};
    const auto old_deps_offset{
        read_field<std::uint32_t>(old_slot, SLOT_DEPS_OFFSET)};
    const auto deps_count{read_field<std::uint16_t>(old_slot, SLOT_DEPS_COUNT)};

    // Copy raw dependency data from old string pool to new pool
    const auto new_deps_offset{static_cast<std::uint32_t>(pool.size())};
    if (deps_count > 0) {
      auto raw_offset{static_cast<std::size_t>(old_deps_offset)};
      for (std::uint16_t dep_index = 0; dep_index < deps_count; ++dep_index) {
        const auto dep_length{
            read_field<std::uint32_t>(this->string_pool, raw_offset)};
        raw_offset += sizeof(std::uint32_t) + dep_length;
      }

      const auto raw_size{raw_offset - old_deps_offset};
      pool.append(
          reinterpret_cast<const char *>(this->string_pool + old_deps_offset),
          raw_size);
    }

    write_slot(key, timestamp, new_deps_offset, deps_count);
  }

  // Write overlay entries
  for (const auto &[entry_path, entry] : this->overlay) {
    const auto timestamp{static_cast<std::int64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(
            entry.file_mark.time_since_epoch())
            .count())};

    const auto deps_offset{static_cast<std::uint32_t>(pool.size())};
    assert(entry.dependencies.size() <= UINT16_MAX);
    const auto deps_count{
        static_cast<std::uint16_t>(entry.dependencies.size())};
    for (const auto &dependency : entry.dependencies) {
      const auto &dep_str{dependency.native()};
      const auto dep_length{static_cast<std::uint32_t>(dep_str.size())};
      pool.append(reinterpret_cast<const char *>(&dep_length),
                  sizeof(dep_length));
      pool.append(dep_str);
    }

    write_slot(entry_path, timestamp, deps_offset, deps_count);
  }

  // Assemble the file
  const auto pool_size{static_cast<std::uint32_t>(pool.size())};
  const std::uint32_t padding{0};

  std::string buffer;
  buffer.reserve(HEADER_SIZE + static_cast<std::size_t>(capacity) * SLOT_SIZE +
                 pool.size());

  buffer.append(reinterpret_cast<const char *>(&STATE_MAGIC),
                sizeof(STATE_MAGIC));
  buffer.append(reinterpret_cast<const char *>(&STATE_VERSION),
                sizeof(STATE_VERSION));
  buffer.append(reinterpret_cast<const char *>(&capacity), sizeof(capacity));
  buffer.append(reinterpret_cast<const char *>(&count), sizeof(count));
  buffer.append(reinterpret_cast<const char *>(&pool_size), sizeof(pool_size));
  buffer.append(reinterpret_cast<const char *>(&padding), sizeof(padding));

  buffer.append(reinterpret_cast<const char *>(slots.data()), slots.size());
  buffer.append(pool);

  std::ofstream stream{path, std::ios::binary};
  assert(!stream.fail());
  stream.write(buffer.data(), static_cast<std::streamsize>(buffer.size()));
}

} // namespace sourcemeta::one
