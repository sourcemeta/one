#include <sourcemeta/core/test.h>
#include <sourcemeta/one/build.h>

#include "test_rules.h"

#include <chrono>     // std::chrono::nanoseconds, std::chrono::duration_cast
#include <cstdint>    // std::uint64_t
#include <filesystem> // std::filesystem::path
#include <string>     // std::string

// A build of one unchanging configuration and version
static constexpr sourcemeta::one::BuildState::InputsFingerprint INPUTS{
    0x0123456789abcdefULL};

// A build of some other configuration, or of the same one by another version
static constexpr sourcemeta::one::BuildState::InputsFingerprint OTHER_INPUTS{
    0xfedcba9876543210ULL};

static auto state_path(const std::string &name) -> std::filesystem::path {
  return std::filesystem::path{BINARY_DIRECTORY} / "state" / name;
}

TEST(round_trip_empty) {
  const auto path{state_path("empty")};
  std::filesystem::create_directories(path.parent_path());

  sourcemeta::one::BuildState original_entries;
  original_entries.configure(
      test_rules::RULES.leaves,
      sourcemeta::one::rules_fingerprint<test_rules::RULES>(), INPUTS,
      test_rules::RULES.sentinel);
  original_entries.save(path);

  sourcemeta::one::BuildState loaded_entries;
  loaded_entries.load(path, test_rules::RULES.leaves,
                      sourcemeta::one::rules_fingerprint<test_rules::RULES>(),
                      INPUTS, test_rules::RULES.sentinel);
  EXPECT_TRUE(loaded_entries.empty());
}

TEST(a_state_is_built_from_the_inputs_it_was_saved_under) {
  // The state is written once a build has finished, so it is the only record
  // that answers for what a build actually applied
  const auto path{state_path("inputs_same")};
  std::filesystem::create_directories(path.parent_path());

  sourcemeta::one::BuildState original_entries;
  original_entries.configure(
      test_rules::RULES.leaves,
      sourcemeta::one::rules_fingerprint<test_rules::RULES>(), INPUTS,
      test_rules::RULES.sentinel);
  original_entries.save(path);

  sourcemeta::one::BuildState loaded_entries;
  loaded_entries.load(path, test_rules::RULES.leaves,
                      sourcemeta::one::rules_fingerprint<test_rules::RULES>(),
                      INPUTS, test_rules::RULES.sentinel);
  EXPECT_TRUE(loaded_entries.built_from_these_inputs());
}

TEST(a_state_is_not_built_from_inputs_it_never_saw) {
  const auto path{state_path("inputs_other")};
  std::filesystem::create_directories(path.parent_path());

  const auto now{std::filesystem::file_time_type::clock::now()};
  sourcemeta::one::BuildState original_entries;
  original_entries.emplace("/output/schemas/foo/%/schema.metapack",
                           {.file_mark = now, .dependencies = {}});
  original_entries.configure(
      test_rules::RULES.leaves,
      sourcemeta::one::rules_fingerprint<test_rules::RULES>(), INPUTS,
      test_rules::RULES.sentinel);
  original_entries.save(path);

  // A configuration edited since, or a newer tool applying the same one. Either
  // way what sits beside this state was derived from something else, and
  // reading the anchor or the version as though it had been applied is what
  // leaves a policy declared and ungated
  sourcemeta::one::BuildState loaded_entries;
  loaded_entries.load(path, test_rules::RULES.leaves,
                      sourcemeta::one::rules_fingerprint<test_rules::RULES>(),
                      OTHER_INPUTS, test_rules::RULES.sentinel);
  EXPECT_FALSE(loaded_entries.built_from_these_inputs());

  // The mismatch withholds the records derived from those inputs, and nothing
  // else. What the state knows about the outputs already there is still needed
  // to tell which of them have to go
  EXPECT_EQ(loaded_entries.size(), 1);
  EXPECT_TRUE(loaded_entries.contains("/output/schemas/foo/%/schema.metapack"));
}

