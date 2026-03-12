#include <sourcemeta/one/build.h>

#include <algorithm>     // std::ranges::sort, std::ranges::all_of
#include <cassert>       // assert
#include <string>        // std::string
#include <string_view>   // std::string_view
#include <unordered_map> // std::unordered_map
#include <unordered_set> // std::unordered_set
#include <vector>        // std::vector

// TODO: Refactor this mess

namespace sourcemeta::one {

static constexpr auto SENTINEL = "%";

static auto schema_base_path(const std::filesystem::path &output,
                             const std::filesystem::path &relative_path)
    -> std::filesystem::path {
  return output / "schemas" / relative_path / SENTINEL;
}

static auto explorer_base_path(const std::filesystem::path &output,
                               const std::filesystem::path &relative_path)
    -> std::filesystem::path {
  return output / "explorer" / relative_path / SENTINEL;
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
                       const bool evaluate,
                       const std::filesystem::path &configuration_path,
                       const std::string_view uri) -> void {
  const auto base{schema_base_path(output, relative_path)};
  const auto explorer_base{explorer_base_path(output, relative_path)};
  const auto schema_metapack{base / "schema.metapack"};
  const auto dependencies_metapack{base / "dependencies.metapack"};
  const auto bundle_metapack{base / "bundle.metapack"};
  const auto health_metapack{base / "health.metapack"};

  declare_target(targets, BuildPlan::Action::Type::Materialise, schema_metapack,
                 {source, configuration_path}, uri);
  declare_target(targets, BuildPlan::Action::Type::Positions,
                 base / "positions.metapack", {schema_metapack}, uri);
  declare_target(targets, BuildPlan::Action::Type::Locations,
                 base / "locations.metapack", {schema_metapack}, uri);
  declare_target(targets, BuildPlan::Action::Type::Dependencies,
                 dependencies_metapack, {schema_metapack}, uri);
  declare_target(targets, BuildPlan::Action::Type::Stats,
                 base / "stats.metapack", {schema_metapack}, uri);
  declare_target(targets, BuildPlan::Action::Type::Health, health_metapack,
                 {schema_metapack, dependencies_metapack}, uri);
  declare_target(targets, BuildPlan::Action::Type::Bundle, bundle_metapack,
                 {schema_metapack, dependencies_metapack}, uri);
  declare_target(targets, BuildPlan::Action::Type::Editor,
                 base / "editor.metapack", {bundle_metapack}, uri);

  if (evaluate) {
    declare_target(targets, BuildPlan::Action::Type::BlazeExhaustive,
                   base / "blaze-exhaustive.metapack", {bundle_metapack}, uri);
    declare_target(targets, BuildPlan::Action::Type::BlazeFast,
                   base / "blaze-fast.metapack", {bundle_metapack}, uri);
  }

  declare_target(targets, BuildPlan::Action::Type::SchemaMetadata,
                 explorer_base / "schema.metapack",
                 {schema_metapack, health_metapack, dependencies_metapack},
                 uri);
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

  std::size_t max_dep_wave{0};
  for (const auto &dependency : target_match->second.dependencies) {
    const auto &dep_string{dependency.string()};
    if (dirty_set.contains(dep_string)) {
      const auto dep_wave{compute_wave(dep_string, targets, dirty_set, cache)};
      if (dep_wave + 1 > max_dep_wave) {
        max_dep_wave = dep_wave + 1;
      }
    }
  }

  cache[path] = max_dep_wave;
  return max_dep_wave;
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
  const auto version_path{output / "version.json"};
  const auto configuration_path{output / "configuration.json"};
  const auto comment_path{output / "comment.json"};
  assert(current_version.empty() == !entries.contains(version_path.native()));
  assert(current_configuration.is_null() ==
         !entries.contains(configuration_path.native()));
  const auto version_changed{current_version != version};
  const auto configuration_changed{current_configuration.is_null() ||
                                   configuration != current_configuration};
  const auto is_full{version_changed || configuration_changed};
  const auto schemas_path{output / "schemas"};
  const auto explorer_path{output / "explorer"};
  const auto dependency_tree_path{output / "dependency-tree.metapack"};

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
                           info.evaluate, configuration_path, uri);
    all_relative_paths.emplace_back(info.relative_path);
  }

  std::unordered_set<std::string> force_dirty;

  if (is_full) {
    for (const auto &[uri, info] : schemas) {
      if (removed_uris.contains(uri)) {
        continue;
      }

      force_dirty.insert(
          (schema_base_path(output, info.relative_path) / "schema.metapack")
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
          (schema_base_path(output, info.relative_path) / "schema.metapack")
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
          (schema_base_path(output, info.relative_path) / "schema.metapack")
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

      const auto web_schema_path{
          (explorer_base_path(output, info.relative_path) /
           "schema-html.metapack")
              .string()};
      if (!entries.contains(web_schema_path)) {
        has_missing_web = true;
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
      !has_missing_web && !has_potential_stale) {
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
      if (dirty_set.contains((base / "schema.metapack").string())) {
        affected_relative_paths.emplace_back(info.relative_path);
      }
    }

    std::vector<std::filesystem::path> all_dependencies;
    all_dependencies.reserve(schemas.size());
    for (const auto &[uri, info] : schemas) {
      if (removed_uris.contains(uri)) {
        continue;
      }

      all_dependencies.emplace_back(
          schema_base_path(output, info.relative_path) /
          "dependencies.metapack");
    }

    declare_target(targets, BuildPlan::Action::Type::DependencyTree,
                   dependency_tree_path, all_dependencies);
    dirty_set.insert(dependency_tree_path.string());

    for (const auto &[uri, info] : schemas) {
      if (removed_uris.contains(uri)) {
        continue;
      }

      const auto dependents_path{schema_base_path(output, info.relative_path) /
                                 "dependents.metapack"};
      declare_target(targets, BuildPlan::Action::Type::Dependents,
                     dependents_path, {dependency_tree_path}, uri);

      const auto base{schema_base_path(output, info.relative_path)};
      if (is_full || dirty_set.contains((base / "schema.metapack").string()) ||
          !entries.contains(dependents_path.native())) {
        dirty_set.insert(dependents_path.string());
      }
    }

    std::vector<std::filesystem::path> all_summaries;
    all_summaries.reserve(schemas.size());
    for (const auto &[uri, info] : schemas) {
      if (removed_uris.contains(uri)) {
        continue;
      }

      all_summaries.emplace_back(
          explorer_base_path(output, info.relative_path) / "schema.metapack");
    }

    const auto search_path{explorer_path / SENTINEL / "search.metapack"};
    declare_target(targets, BuildPlan::Action::Type::SearchIndex, search_path,
                   all_summaries);
    dirty_set.insert(search_path.string());

    const auto affected_dirs{
        collect_affected_directories(schemas_path, affected_relative_paths)};
    for (const auto &directory : affected_dirs) {
      const auto relative{std::filesystem::relative(directory, schemas_path)};
      const auto destination{
          (explorer_path / relative / SENTINEL / "directory.metapack")
              .lexically_normal()};

      std::vector<std::filesystem::path> directory_dependencies;

      for (const auto &schema_relative : affected_relative_paths) {
        if (schema_relative.parent_path() == relative ||
            (relative == "." && !schema_relative.has_parent_path())) {
          directory_dependencies.emplace_back(
              explorer_base_path(output, schema_relative) / "schema.metapack");
        }
      }

      for (const auto &child_directory : affected_dirs) {
        if (child_directory == directory) {
          continue;
        }

        if (child_directory.parent_path() == directory) {
          const auto child_relative{
              std::filesystem::relative(child_directory, schemas_path)};
          directory_dependencies.emplace_back(
              (explorer_path / child_relative / SENTINEL / "directory.metapack")
                  .lexically_normal());
        }
      }

      declare_target(targets, BuildPlan::Action::Type::DirectoryList,
                     destination, std::move(directory_dependencies));
      dirty_set.insert(destination.string());
    }

    if (build_type == BuildPlan::Type::Full) {
      for (const auto &directory : affected_dirs) {
        const auto relative{std::filesystem::relative(directory, schemas_path)};
        const auto directory_metapack{
            (explorer_path / relative / SENTINEL / "directory.metapack")
                .lexically_normal()};
        if (relative == ".") {
          const auto web_index_path{explorer_path / SENTINEL /
                                    "directory-html.metapack"};
          declare_target(targets, BuildPlan::Action::Type::WebIndex,
                         web_index_path, {directory_metapack});
          dirty_set.insert(web_index_path.string());
          if (is_full) {
            const auto not_found_path{explorer_path / SENTINEL /
                                      "404.metapack"};
            declare_target(targets, BuildPlan::Action::Type::WebNotFound,
                           not_found_path, {configuration_path});
            dirty_set.insert(not_found_path.string());
          }
        } else {
          const auto web_dir_path{explorer_path / relative / SENTINEL /
                                  "directory-html.metapack"};
          declare_target(targets, BuildPlan::Action::Type::WebDirectory,
                         web_dir_path, {directory_metapack});
          dirty_set.insert(web_dir_path.string());
        }
      }

      for (const auto &[uri, info] : schemas) {
        if (removed_uris.contains(uri)) {
          continue;
        }

        const auto schema_base{schema_base_path(output, info.relative_path)};
        const auto explorer_base{
            explorer_base_path(output, info.relative_path)};
        const auto web_schema_path{explorer_base / "schema-html.metapack"};
        declare_target(targets, BuildPlan::Action::Type::WebSchema,
                       web_schema_path,
                       {explorer_base / "schema.metapack",
                        schema_base / "dependencies.metapack",
                        schema_base / "health.metapack",
                        schema_base / "dependents.metapack"},
                       uri);

        if (is_full ||
            dirty_set.contains((schema_base / "schema.metapack").string()) ||
            !entries.contains(web_schema_path.native())) {
          dirty_set.insert(web_schema_path.string());
        }
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
    const auto exhaustive_path{base / "blaze-exhaustive.metapack"};
    const auto fast_path{base / "blaze-fast.metapack"};
    const auto schema_dirty{
        dirty_set.contains((base / "schema.metapack").string())};
    if (schema_dirty || is_full) {
      if (entries.contains(exhaustive_path.native())) {
        remove_wave.push_back(
            {BuildPlan::Action::Type::Remove, exhaustive_path, {}, {}});
      }
      if (entries.contains(fast_path.native())) {
        remove_wave.push_back(
            {BuildPlan::Action::Type::Remove, fast_path, {}, {}});
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

  const auto output_prefix{output.string() + "/"};
  const auto version_string{version_path.string()};
  const auto dependency_tree_string{dependency_tree_path.string()};
  const auto routes_path_string{(output / "routes.bin").string()};
  const auto comment_string{comment_path.string()};
  const auto configuration_string{configuration_path.string()};

  std::unordered_set<std::string> stale_roots;
  for (const auto &[entry_path, entry] : entries) {
    if (!entry_path.starts_with(output_prefix)) {
      continue;
    }

    if (entry_path == version_string || entry_path == dependency_tree_string ||
        entry_path == routes_path_string || entry_path == comment_string ||
        entry_path == configuration_string) {
      continue;
    }

    bool is_known{false};
    for (const auto &base : known_bases) {
      if (entry_path == base || entry_path.starts_with(base + "/") ||
          base.starts_with(entry_path + "/")) {
        is_known = true;
        break;
      }
    }

    if (!is_known) {
      std::filesystem::path stale{entry_path};
      while (stale.has_parent_path()) {
        const auto parent{stale.parent_path()};
        if (parent == output) {
          break;
        }

        bool parent_is_known{false};
        const auto parent_string{parent.string()};
        for (const auto &base : known_bases) {
          if (parent_string == base || parent_string.starts_with(base + "/") ||
              base.starts_with(parent_string + "/")) {
            parent_is_known = true;
            break;
          }
        }

        if (parent_is_known) {
          break;
        }

        stale = parent;
      }

      stale_roots.insert(stale.string());
    }
  }

  for (auto &root : stale_roots) {
    remove_wave.push_back(
        {BuildPlan::Action::Type::Remove, std::filesystem::path{root}, {}, {}});
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
      if (filename == "directory-html.metapack" ||
          filename == "schema-html.metapack" || filename == "404.metapack") {
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
      if (filename == "directory-html.metapack" ||
          filename == "schema-html.metapack" || filename == "404.metapack") {
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
    std::vector<BuildPlan::Action> routes_wave;
    routes_wave.push_back(
        {BuildPlan::Action::Type::Routes,
         output / "routes.bin",
         {configuration_path},
         build_type == BuildPlan::Type::Full ? "Full" : "Headless"});
    plan.waves.push_back(std::move(routes_wave));
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
