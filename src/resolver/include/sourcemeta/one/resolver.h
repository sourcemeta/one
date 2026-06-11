#ifndef SOURCEMETA_ONE_RESOLVER_H_
#define SOURCEMETA_ONE_RESOLVER_H_

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/uri.h>

#include <sourcemeta/one/configuration.h>

#include <sourcemeta/one/resolver_error.h>

#include <atomic>     // std::atomic
#include <cstddef>    // std::size_t
#include <deque>      // std::deque
#include <filesystem> // std::filesystem::path, std::filesystem::file_time_type
#include <functional> // std::function, std::reference_wrapper
#include <mutex>      // std::mutex
#include <optional>   // std::optional
#include <shared_mutex>  // std::shared_mutex
#include <string_view>   // std::string_view
#include <unordered_map> // std::unordered_map
#include <utility>       // std::pair

namespace sourcemeta::one {

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
    sourcemeta::core::JSON::String original_identifier{};
    const Configuration::Collection *collection{nullptr};
  };

  using Result =
      std::pair<std::reference_wrapper<const sourcemeta::core::JSON::String>,
                std::reference_wrapper<const Entry>>;

  auto add(const std::filesystem::path &collection_relative_path,
           const Configuration::Collection &collection,
           const std::filesystem::path &path,
           const std::filesystem::file_time_type mtime) -> Result;

  auto emplace(std::string new_identifier, Entry entry) -> void;

  using Views = std::unordered_map<sourcemeta::core::JSON::String, Entry>;

  auto reserve(std::size_t count) -> void { this->views.reserve(count); }

  [[nodiscard]] auto begin() const -> auto { return this->views.begin(); }
  [[nodiscard]] auto end() const -> auto { return this->views.end(); }
  [[nodiscard]] auto size() const -> auto { return this->views.size(); }
  [[nodiscard]] auto data() const -> const Views & { return this->views; }

  [[nodiscard]] auto entry(std::string_view identifier) const -> const Entry &;

private:
  Views views;
  std::shared_mutex mutex;
  std::string_view server_url;
  sourcemeta::core::URI server_uri;

  // Schemas that act as meta-schemas of other schemas get resolved over
  // and over again during indexing (i.e. for every schema that uses them),
  // so we keep their materialised contents in memory. An entry whose `ready`
  // flag is not set means we know the schema is a non-official dialect but
  // didn't resolve it yet. As the number of distinct dialects in a registry
  // is expected to be tiny, we use a flat structure for fast lookups.
  // Lookups are lock-free: we rely on a deque (which never relocates its
  // elements), an atomic entry count published after every append, and a
  // per-entry atomic flag published after the contents are written. The
  // mutex only serializes writers
  struct DialectCacheEntry {
    sourcemeta::core::JSON::String uri;
    sourcemeta::core::JSON contents{nullptr};
    std::atomic<bool> ready{false};
  };
  auto track_dialect(const sourcemeta::core::JSON::String &dialect) -> void;
  auto cache_dialect(const sourcemeta::core::JSON::String &uri,
                     const sourcemeta::core::JSON &schema) const -> void;
  [[nodiscard]] auto
  cached_dialect(const sourcemeta::core::JSON::String &uri) const
      -> std::optional<sourcemeta::core::JSON>;
  mutable std::mutex dialect_mutex;
  mutable std::deque<DialectCacheEntry> dialects;
  mutable std::atomic<std::size_t> dialect_count{0};
};

} // namespace sourcemeta::one

#endif
