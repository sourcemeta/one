#include <sourcemeta/one/web.h>

#include "../page.h"

#include <sourcemeta/core/html.h>
#include <sourcemeta/one/metapack.h>
#include <sourcemeta/one/shared.h>

#include <chrono> // std::chrono

namespace sourcemeta::one {

auto GENERATE_WEB_NOT_FOUND::handler(
    const sourcemeta::one::BuildState &,
    const sourcemeta::one::BuildPlan::Action &action,
    const sourcemeta::one::BuildDynamicCallback &, sourcemeta::one::Resolver &,
    const sourcemeta::one::Configuration &configuration,
    const sourcemeta::core::JSON &) -> void {
  const auto timestamp_start{std::chrono::steady_clock::now()};

  sourcemeta::core::HTMLWriter writer;
  html::make_error_page(writer, configuration, "Not Found",
                        "What you are looking for is not here",
                        "Oops! What you are looking for is not here",
                        "Are you sure the link you got is correct?");
  const auto timestamp_end{std::chrono::steady_clock::now()};

  metapack_write_text(action.destination, writer.str(),
                      "text/html; charset=utf-8", MetapackEncoding::GZIP, {},
                      std::chrono::duration_cast<std::chrono::milliseconds>(
                          timestamp_end - timestamp_start));
}

} // namespace sourcemeta::one
