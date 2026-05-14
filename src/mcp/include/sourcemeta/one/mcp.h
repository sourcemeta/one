#ifndef SOURCEMETA_ONE_MCP_H
#define SOURCEMETA_ONE_MCP_H

#include <sourcemeta/core/json.h>

#include <sourcemeta/one/jsonrpc.h>

#include <sstream>     // std::ostringstream
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move

namespace sourcemeta::one {

inline auto mcp_make_tool_success(const sourcemeta::core::JSON &id,
                                  sourcemeta::core::JSON result)
    -> sourcemeta::core::JSON {
  std::ostringstream payload;
  sourcemeta::core::prettify(result, payload);

  auto text_block{sourcemeta::core::JSON::make_object()};
  text_block.assign_assume_new(std::string{"type"},
                               sourcemeta::core::JSON{"text"});
  text_block.assign_assume_new(std::string{"text"},
                               sourcemeta::core::JSON{payload.str()});

  auto content{sourcemeta::core::JSON::make_array()};
  content.push_back(std::move(text_block));

  auto envelope_result{sourcemeta::core::JSON::make_object()};
  envelope_result.assign_assume_new(std::string{"content"}, std::move(content));
  envelope_result.assign_assume_new(std::string{"structuredContent"},
                                    std::move(result));
  envelope_result.assign_assume_new(std::string{"isError"},
                                    sourcemeta::core::JSON{false});
  return jsonrpc_make_success(id, std::move(envelope_result));
}

inline auto mcp_make_tool_error(const sourcemeta::core::JSON &id,
                                const std::string_view message)
    -> sourcemeta::core::JSON {
  auto text_block{sourcemeta::core::JSON::make_object()};
  text_block.assign_assume_new(std::string{"type"},
                               sourcemeta::core::JSON{"text"});
  text_block.assign_assume_new(std::string{"text"},
                               sourcemeta::core::JSON{message});

  auto content{sourcemeta::core::JSON::make_array()};
  content.push_back(std::move(text_block));

  auto envelope_result{sourcemeta::core::JSON::make_object()};
  envelope_result.assign_assume_new(std::string{"content"}, std::move(content));
  envelope_result.assign_assume_new(std::string{"isError"},
                                    sourcemeta::core::JSON{true});
  return jsonrpc_make_success(id, std::move(envelope_result));
}

} // namespace sourcemeta::one

#endif
