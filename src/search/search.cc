#include <sourcemeta/one/search.h>

#include <sourcemeta/core/text.h>

#include <sourcemeta/one/metapack.h>

#include <algorithm>   // std::min, std::ranges::search
#include <cassert>     // assert
#include <cctype>      // std::tolower
#include <cstring>     // std::memcpy
#include <functional>  // std::function
#include <limits>      // std::numeric_limits
#include <string_view> // std::string_view
#include <utility>     // std::move

namespace sourcemeta::one {

auto make_search(std::vector<SearchEntry> &&entries)
    -> std::vector<std::uint8_t> {
  std::ranges::sort(
      entries, [](const SearchEntry &left, const SearchEntry &right) -> bool {
        if (left.priority != right.priority) {
          return left.priority > right.priority;
        }

        const auto left_metadata =
            (!left.title.empty() ? 1 : 0) + (!left.description.empty() ? 1 : 0);
        const auto right_metadata = (!right.title.empty() ? 1 : 0) +
                                    (!right.description.empty() ? 1 : 0);
        if (left_metadata != right_metadata) {
          return left_metadata > right_metadata;
        }

        if (left.health != right.health) {
          return left.health > right.health;
        }

        return left.path < right.path;
      });

  constexpr auto MAX_FIELD_LENGTH{
      static_cast<std::size_t>(std::numeric_limits<std::uint16_t>::max())};
  constexpr std::string_view TRUNCATION_MARKER{"..."};
  for (auto &entry : entries) {
    sourcemeta::core::truncate(entry.title,
                               MAX_FIELD_LENGTH - TRUNCATION_MARKER.size(),
                               TRUNCATION_MARKER);
    sourcemeta::core::truncate(entry.description,
                               MAX_FIELD_LENGTH - TRUNCATION_MARKER.size(),
                               TRUNCATION_MARKER);
  }
  std::erase_if(entries, [](const SearchEntry &entry) -> bool {
    return entry.path.size() > MAX_FIELD_LENGTH ||
           entry.identifier.size() > MAX_FIELD_LENGTH;
  });

  if (entries.empty()) {
    return {};
  }

  const auto entry_count{static_cast<std::uint32_t>(entries.size())};

  // Compute total payload size
  std::size_t total_size{sizeof(SearchIndexHeader) +
                         entry_count * sizeof(std::uint32_t)};
  for (const auto &entry : entries) {
    total_size += sizeof(SearchRecordHeader) + entry.path.size() +
                  entry.identifier.size() + entry.title.size() +
                  entry.description.size();
  }

  std::vector<std::uint8_t> payload(total_size);
  const auto records_offset{static_cast<std::uint32_t>(
      sizeof(SearchIndexHeader) + entry_count * sizeof(std::uint32_t))};

  // Write header
  SearchIndexHeader header{.entry_count = entry_count,
                           .records_offset = records_offset};
  std::memcpy(payload.data(), &header, sizeof(SearchIndexHeader));

  // Write records and fill offset table
  auto *offset_table{payload.data() + sizeof(SearchIndexHeader)};
  std::size_t record_position{records_offset};
  for (std::uint32_t entry_index{0}; entry_index < entry_count; ++entry_index) {
    const auto &entry{entries[entry_index]};

    // Write this record's offset into the table
    const auto record_offset{static_cast<std::uint32_t>(record_position)};
    std::memcpy(offset_table + entry_index * sizeof(std::uint32_t),
                &record_offset, sizeof(std::uint32_t));

    // Write record header
    const SearchRecordHeader record_header{
        .path_length = static_cast<std::uint16_t>(entry.path.size()),
        .identifier_length =
            static_cast<std::uint16_t>(entry.identifier.size()),
        .title_length = static_cast<std::uint16_t>(entry.title.size()),
        .description_length =
            static_cast<std::uint16_t>(entry.description.size()),
        .bytes_raw = entry.bytes_raw,
        .bytes_bundled = entry.bytes_bundled,
        .priority = entry.priority,
        .health = entry.health};
    std::memcpy(payload.data() + record_position, &record_header,
                sizeof(SearchRecordHeader));
    record_position += sizeof(SearchRecordHeader);

    // Write field data
    std::memcpy(payload.data() + record_position, entry.path.data(),
                entry.path.size());
    record_position += entry.path.size();
    std::memcpy(payload.data() + record_position, entry.identifier.data(),
                entry.identifier.size());
    record_position += entry.identifier.size();
    std::memcpy(payload.data() + record_position, entry.title.data(),
                entry.title.size());
    record_position += entry.title.size();
    std::memcpy(payload.data() + record_position, entry.description.data(),
                entry.description.size());
    record_position += entry.description.size();
  }

  assert(record_position == total_size);
  return payload;
}

static auto case_insensitive_contains(const std::string_view haystack,
                                      const std::string_view needle) -> bool {
  return !std::ranges::search(
              haystack, needle,
              [](const auto left, const auto right) -> bool {
                return std::tolower(static_cast<unsigned char>(left)) ==
                       std::tolower(static_cast<unsigned char>(right));
              })
              .empty();
}

auto search(const std::uint8_t *payload, const std::size_t payload_size,
            const std::string_view query, const std::size_t limit,
            const std::uint8_t scope,
            const std::function<bool(std::string_view)> &filter)
    -> sourcemeta::core::JSON {
  assert((scope &
          ~(SearchScopePath | SearchScopeTitle | SearchScopeDescription)) == 0);
  auto result{sourcemeta::core::JSON::make_array()};
  if (limit == 0 || payload == nullptr ||
      payload_size < sizeof(SearchIndexHeader)) {
    return result;
  }

  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  const auto *header{reinterpret_cast<const SearchIndexHeader *>(payload)};

  if (header->entry_count == 0) {
    return result;
  }

  const auto offset_table_end{sizeof(SearchIndexHeader) +
                              static_cast<std::size_t>(header->entry_count) *
                                  sizeof(std::uint32_t)};
  if (offset_table_end > payload_size) {
    return result;
  }

  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  const auto *offset_table{reinterpret_cast<const std::uint32_t *>(
      payload + sizeof(SearchIndexHeader))};

  for (std::uint32_t entry_index{0}; entry_index < header->entry_count;
       ++entry_index) {
    const auto record_offset{offset_table[entry_index]};
    if (record_offset + sizeof(SearchRecordHeader) > payload_size) {
      break;
    }

    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    const auto *record_header{
        reinterpret_cast<const SearchRecordHeader *>(payload + record_offset)};

    const auto field_data_offset{record_offset + sizeof(SearchRecordHeader)};
    const auto total_field_length{
        static_cast<std::size_t>(record_header->path_length) +
        record_header->identifier_length + record_header->title_length +
        record_header->description_length};
    if (field_data_offset + total_field_length > payload_size) {
      break;
    }

    const auto *field_data{payload + field_data_offset};

    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    const std::string_view path{reinterpret_cast<const char *>(field_data),
                                record_header->path_length};
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    const std::string_view identifier{
        reinterpret_cast<const char *>(field_data + record_header->path_length),
        record_header->identifier_length};
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    const std::string_view title{
        reinterpret_cast<const char *>(field_data + record_header->path_length +
                                       record_header->identifier_length),
        record_header->title_length};
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    const std::string_view description{
        reinterpret_cast<const char *>(field_data + record_header->path_length +
                                       record_header->identifier_length +
                                       record_header->title_length),
        record_header->description_length};

    // Filtering happens inside the scan, before ranking and the limit,
    // so excluded entries neither consume result slots nor leak match
    // cardinality
    if (!filter(path)) {
      continue;
    }

    bool matched{false};

    if (!matched && (scope & SearchScopePath) != 0) {
      matched = case_insensitive_contains(path, query);
    }

    if (!matched && (scope & SearchScopeTitle) != 0) {
      matched = case_insensitive_contains(title, query);
    }

    if (!matched && (scope & SearchScopeDescription) != 0) {
      matched = case_insensitive_contains(description, query);
    }

    if (!matched) {
      continue;
    }

    auto entry{sourcemeta::core::JSON::make_object()};
    entry.assign("path", sourcemeta::core::JSON{path});
    entry.assign("identifier", sourcemeta::core::JSON{identifier});
    entry.assign("title", sourcemeta::core::JSON{title});
    entry.assign("description", sourcemeta::core::JSON{description});
    entry.assign("priority", sourcemeta::core::JSON{static_cast<std::int64_t>(
                                 record_header->priority)});
    entry.assign("health", sourcemeta::core::JSON{static_cast<std::int64_t>(
                               record_header->health)});
    result.push_back(std::move(entry));

    if (result.array_size() >= limit) {
      break;
    }
  }

  return result;
}

SearchView::SearchView(const std::filesystem::path &path) {
  assert(path.is_absolute());
  if (!std::filesystem::exists(path)) {
    return;
  }
  this->view_ = std::make_unique<sourcemeta::core::FileView>(path);
  const auto payload_start_option{metapack_payload_offset(*this->view_)};
  if (!payload_start_option.has_value()) {
    return;
  }
  const auto &payload_start{payload_start_option.value()};
  this->payload_size_ = this->view_->size() - payload_start;
  if (this->payload_size_ > 0) {
    this->payload_ = this->view_->as<std::uint8_t>(payload_start);
  }
}

SearchView::~SearchView() = default;

auto SearchView::search(const std::string_view query, const std::size_t limit,
                        const std::uint8_t scope,
                        const std::function<bool(std::string_view)> &filter)
    -> sourcemeta::core::JSON {
  return sourcemeta::one::search(this->payload_, this->payload_size_, query,
                                 limit, scope, filter);
}

auto SearchView::count() -> std::size_t {
  if (this->payload_ == nullptr ||
      this->payload_size_ < sizeof(SearchIndexHeader)) {
    return 0;
  }

  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  const auto *header{
      reinterpret_cast<const SearchIndexHeader *>(this->payload_)};
  const auto total{static_cast<std::size_t>(header->entry_count)};

  // A corrupt index can claim more entries than its offset table can hold, so
  // report none unless the whole table fits within the payload
  const auto offset_table_end{sizeof(SearchIndexHeader) +
                              total * sizeof(std::uint32_t)};
  if (offset_table_end > this->payload_size_) {
    return 0;
  }

  return total;
}

auto SearchView::at(const std::size_t index) -> SearchListEntry {
  if (this->payload_ == nullptr ||
      this->payload_size_ < sizeof(SearchIndexHeader)) {
    return {};
  }

  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  const auto *header{
      reinterpret_cast<const SearchIndexHeader *>(this->payload_)};
  const auto total{static_cast<std::size_t>(header->entry_count)};
  const auto offset_table_end{sizeof(SearchIndexHeader) +
                              total * sizeof(std::uint32_t)};
  // A corrupt index can lie about its entry count or point a record past the
  // payload. Every field length below is trusted only after it is checked to
  // fit, matching the validation the streaming iterator performs
  if (index >= total || offset_table_end > this->payload_size_) {
    return {};
  }

  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  const auto *offset_table{reinterpret_cast<const std::uint32_t *>(
      this->payload_ + sizeof(SearchIndexHeader))};
  const auto record_offset{offset_table[index]};
  if (record_offset + sizeof(SearchRecordHeader) > this->payload_size_) {
    return {};
  }

  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  const auto *record_header{reinterpret_cast<const SearchRecordHeader *>(
      this->payload_ + record_offset)};
  const auto field_data_offset{record_offset + sizeof(SearchRecordHeader)};
  const auto total_field_length{
      static_cast<std::size_t>(record_header->path_length) +
      record_header->identifier_length + record_header->title_length +
      record_header->description_length};
  if (field_data_offset + total_field_length > this->payload_size_) {
    return {};
  }

  const auto *field_data{this->payload_ + field_data_offset};

  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  const std::string_view path{reinterpret_cast<const char *>(field_data),
                              record_header->path_length};
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  const std::string_view identifier{
      reinterpret_cast<const char *>(field_data + record_header->path_length),
      record_header->identifier_length};
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  const std::string_view title{
      reinterpret_cast<const char *>(field_data + record_header->path_length +
                                     record_header->identifier_length),
      record_header->title_length};
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  const std::string_view description{
      reinterpret_cast<const char *>(field_data + record_header->path_length +
                                     record_header->identifier_length +
                                     record_header->title_length),
      record_header->description_length};

  return {.path = path,
          .identifier = identifier,
          .title = title,
          .description = description,
          .bytes_raw = record_header->bytes_raw,
          .bytes_bundled = record_header->bytes_bundled,
          .priority = record_header->priority,
          .health = record_header->health};
}

auto SearchView::for_each(
    const std::size_t offset, const std::size_t count,
    const std::function<void(const SearchListEntry &)> &callback) -> void {
  if (this->payload_ == nullptr ||
      this->payload_size_ < sizeof(SearchIndexHeader)) {
    return;
  }

  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  const auto *header{
      reinterpret_cast<const SearchIndexHeader *>(this->payload_)};
  const auto total{static_cast<std::size_t>(header->entry_count)};
  if (offset >= total || count == 0) {
    return;
  }

  const auto offset_table_end{sizeof(SearchIndexHeader) +
                              total * sizeof(std::uint32_t)};
  if (offset_table_end > this->payload_size_) {
    return;
  }

  const auto remaining{total - offset};
  const auto effective_count{std::min(count, remaining)};
  const auto last{offset + effective_count};

  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  const auto *offset_table{reinterpret_cast<const std::uint32_t *>(
      this->payload_ + sizeof(SearchIndexHeader))};

  for (std::size_t index{offset}; index < last; ++index) {
    const auto record_offset{offset_table[index]};
    if (record_offset + sizeof(SearchRecordHeader) > this->payload_size_) {
      break;
    }

    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    const auto *record_header{reinterpret_cast<const SearchRecordHeader *>(
        this->payload_ + record_offset)};
    const auto field_data_offset{record_offset + sizeof(SearchRecordHeader)};
    const auto total_field_length{
        static_cast<std::size_t>(record_header->path_length) +
        record_header->identifier_length + record_header->title_length +
        record_header->description_length};
    if (field_data_offset + total_field_length > this->payload_size_) {
      break;
    }

    const auto *field_data{this->payload_ + field_data_offset};

    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    const std::string_view path{reinterpret_cast<const char *>(field_data),
                                record_header->path_length};
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    const std::string_view identifier{
        reinterpret_cast<const char *>(field_data + record_header->path_length),
        record_header->identifier_length};
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    const std::string_view title{
        reinterpret_cast<const char *>(field_data + record_header->path_length +
                                       record_header->identifier_length),
        record_header->title_length};
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    const std::string_view description{
        reinterpret_cast<const char *>(field_data + record_header->path_length +
                                       record_header->identifier_length +
                                       record_header->title_length),
        record_header->description_length};

    callback({.path = path,
              .identifier = identifier,
              .title = title,
              .description = description,
              .bytes_raw = record_header->bytes_raw,
              .bytes_bundled = record_header->bytes_bundled,
              .priority = record_header->priority,
              .health = record_header->health});
  }
}

} // namespace sourcemeta::one
