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
  body.div().attribute("class", "container-fluid p-4");
  body.h2().attribute("class", "fw-bold");
  body.text("This page requires authentication");
  body.close();
  body.p().attribute("class", "lead");
  body.text("Choose how you want to sign in");
  body.close();

  body.div().attribute("class", "d-grid gap-2");
  for (const auto &policy : policies.as_array()) {
    if (!is_interactive(policy)) {
      continue;
    }

    // The link carries no return target on purpose. The login endpoint decides
    // where to land the caller, so nothing here depends on the requested path
    std::string href{configuration.base_path};
    href += "/self/v1/auth/login/";
    href += policy.at("name").to_string();
    body.a()
        .attribute("class", "btn btn-primary")
        .attribute("data-sourcemeta-ui-login", policy.at("name").to_string())
        .attribute("href", href);
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
  // the same policies: were it to embed the directory's own URL or a per-path
  // return target, a caller could tell an existing collection from a missing
  // one by comparing responses. So it names only its providers, its canonical
  // URL is the instance root like the plain denial page, and the return target
  // is deferred to the login endpoint
  sourcemeta::core::HTMLWriter writer;
  html::make_page(writer, configuration, configuration.url, "Sign In",
                  "Sign in to access this page",
                  [&](sourcemeta::core::HTMLWriter &body) -> void {
                    write_providers(body, configuration, policies);
                  });

  const auto timestamp_end{std::chrono::steady_clock::now()};
  metapack_write_text(action.destination, writer.str(),
                      "text/html; charset=utf-8", MetapackEncoding::GZIP, {},
                      std::chrono::duration_cast<std::chrono::milliseconds>(
                          timestamp_end - timestamp_start));
}

} // namespace sourcemeta::one
