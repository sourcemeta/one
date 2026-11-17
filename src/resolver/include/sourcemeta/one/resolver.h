#ifndef SOURCEMETA_ONE_RESOLVER_H_
#define SOURCEMETA_ONE_RESOLVER_H_

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/uri.h>

#include <sourcemeta/one/configuration.h>
#include <sourcemeta/one/shared_leaf.h>

#include <sourcemeta/one/resolver_error.h>

#include <cstdint>    // std::uint8_t
#include <filesystem> // std::filesystem::path, std::filesystem::file_time_type
#include <functional> // std::function, std::reference_wrapper
#include <optional>   // std::optional
#include <shared_mutex>  // std::shared_mutex
#include <string_view>   // std::string_view
#include <unordered_map> // std::unordered_map
#include <utility>       // std::pair
#include <vector>        // std::vector

namespace sourcemeta::one {

// Which file each routed URI was settled on, for the targets a collection
// names by file. Settling happens before anything is indexed, so the dialect a
// target is written against is resolvable only if it is an official one, and
// what the registry ends up serving that target as is not yet known
using ResolveTargets =
    std::unordered_map<std::filesystem::path, sourcemeta::core::JSON::String>;

// A collection names the schemas it routes references to by file, the way the
// manifest format does everywhere else. Every such target is replaced here with
// the URI the registry serves that file under, before the catalog is indexed
auto rebase_resolve_targets(Configuration &configuration) -> ResolveTargets;

// What settling an identity does when the dialect a schema names is nothing
// this catalog can resolve yet. Registering a dialect is what makes it
// resolvable, so a schema naming one this catalog holds answers the question
// differently once that dialect is in, and taking the identifier its path
// implies before then is what makes the answer depend on the order files
// happened to arrive in. A caller that can register more schemas and ask again
// withholds that fallback, and allows it on a pass where nothing is left to
// register and the path is the only answer there is
enum class UnresolvedDialect : std::uint8_t { Withhold, Fallback };

class Resolver {
public:
  explicit Resolver(std::string_view server_url);

  // Just to prevent mistakes
  Resolver(const Resolver &) = delete;
  auto operator=(const Resolver &) -> Resolver & = delete;
  Resolver(Resolver &&) = delete;
  auto operator=(Resolver &&) -> Resolver & = delete;

  using Callback = std::function<void(const std::filesystem::path &)>;

  auto operator()(std::string_view identifier,
                  const Callback &callback = nullptr) const
      -> std::optional<sourcemeta::core::JSON>;

  // The same, for an identifier written the way a schema of this collection
  // writes it, so that whatever the collection routes elsewhere is honoured
  auto operator()(const Configuration::Collection &collection,
                  std::string_view identifier,
                  const Callback &callback = nullptr) const
      -> std::optional<sourcemeta::core::JSON>;

  // The OpenAPI half of the resolver. The two are kept apart because what a
  // reference expects to find at the far end is fixed by where it sits, so a
  // reference landing on the other kind is something the caller reports rather
  // than something resolution hides by answering nothing
  [[nodiscard]] auto openapi(std::string_view identifier,
                             const Callback &callback = nullptr) const
      -> std::optional<sourcemeta::core::JSON>;

  auto cache_path(std::string_view uri, const std::filesystem::path &path)
      -> void;

  struct Entry {
    std::filesystem::path path;
    // This is the collection name plus the final schema path component
    std::filesystem::path relative_path;
    std::filesystem::file_time_type mtime;
    bool evaluate{true};
    std::optional<std::filesystem::path> cache_path{};
    sourcemeta::core::JSON::String dialect{};
    // Whether the schema declares vocabularies of its own
    bool vocabularies{false};
    sourcemeta::core::JSON::String original_identifier{};
    const Configuration::Collection *collection{nullptr};
    // What this entry holds, which decides what the build writes for it
    LeafKind kind{LeafKind::Schema};
  };

  using Result =
      std::pair<std::reference_wrapper<const sourcemeta::core::JSON::String>,
                std::reference_wrapper<const Entry>>;

