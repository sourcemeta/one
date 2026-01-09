#include <sourcemeta/one/resolver.h>
#include <sourcemeta/one/shared.h>

#include <sourcemeta/core/jsonschema.h>
#include <sourcemeta/core/uri.h>
#include <sourcemeta/core/yaml.h>

#include <algorithm> // std::transform
#include <cassert>   // assert
#include <cctype>    // std::tolower
#include <mutex>     // std::mutex, std::lock_guard

static auto rebase(const sourcemeta::one::Configuration::Collection &collection,
                   const sourcemeta::core::JSON::String &uri,
                   const sourcemeta::core::JSON::String &new_base,
                   const sourcemeta::core::JSON::String &new_prefix)
    -> sourcemeta::core::JSON::String {
  sourcemeta::core::URI maybe_relative{uri};
  maybe_relative.relative_to(collection.base);
  if (maybe_relative.is_relative()) {
    auto suffix{maybe_relative.path().value_or("")};
    assert(!suffix.empty());
    return sourcemeta::core::URI{new_base}
        .append_path(new_prefix)
        // TODO: Let `append_path` take a URI
        // TODO: Also implement a move overload
        .append_path(suffix)
        .canonicalize()
        .recompose();
  } else {
    return maybe_relative.recompose();
  }
}

static auto normalise_identifier(const std::string_view identifier)
    -> std::string {
  std::string lowercase{identifier};
  std::ranges::transform(lowercase, lowercase.begin(),
                         [](const auto character) {
                           return static_cast<char>(std::tolower(character));
                         });

  while (true) {
    if (lowercase.ends_with("#")) {
      lowercase.resize(lowercase.size() - 1);
    } else if (lowercase.ends_with(".json") || lowercase.ends_with(".yaml")) {
      lowercase.resize(lowercase.size() - 5);
    } else if (lowercase.ends_with(".yml")) {
      lowercase.resize(lowercase.size() - 4);
    } else if (lowercase.ends_with(".schema")) {
      lowercase.resize(lowercase.size() - 7);
    } else {
      break;
    }
  }

  return lowercase;
}

static auto
normalise_ref(const sourcemeta::one::Configuration::Collection &collection,
              const sourcemeta::core::URI &base, sourcemeta::core::JSON &schema,
              const sourcemeta::core::JSON::String &keyword,
              const sourcemeta::core::JSON::String &reference) -> void {
  // We never want to mess with internal references.
  // We assume those are always well formed
  if (reference.starts_with('#')) {
    return;
  }

  // If we have a match in the configuration resolver, then trust that.
  const auto match{collection.resolve.find(reference)};
  if (match != collection.resolve.cend()) {
    schema.assign(keyword, sourcemeta::core::JSON{match->second});
    return;
  }

  sourcemeta::core::URI value{reference};
  // Lowercase only the path component of the reference. Note that are careful
  // about not lowercasing the entire thing, as the reference may include a
  // JSON Pointer in the fragment
  const auto current_path{value.path()};
  if (current_path.has_value()) {
    value.path(normalise_identifier(current_path.value()));
  }

  if (value.is_absolute()) {
    // Turn the reference into a relative one if possible. That way, everything
    // is good even if we change the identifiers
    value.relative_to(base);
  }

  schema.assign(keyword, sourcemeta::core::JSON{value.recompose()});
}

