#include <sourcemeta/one/web.h>

#include "../helpers.h"
#include "../page.h"

#include <sourcemeta/core/html.h>
#include <sourcemeta/core/markdown.h>
#include <sourcemeta/one/metapack.h>
#include <sourcemeta/one/shared.h>

#include <cassert>    // assert
#include <chrono>     // std::chrono
#include <filesystem> // std::filesystem
#include <sstream>    // std::ostringstream

namespace sourcemeta::one {

auto GenerateWebSchema::handler(
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
      writer, configuration, action.view, canonical, title, description,
      [&](sourcemeta::core::HTMLWriter &body) -> void {
        html::make_breadcrumb(body, meta.at("breadcrumb"));

        body.div().attribute("class", "container-fluid p-4");

        // Content wrapper div
        body.div();

        // Header div
        body.div();
        if (meta.at("private").to_boolean()) {
          body.div().attribute("class", "mb-3");
          html::make_private_badge(body);
          body.close();
        }
        if (meta.defines("title")) {
          body.h2().attribute("class", "fw-bold h4");
          body.text(title);
          body.close();
        }
        if (meta.defines("description")) {
          body.div().attribute("class", "text-secondary");
          body.raw(sourcemeta::core::markdown_to_html(
              meta.at("description").to_string()));
          body.close();
        }
        body.a()
            .attribute("href", meta.at("path").to_string() + ".json")
            .attribute("class", "btn btn-primary me-2")
            .attribute("role", "button");
        body.text("Get JSON Schema");
        body.close();
        body.a()
            .attribute("href", meta.at("path").to_string() + ".json?bundle=1")
            .attribute("class", "btn btn-secondary")
            .attribute("role", "button");
        body.text("Bundle");
        body.close();
        body.close();

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
        body.div()
            .attribute("data-sourcemeta-ui-tab-group", "usage")
            .attribute("class",
                       "bg-light border rounded px-3 py-2 mt-4 d-flex "
                       "flex-wrap flex-md-nowrap align-items-center small "
                       "gap-2");

        body.span().attribute("class", "text-secondary fw-light text-nowrap");
        body.text("Use with");
        body.close();

        // Button group
        body.div()
            .attribute("class", "btn-group flex-shrink-0 me-2")
            .attribute("role", "group");
        body.button()
            .attribute("class", "btn btn-sm btn-outline-secondary")
            .attribute("type", "button")
            .attribute("data-sourcemeta-ui-tab-target", "usage-cli");
        body.text("CLI");
        body.close();
        body.button()
            .attribute("class", "btn btn-sm btn-outline-secondary")
            .attribute("type", "button")
            .attribute("data-sourcemeta-ui-tab-target", "usage-openapi");
        body.text("OpenAPI");
        body.close();
        body.button()
            .attribute("class", "btn btn-sm btn-outline-secondary")
            .attribute("type", "button")
            .attribute("data-sourcemeta-ui-tab-target", "usage-deno");
        body.text("Deno");
        body.close();
        body.close();

        // CLI tab
        body.div()
            .attribute("data-sourcemeta-ui-tab-id", "usage-cli")
            .attribute("class",
                       "d-none d-flex align-items-center flex-grow-1 gap-2")
            .attribute("style", "min-width: 0");
        body.code().attribute("class", "bg-white border p-2 font-monospace "
                                       "flex-grow-1 text-dark text-break");
        body.span("$ ");
        body.a()
            .attribute("href", "/integrations/#json-schema-cli")
            .attribute("target", "_blank")
            .attribute("class", "text-dark");
        body.text("jsonschema install");
        body.close();
        body.span(" " + canonical + " schemas/" + schema_name + ".json");
        body.close();
        body.button()
            .attribute("class", "btn btn-sm btn-outline-secondary")
            .attribute("type", "button")
            .attribute("data-sourcemeta-ui-copy", cli_snippet);
        body.i().attribute("class", "bi bi-clipboard").close();
        body.close();
        body.close();

        // OpenAPI tab
        body.div()
            .attribute("data-sourcemeta-ui-tab-id", "usage-openapi")
            .attribute("class",
                       "d-none d-flex align-items-center flex-grow-1 gap-2")
            .attribute("style", "min-width: 0");
        body.code().attribute("class", "bg-white border p-2 font-monospace "
                                       "flex-grow-1 text-dark text-break");
        body.text(openapi_snippet);
        body.close();
        body.button()
            .attribute("class", "btn btn-sm btn-outline-secondary")
            .attribute("type", "button")
            .attribute("data-sourcemeta-ui-copy", openapi_snippet);
        body.i().attribute("class", "bi bi-clipboard").close();
        body.close();
        body.close();

        // Deno tab
        body.div()
            .attribute("data-sourcemeta-ui-tab-id", "usage-deno")
            .attribute("class",
                       "d-none d-flex align-items-center flex-grow-1 gap-2")
            .attribute("style", "min-width: 0");
        body.code().attribute("class", "bg-white border p-2 font-monospace "
                                       "flex-grow-1 text-dark text-break");
        body.text(deno_snippet);
        body.close();
        body.button()
            .attribute("class", "btn btn-sm btn-outline-secondary")
            .attribute("type", "button")
            .attribute("data-sourcemeta-ui-copy", deno_snippet);
        body.i().attribute("class", "bi bi-clipboard").close();
        body.close();
        body.close();

        body.close();

        // Information table
        body.table().attribute("class", "table table-bordered my-4");

        // Identifier row
        body.tr();
        body.th().attribute("scope", "row").attribute("class", "text-nowrap");
        body.text("Identifier");
        body.close();
        body.td();
        body.code();
        body.a().attribute("href", meta.at("identifier").to_string());
        body.text(meta.at("identifier").to_string());
        body.close();
        body.close();
        body.close();
        body.close();

        // Base Dialect row
        body.tr();
        body.th().attribute("scope", "row").attribute("class", "text-nowrap");
        body.text("Base Dialect");
        body.close();
        body.td();
        html::make_dialect_badge(body, meta.at("baseDialect").to_string());
        body.close();
        body.close();

        // Dialect row
        body.tr();
        body.th().attribute("scope", "row").attribute("class", "text-nowrap");
        body.text("Dialect");
        body.close();
        body.td();
        body.code(meta.at("dialect").to_string());
        body.close();
        body.close();

        // Health row
        body.tr();
        body.th().attribute("scope", "row").attribute("class", "text-nowrap");
        body.text("Health");
        body.close();
        body.td().attribute("class", "align-middle");
        body.div().attribute("style", "max-width: 300px");
        html::make_schema_health_progress_bar(body,
                                              meta.at("health").to_integer());
        body.close();
        body.close();
        body.close();

        // Size row
        body.tr();
        body.th().attribute("scope", "row").attribute("class", "text-nowrap");
        body.text("Size");
        body.close();
        body.td(std::to_string(meta.at("bytes").as_real() / (1024 * 1024)) +
                " MB");
        body.close();

        body.close();

        // Empty div
        body.div().close();

        body.close();

        // Alert section
        if (meta.at("alert").is_string()) {
          body.div()
              .attribute("class", "alert alert-warning mb-3")
              .attribute("role", "alert");
          body.raw(
              sourcemeta::core::markdown_to_html(meta.at("alert").to_string()));
          body.close();
        }

        // Schema editor
        body.div()
            .attribute("id", "schema")
            .attribute("class", "border overflow-auto")
            .attribute("style", "max-height: 400px;")
            .attribute("data-sourcemeta-ui-editor", meta.at("path").to_string())
            .attribute("data-sourcemeta-ui-editor-mode", "readonly")
            .attribute("data-sourcemeta-ui-editor-language", "json");
        body.text("Loading schema...");
        body.close();

        // Details tab group
        body.div()
            .attribute("data-sourcemeta-ui-tab-group", "details")
            .attribute("data-sourcemeta-ui-tab-url-param", "tab");

        // Tab navigation
        body.ul().attribute("class", "nav nav-tabs mt-4 mb-3");

        // Examples tab button
        body.li().attribute("class", "nav-item");
        body.button()
            .attribute("class", "nav-link")
            .attribute("type", "button")
            .attribute("role", "tab")
            .attribute("data-sourcemeta-ui-tab-target", "examples");
        body.span("Examples");
        body.span().attribute(
            "class",
            "ms-2 badge rounded-pill text-bg-secondary align-text-top");
        body.text(std::to_string(meta.at("examples").size()));
        body.close();
        body.close();
        body.close();

        // Dependencies tab button
        body.li().attribute("class", "nav-item");
        body.button()
            .attribute("class", "nav-link")
            .attribute("type", "button")
            .attribute("role", "tab")
            .attribute("data-sourcemeta-ui-tab-target", "dependencies");
        body.span("Dependencies");
        body.span()
            .attribute(
                "class",
                "ms-2 badge rounded-pill text-bg-secondary align-text-top")
            .attribute("data-sourcemeta-ui-dependencies-count", "");
        body.text("...");
        body.close();
        body.close();
        body.close();

        // Dependents tab button
        body.li().attribute("class", "nav-item");
        body.button()
            .attribute("class", "nav-link")
            .attribute("type", "button")
            .attribute("role", "tab")
            .attribute("data-sourcemeta-ui-tab-target", "dependents");
        body.span("Dependents");
        body.span()
            .attribute(
                "class",
                "ms-2 badge rounded-pill text-bg-secondary align-text-top")
            .attribute("data-sourcemeta-ui-dependents-count", "");
        body.text("...");
        body.close();
        body.close();
        body.close();

        // Health tab button
        body.li().attribute("class", "nav-item");
        body.button()
            .attribute("class", "nav-link")
            .attribute("type", "button")
            .attribute("role", "tab")
            .attribute("data-sourcemeta-ui-tab-target", "health");
        body.span("Health");
        body.span().attribute(
            "class",
            "ms-2 badge rounded-pill text-bg-secondary align-text-top");
        body.text(std::to_string(health.at("errors").size()));
        body.close();
        body.close();
        body.close();

        body.close();

        // Examples tab content
        body.div()
            .attribute("data-sourcemeta-ui-tab-id", "examples")
            .attribute("class", "d-none");
        if (meta.at("examples").empty()) {
          body.p("This schema declares 0 examples.");
        } else {
          body.div().attribute("class", "list-group");
          for (const auto &example : meta.at("examples").as_array()) {
            std::ostringstream pretty;
            sourcemeta::core::prettify(example, pretty);
            body.pre().attribute("class", "bg-light p-2 border");
            body.code().attribute("class", "d-block text-primary");
            body.text(pretty.str());
            body.close();
            body.close();
          }
          body.close();
        }
        body.close();

        // Dependencies tab content
        body.div()
            .attribute("data-sourcemeta-ui-tab-id", "dependencies")
            .attribute("class", "d-none");
        body.div()
            .attribute("data-sourcemeta-ui-dependencies",
                       meta.at("path").to_string())
            .attribute("data-sourcemeta-ui-identifier", canonical);
        body.text("Loading...");
        body.close();
        body.close();

        // Dependents tab content
        body.div()
            .attribute("data-sourcemeta-ui-tab-id", "dependents")
            .attribute("class", "d-none");
        body.div()
            .attribute("data-sourcemeta-ui-dependents",
                       meta.at("path").to_string())
            .attribute("data-sourcemeta-ui-identifier", canonical);
        body.text("Loading...");
        body.close();
        body.close();

        // Health tab content
        const auto errors_count{health.at("errors").size()};
        body.div()
            .attribute("data-sourcemeta-ui-tab-id", "health")
            .attribute("class", "d-none");
        if (errors_count == 1) {
          body.p("This schema has " + std::to_string(errors_count) +
                 " quality error.");
        } else {
          body.p("This schema has " + std::to_string(errors_count) +
                 " quality errors.");
        }

        if (!health.at("errors").empty()) {
          body.div().attribute("class", "list-group");
          for (const auto &error : health.at("errors").as_array()) {
            assert(!error.at("pointers").empty());
            std::ostringstream pointers;
            sourcemeta::core::stringify(error.at("pointers"), pointers);

            body.a()
                .attribute("href", "#")
                .attribute("data-sourcemeta-ui-editor-highlight",
                           meta.at("path").to_string())
                .attribute("data-sourcemeta-ui-editor-highlight-pointers",
                           pointers.str())
                .attribute("class",
                           "list-group-item list-group-item-action py-3");

            body.code().attribute("class", "d-block text-primary");
            body.text(error.at("pointers").front().to_string());
            body.close();

            if (error.at("custom").to_boolean()) {
              body.small().attribute("class", "d-block text-body-secondary");
              body.span().attribute("class", "badge text-bg-info me-1");
              body.text("Custom");
              body.close();
              body.span(error.at("name").to_string());
              body.close();
            } else {
              body.small().attribute("class", "d-block text-body-secondary");
              body.text(error.at("name").to_string());
              body.close();
            }

            body.div().attribute("class", "mb-0 mt-2");
            body.raw(sourcemeta::core::markdown_to_html(
                error.at("message").to_string()));
            body.close();

            if (error.at("description").is_string()) {
              body.div().attribute("class", "small mt-2");
              body.raw(sourcemeta::core::markdown_to_html(
                  error.at("description").to_string()));
              body.close();
            }

            body.close();
          }
          body.close();
        }
        body.close();

        body.close();

        body.close();
      });

  const auto timestamp_end{std::chrono::steady_clock::now()};

  metapack_write_text(action.destination, writer.str(),
                      "text/html; charset=utf-8", MetapackEncoding::GZIP, {},
                      std::chrono::duration_cast<std::chrono::milliseconds>(
                          timestamp_end - timestamp_start));
}

} // namespace sourcemeta::one
