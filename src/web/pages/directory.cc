#include <sourcemeta/one/web.h>

#include "../helpers.h"
#include "../page.h"

#include <sourcemeta/core/html.h>
#include <sourcemeta/one/metapack.h>
#include <sourcemeta/one/shared.h>

#include <cassert>    // assert
#include <chrono>     // std::chrono
#include <filesystem> // std::filesystem

namespace sourcemeta::one {

auto GENERATE_WEB_DIRECTORY::handler(
    const sourcemeta::one::BuildState &,
    const sourcemeta::one::BuildPlan::Action &action,
    const sourcemeta::one::BuildDynamicCallback &, sourcemeta::one::Resolver &,
    const sourcemeta::one::Configuration &configuration,
    const sourcemeta::core::JSON &) -> bool {
  const auto timestamp_start{std::chrono::steady_clock::now()};

  const auto directory_option{metapack_read_json(action.dependencies.front())};
  assert(directory_option.has_value());
  const auto &directory{directory_option.value()};
  const auto &canonical{directory.at("url").to_string()};
  const auto &title{directory.defines("title")
                        ? directory.at("title").to_string()
                        : directory.at("path").to_string()};
  const auto description{
      directory.defines("description")
          ? directory.at("description").to_string()
          : ("Schemas located at " + directory.at("path").to_string())};
  sourcemeta::core::HTMLWriter writer;
  html::make_page(writer, configuration, canonical, title, description,
                  [&](sourcemeta::core::HTMLWriter &w) {
                    html::make_breadcrumb(w, directory.at("breadcrumb"));
                    html::make_directory_header(w, directory);
                    html::make_file_manager(w, directory);
                  });

  const auto timestamp_end{std::chrono::steady_clock::now()};
  metapack_write_text(action.destination, writer.str(), "text/html",
                      MetapackEncoding::GZIP, {},
                      std::chrono::duration_cast<std::chrono::milliseconds>(
                          timestamp_end - timestamp_start));
  return true;
}

} // namespace sourcemeta::one
