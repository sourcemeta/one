#include <sourcemeta/one/web.h>

#include "../page.h"

#include <sourcemeta/core/html.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/one/metapack.h>
#include <sourcemeta/one/shared.h>

#include <cassert> // assert
#include <chrono>  // std::chrono
#include <string>  // std::string

namespace sourcemeta::one {

namespace {

auto write_providers(sourcemeta::core::HTMLWriter &body,
                     const sourcemeta::core::JSON &providers) -> void {
  // An instance nobody can sign in to still has a page, and it says so rather
  // than showing an empty space where the ways in would be. What the endpoint
  // answers and what this shows are then the same statement
  if (providers.empty()) {
    body.p().attribute("class", "text-secondary text-center small mb-0");
    body.text("There is no way to sign in to this instance");
    body.close();
    return;
  }

  body.p().attribute("class", "text-secondary text-center small mb-4");
  body.text("Choose how you want to sign in");
  body.close();

  body.div().attribute("class", "d-grid gap-2");
  for (const auto &provider : providers.as_array()) {
    // The link names no return target and, staying at the page's own
    // no-referrer default, carries no referrer either, so the endpoint lands
    // the caller on what the policy governs. Somebody here arrived rather than
    // was sent, so there is no earlier page owed to them, and naming this one
    // would only return them to signing in once they had signed in
    body.a()
        .attribute("class", "btn btn-primary d-flex align-items-center "
                            "justify-content-center")
        .attribute("data-sourcemeta-ui-login", provider.at("name").to_string())
        .attribute("href", provider.at("path").to_string());
    body.i().attribute("class", "bi bi-box-arrow-in-right me-2").close();
    body.text(provider.at("title").to_string());
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

  // Everything this page says about signing in comes from here, so the page and
  // the endpoint that serves the same answer as data cannot drift apart
  const auto login_option{metapack_read_json(action.dependencies.front())};
  assert(login_option.has_value());
  const auto &login{login_option.value()};

  // Only an instance that renders HTML at all renders this, and such an
  // instance is always named
  assert(login.defines("title"));

  // Signing in is what somebody without a session does, so the instance holds
  // one page for all of them rather than one per place it could be reached
  // from. It names only its providers and the instance, its canonical URL is
  // the instance root, and the return target is deferred to the login endpoint
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
      .attribute("src", "/self/v1/static/icon.svg")
      .attribute("alt", "")
      .attribute("width", "48")
      .attribute("height", "48")
      .attribute("class", "mb-3");
  writer.h1().attribute("class", "h5 fw-bold mb-0");
  writer.text(login.at("title").to_string());
  writer.close();
  writer.close();

  write_providers(writer, login.at("providers"));

  writer.close();
  writer.close();
  writer.close();

  html::make_footer(writer);
  writer.script()
      .attribute("async", "")
      .attribute("defer", "")
      .attribute("src", "/self/v1/static/main.min.js?v=" +
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
