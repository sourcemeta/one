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
#include <vector>     // std::vector

namespace sourcemeta::one {

auto GENERATE_WEB_SCHEMA::handler(
    const sourcemeta::one::BuildState &,
    const sourcemeta::one::BuildPlan::Action &action,
    const sourcemeta::one::BuildDynamicCallback &, sourcemeta::one::Resolver &,
    const sourcemeta::one::Configuration &configuration,
    const sourcemeta::core::JSON &) -> bool {
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

  using namespace sourcemeta::core::html;

  std::vector<sourcemeta::core::HTMLNode> container_children;
  std::vector<sourcemeta::core::HTMLNode> content_children;
  std::vector<sourcemeta::core::HTMLNode> header_children;

  // Title and description
  if (meta.defines("title")) {
    header_children.emplace_back(h2({{"class", "fw-bold h4"}}, title));
  }

  if (meta.defines("description")) {
    header_children.emplace_back(
        p({{"class", "text-secondary"}}, meta.at("description").to_string()));
  }

  // Action buttons
  header_children.emplace_back(
      a({{"href", meta.at("path").to_string() + ".json"},
         {"class", "btn btn-primary me-2"},
         {"role", "button"}},
        "Get JSON Schema"));
  header_children.emplace_back(
      a({{"href", meta.at("path").to_string() + ".json?bundle=1"},
         {"class", "btn btn-secondary"},
         {"role", "button"}},
        "Bundle"));

  content_children.emplace_back(div(header_children));

  // Integration snippets
  const auto schema_name{
      std::filesystem::path{meta.at("path").to_string()}.filename().string()};
  const auto cli_snippet{"jsonschema install " + canonical + " schemas/" +
                         schema_name + ".json"};
  const auto openapi_snippet{R"($ref: ")" + canonical + R"(")"};
  const auto deno_snippet{R"(import schema from ")" + canonical +
                          R"(" with { type: "json" };)"};

  std::vector<sourcemeta::core::HTMLNode> usage_buttons;
  usage_buttons.emplace_back(
      button({{"class", "btn btn-sm btn-outline-secondary"},
              {"type", "button"},
              {"data-sourcemeta-ui-tab-target", "usage-cli"}},
             "CLI"));
  usage_buttons.emplace_back(
      button({{"class", "btn btn-sm btn-outline-secondary"},
              {"type", "button"},
              {"data-sourcemeta-ui-tab-target", "usage-openapi"}},
             "OpenAPI"));
  usage_buttons.emplace_back(
      button({{"class", "btn btn-sm btn-outline-secondary"},
              {"type", "button"},
              {"data-sourcemeta-ui-tab-target", "usage-deno"}},
             "Deno"));

  std::vector<sourcemeta::core::HTMLNode> usage_row;
  usage_row.emplace_back(
      span({{"class", "text-secondary fw-light text-nowrap"}}, "Use with"));
  usage_row.emplace_back(
      div({{"class", "btn-group flex-shrink-0 me-2"}, {"role", "group"}},
          usage_buttons));
  usage_row.emplace_back(
      div({{"data-sourcemeta-ui-tab-id", "usage-cli"},
           {"class", "d-none d-flex align-items-center flex-grow-1 gap-2"},
           {"style", "min-width: 0"}},
          code({{"class", "bg-white border p-2 font-monospace flex-grow-1 "
                          "text-dark text-break"}},
               span("$ "),
               a({{"href", "/integrations/#json-schema-cli"},
                  {"target", "_blank"},
                  {"class", "text-dark"}},
                 "jsonschema install"),
               span(" " + canonical + " schemas/" + schema_name + ".json")),
          button({{"class", "btn btn-sm btn-outline-secondary"},
                  {"type", "button"},
                  {"data-sourcemeta-ui-copy", cli_snippet}},
                 i({{"class", "bi bi-clipboard"}}))));
  usage_row.emplace_back(
      div({{"data-sourcemeta-ui-tab-id", "usage-openapi"},
           {"class", "d-none d-flex align-items-center flex-grow-1 gap-2"},
           {"style", "min-width: 0"}},
          code({{"class", "bg-white border p-2 font-monospace flex-grow-1 "
                          "text-dark text-break"}},
               openapi_snippet),
          button({{"class", "btn btn-sm btn-outline-secondary"},
                  {"type", "button"},
                  {"data-sourcemeta-ui-copy", openapi_snippet}},
                 i({{"class", "bi bi-clipboard"}}))));
  usage_row.emplace_back(
      div({{"data-sourcemeta-ui-tab-id", "usage-deno"},
           {"class", "d-none d-flex align-items-center flex-grow-1 gap-2"},
           {"style", "min-width: 0"}},
          code({{"class", "bg-white border p-2 font-monospace flex-grow-1 "
                          "text-dark text-break"}},
               deno_snippet),
          button({{"class", "btn btn-sm btn-outline-secondary"},
                  {"type", "button"},
                  {"data-sourcemeta-ui-copy", deno_snippet}},
                 i({{"class", "bi bi-clipboard"}}))));