namespace sourcemeta::one {

auto Resolver::operator()(
    std::string_view raw_identifier,
    const std::function<void(const std::filesystem::path &)> &callback) const
    -> std::optional<sourcemeta::core::JSON> {
  /////////////////////////////////////////////////////////////////////////////
  // (1) Lookup the schema
  /////////////////////////////////////////////////////////////////////////////

  // Internally, we keep all schema URI identifiers as lowercase to avoid
  // tricky cases with case-insensitive operating systems
  const auto identifier{normalise_identifier(raw_identifier)};
  auto result{this->views.find(identifier)};
  // If we don't recognise the schema, try a fallback as a last resort
  if (result == this->views.cend()) {
    return sourcemeta::core::schema_resolver(identifier);
  }

  /////////////////////////////////////////////////////////////////////////////
  // (2) Avoid rebasing on the fly if possible
  /////////////////////////////////////////////////////////////////////////////

  if (result->second.cache_path.has_value()) {
    // We can guarantee the cached outcome is JSON, so we don't need to try
    // reading as YAML
    auto schema{sourcemeta::one::read_json(result->second.cache_path.value())};
    assert(sourcemeta::core::is_schema(schema));
    if (callback) {
      callback(result->second.cache_path.value());
    }

    return schema;
  }

  /////////////////////////////////////////////////////////////////////////////
  // (3) Read the original schema file
  /////////////////////////////////////////////////////////////////////////////

  auto schema{sourcemeta::core::read_yaml_or_json(result->second.path)};
  assert(sourcemeta::core::is_schema(schema));
  if (callback) {
    callback(result->second.path);
  }

  // If the schema is not an object schema, then we are done
  if (!schema.is_object()) {
    return schema;
  }

  /////////////////////////////////////////////////////////////////////////////
  // (4) Make sure the schema explicitly declares the intended dialect
  /////////////////////////////////////////////////////////////////////////////

  // Note that we have to do this before attempting to analyse the schema, so
  // we can internally resolve any potential custom meta-schema
  schema.assign("$schema", sourcemeta::core::JSON{result->second.dialect});

  /////////////////////////////////////////////////////////////////////////////
  // (5) Normalise all references, if any, to match the new identifier
  /////////////////////////////////////////////////////////////////////////////

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Locations};
  const auto identifier_result{sourcemeta::core::identify(
      schema,
      [this](const auto subidentifier) {
        return this->operator()(subidentifier);
      },
      result->second.dialect)};
  const auto has_identifier{!identifier_result.empty()};
  frame.analyse(
      schema, sourcemeta::core::schema_walker,
      [this](const auto subidentifier) {
        return this->operator()(subidentifier);
      },
      result->second.dialect,
      // Otherwise we will loop over all locations twice
      has_identifier ? std::string_view{} : result->second.original_identifier);

  const auto ref_hash{schema.as_object().hash("$ref")};
  const auto dynamic_ref_hash{schema.as_object().hash("$dynamicRef")};
  for (const auto &entry : frame.locations()) {
    if (entry.second.type ==
            sourcemeta::core::SchemaFrame::LocationType::Resource ||
        entry.second.type ==
            sourcemeta::core::SchemaFrame::LocationType::Subschema) {
      auto &subschema{sourcemeta::core::get(schema, entry.second.pointer)};
      if (subschema.is_object()) {
        const auto maybe_ref{subschema.try_at("$ref", ref_hash)};
        if (maybe_ref) {
          // This is safe, as at this point we have validated all schemas
          // against their meta-schemas
          assert(maybe_ref->is_string());
          normalise_ref(result->second.collection.get(), entry.second.base,
                        subschema, "$ref", maybe_ref->to_string());
        }

        if (entry.second.base_dialect ==
            sourcemeta::core::SchemaBaseDialect::JSON_Schema_2020_12) {
          const auto maybe_dynamic_ref{
              subschema.try_at("$dynamicRef", dynamic_ref_hash)};
          if (maybe_dynamic_ref) {
            // This is safe, as at this point we have validated all schemas
            // against their meta-schemas
            assert(maybe_dynamic_ref->is_string());
            normalise_ref(result->second.collection.get(), entry.second.base,
                          subschema, "$dynamicRef",
                          maybe_dynamic_ref->to_string());
          }
        }
      }
    }
  }

  /////////////////////////////////////////////////////////////////////////////
  // (6) Assign the new final identifier to the schema
  /////////////////////////////////////////////////////////////////////////////

  sourcemeta::core::reidentify(
      schema, result->first,
      [this](const auto subidentifier) {
        return this->operator()(subidentifier);
      },
      result->second.dialect);

  return schema;
}

