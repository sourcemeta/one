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

static auto make_base_string(const std::string &output, const char *directory,
                             const std::string &relative_path) -> std::string {
  std::string result;
  result.reserve(output.size() + relative_path.size() + 16);
  result += output;
  result += '/';
  result += directory;
  result += '/';
  result += relative_path;
  result += '/';
  result += SENTINEL;
  return result;
}

static auto append_filename(const std::string &base, const char *filename)
    -> std::string {
  std::string result;
  result.reserve(base.size() + std::char_traits<char>::length(filename) + 1);
  result += base;
  result += '/';
  result += filename;
  return result;
}

struct Target {
  BuildPlan::Action::Type action;
  std::vector<std::string> dependencies;
  std::string_view data;
};

using TargetMap = std::unordered_map<std::string, Target>;

static auto declare_target(TargetMap &targets, BuildPlan::Action::Type action,
                           std::string destination,
                           std::vector<std::string> dependencies,
                           const std::string_view data = {}) -> void {
  auto iterator{targets.try_emplace(std::move(destination)).first};
  iterator->second = Target{
      .action = action, .dependencies = std::move(dependencies), .data = data};
}

static auto
declare_target_direct(TargetMap &targets, BuildPlan::Action::Type action,
                      std::string destination, const std::string_view data = {})
    -> Target & {
  auto iterator{targets.try_emplace(std::move(destination)).first};
  iterator->second.action = action;
  iterator->second.dependencies.clear();
  iterator->second.data = data;
  return iterator->second;
}

static auto declare_schema_targets(
    TargetMap &targets, const std::string &schema_base,
    const std::string &explorer_base, const std::string &output_string,
    const std::string &source_string, const bool evaluate,
    const BuildPlan::Type build_type, const std::string &configuration_string,
    const std::string_view uri, const BuildPhase phase) -> void {
  for (std::size_t index{0}; index < PER_SCHEMA_RULES.size(); index++) {
    const auto &rule{PER_SCHEMA_RULES[index]};

    if (rule.gate == TargetGate::IfEvaluate && !evaluate) {
      continue;
    }

    if (rule.gate == TargetGate::FullOnly &&
        build_type != BuildPlan::Type::Full) {
      continue;
    }

    if (rule.action == BuildPlan::Action::Type::Dependents &&
        phase == BuildPhase::Produce) {
      continue;
    }

    const auto &base{rule.base == TargetBase::Schema ? schema_base
                                                     : explorer_base};
    auto &target{declare_target_direct(
        targets, rule.action, append_filename(base, rule.filename), uri)};
    target.dependencies.reserve(rule.dependency_count);
    for (std::uint8_t dependency_index{0};
         dependency_index < rule.dependency_count; dependency_index++) {
      const auto &dependency{rule.dependencies[dependency_index]};
      switch (dependency.source) {
        case DependencySource::SchemaBase:
          target.dependencies.push_back(
              append_filename(schema_base, dependency.filename));
          break;
        case DependencySource::ExplorerBase:
          target.dependencies.push_back(
              append_filename(explorer_base, dependency.filename));
          break;
        case DependencySource::GlobalOutput:
          target.dependencies.push_back(
              append_filename(output_string, dependency.filename));
          break;
        case DependencySource::ExternalSource:
          target.dependencies.push_back(source_string);
          break;
        case DependencySource::ExternalConfig:
          target.dependencies.push_back(configuration_string);
          break;
      }
    }
  }
}

