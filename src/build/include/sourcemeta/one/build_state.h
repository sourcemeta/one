#ifndef SOURCEMETA_ONE_BUILD_STATE_H_
#define SOURCEMETA_ONE_BUILD_STATE_H_

#ifndef SOURCEMETA_ONE_BUILD_EXPORT
#include <sourcemeta/one/build_export.h>
#endif

#include <sourcemeta/core/io.h>

#include <array>       // std::array
#include <cstddef>     // std::size_t
#include <cstdint>     // std::uint8_t, std::uint16_t, std::uint32_t
#include <filesystem>  // std::filesystem::path, std::filesystem::file_time_type
#include <memory>      // std::unique_ptr
#include <mutex>       // std::mutex, std::unique_lock
#include <span>        // std::span
#include <string>      // std::string
#include <string_view> // std::string_view
#include <unordered_map> // std::unordered_map
#include <unordered_set> // std::unordered_set
#include <vector>        // std::vector

namespace sourcemeta::one {

struct BuildPlan {
  using Type = std::uint8_t;

  struct Action {
    using Type = std::uint8_t;
    using Dependencies = std::vector<std::filesystem::path>;

    Type type;
    std::filesystem::path destination;
    Dependencies dependencies;
    std::string_view data;
  };

  std::filesystem::path output;
  Type type;
  std::vector<std::vector<Action>> waves;
  std::size_t size{0};
};

enum class TargetGate : std::uint8_t { Always, OnlyInFullMode, IfEvaluate };

enum class DirtyOverride : std::uint8_t {
  Normal,
  ForceIfAffected,
  ForceOnGraphChange
};

enum class DependencySource : std::uint8_t {
  Base,
  GlobalOutput,
  ExternalSource,
  ExternalConfig
};

struct DependencyReference {
  DependencySource source;
  std::uint8_t base;
  const char *filename;
};

inline constexpr std::size_t MAX_DEPENDENCIES_PER_RULE = 4;

struct LeafRule {
  BuildPlan::Action::Type action;
  std::uint8_t base;
  const char *filename;
  TargetGate gate;
  DirtyOverride dirty;
  bool is_root;
  bool combine_only;
  bool container_target;
  bool tracks_dependencies;
  std::array<DependencyReference, MAX_DEPENDENCIES_PER_RULE> dependencies;
  std::uint8_t dependency_count;
};

enum class ContainerScope : std::uint8_t { AllContainers, NonRoot, RootOnly };

enum class ContainerDependencyKind : std::uint8_t {
  LeafMetadata,
  ChildContainers,
  AllContainerListings,
  SameContainerTarget,
  ExternalConfig,
  Global
};

struct ContainerDependency {
  ContainerDependencyKind kind;
  const char *filename;
};

inline constexpr std::size_t MAX_CONTAINER_DEPENDENCIES = 3;

struct ContainerRule {
  BuildPlan::Action::Type action;
  std::uint8_t base;
  const char *filename;
  TargetGate gate;
  ContainerScope scope;
  bool only_full_rebuild;
  bool is_listing;
  std::array<ContainerDependency, MAX_CONTAINER_DEPENDENCIES> dependencies;
  std::uint8_t dependency_count;
};

enum class GlobalTrigger : std::uint8_t {
  FullRebuild,
  WithVersion,
  WithComment,
  OnModeChange
};

struct GlobalRule {
  BuildPlan::Action::Type action;
  const char *filename;
  GlobalTrigger trigger;
  bool external_config_anchor;
  std::array<DependencyReference, MAX_DEPENDENCIES_PER_RULE> dependencies;
  std::uint8_t dependency_count;
};

class SOURCEMETA_ONE_BUILD_EXPORT BuildState {
public:
  struct Entry {
    std::filesystem::file_time_type file_mark;
    std::vector<std::filesystem::path> dependencies;
  };

  struct ResolverEntry {
    std::filesystem::file_time_type file_mark;
    std::string new_identifier;
    std::string original_identifier;
    std::string dialect;
    std::string relative_path;
  };

  BuildState() = default;
  ~BuildState() = default;
  BuildState(BuildState &&) = delete;
  auto operator=(BuildState &&) -> BuildState & = delete;
  BuildState(const BuildState &) = delete;
  auto operator=(const BuildState &) -> BuildState & = delete;

  [[nodiscard]] auto take_lock() const -> std::unique_lock<std::mutex>;

