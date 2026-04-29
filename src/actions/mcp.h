#ifndef SOURCEMETA_ONE_ACTIONS_MCP_H
#define SOURCEMETA_ONE_ACTIONS_MCP_H

#include <sourcemeta/core/json.h>

#include <sstream>     // std::ostringstream
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move

namespace sourcemeta::one {

inline auto mcp_empty() -> sourcemeta::core::JSON {
  auto result{sourcemeta::core::JSON::make_object()};
  result.assign("content", sourcemeta::core::JSON::make_array());
  return result;
}

inline auto mcp_json(sourcemeta::core::JSON payload) -> sourcemeta::core::JSON {
  std::ostringstream stringified;
  sourcemeta::core::prettify(payload, stringified);

  auto text_block{sourcemeta::core::JSON::make_object()};
  text_block.assign("type", sourcemeta::core::JSON{"text"});
  text_block.assign("text", sourcemeta::core::JSON{stringified.str()});

  auto content{sourcemeta::core::JSON::make_array()};
  content.push_back(std::move(text_block));

  auto result{sourcemeta::core::JSON::make_object()};
  result.assign("content", std::move(content));
  result.assign("structuredContent", std::move(payload));
  return result;
}

inline auto mcp_error(const std::string_view title,
                      const std::string_view detail) -> sourcemeta::core::JSON {
  std::string qualified_title{"sourcemeta:one/"};
  qualified_title.append(title);

  auto structured{sourcemeta::core::JSON::make_object()};
  structured.assign("title", sourcemeta::core::JSON{qualified_title});
  structured.assign("detail", sourcemeta::core::JSON{detail});

  auto text_block{sourcemeta::core::JSON::make_object()};
  text_block.assign("type", sourcemeta::core::JSON{"text"});
  text_block.assign("text", sourcemeta::core::JSON{detail});

  auto content{sourcemeta::core::JSON::make_array()};
  content.push_back(std::move(text_block));

  auto result{sourcemeta::core::JSON::make_object()};
  result.assign("isError", sourcemeta::core::JSON{true});
  result.assign("content", std::move(content));
  result.assign("structuredContent", std::move(structured));
  return result;
}

} // namespace sourcemeta::one

#endif
