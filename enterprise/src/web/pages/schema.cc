#include <sourcemeta/one/web.h>

#include "../helpers.h"
#include "../page.h"

#include <sourcemeta/core/html.h>
#include <sourcemeta/one/shared.h>

#include <cassert>    // assert
#include <chrono>     // std::chrono
#include <filesystem> // std::filesystem
#include <functional> // std::reference_wrapper
#include <set>        // std::set
#include <sstream>    // std::ostringstream
#include <vector>     // std::vector

namespace sourcemeta::one {

auto GENERATE_WEB_SCHEMA::handler(
    const std::filesystem::path &destination,
    const sourcemeta::core::BuildDependencies<std::filesystem::path>
        &dependencies,
    const sourcemeta::core::BuildDynamicCallback<std::filesystem::path> &,
    const Context &configuration) -> void {
  const auto timestamp_start{std::chrono::steady_clock::now()};

  const auto meta{read_json(dependencies.front())};
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

  const auto dependencies_json{read_json(dependencies.at(1))};
  const auto health{read_json(dependencies.at(2))};
  assert(health.is_object());
  assert(health.defines("errors"));
  const auto dependents_json{read_json(dependencies.at(3))};
  assert(dependents_json.is_array());

  // Collect unique dependent schemas, preferring direct over indirect.
  // The dependency tree already takes care of self-references, so
  // a schema should never appear as its own dependent
  std::set<sourcemeta::core::JSON::String> direct_dependent_schemas;
  std::set<sourcemeta::core::JSON::String> indirect_dependent_schemas;
  for (const auto &dependent : dependents_json.as_array()) {
    assert(dependent.at("from") != dependent.at("to"));
    if (dependent.at("to") == meta.at("identifier")) {
      direct_dependent_schemas.emplace(dependent.at("from").to_string());
    } else {
      indirect_dependent_schemas.emplace(dependent.at("from").to_string());
    }
  }

  // If a schema is a direct dependent, don't also show it as indirect
  for (const auto &schema : direct_dependent_schemas) {
    indirect_dependent_schemas.erase(schema);
  }

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
      button(
          {{"class", "nav-link"},
           {"type", "button"},
           {"role", "tab"},
           {"data-sourcemeta-ui-tab-target", "dependencies"}},
          span("Dependencies"),
          span({{"class",
                 "ms-2 badge rounded-pill text-bg-secondary align-text-top"}},
               std::to_string(dependencies_json.size())))));
  nav_items.emplace_back(li(
      {{"class", "nav-item"}},
      button(
          {{"class", "nav-link"},
           {"type", "button"},
           {"role", "tab"},
           {"data-sourcemeta-ui-tab-target", "dependents"}},
          span("Dependents"),
          span({{"class",
                 "ms-2 badge rounded-pill text-bg-secondary align-text-top"}},
               std::to_string(direct_dependent_schemas.size() +
                              indirect_dependent_schemas.size())))));
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

  container_children.emplace_back(
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
  container_children.emplace_back(
      div({{"data-sourcemeta-ui-tab-id", "examples"}, {"class", "d-none"}},
          examples_content));

  // Dependencies tab
  std::vector<std::reference_wrapper<const sourcemeta::core::JSON>> direct;
  std::vector<std::reference_wrapper<const sourcemeta::core::JSON>> indirect;
  for (const auto &dependency : dependencies_json.as_array()) {
    if (dependency.at("from") == meta.at("identifier")) {
      direct.emplace_back(dependency);
    } else {
      indirect.emplace_back(dependency);
    }
  }
  std::ostringstream dependency_summary;
  dependency_summary << "This schema has " << direct.size() << " direct "
                     << (direct.size() == 1 ? "dependency" : "dependencies")
                     << " and " << indirect.size() << " indirect "
                     << (indirect.size() == 1 ? "dependency" : "dependencies")
                     << ".";

  std::vector<sourcemeta::core::HTMLNode> dependencies_content;
  dependencies_content.emplace_back(p(dependency_summary.str()));

  if (direct.size() + indirect.size() > 0) {
    std::vector<sourcemeta::core::HTMLNode> dep_table_rows;
    for (const auto &dependency : dependencies_json.as_array()) {
      std::vector<sourcemeta::core::HTMLNode> row_cells;

      if (dependency.at("from") == meta.at("identifier")) {
        std::ostringstream dependency_attribute;
        sourcemeta::core::stringify(dependency.at("at"), dependency_attribute);
        row_cells.emplace_back(
            td(a({{"href", "#"},
                  {"data-sourcemeta-ui-editor-highlight",
                   meta.at("path").to_string()},
                  {"data-sourcemeta-ui-editor-highlight-pointers",
                   dependency_attribute.str()}},
                 code(dependency.at("at").to_string()))));
      } else {
        row_cells.emplace_back(
            td(span({{"class", "badge text-bg-dark"}}, "Indirect")));
      }

      if (dependency.at("to").to_string().starts_with(configuration.url)) {
        std::filesystem::path dependency_schema_url{
            dependency.at("to").to_string().substr(configuration.url.size())};
        if (dependency_schema_url.extension() == ".json") {
          dependency_schema_url.replace_extension("");
        }
        row_cells.emplace_back(
            td(code(a({{"href", dependency_schema_url.string()}},
                      dependency_schema_url.string()))));
      } else {
        row_cells.emplace_back(td(code(dependency.at("to").to_string())));
      }

      dep_table_rows.emplace_back(tr(row_cells));
    }

    dependencies_content.emplace_back(
        table({{"class", "table table-bordered"}},
              thead(tr(th({{"scope", "col"}}, "Origin"),
                       th({{"scope", "col"}}, "Dependency"))),
              tbody(dep_table_rows)));
  }
  container_children.emplace_back(
      div({{"data-sourcemeta-ui-tab-id", "dependencies"}, {"class", "d-none"}},
          dependencies_content));

  // Dependents tab
  std::ostringstream dependent_summary;
  dependent_summary
      << "This schema has " << direct_dependent_schemas.size() << " direct "
      << (direct_dependent_schemas.size() == 1 ? "dependent" : "dependents")
      << " and " << indirect_dependent_schemas.size() << " indirect "
      << (indirect_dependent_schemas.size() == 1 ? "dependent" : "dependents")
      << ".";

  std::vector<sourcemeta::core::HTMLNode> dependents_content;
  dependents_content.emplace_back(p(dependent_summary.str()));

  if (!direct_dependent_schemas.empty() ||
      !indirect_dependent_schemas.empty()) {
    std::vector<sourcemeta::core::HTMLNode> dep_tab_rows;

    const auto render_dependent_row{
        [&dep_tab_rows,
         &configuration](const sourcemeta::core::JSON::String &schema,
                         const bool is_direct) -> void {
          std::vector<sourcemeta::core::HTMLNode> row_cells;
          if (is_direct) {
            row_cells.emplace_back(
                td(span({{"class", "badge text-bg-primary"}}, "Direct")));
          } else {
            row_cells.emplace_back(
                td(span({{"class", "badge text-bg-dark"}}, "Indirect")));
          }

          if (schema.starts_with(configuration.url)) {
            std::filesystem::path dependent_schema_url{
                schema.substr(configuration.url.size())};
            if (dependent_schema_url.extension() == ".json") {
              dependent_schema_url.replace_extension("");
            }
            row_cells.emplace_back(
                td(code(a({{"href", dependent_schema_url.string()}},
                          dependent_schema_url.string()))));
          } else {
            row_cells.emplace_back(td(code(schema)));
          }

          dep_tab_rows.emplace_back(tr(row_cells));
        }};

    for (const auto &schema : direct_dependent_schemas) {
      render_dependent_row(schema, true);
    }

    for (const auto &schema : indirect_dependent_schemas) {
      render_dependent_row(schema, false);
    }

    dependents_content.emplace_back(
        table({{"class", "table table-bordered"}},
              thead(tr(th({{"scope", "col"}}, "Type"),
                       th({{"scope", "col"}}, "Dependent"))),
              tbody(dep_tab_rows)));
  }
  container_children.emplace_back(
      div({{"data-sourcemeta-ui-tab-id", "dependents"}, {"class", "d-none"}},
          dependents_content));

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
      error_children.emplace_back(
          small({{"class", "d-block text-body-secondary"}},
                error.at("name").to_string()));
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
  container_children.emplace_back(
      div({{"data-sourcemeta-ui-tab-id", "health"}, {"class", "d-none"}},
          health_content));

  std::ostringstream html_content;
  html_content << "<!DOCTYPE html>"
               << html::make_page(configuration, canonical, title, description,
                                  html::make_breadcrumb(meta.at("breadcrumb")),
                                  html::div({{"class", "container-fluid p-4"}},
                                            container_children));
  const auto timestamp_end{std::chrono::steady_clock::now()};

  std::filesystem::create_directories(destination.parent_path());
  write_text(destination, html_content.str(), "text/html", Encoding::GZIP,
             sourcemeta::core::JSON{nullptr},
             std::chrono::duration_cast<std::chrono::milliseconds>(
                 timestamp_end - timestamp_start));
}

} // namespace sourcemeta::one
