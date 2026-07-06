#include <sourcemeta/one/build_state.h>

#include <sourcemeta/core/io.h>

#include <cassert> // assert
#include <chrono>  // std::chrono::nanoseconds, std::chrono::duration_cast
#include <cstdint> // std::int64_t, std::uint16_t, std::uint32_t, std::uint64_t
#include <cstring> // std::memcpy, std::memcmp
#include <ostream> // std::ostream
#include <string>  // std::string
#include <vector>  // std::vector

namespace {

constexpr std::uint32_t STATE_MAGIC{0x44455053};
constexpr std::uint32_t STATE_VERSION{3};
constexpr std::uint32_t LEAF_INDEX_MAGIC{0x58444953};
constexpr std::size_t HEADER_SIZE{28};

#pragma pack(push, 1)
struct LeafIndexRecord {
  std::uint32_t relative_path_offset;
  std::uint16_t relative_path_length;
  std::int64_t root_mtime;
  std::uint16_t target_bitmap;
  std::uint8_t has_cross_leaf_deps;
  std::uint8_t padding;
};
#pragma pack(pop)
static_assert(sizeof(LeafIndexRecord) == 18);

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

auto BuildState::take_lock() const -> std::unique_lock<std::mutex> {
  return std::unique_lock<std::mutex>{this->mutex_};
}

auto BuildState::configure(std::span<const LeafRule> rules,
                           std::uint32_t fingerprint, std::string_view sentinel)
    -> void {
  this->leaf_rules = rules;
  this->rules_fingerprint = fingerprint;
  this->sentinel_separator = std::string{"/"} + std::string{sentinel} + "/";
}

auto BuildState::load(const std::filesystem::path &path,
                      std::span<const LeafRule> rules,
                      std::uint32_t fingerprint, std::string_view sentinel)
    -> void {
  this->configure(rules, fingerprint, sentinel);

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

    sourcemeta::core::BinaryReader header_reader{*this->view};
    const auto magic{header_reader.get_dword()};
    const auto version{header_reader.get_dword()};
    const auto on_disk_fingerprint{header_reader.get_dword()};
    if (magic != STATE_MAGIC || version != STATE_VERSION ||
        on_disk_fingerprint != fingerprint) {
      this->table_capacity = 0;
      this->table_slots = nullptr;
      this->string_pool = nullptr;
      this->view.reset();
      this->view_data = nullptr;
      this->entry_count = 0;
      this->resolver_entry_count = 0;
      return;
    }

    this->table_capacity = header_reader.get_dword();
    this->entry_count = header_reader.get_dword();
    const auto pool_size_value{header_reader.get_dword()};
    this->resolver_entry_count = header_reader.get_dword();
    this->table_slots = this->view_data + HEADER_SIZE;
    this->string_pool =
        this->table_slots +
        static_cast<std::size_t>(this->table_capacity) * SLOT_SIZE;

    const auto leaf_table_start{HEADER_SIZE +
                                static_cast<std::size_t>(this->table_capacity) *
                                    SLOT_SIZE +
                                pool_size_value};
    if (leaf_table_start + sizeof(std::uint32_t) * 2 <= file_size &&
        read_field<std::uint32_t>(this->view_data, leaf_table_start) ==
            LEAF_INDEX_MAGIC) {
      this->persisted_leaf_count = read_field<std::uint32_t>(
          this->view_data, leaf_table_start + sizeof(std::uint32_t));
      this->persisted_leaf_table =
          this->view_data + leaf_table_start + sizeof(std::uint32_t) * 2;
    }
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
  this->leaf_index_stale = true;
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
  this->leaf_index_stale = true;
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
  this->leaf_index_stale = true;
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

auto BuildState::in_overlay(const std::string &key) const -> bool {
  return this->overlay.contains(key);
}

auto BuildState::disk_entry(const std::string &key) const -> const Entry * {
  if (this->deleted.contains(key)) {
    return nullptr;
  }

  const auto *slot{this->probe_slot(key, KIND_OUTPUT)};
  if (slot == nullptr) {
    return nullptr;
  }

  return &this->parse_slot_entry(slot);
}

auto BuildState::raw_disk_entry(const std::string &key) const -> const Entry * {
  const auto *slot{this->probe_slot(key, KIND_OUTPUT)};
  if (slot == nullptr) {
    return nullptr;
  }

  return &this->parse_slot_entry(slot);
}

auto BuildState::deleted_keys() const -> const
    std::unordered_set<std::string, TransparentHash, TransparentEqual> & {
  return this->deleted;
}

auto BuildState::build_leaf_index(const std::string &output) const -> void {
  this->leaf_index_cache.clear();
  this->leaf_index_output = output;

  if (this->persisted_leaf_table != nullptr && this->persisted_leaf_count > 0 &&
      this->overlay.empty() && this->deleted.empty()) {
    const auto *cursor{this->persisted_leaf_table};
    for (std::uint32_t index{0}; index < this->persisted_leaf_count; index++) {
      const auto *record{reinterpret_cast<const LeafIndexRecord *>(cursor)};
      cursor += sizeof(LeafIndexRecord);
      const std::string relative_path{reinterpret_cast<const char *>(cursor),
                                      record->relative_path_length};
      cursor += record->relative_path_length;

      using file_time = std::filesystem::file_time_type;
      auto &entry{this->leaf_index_cache[relative_path]};
      entry.root_mtime =
          file_time{std::chrono::duration_cast<file_time::duration>(
              std::chrono::nanoseconds{record->root_mtime})};
      entry.target_bitmap = record->target_bitmap;
      entry.has_cross_leaf_deps = record->has_cross_leaf_deps != 0;
    }

    this->leaf_index_stale = false;
    return;
  }

  const auto primary_prefix{output + "/schemas/"};
  const auto secondary_prefix{output + "/explorer/"};

  auto extract_primary_base = [&](std::string_view key)
      -> std::pair<std::string_view, std::string_view> {
    std::string_view prefix;
    if (key.starts_with(primary_prefix)) {
      prefix = primary_prefix;
    } else if (key.starts_with(secondary_prefix)) {
      prefix = secondary_prefix;
    } else {
      return {{}, {}};
    }

    const auto after_prefix{key.substr(prefix.size())};
    const auto sentinel_pos{after_prefix.find(this->sentinel_separator)};
    if (sentinel_pos == std::string_view::npos) {
      return {{}, {}};
    }

    return {
        after_prefix.substr(0, sentinel_pos),
        after_prefix.substr(sentinel_pos + this->sentinel_separator.size())};
  };

  auto process_key = [&](std::filesystem::file_time_type mtime,
                         bool is_explorer, std::string_view relative_path,
                         std::string_view filename) -> void {
    auto &leaf_entry{this->leaf_index_cache[std::string{relative_path}]};

    for (std::size_t rule_index{0}; rule_index < this->leaf_rules.size();
         rule_index++) {
      const auto &rule{this->leaf_rules[rule_index]};
      if (filename == rule.filename && ((rule.base == 1) == is_explorer)) {
        leaf_entry.target_bitmap |= static_cast<std::uint16_t>(1 << rule_index);
        if (rule.is_root) {
          leaf_entry.root_mtime = mtime;
        }
        break;
      }
    }
  };

  auto check_cross_leaf_deps =
      [&](std::string_view owner_relative,
          const std::vector<std::filesystem::path> &dependencies) -> void {
    for (const auto &dependency : dependencies) {
      const auto [dep_relative, dep_filename] =
          extract_primary_base(dependency.native());
      if (!dep_relative.empty() && dep_relative != owner_relative) {
        auto &leaf_entry{this->leaf_index_cache[std::string{owner_relative}]};
        leaf_entry.has_cross_leaf_deps = true;
        return;
      }
    }
  };

  for (std::uint32_t slot_index = 0; slot_index < this->table_capacity;
       ++slot_index) {
    const auto *slot{this->table_slots + slot_index * SLOT_SIZE};
    if (slot[SLOT_OCCUPIED] == 0 || slot[SLOT_KIND] != KIND_OUTPUT) {
      continue;
    }

    const auto key{slot_key(slot, this->string_pool)};
    if (this->deleted.contains(key) || this->overlay.contains(key)) {
      continue;
    }

    const auto [relative_path, filename] = extract_primary_base(key);
    if (relative_path.empty()) {
      continue;
    }

    const bool is_explorer{key.starts_with(secondary_prefix)};
    const auto nanoseconds{read_field<std::int64_t>(slot, SLOT_TIMESTAMP)};
    using file_time = std::filesystem::file_time_type;
    const auto mtime{file_time{std::chrono::duration_cast<file_time::duration>(
        std::chrono::nanoseconds{nanoseconds})}};

    process_key(mtime, is_explorer, relative_path, filename);

    const auto data_count{read_field<std::uint16_t>(slot, SLOT_DATA_COUNT)};
    if (data_count > 0) {
      auto offset{static_cast<std::size_t>(
          read_field<std::uint32_t>(slot, SLOT_DATA_OFFSET))};
      for (std::uint16_t dependency_index = 0; dependency_index < data_count;
           ++dependency_index) {
        const auto dependency_path{read_pool_string(this->string_pool, offset)};
        const auto [dep_relative, dep_filename] =
            extract_primary_base(dependency_path);
        if (!dep_relative.empty() && dep_relative != relative_path) {
          this->leaf_index_cache[std::string{relative_path}]
              .has_cross_leaf_deps = true;
          break;
        }
      }
    }
  }

  for (const auto &[entry_path, entry_value] : this->overlay) {
    const auto [relative_path, filename] = extract_primary_base(entry_path);
    if (relative_path.empty()) {
      continue;
    }

    const bool is_explorer{
        std::string_view{entry_path}.starts_with(secondary_prefix)};
    process_key(entry_value.file_mark, is_explorer, relative_path, filename);
    check_cross_leaf_deps(relative_path, entry_value.dependencies);
  }

  this->leaf_index_stale = false;
}

auto BuildState::leaf_state(const std::string &output,
                            const std::string &relative_path, const bool,
                            const bool) const -> const LeafStateEntry * {
  if (this->leaf_index_stale || this->leaf_index_output != output) {
    this->build_leaf_index(output);
  }

  const auto match{this->leaf_index_cache.find(relative_path)};
  if (match == this->leaf_index_cache.end()) {
    return nullptr;
  }

  return &match->second;
}

auto BuildState::leaf_relative_paths(const std::string &output) const
    -> std::vector<std::string> {
  if (this->leaf_index_stale || this->leaf_index_output != output) {
    this->build_leaf_index(output);
  }

  std::vector<std::string> result;
  result.reserve(this->leaf_index_cache.size());
  for (const auto &[relative_path, entry] : this->leaf_index_cache) {
    result.push_back(relative_path);
  }

  return result;
}

auto BuildState::save(const std::filesystem::path &path) const -> void {
  if (!this->dirty && this->view && path == this->loaded_path) {
    return;
  }

  const auto output_count{static_cast<std::uint32_t>(this->entry_count)};
  const auto resolver_count{
      static_cast<std::uint32_t>(this->resolver_entry_count)};
  const auto total_count{output_count + resolver_count};

  const bool can_patch{this->table_capacity > 0 &&
                       this->string_pool != nullptr &&
                       total_count < this->table_capacity * 3 / 4};

  std::string pool;
  std::vector<std::uint8_t> slots;
  std::uint32_t capacity;

  std::uint32_t old_pool_size{0};
  std::unordered_set<std::string, TransparentHash, TransparentEqual>
      overlay_updates;
  std::unordered_set<std::string, TransparentHash, TransparentEqual>
      resolver_updates;

  if (can_patch) {
    capacity = this->table_capacity;
    const auto old_slots_size{static_cast<std::size_t>(capacity) * SLOT_SIZE};
    slots.resize(old_slots_size);
    std::memcpy(slots.data(), this->table_slots, old_slots_size);

    sourcemeta::core::BinaryReader pool_size_reader{*this->view};
    // Pool size sits two dwords past the magic/version/fingerprint/capacity/
    // entry_count prefix. Layout: [magic, version, fingerprint, capacity,
    // entry_count, pool_size, resolver_entry_count].
    pool_size_reader.seek(20);
    old_pool_size = pool_size_reader.get_dword();

    auto find_slot{[&](std::string_view entry_key) -> std::uint32_t {
      const auto hash{fnv1a(entry_key.data(), entry_key.size())};
      auto index{static_cast<std::uint32_t>(hash & (capacity - 1))};
      while (slots[index * SLOT_SIZE + SLOT_OCCUPIED] != 0) {
        const auto *slot{slots.data() + index * SLOT_SIZE};
        if (read_field<std::uint64_t>(slot, SLOT_HASH) == hash &&
            slot_key(slot, this->string_pool) == entry_key) {
          return index;
        }
        index = (index + 1) & (capacity - 1);
      }
      return capacity;
    }};

    for (const auto &deleted_key : this->deleted) {
      const auto slot_index{find_slot(deleted_key)};
      if (slot_index < capacity) {
        auto empty{slot_index};
        for (;;) {
          const auto next{(empty + 1) & (capacity - 1)};
          if (slots[next * SLOT_SIZE + SLOT_OCCUPIED] == 0) {
            break;
          }

          const auto next_hash{read_field<std::uint64_t>(
              slots.data() + next * SLOT_SIZE, SLOT_HASH)};
          const auto natural{
              static_cast<std::uint32_t>(next_hash & (capacity - 1))};
          const auto distance_current{(next - natural) & (capacity - 1)};
          const auto distance_new{(empty - natural) & (capacity - 1)};
          if (distance_new <= distance_current) {
            std::memcpy(slots.data() + empty * SLOT_SIZE,
                        slots.data() + next * SLOT_SIZE, SLOT_SIZE);
            empty = next;
          } else {
            break;
          }
        }

        slots[empty * SLOT_SIZE + SLOT_OCCUPIED] = 0;
      }
    }

    for (const auto &[overlay_key, overlay_entry] : this->overlay) {
      if (find_slot(overlay_key) < capacity) {
        overlay_updates.insert(overlay_key);
      }
    }

    for (const auto &[overlay_key, overlay_entry] : this->resolver_overlay) {
      if (find_slot(overlay_key) < capacity) {
        resolver_updates.insert(overlay_key);
      }
    }
  } else {
    capacity = 16;
    while (capacity < total_count * 2) {
      capacity <<= 1;
    }

    pool.reserve(static_cast<std::size_t>(total_count) * 100);
    slots.resize(static_cast<std::size_t>(capacity) * SLOT_SIZE, 0);

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
      const auto data_count{
          read_field<std::uint16_t>(old_slot, SLOT_DATA_COUNT)};

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

      const auto hash{fnv1a(key.data(), key.size())};
      auto index{static_cast<std::uint32_t>(hash & (capacity - 1))};
      while (slots[index * SLOT_SIZE + SLOT_OCCUPIED] != 0) {
        index = (index + 1) & (capacity - 1);
      }

      auto *slot{slots.data() + index * SLOT_SIZE};
      const auto key_offset{static_cast<std::uint32_t>(pool.size())};
      pool.append(key);
      const auto key_length{static_cast<std::uint32_t>(key.size())};

      std::memcpy(slot + SLOT_HASH, &hash, sizeof(hash));
      std::memcpy(slot + SLOT_KEY_OFFSET, &key_offset, sizeof(key_offset));
      std::memcpy(slot + SLOT_KEY_LENGTH, &key_length, sizeof(key_length));
      std::memcpy(slot + SLOT_TIMESTAMP, &timestamp, sizeof(timestamp));
      std::memcpy(slot + SLOT_DATA_OFFSET, &new_data_offset,
                  sizeof(new_data_offset));
      std::memcpy(slot + SLOT_DATA_COUNT, &data_count, sizeof(data_count));
      slot[SLOT_OCCUPIED] = 1;
      slot[SLOT_KIND] = old_kind;
    }
  }

  auto write_new_slot{[&](std::string_view entry_key, std::int64_t timestamp,
                          std::uint32_t data_pool_offset,
                          std::uint16_t data_count, std::uint8_t kind,
                          bool is_update) -> void {
    const auto hash{fnv1a(entry_key.data(), entry_key.size())};
    std::uint32_t index;

    if (is_update) {
      index = static_cast<std::uint32_t>(hash & (capacity - 1));
      while (slots[index * SLOT_SIZE + SLOT_OCCUPIED] != 0) {
        const auto *slot{slots.data() + index * SLOT_SIZE};
        if (read_field<std::uint64_t>(slot, SLOT_HASH) == hash) {
          const auto probe_key_offset{
              read_field<std::uint32_t>(slot, SLOT_KEY_OFFSET)};
          const auto probe_key_length{
              read_field<std::uint32_t>(slot, SLOT_KEY_LENGTH)};
          std::string_view probe_key;
          if (can_patch && probe_key_offset < old_pool_size) {
            probe_key = {reinterpret_cast<const char *>(this->string_pool +
                                                        probe_key_offset),
                         probe_key_length};
          } else {
            const auto offset_in_pool{can_patch
                                          ? probe_key_offset - old_pool_size
                                          : probe_key_offset};
            probe_key = {pool.data() + offset_in_pool, probe_key_length};
          }
          if (probe_key == entry_key) {
            break;
          }
        }
        index = (index + 1) & (capacity - 1);
      }
    } else {
      index = static_cast<std::uint32_t>(hash & (capacity - 1));
      while (slots[index * SLOT_SIZE + SLOT_OCCUPIED] != 0) {
        index = (index + 1) & (capacity - 1);
      }
    }

    auto *slot{slots.data() + index * SLOT_SIZE};
    const auto key_offset{
        static_cast<std::uint32_t>(old_pool_size + pool.size())};
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

  for (const auto &[entry_path, entry] : this->overlay) {
    const auto timestamp{static_cast<std::int64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(
            entry.file_mark.time_since_epoch())
            .count())};

    const auto data_offset{
        static_cast<std::uint32_t>(old_pool_size + pool.size())};
    assert(entry.dependencies.size() <= UINT16_MAX);
    const auto data_count{
        static_cast<std::uint16_t>(entry.dependencies.size())};
    for (const auto &dependency : entry.dependencies) {
      append_pool_string(pool, dependency.native());
    }

    write_new_slot(entry_path, timestamp, data_offset, data_count, KIND_OUTPUT,
                   overlay_updates.contains(entry_path));
  }

  for (const auto &[source_path, cache_entry] : this->resolver_overlay) {
    const auto timestamp{static_cast<std::int64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(
            cache_entry.file_mark.time_since_epoch())
            .count())};

    const auto data_offset{
        static_cast<std::uint32_t>(old_pool_size + pool.size())};
    append_pool_string(pool, cache_entry.new_identifier);
    append_pool_string(pool, cache_entry.original_identifier);
    append_pool_string(pool, cache_entry.dialect);
    append_pool_string(pool, cache_entry.relative_path);

    write_new_slot(source_path, timestamp, data_offset, 4, KIND_RESOLVER,
                   resolver_updates.contains(source_path));
  }

  const auto total_pool_size{
      static_cast<std::uint32_t>(old_pool_size + pool.size())};

  {
    auto read_slot_key{[&](const std::uint8_t *slot) -> std::string_view {
      const auto key_offset{read_field<std::uint32_t>(slot, SLOT_KEY_OFFSET)};
      const auto key_length{read_field<std::uint32_t>(slot, SLOT_KEY_LENGTH)};
      if (can_patch && key_offset < old_pool_size) {
        return {reinterpret_cast<const char *>(this->string_pool + key_offset),
                key_length};
      }
      const auto offset_in_pool{can_patch ? key_offset - old_pool_size
                                          : key_offset};
      return {pool.data() + offset_in_pool, key_length};
    }};

    auto read_slot_pool_string{[&](std::size_t &offset) -> std::string {
      if (can_patch && offset < old_pool_size) {
        return read_pool_string(this->string_pool, offset);
      }
      auto adjusted_offset{can_patch ? offset - old_pool_size : offset};
      auto result{
          read_pool_string(reinterpret_cast<const std::uint8_t *>(pool.data()),
                           adjusted_offset)};
      offset = (can_patch ? old_pool_size : 0) + adjusted_offset;
      return result;
    }};

    const auto output_dir{path.parent_path().string()};
    const auto primary_prefix{output_dir + "/schemas/"};
    const auto secondary_prefix{output_dir + "/explorer/"};

    std::unordered_map<std::string, LeafStateEntry, TransparentHash,
                       TransparentEqual>
        save_leaf_index;

    if (can_patch && this->persisted_leaf_table != nullptr) {
      const auto *record_ptr{this->persisted_leaf_table};
      for (std::uint32_t record_index = 0;
           record_index < this->persisted_leaf_count; ++record_index) {
        const auto *record{
            reinterpret_cast<const LeafIndexRecord *>(record_ptr)};
        const auto relative_path{std::string{
            reinterpret_cast<const char *>(record_ptr + sizeof(*record)),
            record->relative_path_length}};
        auto &leaf_entry{save_leaf_index[relative_path]};
        using file_time = std::filesystem::file_time_type;
        leaf_entry.root_mtime =
            file_time{std::chrono::duration_cast<file_time::duration>(
                std::chrono::nanoseconds{record->root_mtime})};
        leaf_entry.target_bitmap = record->target_bitmap;
        leaf_entry.has_cross_leaf_deps = record->has_cross_leaf_deps != 0;
        record_ptr += sizeof(*record) + record->relative_path_length;
      }

      for (const auto &deleted_key : this->deleted) {
        std::string_view key_prefix;
        if (std::string_view{deleted_key}.starts_with(primary_prefix)) {
          key_prefix = primary_prefix;
        } else if (std::string_view{deleted_key}.starts_with(
                       secondary_prefix)) {
          key_prefix = secondary_prefix;
        } else {
          continue;
        }

        const auto after{
            std::string_view{deleted_key}.substr(key_prefix.size())};
        const auto sentinel_position{after.find(this->sentinel_separator)};
        if (sentinel_position == std::string_view::npos) {
          continue;
        }

        save_leaf_index.erase(std::string{after.substr(0, sentinel_position)});
      }

      std::unordered_set<std::string, TransparentHash, TransparentEqual>
          affected_leaves;
      for (const auto &[overlay_key, overlay_entry] : this->overlay) {
        std::string_view key_view{overlay_key};
        std::string_view key_prefix;
        if (key_view.starts_with(primary_prefix)) {
          key_prefix = primary_prefix;
        } else if (key_view.starts_with(secondary_prefix)) {
          key_prefix = secondary_prefix;
        } else {
          continue;
        }

        const auto after{key_view.substr(key_prefix.size())};
        const auto sentinel_position{after.find(this->sentinel_separator)};
        if (sentinel_position == std::string_view::npos) {
          continue;
        }

        affected_leaves.insert(std::string{after.substr(0, sentinel_position)});
      }

      for (const auto &affected_relative : affected_leaves) {
        auto &leaf_entry{save_leaf_index[affected_relative]};
        leaf_entry = LeafStateEntry{};

        for (std::uint32_t slot_index = 0; slot_index < capacity;
             ++slot_index) {
          const auto *slot{slots.data() + slot_index * SLOT_SIZE};
          if (slot[SLOT_OCCUPIED] == 0 || slot[SLOT_KIND] != KIND_OUTPUT) {
            continue;
          }

          const auto key{read_slot_key(slot)};
          std::string_view key_prefix;
          bool is_explorer{false};
          if (key.starts_with(primary_prefix)) {
            key_prefix = primary_prefix;
          } else if (key.starts_with(secondary_prefix)) {
            key_prefix = secondary_prefix;
            is_explorer = true;
          } else {
            continue;
          }

          const auto after{key.substr(key_prefix.size())};
          const auto sentinel_position{after.find(this->sentinel_separator)};
          if (sentinel_position == std::string_view::npos) {
            continue;
          }

          if (after.substr(0, sentinel_position) != affected_relative) {
            continue;
          }

          const auto filename{after.substr(sentinel_position +
                                           this->sentinel_separator.size())};
          for (std::size_t rule_index{0}; rule_index < this->leaf_rules.size();
               rule_index++) {
            const auto &rule{this->leaf_rules[rule_index]};
            if (filename == rule.filename &&
                ((rule.base == 1) == is_explorer)) {
              leaf_entry.target_bitmap |=
                  static_cast<std::uint16_t>(1 << rule_index);
              if (rule.is_root) {
                const auto nanoseconds{
                    read_field<std::int64_t>(slot, SLOT_TIMESTAMP)};
                using file_time = std::filesystem::file_time_type;
                leaf_entry.root_mtime =
                    file_time{std::chrono::duration_cast<file_time::duration>(
                        std::chrono::nanoseconds{nanoseconds})};
              }
              break;
            }
          }

          const auto data_count{
              read_field<std::uint16_t>(slot, SLOT_DATA_COUNT)};
          if (data_count > 0 && !leaf_entry.has_cross_leaf_deps) {
            auto offset{static_cast<std::size_t>(
                read_field<std::uint32_t>(slot, SLOT_DATA_OFFSET))};
            for (std::uint16_t dep_index = 0; dep_index < data_count;
                 ++dep_index) {
              const auto dependency_path{read_slot_pool_string(offset)};
              std::string_view dep_prefix;
              if (dependency_path.starts_with(primary_prefix)) {
                dep_prefix = primary_prefix;
              } else if (dependency_path.starts_with(secondary_prefix)) {
                dep_prefix = secondary_prefix;
              } else {
                continue;
              }

              const auto dep_after{dependency_path.substr(dep_prefix.size())};
              const auto dep_sentinel{dep_after.find(this->sentinel_separator)};
              if (dep_sentinel != std::string_view::npos &&
                  dep_after.substr(0, dep_sentinel) != affected_relative) {
                leaf_entry.has_cross_leaf_deps = true;
                break;
              }
            }
          }
        }
      }
    } else {

      for (std::uint32_t slot_index = 0; slot_index < capacity; ++slot_index) {
        const auto *slot{slots.data() + slot_index * SLOT_SIZE};
        if (slot[SLOT_OCCUPIED] == 0 || slot[SLOT_KIND] != KIND_OUTPUT) {
          continue;
        }

        const auto key{read_slot_key(slot)};
        std::string_view key_prefix;
        bool is_explorer{false};
        if (key.starts_with(primary_prefix)) {
          key_prefix = primary_prefix;
        } else if (key.starts_with(secondary_prefix)) {
          key_prefix = secondary_prefix;
          is_explorer = true;
        } else {
          continue;
        }

        const auto after{key.substr(key_prefix.size())};
        const auto sentinel_position{after.find(this->sentinel_separator)};
        if (sentinel_position == std::string_view::npos) {
          continue;
        }

        const auto relative_path{after.substr(0, sentinel_position)};
        const auto filename{
            after.substr(sentinel_position + this->sentinel_separator.size())};
        auto &leaf_entry{save_leaf_index[std::string{relative_path}]};

        for (std::size_t rule_index{0}; rule_index < this->leaf_rules.size();
             rule_index++) {
          const auto &rule{this->leaf_rules[rule_index]};
          if (filename == rule.filename && ((rule.base == 1) == is_explorer)) {
            leaf_entry.target_bitmap |=
                static_cast<std::uint16_t>(1 << rule_index);
            if (rule.is_root) {
              const auto nanoseconds{
                  read_field<std::int64_t>(slot, SLOT_TIMESTAMP)};
              using file_time = std::filesystem::file_time_type;
              leaf_entry.root_mtime =
                  file_time{std::chrono::duration_cast<file_time::duration>(
                      std::chrono::nanoseconds{nanoseconds})};
            }
            break;
          }
        }

        const auto data_count{read_field<std::uint16_t>(slot, SLOT_DATA_COUNT)};
        if (data_count > 0 && !leaf_entry.has_cross_leaf_deps) {
          auto offset{static_cast<std::size_t>(
              read_field<std::uint32_t>(slot, SLOT_DATA_OFFSET))};
          for (std::uint16_t dep_index = 0; dep_index < data_count;
               ++dep_index) {
            const auto dependency_path{read_slot_pool_string(offset)};
            std::string_view dep_prefix;
            if (dependency_path.starts_with(primary_prefix)) {
              dep_prefix = primary_prefix;
            } else if (dependency_path.starts_with(secondary_prefix)) {
              dep_prefix = secondary_prefix;
            } else {
              continue;
            }

            const auto dep_after{dependency_path.substr(dep_prefix.size())};
            const auto dep_sentinel{dep_after.find(this->sentinel_separator)};
            if (dep_sentinel != std::string_view::npos &&
                dep_after.substr(0, dep_sentinel) != relative_path) {
              leaf_entry.has_cross_leaf_deps = true;
              break;
            }
          }
        }
      }
    }

    std::string leaf_index_buffer;
    const auto leaf_count{static_cast<std::uint32_t>(save_leaf_index.size())};
    leaf_index_buffer.reserve(sizeof(LEAF_INDEX_MAGIC) + sizeof(leaf_count) +
                              leaf_count * (sizeof(LeafIndexRecord) + 64));
    leaf_index_buffer.append(reinterpret_cast<const char *>(&LEAF_INDEX_MAGIC),
                             sizeof(LEAF_INDEX_MAGIC));
    leaf_index_buffer.append(reinterpret_cast<const char *>(&leaf_count),
                             sizeof(leaf_count));

    for (const auto &[relative_path, leaf_entry] : save_leaf_index) {
      LeafIndexRecord record{};
      record.relative_path_offset = 0;
      record.relative_path_length =
          static_cast<std::uint16_t>(relative_path.size());
      record.root_mtime = std::chrono::duration_cast<std::chrono::nanoseconds>(
                              leaf_entry.root_mtime.time_since_epoch())
                              .count();
      record.target_bitmap = leaf_entry.target_bitmap;
      record.has_cross_leaf_deps = leaf_entry.has_cross_leaf_deps ? 1 : 0;
      record.padding = 0;
      leaf_index_buffer.append(reinterpret_cast<const char *>(&record),
                               sizeof(record));
      leaf_index_buffer.append(relative_path);
    }

    sourcemeta::core::atomic_write_file(
        path, [&](std::ostream &stream) -> void {
          sourcemeta::core::BinaryWriter writer{stream};
          writer.put_dword(STATE_MAGIC);
          writer.put_dword(STATE_VERSION);
          writer.put_dword(this->rules_fingerprint);
          writer.put_dword(capacity);
          writer.put_dword(output_count);
          writer.put_dword(total_pool_size);
          writer.put_dword(resolver_count);

          writer.put_bytes(reinterpret_cast<const std::byte *>(slots.data()),
                           slots.size());

          if (can_patch && old_pool_size > 0) {
            writer.put_bytes(
                reinterpret_cast<const std::byte *>(this->string_pool),
                old_pool_size);
          }
          if (!pool.empty()) {
            writer.put_bytes(reinterpret_cast<const std::byte *>(pool.data()),
                             pool.size());
          }

          writer.put_bytes(
              reinterpret_cast<const std::byte *>(leaf_index_buffer.data()),
              leaf_index_buffer.size());
        });
  }
}

} // namespace sourcemeta::one
