#ifndef SOURCEMETA_ONE_BUILD_TEST_UTILS_H_
#define SOURCEMETA_ONE_BUILD_TEST_UTILS_H_

#include <gtest/gtest.h>

#include <sourcemeta/one/build.h>

#include <algorithm>  // std::sort
#include <cstddef>    // std::size_t
#include <filesystem> // std::filesystem::path, std::filesystem::file_time_type
#include <set>        // std::set
#include <string>     // std::string
#include <unordered_map> // std::unordered_map
#include <unordered_set> // std::unordered_set
#include <vector>        // std::vector

auto MTIME(int seconds) -> std::filesystem::file_time_type {
  return std::filesystem::file_time_type{std::chrono::seconds{seconds}};
}

static auto
__check_no_intra_wave_dependencies(const sourcemeta::one::BuildPlan &plan)
    -> void {
  for (std::size_t wave_index{0}; wave_index < plan.waves.size();
       wave_index++) {
    std::unordered_set<std::string> destinations;
    for (const auto &action : plan.waves[wave_index]) {
      destinations.insert(action.destination.string());
    }

    for (const auto &action : plan.waves[wave_index]) {
      for (const auto &dependency : action.dependencies) {
        EXPECT_FALSE(destinations.contains(dependency.string()))
            << "Wave " << wave_index << ": action "
            << action.destination.string() << " depends on "
            << dependency.string() << " which is produced in the same wave";
      }
    }
  }
}

static auto
__check_no_forward_dependencies(const sourcemeta::one::BuildPlan &plan)
    -> void {
  for (std::size_t wave_index{0}; wave_index < plan.waves.size();
       wave_index++) {
    std::unordered_set<std::string> future_destinations;
    for (std::size_t future{wave_index + 1}; future < plan.waves.size();
         future++) {
      for (const auto &action : plan.waves[future]) {
        future_destinations.insert(action.destination.string());
      }
    }

    for (const auto &action : plan.waves[wave_index]) {
      for (const auto &dependency : action.dependencies) {
        EXPECT_FALSE(future_destinations.contains(dependency.string()))
            << "Wave " << wave_index << ": action "
            << action.destination.string() << " depends on "
            << dependency.string() << " which is produced in a future wave";
      }
    }
  }
}

static auto __is_under(const std::filesystem::path &path,
                       const std::filesystem::path &base) -> bool {
  const auto &path_native{path.native()};
  const auto &base_native{base.native()};
  return path_native.size() > base_native.size() &&
         path_native.starts_with(base_native);
}

static auto
__check_no_removed_references(const sourcemeta::one::BuildPlan &plan) -> void {
  std::vector<const std::filesystem::path *> remove_destinations;
  for (const auto &wave : plan.waves) {
    for (const auto &action : wave) {
      if (action.type == sourcemeta::one::BuildAction::Remove) {
        remove_destinations.push_back(&action.destination);
      }
    }
  }

  for (const auto &wave : plan.waves) {
    for (const auto &action : wave) {
      if (action.type == sourcemeta::one::BuildAction::Remove) {
        continue;
      }

      for (const auto *remove_path : remove_destinations) {
        EXPECT_FALSE(__is_under(action.destination, *remove_path))
            << "Action " << action.destination.string()
            << " has destination under removed path " << remove_path->string();

        for (const auto &dependency : action.dependencies) {
          EXPECT_FALSE(__is_under(dependency, *remove_path))
              << "Action " << action.destination.string() << " depends on "
              << dependency.string() << " which is under removed path "
              << remove_path->string();
        }
      }
    }
  }
}

