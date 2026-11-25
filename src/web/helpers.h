#ifndef SOURCEMETA_ONE_WEB_HELPERS_H_
#define SOURCEMETA_ONE_WEB_HELPERS_H_

#include <sourcemeta/core/html.h>
#include <sourcemeta/core/markdown.h>
#include <sourcemeta/one/configuration.h>
#include <sourcemeta/one/shared.h>

#include <algorithm>   // std::ranges::none_of
#include <cassert>     // assert
#include <cctype>      // std::toupper
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
    }

    if (health > 60) {
      return {"progress-bar text-bg-warning",
              "width:" + std::to_string(health) + "%"};
    }

    if (health == 0) {
      // Otherwise if we set width: 0px, then the label is not shown
      return {"progress-bar text-bg-danger", ""};
    }

    return {"progress-bar text-bg-danger",
            "width:" + std::to_string(health) + "%"};
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

// What a badge says of what a resource conforms to, which is the name the
// ecosystem knows it by, whether that name is the latest one, and where a
// reader can go to read about it
struct ConformanceBadge {
  std::string name;
  bool is_latest;
  std::string reference;
};

inline auto learnjsonschema_badge(const std::string_view name,
                                  const bool is_latest) -> ConformanceBadge {
  std::string display{name};
  display[0] =
      static_cast<char>(std::toupper(static_cast<unsigned char>(display[0])));
  return {.name = std::move(display),
          .is_latest = is_latest,
          .reference = "https://www.learnjsonschema.com/" + std::string{name}};
}

inline auto conformance_badge_of(const sourcemeta::core::JSON::String &uri)
    -> ConformanceBadge {
  if (uri == "https://json-schema.org/draft/2020-12/schema" ||
      uri == "https://json-schema.org/draft/2020-12/hyper-schema") {
    return learnjsonschema_badge("2020-12", true);
  }

  if (uri == "https://json-schema.org/draft/2019-09/schema" ||
      uri == "https://json-schema.org/draft/2019-09/hyper-schema") {
    return learnjsonschema_badge("2019-09", false);
  }

  if (uri == "http://json-schema.org/draft-07/schema#" ||
      uri == "http://json-schema.org/draft-07/hyper-schema#") {
    return learnjsonschema_badge("draft7", false);
  }

  if (uri == "http://json-schema.org/draft-06/schema#" ||
      uri == "http://json-schema.org/draft-06/hyper-schema#") {
    return learnjsonschema_badge("draft6", false);
  }

  if (uri == "http://json-schema.org/draft-04/schema#" ||
      uri == "http://json-schema.org/draft-04/hyper-schema#") {
    return learnjsonschema_badge("draft4", false);
  }

  if (uri == "http://json-schema.org/draft-03/schema#" ||
      uri == "http://json-schema.org/draft-03/hyper-schema#") {
    return learnjsonschema_badge("draft3", false);
  }

  // A revision of the OpenAPI Specification names itself at the URL that
  // publishes it, so the badge sends a reader there rather than to a
  // reference that only covers dialects
  if (uri == "https://spec.openapis.org/oas/v3.2") {
    return {.name = "OpenAPI 3.2", .is_latest = true, .reference = uri};
  }

  if (uri == "https://spec.openapis.org/oas/v3.1") {
    return {.name = "OpenAPI 3.1", .is_latest = false, .reference = uri};
  }

  if (uri == "https://spec.openapis.org/oas/v3.0") {
    return {.name = "OpenAPI 3.0", .is_latest = false, .reference = uri};
  }

  return learnjsonschema_badge("unknown", false);
}

inline auto
make_conformance_badge(sourcemeta::core::HTMLWriter &writer,
                       const sourcemeta::core::JSON::String &conforms_to)
    -> void {
  const auto badge{conformance_badge_of(conforms_to)};
  writer.a().attribute("href", badge.reference).attribute("target", "_blank");
  writer.span().attribute(
      "class",
      "align-middle badge " +
          std::string(badge.is_latest ? "text-bg-primary" : "text-bg-danger"));
  writer.text(badge.name);
  writer.close();
  writer.close();
}

// What a listing says about a schema, said on the schema itself. A reader who
// clicks through a locked row would otherwise be given no sign that what they
// are looking at is private, which is the question somebody about to pass on a
// link is asking
inline auto make_private_badge(sourcemeta::core::HTMLWriter &writer) -> void {
  writer.span().attribute(
      "class", "align-middle badge rounded-pill border border-warning "
               "text-warning fw-normal");
  writer.text("Private");
  writer.close();
}

