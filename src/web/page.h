#ifndef SOURCEMETA_ONE_WEB_PAGE_H_
#define SOURCEMETA_ONE_WEB_PAGE_H_

#include <sourcemeta/core/html.h>
#include <sourcemeta/one/authentication.h>
#include <sourcemeta/one/configuration.h>
#include <sourcemeta/one/shared.h>

#include "checksum_css.h"
#include "checksum_js.h"

#include <algorithm>   // std::ranges::any_of, std::ranges::find
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::forward

namespace sourcemeta::one::html {

// Whether a policy signs a person in rather than admitting a program, which is
// what decides everywhere a way in or out could be offered
[[nodiscard]] inline auto
is_interactive(const Configuration::AuthenticationEntry &policy) -> bool {
  return policy.type == Configuration::AuthenticationEntry::Type::OIDC;
}

// What the bar offers follows from the view it is written for, since a page is
// written once per view and read by whoever that view is for
inline auto make_session_control(sourcemeta::core::HTMLWriter &writer,
                                 const Configuration &configuration,
                                 const std::string_view view) -> void {
  // The anonymous view is what somebody without a session is served, so it
  // offers the way in, and only where there is somewhere to go. An instance
  // nobody signs into interactively grows no control at all, which is also
  // what an instance with no policies at all gets
  if (view == VIEW_PUBLIC) {
    if (!std::ranges::any_of(configuration.authentication, is_interactive)) {
      return;
    }

    writer.a()
        .attribute("class", "ms-md-3 btn btn-outline-secondary mt-2 mt-md-0 "
                            "w-100 w-md-auto")
        .attribute("role", "button")
        .attribute("data-sourcemeta-ui-signin", "")
        .attribute("href", "/self/v1/auth/login");
    writer.i().attribute("class", "me-2 bi bi-box-arrow-in-right").close();
    writer.text("Sign In");
    writer.close();
    return;
  }

  // Every other view is named after what admits it, so a view somebody signed
  // into is one a policy of that name signs people into. A view a program
  // reaches has no session behind it and so nothing to end
  const auto policy{
      std::ranges::find(configuration.authentication, view,
                        &Configuration::AuthenticationEntry::name)};
  if (policy == configuration.authentication.cend() ||
      !is_interactive(*policy)) {
    return;
  }

  // Signing out ends a session at the provider, which RFC 9110 Section 9.2.1
  // puts outside what a link may do, so the control is a form rather than an
  // anchor
  writer.form()
      .attribute("class", "ms-md-3 mt-2 mt-md-0 w-100 w-md-auto")
      .attribute("method", "post")
      .attribute("action", "/self/v1/auth/logout");
  writer.button()
      .attribute("class", "btn btn-outline-secondary w-100")
      .attribute("type", "submit")
      .attribute("data-sourcemeta-ui-signout", "");
  writer.i().attribute("class", "me-2 bi bi-box-arrow-right").close();
  writer.text("Sign Out");
  writer.close();
  writer.close();
}

inline auto make_navigation(sourcemeta::core::HTMLWriter &writer,
                            const Configuration &configuration,
                            const std::string_view view) -> void {
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

  make_session_control(writer, configuration, view);

  writer.close();
  writer.close();
}

inline auto make_footer(sourcemeta::core::HTMLWriter &writer) -> void {
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
      .attribute("src", "/self/v1/static/icon.svg")
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
  writer.link().attribute("rel", "canonical").attribute("href", canonical);
  writer.link()
      .attribute("rel", "stylesheet")
      .attribute("href",
                 // For cache busting, to force browsers to refresh styles
                 // on any update
                 "/self/v1/static/style.min.css?v=" +
                     std::string{SOURCEMETA_ONE_CSS_CHECKSUM});
  writer.link()
      .attribute("rel", "icon")
      .attribute("href", "/self/v1/static/favicon.ico")
      .attribute("sizes", "any");
  writer.link()
      .attribute("rel", "icon")
      .attribute("href", "/self/v1/static/icon.svg")
      .attribute("type", "image/svg+xml");
  writer.link()
      .attribute("rel", "shortcut icon")
      .attribute("href", "/self/v1/static/apple-touch-icon.png")
      .attribute("type", "image/png");
  writer.link()
      .attribute("rel", "apple-touch-icon")
      .attribute("href", "/self/v1/static/apple-touch-icon.png")
      .attribute("sizes", "180x180");
  writer.link()
      .attribute("rel", "manifest")
      .attribute("href", "/self/v1/static/manifest.webmanifest");
  writer.raw(configuration.html->head.value_or(""));
  writer.close();
}

template <typename BodyWriter>
inline auto make_page(sourcemeta::core::HTMLWriter &writer,
                      const Configuration &configuration,
                      const std::string_view view, const std::string &canonical,
                      const std::string &title, const std::string &description,
                      BodyWriter &&write_body) -> void {
  writer.raw("<!DOCTYPE html>");
  writer.html().attribute("class", "h-100").attribute("lang", "en");
  make_head(writer, configuration, canonical, title, description);
  writer.body().attribute("class", "h-100 d-flex flex-column");
  make_navigation(writer, configuration, view);
  std::forward<BodyWriter>(write_body)(writer);
  make_footer(writer);
  writer.script()
      .attribute("async", "")
      .attribute("defer", "")
      .attribute("src",
                 // For cache busting, to force browsers to refresh styles
                 // on any update
                 "/self/v1/static/main.min.js?v=" +
                     std::string{SOURCEMETA_ONE_JS_CHECKSUM});
  writer.close();
  writer.close();
  writer.close();
}

inline auto
make_error_page(sourcemeta::core::HTMLWriter &writer,
                const Configuration &configuration, const std::string_view view,
                const std::string &title, const std::string &description,
                const std::string &heading, const std::string &lead) -> void {
  make_page(writer, configuration, view, configuration.url, title, description,
            [&](sourcemeta::core::HTMLWriter &body) -> void {
              body.div().attribute("class", "container-fluid p-4");
              body.h2().attribute("class", "fw-bold");
              body.text(heading);
              body.close();
              body.p().attribute("class", "lead");
              body.text(lead);
              body.close();
              body.a().attribute("href", configuration.url);
              body.text("Get back to the home page");
              body.close();
              body.close();
            });
}

} // namespace sourcemeta::one::html

#endif