static auto
__check_dependencies_resolvable(const sourcemeta::one::BuildPlan &plan,
                                const sourcemeta::one::BuildEntries &entries,
                                const std::filesystem::path &output) -> void {
  const auto &output_prefix{output.native()};
  for (std::size_t wave_index{0}; wave_index < plan.waves.size();
       wave_index++) {
    std::unordered_set<std::string> prior_destinations;
    for (std::size_t prior{0}; prior < wave_index; prior++) {
      for (const auto &action : plan.waves[prior]) {
        prior_destinations.insert(action.destination.string());
      }
    }

    for (const auto &action : plan.waves[wave_index]) {
      for (const auto &dependency : action.dependencies) {
        const auto &dependency_string{dependency.string()};
        if (!dependency.native().starts_with(output_prefix)) {
          continue;
        }

        EXPECT_TRUE(prior_destinations.contains(dependency_string) ||
                    entries.contains(dependency_string))
            << "Wave " << wave_index << ": action "
            << action.destination.string() << " depends on "
            << dependency_string << " which is not produced by a previous wave"
            << " and not in entries";
      }
    }
  }
}

static auto
__check_no_duplicate_destinations(const sourcemeta::one::BuildPlan &plan)
    -> void {
  std::unordered_set<std::string> all_destinations;
  for (const auto &wave : plan.waves) {
    for (const auto &action : wave) {
      const auto &destination_string{action.destination.string()};
      EXPECT_FALSE(all_destinations.contains(destination_string))
          << "Duplicate destination: " << destination_string;
      all_destinations.insert(destination_string);
    }
  }
}

static auto __ADD_ENTRY_IMPL(sourcemeta::one::BuildEntries &entries,
                             const std::filesystem::path &,
                             const std::filesystem::path &path,
                             sourcemeta::one::BuildEntry entry) -> void {
  entries[path.string()] = std::move(entry);
}

static auto ADD_ENTRY(sourcemeta::one::BuildEntries &entries,
                      const std::filesystem::path &output,
                      const std::filesystem::path &path,
                      const std::filesystem::file_time_type mark) -> void {
  sourcemeta::one::BuildEntry entry;
  entry.file_mark = mark;
  __ADD_ENTRY_IMPL(entries, output, path, std::move(entry));
}

static auto ADD_ENTRY(sourcemeta::one::BuildEntries &entries,
                      const std::filesystem::path &output,
                      const std::filesystem::path &path,
                      sourcemeta::one::BuildEntry entry) -> void {
  __ADD_ENTRY_IMPL(entries, output, path, std::move(entry));
}

