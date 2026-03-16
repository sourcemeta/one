#ifndef SOURCEMETA_ONE_RESOLVER_H_
#define SOURCEMETA_ONE_RESOLVER_H_

#include <sourcemeta/core/json.h>

#include <sourcemeta/one/configuration.h>

#include <sourcemeta/one/resolver_error.h>

#include <filesystem> // std::filesystem::path, std::filesystem::file_time_type
#include <functional> // std::function, std::reference_wrapper
#include <optional>   // std::optional
#include <shared_mutex>  // std::shared_mutex
#include <string_view>   // std::string_view
#include <unordered_map> // std::unordered_map

namespace sourcemeta::one {

class Resolver {
public:
  Resolver() = default;
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

  struct AddResult {
    std::reference_wrapper<const sourcemeta::core::JSON::String>
        original_identifier;
    std::reference_wrapper<const sourcemeta::core::JSON::String> new_identifier;
    std::reference_wrapper<const Entry> entry;
  };

  auto add(const sourcemeta::core::JSON::String &server_url,
           const std::filesystem::path &collection_relative_path,
           const Configuration::Collection &collection,
           const std::filesystem::path &path,
           std::filesystem::file_time_type mtime) -> AddResult;

  auto emplace(std::string new_identifier, Entry entry) -> void;
  auto emplace_unlocked(std::string new_identifier, Entry entry) -> void;

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
};

} // namespace sourcemeta::one

#endif
