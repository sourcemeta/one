#include <sourcemeta/one/search.h>

#include <sourcemeta/one/metapack.h>

#include <algorithm> // std::ranges::search
#include <cassert>   // assert
#include <cctype>    // std::tolower
#include <sstream>   // std::ostringstream
#include <utility>   // std::move

namespace sourcemeta::one {

auto make_search(std::vector<SearchEntry> &&entries)
    -> std::vector<std::uint8_t> {
  // Prioritise entries that have more metadata filled in,
  // then sort lexicographically by path
  std::ranges::sort(entries, [](const SearchEntry &left,
                                const SearchEntry &right) {
    const auto left_score =
        (!left.title.empty() ? 1 : 0) + (!left.description.empty() ? 1 : 0);
    const auto right_score =
        (!right.title.empty() ? 1 : 0) + (!right.description.empty() ? 1 : 0);
    if (left_score != right_score) {
      return left_score > right_score;
    }

    // TODO: Ideally we sort based on schema health too, given
    // lint results
    if (left_score > 0) {
      return left.path < right.path;
    }

    return false;
  });

  std::ostringstream buffer;
  for (const auto &entry : entries) {
    auto json_entry{sourcemeta::core::JSON::make_array()};
    json_entry.push_back(sourcemeta::core::JSON{entry.path});
    json_entry.push_back(sourcemeta::core::JSON{entry.title});
    json_entry.push_back(sourcemeta::core::JSON{entry.description});
    sourcemeta::core::stringify(json_entry, buffer);
    buffer << '\n';
  }

  const auto result{buffer.str()};
  return {result.begin(), result.end()};
}

auto search(const std::uint8_t *payload, const std::size_t payload_size,
            const std::string_view query) -> sourcemeta::core::JSON {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  const std::string_view data{reinterpret_cast<const char *>(payload),
                              payload_size};

  auto result{sourcemeta::core::JSON::make_array()};
  std::size_t line_start{0};
  while (line_start < data.size()) {
    auto line_end{data.find('\n', line_start)};
    if (line_end == std::string_view::npos) {
      line_end = data.size();
    }

    const auto line{data.substr(line_start, line_end - line_start)};
    line_start = line_end + 1;

    if (line.empty()) {
      continue;
    }

    if (std::ranges::search(line, query, [](const auto left, const auto right) {
          return std::tolower(left) == std::tolower(right);
        }).empty()) {
      continue;
    }

    auto entry{sourcemeta::core::JSON::make_object()};
    const std::string line_string{line};
    auto line_json{sourcemeta::core::parse_json(line_string)};
    entry.assign("path", std::move(line_json.at(0)));
    entry.assign("title", std::move(line_json.at(1)));
    entry.assign("description", std::move(line_json.at(2)));
    result.push_back(std::move(entry));

    constexpr auto MAXIMUM_SEARCH_COUNT{10};
    if (result.array_size() >= MAXIMUM_SEARCH_COUNT) {
      break;
    }
  }

  return result;
}

SearchView::SearchView(std::filesystem::path path) : path_{std::move(path)} {}

SearchView::~SearchView() = default;

auto SearchView::ensure_open() -> void {
  if (this->view_) {
    return;
  }

  assert(std::filesystem::exists(this->path_));
  assert(this->path_.is_absolute());
  this->view_ = std::make_unique<sourcemeta::core::FileView>(this->path_);
  const auto payload_start_option{metapack_payload_offset(*this->view_)};
  assert(payload_start_option.has_value());
  const auto &payload_start{payload_start_option.value()};
  this->payload_size_ = this->view_->size() - payload_start;
  this->payload_ = this->view_->as<std::uint8_t>(payload_start);
}

auto SearchView::search(const std::string_view query)
    -> sourcemeta::core::JSON {
  this->ensure_open();
  return sourcemeta::one::search(this->payload_, this->payload_size_, query);
}

} // namespace sourcemeta::one
