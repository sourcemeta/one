#include <sourcemeta/one/web.h>

#include "../page.h"

#include <sourcemeta/core/html.h>
#include <sourcemeta/one/metapack.h>
#include <sourcemeta/one/shared.h>

#include <algorithm> // std::ranges::any_of
#include <chrono>    // std::chrono
#include <string>    // std::string

namespace sourcemeta::one {

namespace {

auto write_providers(sourcemeta::core::HTMLWriter &body,
                     const sourcemeta::one::Configuration &configuration)
    -> void {
  std::string prefix{configuration.base_path};
  prefix += "/self/v1/auth/login/";

  body.p().attribute("class", "text-secondary text-center small mb-4");
  body.text("Choose how you want to sign in");
  body.close();

  body.div().attribute("class", "d-grid gap-2");
  for (const auto &entry : configuration.authentication) {
    if (entry.type != Configuration::AuthenticationEntry::Type::OIDC) {
      continue;
    }

    body.a()
        .attribute("class", "btn btn-primary d-flex align-items-center "
                            "justify-content-center")
        .attribute("data-sourcemeta-ui-login", "")
        .attribute("href", prefix + entry.name);
    body.i().attribute("class", "bi bi-box-arrow-in-right me-2").close();
    body.text(entry.title);
    body.close();
  }
  body.close();
}

auto write_empty(sourcemeta::core::HTMLWriter &body,
                 const sourcemeta::one::Configuration &configuration) -> void {
  body.p().attribute("class", "text-secondary text-center small mb-4");
  body.text("No identity provider is configured for signing in");
  body.close();

  body.div().attribute("class", "d-grid");
  body.a()
      .attribute("class", "btn btn-outline-secondary")
      .attribute("href", configuration.url);
  body.text("Back to home");
  body.close();
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

  const auto interactive{std::ranges::any_of(
      configuration.authentication, [](const auto &entry) -> bool {
        return entry.type == Configuration::AuthenticationEntry::Type::OIDC;
      })};

  sourcemeta::core::HTMLWriter writer;
  writer.raw("<!DOCTYPE html>");
  writer.html().attribute("class", "h-100").attribute("lang", "en");
  html::make_head(writer, configuration,
                  configuration.url + "/self/v1/auth/login", "Sign In",
                  "Sign in to this instance");
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

  if (interactive) {
    write_providers(writer, configuration);
  } else {
    write_empty(writer, configuration);
  }

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

  const auto timestamp_end{std::chrono::steady_clock::now()};
  metapack_write_text(action.destination, writer.str(),
                      "text/html; charset=utf-8", MetapackEncoding::GZIP, {},
                      std::chrono::duration_cast<std::chrono::milliseconds>(
                          timestamp_end - timestamp_start));
}

} // namespace sourcemeta::one
