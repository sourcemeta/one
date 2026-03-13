#include <sourcemeta/one/build.h>

#include "rules.h"

#include <algorithm>     // std::ranges::sort, std::ranges::all_of
#include <cassert>       // assert
#include <cstdint>       // std::size_t
#include <string>        // std::string
#include <string_view>   // std::string_view
#include <unordered_map> // std::unordered_map
#include <unordered_set> // std::unordered_set
#include <vector>        // std::vector

namespace sourcemeta::one {

static constexpr auto SENTINEL = "%";

static auto schema_base_path(const std::filesystem::path &output,
                             const std::filesystem::path &relative_path)
    -> std::filesystem::path {
  return output / SCHEMAS_DIRECTORY / relative_path / SENTINEL;
}

static auto explorer_base_path(const std::filesystem::path &output,
                               const std::filesystem::path &relative_path)
    -> std::filesystem::path {
  return output / EXPLORER_DIRECTORY / relative_path / SENTINEL;
}

struct Target {
  BuildPlan::Action::Type action;
  std::filesystem::path destination;
  std::vector<std::filesystem::path> dependencies;
  std::string_view data;
};

using TargetMap = std::unordered_map<std::string, Target>;

static auto declare_target(TargetMap &targets, BuildPlan::Action::Type action,
                           std::filesystem::path destination,
                           std::vector<std::filesystem::path> dependencies,
                           const std::string_view data = {}) -> void {
  const auto key{destination.string()};
  targets.emplace(key, Target{.action = action,
                              .destination = std::move(destination),
                              .dependencies = std::move(dependencies),
                              .data = data});
}

static auto
declare_schema_targets(TargetMap &targets, const std::filesystem::path &output,
                       const std::filesystem::path &source,
                       const std::filesystem::path &relative_path,
                       const bool evaluate, const BuildPlan::Type build_type,
                       const std::filesystem::path &configuration_path,
                       const std::string_view uri) -> void {
  const auto schema_base{schema_base_path(output, relative_path)};
  const auto explorer_base{explorer_base_path(output, relative_path)};

  for (std::size_t index{0}; index < PER_SCHEMA_RULES.size(); index++) {
    const auto &rule{PER_SCHEMA_RULES[index]};

    if (rule.gate == TargetGate::IfEvaluate && !evaluate) {
      continue;
    }

    if (rule.gate == TargetGate::FullOnly &&
        build_type != BuildPlan::Type::Full) {
      continue;
    }

    const auto &base{rule.base == TargetBase::Schema ? schema_base
                                                     : explorer_base};
    auto destination{base / rule.filename};

    std::vector<std::filesystem::path> dependencies;
    dependencies.reserve(rule.dependency_count);
    for (std::uint8_t dependency_index{0};
         dependency_index < rule.dependency_count; dependency_index++) {
      const auto &dependency{rule.dependencies[dependency_index]};
      switch (dependency.source) {
        case DependencySource::SchemaBase:
          dependencies.emplace_back(schema_base / dependency.filename);
          break;
        case DependencySource::ExplorerBase:
          dependencies.emplace_back(explorer_base / dependency.filename);
          break;
        case DependencySource::GlobalOutput:
          dependencies.emplace_back(output / dependency.filename);
          break;
        case DependencySource::ExternalSource:
          dependencies.emplace_back(source);
          break;
        case DependencySource::ExternalConfig:
          dependencies.emplace_back(configuration_path);
          break;
      }
    }

    declare_target(targets, rule.action, std::move(destination),
                   std::move(dependencies), uri);
  }
}

enum class DirtyState : std::uint8_t { Unknown, Clean, Dirty };

static auto is_dirty(const std::string &target_path, const TargetMap &targets,
                     const BuildState &entries,
                     const std::unordered_set<std::string> &force_dirty,
                     std::unordered_map<std::string, DirtyState> &cache)
    -> bool {
  const auto cached{cache.find(target_path)};
  if (cached != cache.end()) {
    return cached->second == DirtyState::Dirty;
  }

  cache[target_path] = DirtyState::Clean;

  const auto target_match{targets.find(target_path)};
  if (target_match == targets.end()) {
    return false;
  }

  if (force_dirty.contains(target_path)) {
    cache[target_path] = DirtyState::Dirty;
    return true;
  }

  const auto *state_entry{entries.entry(target_path)};
  if (state_entry == nullptr) {
    cache[target_path] = DirtyState::Dirty;
    return true;
  }

  for (const auto &dependency : target_match->second.dependencies) {
    if (is_dirty(dependency.string(), targets, entries, force_dirty, cache)) {
      cache[target_path] = DirtyState::Dirty;
      return true;
    }
  }

  for (const auto &dependency : state_entry->dependencies) {
    if (is_dirty(dependency.string(), targets, entries, force_dirty, cache)) {
      cache[target_path] = DirtyState::Dirty;
      return true;
    }
  }

  return false;
}

static auto compute_wave(const std::string &path, const TargetMap &targets,
                         const std::unordered_set<std::string> &dirty_set,
                         std::unordered_map<std::string, std::size_t> &cache)
    -> std::size_t {
  const auto cached{cache.find(path)};
  if (cached != cache.end()) {
    return cached->second;
  }

  cache[path] = 0;

  const auto target_match{targets.find(path)};
  if (target_match == targets.end()) {
    return 0;
  }

  std::size_t max_dependency_wave{0};
  for (const auto &dependency : target_match->second.dependencies) {
    const auto &dependency_string{dependency.string()};
    if (dirty_set.contains(dependency_string)) {
      const auto dependency_wave{
          compute_wave(dependency_string, targets, dirty_set, cache)};
      if (dependency_wave + 1 > max_dependency_wave) {
        max_dependency_wave = dependency_wave + 1;
      }
    }
  }

  cache[path] = max_dependency_wave;
  return max_dependency_wave;
}

static auto collect_affected_directories(
    const std::filesystem::path &schemas_path,
    const std::vector<std::filesystem::path> &affected_relative_paths)
    -> std::vector<std::filesystem::path> {
  std::unordered_set<std::string> directory_set;
  directory_set.insert(schemas_path.string());

  for (const auto &relative_path : affected_relative_paths) {
    auto current{(schemas_path / relative_path).parent_path()};
    while (current != schemas_path) {
      directory_set.insert(current.string());
      current = current.parent_path();
    }
  }

  std::vector<std::filesystem::path> result;
  result.reserve(directory_set.size());
  for (const auto &entry : directory_set) {
    result.emplace_back(entry);
  }

  std::ranges::sort(result, [](const std::filesystem::path &left,
                               const std::filesystem::path &right) {
    const auto left_depth{std::distance(left.begin(), left.end())};
    const auto right_depth{std::distance(right.begin(), right.end())};
    if (left_depth == right_depth) {
      return left < right;
    }

    return left_depth > right_depth;
  });

  return result;
}

auto delta(const BuildPlan::Type build_type, const BuildState &entries,
           const std::filesystem::path &output, const Resolver::Views &schemas,
           const std::string_view version,
           const std::string_view current_version,
           const std::string_view comment,
           const sourcemeta::core::JSON &configuration,
           const sourcemeta::core::JSON &current_configuration,
           const std::vector<std::filesystem::path> &changed,
           const std::vector<std::filesystem::path> &removed) -> BuildPlan {
  assert(output.is_absolute());
  assert(std::ranges::all_of(schemas, [](const auto &entry) {
    return entry.second.path.is_absolute() &&
           !entry.second.relative_path.is_absolute();
  }));
  assert(std::ranges::all_of(
      changed, [](const auto &path) { return path.is_absolute(); }));
  assert(std::ranges::all_of(
      removed, [](const auto &path) { return path.is_absolute(); }));

  assert(!version.empty());
  assert(!configuration.is_null());
  const auto version_path{output / VERSION_RULE.filename};
  const auto configuration_path{output / CONFIGURATION_RULE.filename};
  const auto comment_path{output / COMMENT_RULE.filename};
  assert(current_version.empty() == !entries.contains(version_path.native()));
  assert(current_configuration.is_null() ==
         !entries.contains(configuration_path.native()));
  const auto version_changed{current_version != version};
  const auto configuration_changed{current_configuration.is_null() ||
                                   configuration != current_configuration};
  const auto is_full{version_changed || configuration_changed};
  const auto schemas_path{output / SCHEMAS_DIRECTORY};
  const auto explorer_path{output / EXPLORER_DIRECTORY};
  const auto comment_string{comment_path.string()};

  std::unordered_set<std::string> removed_uris;
  if (!changed.empty() || !removed.empty()) {
    std::unordered_set<std::string> removed_set;
    removed_set.reserve(removed.size());
    for (const auto &path : removed) {
      removed_set.insert(path.string());
    }

    for (const auto &[uri, info] : schemas) {
      if (removed_set.contains(info.path.string())) {
        removed_uris.insert(uri);
      }
    }
  }

  TargetMap targets;

  std::vector<std::filesystem::path> all_relative_paths;
  all_relative_paths.reserve(schemas.size());

  for (const auto &[uri, info] : schemas) {
    if (removed_uris.contains(uri)) {
      continue;
    }

    declare_schema_targets(targets, output, info.path, info.relative_path,
                           info.evaluate, build_type, configuration_path, uri);
    all_relative_paths.emplace_back(info.relative_path);
  }

  std::unordered_set<std::string> force_dirty;

  if (is_full) {
    for (const auto &[uri, info] : schemas) {
      if (removed_uris.contains(uri)) {
        continue;
      }

      force_dirty.insert(
          (schema_base_path(output, info.relative_path) / ROOT_RULE.filename)
              .string());
    }
  } else if (!changed.empty()) {
    std::unordered_set<std::string> changed_set;
    changed_set.reserve(changed.size());
    for (const auto &path : changed) {
      changed_set.insert(path.string());
    }

    for (const auto &[uri, info] : schemas) {
      if (removed_uris.contains(uri)) {
        continue;
      }

      if (!changed_set.contains(info.path.string())) {
        continue;
      }

      const auto metapack_path{
          (schema_base_path(output, info.relative_path) / ROOT_RULE.filename)
              .string()};
      if (entries.is_stale(metapack_path, info.mtime)) {
        force_dirty.insert(metapack_path);
      }
    }
  } else if (!is_full) {
    for (const auto &[uri, info] : schemas) {
      if (removed_uris.contains(uri)) {
        continue;
      }

      const auto metapack_path{
          (schema_base_path(output, info.relative_path) / ROOT_RULE.filename)
              .string()};
      if (entries.is_stale(metapack_path, info.mtime)) {
        force_dirty.insert(metapack_path);
      }
    }
  }

  std::unordered_map<std::string, DirtyState> dirty_cache;
  std::unordered_set<std::string> dirty_set;

  for (const auto &[target_path, target] : targets) {
    if (is_dirty(target_path, targets, entries, force_dirty, dirty_cache)) {
      dirty_set.insert(target_path);
    }
  }

  bool has_missing_web{false};
  if (build_type == BuildPlan::Type::Full && dirty_set.empty() && !is_full) {
    for (const auto &[uri, info] : schemas) {
      if (removed_uris.contains(uri)) {
        continue;
      }

      for (const auto &rule : PER_SCHEMA_RULES) {
        if (rule.gate != TargetGate::FullOnly) {
          continue;
        }

        const auto &base{rule.base == TargetBase::Schema
                             ? schema_base_path(output, info.relative_path)
                             : explorer_base_path(output, info.relative_path)};
        if (!entries.contains((base / rule.filename).string())) {
          has_missing_web = true;
          break;
        }
      }

      if (has_missing_web) {
        break;
      }
    }
  }

  bool has_stale_web{false};
  if (build_type == BuildPlan::Type::Headless && dirty_set.empty() &&
      !is_full) {
    const auto explorer_prefix{explorer_path.string() + "/"};
    for (const auto &[entry_path, entry] : entries) {
      if (!entry_path.starts_with(explorer_prefix)) {
        continue;
      }

      const auto last_slash{entry_path.rfind('/')};
      if (last_slash == std::string::npos) {
        continue;
      }

      const std::string_view filename{entry_path.data() + last_slash + 1,
                                      entry_path.size() - last_slash - 1};
      for (const auto &rule : PER_SCHEMA_RULES) {
        if (rule.gate == TargetGate::FullOnly && filename == rule.filename) {
          has_stale_web = true;
          break;
        }
      }

      if (!has_stale_web) {
        for (const auto &rule : DIRECTORY_RULES) {
          if (rule.gate == TargetGate::FullOnly && filename == rule.filename) {
            has_stale_web = true;
            break;
          }
        }
      }

      if (has_stale_web) {
        break;
      }
    }
  }

  bool has_potential_stale{false};
  {
    std::unordered_set<std::string> current_schema_bases;
    for (const auto &[uri, info] : schemas) {
      current_schema_bases.insert(
          schema_base_path(output, info.relative_path).string());
    }

    const auto schemas_prefix{schemas_path.string() + "/"};
    for (const auto &[entry_path, entry] : entries) {
      if (!entry_path.starts_with(schemas_prefix)) {
        continue;
      }

      const auto sentinel_pos{entry_path.find("/%/")};
      if (sentinel_pos == std::string::npos) {
        continue;
      }

      const auto base{entry_path.substr(0, sentinel_pos + 2)};
      if (!current_schema_bases.contains(base)) {
        has_potential_stale = true;
        break;
      }
    }
  }

  if (!is_full && dirty_set.empty() && removed_uris.empty() &&
      !has_missing_web && !has_stale_web && !has_potential_stale) {
    if (!comment.empty()) {
      BuildPlan plan;
      plan.output = output;
      plan.type = build_type;
      plan.waves.push_back({{.type = BuildPlan::Action::Type::Comment,
                             .destination = comment_path,
                             .dependencies = {},
                             .data = comment}});
      plan.size = 1;
      return plan;
    }

    if (comment.empty() && entries.contains(comment_path.native())) {
      BuildPlan plan;
      plan.output = output;
      plan.type = build_type;
      plan.waves.push_back({{.type = BuildPlan::Action::Type::Remove,
                             .destination = comment_path,
                             .dependencies = {},
                             .data = {}}});
      plan.size = 1;
      return plan;
    }

    return {.output = output, .type = build_type, .waves = {}};
  }

  const auto has_schema_work{is_full || !dirty_set.empty() ||
                             !removed_uris.empty() || has_missing_web};

  std::vector<std::filesystem::path> affected_relative_paths;
  if (has_schema_work) {
    for (const auto &[uri, info] : schemas) {
      if (removed_uris.contains(uri)) {
        continue;
      }

      if (is_full) {
        affected_relative_paths.emplace_back(info.relative_path);
        continue;
      }

      const auto base{schema_base_path(output, info.relative_path)};
      if (dirty_set.contains((base / ROOT_RULE.filename).string())) {
        affected_relative_paths.emplace_back(info.relative_path);
      }
    }

    for (std::size_t index{0}; index < AGGREGATE_RULES.size(); index++) {
      const auto &rule{AGGREGATE_RULES[index]};

      std::vector<std::filesystem::path> all_collected;
      all_collected.reserve(schemas.size());
      for (const auto &[uri, info] : schemas) {
        if (removed_uris.contains(uri)) {
          continue;
        }

        const auto base{rule.collector_base == TargetBase::Schema
                            ? schema_base_path(output, info.relative_path)
                            : explorer_base_path(output, info.relative_path)};
        all_collected.emplace_back(base / rule.collector_filename);
      }

      const auto destination{
          rule.output_base == AggregateOutputBase::Explorer
              ? (explorer_path / SENTINEL / rule.output_filename)
              : (output / rule.output_filename)};

      declare_target(targets, rule.action, destination,
                     std::move(all_collected));
      dirty_set.insert(destination.string());
    }

    // TODO: This is a coarse boolean that forces ALL dependents and
    // WebSchema to rebuild whenever any schema is added or removed.
    // Ideally we would narrow this to only the affected schemas, but
    // dependency-tree.metapack is a global aggregate, so any change
    // fans out to every dependent. Fixing this requires content-based
    // dirty checking or breaking the global bottleneck.
    auto has_graph_change{!removed_uris.empty()};
    if (!has_graph_change) {
      for (const auto &[uri, info] : schemas) {
        if (!entries.contains((schema_base_path(output, info.relative_path) /
                               ROOT_RULE.filename)
                                  .native())) {
          has_graph_change = true;
          break;
        }
      }
    }

    for (const auto &[uri, info] : schemas) {
      if (removed_uris.contains(uri)) {
        continue;
      }

      const auto schema_base{schema_base_path(output, info.relative_path)};
      const auto explorer_base{explorer_base_path(output, info.relative_path)};
      const auto schema_is_dirty{
          dirty_set.contains((schema_base / ROOT_RULE.filename).string())};

      for (std::size_t index{0}; index < PER_SCHEMA_RULES.size(); index++) {
        const auto &rule{PER_SCHEMA_RULES[index]};
        if (rule.dirty == DirtyOverride::Normal) {
          continue;
        }

        if (rule.gate == TargetGate::FullOnly &&
            build_type != BuildPlan::Type::Full) {
          continue;
        }

        const auto &base{rule.base == TargetBase::Schema ? schema_base
                                                         : explorer_base};
        const auto target_path_string{(base / rule.filename).string()};
        auto should_force{is_full || schema_is_dirty ||
                          !entries.contains(target_path_string)};

        if (!should_force && has_graph_change &&
            (rule.dirty == DirtyOverride::ForceOnGraphChange ||
             rule.dirty == DirtyOverride::ForceIfAffected)) {
          should_force = true;
        }

        if (should_force) {
          dirty_set.insert(target_path_string);
        }
      }
    }

    const auto affected_dirs{
        collect_affected_directories(schemas_path, affected_relative_paths)};

    for (const auto &rule : DIRECTORY_RULES) {
      if (rule.gate == TargetGate::FullOnly &&
          build_type != BuildPlan::Type::Full) {
        continue;
      }

      for (const auto &directory : affected_dirs) {
        const auto relative{std::filesystem::relative(directory, schemas_path)};
        const auto is_root_dir{relative == "."};

        if (rule.scope == DirectoryScope::RootOnly && !is_root_dir) {
          continue;
        }

        if (rule.scope == DirectoryScope::NonRoot && is_root_dir) {
          continue;
        }

        if (rule.only_full_rebuild && !is_full) {
          continue;
        }

        const auto destination{
            (explorer_path / relative / SENTINEL / rule.filename)
                .lexically_normal()};

        std::vector<std::filesystem::path> rule_dependencies;

        for (std::uint8_t dependency_index{0};
             dependency_index < rule.dependency_count; dependency_index++) {
          const auto &dependency{rule.dependencies[dependency_index]};
          switch (dependency.kind) {
            case DirectoryDependencyKind::SchemaMetadata:
              for (const auto &schema_relative : all_relative_paths) {
                if (schema_relative.parent_path() == relative ||
                    (is_root_dir && !schema_relative.has_parent_path())) {
                  rule_dependencies.emplace_back(
                      explorer_base_path(output, schema_relative) /
                      SCHEMA_METADATA_RULE.filename);
                }
              }
              break;
            case DirectoryDependencyKind::ChildDirectories:
              for (const auto &other_directory : affected_dirs) {
                auto other_relative{
                    std::filesystem::relative(other_directory, schemas_path)};
                if (other_relative == relative) {
                  continue;
                }

                auto other_parent{other_relative.parent_path()};
                if (other_parent.empty()) {
                  other_parent = ".";
                }

                if (other_parent == relative) {
                  rule_dependencies.emplace_back(
                      (explorer_path / other_relative / SENTINEL /
                       DIRECTORY_RULES[0].filename)
                          .lexically_normal());
                }
              }
              break;
            case DirectoryDependencyKind::SameDirectoryTarget:
              rule_dependencies.emplace_back(
                  (explorer_path / relative / SENTINEL / dependency.filename)
                      .lexically_normal());
              break;
            case DirectoryDependencyKind::ExternalConfig:
              rule_dependencies.emplace_back(configuration_path);
              break;
          }
        }

        declare_target(targets, rule.action, destination,
                       std::move(rule_dependencies));
        dirty_set.insert(destination.string());
      }
    }
  } // has_schema_work

  std::unordered_map<std::string, std::size_t> wave_cache;
  std::size_t max_wave{0};

  for (const auto &target_path : dirty_set) {
    if (!targets.contains(target_path)) {
      continue;
    }

    const auto wave{compute_wave(target_path, targets, dirty_set, wave_cache)};
    if (wave > max_wave) {
      max_wave = wave;
    }
  }

  std::vector<std::vector<BuildPlan::Action>> dag_waves;
  if (!dirty_set.empty()) {
    dag_waves.resize(max_wave + 1);
    for (const auto &target_path : dirty_set) {
      if (!targets.contains(target_path)) {
        continue;
      }

      const auto &target{targets.at(target_path)};
      const auto wave_it{wave_cache.find(target_path)};
      const auto wave{wave_it != wave_cache.end() ? wave_it->second
                                                  : std::size_t{0}};
      dag_waves[wave].push_back({target.action, target.destination,
                                 target.dependencies, target.data});
    }
  }

  std::vector<BuildPlan::Action> remove_wave;

  for (const auto &uri : removed_uris) {
    const auto match{schemas.find(uri)};
    if (match == schemas.end()) {
      continue;
    }

    remove_wave.push_back(
        {BuildPlan::Action::Type::Remove,
         schema_base_path(output, match->second.relative_path),
         {},
         {}});
    remove_wave.push_back(
        {BuildPlan::Action::Type::Remove,
         explorer_base_path(output, match->second.relative_path),
         {},
         {}});
  }

  for (const auto &[uri, info] : schemas) {
    if (removed_uris.contains(uri) || info.evaluate) {
      continue;
    }

    const auto base{schema_base_path(output, info.relative_path)};
    const auto schema_dirty{
        dirty_set.contains((base / ROOT_RULE.filename).string())};
    if (schema_dirty || is_full) {
      for (const auto &rule : PER_SCHEMA_RULES) {
        if (rule.gate != TargetGate::IfEvaluate) {
          continue;
        }

        const auto &target_base{
            rule.base == TargetBase::Schema
                ? base
                : explorer_base_path(output, info.relative_path)};
        const auto target_path{target_base / rule.filename};
        if (entries.contains(target_path.native())) {
          remove_wave.push_back(
              {BuildPlan::Action::Type::Remove, target_path, {}, {}});
        }
      }
    }
  }

  std::unordered_set<std::string> known_bases;
  for (const auto &[uri, info] : schemas) {
    known_bases.insert(schema_base_path(output, info.relative_path).string());
    known_bases.insert(explorer_base_path(output, info.relative_path).string());
    auto current{info.relative_path};
    while (current.has_parent_path() && current.parent_path() != current) {
      current = current.parent_path();
      known_bases.insert((explorer_path / current / SENTINEL).string());
    }
  }
  known_bases.insert((explorer_path / SENTINEL).string());

  std::unordered_set<std::string> known_ancestors;
  known_ancestors.reserve(known_bases.size() * 3);
  for (const auto &base : known_bases) {
    auto slash_pos{base.rfind('/')};
    while (slash_pos != std::string::npos && slash_pos > 0) {
      const auto ancestor{base.substr(0, slash_pos)};
      if (!known_ancestors.insert(ancestor).second) {
        break;
      }
      slash_pos = ancestor.rfind('/');
    }
  }

  const auto output_prefix{output.string() + "/"};

  std::unordered_set<std::string> global_skip_paths;
  for (const auto &rule : AGGREGATE_RULES) {
    const auto path{rule.output_base == AggregateOutputBase::Explorer
                        ? (explorer_path / SENTINEL / rule.output_filename)
                        : (output / rule.output_filename)};
    global_skip_paths.insert(path.string());
  }
  for (const auto &rule : GLOBAL_RULES) {
    global_skip_paths.insert((output / rule.filename).string());
  }

  std::unordered_set<std::string> stale_roots;
  for (const auto &[entry_path, entry] : entries) {
    if (!entry_path.starts_with(output_prefix)) {
      continue;
    }

    if (global_skip_paths.contains(entry_path)) {
      continue;
    }

    bool is_known{false};
    const auto sentinel_pos{entry_path.find("/%/")};
    if (sentinel_pos != std::string::npos) {
      is_known = known_bases.contains(entry_path.substr(0, sentinel_pos + 2));
    }

    if (!is_known) {
      is_known = known_bases.contains(entry_path) ||
                 known_ancestors.contains(entry_path);
    }

    if (!is_known) {
      auto stale_end{entry_path.size()};
      while (true) {
        const auto slash_pos{entry_path.rfind('/', stale_end - 1)};
        if (slash_pos == std::string::npos ||
            slash_pos < output_prefix.size()) {
          break;
        }
        const auto parent{entry_path.substr(0, slash_pos)};
        if (known_bases.contains(parent) || known_ancestors.contains(parent)) {
          break;
        }
        stale_end = slash_pos;
      }

      stale_roots.insert(entry_path.substr(0, stale_end));
    }
  }

  for (auto &root : stale_roots) {
    remove_wave.push_back(
        {BuildPlan::Action::Type::Remove, std::filesystem::path{root}, {}, {}});
  }

  std::unordered_set<std::string_view> web_only_filenames;
  for (const auto &rule : PER_SCHEMA_RULES) {
    if (rule.gate == TargetGate::FullOnly) {
      web_only_filenames.insert(rule.filename);
    }
  }
  for (const auto &rule : DIRECTORY_RULES) {
    if (rule.gate == TargetGate::FullOnly) {
      web_only_filenames.insert(rule.filename);
    }
  }

  bool web_removed{false};
  if (build_type == BuildPlan::Type::Headless) {
    const auto explorer_prefix{explorer_path.string() + "/"};
    for (const auto &[entry_path, entry] : entries) {
      if (!entry_path.starts_with(explorer_prefix)) {
        continue;
      }

      const auto last_slash{entry_path.rfind('/')};
      if (last_slash == std::string::npos) {
        continue;
      }

      const std::string_view filename{entry_path.data() + last_slash + 1,
                                      entry_path.size() - last_slash - 1};
      if (web_only_filenames.contains(filename)) {
        remove_wave.push_back({BuildPlan::Action::Type::Remove,
                               std::filesystem::path{entry_path},
                               {},
                               {}});
        web_removed = true;
      }
    }
  }

  bool web_added{false};
  if (build_type == BuildPlan::Type::Full && !is_full) {
    const auto explorer_prefix{explorer_path.string() + "/"};
    bool had_web_entries{false};
    for (const auto &[entry_path, entry] : entries) {
      if (!entry_path.starts_with(explorer_prefix)) {
        continue;
      }
      const auto last_slash{entry_path.rfind('/')};
      if (last_slash == std::string::npos) {
        continue;
      }
      const std::string_view filename{entry_path.data() + last_slash + 1,
                                      entry_path.size() - last_slash - 1};
      if (web_only_filenames.contains(filename)) {
        had_web_entries = true;
        break;
      }
    }
    web_added = !had_web_entries;
  }

  BuildPlan plan;
  plan.output = output;
  plan.type = build_type;

  if (is_full) {
    std::vector<BuildPlan::Action> init_wave;
    init_wave.push_back({.type = BuildPlan::Action::Type::Version,
                         .destination = version_path,
                         .dependencies = {},
                         .data = version});
    init_wave.push_back({.type = BuildPlan::Action::Type::Configuration,
                         .destination = configuration_path,
                         .dependencies = {},
                         .data = {}});
    if (!comment.empty()) {
      init_wave.push_back({.type = BuildPlan::Action::Type::Comment,
                           .destination = comment_path,
                           .dependencies = {},
                           .data = comment});
    } else if (entries.contains(comment_string)) {
      remove_wave.push_back(
          {BuildPlan::Action::Type::Remove, comment_path, {}, {}});
    }
    plan.waves.push_back(std::move(init_wave));
  } else if (!comment.empty()) {
    plan.waves.push_back({{.type = BuildPlan::Action::Type::Comment,
                           .destination = comment_path,
                           .dependencies = {},
                           .data = comment}});
  } else if (entries.contains(comment_string)) {
    remove_wave.push_back(
        {BuildPlan::Action::Type::Remove, comment_path, {}, {}});
  }

  for (auto &wave : dag_waves) {
    if (!wave.empty()) {
      plan.waves.push_back(std::move(wave));
    }
  }

  if (is_full || web_added || web_removed) {
    std::vector<BuildPlan::Action> global_wave;
    for (const auto &rule : GLOBAL_RULES) {
      if (rule.trigger != GlobalTrigger::WebTransition) {
        continue;
      }

      global_wave.push_back(
          {rule.action,
           output / rule.filename,
           {configuration_path},
           build_type == BuildPlan::Type::Full ? "Full" : "Headless"});
    }

    if (!global_wave.empty()) {
      plan.waves.push_back(std::move(global_wave));
    }
  }

  if (!remove_wave.empty()) {
    std::ranges::sort(remove_wave, [](const BuildPlan::Action &left,
                                      const BuildPlan::Action &right) {
      return left.destination < right.destination;
    });
    std::vector<BuildPlan::Action> deduped_remove;
    deduped_remove.reserve(remove_wave.size());
    for (auto &entry : remove_wave) {
      const auto &path{entry.destination.string()};
      bool is_child{false};
      for (const auto &kept : deduped_remove) {
        const auto &parent{kept.destination.string()};
        if (path.size() > parent.size() && path[parent.size()] == '/' &&
            path.starts_with(parent)) {
          is_child = true;
          break;
        }
      }

      if (!is_child) {
        deduped_remove.push_back(std::move(entry));
      }
    }

    plan.waves.push_back(std::move(deduped_remove));
  }

  for (auto &wave : plan.waves) {
    std::ranges::sort(wave, [](const BuildPlan::Action &left,
                               const BuildPlan::Action &right) {
      return left.destination < right.destination;
    });
    plan.size += wave.size();
  }

  return plan;
}

} // namespace sourcemeta::one