inline auto make_directory_header(sourcemeta::core::HTMLWriter &writer,
                                  const sourcemeta::core::JSON &directory)
    -> void {
  const auto is_private{directory.at("private").to_boolean()};

  // A directory says what it is whether or not its collection was given a name
  // to say it under, so the mark cannot depend on there being a header to hang
  // it from
  if (!directory.defines("title")) {
    if (is_private) {
      writer.div().attribute("class", "container-fluid px-4 pt-4");
      make_private_badge(writer);
      writer.close();
    } else {
      writer.div().close();
    }

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
  if (is_private) {
    writer.div().attribute("class", "mb-2");
    make_private_badge(writer);
    writer.close();
  }
  writer.h2().attribute("class", "fw-bold h4");
  writer.text(directory.at("title").to_string());
  writer.close();

  if (directory.defines("description")) {
    writer.div().attribute("class", "text-secondary");
    writer.raw(sourcemeta::core::markdown_to_html(
        directory.at("description").to_string()));
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
                                  const sourcemeta::core::JSON &entry,
                                  const bool is_private) -> void {
  writer.tr();

  // Type column
  writer.td().attribute("class", "text-nowrap");
  if (entry.at("type").to_string() == "directory") {
    // A directory any policy governs is shown locked rather than as a folder,
    // wherever it sits under that policy. What it marks is that the directory
    // is private, which somebody deciding whether to pass its link on wants to
    // know as much three levels in as at the boundary
    if (is_private) {
      writer.i().attribute("class", "bi bi-lock-fill text-warning").close();
    } else if (entry.defines("github") && !entry.at("github").includes('/')) {
      writer.img()
          .attribute("src", "https://github.com/" +
                                entry.at("github").to_string() + ".png?size=80")
          .attribute("width", "40")
          .attribute("height", "40");
    } else {
      writer.i().attribute("class", "bi bi-folder-fill").close();
    }
  } else {
    make_conformance_badge(writer, entry.at("conformsTo").to_string());
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
  writer.td().attribute("class", "small");
  if (entry.defines("description")) {
    writer.raw(sourcemeta::core::markdown_to_html(
        entry.at("description").to_string()));
  } else {
    writer.text("-");
  }
  writer.close();

  // Schemas column
  writer.td();
  writer.small(entry.defines("schemas")
                   ? std::to_string(entry.at("schemas").to_integer())
                   : "-");
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
  writer.text("Schemas");
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

// What a listing can take the visitor to, which an OpenAPI description is not
// until a page answers for one
inline auto is_navigable(const sourcemeta::core::JSON &entry) -> bool {
  return entry.at("type").to_string() != "openapi";
}

inline auto make_file_manager(sourcemeta::core::HTMLWriter &writer,
                              const sourcemeta::core::JSON &directory) -> void {
  if (std::ranges::none_of(directory.at("entries").as_array(), is_navigable)) {
    writer.div().attribute("class", "container-fluid p-4 flex-grow-1");
    writer.p(
        "Things look a bit empty over here. Try ingesting some schemas using "
        "the configuration file!");
    writer.close();
    return;
  }

  constexpr std::string_view SELF_PATH{"/self"};
  constexpr std::string_view SELF_PATH_SLASH{"/self/"};

  writer.div().attribute("class", "container-fluid p-4 flex-grow-1");

  bool has_regular_entries{false};
  for (const auto &entry : directory.at("entries").as_array()) {
    const auto path{entry.at("path").to_string()};
    if (path != SELF_PATH && path != SELF_PATH_SLASH && is_navigable(entry)) {
      if (!has_regular_entries) {
        writer.table().attribute(
            "class", "table table-bordered border-light-subtle table-light");
        make_file_manager_table_header(writer);
        writer.tbody();
        has_regular_entries = true;
      }

      make_file_manager_row(writer, entry, entry.at("private").to_boolean());
    }
  }

  if (has_regular_entries) {
    writer.close();
    writer.close();
  }

  for (const auto &entry : directory.at("entries").as_array()) {
    const auto path{entry.at("path").to_string()};
    if (path == SELF_PATH || path == SELF_PATH_SLASH) {
      writer.h6().attribute("class", "text-secondary mt-4 mb-3");
      writer.text("Special directories");
      writer.close();
      writer.table().attribute(
          "class", "table table-bordered border-light-subtle table-light");
      make_file_manager_table_header(writer);
      writer.tbody();
      make_file_manager_row(writer, entry, entry.at("private").to_boolean());
      writer.close();
      writer.close();
      break;
    }
  }

  writer.close();
}

} // namespace sourcemeta::one::html

#endif
