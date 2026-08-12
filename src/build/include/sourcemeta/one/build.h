#ifndef SOURCEMETA_ONE_BUILD_H_
#define SOURCEMETA_ONE_BUILD_H_

#ifndef SOURCEMETA_ONE_BUILD_EXPORT
#include <sourcemeta/one/build_export.h>
#endif

#include <sourcemeta/core/json.h>

#include <sourcemeta/one/build_error.h>
#include <sourcemeta/one/build_state.h>

#include <array>      // std::array
#include <cstdint>    // std::uint8_t, std::uint32_t, std::uint64_t, std::size_t
#include <filesystem> // std::filesystem::path, std::filesystem::file_time_type
#include <functional> // std::function
#include <limits>     // std::numeric_limits
#include <span>       // std::span
#include <string>     // std::string
#include <string_view> // std::string_view
#include <utility>     // std::pair

namespace sourcemeta::one {

using BuildDynamicCallback = std::function<void(const std::filesystem::path &)>;

struct BuildLimits {
  std::uint64_t maximum_direct_directory_entries{0};
};

enum class BuildPhase : std::uint8_t { Produce, Combine };

struct LeafView {
  const std::filesystem::path *path;
  const std::filesystem::path *relative_path;
  std::filesystem::file_time_type mtime;
  bool evaluate{true};
};

using LeafSet = std::span<const std::pair<std::string_view, LeafView>>;

template <std::size_t S, std::size_t D, std::size_t G, std::size_t B>
struct DeltaRuleSet {
  // Each leaf rule's index is shifted into a std::uint16_t target bitmap, so
  // more rules than the bitmap has bits would silently truncate 1 << index
  static_assert(S <= std::numeric_limits<std::uint16_t>::digits,
                "Too many leaf rules for the target bitmap width");
  std::array<LeafRule, S> leaves;
  std::array<ContainerRule, D> containers;
  std::array<GlobalRule, G> globals;
  std::array<DirectoryRule, B> directories;
  std::string_view sentinel;
  BuildPlan::Action::Type remove_action;
  BuildPlan::Type full_mode;
};

template <const auto &RuleSet>
[[nodiscard]] consteval auto find_root_leaf_index() -> std::size_t {
  for (std::size_t index{0}; index < RuleSet.leaves.size(); index++) {
    if (RuleSet.leaves[index].is_root) {
      return index;
    }
  }

  return 0;
}

template <const auto &RuleSet>
[[nodiscard]] consteval auto find_container_target_leaf_index() -> std::size_t {
  for (std::size_t index{0}; index < RuleSet.leaves.size(); index++) {
    if (RuleSet.leaves[index].container_target) {
      return index;
    }
  }

  return 0;
}

template <const auto &RuleSet>
[[nodiscard]] consteval auto find_dependency_tracking_leaf_index()
    -> std::size_t {
  for (std::size_t index{0}; index < RuleSet.leaves.size(); index++) {
    if (RuleSet.leaves[index].tracks_dependencies) {
      return index;
    }
  }

  return 0;
}

template <const auto &RuleSet>
[[nodiscard]] consteval auto find_combine_only_leaf_index() -> std::size_t {
  for (std::size_t index{0}; index < RuleSet.leaves.size(); index++) {
    if (RuleSet.leaves[index].combine_only) {
      return index;
    }
  }

  return 0;
}

template <const auto &RuleSet>
[[nodiscard]] consteval auto find_listing_container_index() -> std::size_t {
  for (std::size_t index{0}; index < RuleSet.containers.size(); index++) {
    if (RuleSet.containers[index].is_listing) {
      return index;
    }
  }

  return 0;
}

template <const auto &RuleSet>
[[nodiscard]] consteval auto
find_global_index_by_trigger(const GlobalTrigger trigger) -> std::size_t {
  for (std::size_t index{0}; index < RuleSet.globals.size(); index++) {
    if (RuleSet.globals[index].trigger == trigger) {
      return index;
    }
  }

  return 0;
}

template <const auto &RuleSet>
[[nodiscard]] consteval auto find_external_config_global_index()
    -> std::size_t {
  for (std::size_t index{0}; index < RuleSet.globals.size(); index++) {
    if (RuleSet.globals[index].external_config_anchor) {
      return index;
    }
  }

  return 0;
}

constexpr auto fingerprint_mix(std::uint32_t &hash, const std::uint8_t byte)
    -> void {
  hash ^= byte;
  hash *= 0x01000193U;
}

constexpr auto fingerprint_text(std::uint32_t &hash, const char *text) -> void {
  if (text == nullptr) {
    fingerprint_mix(hash, 0);
    return;
  }

  for (const char *cursor{text}; *cursor != '\0'; cursor++) {
    fingerprint_mix(hash, static_cast<std::uint8_t>(*cursor));
  }
}

// What a rule produces follows from more than the action naming it, so
// everything deciding that is read here. A recorded state whose rules differ
// from these was built by a different set of them, and reusing what it holds
// would keep whatever those produced. What a rule depends on is the case that
// brought this about: a rule that grew one produces something else from the
// same action, and a build noticing only the action would serve the old answer
// until something unrelated forced it to look again
template <const auto &RuleSet>
[[nodiscard]] consteval auto rules_fingerprint() -> std::uint32_t {
  std::uint32_t hash{0x811c9dc5U};
  for (const auto &rule : RuleSet.leaves) {
    fingerprint_mix(hash, rule.action);
    fingerprint_mix(hash, rule.base);
    fingerprint_text(hash, rule.filename);
    fingerprint_mix(hash, static_cast<std::uint8_t>(rule.gate));
    fingerprint_mix(hash, static_cast<std::uint8_t>(rule.dirty));
    fingerprint_mix(hash, static_cast<std::uint8_t>(rule.is_root));
    fingerprint_mix(hash, static_cast<std::uint8_t>(rule.combine_only));
    fingerprint_mix(hash, static_cast<std::uint8_t>(rule.container_target));
    fingerprint_mix(hash, static_cast<std::uint8_t>(rule.tracks_dependencies));
    fingerprint_mix(hash, rule.dependency_count);
    for (std::uint8_t index{0}; index < rule.dependency_count; index++) {
      const auto &dependency{rule.dependencies[index]};
      fingerprint_mix(hash, static_cast<std::uint8_t>(dependency.source));
      fingerprint_mix(hash, dependency.base);
      fingerprint_text(hash, dependency.filename);
    }
  }
  for (const auto &rule : RuleSet.containers) {
    fingerprint_mix(hash, rule.action);
    fingerprint_mix(hash, rule.base);
    fingerprint_text(hash, rule.filename);
    fingerprint_mix(hash, static_cast<std::uint8_t>(rule.gate));
    fingerprint_mix(hash, static_cast<std::uint8_t>(rule.scope));
    fingerprint_mix(hash, static_cast<std::uint8_t>(rule.only_full_rebuild));
    fingerprint_mix(hash, static_cast<std::uint8_t>(rule.is_listing));
    fingerprint_mix(hash, rule.dependency_count);
    for (std::uint8_t index{0}; index < rule.dependency_count; index++) {
      const auto &dependency{rule.dependencies[index]};
      fingerprint_mix(hash, static_cast<std::uint8_t>(dependency.kind));
      fingerprint_text(hash, dependency.filename);
    }
  }
  for (const auto &rule : RuleSet.globals) {
    fingerprint_mix(hash, rule.action);
    fingerprint_text(hash, rule.filename);
    fingerprint_mix(hash, static_cast<std::uint8_t>(rule.trigger));
    fingerprint_mix(hash,
                    static_cast<std::uint8_t>(rule.external_config_anchor));
    fingerprint_mix(hash, rule.dependency_count);
    for (std::uint8_t index{0}; index < rule.dependency_count; index++) {
      const auto &dependency{rule.dependencies[index]};
      fingerprint_mix(hash, static_cast<std::uint8_t>(dependency.source));
      fingerprint_mix(hash, dependency.base);
      fingerprint_text(hash, dependency.filename);
    }
  }

  return hash;
}

struct DeltaRuleIndices {
  std::size_t root;
  std::size_t metadata;
  std::size_t container_list;
  std::size_t version;
  std::size_t configuration;
  std::size_t comment;
  std::size_t dependencies;
  std::size_t dependents;
  std::size_t mode_global;
  std::uint32_t fingerprint;
};

// Whether a view holds the artifacts describing a leaf. A build emits them only
// where this says so, which is what makes every count derived from them true of
// that view without anything downstream knowing why. It is answered by whoever
// asks for a build rather than worked out here, since what decides it is known
// where the policies are read and nowhere else
using ViewFilter = std::function<bool(std::size_t, std::string_view)>;

// Every view holds every leaf, which is what a build with nothing to hide asks
// for
[[nodiscard]] inline auto view_filter_everything() -> ViewFilter {
  return [](const std::size_t, const std::string_view) -> bool { return true; };
}

SOURCEMETA_ONE_BUILD_EXPORT
auto delta_engine(
    BuildPhase phase, BuildPlan::Type build_type, const BuildState &entries,
    const std::filesystem::path &output, const LeafSet &leaves,
    std::string_view version, bool incremental, std::string_view comment,
    std::string_view mode_label, const BuildLimits &limits,
    std::span<const LeafRule> leaf_rules,
    std::span<const ContainerRule> container_rules,
    std::span<const GlobalRule> global_rules,
    std::span<const DirectoryRule> directories,
    std::span<const std::string_view> views, const ViewFilter &visible,
    std::string_view sentinel, BuildPlan::Action::Type remove_action,
    BuildPlan::Type full_mode, const DeltaRuleIndices &indices) -> BuildPlan;

template <const auto &RuleSet>
auto delta(const BuildPhase phase, const BuildPlan::Type build_type,
           const BuildState &entries, const std::filesystem::path &output,
           const LeafSet &leaves, const std::string_view version,
           const bool incremental, const std::string_view comment,
           const std::string_view mode_label, const BuildLimits &limits,
           const std::span<const std::string_view> views,
           const ViewFilter &visible) -> BuildPlan {
  constexpr DeltaRuleIndices INDICES{
      .root = find_root_leaf_index<RuleSet>(),
      .metadata = find_container_target_leaf_index<RuleSet>(),
      .container_list = find_listing_container_index<RuleSet>(),
      .version =
          find_global_index_by_trigger<RuleSet>(GlobalTrigger::WithVersion),
      .configuration = find_external_config_global_index<RuleSet>(),
      .comment =
          find_global_index_by_trigger<RuleSet>(GlobalTrigger::WithComment),
      .dependencies = find_dependency_tracking_leaf_index<RuleSet>(),
      .dependents = find_combine_only_leaf_index<RuleSet>(),
      .mode_global =
          find_global_index_by_trigger<RuleSet>(GlobalTrigger::OnModeChange),
      .fingerprint = rules_fingerprint<RuleSet>()};

  return delta_engine(phase, build_type, entries, output, leaves, version,
                      incremental, comment, mode_label, limits, RuleSet.leaves,
                      RuleSet.containers, RuleSet.globals, RuleSet.directories,
                      views, visible, RuleSet.sentinel, RuleSet.remove_action,
                      RuleSet.full_mode, INDICES);
}

} // namespace sourcemeta::one

#endif // SOURCEMETA_ONE_BUILD_H_
