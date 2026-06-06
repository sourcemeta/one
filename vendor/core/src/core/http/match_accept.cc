#include <sourcemeta/core/http.h>

#include "helpers.h"

#include <cassert>          // assert
#include <cstddef>          // std::size_t
#include <cstdint>          // std::uint8_t
#include <initializer_list> // std::initializer_list
#include <string_view>      // std::string_view

namespace {

auto media_specificity(const std::string_view range,
                       const std::string_view candidate) noexcept
    -> std::uint8_t {
  if (sourcemeta::core::http_iequals_ascii(range, candidate)) {
    return 3;
  }
  if (range == "*/*") {
    return 1;
  }
  const auto range_slash{range.find('/')};
  const auto candidate_slash{candidate.find('/')};
  if (range_slash == std::string_view::npos ||
      candidate_slash == std::string_view::npos) {
    return 0;
  }
  if (range.size() - range_slash != 2 || range[range_slash + 1] != '*') {
    return 0;
  }
  if (range_slash != candidate_slash) {
    return 0;
  }
  for (std::size_t index{0}; index < range_slash; ++index) {
    if (sourcemeta::core::http_ascii_lower(range[index]) !=
        sourcemeta::core::http_ascii_lower(candidate[index])) {
      return 0;
    }
  }
  return 2;
}

} // namespace

namespace sourcemeta::core {

auto http_match_accept(const std::string_view accept_header,
                       const std::initializer_list<std::string_view> candidates)
    -> std::string_view {
  if (candidates.size() == 0) {
    return {};
  }
  if (http_trim_leading_ows(accept_header).empty()) {
    return *candidates.begin();
  }

  std::string_view best{};
  float best_quality{0.0f};
  std::uint8_t best_specificity{0};
  std::size_t best_order{candidates.size()};

  std::size_t order{0};
  for (const auto candidate : candidates) {
    assert(!candidate.empty());
    assert(candidate.find('/') != std::string_view::npos);
    assert(candidate.find('/') > 0);
    assert(candidate.find('/') < candidate.size() - 1);
    assert(candidate.find_first_of(" \t,;*") == std::string_view::npos);
    float candidate_quality{0.0f};
    std::uint8_t candidate_specificity{0};
    http_for_each_accept_entry(
        accept_header, [&](const std::string_view value, const float quality) {
          const std::uint8_t specificity{media_specificity(value, candidate)};
          if (specificity == 0) {
            return;
          }
          if (specificity > candidate_specificity ||
              (specificity == candidate_specificity &&
               quality > candidate_quality)) {
            candidate_quality = quality;
            candidate_specificity = specificity;
          }
        });
    if (candidate_quality > 0.0f &&
        (candidate_quality > best_quality ||
         (candidate_quality == best_quality &&
          candidate_specificity > best_specificity) ||
         (candidate_quality == best_quality &&
          candidate_specificity == best_specificity && order < best_order))) {
      best = candidate;
      best_quality = candidate_quality;
      best_specificity = candidate_specificity;
      best_order = order;
    }
    ++order;
  }
  return best;
}

} // namespace sourcemeta::core
