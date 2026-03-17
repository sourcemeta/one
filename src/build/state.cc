#include <sourcemeta/one/build_state.h>

#include <sourcemeta/core/io.h>

#include <cassert> // assert
#include <chrono>  // std::chrono::nanoseconds, std::chrono::duration_cast
#include <cstdint> // std::int64_t, std::uint16_t, std::uint32_t, std::uint64_t
#include <cstring> // std::memcpy, std::memcmp
#include <fstream> // std::ofstream
#include <queue>   // std::queue
#include <string>  // std::string
#include <unordered_map> // std::unordered_map
#include <unordered_set> // std::unordered_set
#include <vector>        // std::vector

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
//   24: data_offset  u32  (into string pool for deps or resolver strings)
//   28: data_count   u16  (dep count for output, string count for resolver)
//   30: occupied     u8
//   31: kind         u8   (0=output, 1=resolver_cache)
constexpr std::size_t SLOT_SIZE{32};
constexpr std::size_t SLOT_HASH{0};
constexpr std::size_t SLOT_KEY_OFFSET{8};
constexpr std::size_t SLOT_KEY_LENGTH{12};
constexpr std::size_t SLOT_TIMESTAMP{16};
constexpr std::size_t SLOT_DATA_OFFSET{24};
constexpr std::size_t SLOT_DATA_COUNT{28};
constexpr std::size_t SLOT_OCCUPIED{30};
constexpr std::size_t SLOT_KIND{31};

constexpr std::uint8_t KIND_OUTPUT{0};
constexpr std::uint8_t KIND_RESOLVER{1};

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

auto read_pool_string(const std::uint8_t *pool, std::size_t &offset)
    -> std::string {
  const auto length{read_field<std::uint32_t>(pool, offset)};
  offset += sizeof(std::uint32_t);
  std::string result{reinterpret_cast<const char *>(pool + offset), length};
  offset += length;
  return result;
}

auto append_pool_string(std::string &pool, const std::string &value) -> void {
  const auto length{static_cast<std::uint32_t>(value.size())};
  pool.append(reinterpret_cast<const char *>(&length), sizeof(length));
  pool.append(value);
}

} // anonymous namespace

namespace sourcemeta::one {

using mark_type = std::filesystem::file_time_type;

auto BuildState::take_lock() const -> std::unique_lock<std::recursive_mutex> {
  return std::unique_lock<std::recursive_mutex>{this->mutex_};
}

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
      this->resolver_entry_count = 0;
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
      this->resolver_entry_count = 0;
      return;
    }

    this->table_capacity = read_field<std::uint32_t>(this->view_data, 8);
    this->entry_count = read_field<std::uint32_t>(this->view_data, 12);
    // Offset 20 was padding in original v2, now stores resolver count
    this->resolver_entry_count = read_field<std::uint32_t>(this->view_data, 20);
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
    this->resolver_entry_count = 0;
    throw;
  }
}

auto BuildState::probe_slot(std::string_view key, std::uint8_t kind) const
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

    if (read_field<std::uint64_t>(slot, SLOT_HASH) == hash &&
        slot[SLOT_KIND] == kind) {
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

  const auto data_count{read_field<std::uint16_t>(slot, SLOT_DATA_COUNT)};
  if (data_count > 0) {
    cached.dependencies.reserve(data_count);
    auto offset{static_cast<std::size_t>(
        read_field<std::uint32_t>(slot, SLOT_DATA_OFFSET))};
    for (std::uint16_t dep_index = 0; dep_index < data_count; ++dep_index) {
      cached.dependencies.emplace_back(
          read_pool_string(this->string_pool, offset));
    }
  }

  return cached;
}

