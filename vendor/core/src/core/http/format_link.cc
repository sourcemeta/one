#include <sourcemeta/core/http.h>

#include <cstddef> // std::size_t
#include <span>    // std::span
#include <string>  // std::string

namespace {

constexpr std::size_t LINK_PREFIX_BYTES{1};
constexpr std::size_t LINK_REL_BYTES{8};
constexpr std::size_t LINK_REL_TRAILER_BYTES{1};
constexpr std::size_t LINK_PARAMETER_BYTES{4};

auto required_size(const sourcemeta::core::HTTPLink &link) noexcept
    -> std::size_t {
  std::size_t size{LINK_PREFIX_BYTES + link.target.size() + LINK_REL_BYTES +
                   link.rel.size() + LINK_REL_TRAILER_BYTES};
  for (const auto &[name, value] : link.parameters) {
    size += LINK_PARAMETER_BYTES + name.size() + value.size();
  }
  return size;
}

} // namespace

namespace sourcemeta::core {

auto http_format_link(const HTTPLink &link, std::string &out) -> void {
  out.reserve(out.size() + required_size(link));
  out.push_back('<');
  out.append(link.target);
  out.append(">; rel=\"");
  out.append(link.rel);
  out.push_back('"');
  for (const auto &[name, value] : link.parameters) {
    out.append("; ");
    out.append(name);
    out.append("=\"");
    out.append(value);
    out.push_back('"');
  }
}

auto http_format_link(const HTTPLink &link) -> std::string {
  std::string out;
  http_format_link(link, out);
  return out;
}

auto http_format_links(std::span<const HTTPLink> links, std::string &out)
    -> void {
  std::size_t total{0};
  for (const auto &link : links) {
    total += required_size(link) + 2;
  }
  if (total >= 2) {
    total -= 2;
  }
  out.reserve(out.size() + total);
  bool first{true};
  for (const auto &link : links) {
    if (!first) {
      out.append(", ");
    }
    http_format_link(link, out);
    first = false;
  }
}

auto http_format_links(std::span<const HTTPLink> links) -> std::string {
  std::string out;
  http_format_links(links, out);
  return out;
}

} // namespace sourcemeta::core
