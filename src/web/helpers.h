#ifndef SOURCEMETA_ONE_WEB_HELPERS_H_
#define SOURCEMETA_ONE_WEB_HELPERS_H_

#include <sourcemeta/core/html.h>
#include <sourcemeta/one/configuration.h>
#include <sourcemeta/one/shared.h>

#include <cassert>     // assert
#include <string>      // std::string
#include <string_view> // std::string_view

namespace sourcemeta::one::html {

inline auto make_breadcrumb(sourcemeta::core::HTMLWriter &writer,
                            const sourcemeta::core::JSON &breadcrumb) -> void {
  assert(breadcrumb.is_array());
  assert(!breadcrumb.empty());

  writer.nav()
      .attribute("class", "container-fluid px-4 py-2 bg-light bg-gradient "
                          "border-bottom font-monospace")
      .attribute("aria-label", "breadcrumb");

  writer.ol().attribute("class", "breadcrumb mb-0");

  // First item: back arrow
  writer.li().attribute("class", "breadcrumb-item");
  writer.a().attribute("href", "/");
  writer.i().attribute("class", "bi bi-arrow-left").close();
  writer.close();
  writer.close();

  for (auto iterator = breadcrumb.as_array().cbegin();
       iterator != breadcrumb.as_array().cend(); ++iterator) {
    if (std::next(iterator) == breadcrumb.as_array().cend()) {
      writer.li()
          .attribute("class", "breadcrumb-item active")
          .attribute("aria-current", "page");
      writer.text(iterator->at("name").to_string());
      writer.close();
    } else {
      writer.li().attribute("class", "breadcrumb-item");
      writer.a().attribute("href", iterator->at("path").to_string());
      writer.text(iterator->at("name").to_string());
      writer.close();
      writer.close();
    }
  }

  writer.close();
  writer.close();
}

inline auto
make_schema_health_progress_bar(sourcemeta::core::HTMLWriter &writer,
                                const sourcemeta::core::JSON::Integer health)
    -> void {
  const auto [progress_class, progress_style] =
      [health]() -> std::pair<std::string, std::string> {
    if (health > 90) {
      return {"progress-bar text-bg-success",
              "width:" + std::to_string(health) + "%"};
    } else if (health > 60) {
      return {"progress-bar text-bg-warning",
              "width:" + std::to_string(health) + "%"};
    } else if (health == 0) {
      // Otherwise if we set width: 0px, then the label is not shown
      return {"progress-bar text-bg-danger", ""};
    } else {
      return {"progress-bar text-bg-danger",
              "width:" + std::to_string(health) + "%"};
    }
  }();

  writer.div()
      .attribute("class", "progress")
      .attribute("role", "progressbar")
      .attribute("aria-label", "Schema health score")
      .attribute("aria-valuenow", std::to_string(health))
      .attribute("aria-valuemin", "0")
      .attribute("aria-valuemax", "100");

  writer.div().attribute("class", progress_class);
  if (!progress_style.empty()) {
    writer.attribute("style", progress_style);
  }
  writer.text(std::to_string(health) + "%");
  writer.close();
  writer.close();
}

inline auto
make_dialect_badge(sourcemeta::core::HTMLWriter &writer,
                   const sourcemeta::core::JSON::String &base_dialect_uri)
    -> void {
  const auto [short_name, is_current] =
      [&base_dialect_uri]() -> std::pair<std::string, bool> {
    if (base_dialect_uri == "https://json-schema.org/draft/2020-12/schema") {
      return {"2020-12", true};
    } else if (base_dialect_uri ==
               "https://json-schema.org/draft/2019-09/schema") {
      return {"2019-09", false};
    } else if (base_dialect_uri == "http://json-schema.org/draft-07/schema#") {
      return {"draft7", false};
    } else if (base_dialect_uri == "http://json-schema.org/draft-06/schema#") {
      return {"draft6", false};
    } else if (base_dialect_uri == "http://json-schema.org/draft-04/schema#") {
      return {"draft4", false};
    } else {
      return {"unknown", false};
    }
  }();

  // Capitalize first character
  std::string display_name = short_name;
  if (!display_name.empty()) {
    display_name[0] = static_cast<char>(std::toupper(display_name[0]));
  }

  writer.a()
      .attribute("href", "https://www.learnjsonschema.com/" + short_name)
      .attribute("target", "_blank");
  writer.span().attribute(
      "class",
      "align-middle badge " +
          std::string(is_current ? "text-bg-primary" : "text-bg-danger"));
  writer.text(display_name);
  writer.close();
  writer.close();
}

inline auto make_directory_header(sourcemeta::core::HTMLWriter &writer,
                                  const sourcemeta::core::JSON &directory)
    -> void {
  if (!directory.defines("title")) {
    writer.div().close();
    return;
  }

  writer.div().attribute("class", "container-fluid px-4 pt-4 d-flex");

  if (directory.defines("github") && !directory.at("github").includes('/')) {
    writer.img()
        .attribute("src", "https://github.com/" +
                              directory.at("github").to_string() +
                              ".png?size=200")
        .attribute("width", "100")
        .attribute("height", "100")
        .attribute("class", "img-thumbnail me-4");
  }

  // Title section
  writer.div();
  writer.h2().attribute("class", "fw-bold h4");
  writer.text(directory.at("title").to_string());
  writer.close();

  if (directory.defines("description")) {
    writer.p().attribute("class", "text-secondary");
    writer.text(directory.at("description").to_string());
    writer.close();
  }

  if (directory.defines("email") || directory.defines("github") ||
      directory.defines("website")) {
    writer.div(); // contact div

    if (directory.defines("github")) {
      writer.small().attribute("class",
                               "me-3 d-block mb-2 mb-md-0 d-md-inline-block");
      writer.i().attribute("class", "bi bi-github text-secondary me-1").close();
      writer.a()
          .attribute("href",
                     "https://github.com/" + directory.at("github").to_string())
          .attribute("class", "text-secondary")
          .attribute("target", "_blank");
      writer.text(directory.at("github").to_string());
      writer.close();
      writer.close();
    }

    if (directory.defines("website")) {
      writer.small().attribute("class",
                               "me-3 d-block mb-2 mb-md-0 d-md-inline-block");
      writer.i()
          .attribute("class", "bi bi-link-45deg text-secondary me-1")
          .close();
      writer.a()
          .attribute("href", directory.at("website").to_string())
          .attribute("class", "text-secondary")
          .attribute("target", "_blank");
      writer.text(directory.at("website").to_string());
      writer.close();
      writer.close();
    }

    if (directory.defines("email")) {
      writer.small().attribute("class",
                               "me-3 d-block mb-2 mb-md-0 d-md-inline-block");
      writer.i()
          .attribute("class", "bi bi-envelope text-secondary me-1")
          .close();
      writer.a()
          .attribute("href", "mailto:" + directory.at("email").to_string())
          .attribute("class", "text-secondary");
      writer.text(directory.at("email").to_string());
      writer.close();
      writer.close();
    }

    writer.close();
  }

  writer.close();
  writer.close();
}

inline auto make_file_manager_row(sourcemeta::core::HTMLWriter &writer,
                                  const sourcemeta::core::JSON &entry) -> void {
  writer.tr();

  // Type column
  writer.td().attribute("class", "text-nowrap");
  if (entry.at("type").to_string() == "directory") {
    if (entry.defines("github") && !entry.at("github").includes('/')) {
      writer.img()
          .attribute("src", "https://github.com/" +
                                entry.at("github").to_string() + ".png?size=80")
          .attribute("width", "40")
          .attribute("height", "40");
    } else {
      writer.i().attribute("class", "bi bi-folder-fill").close();
    }
  } else {
    make_dialect_badge(writer, entry.at("baseDialect").to_string());
  }
  writer.close();

  // Name column
  writer.td().attribute("class", "font-monospace");
  writer.a().attribute("href", entry.at("path").to_string());
  writer.text(entry.at("name").to_string());
  writer.close();
  writer.close();

  // Title column
  writer.td();
  writer.small(entry.defines("title") ? entry.at("title").to_string() : "-");
  writer.close();

  // Description column
  writer.td();
  writer.small(
      entry.defines("description") ? entry.at("description").to_string() : "-");
  writer.close();

  // Dependencies column
  writer.td();
  writer.small(entry.defines("dependencies")
                   ? std::to_string(entry.at("dependencies").to_integer())
                   : "-");
  writer.close();

  // Health column
  writer.td().attribute("class", "align-middle");
  make_schema_health_progress_bar(writer, entry.at("health").to_integer());
  writer.close();

  writer.close();
}

inline auto make_file_manager_table_header(sourcemeta::core::HTMLWriter &writer)
    -> void {
  writer.thead();
  writer.tr();
  writer.th()
      .attribute("scope", "col")
      .attribute("style", "width: 50px")
      .close();
  writer.th().attribute("scope", "col");
  writer.text("Name");
  writer.close();
  writer.th().attribute("scope", "col");
  writer.text("Title");
  writer.close();
  writer.th().attribute("scope", "col");
  writer.text("Description");
  writer.close();
  writer.th().attribute("scope", "col");
  writer.text("Dependencies");
  writer.close();
  writer.th().attribute("scope", "col").attribute("style", "width: 150px");
  writer.text("Health");
  writer.close();
  writer.close();
  writer.close();
}

inline auto make_file_manager(sourcemeta::core::HTMLWriter &writer,
                              const sourcemeta::core::JSON &directory) -> void {
  if (directory.at("entries").empty()) {
    writer.div().attribute("class", "container-fluid p-4 flex-grow-1");
    writer.p(
        "Things look a bit empty over here. Try ingesting some schemas using "
        "the configuration file!");
    writer.close();
    return;
  }

  // First pass: check what we have
  bool has_regular_entries = false;
  bool has_special_entries = false;
  for (const auto &entry : directory.at("entries").as_array()) {
    const auto path = entry.at("path").to_string();
    if (path == "/self" || path == "/self/") {
      has_special_entries = true;
    } else {
      has_regular_entries = true;
    }
  }

  writer.div().attribute("class", "container-fluid p-4 flex-grow-1");

  if (has_regular_entries) {
    writer.table().attribute(
        "class", "table table-bordered border-light-subtle table-light");
    make_file_manager_table_header(writer);
    writer.tbody();
    for (const auto &entry : directory.at("entries").as_array()) {
      const auto path = entry.at("path").to_string();
      if (path != "/self" && path != "/self/") {
        make_file_manager_row(writer, entry);
      }
    }
    writer.close();
    writer.close();
  }

  if (has_special_entries) {
    writer.h6().attribute("class", "text-secondary mt-4 mb-3");
    writer.text("Special directories");
    writer.close();
    writer.table().attribute(
        "class", "table table-bordered border-light-subtle table-light");
    make_file_manager_table_header(writer);
    writer.tbody();
    for (const auto &entry : directory.at("entries").as_array()) {
      const auto path = entry.at("path").to_string();
      if (path == "/self" || path == "/self/") {
        make_file_manager_row(writer, entry);
      }
    }
    writer.close();
    writer.close();
  }

  writer.close();
}

} // namespace sourcemeta::one::html

#endif
