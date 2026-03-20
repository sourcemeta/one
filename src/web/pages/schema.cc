#include <sourcemeta/one/web.h>

#include "../helpers.h"
#include "../page.h"

#include <sourcemeta/core/html.h>
#include <sourcemeta/one/metapack.h>
#include <sourcemeta/one/shared.h>

#include <cassert>    // assert
#include <chrono>     // std::chrono
#include <filesystem> // std::filesystem
#include <sstream>    // std::ostringstream

namespace sourcemeta::one {

auto GENERATE_WEB_SCHEMA::handler(
    const sourcemeta::one::BuildState &,
    const sourcemeta::one::BuildPlan::Action &action,
    const sourcemeta::one::BuildDynamicCallback &, sourcemeta::one::Resolver &,
    const sourcemeta::one::Configuration &configuration,
    const sourcemeta::core::JSON &) -> void {
  const auto timestamp_start{std::chrono::steady_clock::now()};

  const auto meta_option{metapack_read_json(action.dependencies.front())};
  assert(meta_option.has_value());
  const auto &meta{meta_option.value()};
  const auto &canonical{meta.at("identifier").to_string()};
  const auto &title{meta.defines("title") ? meta.at("title").to_string()
                                          : meta.at("path").to_string()};
  const auto description{
      meta.defines("description")
          ? meta.at("description").to_string()
          : ("Schemas located at " + meta.at("path").to_string())};

  const auto health_option{metapack_read_json(action.dependencies.at(1))};
  assert(health_option.has_value());
  const auto &health{health_option.value()};
  assert(health.is_object());
  assert(health.defines("errors"));

  sourcemeta::core::HTMLWriter writer;
  html::make_page(
      writer, configuration, canonical, title, description,
      [&](sourcemeta::core::HTMLWriter &w) {
        html::make_breadcrumb(w, meta.at("breadcrumb"));

        w.div().attribute("class", "container-fluid p-4");

        // Content wrapper div
        w.div();

        // Header div
        w.div();
        if (meta.defines("title")) {
          w.h2().attribute("class", "fw-bold h4");
          w.text(title);
          w.close();
        }
        if (meta.defines("description")) {
          w.p().attribute("class", "text-secondary");
          w.text(meta.at("description").to_string());
          w.close();
        }
        w.a()
            .attribute("href", meta.at("path").to_string() + ".json")
            .attribute("class", "btn btn-primary me-2")
            .attribute("role", "button");
        w.text("Get JSON Schema");
        w.close();
        w.a()
            .attribute("href", meta.at("path").to_string() + ".json?bundle=1")
            .attribute("class", "btn btn-secondary")
            .attribute("role", "button");
        w.text("Bundle");
        w.close();
        w.close();

        // Integration snippets
        const auto schema_name{
            std::filesystem::path{meta.at("path").to_string()}
                .filename()
                .string()};
        const auto cli_snippet{"jsonschema install " + canonical + " schemas/" +
                               schema_name + ".json"};
        const auto openapi_snippet{R"($ref: ")" + canonical + R"(")"};
        const auto deno_snippet{R"(import schema from ")" + canonical +
                                R"(" with { type: "json" };)"};

        // Usage tab group
        w.div()
            .attribute("data-sourcemeta-ui-tab-group", "usage")
            .attribute("class",
                       "bg-light border rounded px-3 py-2 mt-4 d-flex "
                       "flex-wrap flex-md-nowrap align-items-center small "
                       "gap-2");

        w.span().attribute("class", "text-secondary fw-light text-nowrap");
        w.text("Use with");
        w.close();

        // Button group
        w.div()
            .attribute("class", "btn-group flex-shrink-0 me-2")
            .attribute("role", "group");
        w.button()
            .attribute("class", "btn btn-sm btn-outline-secondary")
            .attribute("type", "button")
            .attribute("data-sourcemeta-ui-tab-target", "usage-cli");
        w.text("CLI");
        w.close();
        w.button()
            .attribute("class", "btn btn-sm btn-outline-secondary")
            .attribute("type", "button")
            .attribute("data-sourcemeta-ui-tab-target", "usage-openapi");
        w.text("OpenAPI");
        w.close();
        w.button()
            .attribute("class", "btn btn-sm btn-outline-secondary")
            .attribute("type", "button")
            .attribute("data-sourcemeta-ui-tab-target", "usage-deno");
        w.text("Deno");
        w.close();
        w.close();

        // CLI tab
        w.div()
            .attribute("data-sourcemeta-ui-tab-id", "usage-cli")
            .attribute("class",
                       "d-none d-flex align-items-center flex-grow-1 gap-2")
            .attribute("style", "min-width: 0");
        w.code().attribute("class", "bg-white border p-2 font-monospace "
                                    "flex-grow-1 text-dark text-break");
        w.span("$ ");
        w.a()
            .attribute("href", "/integrations/#json-schema-cli")
            .attribute("target", "_blank")
            .attribute("class", "text-dark");
        w.text("jsonschema install");
        w.close();
        w.span(" " + canonical + " schemas/" + schema_name + ".json");
        w.close();
        w.button()
            .attribute("class", "btn btn-sm btn-outline-secondary")
            .attribute("type", "button")
            .attribute("data-sourcemeta-ui-copy", cli_snippet);
        w.i().attribute("class", "bi bi-clipboard").close();
        w.close();
        w.close();

        // OpenAPI tab
        w.div()
            .attribute("data-sourcemeta-ui-tab-id", "usage-openapi")
            .attribute("class",
                       "d-none d-flex align-items-center flex-grow-1 gap-2")
            .attribute("style", "min-width: 0");
        w.code().attribute("class", "bg-white border p-2 font-monospace "
                                    "flex-grow-1 text-dark text-break");
        w.text(openapi_snippet);
        w.close();
        w.button()
            .attribute("class", "btn btn-sm btn-outline-secondary")
            .attribute("type", "button")
            .attribute("data-sourcemeta-ui-copy", openapi_snippet);
        w.i().attribute("class", "bi bi-clipboard").close();
        w.close();
        w.close();

        // Deno tab
        w.div()
            .attribute("data-sourcemeta-ui-tab-id", "usage-deno")
            .attribute("class",
                       "d-none d-flex align-items-center flex-grow-1 gap-2")
            .attribute("style", "min-width: 0");
        w.code().attribute("class", "bg-white border p-2 font-monospace "
                                    "flex-grow-1 text-dark text-break");
        w.text(deno_snippet);
        w.close();
        w.button()
            .attribute("class", "btn btn-sm btn-outline-secondary")
            .attribute("type", "button")
            .attribute("data-sourcemeta-ui-copy", deno_snippet);
        w.i().attribute("class", "bi bi-clipboard").close();
        w.close();
        w.close();

        w.close();

        // Information table
        w.table().attribute("class", "table table-bordered my-4");

        // Identifier row
        w.tr();
        w.th().attribute("scope", "row").attribute("class", "text-nowrap");
        w.text("Identifier");
        w.close();
        w.td();
        w.code();
        w.a().attribute("href", meta.at("identifier").to_string());
        w.text(meta.at("identifier").to_string());
        w.close();
        w.close();
        w.close();
        w.close();

        // Base Dialect row
        w.tr();
        w.th().attribute("scope", "row").attribute("class", "text-nowrap");
        w.text("Base Dialect");
        w.close();
        w.td();
        html::make_dialect_badge(w, meta.at("baseDialect").to_string());
        w.close();
        w.close();

        // Dialect row
        w.tr();
        w.th().attribute("scope", "row").attribute("class", "text-nowrap");
        w.text("Dialect");
        w.close();
        w.td();
        w.code(meta.at("dialect").to_string());
        w.close();
        w.close();

        // Health row
        w.tr();
        w.th().attribute("scope", "row").attribute("class", "text-nowrap");
        w.text("Health");
        w.close();
        w.td().attribute("class", "align-middle");
        w.div().attribute("style", "max-width: 300px");
        html::make_schema_health_progress_bar(w,
                                              meta.at("health").to_integer());
        w.close();
        w.close();
        w.close();

        // Size row
        w.tr();
        w.th().attribute("scope", "row").attribute("class", "text-nowrap");
        w.text("Size");
        w.close();
        w.td(std::to_string(meta.at("bytes").as_real() / (1024 * 1024)) +
             " MB");
        w.close();

        w.close();

        // Empty div
        w.div().close();

        w.close();

        // Alert section
        if (meta.at("alert").is_string()) {
          w.div()
              .attribute("class", "alert alert-warning mb-3")
              .attribute("role", "alert");
          w.raw(meta.at("alert").to_string());
          w.close();
        }

        // Schema editor
        w.div()
            .attribute("id", "schema")
            .attribute("class", "border overflow-auto")
            .attribute("style", "max-height: 400px;")
            .attribute("data-sourcemeta-ui-editor", meta.at("path").to_string())
            .attribute("data-sourcemeta-ui-editor-mode", "readonly")
            .attribute("data-sourcemeta-ui-editor-language", "json");
        w.text("Loading schema...");
        w.close();

        // Details tab group
        w.div()
            .attribute("data-sourcemeta-ui-tab-group", "details")
            .attribute("data-sourcemeta-ui-tab-url-param", "tab");

        // Tab navigation
        w.ul().attribute("class", "nav nav-tabs mt-4 mb-3");

        // Examples tab button
        w.li().attribute("class", "nav-item");
        w.button()
            .attribute("class", "nav-link")
            .attribute("type", "button")
            .attribute("role", "tab")
            .attribute("data-sourcemeta-ui-tab-target", "examples");
        w.span("Examples");
        w.span().attribute(
            "class",
            "ms-2 badge rounded-pill text-bg-secondary align-text-top");
        w.text(std::to_string(meta.at("examples").size()));
        w.close();
        w.close();
        w.close();

        // Dependencies tab button
        w.li().attribute("class", "nav-item");
        w.button()
            .attribute("class", "nav-link")
            .attribute("type", "button")
            .attribute("role", "tab")
            .attribute("data-sourcemeta-ui-tab-target", "dependencies");
        w.span("Dependencies");
        w.span()
            .attribute(
                "class",
                "ms-2 badge rounded-pill text-bg-secondary align-text-top")
            .attribute("data-sourcemeta-ui-dependencies-count", "");
        w.text("...");
        w.close();
        w.close();
        w.close();

        // Dependents tab button
        w.li().attribute("class", "nav-item");
        w.button()
            .attribute("class", "nav-link")
            .attribute("type", "button")
            .attribute("role", "tab")
            .attribute("data-sourcemeta-ui-tab-target", "dependents");
        w.span("Dependents");
        w.span()
            .attribute(
                "class",
                "ms-2 badge rounded-pill text-bg-secondary align-text-top")
            .attribute("data-sourcemeta-ui-dependents-count", "");
        w.text("...");
        w.close();
        w.close();
        w.close();

        // Health tab button
        w.li().attribute("class", "nav-item");
        w.button()
            .attribute("class", "nav-link")
            .attribute("type", "button")
            .attribute("role", "tab")
            .attribute("data-sourcemeta-ui-tab-target", "health");
        w.span("Health");
        w.span().attribute(
            "class",
            "ms-2 badge rounded-pill text-bg-secondary align-text-top");
        w.text(std::to_string(health.at("errors").size()));
        w.close();
        w.close();
        w.close();

        w.close();

        // Examples tab content
        w.div()
            .attribute("data-sourcemeta-ui-tab-id", "examples")
            .attribute("class", "d-none");
        if (meta.at("examples").empty()) {
          w.p("This schema declares 0 examples.");
        } else {
          w.div().attribute("class", "list-group");
          for (const auto &example : meta.at("examples").as_array()) {
            std::ostringstream pretty;
            sourcemeta::core::prettify(example, pretty);
            w.pre().attribute("class", "bg-light p-2 border");
            w.code().attribute("class", "d-block text-primary");
            w.text(pretty.str());
            w.close();
            w.close();
          }
          w.close();
        }
        w.close();

        // Dependencies tab content
        w.div()
            .attribute("data-sourcemeta-ui-tab-id", "dependencies")
            .attribute("class", "d-none");
        w.div()
            .attribute("data-sourcemeta-ui-dependencies",
                       meta.at("path").to_string())
            .attribute("data-sourcemeta-ui-identifier", canonical);
        w.text("Loading...");
        w.close();
        w.close();

        // Dependents tab content
        w.div()
            .attribute("data-sourcemeta-ui-tab-id", "dependents")
            .attribute("class", "d-none");
        w.div()
            .attribute("data-sourcemeta-ui-dependents",
                       meta.at("path").to_string())
            .attribute("data-sourcemeta-ui-identifier", canonical);
        w.text("Loading...");
        w.close();
        w.close();

        // Health tab content
        const auto errors_count{health.at("errors").size()};
        w.div()
            .attribute("data-sourcemeta-ui-tab-id", "health")
            .attribute("class", "d-none");
        if (errors_count == 1) {
          w.p("This schema has " + std::to_string(errors_count) +
              " quality error.");
        } else {
          w.p("This schema has " + std::to_string(errors_count) +
              " quality errors.");
        }

        if (errors_count > 0) {
          w.div().attribute("class", "list-group");
          for (const auto &error : health.at("errors").as_array()) {
            assert(error.at("pointers").size() >= 1);
            std::ostringstream pointers;
            sourcemeta::core::stringify(error.at("pointers"), pointers);

            w.a()
                .attribute("href", "#")
                .attribute("data-sourcemeta-ui-editor-highlight",
                           meta.at("path").to_string())
                .attribute("data-sourcemeta-ui-editor-highlight-pointers",
                           pointers.str())
                .attribute("class",
                           "list-group-item list-group-item-action py-3");

            w.code().attribute("class", "d-block text-primary");
            w.text(error.at("pointers").front().to_string());
            w.close();

            if (error.at("custom").to_boolean()) {
              w.small().attribute("class", "d-block text-body-secondary");
              w.span().attribute("class", "badge text-bg-info me-1");
              w.text("Custom");
              w.close();
              w.span(error.at("name").to_string());
              w.close();
            } else {
              w.small().attribute("class", "d-block text-body-secondary");
              w.text(error.at("name").to_string());
              w.close();
            }

            w.p().attribute("class", "mb-0 mt-2");
            w.text(error.at("message").to_string());
            w.close();

            if (error.at("description").is_string()) {
              w.small().attribute("class", "mt-2");
              w.text(error.at("description").to_string());
              w.close();
            }

            w.close();
          }
          w.close();
        }
        w.close();

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
