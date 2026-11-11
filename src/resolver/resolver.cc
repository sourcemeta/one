#include <sourcemeta/one/resolver.h>
#include <sourcemeta/one/shared.h>

#include <sourcemeta/core/error.h>
#include <sourcemeta/core/jsonschema.h>
#include <sourcemeta/core/metapack.h>
#include <sourcemeta/core/openapi.h>
#include <sourcemeta/core/text.h>
#include <sourcemeta/core/uri.h>
#include <sourcemeta/core/yaml.h>

#include <algorithm>     // std::ranges::find_if
#include <cassert>       // assert
#include <mutex>         // std::mutex, std::lock_guard
#include <optional>      // std::optional, std::nullopt
#include <shared_mutex>  // std::shared_lock
#include <sstream>       // std::ostringstream
#include <string>        // std::string
#include <system_error>  // std::error_code
#include <unordered_set> // std::unordered_set

// The `.json` extension is not a URI concern, so this single alternative is
// tried by hand, and always after the identifier itself
static auto resolve_alternative(const std::string &identifier) -> std::string {
  return identifier.ends_with(".json")
             ? identifier.substr(0, identifier.size() - 5)
             : identifier + ".json";
}

// Keys are stored canonicalised, so a reference spelled the way its key is
// spelled matches without any further work, and the remaining spellings are
// tried in turn. Most collections declare no mapping at all, and every
// reference of every schema comes through here, so that case does no work
static auto
find_resolve_match(const sourcemeta::one::Configuration::Collection &collection,
                   const std::string &identifier) {
  if (collection.resolve.empty()) {
    return collection.resolve.cend();
  }

  auto match{collection.resolve.find(identifier)};
  if (match != collection.resolve.cend()) {
    return match;
  }

  match = collection.resolve.find(resolve_alternative(identifier));
  if (match != collection.resolve.cend()) {
    return match;
  }

  std::string canonical;
  try {
    canonical = sourcemeta::core::URI::canonicalize(identifier);
  } catch (const sourcemeta::core::URIParseError &) {
    return collection.resolve.cend();
  }

  if (canonical == identifier) {
    return collection.resolve.cend();
  }

  match = collection.resolve.find(canonical);
  if (match != collection.resolve.cend()) {
    return match;
  }

  return collection.resolve.find(resolve_alternative(canonical));
}

static auto
pre_resolve(const sourcemeta::one::Configuration::Collection &collection,
            const std::string_view uri, const sourcemeta::core::URI &server)
    -> std::optional<std::string> {
  if (collection.resolve.empty()) {
    return std::nullopt;
  }

  const auto match{find_resolve_match(collection, std::string{uri})};
  if (match == collection.resolve.cend()) {
    return std::nullopt;
  }
  sourcemeta::core::URI target{match->second};
  if (target.is_relative()) {
    sourcemeta::core::URI merged{server};
    merged.append_path(std::move(target));
    return merged.recompose();
  }
  return target.recompose();
}

static auto rebase(const sourcemeta::one::Configuration::Collection &collection,
                   const sourcemeta::core::JSON::String &uri,
                   const sourcemeta::core::URI &new_base,
                   const sourcemeta::core::JSON::String &new_prefix)
    -> sourcemeta::core::JSON::String {
  // A collection owns a whole path prefix of its base URI, so what an absolute
  // identifier is measured against is containment on component boundaries, and
  // not the relative reference that resolution would take back to it
  const sourcemeta::core::URI current{uri};
  std::optional<std::string> suffix;
  if (current.is_relative()) {
    suffix = current.path().value_or("");
  } else if (current.scheme() == collection.base_uri.scheme() &&
             current.has_same_authority(collection.base_uri)) {
    suffix = sourcemeta::core::URI::strip_path_prefix(
        current.path().value_or(""), collection.base_uri.path().value_or(""));
  }

  if (!suffix.has_value()) {
    return current.recompose();
  }

  assert(!suffix.value().empty());
  return sourcemeta::core::URI{new_base}
      .append_path(new_prefix)
      .append_path(suffix.value())
      .canonicalize()
      .recompose();
}

// The identifier a schema declares, or the given default when it declares
// none. A schema whose dialect cannot be determined or resolved also takes the
// default, as what such a schema is worth is settled further along, where the
// file it came from is still at hand
static auto
declared_identifier(const sourcemeta::core::JSON &schema,
                    const sourcemeta::core::SchemaResolver &resolver,
                    const std::string_view default_dialect,
                    const std::string_view default_identifier)
    -> sourcemeta::core::JSON::String {
  try {
    const sourcemeta::core::SchemaFrame frame{
        sourcemeta::core::SchemaFrame::Mode::Root,
        schema,
        sourcemeta::core::schema_walker,
        resolver,
        default_dialect,
        default_identifier,
        sourcemeta::core::SchemaFrame::IdentifierMode::Fallback};
    return frame.root();
  } catch (const sourcemeta::core::SchemaUnknownBaseDialectError &) {
    return sourcemeta::core::JSON::String{default_identifier};
  } catch (const sourcemeta::core::SchemaResolutionError &) {
    return sourcemeta::core::JSON::String{default_identifier};
  }
}

// Whether the dialect in force gives a keyword any meaning at all, which is
// what the vocabularies it draws on say rather than what its base dialect
// happens to name
static auto
dialect_defines(const sourcemeta::core::SchemaVocabularies &vocabularies,
                const std::string_view keyword) -> bool {
  return sourcemeta::core::schema_walker(keyword, vocabularies).type !=
         sourcemeta::core::SchemaKeywordType::Unknown;
}