auto BuildState::parse_slot_resolver_entry(const std::uint8_t *slot) const
    -> const ResolverEntry & {
  const auto key{slot_key(slot, this->string_pool)};

  const auto cache_match{this->resolver_lazy_cache.find(key)};
  if (cache_match != this->resolver_lazy_cache.end()) {
    return cache_match->second;
  }

  auto &cached{this->resolver_lazy_cache[std::string{key}]};

  const auto nanoseconds{read_field<std::int64_t>(slot, SLOT_TIMESTAMP)};
  cached.file_mark = mark_type{std::chrono::duration_cast<mark_type::duration>(
      std::chrono::nanoseconds{nanoseconds})};

  auto offset{static_cast<std::size_t>(
      read_field<std::uint32_t>(slot, SLOT_DATA_OFFSET))};
  cached.new_identifier = read_pool_string(this->string_pool, offset);
  cached.original_identifier = read_pool_string(this->string_pool, offset);
  cached.dialect = read_pool_string(this->string_pool, offset);
  cached.relative_path = read_pool_string(this->string_pool, offset);

  return cached;
}

auto BuildState::contains(const std::string &key) const -> bool {
  if (this->overlay.contains(key)) {
    return true;
  }

  if (this->deleted.contains(key)) {
    return false;
  }

  return this->probe_slot(key, KIND_OUTPUT) != nullptr;
}