  // Throws sourcemeta::core::SchemaResolutionError without registering
  // anything when the schema names a dialect this catalog cannot resolve yet
  // and the caller withheld the fallback, so that the caller may register the
  // rest of the catalog and ask again. A caller with nothing left to register
  // has no second answer to wait for, which is why the fallback is what asking
  // once gets
  auto add(const std::filesystem::path &collection_relative_path,
           const Configuration::Collection &collection,
           const std::filesystem::path &path,
           const std::filesystem::file_time_type mtime,
           UnresolvedDialect unresolved_dialect = UnresolvedDialect::Fallback)
      -> Result;

  auto emplace(std::string new_identifier, Entry entry) -> void;

  // What the collections route by file, so that a target the registry ends up
  // serving under something other than the URI routing was settled on is
  // still reachable by that URI. Settled before anything is indexed, which is
  // why routing cannot be held to what a target turns out to declare, and why
  // this has to be told rather than worked out. Call before registering
  // anything, as nothing reads it under a lock
  auto track_resolve_targets(ResolveTargets targets) -> void;

  using Views = std::unordered_map<sourcemeta::core::JSON::String, Entry>;

  auto reserve(std::size_t count) -> void { this->views_.reserve(count); }

  [[nodiscard]] auto begin() const -> auto { return this->views_.begin(); }
  [[nodiscard]] auto end() const -> auto { return this->views_.end(); }
  [[nodiscard]] auto size() const -> auto { return this->views_.size(); }
  [[nodiscard]] auto data() const -> const Views & { return this->views_; }

  [[nodiscard]] auto entry(std::string_view identifier) const -> const Entry &;

  // The dialect this instance serves the given schema as, or nothing at all
  // for a schema it does not serve. A schema whose dialect reserves no keyword
  // to declare one carries it nowhere else
  [[nodiscard]] auto dialect_of(std::string_view identifier) const
      -> std::string_view;

  // Whether any schema in the catalog declares the given URI as its dialect
  [[nodiscard]] auto is_dialect(std::string_view uri) const -> bool;

  // Whether this instance serves a leaf under the given identity, which is the
  // one spelling a reference has to name to reach it
  [[nodiscard]] auto
  serves_identity(const sourcemeta::core::JSON::String &identifier) const
      -> bool;

private:
  auto add_openapi(const std::filesystem::path &collection_relative_path,
                   const Configuration::Collection &collection,
                   const std::filesystem::path &path,
                   std::filesystem::file_time_type mtime,
                   const sourcemeta::core::JSON &document) -> Result;

  // Note the URI a collection routes to this file, where it is not already
  // the one the file came to be served under
  auto alias_resolve_target(const std::filesystem::path &path,
                            const sourcemeta::core::JSON::String &served)
      -> void;

  Views views_;
  // Resolution is a const operation that runs concurrently with the
  // writers that commit cached materialisation paths, so readers must
  // be able to take the shared lock too
  mutable std::shared_mutex mutex_;
  std::string_view server_url_;
  sourcemeta::core::URI server_uri_;

  // Settled before anything is registered and never written again, so
  // resolution reads it without taking the lock that guards the views
  ResolveTargets resolve_targets_;
  // What routing was settled on, for the targets that came to be served
  // under something else. Guarded alongside the views, as registering a
  // leaf is what fills it
  std::unordered_map<sourcemeta::core::JSON::String,
                     sourcemeta::core::JSON::String>
      resolve_aliases_;

  // Schemas that act as meta-schemas of other schemas get resolved over
  // and over again during indexing (i.e. for every schema that uses them),
  // so we keep their materialised contents in memory. An entry without
  // contents means we know the schema is a non-official dialect but didn't
  // resolve it yet. As the number of distinct dialects in a registry is
  // expected to be tiny, we use a flat structure for fast lookups. Note
  // that readers must hold a shared lock for the entire scan: appends
  // mutate the container internals, so element address stability alone
  // would not make unlocked reads safe
  auto track_dialect(const sourcemeta::core::JSON::String &dialect) -> void;
  auto cache_dialect(const sourcemeta::core::JSON::String &uri,
                     const sourcemeta::core::JSON &schema) const -> void;
  [[nodiscard]] auto
  cached_dialect(const sourcemeta::core::JSON::String &uri) const
      -> std::optional<sourcemeta::core::JSON>;
  mutable std::shared_mutex dialect_mutex_;
  mutable std::vector<std::pair<sourcemeta::core::JSON::String,
                                std::optional<sourcemeta::core::JSON>>>
      dialects_;
};

} // namespace sourcemeta::one

#endif