static auto normalise_identifier(const std::string_view identifier)
    -> std::string {
  std::string lowercase{identifier};
  sourcemeta::core::to_lowercase(lowercase);

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

// What a reference becomes once it names what this instance serves, or nothing
// at all for one to leave exactly as it was written. Both kinds of leaf rebase
// a reference through here, as what a reference means does not depend on the
// kind of document that spells it
static auto
rebase_reference(const sourcemeta::one::Configuration::Collection &collection,
                 const sourcemeta::core::URI &base,
                 const sourcemeta::core::JSON::String &reference,
                 const sourcemeta::core::URI &server)
    -> std::optional<sourcemeta::core::JSON::String> {
  // We never want to mess with internal references.
  // We assume those are always well formed
  if (reference.starts_with('#')) {
    return std::nullopt;
  }

  // If we have a match in the configuration resolver, then trust that.
  const auto match{find_resolve_match(collection, reference)};
  if (match != collection.resolve.cend()) {
    sourcemeta::core::URI target{match->second};
    if (target.is_relative()) {
      sourcemeta::core::URI merged{server};
      merged.append_path(std::move(target));
      target = std::move(merged);
    }
    const auto target_path{target.path()};
    if (target_path.has_value()) {
      target.path(normalise_identifier(target_path.value()));
    }
    // For targets in the instance URL's namespace, store the served value
    // as a relative reference so the schema body stays portable across
    // deployments. For foreign authorities, keep the full URI so reference
    // resolution sees it as-is (and fails honestly if unregistered).
    if (target.has_same_authority(server)) {
      return target.recompose_relative();
    }

    return target.recompose();
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

  return value.recompose();
}

// What this instance serves a declared dialect as, which is what a document is
// analysed against rather than whatever it happened to name. Both kinds of leaf
// settle it this way, as a document that names a dialect this catalog holds
// names it the way the collection writes it either way
static auto
served_dialect(const sourcemeta::one::Configuration::Collection &collection,
               const std::filesystem::path &location,
               const std::string_view declared,
               const sourcemeta::core::URI &server) -> std::string {
  std::string raw{declared};
  auto rewritten{pre_resolve(collection, raw, server)};
  bool resolved_to_instance{false};
  if (rewritten.has_value()) {
    raw = std::move(*rewritten);
    // If pre_resolve yielded a URI already in the instance URL's authority,
    // it is already in the canonical registry form. The rebase machinery
    // exists to translate baseUri-canonical identifiers into server-URL
    // form, and would only mangle a URI that is already there.
    resolved_to_instance =
        sourcemeta::core::URI{raw}.has_same_authority(server);
  }

  if (resolved_to_instance) {
    return normalise_identifier(raw);
  }

  if (sourcemeta::core::schema_is_known(raw)) {
    return raw;
  }

  return rebase(collection, normalise_identifier(raw), server, location);
}

static auto
normalise_ref(const sourcemeta::one::Configuration::Collection &collection,
              const sourcemeta::core::URI &base, sourcemeta::core::JSON &schema,
              const sourcemeta::core::JSON::String &keyword,
              const sourcemeta::core::JSON::String &reference,
              const sourcemeta::core::URI &server) -> void {
  const auto rewritten{rebase_reference(collection, base, reference, server)};
  if (rewritten.has_value()) {
    schema.assign(keyword, sourcemeta::core::JSON{rewritten.value()});
  }
}

// What a schema calls itself, and what this instance serves it as. Both are
// settled together because the second is derived from the first
struct SchemaIdentity {
  sourcemeta::core::JSON::String current;
  sourcemeta::core::JSON::String served;
};

// The identifier a file takes from where it sits, which is what it is served
// as wherever it names nothing of its own
static auto default_identifier_of(
    const sourcemeta::one::Configuration::Collection &collection,
    const std::filesystem::path &path) -> sourcemeta::core::JSON::String {
  return sourcemeta::core::URI{collection.base_uri}
      .append_path(normalise_identifier(
          std::filesystem::relative(path, collection.absolute_path).string()))
      .canonicalize()
      .recompose();
}

// What this instance serves a file as, given what the file calls itself. Every
// step from here down is the same whatever the file holds, so both kinds of
// leaf settle their identity through this rather than each spelling the steps
// out for itself
static auto
settle_identity(const sourcemeta::one::Configuration::Collection &collection,
                const std::filesystem::path &location,
                const std::filesystem::path &path,
                const sourcemeta::core::JSON::String &declared,
                const sourcemeta::core::JSON::String &default_identifier,
                const sourcemeta::core::URI &server) -> SchemaIdentity {
  sourcemeta::core::URI identifier_uri{normalise_identifier(declared)};
  identifier_uri.canonicalize();
  auto identifier{identifier_uri.is_relative()
                      ? sourcemeta::core::URI{collection.base_uri}
                            .append_path(std::move(identifier_uri))
                            .canonicalize()
                            .recompose()
                      : identifier_uri.recompose()};
  // We have to do something if the schema is the base. Note that URI
  // canonicalisation technically cannot remove trailing slashes as they might
  // have meaning in certain use cases. But we still consider them equal in
  // the context of the One
  if (identifier == collection.base || identifier == collection.base + "/") {
    identifier = default_identifier;
  }
  // A final check that everything went well
  if (!identifier.starts_with(collection.base)) {
    throw sourcemeta::one::ResolverOutsideBaseError(path, identifier,
                                                    collection.base);
  }
  // Otherwise we have things like "../" that should not be there
  assert(identifier.find("..") == std::string::npos);

  auto served{rebase(collection, identifier, server, location)};
  // Otherwise we have things like "../" that should not be there
  assert(served.find("..") == std::string::npos);
  return {.current = std::move(identifier), .served = std::move(served)};
}

static auto
schema_identity(const sourcemeta::one::Configuration::Collection &collection,
                const std::filesystem::path &location,
                const std::filesystem::path &path,
                const sourcemeta::core::JSON &schema,
                const sourcemeta::core::SchemaResolver &resolver,
                const sourcemeta::core::URI &server) -> SchemaIdentity {
  const auto default_identifier{default_identifier_of(collection, path)};
  return settle_identity(
      collection, location, path,
      declared_identifier(schema, resolver,
                          collection.default_dialect.value_or(""),
                          default_identifier),
      default_identifier, server);
}

// The same, for an OpenAPI description. The identity keyword a revision
// reserves is what differs, and the collection's default dialect plays no part,
// as the dialect of a Schema Object is fixed by the revision with
// `jsonSchemaDialect` as the only override
static auto
openapi_identity(const sourcemeta::one::Configuration::Collection &collection,
                 const std::filesystem::path &location,
                 const std::filesystem::path &path,
                 const sourcemeta::core::JSON &document,
                 const sourcemeta::core::URI &server) -> SchemaIdentity {
  const auto default_identifier{default_identifier_of(collection, path)};
  return settle_identity(
      collection, location, path,
      sourcemeta::core::openapi_base(document, default_identifier),
      default_identifier, server);
}

// Whether a collection would take this file in when it walks its directory,
// which is what decides that the registry serves it under a URI of its own
static auto serves(const sourcemeta::one::Configuration::Collection &collection,
                   const std::filesystem::path &path) -> bool {
  if (!sourcemeta::core::is_under_path(path, collection.absolute_path)) {
    return false;
  }

  if (std::ranges::any_of(collection.ignore, [&path](const auto &ignored) {
        return sourcemeta::core::is_under_path(path, ignored);
      })) {
    return false;
  }

  const auto native{path.string()};
  return std::ranges::any_of(
      collection.extension, [&path, &native](const auto &extension) {
        return extension.empty() ? !path.has_extension()
                                 : native.ends_with(extension);
      });
}

namespace sourcemeta::one {

auto rebase_resolve_targets(Configuration &configuration) -> void {
  const sourcemeta::core::URI server{configuration.url};
  for (auto &entry : configuration.entries) {
    auto *collection{std::get_if<Configuration::Collection>(&entry.second)};
    if (collection == nullptr || collection->resolve.empty()) {
      continue;
    }

    // Which configuration file declared this collection, so that a bad entry
    // points at the file the reader has to edit
    const auto *declared_in{collection->extra.try_at("x-sourcemeta-one:path")};
    const auto declared_path{
        declared_in == nullptr
            ? configuration.path
            : std::filesystem::path{declared_in->to_string()}};

    std::unordered_map<sourcemeta::core::JSON::String,
                       sourcemeta::core::JSON::String>
        result;
    result.reserve(collection->resolve.size());
    for (const auto &pair : collection->resolve) {
      const sourcemeta::core::URI value{pair.second};
      if (!value.is_relative()) {
        result.emplace(pair.first, pair.second);
        continue;
      }

      const auto target{sourcemeta::core::weakly_canonical(
          collection->base_path / value.to_path())};
      if (!std::filesystem::exists(target)) {
        throw ResolverUnknownTargetError(declared_path, pair.first, target);
      }

      if (!std::filesystem::is_regular_file(target)) {
        throw ResolverTargetNotAFileError(declared_path, pair.first, target);
      }

      // Only a collection that would actually index the target can lend it a
      // URI, so a collection that merely contains it does not count. Two
      // collections may overlap on disk while only one of them takes the file
      const Configuration::Collection *owner{nullptr};
      const std::filesystem::path *owner_location{nullptr};
      for (const auto &candidate : configuration.entries) {
        const auto *other{
            std::get_if<Configuration::Collection>(&candidate.second)};
        if (other == nullptr || !serves(*other, target)) {
          continue;
        }

        if (owner != nullptr) {
          throw ResolverAmbiguousTargetError(declared_path, pair.first, target);
        }

        owner = other;
        owner_location = &candidate.first;
      }

      if (owner == nullptr) {
        throw ResolverUnmountedTargetError(declared_path, pair.first, target);
      }

      // A boolean schema is never served under an identity of its own, so
      // neither of these can be routed to
      auto contents{sourcemeta::core::read_yaml_or_json(target)};
      if (!contents.is_object()) {
        throw ResolverTargetNotASchemaError(declared_path, pair.first, target);
      }

      result.emplace(
          pair.first,
          sourcemeta::core::openapi_is_document(contents)
              ? openapi_identity(*owner, *owner_location, target, contents,
                                 server)
                    .served
              : schema_identity(*owner, *owner_location, target, contents,
                                sourcemeta::core::schema_resolver, server)
                    .served);
    }

    collection->resolve = std::move(result);
  }
}

Resolver::Resolver(const std::string_view url)
    : server_url_{url}, server_uri_{std::string{url}} {}

auto Resolver::operator()(const Configuration::Collection &collection,
                          const std::string_view identifier,
                          const Callback &callback) const
    -> std::optional<sourcemeta::core::JSON> {
  const auto rewritten{pre_resolve(collection, identifier, this->server_uri_)};
  if (rewritten.has_value()) {
    return this->operator()(rewritten.value(), callback);
  }

  return this->operator()(identifier, callback);
}

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

  // When resolving a schema, the resolver needs to determine its dialect in
  // order to normalise references. This involves resolving the schema's
  // metaschema, which may itself be a schema managed by this resolver. That
  // triggers a re-entrant resolution call, and if the metaschema chain is
  // circular, the resolver recurses infinitely. This set tracks which
  // identifiers are currently being resolved so we can detect the cycle and
  // return nullopt instead, letting the caller handle the missing metaschema.
  thread_local std::unordered_set<std::string> resolving;
  if (!resolving.emplace(identifier).second) {
    return std::nullopt;
  }

  struct ResolveGuard {
    std::string uri;
    ~ResolveGuard() { resolving.erase(uri); }
  } resolve_guard{std::string{identifier}};

  // The cached materialisation path is the only part of an entry that is
  // mutated after registration, and those writes happen concurrently with
  // resolution, so it must be snapshotted under the shared lock. The rest
  // of the entry is immutable by the time concurrent resolution starts, so
  // it can be safely referenced after releasing the lock. Note that growing
  // an unordered container invalidates iterators but never pointers or
  // references to its elements, and entries are never erased, so the
  // addresses captured here outlive concurrent registrations
  const Entry *view{nullptr};
  const sourcemeta::core::JSON::String *new_identifier{nullptr};
  std::optional<std::filesystem::path> cached_path;
  {
    std::shared_lock lock{this->mutex_};
    const auto result{this->views_.find(identifier)};
    if (result != this->views_.cend()) {
      view = &result->second;
      new_identifier = &result->first;
      cached_path = result->second.cache_path;
    }
  }

  // This is the schema half of the resolver, so an OpenAPI description is
  // nothing it answers for. A catalog schema referencing into one therefore
  // fails the way a reference to nothing fails, naming the file that spelled it
  if (view != nullptr && view->kind != LeafKind::Schema) {
    return std::nullopt;
  }

  // If we don't recognise the schema, try a fallback as a last resort
  if (view == nullptr) {
    auto fallback{sourcemeta::core::schema_resolver(identifier)};
    if (!fallback.has_value()) {
      return std::nullopt;
    }

    return std::move(fallback).to_owned();
  }

  auto cached{this->cached_dialect(identifier)};
  if (cached.has_value()) {
    if (callback) {
      callback(cached_path.has_value() ? cached_path.value() : view->path);
    }

    return cached;
  }

  /////////////////////////////////////////////////////////////////////////////
  // (2) Avoid rebasing on the fly if possible
  /////////////////////////////////////////////////////////////////////////////

  if (cached_path.has_value()) {
    // We can guarantee the cached outcome is JSON, so we don't need to try
    // reading as YAML
    auto schema_option{
        sourcemeta::core::metapack_read_json(cached_path.value())};
    assert(schema_option.has_value());
    auto schema{std::move(schema_option.value())};
    assert(schema.is_object() || schema.is_boolean());
    if (callback) {
      callback(cached_path.value());
    }

    this->cache_dialect(identifier, schema);
    return schema;
  }

  /////////////////////////////////////////////////////////////////////////////
  // (3) Read the original schema file
  /////////////////////////////////////////////////////////////////////////////

  auto schema{sourcemeta::core::read_yaml_or_json(view->path)};
  assert(schema.is_object() || schema.is_boolean());
  if (callback) {
    callback(view->path);
  }

  // If the schema is not an object schema, then we are done
  if (!schema.is_object()) {
    return schema;
  }

  /////////////////////////////////////////////////////////////////////////////
  // (4) Stop the schema from declaring a dialect, so that the one this
  // instance serves it as is named from the outside
  /////////////////////////////////////////////////////////////////////////////

  // What the schema calls its dialect was already settled at registration,
  // where it was also rebased, so the entry rather than the document is what
  // every step below reads. A dialect that reserves no keyword to declare it
  // cannot be analysed with one written in at all
  schema.erase("$schema");

  /////////////////////////////////////////////////////////////////////////////
  // (5) Normalise all references, if any, to match the new identifier
  /////////////////////////////////////////////////////////////////////////////

  const sourcemeta::core::SchemaResolver subresolver{
      [this](
          const auto subidentifier) -> std::optional<sourcemeta::core::JSON> {
        return this->operator()(subidentifier);
      }};

  const sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::Locations,
      schema,
      sourcemeta::core::schema_walker,
      subresolver,
      view->dialect,
      view->original_identifier,
      sourcemeta::core::SchemaFrame::IdentifierMode::Fallback};

  const auto ref_hash{schema.as_object().hash("$ref")};
  const auto dynamic_ref_hash{schema.as_object().hash("$dynamicRef")};
  frame.for_each_subschema([this, &schema, &view, ref_hash,
                            dynamic_ref_hash](const auto &location) -> void {
    auto &subschema{sourcemeta::core::get(schema, location.pointer)};
    if (!subschema.is_object()) {
      return;
    }

    const auto maybe_ref{subschema.try_at("$ref", ref_hash)};
    const auto maybe_dynamic_ref{
        location.base_dialect ==
                sourcemeta::core::SchemaBaseDialect::JSON_SCHEMA_2020_12
            ? subschema.try_at("$dynamicRef", dynamic_ref_hash)
            : nullptr};
    const auto has_ref{maybe_ref && maybe_ref->is_string()};
    const auto has_dynamic_ref{maybe_dynamic_ref &&
                               maybe_dynamic_ref->is_string()};
    if (!has_ref && !has_dynamic_ref) {
      return;
    }

    const sourcemeta::core::URI subschema_base{std::string{location.base}};
    if (has_ref) {
      normalise_ref(*view->collection, subschema_base, subschema, "$ref",
                    maybe_ref->to_string(), this->server_uri_);
    }
    if (has_dynamic_ref) {
      normalise_ref(*view->collection, subschema_base, subschema, "$dynamicRef",
                    maybe_dynamic_ref->to_string(), this->server_uri_);
    }
  });

  /////////////////////////////////////////////////////////////////////////////
  // (6) Declare the dialect and the new final identifier, as far as the
  // dialect in force reserves the keywords to do so
  /////////////////////////////////////////////////////////////////////////////

  const auto &root_location{frame.root_location().value().get()};
  const auto &root_vocabularies{frame.vocabularies(root_location, subresolver)};

  if (dialect_defines(root_vocabularies, "$schema")) {
    schema.assign("$schema", sourcemeta::core::JSON{view->dialect});
  }

  if (dialect_defines(root_vocabularies,
                      sourcemeta::core::schema_identifier_keyword(
                          root_location.base_dialect))) {
    sourcemeta::core::schema_reidentify(schema, *new_identifier,
                                        sourcemeta::core::schema_walker,
                                        subresolver, view->dialect);
  }

  this->cache_dialect(identifier, schema);
  return schema;
}

// What an OpenAPI description reaches for and does not hold, as the document
// spells it and where it spells it, with nothing named at all where the frame
// measures it incomplete without saying which one place makes it so. The
// document is where the reference lands once the document is served, ahead of
// any fragment, which is the served identity itself for one that names a
// position of the document it already sits in
struct OpenAPIDanglingConnection {
  sourcemeta::core::JSON::String reference;
  sourcemeta::core::JSON::String document;
  sourcemeta::core::Pointer location;
};

// What a destination names ahead of its fragment, which a reference carries as
// the two sides of one string
static auto
destination_document(const sourcemeta::core::JSON::StringView destination)
    -> sourcemeta::core::JSON::String {
  const auto fragment{destination.find('#')};
  return sourcemeta::core::JSON::String{
      fragment == sourcemeta::core::JSON::StringView::npos
          ? destination
          : destination.substr(0, fragment)};
}

// What the references of one OpenAPI description are measured against, which
// is the collection that spells them, the identity the document came with and
// the identity this instance serves it as
struct OpenAPIReferenceBase {
  const sourcemeta::one::Configuration::Collection &collection;
  const sourcemeta::core::URI &original;
  const sourcemeta::core::URI &served;
};

// Where a reference of an OpenAPI description lands once the document is
// served, which is the reference as the rewrite will write it resolved against
// the identity this instance serves the document under.
//
// This reads what the author wrote rather than what the frame resolved,
// because a frame measures a reference against the identity the document came
// with. One written against the root of this instance therefore resolves, in
// the frame, to a place that never meant anything, while the rewrite leaves it
// alone and a reader of the served document lands where the author meant
static auto reference_lands_at(const OpenAPIReferenceBase &identity,
                               const sourcemeta::core::URI &server,
                               const sourcemeta::core::JSON::String &reference)
    -> sourcemeta::core::JSON::String {
  const auto rewritten{rebase_reference(identity.collection, identity.original,
                                        reference, server)};
  sourcemeta::core::URI target{rewritten.value_or(reference)};
  if (target.is_relative()) {
    target.resolve_from(identity.served);
  }

  return destination_document(target.recompose());
}

// Whether a connection of an OpenAPI description reaches another document that
// this instance serves, which is what lets one built out of several be served
// as the documents it is made of. A connection landing back on the document
// that spells it reaches no other document at all, so one of those is never
// reaching anything
static auto reaches_served_document(const sourcemeta::one::Resolver &resolver,
                                    const sourcemeta::core::URI &served,
                                    const OpenAPIDanglingConnection &connection)
    -> bool {
  return !connection.document.empty() &&
         normalise_identifier(connection.document) !=
             normalise_identifier(served.recompose()) &&
         resolver.serves_identity(connection.document);
}

// The first connection an OpenAPI description makes that reaches no document
// this catalog holds, which is the one thing that stops one built out of
// several from being served. Nothing at all where every connection of it
// reaches a document the catalog holds, and a connection naming nothing where
// the frame measures the description incomplete without saying which place
// makes it so. A frame settles every connection of its own ahead of what the
// Schema Objects make, and reports each one as landing or not, so this reads
// them in that same order
static auto
first_unreached_connection(const sourcemeta::one::Resolver &resolver,
                           const OpenAPIReferenceBase &identity,
                           const sourcemeta::core::URI &server,
                           const sourcemeta::core::OpenAPIFrame &frame,
                           const sourcemeta::core::JSON &document)
    -> std::optional<OpenAPIDanglingConnection> {
  auto held{false};
  for (const auto &entry : frame.references()) {
    if (!entry.second.dangling) {
      continue;
    }

    const OpenAPIDanglingConnection connection{
        .reference = entry.second.original,
        .document = reference_lands_at(identity, server, entry.second.original),
        .location = entry.second.origin};
    if (!reaches_served_document(resolver, identity.served, connection)) {
      return connection;
    }

    held = true;
  }

  for (const auto &entry : frame.security_references()) {
    if (!entry.second.dangling) {
      continue;
    }

    const OpenAPIDanglingConnection connection{
        .reference = entry.second.original,
        .document = reference_lands_at(identity, server, entry.second.original),
        .location = entry.second.origin};
    if (!reaches_served_document(resolver, identity.served, connection)) {
      return connection;
    }

    held = true;
  }

  // A Discriminator Object names a schema by the value of a member rather than
  // by a field of its own, so what the author wrote is read off the document
  for (const auto &discriminator : frame.discriminators()) {
    if (!discriminator.dangling) {
      continue;
    }

    const auto &mapping{sourcemeta::core::get(document, discriminator.origin)};
    const sourcemeta::core::JSON::String written{
        mapping.is_string() ? mapping.to_string() : discriminator.destination};
    const OpenAPIDanglingConnection connection{
        .reference = written,
        .document = reference_lands_at(identity, server, written),
        .location = discriminator.origin};
    if (!reaches_served_document(resolver, identity.served, connection)) {
      return connection;
    }

    held = true;
  }

  std::optional<OpenAPIDanglingConnection> result;
  frame.schemas().for_each_unresolved_reference(
      [&resolver, &identity, &server, &result,
       &held](const auto &pointer, const auto &reference) -> void {
        // Naming a dialect this build already comes with asks nothing of
        // whoever reads the OpenAPI description, so it is the one reference
        // out of a Schema Object that leaves one whole
        if (result.has_value() ||
            (pointer.back().is_property() &&
             pointer.back().to_property() == "$schema" &&
             sourcemeta::core::schema_is_known(reference.destination))) {
          return;
        }

        const sourcemeta::core::JSON::String written{reference.original};
        const OpenAPIDanglingConnection connection{
            .reference = written,
            .document = reference_lands_at(identity, server, written),
            .location = sourcemeta::core::to_pointer(pointer)};
        if (!reaches_served_document(resolver, identity.served, connection)) {
          result = connection;
          return;
        }

        held = true;
      });

  if (result.has_value()) {
    return result;
  }

  // A frame reporting an incomplete OpenAPI description that this walk finds
  // every connection of is a frame reading something this walk cannot see, so
  // the refusal stands and names nothing
  return held ? std::nullopt
              : std::optional<OpenAPIDanglingConnection>{std::in_place};
}

auto Resolver::openapi(const std::string_view raw_identifier,
                       const Callback &callback) const
    -> std::optional<sourcemeta::core::JSON> {
  const auto identifier{normalise_identifier(raw_identifier)};

  const Entry *view{nullptr};
  const sourcemeta::core::JSON::String *new_identifier{nullptr};
  std::optional<std::filesystem::path> cached_path;
  {
    std::shared_lock lock{this->mutex_};
    const auto result{this->views_.find(identifier)};
    if (result == this->views_.cend()) {
      return std::nullopt;
    }

    view = &result->second;
    new_identifier = &result->first;
    cached_path = result->second.cache_path;
  }

  // A Schema Object of an OpenAPI description may reach a schema of this
  // catalog, and what comes back then is a schema rather than a description,
  // which is the one case where the two halves of the resolver meet
  if (view->kind != LeafKind::OpenAPI) {
    return this->operator()(identifier, callback);
  }

  /////////////////////////////////////////////////////////////////////////////
  // (1) Avoid rebasing on the fly if possible
  /////////////////////////////////////////////////////////////////////////////

  if (cached_path.has_value()) {
    auto document_option{
        sourcemeta::core::metapack_read_json(cached_path.value())};
    assert(document_option.has_value());
    if (callback) {
      callback(cached_path.value());
    }

    return document_option;
  }

  /////////////////////////////////////////////////////////////////////////////
  // (2) Read the original description file
  /////////////////////////////////////////////////////////////////////////////

  auto document{sourcemeta::core::read_yaml_or_json(view->path)};
  assert(document.is_object());
  if (callback) {
    callback(view->path);
  }

  const sourcemeta::core::SchemaResolver subresolver{
      [this, &callback](
          const auto subidentifier) -> std::optional<sourcemeta::core::JSON> {
        return this->operator()(subidentifier, callback);
      }};

  /////////////////////////////////////////////////////////////////////////////
  // (3) Declare what the OpenAPI description calls itself, where the revision
  // reserves a field for it, so that the base every reference below is measured
  // against is the one the document came with and so that the formatter ranks
  // the field where the specification puts it. What this instance serves the
  // OpenAPI description as is written into the same field once the frame has
  // been read
  /////////////////////////////////////////////////////////////////////////////

  const auto declares_identity{
      sourcemeta::core::openapi_version(document).value() >=
      sourcemeta::core::OpenAPIVersion::OPENAPI_3_2};
  if (declares_identity) {
    document.assign("$self", sourcemeta::core::JSON{view->original_identifier});
  }

  const sourcemeta::core::OpenAPIFrame frame{
      document, sourcemeta::core::schema_walker, subresolver,
      view->original_identifier};

  const sourcemeta::core::URI base{view->original_identifier};
  const sourcemeta::core::URI served{*new_identifier};
  const auto &collection{*view->collection};
  const OpenAPIReferenceBase identity{
      .collection = collection, .original = base, .served = served};

  // An OpenAPI description may be built out of several documents, so reaching
  // beyond this one is only an error where what it reaches is not something
  // this instance serves. A reference naming a position of the OpenAPI
  // description itself reaches the document it already sits in
  if (!frame.standalone()) {
    auto unreached{first_unreached_connection(
        *this, identity, this->server_uri_, frame, document)};
    if (unreached.has_value()) {
      throw ResolverUnresolvableOpenAPIReferenceError(
          view->path, std::move(unreached->reference),
          std::move(unreached->location));
    }
  }

  /////////////////////////////////////////////////////////////////////////////
  // (4) Normalise every reference to match the new identifier
  /////////////////////////////////////////////////////////////////////////////

  // A Security Requirement Object names a scheme by the key of a member rather
  // than by its value, so this is the one rewrite that moves a member
  frame.for_each_security_reference([this, &collection, &base,
                                     &document](const auto &,
                                                const auto &reference) -> void {
    const auto rewritten{rebase_reference(collection, base, reference.original,
                                          this->server_uri_)};
    if (!rewritten.has_value() || rewritten.value() == reference.original) {
      return;
    }

    auto &holder{sourcemeta::core::get(document, reference.origin.initial())};
    auto scopes{holder.at(reference.original)};
    holder.erase(reference.original);
    holder.assign(rewritten.value(), std::move(scopes));
  });

  frame.for_each_reference([this, &collection, &base, &document](
                               const auto &, const auto &reference) -> void {
    const auto rewritten{rebase_reference(collection, base, reference.original,
                                          this->server_uri_)};
    if (rewritten.has_value()) {
      sourcemeta::core::get(document, reference.origin) =
          sourcemeta::core::JSON{rewritten.value()};
    }
  });

  const auto ref_hash{document.as_object().hash("$ref")};
  const auto dynamic_ref_hash{document.as_object().hash("$dynamicRef")};
  frame.schemas().for_each_subschema(
      [this, &collection, &document, ref_hash,
       dynamic_ref_hash](const auto &location) -> void {
        auto &subschema{sourcemeta::core::get(document, location.pointer)};
        if (!subschema.is_object()) {
          return;
        }

        const auto maybe_ref{subschema.try_at("$ref", ref_hash)};
        const auto maybe_dynamic_ref{
            location.base_dialect ==
                    sourcemeta::core::SchemaBaseDialect::JSON_SCHEMA_2020_12
                ? subschema.try_at("$dynamicRef", dynamic_ref_hash)
                : nullptr};
        const auto has_ref{maybe_ref && maybe_ref->is_string()};
        const auto has_dynamic_ref{maybe_dynamic_ref &&
                                   maybe_dynamic_ref->is_string()};
        if (!has_ref && !has_dynamic_ref) {
          return;
        }

        const sourcemeta::core::URI subschema_base{std::string{location.base}};
        if (has_ref) {
          normalise_ref(collection, subschema_base, subschema, "$ref",
                        maybe_ref->to_string(), this->server_uri_);
        }
        if (has_dynamic_ref) {
          normalise_ref(collection, subschema_base, subschema, "$dynamicRef",
                        maybe_dynamic_ref->to_string(), this->server_uri_);
        }
      });

  /////////////////////////////////////////////////////////////////////////////
  // (5) Declare the dialect the Schema Objects are held to, as far as the
  // revision reserves the field to do so
  /////////////////////////////////////////////////////////////////////////////

  if (!view->dialect.empty()) {
    document.assign("jsonSchemaDialect", sourcemeta::core::JSON{view->dialect});
  }

  /////////////////////////////////////////////////////////////////////////////
  // (6) Settle the order of the document, and only then the identity, as
  // formatting reads the frame and the frame points into the document
  /////////////////////////////////////////////////////////////////////////////

  sourcemeta::core::openapi_format(document, frame);

  if (declares_identity) {
    document.assign("$self", sourcemeta::core::JSON{*new_identifier});
  }

  return document;
}

auto Resolver::track_dialect(const sourcemeta::core::JSON::String &dialect)
    -> void {
  // A leaf whose dialect is fixed by something other than a field of its own
  // declares none here, and nothing is tracked for a document that names
  // nothing
  if (dialect.empty()) {
    return;
  }

  // Official dialects are resolved through the static fallback resolver
  // rather than through this resolver, so we never get the chance to cache
  // them anyway
  if (sourcemeta::core::schema_is_known(dialect)) {
    return;
  }

  std::unique_lock lock{this->dialect_mutex_};
  const auto match{std::ranges::find_if(this->dialects_,
                                        [&dialect](const auto &entry) -> bool {
                                          return entry.first == dialect;
                                        })};
  if (match == this->dialects_.cend()) {
    this->dialects_.emplace_back(dialect, std::nullopt);
  }
}

auto Resolver::is_dialect(const std::string_view uri) const -> bool {
  const std::shared_lock lock{this->dialect_mutex_};
  return std::ranges::any_of(
      this->dialects_,
      [&uri](const auto &entry) -> bool { return entry.first == uri; });
}

auto Resolver::cache_dialect(const sourcemeta::core::JSON::String &uri,
                             const sourcemeta::core::JSON &schema) const
    -> void {
  // Most schemas are not dialects, so we first check whether there is
  // anything to do while only holding the shared lock
  {
    std::shared_lock lock{this->dialect_mutex_};
    const auto match{std::ranges::find_if(
        this->dialects_,
        [&uri](const auto &entry) -> bool { return entry.first == uri; })};
    if (match == this->dialects_.cend() || match->second.has_value()) {
      return;
    }
  }

  std::unique_lock lock{this->dialect_mutex_};
  const auto match{
      std::ranges::find_if(this->dialects_, [&uri](const auto &entry) -> bool {
        return entry.first == uri;
      })};
  if (match != this->dialects_.cend() && !match->second.has_value()) {
    match->second = schema;
  }
}

auto Resolver::cached_dialect(const sourcemeta::core::JSON::String &uri) const
    -> std::optional<sourcemeta::core::JSON> {
  std::shared_lock lock{this->dialect_mutex_};
  const auto match{
      std::ranges::find_if(this->dialects_, [&uri](const auto &entry) -> bool {
        return entry.first == uri;
      })};
  if (match != this->dialects_.cend() && match->second.has_value()) {
    return match->second;
  }

  return std::nullopt;
}

auto Resolver::add(const std::filesystem::path &collection_relative_path,
                   const Configuration::Collection &collection,
                   const std::filesystem::path &path,
                   const std::filesystem::file_time_type mtime) -> Result {
  /////////////////////////////////////////////////////////////////////////////
  // (1) Read the schema file
  /////////////////////////////////////////////////////////////////////////////
  assert(path.is_absolute());
  try {
    const auto schema{sourcemeta::core::read_yaml_or_json(path)};
    if (schema.is_boolean()) {
      throw ResolverBooleanSchemaError(path);
    }

    if (!schema.is_object()) {
      throw ResolverNotASchemaError(path);
    }

    if (sourcemeta::core::openapi_is_document(schema)) {
      return this->add_openapi(collection_relative_path, collection, path,
                               mtime, schema);
    }

    const std::string default_dialect_str{
        collection.default_dialect.value_or("")};

    /////////////////////////////////////////////////////////////////////////////
    // (2) and (3) Determine what the schema calls itself and what this instance
    // serves it as
    /////////////////////////////////////////////////////////////////////////////
    const auto identity{schema_identity(
        collection, collection_relative_path, path, schema,
        [this, &collection](
            const auto subidentifier) -> std::optional<sourcemeta::core::JSON> {
          return this->operator()(collection, subidentifier);
        },
        this->server_uri_)};
    const auto &identifier{identity.current};
    const auto &new_identifier{identity.served};

    /////////////////////////////////////////////////////////////////////////////
    // (4) Determine the dialect of the schema, which we also need to make sure
    // we rebase according to the one base URI, etc
    /////////////////////////////////////////////////////////////////////////////
    const auto *declared_dialect{schema.is_object() ? schema.try_at("$schema")
                                                    : nullptr};
    std::string raw_dialect{declared_dialect != nullptr &&
                                    declared_dialect->is_string()
                                ? declared_dialect->to_string()
                                : default_dialect_str};
    if (raw_dialect.empty()) {
      throw sourcemeta::core::SchemaUnknownDialectError();
    }
    auto current_dialect{served_dialect(collection, collection_relative_path,
                                        raw_dialect, this->server_uri_)};
    // Otherwise we messed things up
    assert(!current_dialect.ends_with("#.json"));

    /////////////////////////////////////////////////////////////////////////////
    // (5) Safely one the schema entry in the resolver
    /////////////////////////////////////////////////////////////////////////////

    const auto *vocabularies{schema.is_object() ? schema.try_at("$vocabulary")
                                                : nullptr};
    const auto declares_vocabularies{vocabularies != nullptr &&
                                     vocabularies->is_object()};

    const auto evaluate{Configuration::should_evaluate(collection)};

    std::unique_lock lock{this->mutex_};
    auto result{this->views_.emplace(
        new_identifier,
        Entry{.path = path,
              .relative_path = sourcemeta::core::URI{new_identifier}
                                   .relative_to(this->server_uri_)
                                   .recompose(),
              .mtime = mtime,
              .evaluate = evaluate,
              .cache_path = std::nullopt,
              .dialect = std::move(current_dialect),
              .vocabularies = declares_vocabularies,
              .original_identifier = identifier,
              .collection = &collection,
              .kind = LeafKind::Schema})};
    lock.unlock();
    if (!result.second && result.first->second.path != path) {
      throw sourcemeta::core::FileError<sourcemeta::core::SchemaFrameError>(
          path,
          sourcemeta::core::StaticString{
              "Cannot register the same identifier twice"},
          result.first->first);
    }
    this->track_dialect(result.first->second.dialect);
    return {result.first->first, result.first->second};
  } catch (const sourcemeta::core::SchemaKeywordError &error) {
    throw sourcemeta::core::FileError<sourcemeta::core::SchemaKeywordError>(
        path, error);
  } catch (const sourcemeta::core::SchemaUnknownDialectError &) {
    throw sourcemeta::core::FileError<
        sourcemeta::core::SchemaUnknownDialectError>(path);
  } catch (const sourcemeta::core::URIParseError &) {
    const auto reread{sourcemeta::core::read_yaml_or_json(path)};
    const auto &id_keyword{reread.defines("$id") ? "$id" : "id"};
    std::ostringstream value_stream;
    sourcemeta::core::stringify(reread.at(id_keyword), value_stream);
    throw sourcemeta::core::FileError<sourcemeta::core::SchemaKeywordError>(
        path,
        sourcemeta::core::StaticString{
            "The schema identifier is not a valid URI"},
        id_keyword, value_stream.str());
  } catch (const sourcemeta::core::YAMLParseError &error) {
    throw sourcemeta::core::FileError<sourcemeta::core::YAMLParseError>(path,
                                                                        error);
  }
}

auto Resolver::add_openapi(
    const std::filesystem::path &collection_relative_path,
    const Configuration::Collection &collection,
    const std::filesystem::path &path,
    const std::filesystem::file_time_type mtime,
    const sourcemeta::core::JSON &document) -> Result {
  // A revision this build cannot read is still an OpenAPI description, so
  // quoting back what the file named is the only thing to do with it
  if (!sourcemeta::core::openapi_version(document).has_value()) {
    throw ResolverUnsupportedOpenAPIVersionError(
        path, sourcemeta::core::JSON::String{
                  sourcemeta::core::openapi_version_string(document).value()});
  }

  const auto identity{openapi_identity(collection, collection_relative_path,
                                       path, document, this->server_uri_)};

  // The collection's default dialect plays no part here, as the dialect of a
  // Schema Object is fixed by the revision with this field as the only
  // override. A revision that reserves no such field declares nothing
  const auto *declared_dialect{document.try_at("jsonSchemaDialect")};
  auto dialect{declared_dialect != nullptr && declared_dialect->is_string()
                   ? served_dialect(collection, collection_relative_path,
                                    declared_dialect->to_string(),
                                    this->server_uri_)
                   : std::string{}};

  std::unique_lock lock{this->mutex_};
  auto result{this->views_.emplace(
      identity.served,
      Entry{.path = path,
            .relative_path = sourcemeta::core::URI{identity.served}
                                 .relative_to(this->server_uri_)
                                 .recompose(),
            .mtime = mtime,
            .evaluate = Configuration::should_evaluate(collection),
            .cache_path = std::nullopt,
            .dialect = std::move(dialect),
            .vocabularies = false,
            .original_identifier = identity.current,
            .collection = &collection,
            .kind = LeafKind::OpenAPI})};
  lock.unlock();
  if (!result.second && result.first->second.path != path) {
    throw sourcemeta::core::FileError<sourcemeta::core::SchemaFrameError>(
        path,
        sourcemeta::core::StaticString{
            "Cannot register the same identifier twice"},
        result.first->first);
  }

  this->track_dialect(result.first->second.dialect);
  return {result.first->first, result.first->second};
}

auto Resolver::emplace(std::string new_identifier, Entry entry) -> void {
  assert(std::filesystem::exists(entry.path));
  assert(entry.cache_path.has_value());
  // A cache hit means a finished build wrote this artifact, so it being gone
  // means the output directory was modified from outside. That violates the
  // one assumption the cache rests on, and it has to fail the same way in
  // every build type rather than trip an assertion only where those exist.
  // The check must not itself throw, since a path the build cannot examine,
  // whatever the reason, is equally a record it cannot honour
  std::error_code existence_error;
  if (!std::filesystem::exists(entry.cache_path.value(), existence_error)) {
    throw ResolverMissingCachedArtifactError{entry.cache_path.value()};
  }

  assert(entry.collection);
  const auto path{entry.path};
  auto result{
      this->views_.emplace(std::move(new_identifier), std::move(entry))};
  if (!result.second && result.first->second.path != path) {
    throw sourcemeta::core::SchemaFrameError(
        "Cannot register the same identifier twice", result.first->first);
  }
  this->track_dialect(result.first->second.dialect);
}

auto Resolver::entry(const std::string_view identifier) const -> const Entry & {
  const auto result{this->views_.find(std::string{identifier})};
  assert(result != this->views_.cend());
  // An OpenAPI description whose revision reserves no field to declare a
  // dialect, or that declares none, carries one nowhere
  assert(result->second.kind == LeafKind::OpenAPI ||
         !result->second.dialect.empty());
  assert(!result->second.original_identifier.empty());
  assert(result->second.collection != nullptr);
  return result->second;
}

auto Resolver::serves_identity(
    const sourcemeta::core::JSON::String &identifier) const -> bool {
  std::shared_lock lock{this->mutex_};
  return this->views_.contains(normalise_identifier(identifier));
}

auto Resolver::dialect_of(const std::string_view identifier) const
    -> std::string_view {
  const auto result{this->views_.find(std::string{identifier})};
  return result == this->views_.cend()
             ? std::string_view{}
             : std::string_view{result->second.dialect};
}

auto Resolver::cache_path(const std::string_view uri,
                          const std::filesystem::path &path) -> void {
  assert(std::filesystem::exists(path));
  // As we are modifying the actual map
  std::unique_lock lock{this->mutex_};
  auto entry{this->views_.find(std::string{uri})};
  assert(entry != this->views_.cend());
  // A leaf whose selection changed is planned again without its own file
  // changing, and what it materialises into does not move
  assert(!entry->second.cache_path.has_value() ||
         entry->second.cache_path.value() == path);
  entry->second.cache_path = path;
}

} // namespace sourcemeta::one