TEST(a_state_that_was_never_written_is_built_from_nothing) {
  // Nothing has finished here, so nothing beside it was derived from anything
  const auto path{state_path("inputs_absent")};
  std::filesystem::create_directories(path.parent_path());
  std::filesystem::remove(path);

  sourcemeta::one::BuildState entries;
  entries.load(path, test_rules::RULES.leaves,
               sourcemeta::one::rules_fingerprint<test_rules::RULES>(), INPUTS,
               test_rules::RULES.sentinel);
  EXPECT_FALSE(entries.built_from_these_inputs());
}

TEST(round_trip_single_entry_no_deps) {
  const auto path{state_path("single_no_deps")};
  std::filesystem::create_directories(path.parent_path());

  const auto now{std::filesystem::file_time_type::clock::now()};
  sourcemeta::one::BuildState original_entries;
  original_entries.emplace("/output/schemas/foo/%/schema.metapack",
                           {.file_mark = now, .dependencies = {}});

  original_entries.configure(
      test_rules::RULES.leaves,
      sourcemeta::one::rules_fingerprint<test_rules::RULES>(), INPUTS,
      test_rules::RULES.sentinel);
  original_entries.save(path);

  sourcemeta::one::BuildState loaded_entries;
  loaded_entries.load(path, test_rules::RULES.leaves,
                      sourcemeta::one::rules_fingerprint<test_rules::RULES>(),
                      INPUTS, test_rules::RULES.sentinel);
  EXPECT_EQ(loaded_entries.size(), 1);
  EXPECT_TRUE(loaded_entries.contains("/output/schemas/foo/%/schema.metapack"));

  const auto *result{
      loaded_entries.entry("/output/schemas/foo/%/schema.metapack")};
  EXPECT_NE(result, nullptr);
  EXPECT_TRUE(result->dependencies.empty());
}

TEST(round_trip_with_file_mark) {
  const auto path{state_path("with_mark")};
  std::filesystem::create_directories(path.parent_path());

  const auto now{std::filesystem::file_time_type::clock::now()};
  sourcemeta::one::BuildState original_entries;
  original_entries.emplace("/output/schemas/foo/%/schema.metapack",
                           {.file_mark = now, .dependencies = {}});

  original_entries.configure(
      test_rules::RULES.leaves,
      sourcemeta::one::rules_fingerprint<test_rules::RULES>(), INPUTS,
      test_rules::RULES.sentinel);
  original_entries.save(path);

  sourcemeta::one::BuildState loaded_entries;
  loaded_entries.load(path, test_rules::RULES.leaves,
                      sourcemeta::one::rules_fingerprint<test_rules::RULES>(),
                      INPUTS, test_rules::RULES.sentinel);
  EXPECT_EQ(loaded_entries.size(), 1);

  const auto *result{
      loaded_entries.entry("/output/schemas/foo/%/schema.metapack")};
  EXPECT_NE(result, nullptr);

  const auto original_ns{std::chrono::duration_cast<std::chrono::nanoseconds>(
                             now.time_since_epoch())
                             .count()};
  const auto loaded_ns{std::chrono::duration_cast<std::chrono::nanoseconds>(
                           result->file_mark.time_since_epoch())
                           .count()};
  EXPECT_EQ(original_ns, loaded_ns);
}

