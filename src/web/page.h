#ifndef SOURCEMETA_ONE_WEB_PAGE_H_
#define SOURCEMETA_ONE_WEB_PAGE_H_

#include <sourcemeta/core/html.h>
#include <sourcemeta/one/configuration.h>
#include <sourcemeta/one/shared.h>

#include "checksum_css.h"
#include "checksum_js.h"

#include <string>  // std::string
#include <utility> // std::forward

namespace sourcemeta::one::html {

inline auto make_navigation(sourcemeta::core::HTMLWriter &writer,
                            const Configuration &configuration) -> void {
  writer.nav().attribute("class", "navbar navbar-expand border-bottom bg-body");

  writer.div().attribute("class",
                         "container-fluid px-4 py-1 align-items-center "
                         "flex-column flex-md-row");

  // Brand link
  writer.a()
      .attribute("class", "navbar-brand me-0 me-md-3 d-flex align-items-center "
                          "w-100 w-md-auto")
      .attribute("href", configuration.url);
  writer.span().attribute("class", "fw-bold me-1");
  writer.text(configuration.html->name);
  writer.close();
  writer.span().attribute("class", "fw-lighter");
  writer.text(" Schemas");
  writer.close();
  writer.close();

  // Search section
  writer.div().attribute(
      "class", "mt-2 mt-md-0 flex-grow-1 position-relative w-100 w-md-auto");
  writer.div().attribute("class", "input-group");
  writer.span().attribute("class", "input-group-text");
  writer.i().attribute("class", "bi bi-search").close();
  writer.close();
  writer.input()
      .attribute("class", "form-control")
      .attribute("type", "search")
      .attribute("id", "search")
      .attribute("placeholder", "Search")
      .attribute("aria-label", "Search")
      .attribute("autocomplete", "off");
  writer.close();
  writer.ul()
      .attribute("class",
                 "d-none list-group position-absolute w-100 mt-2 shadow-sm")
      .attribute("id", "search-result");
  writer.close();
  writer.close();

  // Action button
  if (configuration.html->action.has_value()) {
    writer.a()
        .attribute("class", "ms-md-3 btn btn-dark mt-2 mt-md-0 w-100 w-md-auto")
        .attribute("role", "button")
        .attribute("href", configuration.html->action.value().url);
    writer.i()
        .attribute("class",
                   "me-2 bi bi-" + configuration.html->action.value().icon)
        .close();
    writer.text(configuration.html->action.value().title);
    writer.close();
  }

  writer.close();
  writer.close();
}

inline auto make_footer(sourcemeta::core::HTMLWriter &writer,
                        const Configuration &configuration) -> void {
  std::string information{" "};
  information += edition();
  information += " v";
  information += version();
  information += " \xC2\xA9 2026 ";

  writer.div().attribute("class", "container-fluid px-4 mb-2");
  writer.footer().attribute(
      "class", "border-top text-secondary py-3 d-flex align-items-center "
               "justify-content-between flex-column flex-md-row");

  // Left section
  writer.small().attribute("class", "mb-2 mb-md-0");
  writer.img()
      .attribute("src", configuration.base_path + "/self/static/icon.svg")
      .attribute("alt", "Sourcemeta")
      .attribute("height", "25")
      .attribute("width", "25")
      .attribute("class", "me-2");
  writer.a()
      .attribute("href", "https://github.com/sourcemeta/one")
      .attribute("class", "text-secondary")
      .attribute("target", "_blank");
  writer.text("One");
  writer.close();
  writer.text(information);
  writer.a()
      .attribute("href", "https://www.sourcemeta.com")
      .attribute("class", "text-secondary")
      .attribute("target", "_blank");
  writer.text("Sourcemeta");
  writer.close();
  writer.close();

  // Right section
  writer.small();
  writer.a()
      .attribute("href", "https://github.com/sourcemeta/one/discussions")
      .attribute("class", "text-secondary")
      .attribute("target", "_blank");
  writer.i().attribute("class", "bi bi-question-square me-2").close();
  writer.text("Need Help?");
  writer.close();
  writer.close();

  writer.close();
  writer.close();
}

inline auto make_head(sourcemeta::core::HTMLWriter &writer,
                      const Configuration &configuration,
                      const std::string &canonical,
                      const std::string &page_title,
                      const std::string &description) -> void {
  writer.head();
  writer.meta().attribute("charset", "utf-8");
  writer.meta()
      .attribute("name", "referrer")
      .attribute("content", "no-referrer");
  writer.meta()
      .attribute("name", "viewport")
      .attribute("content", "width=device-width, initial-scale=1.0");
  writer.meta()
      .attribute("http-equiv", "x-ua-compatible")
      .attribute("content", "ie=edge");
  writer.title(page_title);
  writer.meta()
      .attribute("name", "description")
      .attribute("content", description);
  writer.meta()
      .attribute("name", "base-path")
      .attribute("content", configuration.base_path);
  writer.link().attribute("rel", "canonical").attribute("href", canonical);
  writer.link()
      .attribute("rel", "stylesheet")
      .attribute("href",
                 // For cache busting, to force browsers to refresh styles
                 // on any update
                 configuration.base_path + "/self/static/style.min.css?v=" +
                     std::string{SOURCEMETA_ONE_CSS_CHECKSUM});
  writer.link()
      .attribute("rel", "icon")
      .attribute("href", configuration.base_path + "/self/static/favicon.ico")
      .attribute("sizes", "any");
  writer.link()
      .attribute("rel", "icon")
      .attribute("href", configuration.base_path + "/self/static/icon.svg")
      .attribute("type", "image/svg+xml");
  writer.link()
      .attribute("rel", "shortcut icon")
      .attribute("href",
                 configuration.base_path + "/self/static/apple-touch-icon.png")
      .attribute("type", "image/png");
  writer.link()
      .attribute("rel", "apple-touch-icon")
      .attribute("href",
                 configuration.base_path + "/self/static/apple-touch-icon.png")
      .attribute("sizes", "180x180");
  writer.link()
      .attribute("rel", "manifest")
      .attribute("href",
                 configuration.base_path + "/self/static/manifest.webmanifest");
  writer.raw(configuration.html->head.value_or(""));
  writer.close();
}

template <typename BodyWriter>
inline auto make_page(sourcemeta::core::HTMLWriter &writer,
                      const Configuration &configuration,
                      const std::string &canonical, const std::string &title,
                      const std::string &description, BodyWriter &&write_body)
    -> void {
  writer.raw("<!DOCTYPE html>");
  writer.html().attribute("class", "h-100").attribute("lang", "en");
  make_head(writer, configuration, canonical, title, description);
  writer.body().attribute("class", "h-100 d-flex flex-column");
  make_navigation(writer, configuration);
  std::forward<BodyWriter>(write_body)(writer);
  make_footer(writer, configuration);
  writer.script()
      .attribute("async", "")
      .attribute("defer", "")
      .attribute("src",
                 // For cache busting, to force browsers to refresh styles
                 // on any update
                 configuration.base_path + "/self/static/main.min.js?v=" +
                     std::string{SOURCEMETA_ONE_JS_CHECKSUM});
  writer.close();
  writer.close();
  writer.close();
}

} // namespace sourcemeta::one::html

#endif
