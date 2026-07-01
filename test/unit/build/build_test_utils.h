#ifndef SOURCEMETA_ONE_BUILD_TEST_UTILS_H_
#define SOURCEMETA_ONE_BUILD_TEST_UTILS_H_

#include <sourcemeta/core/test.h>
#include <sourcemeta/one/build.h>

#include "test_rules.h"

#include <algorithm>  // std::ranges::sort
#include <cstddef>    // std::size_t
#include <filesystem> // std::filesystem::path, std::filesystem::file_time_type
#include <set>        // std::set
#include <string>     // std::string
#include <unordered_map> // std::unordered_map
#include <unordered_set> // std::unordered_set
#include <vector>        // std::vector

struct TestLeafEntry {
  std::string identifier;
  std::filesystem::path path;
  std::filesystem::path relative_path;
  std::filesystem::file_time_type mtime;
  bool evaluate{true};
};

class TestLeaves {
public:
  TestLeaves() = default;
  TestLeaves(std::initializer_list<TestLeafEntry> init)
      : storage_(init.begin(), init.end()) {
    refs_.reserve(storage_.size());
    for (const auto &entry : storage_) {
      refs_.emplace_back(
          std::string_view{entry.identifier},
          sourcemeta::one::LeafView{.path = &entry.path,
                                    .relative_path = &entry.relative_path,
                                    .mtime = entry.mtime,
                                    .evaluate = entry.evaluate});
    }
  }

  TestLeaves(const TestLeaves &) = delete;
  auto operator=(const TestLeaves &) -> TestLeaves & = delete;
  TestLeaves(TestLeaves &&) = delete;
  auto operator=(TestLeaves &&) -> TestLeaves & = delete;
  ~TestLeaves() = default;

  // NOLINTNEXTLINE(google-explicit-constructor)
  operator sourcemeta::one::LeafSet() const { return refs_; }

private:
  std::vector<TestLeafEntry> storage_;
  std::vector<std::pair<std::string_view, sourcemeta::one::LeafView>> refs_;
};

[[maybe_unused]] static auto MTIME(int seconds)
    -> std::filesystem::file_time_type {
  return std::filesystem::file_time_type{std::chrono::seconds{seconds}};
}

[[maybe_unused]] static auto
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
        EXPECT_FALSE(destinations.contains(dependency.string()));
      }
    }
  }
}

[[maybe_unused]] static auto
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
        EXPECT_FALSE(future_destinations.contains(dependency.string()));
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

[[maybe_unused]] static auto
__check_no_removed_references(const sourcemeta::one::BuildPlan &plan) -> void {
  std::vector<const std::filesystem::path *> remove_destinations;
  for (const auto &wave : plan.waves) {
    for (const auto &action : wave) {
      if (action.type == test_rules::ACTION_REMOVE) {
        remove_destinations.push_back(&action.destination);
      }
    }
  }

  for (const auto &wave : plan.waves) {
    for (const auto &action : wave) {
      if (action.type == test_rules::ACTION_REMOVE) {
        continue;
      }

      for (const auto *remove_path : remove_destinations) {
        EXPECT_FALSE(__is_under(action.destination, *remove_path));

        for (const auto &dependency : action.dependencies) {
          EXPECT_FALSE(__is_under(dependency, *remove_path));
        }
      }
    }
  }
}

[[maybe_unused]] static auto
__check_dependencies_resolvable(const sourcemeta::one::BuildPlan &plan,
                                const sourcemeta::one::BuildState &entries,
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
                    entries.contains(dependency_string));
      }
    }
  }
}

[[maybe_unused]] static auto
__check_no_duplicate_destinations(const sourcemeta::one::BuildPlan &plan)
    -> void {
  std::unordered_set<std::string> all_destinations;
  for (const auto &wave : plan.waves) {
    for (const auto &action : wave) {
      const auto &destination_string{action.destination.string()};
      EXPECT_FALSE(all_destinations.contains(destination_string));
      all_destinations.insert(destination_string);
    }
  }
}

[[maybe_unused]] static auto
ADD_LEAF_ENTRIES(sourcemeta::one::BuildState &entries,
                 const std::filesystem::path &output,
                 const std::filesystem::path &relative_output, const bool web,
                 const std::filesystem::file_time_type mark) -> void {
  const auto primary_base{output / "primary" / relative_output / "%"};
  const auto secondary_base{output / "secondary" / relative_output / "%"};
  entries.emplace(primary_base / "primary.bin",
                  {.file_mark = mark, .dependencies = {}});
  entries.emplace(secondary_base / "metadata.bin",
                  {.file_mark = mark, .dependencies = {}});
  if (web) {
    entries.emplace(secondary_base / "web.bin",
                    {.file_mark = mark, .dependencies = {}});
  }
}

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define EXPECT_CONSISTENT_PLAN(plan, entries, output, build_mode,              \
                               expected_waves, expected_size)                  \
  do {                                                                         \
    EXPECT_EQ((plan).output, (output));                                        \
    EXPECT_EQ((plan).type, (build_mode));                                      \
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
                      expected_dest, expected_data, ...)                       \
  do {                                                                         \
    EXPECT_EQ((plan).waves[(wave)].size(), (wave_size));                       \
    const auto &action_ref_##wave##_##index{(plan).waves[(wave)][(index)]};    \
    EXPECT_EQ(action_ref_##wave##_##index.type, (expected_type));              \
    EXPECT_EQ(action_ref_##wave##_##index.destination, (expected_dest));       \
    EXPECT_EQ(action_ref_##wave##_##index.data,                                \
              std::string_view{expected_data});                                \
    const std::vector<std::filesystem::path> expected_deps_##wave##_##index{   \
        __VA_ARGS__};                                                          \
    EXPECT_EQ(action_ref_##wave##_##index.dependencies,                        \
              expected_deps_##wave##_##index);                                 \
  } while (false)

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define EXPECT_ACTION_UNORDERED(plan, wave, index, wave_size, expected_type,   \
                                expected_dest, expected_data, ...)             \
  do {                                                                         \
    EXPECT_EQ((plan).waves[(wave)].size(), (wave_size));                       \
    const auto &action_ref_##wave##_##index{(plan).waves[(wave)][(index)]};    \
    EXPECT_EQ(action_ref_##wave##_##index.type, (expected_type));              \
    EXPECT_EQ(action_ref_##wave##_##index.destination, (expected_dest));       \
    EXPECT_EQ(action_ref_##wave##_##index.data,                                \
              std::string_view{expected_data});                                \
    auto actual_deps_##wave##_##index{                                         \
        action_ref_##wave##_##index.dependencies};                             \
    std::ranges::sort(actual_deps_##wave##_##index);                           \
    std::vector<std::filesystem::path> expected_deps_##wave##_##index{         \
        __VA_ARGS__};                                                          \
    std::ranges::sort(expected_deps_##wave##_##index);                         \
    EXPECT_EQ(actual_deps_##wave##_##index, expected_deps_##wave##_##index);   \
  } while (false)

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define EXPECT_TOTAL_FILES(plan, entries, ...)                                 \
  do {                                                                         \
    std::set<std::filesystem::path> total_files_result;                        \
    for (const auto total_files_key : (entries).keys()) {                      \
      total_files_result.insert(std::filesystem::path{total_files_key});       \
    }                                                                          \
    for (const auto &total_files_wave : (plan).waves) {                        \
      for (const auto &total_files_action : total_files_wave) {                \
        if (total_files_action.type == test_rules::ACTION_REMOVE) {            \
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
