#include <sourcemeta/one/web.h>

#include "../helpers.h"
#include "../page.h"

#include <sourcemeta/core/html.h>
#include <sourcemeta/one/metapack.h>
#include <sourcemeta/one/shared.h>

#include <cassert> // assert
#include <chrono>  // std::chrono

namespace sourcemeta::one {

auto GenerateWebDirectory::handler(
    const sourcemeta::one::BuildState &,
    const sourcemeta::one::BuildPlan::Action &action,
    const sourcemeta::one::BuildDynamicCallback &, sourcemeta::one::Resolver &,
    const sourcemeta::one::Configuration &configuration,
    const sourcemeta::core::JSON &) -> void {
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
  html::make_page(writer, configuration, action.view, canonical, title,
                  description, [&](sourcemeta::core::HTMLWriter &body) -> void {
                    html::make_breadcrumb(body, directory.at("breadcrumb"));
                    html::make_directory_header(body, directory);
                    html::make_file_manager(body, directory);
                  });

  const auto timestamp_end{std::chrono::steady_clock::now()};
  metapack_write_text(action.destination, writer.str(),
                      "text/html; charset=utf-8", MetapackEncoding::GZIP, {},
                      std::chrono::duration_cast<std::chrono::milliseconds>(
                          timestamp_end - timestamp_start));
}

} // namespace sourcemeta::one
