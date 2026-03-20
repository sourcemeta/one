#include <sourcemeta/one/web.h>

#include "../helpers.h"
#include "../page.h"

#include <sourcemeta/core/html.h>
#include <sourcemeta/one/metapack.h>
#include <sourcemeta/one/shared.h>

#include <chrono>     // std::chrono
#include <filesystem> // std::filesystem

namespace sourcemeta::one {

auto GENERATE_WEB_NOT_FOUND::handler(
    const sourcemeta::one::BuildState &,
    const sourcemeta::one::BuildPlan::Action &action,
    const sourcemeta::one::BuildDynamicCallback &, sourcemeta::one::Resolver &,
    const sourcemeta::one::Configuration &configuration,
    const sourcemeta::core::JSON &) -> void {
  const auto timestamp_start{std::chrono::steady_clock::now()};

  const auto &canonical{configuration.url};
  const auto title{"Not Found"};
  const auto description{"What you are looking for is not here"};
  sourcemeta::core::HTMLWriter writer;
  html::make_page(writer, configuration, canonical, title, description,
                  [&](sourcemeta::core::HTMLWriter &w) {
                    w.div().attribute("class", "container-fluid p-4");
                    w.h2().attribute("class", "fw-bold");
                    w.text("Oops! What you are looking for is not here");
                    w.close();
                    w.p().attribute("class", "lead");
                    w.text("Are you sure the link you got is correct?");
                    w.close();
                    w.a().attribute("href", "/");
                    w.text("Get back to the home page");
                    w.close();
                    w.close();
                  });
  const auto timestamp_end{std::chrono::steady_clock::now()};

  metapack_write_text(action.destination, writer.str(), "text/html",
                      MetapackEncoding::GZIP, {},
                      std::chrono::duration_cast<std::chrono::milliseconds>(
                          timestamp_end - timestamp_start));
}

} // namespace sourcemeta::one