  content_children.emplace_back(
      div({{"data-sourcemeta-ui-tab-group", "usage"},
           {"class", "bg-light border rounded px-3 py-2 mt-4 d-flex "
                     "flex-wrap flex-md-nowrap align-items-center small "
                     "gap-2"}},
          usage_row));

  // Information table
  std::vector<sourcemeta::core::HTMLNode> table_rows;

  // Identifier row
  table_rows.emplace_back(
      tr(th({{"scope", "row"}, {"class", "text-nowrap"}}, "Identifier"),
         td(code(a({{"href", meta.at("identifier").to_string()}},
                   meta.at("identifier").to_string())))));

  // Base Dialect row
  std::ostringstream badge_stream;
  badge_stream << html::make_dialect_badge(meta.at("baseDialect").to_string());
  table_rows.emplace_back(
      tr(th({{"scope", "row"}, {"class", "text-nowrap"}}, "Base Dialect"),
         td(raw(badge_stream.str()))));

  // Dialect row
  table_rows.emplace_back(
      tr(th({{"scope", "row"}, {"class", "text-nowrap"}}, "Dialect"),
         td(code(meta.at("dialect").to_string()))));

  // Health row
  table_rows.emplace_back(
      tr(th({{"scope", "row"}, {"class", "text-nowrap"}}, "Health"),
         td({{"class", "align-middle"}},
            div({{"style", "max-width: 300px"}},
                html::make_schema_health_progress_bar(
                    meta.at("health").to_integer())))));

  // Size row
  table_rows.emplace_back(tr(
      th({{"scope", "row"}, {"class", "text-nowrap"}}, "Size"),
      td(std::to_string(meta.at("bytes").as_real() / (1024 * 1024)) + " MB")));

  content_children.emplace_back(
      table({{"class", "table table-bordered my-4"}}, table_rows));
  content_children.emplace_back(div({})); // Close div for content

  container_children.emplace_back(div(content_children));

  // Alert section
  if (meta.at("alert").is_string()) {
    container_children.emplace_back(
        div({{"class", "alert alert-warning mb-3"}, {"role", "alert"}},
            raw(meta.at("alert").to_string())));
  }

  container_children.emplace_back(
      div({{"id", "schema"},
           {"class", "border overflow-auto"},
           {"style", "max-height: 400px;"},
           {"data-sourcemeta-ui-editor", meta.at("path").to_string()},
           {"data-sourcemeta-ui-editor-mode", "readonly"},
           {"data-sourcemeta-ui-editor-language", "json"}},
          "Loading schema..."));

  const auto health_option{metapack_read_json(action.dependencies.at(1))};
  assert(health_option.has_value());
  const auto &health{health_option.value()};
  assert(health.is_object());
  assert(health.defines("errors"));

  std::vector<sourcemeta::core::HTMLNode> details_children;

  // Tab navigation
  std::vector<sourcemeta::core::HTMLNode> nav_items;
  nav_items.emplace_back(li(
      {{"class", "nav-item"}},
      button(
          {{"class", "nav-link"},
           {"type", "button"},
           {"role", "tab"},
           {"data-sourcemeta-ui-tab-target", "examples"}},
          span("Examples"),
          span({{"class",
                 "ms-2 badge rounded-pill text-bg-secondary align-text-top"}},
               std::to_string(meta.at("examples").size())))));
  nav_items.emplace_back(li(
      {{"class", "nav-item"}},
      button({{"class", "nav-link"},
              {"type", "button"},
              {"role", "tab"},
              {"data-sourcemeta-ui-tab-target", "dependencies"}},
             span("Dependencies"),
             span({{"class",
                    "ms-2 badge rounded-pill text-bg-secondary align-text-top"},
                   {"data-sourcemeta-ui-dependencies-count", ""}},
                  "..."))));
  nav_items.emplace_back(li(
      {{"class", "nav-item"}},
      button({{"class", "nav-link"},
              {"type", "button"},
              {"role", "tab"},
              {"data-sourcemeta-ui-tab-target", "dependents"}},
             span("Dependents"),
             span({{"class",
                    "ms-2 badge rounded-pill text-bg-secondary align-text-top"},
                   {"data-sourcemeta-ui-dependents-count", ""}},
                  "..."))));
  nav_items.emplace_back(li(
      {{"class", "nav-item"}},
      button(
          {{"class", "nav-link"},
           {"type", "button"},
           {"role", "tab"},
           {"data-sourcemeta-ui-tab-target", "health"}},
          span("Health"),
          span({{"class",
                 "ms-2 badge rounded-pill text-bg-secondary align-text-top"}},
               std::to_string(health.at("errors").size())))));

