#include <sourcemeta/one/web.h>

#include "../page.h"

#include <sourcemeta/core/html.h>
#include <sourcemeta/one/metapack.h>
#include <sourcemeta/one/shared.h>

#include <algorithm> // std::ranges::any_of
#include <cassert>   // assert
#include <chrono>    // std::chrono
#include <string>    // std::string

namespace sourcemeta::one {

namespace {

auto is_interactive(const sourcemeta::core::JSON &policy) -> bool {
  return policy.at("type").to_string() == "oidc";
}

auto write_providers(sourcemeta::core::HTMLWriter &body,
                     const sourcemeta::one::Configuration &configuration,
                     const sourcemeta::core::JSON &policies) -> void {
  body.p().attribute("class", "text-secondary text-center small mb-4");
  body.text("Choose how you want to sign in");
  body.close();

  body.div().attribute("class", "d-grid gap-2");
  for (const auto &policy : policies.as_array()) {
    if (!is_interactive(policy)) {
      continue;
    }

    // The link carries no return target on purpose. The login endpoint decides
    // where to land the caller, so nothing here depends on the requested path.
    // The whole page stays at the site-wide no-referrer default, so only this
    // navigation opts in to a same-origin referrer, handing the endpoint the
    // denied path while every other request from the page still leaks nothing
    std::string href{configuration.base_path};
    href += "/self/v1/auth/login/";
    href += policy.at("name").to_string();
    body.a()
        .attribute("class", "btn btn-primary d-flex align-items-center "
                            "justify-content-center")
        .attribute("data-sourcemeta-ui-login", policy.at("name").to_string())
        .attribute("referrerpolicy", "same-origin")
        .attribute("href", href);
    body.i().attribute("class", "bi bi-box-arrow-in-right me-2").close();
    body.text(policy.at("title").to_string());
    body.close();
  }
  body.close();
}

} // namespace

auto GENERATE_WEB_LOGIN::handler(
    const sourcemeta::one::BuildState &,
    const sourcemeta::one::BuildPlan::Action &action,
    const sourcemeta::one::BuildDynamicCallback &, sourcemeta::one::Resolver &,
    const sourcemeta::one::Configuration &configuration,
    const sourcemeta::core::JSON &) -> void {
  const auto timestamp_start{std::chrono::steady_clock::now()};

  const auto directory_option{metapack_read_json(action.dependencies.front())};
  assert(directory_option.has_value());
  const auto &directory{directory_option.value()};
  const auto &policies{directory.at("policies")};

  // A directory that no interactive policy governs has no login to offer, so it
  // gets an empty artifact rather than a page. Its presence keeps the build
  // plan uniform, and the empty body is the signal to fall back to the plain
  // denial when serving
  if (!std::ranges::any_of(policies.as_array(), is_interactive)) {
    const auto timestamp_end{std::chrono::steady_clock::now()};
    metapack_write_text(action.destination, "", "text/html; charset=utf-8",
                        MetapackEncoding::GZIP, {},
                        std::chrono::duration_cast<std::chrono::milliseconds>(
                            timestamp_end - timestamp_start));
    return;
  }

  // This page is served for every path an interactive policy governs, including
  // ones that resolve to no schema at all. Denials must not disclose which
  // schemas exist, so the page has to be byte-identical for every path under
  // the same policies: it names only its providers and the instance, its
  // canonical URL is the instance root, and the return target is deferred to
  // the login endpoint
  sourcemeta::core::HTMLWriter writer;
  writer.raw("<!DOCTYPE html>");
  writer.html().attribute("class", "h-100").attribute("lang", "en");
  html::make_head(writer, configuration, configuration.url, "Sign In",
                  "Sign in to access this page");
  writer.body().attribute("class",
                          "h-100 d-flex flex-column bg-body-secondary");

  writer.main().attribute(
      "class",
      "flex-grow-1 d-flex align-items-center justify-content-center p-3");
  writer.div()
      .attribute("class", "card bg-white border-0 shadow-sm w-100")
      .attribute("style", "max-width: 22rem;");
  writer.div().attribute("class", "card-body p-4");

  writer.div().attribute("class", "text-center mb-4");
  writer.img()
      .attribute("src", configuration.base_path + "/self/v1/static/icon.svg")
      .attribute("alt", "")
      .attribute("width", "48")
      .attribute("height", "48")
      .attribute("class", "mb-3");
  writer.h1().attribute("class", "h5 fw-bold mb-0");
  writer.text(configuration.html->name);
  writer.close();
  writer.close();

  write_providers(writer, configuration, policies);

  writer.close();
  writer.close();
  writer.close();

  html::make_footer(writer, configuration);
  writer.script()
      .attribute("async", "")
      .attribute("defer", "")
      .attribute("src", configuration.base_path +
                            "/self/v1/static/main.min.js?v=" +
                            std::string{SOURCEMETA_ONE_JS_CHECKSUM});
  writer.close();
  writer.close();
  writer.close();

  const auto timestamp_end{std::chrono::steady_clock::now()};
  metapack_write_text(action.destination, writer.str(),
                      "text/html; charset=utf-8", MetapackEncoding::GZIP, {},
                      std::chrono::duration_cast<std::chrono::milliseconds>(
                          timestamp_end - timestamp_start));
}

} // namespace sourcemeta::one
