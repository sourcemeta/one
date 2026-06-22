#include <sourcemeta/one/build.h>

#include <algorithm>     // std::ranges::sort, std::ranges::all_of
#include <cassert>       // assert
#include <cstdint>       // std::size_t
#include <span>          // std::span
#include <string>        // std::string
#include <string_view>   // std::string_view
#include <unordered_map> // std::unordered_map
#include <unordered_set> // std::unordered_set
#include <vector>        // std::vector

namespace sourcemeta::one {

static auto make_base_string(const std::string &output,
                             const std::string_view directory,
                             const std::string &relative_path,
                             const std::string_view sentinel) -> std::string {
  std::string result;
  result.reserve(output.size() + directory.size() + relative_path.size() +
                 sentinel.size() + 16);
  result += output;
  result += '/';
  result += directory;
  result += '/';
  result += relative_path;
  result += '/';
  result += sentinel;
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

static auto declare_leaf_targets(
    TargetMap &targets, std::span<const std::string> bases,
    const std::string &output_string, const std::string &source_string,
    const bool evaluate, const BuildPlan::Type build_type,
    const BuildPlan::Type full_mode, const std::string &configuration_string,
    const std::string_view uri, const BuildPhase phase,
    std::span<const LeafRule> leaf_rules) -> void {
  for (std::size_t index{0}; index < leaf_rules.size(); index++) {
    const auto &rule{leaf_rules[index]};

    if (rule.gate == TargetGate::IfEvaluate && !evaluate) {
      continue;
    }

    if (rule.gate == TargetGate::OnlyInFullMode && build_type != full_mode) {
      continue;
    }

    if (rule.combine_only && phase == BuildPhase::Produce) {
      continue;
    }

    const auto &base{bases[rule.base]};
    auto &target{declare_target_direct(
        targets, rule.action, append_filename(base, rule.filename), uri)};
    target.dependencies.reserve(rule.dependency_count);
    for (std::uint8_t dependency_index{0};
         dependency_index < rule.dependency_count; dependency_index++) {
      const auto &dependency{rule.dependencies[dependency_index]};
      switch (dependency.source) {
        case DependencySource::Base:
          target.dependencies.push_back(
              append_filename(bases[dependency.base], dependency.filename));
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
    const std::filesystem::path &primary_path,
    const std::vector<std::filesystem::path> &affected_relative_paths)
    -> std::vector<std::filesystem::path> {
  std::unordered_set<std::string> directory_set;
  directory_set.insert(primary_path.string());

  for (const auto &relative_path : affected_relative_paths) {
    auto current{(primary_path / relative_path).parent_path()};
    while (current != primary_path) {
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

auto delta_engine(const BuildPhase phase, const BuildPlan::Type build_type,
                  const BuildState &entries,
                  const std::filesystem::path &output, const LeafSet &leaves,
                  const std::string_view version, const bool incremental,
                  const std::string_view comment,
                  const std::string_view mode_label, const BuildLimits &limits,
                  std::span<const LeafRule> leaf_rules,
                  std::span<const ContainerRule> container_rules,
                  std::span<const GlobalRule> global_rules,
                  std::span<const std::string_view> directories,
                  const std::string_view sentinel,
                  const BuildPlan::Action::Type remove_action,
                  const BuildPlan::Type full_mode,
                  const DeltaRuleIndices &indices) -> BuildPlan {
  assert(output.is_absolute());
  assert(std::ranges::all_of(leaves, [](const auto &entry) {
    return entry.second.path->is_absolute() &&
           !entry.second.relative_path->is_absolute();
  }));
  assert(!version.empty());
  assert(directories.size() >= 2);
  const auto primary_directory{directories[0]};
  const auto secondary_directory{directories[1]};
  const std::string sentinel_separator{std::string{"/"} +
                                       std::string{sentinel} + "/"};
  const std::string dependencies_suffix{
      sentinel_separator + leaf_rules[indices.dependencies].filename};

  if (phase == BuildPhase::Combine) {
    const auto &output_string{output.native()};
    const auto primary_prefix{output_string + "/" +
                              std::string{primary_directory} + "/"};

    auto extract_cross_leaf_references{[&primary_prefix, &sentinel_separator](
                                           const BuildState::Entry *state_entry,
                                           std::string_view owner_base)
                                           -> std::unordered_set<std::string> {
      std::unordered_set<std::string> result;
      if (state_entry == nullptr) {
        return result;
      }

      for (const auto &dependency : state_entry->dependencies) {
        const auto &dependency_path{dependency.native()};
        if (!dependency_path.starts_with(primary_prefix)) {
          continue;
        }

        const auto sentinel_position{dependency_path.find(sentinel_separator)};
        if (sentinel_position == std::string::npos) {
          continue;
        }

        const auto relative{dependency_path.substr(
            primary_prefix.size(), sentinel_position - primary_prefix.size())};
        if (relative != owner_base) {
          result.insert(std::string{relative});
        }
      }

      return result;
    }};

    const auto owner_start{primary_prefix.size()};

    std::unordered_set<std::string> affected_leaves;
    for (const auto key : entries.keys()) {
      if (!key.ends_with(dependencies_suffix)) {
        continue;
      }

      if (!entries.in_overlay(std::string{key})) {
        continue;
      }

      const auto *new_entry{entries.entry(std::string{key})};
      const auto *old_entry{entries.disk_entry(std::string{key})};

      const auto owner_sentinel{key.find(sentinel_separator, owner_start)};
      if (owner_sentinel == std::string_view::npos) {
        continue;
      }

      const auto owner_base{
          key.substr(owner_start, owner_sentinel - owner_start)};
      const auto old_references{
          extract_cross_leaf_references(old_entry, owner_base)};
      const auto new_references{
          extract_cross_leaf_references(new_entry, owner_base)};

      for (const auto &reference : new_references) {
        if (!old_references.contains(reference)) {
          affected_leaves.insert(reference);
        }
      }

      for (const auto &reference : old_references) {
        if (!new_references.contains(reference)) {
          affected_leaves.insert(reference);
        }
      }

      if (old_entry == nullptr) {
        affected_leaves.insert(std::string{owner_base});
      }
    }

    for (const auto &deleted_key : entries.deleted_keys()) {
      if (!deleted_key.ends_with(dependencies_suffix)) {
        continue;
      }

      if (!deleted_key.starts_with(primary_prefix)) {
        continue;
      }

      const auto *old_entry{entries.raw_disk_entry(deleted_key)};
      if (old_entry == nullptr) {
        continue;
      }

      const auto owner_sentinel{
          deleted_key.find(sentinel_separator, owner_start)};
      if (owner_sentinel == std::string::npos) {
        continue;
      }

      const auto owner_base{
          deleted_key.substr(owner_start, owner_sentinel - owner_start)};
      for (const auto &reference :
           extract_cross_leaf_references(old_entry, owner_base)) {
        affected_leaves.insert(reference);
      }
    }

    BuildPlan plan;
    plan.output = output;
    plan.type = build_type;

    if (!affected_leaves.empty()) {
      std::unordered_map<std::string, std::vector<std::string>>
          reverse_dependency_index;
      for (const auto dependency_key : entries.keys()) {
        if (!dependency_key.ends_with(dependencies_suffix)) {
          continue;
        }

        const auto *dependency_entry{
            entries.entry(std::string{dependency_key})};
        if (dependency_entry == nullptr) {
          continue;
        }

        for (const auto &dependency : dependency_entry->dependencies) {
          const auto &dependency_path{dependency.native()};
          if (!dependency_path.starts_with(primary_prefix)) {
            continue;
          }

          const auto sentinel_position{
              dependency_path.find(sentinel_separator, owner_start)};
          if (sentinel_position == std::string::npos) {
            continue;
          }

          auto referenced_leaf{dependency_path.substr(
              owner_start, sentinel_position - owner_start)};
          if (affected_leaves.contains(referenced_leaf)) {
            reverse_dependency_index[std::move(referenced_leaf)].emplace_back(
                dependency_key);
          }
        }
      }

      for (auto &[leaf, dependency_keys] : reverse_dependency_index) {
        std::ranges::sort(dependency_keys);
        const auto [first, last] = std::ranges::unique(dependency_keys);
        dependency_keys.erase(first, last);
      }

      std::vector<BuildPlan::Action> dependents_wave;
      for (const auto &[uri, info] : leaves) {
        const auto &relative_string{info.relative_path->native()};
        if (!affected_leaves.contains(relative_string)) {
          continue;
        }

        auto primary_base{make_base_string(output_string, primary_directory,
                                           relative_string, sentinel)};
        auto destination{std::filesystem::path{append_filename(
            primary_base, leaf_rules[indices.dependents].filename)}};

        BuildPlan::Action::Dependencies action_dependencies;
        const auto reverse_iterator{
            reverse_dependency_index.find(relative_string)};
        if (reverse_iterator != reverse_dependency_index.end()) {
          action_dependencies.reserve(reverse_iterator->second.size());
          for (const auto &dependency_key : reverse_iterator->second) {
            action_dependencies.emplace_back(dependency_key);
          }
        }

        dependents_wave.push_back({leaf_rules[indices.dependents].action,
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

  const auto version_path{output / global_rules[indices.version].filename};
  const auto configuration_path{output /
                                global_rules[indices.configuration].filename};
  const auto comment_path{output / global_rules[indices.comment].filename};
  const auto is_full{!incremental};
  const auto primary_path{output / primary_directory};
  const auto secondary_path{output / secondary_directory};
  const auto comment_string{comment_path.string()};

  if (!is_full) {
    const auto &output_string{output.native()};
    bool fast_path_dirty{false};

    const auto mode_global_string{
        (output / global_rules[indices.mode_global].filename).string()};
    const auto had_full_mode{entries.contains(mode_global_string)};
    const auto needs_full_mode{build_type == full_mode};
    if (had_full_mode != needs_full_mode) {
      fast_path_dirty = true;
    }

    std::string primary_buffer;
    std::string secondary_buffer;
    primary_buffer.reserve(output_string.size() + 64);
    secondary_buffer.reserve(output_string.size() + 64);
    std::vector<std::string> current_bases;
    current_bases.reserve(leaves.size());

    if (!fast_path_dirty) {
      for (const auto &[uri, info] : leaves) {
        const auto &relative_string{info.relative_path->native()};

        primary_buffer.clear();
        primary_buffer += output_string;
        primary_buffer += '/';
        primary_buffer += primary_directory;
        primary_buffer += '/';
        primary_buffer += relative_string;
        primary_buffer += '/';
        primary_buffer += sentinel;
        current_bases.push_back(primary_buffer);

        const auto *leaf_entry{
            entries.leaf_state(output_string, relative_string, info.evaluate,
                               build_type == full_mode)};
        if (leaf_entry == nullptr) {
          fast_path_dirty = true;
          break;
        }

        if (info.mtime > leaf_entry->root_mtime) {
          fast_path_dirty = true;
          break;
        }

        std::uint16_t expected_bitmap{0};
        for (std::size_t rule_index{0}; rule_index < leaf_rules.size();
             rule_index++) {
          const auto &rule{leaf_rules[rule_index]};
          if (rule.gate == TargetGate::IfEvaluate && !info.evaluate) {
            continue;
          }

          if (rule.gate == TargetGate::OnlyInFullMode &&
              build_type != full_mode) {
            continue;
          }

          expected_bitmap |= static_cast<std::uint16_t>(1 << rule_index);
        }

        if ((leaf_entry->target_bitmap & expected_bitmap) != expected_bitmap) {
          fast_path_dirty = true;
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

      const auto primary_prefix{output_string + '/' +
                                std::string{primary_directory} + '/'};
      for (const auto entry_path : entries.keys()) {
        if (!entry_path.starts_with(primary_prefix)) {
          continue;
        }

        const auto sentinel_position{entry_path.find(sentinel_separator)};
        if (sentinel_position == std::string_view::npos) {
          continue;
        }

        if (!base_set.contains(entry_path.substr(0, sentinel_position + 2))) {
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
        plan.waves.push_back({{.type = global_rules[indices.comment].action,
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
        plan.waves.push_back({{.type = remove_action,
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
  targets.reserve(leaves.size() * leaf_rules.size());
  const auto &output_string{output.native()};
  const auto configuration_string{configuration_path.string()};
  const auto secondary_string{secondary_path.string()};

  struct ActiveLeaf {
    std::string_view uri;
    const LeafView *info;
    std::string primary_base;
    std::string secondary_base;
    std::string root_path;
  };

  std::vector<ActiveLeaf> active_leaves;
  active_leaves.reserve(leaves.size());
  std::vector<std::filesystem::path> all_relative_paths;
  all_relative_paths.reserve(leaves.size());

  std::unordered_set<std::string> dirty_relative_paths;
  for (const auto &[uri, info] : leaves) {
    const auto &relative_string{info.relative_path->native()};
    all_relative_paths.emplace_back(*info.relative_path);

    auto primary_base{make_base_string(output_string, primary_directory,
                                       relative_string, sentinel)};
    auto secondary_base{make_base_string(output_string, secondary_directory,
                                         relative_string, sentinel)};
    auto root_path{
        append_filename(primary_base, leaf_rules[indices.root].filename)};

    const auto *cached_leaf_state{
        is_full ? nullptr
                : entries.leaf_state(output_string, relative_string,
                                     info.evaluate, build_type == full_mode)};

    bool leaf_dirty{is_full || cached_leaf_state == nullptr};
    if (!leaf_dirty) {
      leaf_dirty = info.mtime > cached_leaf_state->root_mtime;
    }

    bool has_missing_targets{false};
    if (!leaf_dirty && cached_leaf_state != nullptr) {
      std::uint16_t expected_bitmap{0};
      for (std::size_t rule_index{0}; rule_index < leaf_rules.size();
           rule_index++) {
        const auto &rule{leaf_rules[rule_index]};
        if (rule.gate == TargetGate::IfEvaluate && !info.evaluate) {
          continue;
        }
        if (rule.gate == TargetGate::OnlyInFullMode &&
            build_type != full_mode) {
          continue;
        }
        expected_bitmap |= static_cast<std::uint16_t>(1 << rule_index);
      }
      has_missing_targets = (cached_leaf_state->target_bitmap &
                             expected_bitmap) != expected_bitmap;
    }

    const bool needs_targets{leaf_dirty || has_missing_targets ||
                             (cached_leaf_state != nullptr &&
                              cached_leaf_state->has_cross_leaf_deps)};

    if (leaf_dirty) {
      dirty_relative_paths.insert(relative_string);
    }

    if (needs_targets) {
      const std::array<std::string, 2> bases{{primary_base, secondary_base}};
      declare_leaf_targets(targets, bases, output_string, info.path->native(),
                           info.evaluate, build_type, full_mode,
                           configuration_string, uri, phase, leaf_rules);
    }

    active_leaves.push_back({uri, &info, std::move(primary_base),
                             std::move(secondary_base), std::move(root_path)});
  }

  std::unordered_set<std::string_view> force_dirty;
  for (const auto &leaf : active_leaves) {
    if (dirty_relative_paths.contains(leaf.info->relative_path->native())) {
      force_dirty.insert(leaf.root_path);
    }
  }

  std::unordered_set<std::string_view> current_primary_bases;
  current_primary_bases.reserve(active_leaves.size());
  for (const auto &leaf : active_leaves) {
    current_primary_bases.insert(leaf.primary_base);
  }

  std::unordered_set<std::string> removed_entries;
  {
    const auto state_leaves{entries.leaf_relative_paths(output_string)};
    for (const auto &state_relative : state_leaves) {
      const auto primary_base{make_base_string(output_string, primary_directory,
                                               state_relative, sentinel)};
      if (!current_primary_bases.contains(primary_base)) {
        const auto secondary_base{make_base_string(
            output_string, secondary_directory, state_relative, sentinel)};
        for (const auto &rule : leaf_rules) {
          const auto &base{rule.base == 0 ? primary_base : secondary_base};
          removed_entries.insert(append_filename(base, rule.filename));
        }
      }
    }
  }

  std::unordered_set<std::string> dirty_set;
  {
    for (const auto &leaf : active_leaves) {
      if (!force_dirty.contains(leaf.root_path)) {
        continue;
      }

      for (std::size_t index{0}; index < leaf_rules.size(); index++) {
        const auto &rule{leaf_rules[index]};
        if (rule.gate == TargetGate::IfEvaluate && !leaf.info->evaluate) {
          continue;
        }

        if (rule.gate == TargetGate::OnlyInFullMode &&
            build_type != full_mode) {
          continue;
        }

        const auto &base{rule.base == 0 ? leaf.primary_base
                                        : leaf.secondary_base};
        dirty_set.insert(append_filename(base, rule.filename));
      }
    }

    const auto primary_prefix_string{output_string + "/" +
                                     std::string{primary_directory} + "/"};
    const auto secondary_prefix_string{output_string + "/" +
                                       std::string{secondary_directory} + "/"};
    std::unordered_map<std::string_view, std::vector<std::string_view>>
        reverse_adjacency;
    std::unordered_map<std::string, std::vector<std::string>>
        reverse_state_dependencies;
    for (const auto &[target_path, target] : targets) {
      for (const auto &dependency : target.dependencies) {
        reverse_adjacency[dependency].push_back(target_path);
      }

      if (dirty_set.contains(target_path)) {
        continue;
      }

      const std::string_view target_view{target_path};
      std::string_view after_prefix;
      if (target_view.starts_with(primary_prefix_string)) {
        after_prefix = target_view.substr(primary_prefix_string.size());
      } else if (target_view.starts_with(secondary_prefix_string)) {
        after_prefix = target_view.substr(secondary_prefix_string.size());
      }

      if (!after_prefix.empty()) {
        const auto sentinel_position{after_prefix.find(sentinel_separator)};
        if (sentinel_position != std::string_view::npos) {
          const auto target_relative{after_prefix.substr(0, sentinel_position)};
          const auto *leaf_entry{entries.leaf_state(
              output_string, std::string{target_relative}, true, true)};
          if (leaf_entry != nullptr && !leaf_entry->has_cross_leaf_deps) {
            const auto target_filename{
                after_prefix.substr(sentinel_position + 3)};
            bool target_known{false};
            for (std::size_t rule_index{0}; rule_index < leaf_rules.size();
                 rule_index++) {
              const auto &rule{leaf_rules[rule_index]};
              const bool is_explorer{
                  target_view.starts_with(secondary_prefix_string)};
              if (target_filename == rule.filename &&
                  ((rule.base == 1) == is_explorer)) {
                target_known =
                    (leaf_entry->target_bitmap & (1 << rule_index)) != 0;
                break;
              }
            }

            if (!target_known) {
              dirty_set.insert(target_path);
            }

            continue;
          }
        }
      }

      const auto *state_entry{entries.entry(target_path)};
      if (state_entry == nullptr) {
        dirty_set.insert(target_path);
        continue;
      }

      for (const auto &dependency : state_entry->dependencies) {
        reverse_state_dependencies[dependency.native()].push_back(target_path);
      }
    }

    for (const auto &dirty_path : dirty_set) {
      const auto match{reverse_state_dependencies.find(dirty_path)};
      if (match != reverse_state_dependencies.end()) {
        for (const auto &dependent : match->second) {
          dirty_set.insert(dependent);
        }
      }
    }

    for (const auto &removed_path : removed_entries) {
      const auto match{reverse_state_dependencies.find(removed_path)};
      if (match != reverse_state_dependencies.end()) {
        for (const auto &dependent : match->second) {
          dirty_set.insert(dependent);
        }
      }
    }

    std::vector<std::string_view> worklist;
    worklist.reserve(dirty_set.size());
    for (const auto &dirty_path : dirty_set) {
      worklist.push_back(dirty_path);
    }

    for (std::size_t index{0}; index < worklist.size(); index++) {
      const auto match{reverse_adjacency.find(worklist[index])};
      if (match == reverse_adjacency.end()) {
        continue;
      }

      for (const auto &dependent : match->second) {
        if (dirty_set.insert(std::string{dependent}).second) {
          worklist.push_back(dependent);
        }
      }
    }
  }

  bool has_missing_mode_outputs{false};
  if (build_type == full_mode && dirty_set.empty() && !is_full) {
    for (const auto &leaf : active_leaves) {
      for (const auto &rule : leaf_rules) {
        if (rule.gate != TargetGate::OnlyInFullMode) {
          continue;
        }

        const auto &base{rule.base == 0 ? leaf.primary_base
                                        : leaf.secondary_base};
        if (!entries.contains(append_filename(base, rule.filename))) {
          has_missing_mode_outputs = true;
          break;
        }
      }

      if (has_missing_mode_outputs) {
        break;
      }
    }
  }

  bool has_stale_mode_outputs{false};
  if (build_type != full_mode && dirty_set.empty() && !is_full) {
    const auto secondary_prefix{secondary_path.string() + "/"};
    for (const auto entry_path : entries.keys()) {
      if (!entry_path.starts_with(secondary_prefix)) {
        continue;
      }

      const auto last_slash{entry_path.rfind('/')};
      if (last_slash == std::string_view::npos) {
        continue;
      }

      const std::string_view filename{entry_path.data() + last_slash + 1,
                                      entry_path.size() - last_slash - 1};
      for (const auto &rule : leaf_rules) {
        if (rule.gate == TargetGate::OnlyInFullMode &&
            filename == rule.filename) {
          has_stale_mode_outputs = true;
          break;
        }
      }

      if (!has_stale_mode_outputs) {
        for (const auto &rule : container_rules) {
          if (rule.gate == TargetGate::OnlyInFullMode &&
              filename == rule.filename) {
            has_stale_mode_outputs = true;
            break;
          }
        }
      }

      if (has_stale_mode_outputs) {
        break;
      }
    }
  }

  const auto has_potential_stale{!removed_entries.empty()};

  if (!is_full && dirty_set.empty() && !has_missing_mode_outputs &&
      !has_stale_mode_outputs && !has_potential_stale) {
    if (!comment.empty()) {
      BuildPlan plan;
      plan.output = output;
      plan.type = build_type;
      plan.waves.push_back({{.type = global_rules[indices.comment].action,
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
      plan.waves.push_back({{.type = remove_action,
                             .destination = comment_path,
                             .dependencies = {},
                             .data = {}}});
      plan.size = 1;
      return plan;
    }

    return {.output = output, .type = build_type, .waves = {}, .size = 0};
  }

  const auto has_leaf_work{is_full || !dirty_set.empty() ||
                           has_missing_mode_outputs || has_potential_stale};

  std::vector<std::filesystem::path> affected_relative_paths;
  if (has_leaf_work) {
    for (const auto &leaf : active_leaves) {
      if (is_full) {
        affected_relative_paths.emplace_back(*leaf.info->relative_path);
        continue;
      }

      if (dirty_set.contains(leaf.root_path)) {
        affected_relative_paths.emplace_back(*leaf.info->relative_path);
      }
    }

    auto has_graph_change{false};
    if (!has_graph_change) {
      for (const auto &leaf : active_leaves) {
        if (!entries.contains(leaf.root_path)) {
          has_graph_change = true;
          break;
        }
      }
    }

    for (const auto &leaf : active_leaves) {
      const auto leaf_is_dirty{dirty_set.contains(leaf.root_path)};

      for (std::size_t index{0}; index < leaf_rules.size(); index++) {
        const auto &rule{leaf_rules[index]};
        if (rule.dirty == DirtyOverride::Normal) {
          continue;
        }

        if (rule.gate == TargetGate::OnlyInFullMode &&
            build_type != full_mode) {
          continue;
        }

        const auto &base{rule.base == 0 ? leaf.primary_base
                                        : leaf.secondary_base};
        auto target_path_string{append_filename(base, rule.filename)};
        auto should_force{is_full || leaf_is_dirty ||
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

    const auto affected_directories{
        collect_affected_directories(primary_path, affected_relative_paths)};
    const auto all_directories{
        collect_affected_directories(primary_path, all_relative_paths)};

    for (const auto &rule : container_rules) {
      if (rule.gate == TargetGate::OnlyInFullMode && build_type != full_mode) {
        continue;
      }

      for (const auto &directory : affected_directories) {
        const auto relative{std::filesystem::relative(directory, primary_path)};
        const auto is_root_directory{relative == "."};

        if (rule.scope == ContainerScope::RootOnly && !is_root_directory) {
          continue;
        }

        if (rule.scope == ContainerScope::NonRoot && is_root_directory) {
          continue;
        }

        if (rule.only_full_rebuild && !is_full) {
          continue;
        }

        auto destination{(secondary_path / relative / sentinel / rule.filename)
                             .lexically_normal()
                             .string()};

        std::vector<std::string> rule_dependencies;

        for (std::uint8_t dependency_index{0};
             dependency_index < rule.dependency_count; dependency_index++) {
          const auto &dependency{rule.dependencies[dependency_index]};
          switch (dependency.kind) {
            case ContainerDependencyKind::LeafMetadata:
              for (const auto &leaf_relative : all_relative_paths) {
                if (leaf_relative.parent_path() == relative ||
                    (is_root_directory && !leaf_relative.has_parent_path())) {
                  rule_dependencies.push_back(append_filename(
                      make_base_string(output_string, secondary_directory,
                                       leaf_relative.native(), sentinel),
                      leaf_rules[indices.metadata].filename));
                }
              }
              break;
            case ContainerDependencyKind::ChildContainers:
              for (const auto &other_directory : affected_directories) {
                auto other_relative{
                    std::filesystem::relative(other_directory, primary_path)};
                if (other_relative == relative) {
                  continue;
                }

                auto other_parent{other_relative.parent_path()};
                if (other_parent.empty()) {
                  other_parent = ".";
                }

                if (other_parent == relative) {
                  rule_dependencies.push_back(
                      (secondary_path / other_relative / sentinel /
                       container_rules[indices.container_list].filename)
                          .lexically_normal()
                          .string());
                }
              }
              break;
            case ContainerDependencyKind::AllContainerListings:
              for (const auto &any_directory : all_directories) {
                const auto directory_relative{
                    std::filesystem::relative(any_directory, primary_path)};
                rule_dependencies.push_back(
                    (directory_relative == "."
                         ? secondary_path / sentinel /
                               container_rules[indices.container_list].filename
                         : secondary_path / directory_relative / sentinel /
                               container_rules[indices.container_list].filename)
                        .lexically_normal()
                        .string());
              }
              break;
            case ContainerDependencyKind::SameContainerTarget:
              rule_dependencies.push_back(
                  (secondary_path / relative / sentinel / dependency.filename)
                      .lexically_normal()
                      .string());
              break;
            case ContainerDependencyKind::ExternalConfig:
              rule_dependencies.push_back(configuration_string);
              break;
            case ContainerDependencyKind::Global:
              rule_dependencies.push_back(
                  (output / global_rules[indices.mode_global].filename)
                      .lexically_normal()
                      .string());
              break;
          }
        }

        if (rule.action == container_rules[indices.container_list].action &&
            limits.maximum_direct_directory_entries > 0 &&
            rule_dependencies.size() >
                limits.maximum_direct_directory_entries) {
          throw BuildTooManyDirectoryEntriesError(directory,
                                                  rule_dependencies.size());
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
      auto target_iterator{targets.find(target_path)};
      if (target_iterator == targets.end()) {
        continue;
      }

      auto &target{target_iterator->second};
      const auto wave_iterator{wave_cache.find(target_path)};
      const auto wave{wave_iterator != wave_cache.end() ? wave_iterator->second
                                                        : std::size_t{0}};
      BuildPlan::Action::Dependencies action_dependencies;
      action_dependencies.reserve(target.dependencies.size());
      for (auto &dependency : target.dependencies) {
        action_dependencies.emplace_back(std::move(dependency));
      }

      dag_waves[wave].push_back({target.action,
                                 std::filesystem::path{target_path},
                                 std::move(action_dependencies), target.data});
    }
  }

  std::vector<BuildPlan::Action> remove_wave;

  for (const auto &leaf : active_leaves) {
    if (leaf.info->evaluate) {
      continue;
    }

    const auto leaf_dirty{dirty_set.contains(leaf.root_path)};
    if (leaf_dirty || is_full) {
      for (const auto &rule : leaf_rules) {
        if (rule.gate != TargetGate::IfEvaluate) {
          continue;
        }

        const auto &target_base{rule.base == 0 ? leaf.primary_base
                                               : leaf.secondary_base};
        auto target_path{append_filename(target_base, rule.filename)};
        if (entries.contains(target_path)) {
          remove_wave.push_back({remove_action,
                                 std::filesystem::path{std::move(target_path)},
                                 {},
                                 {}});
        }
      }
    }
  }

  if (has_potential_stale || is_full) {
    std::unordered_set<std::string> known_bases;
    known_bases.reserve(active_leaves.size() * 2 + 1);
    for (const auto &leaf : active_leaves) {
      known_bases.insert(leaf.primary_base);
      known_bases.insert(leaf.secondary_base);
      auto current{*leaf.info->relative_path};
      while (current.has_parent_path() && current.parent_path() != current) {
        current = current.parent_path();
        known_bases.insert(secondary_string + '/' + current.string() + '/' +
                           std::string{sentinel});
      }
    }
    known_bases.insert(secondary_string + '/' + std::string{sentinel});

    std::unordered_set<std::string> known_ancestors;
    known_ancestors.reserve(known_bases.size() * 3);
    for (const auto &base : known_bases) {
      auto slash_position{base.rfind('/')};
      while (slash_position != std::string::npos && slash_position > 0) {
        const auto ancestor{base.substr(0, slash_position)};
        if (!known_ancestors.insert(ancestor).second) {
          break;
        }
        slash_position = ancestor.rfind('/');
      }
    }

    const auto output_prefix{output_string + "/"};

    std::unordered_set<std::string> global_skip_paths;
    for (const auto &rule : global_rules) {
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
      const auto sentinel_position{entry_path.find(sentinel_separator)};
      if (sentinel_position != std::string::npos) {
        is_known =
            known_bases.contains(entry_path.substr(0, sentinel_position + 2));
      }

      if (!is_known) {
        is_known = known_bases.contains(entry_path) ||
                   known_ancestors.contains(entry_path);
      }

      if (!is_known) {
        auto stale_end{entry_path.size()};
        while (true) {
          const auto slash_position{entry_path.rfind('/', stale_end - 1)};
          if (slash_position == std::string::npos ||
              slash_position < output_prefix.size()) {
            break;
          }
          const auto parent{entry_path.substr(0, slash_position)};
          if (known_bases.contains(parent) ||
              known_ancestors.contains(parent)) {
            break;
          }
          stale_end = slash_position;
        }

        stale_roots.insert(entry_path.substr(0, stale_end));
      }
    }

    for (auto &root : stale_roots) {
      remove_wave.push_back(
          {remove_action, std::filesystem::path{root}, {}, {}});
    }
  }

  std::unordered_set<std::string_view> mode_only_filenames;
  for (const auto &rule : leaf_rules) {
    if (rule.gate == TargetGate::OnlyInFullMode) {
      mode_only_filenames.insert(rule.filename);
    }
  }
  for (const auto &rule : container_rules) {
    if (rule.gate == TargetGate::OnlyInFullMode) {
      mode_only_filenames.insert(rule.filename);
    }
  }

  bool mode_removed{false};
  if (build_type != full_mode) {
    const auto secondary_prefix{secondary_path.string() + "/"};
    for (const auto entry_path : entries.keys()) {
      if (!entry_path.starts_with(secondary_prefix)) {
        continue;
      }

      const auto last_slash{entry_path.rfind('/')};
      if (last_slash == std::string_view::npos) {
        continue;
      }

      const std::string_view filename{entry_path.data() + last_slash + 1,
                                      entry_path.size() - last_slash - 1};
      if (mode_only_filenames.contains(filename)) {
        remove_wave.push_back(
            {remove_action, std::filesystem::path{entry_path}, {}, {}});
        mode_removed = true;
      }
    }
  }

  bool mode_added{false};
  if (build_type == full_mode && !is_full) {
    const auto secondary_prefix{secondary_path.string() + "/"};
    bool had_mode_entries{false};
    for (const auto entry_path : entries.keys()) {
      if (!entry_path.starts_with(secondary_prefix)) {
        continue;
      }
      const auto last_slash{entry_path.rfind('/')};
      if (last_slash == std::string_view::npos) {
        continue;
      }
      const std::string_view filename{entry_path.data() + last_slash + 1,
                                      entry_path.size() - last_slash - 1};
      if (mode_only_filenames.contains(filename)) {
        had_mode_entries = true;
        break;
      }
    }
    mode_added = !had_mode_entries;
  }

  BuildPlan plan;
  plan.output = output;
  plan.type = build_type;

  if (is_full) {
    std::vector<BuildPlan::Action> initialization_wave;
    initialization_wave.push_back({.type = global_rules[indices.version].action,
                                   .destination = version_path,
                                   .dependencies = {},
                                   .data = version});
    initialization_wave.push_back(
        {.type = global_rules[indices.configuration].action,
         .destination = configuration_path,
         .dependencies = {},
         .data = {}});

    // Globals derived purely from the configuration regenerate on every full
    // rebuild, which is exactly when the configuration or version changes. The
    // configuration anchor itself is already scheduled above. A global that
    // depends on another global's output cannot run in this wave, so it is
    // scheduled after the dependency below
    for (const auto &rule : global_rules) {
      if (rule.trigger != GlobalTrigger::FullRebuild ||
          rule.external_config_anchor || rule.dependency_count > 0) {
        continue;
      }

      initialization_wave.push_back({.type = rule.action,
                                     .destination = output / rule.filename,
                                     .dependencies = {},
                                     .data = {}});
    }

    if (!comment.empty()) {
      initialization_wave.push_back(
          {.type = global_rules[indices.comment].action,
           .destination = comment_path,
           .dependencies = {},
           .data = comment});
    } else if (entries.contains(comment_string)) {
      remove_wave.push_back({remove_action, comment_path, {}, {}});
    }
    plan.waves.push_back(std::move(initialization_wave));
  } else if (!comment.empty()) {
    plan.waves.push_back({{.type = global_rules[indices.comment].action,
                           .destination = comment_path,
                           .dependencies = {},
                           .data = comment}});
  } else if (entries.contains(comment_string)) {
    remove_wave.push_back({remove_action, comment_path, {}, {}});
  }

  if (is_full || mode_added || mode_removed) {
    std::vector<BuildPlan::Action> global_wave;
    for (const auto &rule : global_rules) {
      if (rule.trigger != GlobalTrigger::OnModeChange) {
        continue;
      }

      global_wave.push_back({rule.action,
                             output / rule.filename,
                             {configuration_path},
                             mode_label});
    }

    if (!global_wave.empty()) {
      plan.waves.push_back(std::move(global_wave));
    }
  }

  // Globals that depend on another global's output run after it, in their own
  // wave between the dependency-free globals above and the per-schema work
  // below. They regenerate on a full rebuild, when the configuration changes
  if (is_full) {
    std::vector<BuildPlan::Action> dependent_global_wave;
    for (const auto &rule : global_rules) {
      if (rule.trigger != GlobalTrigger::FullRebuild ||
          rule.external_config_anchor || rule.dependency_count == 0) {
        continue;
      }

      BuildPlan::Action::Dependencies dependencies;
      dependencies.reserve(rule.dependency_count);
      for (std::uint8_t dependency_index{0};
           dependency_index < rule.dependency_count; dependency_index++) {
        const auto &dependency{rule.dependencies[dependency_index]};
        switch (dependency.source) {
          case DependencySource::GlobalOutput:
            dependencies.push_back(output / dependency.filename);
            break;
          case DependencySource::ExternalConfig:
            dependencies.push_back(configuration_path);
            break;
          case DependencySource::Base:
          case DependencySource::ExternalSource:
            break;
        }
      }

      dependent_global_wave.push_back({.type = rule.action,
                                       .destination = output / rule.filename,
                                       .dependencies = std::move(dependencies),
                                       .data = {}});
    }

    if (!dependent_global_wave.empty()) {
      plan.waves.push_back(std::move(dependent_global_wave));
    }
  }

  for (auto &wave : dag_waves) {
    if (!wave.empty()) {
      plan.waves.push_back(std::move(wave));
    }
  }

  if (!remove_wave.empty()) {
    std::ranges::sort(remove_wave, [](const BuildPlan::Action &left,
                                      const BuildPlan::Action &right) {
      return left.destination < right.destination;
    });
    std::vector<BuildPlan::Action> deduplicated_removals;
    deduplicated_removals.reserve(remove_wave.size());
    for (auto &entry : remove_wave) {
      const auto &path{entry.destination.string()};
      bool is_child{false};
      for (const auto &kept : deduplicated_removals) {
        const auto &parent{kept.destination.string()};
        if (path.size() > parent.size() && path[parent.size()] == '/' &&
            path.starts_with(parent)) {
          is_child = true;
          break;
        }
      }

      if (!is_child) {
        deduplicated_removals.push_back(std::move(entry));
      }
    }

    plan.waves.push_back(std::move(deduplicated_removals));
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