  details_children.emplace_back(
      ul({{"class", "nav nav-tabs mt-4 mb-3"}}, nav_items));

  // Examples tab
  std::vector<sourcemeta::core::HTMLNode> examples_content;
  if (meta.at("examples").empty()) {
    examples_content.emplace_back(p("This schema declares 0 examples."));
  } else {
    std::vector<sourcemeta::core::HTMLNode> example_items;
    for (const auto &example : meta.at("examples").as_array()) {
      std::ostringstream pretty;
      sourcemeta::core::prettify(example, pretty);
      example_items.emplace_back(
          pre({{"class", "bg-light p-2 border"}},
              code({{"class", "d-block text-primary"}}, pretty.str())));
    }
    examples_content.emplace_back(
        div({{"class", "list-group"}}, example_items));
  }
  details_children.emplace_back(
      div({{"data-sourcemeta-ui-tab-id", "examples"}, {"class", "d-none"}},
          examples_content));

  details_children.emplace_back(
      div({{"data-sourcemeta-ui-tab-id", "dependencies"}, {"class", "d-none"}},
          div({{"data-sourcemeta-ui-dependencies", meta.at("path").to_string()},
               {"data-sourcemeta-ui-identifier", canonical}},
              "Loading...")));
  details_children.emplace_back(
      div({{"data-sourcemeta-ui-tab-id", "dependents"}, {"class", "d-none"}},
          div({{"data-sourcemeta-ui-dependents", meta.at("path").to_string()},
               {"data-sourcemeta-ui-identifier", canonical}},
              "Loading...")));

  // Health tab
  const auto errors_count{health.at("errors").size()};
  std::vector<sourcemeta::core::HTMLNode> health_content;
  if (errors_count == 1) {
    health_content.emplace_back(p(
        "This schema has " + std::to_string(errors_count) + " quality error."));
  } else {
    health_content.emplace_back(p("This schema has " +
                                  std::to_string(errors_count) +
                                  " quality errors."));
  }

  if (errors_count > 0) {
    std::vector<sourcemeta::core::HTMLNode> error_items;
    for (const auto &error : health.at("errors").as_array()) {
      assert(error.at("pointers").size() >= 1);
      std::ostringstream pointers;
      sourcemeta::core::stringify(error.at("pointers"), pointers);

      std::vector<sourcemeta::core::HTMLNode> error_children;
      error_children.emplace_back(
          code({{"class", "d-block text-primary"}},
               error.at("pointers").front().to_string()));
      if (error.at("custom").to_boolean()) {
        error_children.emplace_back(
            small({{"class", "d-block text-body-secondary"}},
                  span({{"class", "badge text-bg-info me-1"}}, "Custom"),
                  span(error.at("name").to_string())));
      } else {
        error_children.emplace_back(
            small({{"class", "d-block text-body-secondary"}},
                  error.at("name").to_string()));
      }
      error_children.emplace_back(
          p({{"class", "mb-0 mt-2"}}, error.at("message").to_string()));
      if (error.at("description").is_string()) {
        error_children.emplace_back(
            small({{"class", "mt-2"}}, error.at("description").to_string()));
      }

      error_items.emplace_back(a(
          {{"href", "#"},
           {"data-sourcemeta-ui-editor-highlight", meta.at("path").to_string()},
           {"data-sourcemeta-ui-editor-highlight-pointers", pointers.str()},
           {"class", "list-group-item list-group-item-action py-3"}},
          error_children));
    }
    health_content.emplace_back(div({{"class", "list-group"}}, error_items));
  }
  details_children.emplace_back(
      div({{"data-sourcemeta-ui-tab-id", "health"}, {"class", "d-none"}},
          health_content));

  container_children.emplace_back(
      div({{"data-sourcemeta-ui-tab-group", "details"},
           {"data-sourcemeta-ui-tab-url-param", "tab"}},
          details_children));

  std::ostringstream html_content;
  html_content << "<!DOCTYPE html>"
               << html::make_page(configuration, canonical, title, description,
                                  html::make_breadcrumb(meta.at("breadcrumb")),
                                  html::div({{"class", "container-fluid p-4"}},
                                            container_children));
  const auto timestamp_end{std::chrono::steady_clock::now()};

  metapack_write_text(action.destination, html_content.str(), "text/html",
                      MetapackEncoding::GZIP, {},
                      std::chrono::duration_cast<std::chrono::milliseconds>(
                          timestamp_end - timestamp_start));
  return true;
}

} // namespace sourcemeta::one