TEST(round_trip_with_dependencies) {
  const auto path{state_path("with_deps")};
  std::filesystem::create_directories(path.parent_path());

  const auto now{std::filesystem::file_time_type::clock::now()};
  sourcemeta::one::BuildState original_entries;
  original_entries.emplace(
      "/output/schemas/foo/%/dependencies.metapack",
      {.file_mark = now,
       .dependencies = {"/output/schemas/bar/%/schema.metapack",
                        "/output/schemas/baz/%/schema.metapack",
                        "/output/schemas/qux/%/schema.metapack"}});

  original_entries.configure(
      test_rules::RULES.leaves,
      sourcemeta::one::rules_fingerprint<test_rules::RULES>(), INPUTS,
      test_rules::RULES.sentinel);
  original_entries.save(path);

  sourcemeta::one::BuildState loaded_entries;
  loaded_entries.load(path, test_rules::RULES.leaves,
                      sourcemeta::one::rules_fingerprint<test_rules::RULES>(),
                      INPUTS, test_rules::RULES.sentinel);
  EXPECT_EQ(loaded_entries.size(), 1);

  const auto *result{
      loaded_entries.entry("/output/schemas/foo/%/dependencies.metapack")};
  EXPECT_NE(result, nullptr);
  EXPECT_EQ(result->dependencies.size(), 3);
  EXPECT_EQ(result->dependencies[0], "/output/schemas/bar/%/schema.metapack");
  EXPECT_EQ(result->dependencies[1], "/output/schemas/baz/%/schema.metapack");
  EXPECT_EQ(result->dependencies[2], "/output/schemas/qux/%/schema.metapack");
}

TEST(round_trip_multiple_entries) {
  const auto path{state_path("multiple")};
  std::filesystem::create_directories(path.parent_path());

  const auto now{std::filesystem::file_time_type::clock::now()};
  sourcemeta::one::BuildState original_entries;
  original_entries.emplace("/output/schemas/foo/%/schema.metapack",
                           {.file_mark = now, .dependencies = {}});
  original_entries.emplace(
      "/output/schemas/foo/%/dependencies.metapack",
      {.file_mark = now,
       .dependencies = {"/output/schemas/bar/%/schema.metapack"}});
  original_entries.emplace("/output/configuration.json",
                           {.file_mark = now, .dependencies = {}});

  original_entries.configure(
      test_rules::RULES.leaves,
      sourcemeta::one::rules_fingerprint<test_rules::RULES>(), INPUTS,
      test_rules::RULES.sentinel);
  original_entries.save(path);

  sourcemeta::one::BuildState loaded_entries;
  loaded_entries.load(path, test_rules::RULES.leaves,
                      sourcemeta::one::rules_fingerprint<test_rules::RULES>(),
                      INPUTS, test_rules::RULES.sentinel);
  EXPECT_EQ(loaded_entries.size(), 3);
  EXPECT_TRUE(loaded_entries.contains("/output/schemas/foo/%/schema.metapack"));
  EXPECT_TRUE(
      loaded_entries.contains("/output/schemas/foo/%/dependencies.metapack"));
  EXPECT_TRUE(loaded_entries.contains("/output/configuration.json"));

  const auto *dependencies_entry{
      loaded_entries.entry("/output/schemas/foo/%/dependencies.metapack")};
  EXPECT_NE(dependencies_entry, nullptr);
  EXPECT_EQ(dependencies_entry->dependencies.size(), 1);
}

TEST(forget_removes_children) {
  const auto now{std::filesystem::file_time_type::clock::now()};
  sourcemeta::one::BuildState entries;
  entries.emplace("/output/schemas/foo/%/schema.metapack",
                  {.file_mark = now, .dependencies = {}});
  entries.emplace("/output/schemas/foo/%/dependencies.metapack",
                  {.file_mark = now, .dependencies = {}});
  entries.emplace("/output/schemas/foo/%/locations.metapack",
                  {.file_mark = now, .dependencies = {}});
  entries.emplace("/output/schemas/bar/%/schema.metapack",
                  {.file_mark = now, .dependencies = {}});
  entries.emplace("/output/configuration.json",
                  {.file_mark = now, .dependencies = {}});
  EXPECT_EQ(entries.size(), 5);

  entries.forget("/output/schemas/foo/%");

  EXPECT_EQ(entries.size(), 2);
  EXPECT_FALSE(entries.contains("/output/schemas/foo/%/schema.metapack"));
  EXPECT_FALSE(entries.contains("/output/schemas/foo/%/dependencies.metapack"));
  EXPECT_FALSE(entries.contains("/output/schemas/foo/%/locations.metapack"));
  EXPECT_TRUE(entries.contains("/output/schemas/bar/%/schema.metapack"));
  EXPECT_TRUE(entries.contains("/output/configuration.json"));
}