static auto
compute_wave(const std::string &path, const TargetMap &targets,
             const std::unordered_set<std::string> &dirty_set,
             std::unordered_map<std::string_view, std::size_t> &cache)
    -> std::size_t {
  const std::string_view path_view{path};
  const auto cached{cache.find(path_view)};
  if (cached != cache.end()) {
    return cached->second;
  }

  cache[path_view] = 0;

  const auto target_match{targets.find(path)};
  if (target_match == targets.end()) {
    return 0;
  }

  std::size_t max_dependency_wave{0};
  for (const auto &dependency : target_match->second.dependencies) {
    if (dirty_set.contains(dependency)) {
      const auto dependency_wave{
          compute_wave(dependency, targets, dirty_set, cache)};
      if (dependency_wave + 1 > max_dependency_wave) {
        max_dependency_wave = dependency_wave + 1;
      }
    }
  }

  cache[path_view] = max_dependency_wave;
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

auto delta(const BuildPhase phase, const BuildPlan::Type build_type,
           const BuildState &entries, const std::filesystem::path &output,
           const Resolver::Views &schemas, const std::string_view version,
           const bool incremental, const std::string_view comment,
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

  if (phase == BuildPhase::Combine) {
    const auto &output_string{output.native()};
    const auto schemas_prefix{output_string + "/" + SCHEMAS_DIRECTORY + "/"};

    auto extract_cross_schema_refs{
        [&schemas_prefix](
            const BuildState::Entry *state_entry,
            std::string_view owner_base) -> std::unordered_set<std::string> {
          std::unordered_set<std::string> result;
          if (state_entry == nullptr) {
            return result;
          }

          for (const auto &dependency : state_entry->dependencies) {
            const auto &dependency_path{dependency.native()};
            if (!dependency_path.starts_with(schemas_prefix)) {
              continue;
            }

            const auto sentinel_position{dependency_path.find("/%/")};
            if (sentinel_position == std::string::npos) {
              continue;
            }

            const auto relative{dependency_path.substr(
                schemas_prefix.size(),
                sentinel_position - schemas_prefix.size())};
            if (relative != owner_base) {
              result.insert(std::string{relative});
            }
          }

          return result;
        }};

    const auto owner_start{schemas_prefix.size()};

    std::unordered_set<std::string> affected_schemas;
    for (const auto key : entries.keys()) {
      if (!key.ends_with("/%/dependencies.metapack")) {
        continue;
      }

      if (!entries.in_overlay(std::string{key})) {
        continue;
      }

      const auto *new_entry{entries.entry(std::string{key})};
      const auto *old_entry{entries.disk_entry(std::string{key})};

      const auto owner_sentinel{key.find("/%/", owner_start)};
      if (owner_sentinel == std::string_view::npos) {
        continue;
      }

      const auto owner_base{
          key.substr(owner_start, owner_sentinel - owner_start)};
      const auto old_refs{extract_cross_schema_refs(old_entry, owner_base)};
      const auto new_refs{extract_cross_schema_refs(new_entry, owner_base)};

      for (const auto &reference : new_refs) {
        if (!old_refs.contains(reference)) {
          affected_schemas.insert(reference);
        }
      }

      for (const auto &reference : old_refs) {
        if (!new_refs.contains(reference)) {
          affected_schemas.insert(reference);
        }
      }

      if (old_entry == nullptr) {
        affected_schemas.insert(std::string{owner_base});
      }
    }

    for (const auto &deleted_key : entries.deleted_keys()) {
      if (!deleted_key.ends_with("/%/dependencies.metapack")) {
        continue;
      }

      if (!deleted_key.starts_with(schemas_prefix)) {
        continue;
      }

      const auto *old_entry{entries.raw_disk_entry(deleted_key)};
      if (old_entry == nullptr) {
        continue;
      }

      const auto owner_sentinel{deleted_key.find("/%/", owner_start)};
      if (owner_sentinel == std::string::npos) {
        continue;
      }

      const auto owner_base{
          deleted_key.substr(owner_start, owner_sentinel - owner_start)};
      for (const auto &reference :
           extract_cross_schema_refs(old_entry, owner_base)) {
        affected_schemas.insert(reference);
      }
    }

    BuildPlan plan;
    plan.output = output;
    plan.type = build_type;

    if (!affected_schemas.empty()) {
      // Build a reverse index: for each schema relative path, collect
      // the dependencies.metapack keys that reference it. This is a
      // single O(keys) pass instead of O(affected × keys).
      std::unordered_map<std::string, std::vector<std::string>>
          reverse_dep_index;
      for (const auto dep_key : entries.keys()) {
        if (!dep_key.ends_with("/%/dependencies.metapack")) {
          continue;
        }

        const auto *dep_entry{entries.entry(std::string{dep_key})};
        if (dep_entry == nullptr) {
          continue;
        }

        for (const auto &dependency : dep_entry->dependencies) {
          const auto &dep_path{dependency.native()};
          if (!dep_path.starts_with(schemas_prefix)) {
            continue;
          }

          const auto sentinel_pos{dep_path.find("/%/", owner_start)};
          if (sentinel_pos == std::string::npos) {
            continue;
          }

          auto referenced_schema{
              dep_path.substr(owner_start, sentinel_pos - owner_start)};
          if (affected_schemas.contains(referenced_schema)) {
            reverse_dep_index[std::move(referenced_schema)].emplace_back(
                dep_key);
          }
        }
      }

      // Deduplicate: a single dep_key may reference the same schema
      // through multiple dependency paths
      for (auto &[schema, dep_keys] : reverse_dep_index) {
        std::ranges::sort(dep_keys);
        const auto [first, last] = std::ranges::unique(dep_keys);
        dep_keys.erase(first, last);
      }

      std::vector<BuildPlan::Action> dependents_wave;
      for (const auto &[uri, info] : schemas) {
        const auto &relative_string{info.relative_path.native()};
        if (!affected_schemas.contains(relative_string)) {
          continue;
        }

        auto schema_base{make_base_string(output_string, SCHEMAS_DIRECTORY,
                                          relative_string)};
        auto destination{std::filesystem::path{
            append_filename(schema_base, "dependents.metapack")}};

        BuildPlan::Action::Dependencies action_dependencies;
        const auto reverse_it{reverse_dep_index.find(relative_string)};
        if (reverse_it != reverse_dep_index.end()) {
          action_dependencies.reserve(reverse_it->second.size());
          for (const auto &dep_key : reverse_it->second) {
            action_dependencies.emplace_back(dep_key);
          }
        }

        dependents_wave.push_back({BuildPlan::Action::Type::Dependents,
                                   std::move(destination),
                                   std::move(action_dependencies), uri});
      }

      std::ranges::sort(dependents_wave, [](const BuildPlan::Action &left,
                                            const BuildPlan::Action &right) {
        return left.destination < right.destination;
      });
      plan.size = dependents_wave.size();
      plan.waves.push_back(std::move(dependents_wave));
    }

    return plan;
  }

  const auto version_path{output / VERSION_RULE.filename};
  const auto configuration_path{output / CONFIGURATION_RULE.filename};
  const auto comment_path{output / COMMENT_RULE.filename};
  const auto is_full{!incremental};
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

  if (!is_full && changed.empty() && removed.empty()) {
    const auto &output_string{output.native()};
    bool fast_path_dirty{false};

    const auto routes_string{(output / "routes.bin").string()};
    const auto had_web{entries.contains(routes_string)};
    const auto needs_web{build_type == BuildPlan::Type::Full};
    if (had_web != needs_web) {
      fast_path_dirty = true;
    }

    std::string schema_buffer;
    std::string explorer_buffer;
    schema_buffer.reserve(output_string.size() + 64);
    explorer_buffer.reserve(output_string.size() + 64);
    std::vector<std::string> current_bases;
    current_bases.reserve(schemas.size());

    if (!fast_path_dirty) {
      for (const auto &[uri, info] : schemas) {
        const auto &relative_string{info.relative_path.native()};

        schema_buffer.clear();
        schema_buffer += output_string;
        schema_buffer += '/';
        schema_buffer += SCHEMAS_DIRECTORY;
        schema_buffer += '/';
        schema_buffer += relative_string;
        schema_buffer += '/';
        schema_buffer += SENTINEL;
        const auto schema_base_end{schema_buffer.size()};
        current_bases.push_back(schema_buffer);

        bool explorer_built{false};
        std::size_t explorer_base_end{0};

        for (const auto &rule : PER_SCHEMA_RULES) {
          if (rule.gate == TargetGate::IfEvaluate && !info.evaluate) {
            continue;
          }

          if (rule.gate == TargetGate::FullOnly &&
              build_type != BuildPlan::Type::Full) {
            continue;
          }

          if (rule.base == TargetBase::Explorer && !explorer_built) {
            explorer_buffer.clear();
            explorer_buffer += output_string;
            explorer_buffer += '/';
            explorer_buffer += EXPLORER_DIRECTORY;
            explorer_buffer += '/';
            explorer_buffer += relative_string;
            explorer_buffer += '/';
            explorer_buffer += SENTINEL;
            explorer_base_end = explorer_buffer.size();
            explorer_built = true;
          }

          auto &buffer{rule.base == TargetBase::Schema ? schema_buffer
                                                       : explorer_buffer};
          const auto base_end{rule.base == TargetBase::Schema
                                  ? schema_base_end
                                  : explorer_base_end};
          buffer.resize(base_end);
          buffer += '/';
          buffer += rule.filename;

          if (rule.is_root) {
            if (entries.is_stale(buffer, info.mtime)) {
              fast_path_dirty = true;
              break;
            }
          } else if (!entries.contains(buffer)) {
            fast_path_dirty = true;
            break;
          }
        }

        if (fast_path_dirty) {
          break;
        }
      }
    }

    if (!fast_path_dirty) {
      std::unordered_set<std::string_view> base_set;
      base_set.reserve(current_bases.size());
      for (const auto &base : current_bases) {
        base_set.insert(base);
      }

      const auto schemas_prefix{output_string + '/' + SCHEMAS_DIRECTORY + '/'};
      for (const auto entry_path : entries.keys()) {
        if (!entry_path.starts_with(schemas_prefix)) {
          continue;
        }

        const auto sentinel_pos{entry_path.find("/%/")};
        if (sentinel_pos == std::string_view::npos) {
          continue;
        }

        if (!base_set.contains(entry_path.substr(0, sentinel_pos + 2))) {
          fast_path_dirty = true;
          break;
        }
      }
    }

    if (!fast_path_dirty) {
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

      if (entries.contains(comment_string)) {
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

      return {.output = output, .type = build_type, .waves = {}, .size = 0};
    }
  }

  TargetMap targets;
  targets.reserve(schemas.size() * PER_SCHEMA_RULES.size() +
                  AGGREGATE_RULES.size());
  const auto &output_string{output.native()};
  const auto configuration_string{configuration_path.string()};
  const auto explorer_string{explorer_path.string()};

  struct ActiveSchema {
    std::string_view uri;
    const Resolver::Entry *info;
    std::string schema_base;
    std::string explorer_base;
    std::string root_path;
  };

  std::vector<ActiveSchema> active_schemas;
  active_schemas.reserve(schemas.size());
  std::vector<std::filesystem::path> all_relative_paths;
  all_relative_paths.reserve(schemas.size());

  for (const auto &[uri, info] : schemas) {
    if (removed_uris.contains(uri)) {
      continue;
    }

    const auto &relative_string{info.relative_path.native()};
    auto schema_base{
        make_base_string(output_string, SCHEMAS_DIRECTORY, relative_string)};
    auto explorer_base{
        make_base_string(output_string, EXPLORER_DIRECTORY, relative_string)};
    declare_schema_targets(targets, schema_base, explorer_base, output_string,
                           info.path.native(), info.evaluate, build_type,
                           configuration_string, uri, phase);
    all_relative_paths.emplace_back(info.relative_path);
    auto root_path{append_filename(schema_base, ROOT_RULE.filename)};
    active_schemas.push_back({uri, &info, std::move(schema_base),
                              std::move(explorer_base), std::move(root_path)});
  }

  std::unordered_set<std::string_view> force_dirty;

  if (is_full) {
    for (const auto &schema : active_schemas) {
      force_dirty.insert(schema.root_path);
    }
  } else if (!changed.empty()) {
    std::unordered_set<std::string> changed_set;
    changed_set.reserve(changed.size());
    for (const auto &path : changed) {
      changed_set.insert(path.native());
    }

    for (const auto &schema : active_schemas) {
      if (!changed_set.contains(schema.info->path.native())) {
        continue;
      }

      if (entries.is_stale(schema.root_path, schema.info->mtime)) {
        force_dirty.insert(schema.root_path);
      }
    }
  } else if (!is_full) {
    for (const auto &schema : active_schemas) {
      if (entries.is_stale(schema.root_path, schema.info->mtime)) {
        force_dirty.insert(schema.root_path);
      }
    }
  }

  std::unordered_set<std::string_view> current_schema_bases;
  current_schema_bases.reserve(active_schemas.size());
  for (const auto &schema : active_schemas) {
    current_schema_bases.insert(schema.schema_base);
  }

  std::unordered_set<std::string> removed_entries;
  {
    const auto schemas_prefix{output_string + '/' + SCHEMAS_DIRECTORY + '/'};
    for (const auto entry_path : entries.keys()) {
      if (!entry_path.starts_with(schemas_prefix)) {
        continue;
      }

      const auto sentinel_pos{entry_path.find("/%/")};
      if (sentinel_pos == std::string_view::npos) {
        continue;
      }

      if (!current_schema_bases.contains(
              entry_path.substr(0, sentinel_pos + 2))) {
        removed_entries.emplace(entry_path);
      }
    }
  }

  std::unordered_set<std::string> dirty_set;
  {
    for (const auto &schema : active_schemas) {
      if (!force_dirty.contains(schema.root_path)) {
        continue;
      }

      for (std::size_t index{0}; index < PER_SCHEMA_RULES.size(); index++) {
        const auto &rule{PER_SCHEMA_RULES[index]};
        if (rule.gate == TargetGate::IfEvaluate && !schema.info->evaluate) {
          continue;
        }

        if (rule.gate == TargetGate::FullOnly &&
            build_type != BuildPlan::Type::Full) {
          continue;
        }

        const auto &base{rule.base == TargetBase::Schema
                             ? schema.schema_base
                             : schema.explorer_base};
        dirty_set.insert(append_filename(base, rule.filename));
      }
    }

    bool propagation_changed{true};
    while (propagation_changed) {
      propagation_changed = false;
      for (const auto &[target_path, target] : targets) {
        if (dirty_set.contains(target_path)) {
          continue;
        }

        for (const auto &dep : target.dependencies) {
          if (dirty_set.contains(dep)) {
            dirty_set.insert(target_path);
            propagation_changed = true;
            goto next_target;
          }
        }

        {
          const auto *state_entry{entries.entry(target_path)};
          if (state_entry == nullptr) {
            dirty_set.insert(target_path);
            propagation_changed = true;
            continue;
          }

          for (const auto &dep : state_entry->dependencies) {
            if (dirty_set.contains(dep.native()) ||
                removed_entries.contains(dep.native())) {
              dirty_set.insert(target_path);
              propagation_changed = true;
              break;
            }
          }
        }

      next_target:;
      }
    }
  }

  bool has_missing_web{false};
  if (build_type == BuildPlan::Type::Full && dirty_set.empty() && !is_full) {
    for (const auto &schema : active_schemas) {
      for (const auto &rule : PER_SCHEMA_RULES) {
        if (rule.gate != TargetGate::FullOnly) {
          continue;
        }

        const auto &base{rule.base == TargetBase::Schema
                             ? schema.schema_base
                             : schema.explorer_base};
        if (!entries.contains(append_filename(base, rule.filename))) {
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
    for (const auto entry_path : entries.keys()) {
      if (!entry_path.starts_with(explorer_prefix)) {
        continue;
      }

      const auto last_slash{entry_path.rfind('/')};
      if (last_slash == std::string_view::npos) {
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

  const auto has_potential_stale{!removed_entries.empty()};

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

    return {.output = output, .type = build_type, .waves = {}, .size = 0};
  }

  const auto has_schema_work{is_full || !dirty_set.empty() ||
                             !removed_uris.empty() || has_missing_web ||
                             has_potential_stale};

  std::vector<std::filesystem::path> affected_relative_paths;
  if (has_schema_work) {
    for (const auto &schema : active_schemas) {
      if (is_full) {
        affected_relative_paths.emplace_back(schema.info->relative_path);
        continue;
      }

      if (dirty_set.contains(schema.root_path)) {
        affected_relative_paths.emplace_back(schema.info->relative_path);
      }
    }

    for (std::size_t index{0}; index < AGGREGATE_RULES.size(); index++) {
      const auto &rule{AGGREGATE_RULES[index]};

      std::vector<std::string> all_collected;
      all_collected.reserve(active_schemas.size());
      for (const auto &schema : active_schemas) {
        const auto &base{rule.collector_base == TargetBase::Schema
                             ? schema.schema_base
                             : schema.explorer_base};
        all_collected.push_back(append_filename(base, rule.collector_filename));
      }

      std::string destination;
      if (rule.output_base == AggregateOutputBase::Explorer) {
        destination.reserve(
            explorer_string.size() + 4 +
            std::char_traits<char>::length(rule.output_filename));
        destination += explorer_string;
        destination += '/';
        destination += SENTINEL;
        destination += '/';
        destination += rule.output_filename;
      } else {
        destination.reserve(
            output_string.size() + 1 +
            std::char_traits<char>::length(rule.output_filename));
        destination += output_string;
        destination += '/';
        destination += rule.output_filename;
      }

      declare_target(targets, rule.action, destination,
                     std::move(all_collected));
      dirty_set.insert(std::move(destination));
    }

    auto has_graph_change{!removed_uris.empty()};
    if (!has_graph_change) {
      for (const auto &schema : active_schemas) {
        if (!entries.contains(schema.root_path)) {
          has_graph_change = true;
          break;
        }
      }
    }

    for (const auto &schema : active_schemas) {
      const auto schema_is_dirty{dirty_set.contains(schema.root_path)};

      for (std::size_t index{0}; index < PER_SCHEMA_RULES.size(); index++) {
        const auto &rule{PER_SCHEMA_RULES[index]};
        if (rule.dirty == DirtyOverride::Normal) {
          continue;
        }

        if (rule.gate == TargetGate::FullOnly &&
            build_type != BuildPlan::Type::Full) {
          continue;
        }

        const auto &base{rule.base == TargetBase::Schema
                             ? schema.schema_base
                             : schema.explorer_base};
        auto target_path_string{append_filename(base, rule.filename)};
        auto should_force{is_full || schema_is_dirty ||
                          !entries.contains(target_path_string)};

        if (!should_force && has_graph_change &&
            (rule.dirty == DirtyOverride::ForceOnGraphChange ||
             rule.dirty == DirtyOverride::ForceIfAffected)) {
          should_force = true;
        }

        if (should_force) {
          dirty_set.insert(std::move(target_path_string));
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

        auto destination{(explorer_path / relative / SENTINEL / rule.filename)
                             .lexically_normal()
                             .string()};

        std::vector<std::string> rule_dependencies;

        for (std::uint8_t dependency_index{0};
             dependency_index < rule.dependency_count; dependency_index++) {
          const auto &dependency{rule.dependencies[dependency_index]};
          switch (dependency.kind) {
            case DirectoryDependencyKind::SchemaMetadata:
              for (const auto &schema_relative : all_relative_paths) {
                if (schema_relative.parent_path() == relative ||
                    (is_root_dir && !schema_relative.has_parent_path())) {
                  rule_dependencies.push_back(append_filename(
                      make_base_string(output_string, EXPLORER_DIRECTORY,
                                       schema_relative.native()),
                      SCHEMA_METADATA_RULE.filename));
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
                  rule_dependencies.push_back(
                      (explorer_path / other_relative / SENTINEL /
                       DIRECTORY_RULES[0].filename)
                          .lexically_normal()
                          .string());
                }
              }
              break;
            case DirectoryDependencyKind::SameDirectoryTarget:
              rule_dependencies.push_back(
                  (explorer_path / relative / SENTINEL / dependency.filename)
                      .lexically_normal()
                      .string());
              break;
            case DirectoryDependencyKind::ExternalConfig:
              rule_dependencies.push_back(configuration_string);
              break;
          }
        }

        declare_target(targets, rule.action, destination,
                       std::move(rule_dependencies));
        dirty_set.insert(std::move(destination));
      }
    }
  }

  std::unordered_map<std::string_view, std::size_t> wave_cache;
  std::size_t max_wave{0};

  for (const auto &target_path : dirty_set) {
    if (targets.find(target_path) == targets.end()) {
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
      auto target_it{targets.find(target_path)};
      if (target_it == targets.end()) {
        continue;
      }

      auto &target{target_it->second};
      const auto wave_it{wave_cache.find(target_path)};
      const auto wave{wave_it != wave_cache.end() ? wave_it->second
                                                  : std::size_t{0}};
      BuildPlan::Action::Dependencies action_deps;
      action_deps.reserve(target.dependencies.size());
      for (auto &dep : target.dependencies) {
        action_deps.emplace_back(std::move(dep));
      }

      dag_waves[wave].push_back({target.action,
                                 std::filesystem::path{target_path},
                                 std::move(action_deps), target.data});
    }
  }

  std::vector<BuildPlan::Action> remove_wave;

  for (const auto &uri : removed_uris) {
    const auto match{schemas.find(uri)};
    if (match == schemas.end()) {
      continue;
    }

    const auto &relative_string{match->second.relative_path.native()};
    remove_wave.push_back(
        {BuildPlan::Action::Type::Remove,
         std::filesystem::path{make_base_string(
             output_string, SCHEMAS_DIRECTORY, relative_string)},
         {},
         {}});
    remove_wave.push_back(
        {BuildPlan::Action::Type::Remove,
         std::filesystem::path{make_base_string(
             output_string, EXPLORER_DIRECTORY, relative_string)},
         {},
         {}});
  }

  for (const auto &schema : active_schemas) {
    if (schema.info->evaluate) {
      continue;
    }

    const auto schema_dirty{dirty_set.contains(schema.root_path)};
    if (schema_dirty || is_full) {
      for (const auto &rule : PER_SCHEMA_RULES) {
        if (rule.gate != TargetGate::IfEvaluate) {
          continue;
        }

        const auto &target_base{rule.base == TargetBase::Schema
                                    ? schema.schema_base
                                    : schema.explorer_base};
        auto target_path{append_filename(target_base, rule.filename)};
        if (entries.contains(target_path)) {
          remove_wave.push_back({BuildPlan::Action::Type::Remove,
                                 std::filesystem::path{std::move(target_path)},
                                 {},
                                 {}});
        }
      }
    }
  }

  if (has_potential_stale || !removed_uris.empty() || is_full) {
    std::unordered_set<std::string> known_bases;
    known_bases.reserve(active_schemas.size() * 2 + 1);
    for (const auto &schema : active_schemas) {
      known_bases.insert(schema.schema_base);
      known_bases.insert(schema.explorer_base);
      auto current{schema.info->relative_path};
      while (current.has_parent_path() && current.parent_path() != current) {
        current = current.parent_path();
        known_bases.insert(explorer_string + '/' + current.string() + '/' +
                           SENTINEL);
      }
    }
    known_bases.insert(explorer_string + '/' + SENTINEL);

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

    const auto output_prefix{output_string + "/"};

    std::unordered_set<std::string> global_skip_paths;
    for (const auto &rule : AGGREGATE_RULES) {
      if (rule.output_base == AggregateOutputBase::Explorer) {
        global_skip_paths.insert(explorer_string + '/' + SENTINEL + '/' +
                                 rule.output_filename);
      } else {
        global_skip_paths.insert(output_string + '/' + rule.output_filename);
      }
    }
    for (const auto &rule : GLOBAL_RULES) {
      global_skip_paths.insert(output_string + '/' + rule.filename);
    }

    std::unordered_set<std::string> stale_roots;
    for (const auto entry_key : entries.keys()) {
      const std::string entry_path{entry_key};
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
          if (known_bases.contains(parent) ||
              known_ancestors.contains(parent)) {
            break;
          }
          stale_end = slash_pos;
        }

        stale_roots.insert(entry_path.substr(0, stale_end));
      }
    }

    for (auto &root : stale_roots) {
      remove_wave.push_back({BuildPlan::Action::Type::Remove,
                             std::filesystem::path{root},
                             {},
                             {}});
    }
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
    for (const auto entry_path : entries.keys()) {
      if (!entry_path.starts_with(explorer_prefix)) {
        continue;
      }

      const auto last_slash{entry_path.rfind('/')};
      if (last_slash == std::string_view::npos) {
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
    for (const auto entry_path : entries.keys()) {
      if (!entry_path.starts_with(explorer_prefix)) {
        continue;
      }
      const auto last_slash{entry_path.rfind('/')};
      if (last_slash == std::string_view::npos) {
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