static auto ADD_SCHEMA_ENTRIES(sourcemeta::one::BuildEntries &entries,
                               const std::filesystem::path &output,
                               const std::filesystem::path &relative_output,
                               const bool evaluate, const bool web,
                               const std::filesystem::file_time_type mark)
    -> void {
  const auto base{output / "schemas" / relative_output / "%"};
  const auto explorer_base{output / "explorer" / relative_output / "%"};
  ADD_ENTRY(entries, output, base / "schema.metapack", mark);
  ADD_ENTRY(entries, output, base / "dependencies.metapack", mark);
  ADD_ENTRY(entries, output, base / "locations.metapack", mark);
  ADD_ENTRY(entries, output, base / "positions.metapack", mark);
  ADD_ENTRY(entries, output, base / "stats.metapack", mark);
  ADD_ENTRY(entries, output, base / "bundle.metapack", mark);
  ADD_ENTRY(entries, output, base / "health.metapack", mark);
  ADD_ENTRY(entries, output, base / "editor.metapack", mark);
  ADD_ENTRY(entries, output, base / "dependents.metapack", mark);
  ADD_ENTRY(entries, output, explorer_base / "schema.metapack", mark);
  if (evaluate) {
    ADD_ENTRY(entries, output, base / "blaze-exhaustive.metapack", mark);
    ADD_ENTRY(entries, output, base / "blaze-fast.metapack", mark);
  }
  if (web) {
    ADD_ENTRY(entries, output, explorer_base / "schema-html.metapack", mark);
    ADD_ENTRY(entries, output, explorer_base / "directory-html.metapack", mark);
  }
}

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define EXPECT_CONSISTENT_PLAN(plan, entries, output, build_type,              \
                               expected_waves, expected_size)                  \
  do {                                                                         \
    EXPECT_EQ((plan).output, (output));                                        \
    EXPECT_EQ((plan).type, sourcemeta::one::BuildType::build_type);            \
    EXPECT_EQ((plan).waves.size(), (expected_waves));                          \
    EXPECT_EQ((plan).size, (expected_size));                                   \
    __check_no_intra_wave_dependencies(plan);                                  \
    __check_no_forward_dependencies(plan);                                     \
    __check_no_removed_references(plan);                                       \
    __check_dependencies_resolvable(plan, entries, output);                    \
    __check_no_duplicate_destinations(plan);                                   \
  } while (false)

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define EXPECT_ACTION(plan, wave, index, wave_size, expected_type,             \
                      expected_dest, ...)                                      \
  do {                                                                         \
    EXPECT_EQ((plan).waves[(wave)].size(), (wave_size));                       \
    const auto &action_ref_##wave##_##index{(plan).waves[(wave)][(index)]};    \
    EXPECT_EQ(action_ref_##wave##_##index.type,                                \
              sourcemeta::one::BuildAction::expected_type);                    \
    EXPECT_EQ(action_ref_##wave##_##index.destination, (expected_dest));       \
    const std::vector<std::filesystem::path> expected_deps_##wave##_##index{   \
        __VA_ARGS__};                                                          \
    EXPECT_EQ(action_ref_##wave##_##index.dependencies,                        \
              expected_deps_##wave##_##index);                                 \
  } while (false)

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define EXPECT_ACTION_UNORDERED(plan, wave, index, wave_size, expected_type,   \
                                expected_dest, ...)                            \
  do {                                                                         \
    EXPECT_EQ((plan).waves[(wave)].size(), (wave_size));                       \
    const auto &action_ref_##wave##_##index{(plan).waves[(wave)][(index)]};    \
    EXPECT_EQ(action_ref_##wave##_##index.type,                                \
              sourcemeta::one::BuildAction::expected_type);                    \
    EXPECT_EQ(action_ref_##wave##_##index.destination, (expected_dest));       \
    auto actual_deps_##wave##_##index{                                         \
        action_ref_##wave##_##index.dependencies};                             \
    std::sort(actual_deps_##wave##_##index.begin(),                            \
              actual_deps_##wave##_##index.end());                             \
    std::vector<std::filesystem::path> expected_deps_##wave##_##index{         \
        __VA_ARGS__};                                                          \
    std::sort(expected_deps_##wave##_##index.begin(),                          \
              expected_deps_##wave##_##index.end());                           \
    EXPECT_EQ(actual_deps_##wave##_##index, expected_deps_##wave##_##index);   \
  } while (false)

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define EXPECT_TOTAL_FILES(plan, entries, ...)                                 \
  do {                                                                         \
    std::set<std::filesystem::path> total_files_result;                        \
    for (const auto &[total_files_path, total_files_entry] : (entries)) {      \
      total_files_result.insert(total_files_path);                             \
    }                                                                          \
    for (const auto &total_files_wave : (plan).waves) {                        \
      for (const auto &total_files_action : total_files_wave) {                \
        if (total_files_action.type == sourcemeta::one::BuildAction::Remove) { \
          total_files_result.erase(total_files_action.destination);            \
          const auto total_files_prefix{                                       \
              total_files_action.destination.string() + "/"};                  \
          for (auto total_files_iterator = total_files_result.begin();         \
               total_files_iterator != total_files_result.end();) {            \
            if (total_files_iterator->string().starts_with(                    \
                    total_files_prefix)) {                                     \
              total_files_iterator =                                           \
                  total_files_result.erase(total_files_iterator);              \
            } else {                                                           \
              total_files_iterator++;                                          \
            }                                                                  \
          }                                                                    \
        } else {                                                               \
          total_files_result.insert(total_files_action.destination);           \
        }                                                                      \
      }                                                                        \
    }                                                                          \
    const std::set<std::filesystem::path> total_files_expected{__VA_ARGS__};   \
    EXPECT_EQ(total_files_result, total_files_expected);                       \
  } while (false)

#endif