auto BuildState::entry(const std::string &key) const -> const Entry * {
  const auto overlay_match{this->overlay.find(key)};
  if (overlay_match != this->overlay.end()) {
    return &overlay_match->second;
  }

  if (this->deleted.contains(key)) {
    return nullptr;
  }

  const auto *slot{this->probe_slot(key, KIND_OUTPUT)};
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

  const auto *slot{this->probe_slot(key, KIND_OUTPUT)};
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
  const auto was_live{this->overlay.contains(key) ||
                      (this->probe_slot(key, KIND_OUTPUT) != nullptr &&
                       !this->deleted.contains(key))};

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
    if (slot[SLOT_OCCUPIED] == 0 || slot[SLOT_KIND] != KIND_OUTPUT) {
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
  const auto was_live{this->overlay.contains(key) ||
                      (this->probe_slot(key, KIND_OUTPUT) != nullptr &&
                       !this->deleted.contains(key))};

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
    if (slot[SLOT_OCCUPIED] == 0 || slot[SLOT_KIND] != KIND_OUTPUT) {
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

auto BuildState::resolve(const std::string &source_path,
                         const std::filesystem::file_time_type mtime) const
    -> const ResolverEntry * {
  const auto overlay_match{this->resolver_overlay.find(source_path)};
  if (overlay_match != this->resolver_overlay.end()) {
    if (mtime <= overlay_match->second.file_mark) {
      return &overlay_match->second;
    }

    return nullptr;
  }

  const auto *slot{this->probe_slot(source_path, KIND_RESOLVER)};
  if (slot == nullptr) {
    return nullptr;
  }

  const auto nanoseconds{read_field<std::int64_t>(slot, SLOT_TIMESTAMP)};
  const auto cached_mark{
      mark_type{std::chrono::duration_cast<mark_type::duration>(
          std::chrono::nanoseconds{nanoseconds})}};
  if (mtime > cached_mark) {
    return nullptr;
  }

  return &this->parse_slot_resolver_entry(slot);
}

auto BuildState::commit(const std::string &source_path, ResolverEntry entry)
    -> void {
  const auto was_live{this->resolver_overlay.contains(source_path) ||
                      this->probe_slot(source_path, KIND_RESOLVER) != nullptr};

  this->resolver_overlay[source_path] = std::move(entry);

  if (!was_live) {
    this->resolver_entry_count++;
  }

  this->dirty = true;
}

auto BuildState::dependents_of(const std::string_view uri,
                               const std::string &server_url,
                               const DependentCallback &callback) const
    -> void {
  const auto lock{this->take_lock()};
  if (!this->dependents_computed_) {
    const auto output_directory{this->loaded_path.parent_path().string()};
    const auto schemas_prefix{output_directory + "/schemas/"};
    static constexpr std::string_view SENTINEL{"/%/"};

    using DirectMap =
        std::unordered_map<std::string, std::unordered_set<std::string>>;
    DirectMap direct;
    std::unordered_map<std::string, std::filesystem::path> uri_to_source;

    for (const auto key : this->keys()) {
      if (!key.ends_with("/%/dependencies.metapack")) {
        continue;
      }

      const auto *state_entry{this->entry(std::string{key})};
      if (state_entry == nullptr) {
        continue;
      }

      const auto relative_start{schemas_prefix.size()};
      const auto sentinel_position{key.find(SENTINEL, relative_start)};
      if (sentinel_position == std::string_view::npos) {
        continue;
      }

      const auto owner_relative{
          key.substr(relative_start, sentinel_position - relative_start)};
      const auto owner_uri{server_url + std::string{owner_relative}};
      uri_to_source.try_emplace(owner_uri,
                                std::filesystem::path{std::string{key}});

      for (const auto &dependency : state_entry->dependencies) {
        const auto &dependency_path{dependency.native()};
        if (!dependency_path.starts_with(schemas_prefix)) {
          continue;
        }

        const auto dependency_sentinel{
            dependency_path.find(SENTINEL, relative_start)};
        if (dependency_sentinel == std::string::npos) {
          continue;
        }

        const auto dependency_relative{dependency_path.substr(
            relative_start, dependency_sentinel - relative_start)};
        if (dependency_relative == owner_relative) {
          continue;
        }

        const auto dependency_uri{server_url + dependency_relative};
        direct[dependency_uri].emplace(owner_uri);
      }
    }

    for (const auto &[target, _] : direct) {
      auto &dependents{this->dependents_cache_[target]};
      std::unordered_set<std::string_view> visited;
      visited.emplace(target);
      std::queue<std::string> queue;
      queue.emplace(target);
      while (!queue.empty()) {
        const auto current{std::move(queue.front())};
        queue.pop();
        const auto match{direct.find(current)};
        if (match == direct.cend()) {
          continue;
        }

        for (const auto &dependent : match->second) {
          const auto source_match{uri_to_source.find(dependent)};
          assert(source_match != uri_to_source.cend());
          dependents.insert({dependent, current, source_match->second});
          if (visited.emplace(dependent).second) {
            queue.emplace(dependent);
          }
        }
      }
    }

    this->dependents_computed_ = true;
  }

  const auto match{this->dependents_cache_.find(std::string{uri})};
  if (match == this->dependents_cache_.cend()) {
    return;
  }

  for (const auto &dependent : match->second) {
    callback(dependent.from, dependent.to, dependent.source);
  }
}

auto BuildState::save(const std::filesystem::path &path) const -> void {
  if (!this->dirty && this->view && path == this->loaded_path) {
    return;
  }

  const auto output_count{static_cast<std::uint32_t>(this->entry_count)};
  const auto resolver_count{
      static_cast<std::uint32_t>(this->resolver_entry_count)};
  const auto total_count{output_count + resolver_count};

  std::uint32_t capacity{16};
  while (capacity < total_count * 2) {
    capacity <<= 1;
  }

  std::string pool;
  pool.reserve(static_cast<std::size_t>(total_count) * 100);
  std::vector<std::uint8_t> slots(
      static_cast<std::size_t>(capacity) * SLOT_SIZE, 0);

  auto write_slot{[&](std::string_view entry_key, std::int64_t timestamp,
                      std::uint32_t data_pool_offset, std::uint16_t data_count,
                      std::uint8_t kind) {
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
    std::memcpy(slot + SLOT_DATA_OFFSET, &data_pool_offset,
                sizeof(data_pool_offset));
    std::memcpy(slot + SLOT_DATA_COUNT, &data_count, sizeof(data_count));
    slot[SLOT_OCCUPIED] = 1;
    slot[SLOT_KIND] = kind;
  }};

  // Write on-disk entries (both kinds, not deleted/overlayed)
  for (std::uint32_t slot_index = 0; slot_index < this->table_capacity;
       ++slot_index) {
    const auto *old_slot{this->table_slots + slot_index * SLOT_SIZE};
    if (old_slot[SLOT_OCCUPIED] == 0) {
      continue;
    }

    const auto old_kind{old_slot[SLOT_KIND]};
    const auto key{slot_key(old_slot, this->string_pool)};

    if (old_kind == KIND_OUTPUT) {
      if (this->deleted.contains(key) || this->overlay.contains(key)) {
        continue;
      }
    } else if (old_kind == KIND_RESOLVER) {
      if (this->resolver_overlay.contains(key)) {
        continue;
      }
    }

    const auto timestamp{read_field<std::int64_t>(old_slot, SLOT_TIMESTAMP)};
    const auto old_data_offset{
        read_field<std::uint32_t>(old_slot, SLOT_DATA_OFFSET)};
    const auto data_count{read_field<std::uint16_t>(old_slot, SLOT_DATA_COUNT)};

    // Copy raw data from old string pool to new pool
    const auto new_data_offset{static_cast<std::uint32_t>(pool.size())};
    if (data_count > 0) {
      auto raw_offset{static_cast<std::size_t>(old_data_offset)};
      for (std::uint16_t item_index = 0; item_index < data_count;
           ++item_index) {
        const auto item_length{
            read_field<std::uint32_t>(this->string_pool, raw_offset)};
        raw_offset += sizeof(std::uint32_t) + item_length;
      }

      const auto raw_size{raw_offset - old_data_offset};
      pool.append(
          reinterpret_cast<const char *>(this->string_pool + old_data_offset),
          raw_size);
    }

    write_slot(key, timestamp, new_data_offset, data_count, old_kind);
  }

  // Write output overlay entries (kind=0)
  for (const auto &[entry_path, entry] : this->overlay) {
    const auto timestamp{static_cast<std::int64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(
            entry.file_mark.time_since_epoch())
            .count())};

    const auto data_offset{static_cast<std::uint32_t>(pool.size())};
    assert(entry.dependencies.size() <= UINT16_MAX);
    const auto data_count{
        static_cast<std::uint16_t>(entry.dependencies.size())};
    for (const auto &dependency : entry.dependencies) {
      append_pool_string(pool, dependency.native());
    }

    write_slot(entry_path, timestamp, data_offset, data_count, KIND_OUTPUT);
  }

  // Write resolver overlay entries (kind=1)
  for (const auto &[source_path, cache_entry] : this->resolver_overlay) {
    const auto timestamp{static_cast<std::int64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(
            cache_entry.file_mark.time_since_epoch())
            .count())};

    const auto data_offset{static_cast<std::uint32_t>(pool.size())};
    append_pool_string(pool, cache_entry.new_identifier);
    append_pool_string(pool, cache_entry.original_identifier);
    append_pool_string(pool, cache_entry.dialect);
    append_pool_string(pool, cache_entry.relative_path);

    write_slot(source_path, timestamp, data_offset, 4, KIND_RESOLVER);
  }

  // Assemble the file
  const auto pool_size{static_cast<std::uint32_t>(pool.size())};

  std::string buffer;
  buffer.reserve(HEADER_SIZE + static_cast<std::size_t>(capacity) * SLOT_SIZE +
                 pool.size());

  buffer.append(reinterpret_cast<const char *>(&STATE_MAGIC),
                sizeof(STATE_MAGIC));
  buffer.append(reinterpret_cast<const char *>(&STATE_VERSION),
                sizeof(STATE_VERSION));
  buffer.append(reinterpret_cast<const char *>(&capacity), sizeof(capacity));
  buffer.append(reinterpret_cast<const char *>(&output_count),
                sizeof(output_count));
  buffer.append(reinterpret_cast<const char *>(&pool_size), sizeof(pool_size));
  buffer.append(reinterpret_cast<const char *>(&resolver_count),
                sizeof(resolver_count));

  buffer.append(reinterpret_cast<const char *>(slots.data()), slots.size());
  buffer.append(pool);

  std::ofstream stream{path, std::ios::binary};
  assert(!stream.fail());
  stream.write(buffer.data(), static_cast<std::streamsize>(buffer.size()));
}

} // namespace sourcemeta::one