auto Resolver::add(const sourcemeta::core::JSON::String &server_url,
                   const std::filesystem::path &collection_relative_path,
                   const Configuration::Collection &collection,
                   const std::filesystem::path &path)
    -> std::pair<std::reference_wrapper<const sourcemeta::core::JSON::String>,
                 std::reference_wrapper<const sourcemeta::core::JSON::String>> {
  /////////////////////////////////////////////////////////////////////////////
  // (1) Read the schema file
  /////////////////////////////////////////////////////////////////////////////
  assert(path.is_absolute());
  const auto schema{sourcemeta::core::read_yaml_or_json(path)};
  assert(sourcemeta::core::is_schema(schema));

  const std::string default_dialect_str{
      collection.default_dialect.value_or("")};

  /////////////////////////////////////////////////////////////////////////////
  // (2) Try our best to determine the identifier of the schema, defaulting to a
  // file-system-based identifier based on the *current* URI
  /////////////////////////////////////////////////////////////////////////////
  const auto default_identifier{
      sourcemeta::core::URI{collection.base}
          .append_path(normalise_identifier(
              std::filesystem::relative(path, collection.absolute_path)
                  .string()))
          .canonicalize()
          .recompose()};
  sourcemeta::core::URI identifier_uri{
      normalise_identifier(sourcemeta::core::identify(
          schema,
          [this](const auto subidentifier) {
            return this->operator()(subidentifier);
          },
          default_dialect_str, default_identifier))};
  identifier_uri.canonicalize();
  auto identifier{
      identifier_uri.is_relative()
          // TODO: Becase with `try_resolve_from`, `https://example.com/foo` +
          // `bar.json` will be `https://example.com/bar.json` instead of
          // `https://example.com/foo/bar.json`. Maybe we need an `append_from`?
          ? (collection.base + "/" + identifier_uri.recompose())
          : identifier_uri.recompose()};
  // We have to do something if the schema is the base. Note that URI
  // canonicalisation technically cannot remove trailing slashes as they might
  // have meaning in certain use cases. But we still consider them equal in the
  // context of the One
  if (identifier == collection.base || identifier == collection.base + "/") {
    identifier = default_identifier;
  }
  // A final check that everything went well
  if (!identifier.starts_with(collection.base)) {
    throw ResolverOutsideBaseError(identifier, collection.base);
  }
  // Otherwise we have things like "../" that should not be there
  assert(identifier.find("..") == std::string::npos);

  /////////////////////////////////////////////////////////////////////////////
  // (3) Determine the new URI of the schema, from the one base URI
  /////////////////////////////////////////////////////////////////////////////
  const auto new_identifier{
      rebase(collection, identifier, server_url, collection_relative_path)};
  // Otherwise we have things like "../" that should not be there
  assert(new_identifier.find("..") == std::string::npos);

  /////////////////////////////////////////////////////////////////////////////
  // (4) Determine the dialect of the schema, which we also need to make sure
  // we rebase according to the one base URI, etc
  /////////////////////////////////////////////////////////////////////////////
  const auto raw_dialect{
      sourcemeta::core::dialect(schema, default_dialect_str)};
  // If we couldn't determine the dialect, we would be in trouble!
  assert(!raw_dialect.empty());
  // Don't modify references to official meta-schemas
  // TODO: This line may be unnecessarily slow. We should have a different
  // function that just checks for string equality in an `std::unordered_map`
  // of official dialects without constructing the final object
  const auto is_official_dialect{
      sourcemeta::core::schema_resolver(raw_dialect).has_value()};
  auto current_dialect{is_official_dialect
                           ? std::string{raw_dialect}
                           : rebase(collection,
                                    normalise_identifier(raw_dialect),
                                    server_url, collection_relative_path)};
  // Otherwise we messed things up
  assert(!current_dialect.ends_with("#.json"));

  /////////////////////////////////////////////////////////////////////////////
  // (5) Safely one the schema entry in the resolver
  /////////////////////////////////////////////////////////////////////////////
  std::unique_lock lock{this->mutex};
  auto result{this->views.emplace(
      new_identifier,
      Entry{.cache_path = std::nullopt,
            .path = path,
            .dialect = std::move(current_dialect),
            .relative_path = sourcemeta::core::URI{new_identifier}
                                 .relative_to(server_url)
                                 .recompose(),
            .original_identifier = identifier,
            .collection = collection})};
  lock.unlock();
  if (!result.second && result.first->second.path != path) {
    throw sourcemeta::core::SchemaFrameError(
        result.first->first, "Cannot register the same identifier twice");
  }

  return {result.first->second.original_identifier, result.first->first};
}

auto Resolver::cache_path(const sourcemeta::core::JSON::String &uri,
                          const std::filesystem::path &path) -> void {
  assert(std::filesystem::exists(path));
  // As we are modifying the actual map
  std::unique_lock lock{this->mutex};
  auto entry{this->views.find(uri)};
  assert(entry != this->views.cend());
  assert(!entry->second.cache_path.has_value());
  entry->second.cache_path = path;
}

} // namespace sourcemeta::one
