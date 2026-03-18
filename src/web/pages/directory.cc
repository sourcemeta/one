#include <sourcemeta/one/web.h>

#include "../helpers.h"
#include "../page.h"

#include <sourcemeta/core/html.h>
#include <sourcemeta/one/metapack.h>
#include <sourcemeta/one/shared.h>

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

  const auto directory{metapack_read_json(action.dependencies.front())};
  const auto &canonical{directory.at("url").to_string()};
  const auto &title{directory.defines("title")
                        ? directory.at("title").to_string()
                        : directory.at("path").to_string()};
  const auto description{
      directory.defines("description")
          ? directory.at("description").to_string()
          : ("Schemas located at " + directory.at("path").to_string())};
  std::ostringstream html_content;
  html_content << "<!DOCTYPE html>"
               << html::make_page(
                      configuration, canonical, title, description,
                      html::make_breadcrumb(directory.at("breadcrumb")),
                      html::make_directory_header(directory),
                      html::make_file_manager(directory));

  const auto timestamp_end{std::chrono::steady_clock::now()};
  metapack_write_text(action.destination, html_content.str(), "text/html",
                      MetapackEncoding::GZIP, {},
                      std::chrono::duration_cast<std::chrono::milliseconds>(
                          timestamp_end - timestamp_start));
  return true;
}

} // namespace sourcemeta::one