  auto configure(std::span<const LeafRule> leaf_rules,
                 std::uint32_t rules_fingerprint, std::string_view sentinel)
      -> void;
  auto load(const std::filesystem::path &path,
            std::span<const LeafRule> leaf_rules,
            std::uint32_t rules_fingerprint, std::string_view sentinel) -> void;
  auto save(const std::filesystem::path &path) const -> void;

  [[nodiscard]] auto empty() const -> bool { return this->entry_count == 0; }
  [[nodiscard]] auto size() const -> std::size_t { return this->entry_count; }

  [[nodiscard]] auto contains(const std::string &key) const -> bool;
  [[nodiscard]] auto entry(const std::string &key) const -> const Entry *;
  [[nodiscard]] auto
  is_stale(const std::string &key,
           std::filesystem::file_time_type source_mtime) const -> bool;
  auto commit(const std::filesystem::path &path,
              std::vector<std::filesystem::path> dependencies) -> void;
  auto commit(const std::string &source_path, ResolverEntry entry) -> void;
  auto forget(const std::string &key) -> void;
  auto emplace(const std::filesystem::path &path, Entry entry) -> void;
  [[nodiscard]] auto keys() const -> const std::vector<std::string_view> &;

  [[nodiscard]] auto resolve(const std::string &source_path,
                             std::filesystem::file_time_type mtime) const
      -> const ResolverEntry *;

  [[nodiscard]] auto in_overlay(const std::string &key) const -> bool;
  [[nodiscard]] auto disk_entry(const std::string &key) const -> const Entry *;
  [[nodiscard]] auto raw_disk_entry(const std::string &key) const
      -> const Entry *;

  struct TransparentHash {
    using is_transparent = void;
    auto operator()(std::string_view value) const noexcept -> std::size_t {
      return std::hash<std::string_view>{}(value);
    }
  };

  struct TransparentEqual {
    using is_transparent = void;
    auto operator()(std::string_view left,
                    std::string_view right) const noexcept -> bool {
      return left == right;
    }
  };

  [[nodiscard]] auto deleted_keys() const -> const
      std::unordered_set<std::string, TransparentHash, TransparentEqual> &;

  struct LeafStateEntry {
    std::filesystem::file_time_type root_mtime;
    std::uint16_t target_bitmap;
    bool has_cross_leaf_deps;
  };

  [[nodiscard]] auto leaf_state(const std::string &output,
                                const std::string &relative_path, bool evaluate,
                                bool web) const -> const LeafStateEntry *;

  [[nodiscard]] auto leaf_relative_paths(const std::string &output) const
      -> std::vector<std::string>;

private:
  auto build_leaf_index(const std::string &output) const -> void;
  auto probe_slot(std::string_view key, std::uint8_t kind) const
      -> const std::uint8_t *;
  auto parse_slot_entry(const std::uint8_t *slot) const -> const Entry &;
  auto parse_slot_resolver_entry(const std::uint8_t *slot) const
      -> const ResolverEntry &;

  std::unique_ptr<sourcemeta::core::FileView> view;
  const std::uint8_t *view_data{nullptr};

  std::uint32_t table_capacity{0};
  const std::uint8_t *table_slots{nullptr};
  const std::uint8_t *string_pool{nullptr};

  std::unordered_map<std::string, Entry, TransparentHash, TransparentEqual>
      overlay;
  std::unordered_set<std::string, TransparentHash, TransparentEqual> deleted;
  mutable std::unordered_map<std::string, Entry, TransparentHash,
                             TransparentEqual>
      lazy_cache;

  std::unordered_map<std::string, ResolverEntry, TransparentHash,
                     TransparentEqual>
      resolver_overlay;
  mutable std::unordered_map<std::string, ResolverEntry, TransparentHash,
                             TransparentEqual>
      resolver_lazy_cache;

  mutable std::mutex mutex_;
  std::filesystem::path loaded_path;
  std::size_t entry_count{0};
  std::size_t resolver_entry_count{0};
  bool dirty{false};
  mutable bool keys_stale{true};
  mutable std::vector<std::string_view> cached_keys;
  mutable bool leaf_index_stale{true};
  mutable std::string leaf_index_output;
  mutable std::unordered_map<std::string, LeafStateEntry, TransparentHash,
                             TransparentEqual>
      leaf_index_cache;
  const std::uint8_t *persisted_leaf_table{nullptr};
  std::uint32_t persisted_leaf_count{0};

  std::span<const LeafRule> leaf_rules{};
  std::uint32_t rules_fingerprint{0};
  std::string sentinel_separator{};
};

} // namespace sourcemeta::one

#endif // SOURCEMETA_ONE_BUILD_STATE_H_
