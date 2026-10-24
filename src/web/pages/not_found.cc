#include <sourcemeta/one/web.h>

#include "../page.h"

#include <sourcemeta/core/html.h>
#include <sourcemeta/one/metapack.h>
#include <sourcemeta/one/shared.h>

#include <chrono> // std::chrono

namespace sourcemeta::one {

auto GenerateWebNotFound::handler(
    const sourcemeta::one::BuildState &,
    const sourcemeta::one::BuildPlan::Action &action,
    const sourcemeta::one::BuildDynamicCallback &, sourcemeta::one::Resolver &,
    const sourcemeta::one::Configuration &configuration,
    const sourcemeta::core::JSON &) -> void {

  sourcemeta::core::HTMLWriter writer;
  html::make_error_page(writer, configuration, action.view, "Not Found",
                        "What you are looking for is not here",
                        "Oops! What you are looking for is not here",
                        "Are you sure the link you got is correct?");

  metapack_write_text(action.destination, writer.str(),
                      "text/html; charset=utf-8", MetapackEncoding::GZIP, {});
}

} // namespace sourcemeta::one
