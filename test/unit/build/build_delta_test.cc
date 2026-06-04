#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/one/build.h>

#include "build_test_utils.h"
#include "test_rules.h"

#include <filesystem> // std::filesystem::path

TEST(Build_delta, full_empty_registry) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  const TestLeaves schemas;

  entries.configure(test_rules::RULES.leaves,
                    sourcemeta::one::rules_fingerprint<test_rules::RULES>(),
                    test_rules::RULES.sentinel);
  const auto plan{sourcemeta::one::delta<test_rules::RULES>(
      sourcemeta::one::BuildPhase::Produce, test_rules::MODE_FULL, entries,
      output, schemas, "1.0.0", false, "", "Full", {})};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, test_rules::MODE_FULL, 3, 4);

  EXPECT_ACTION(plan, 0, 0, 2, test_rules::ACTION_CONFIGURATION,
                output / "configuration.json", "");
  EXPECT_ACTION(plan, 0, 1, 2, test_rules::ACTION_VERSION,
                output / "version.json", "1.0.0");

  EXPECT_ACTION(plan, 1, 0, 1, test_rules::ACTION_ROUTES, output / "routes.bin",
                "Full", output / "configuration.json");

  EXPECT_ACTION(plan, 2, 0, 1, test_rules::ACTION_LISTING,
                output / "secondary" / "%" / "listing.bin", "");

  EXPECT_TOTAL_FILES(plan, entries, output / "configuration.json",
                     output / "version.json", output / "routes.bin",
                     output / "secondary" / "%" / "listing.bin");
}

TEST(Build_delta, full_single_leaf) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  const TestLeaves schemas{
      {"https://example.com/foo", "/src/foo.json", "foo", MTIME(100)}};

  entries.configure(test_rules::RULES.leaves,
                    sourcemeta::one::rules_fingerprint<test_rules::RULES>(),
                    test_rules::RULES.sentinel);
  const auto plan{sourcemeta::one::delta<test_rules::RULES>(
      sourcemeta::one::BuildPhase::Produce, test_rules::MODE_FULL, entries,
      output, schemas, "1.0.0", false, "", "Full", {})};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, test_rules::MODE_FULL, 5, 7);

  EXPECT_ACTION(plan, 0, 0, 2, test_rules::ACTION_CONFIGURATION,
                output / "configuration.json", "");
  EXPECT_ACTION(plan, 0, 1, 2, test_rules::ACTION_VERSION,
                output / "version.json", "1.0.0");

  EXPECT_ACTION(plan, 1, 0, 1, test_rules::ACTION_ROUTES, output / "routes.bin",
                "Full", output / "configuration.json");

  EXPECT_ACTION(plan, 2, 0, 1, test_rules::ACTION_PRIMARY,
                output / "primary" / "foo" / "%" / "primary.bin",
                "https://example.com/foo",
                std::filesystem::path{"/"} / "src" / "foo.json",
                output / "configuration.json");

  EXPECT_ACTION(plan, 3, 0, 1, test_rules::ACTION_METADATA,
                output / "secondary" / "foo" / "%" / "metadata.bin",
                "https://example.com/foo",
                output / "primary" / "foo" / "%" / "primary.bin");

  EXPECT_ACTION_UNORDERED(plan, 4, 0, 2, test_rules::ACTION_LISTING,
                          output / "secondary" / "%" / "listing.bin", "",
                          output / "secondary" / "foo" / "%" / "metadata.bin");
  EXPECT_ACTION_UNORDERED(plan, 4, 1, 2, test_rules::ACTION_WEB,
                          output / "secondary" / "foo" / "%" / "web.bin",
                          "https://example.com/foo",
                          output / "secondary" / "foo" / "%" / "metadata.bin");

  EXPECT_TOTAL_FILES(plan, entries, output / "configuration.json",
                     output / "version.json", output / "routes.bin",
                     output / "primary" / "foo" / "%" / "primary.bin",
                     output / "secondary" / "foo" / "%" / "metadata.bin",
                     output / "secondary" / "foo" / "%" / "web.bin",
                     output / "secondary" / "%" / "listing.bin");
}

TEST(Build_delta, full_single_leaf_headless_skips_full_only) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  const TestLeaves schemas{
      {"https://example.com/foo", "/src/foo.json", "foo", MTIME(100)}};

  entries.configure(test_rules::RULES.leaves,
                    sourcemeta::one::rules_fingerprint<test_rules::RULES>(),
                    test_rules::RULES.sentinel);
  const auto plan{sourcemeta::one::delta<test_rules::RULES>(
      sourcemeta::one::BuildPhase::Produce, test_rules::MODE_HEADLESS, entries,
      output, schemas, "1.0.0", false, "", "Headless", {})};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, test_rules::MODE_HEADLESS, 5,
                         6);

  EXPECT_ACTION(plan, 0, 0, 2, test_rules::ACTION_CONFIGURATION,
                output / "configuration.json", "");
  EXPECT_ACTION(plan, 0, 1, 2, test_rules::ACTION_VERSION,
                output / "version.json", "1.0.0");

  EXPECT_ACTION(plan, 1, 0, 1, test_rules::ACTION_ROUTES, output / "routes.bin",
                "Headless", output / "configuration.json");

  EXPECT_ACTION(plan, 2, 0, 1, test_rules::ACTION_PRIMARY,
                output / "primary" / "foo" / "%" / "primary.bin",
                "https://example.com/foo",
                std::filesystem::path{"/"} / "src" / "foo.json",
                output / "configuration.json");

  EXPECT_ACTION(plan, 3, 0, 1, test_rules::ACTION_METADATA,
                output / "secondary" / "foo" / "%" / "metadata.bin",
                "https://example.com/foo",
                output / "primary" / "foo" / "%" / "primary.bin");

  EXPECT_ACTION(plan, 4, 0, 1, test_rules::ACTION_LISTING,
                output / "secondary" / "%" / "listing.bin", "",
                output / "secondary" / "foo" / "%" / "metadata.bin");

  EXPECT_TOTAL_FILES(plan, entries, output / "configuration.json",
                     output / "version.json", output / "routes.bin",
                     output / "primary" / "foo" / "%" / "primary.bin",
                     output / "secondary" / "foo" / "%" / "metadata.bin",
                     output / "secondary" / "%" / "listing.bin");
}

TEST(Build_delta, full_nested_leaf_path) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  const TestLeaves schemas{
      {"https://example.com/a/b/c", "/src/abc.json", "a/b/c", MTIME(100)}};

  entries.configure(test_rules::RULES.leaves,
                    sourcemeta::one::rules_fingerprint<test_rules::RULES>(),
                    test_rules::RULES.sentinel);
  const auto plan{sourcemeta::one::delta<test_rules::RULES>(
      sourcemeta::one::BuildPhase::Produce, test_rules::MODE_FULL, entries,
      output, schemas, "1.0.0", false, "", "Full", {})};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, test_rules::MODE_FULL, 7, 9);

  EXPECT_ACTION(plan, 0, 0, 2, test_rules::ACTION_CONFIGURATION,
                output / "configuration.json", "");
  EXPECT_ACTION(plan, 0, 1, 2, test_rules::ACTION_VERSION,
                output / "version.json", "1.0.0");

  EXPECT_ACTION(plan, 1, 0, 1, test_rules::ACTION_ROUTES, output / "routes.bin",
                "Full", output / "configuration.json");

  EXPECT_ACTION(plan, 2, 0, 1, test_rules::ACTION_PRIMARY,
                output / "primary" / "a" / "b" / "c" / "%" / "primary.bin",
                "https://example.com/a/b/c",
                std::filesystem::path{"/"} / "src" / "abc.json",
                output / "configuration.json");

  EXPECT_ACTION(plan, 3, 0, 1, test_rules::ACTION_METADATA,
                output / "secondary" / "a" / "b" / "c" / "%" / "metadata.bin",
                "https://example.com/a/b/c",
                output / "primary" / "a" / "b" / "c" / "%" / "primary.bin");

  EXPECT_ACTION_UNORDERED(
      plan, 4, 0, 2, test_rules::ACTION_LISTING,
      output / "secondary" / "a" / "b" / "%" / "listing.bin", "",
      output / "secondary" / "a" / "b" / "c" / "%" / "metadata.bin");
  EXPECT_ACTION_UNORDERED(
      plan, 4, 1, 2, test_rules::ACTION_WEB,
      output / "secondary" / "a" / "b" / "c" / "%" / "web.bin",
      "https://example.com/a/b/c",
      output / "secondary" / "a" / "b" / "c" / "%" / "metadata.bin");

  EXPECT_ACTION(plan, 5, 0, 1, test_rules::ACTION_LISTING,
                output / "secondary" / "a" / "%" / "listing.bin", "",
                output / "secondary" / "a" / "b" / "%" / "listing.bin");

  EXPECT_ACTION(plan, 6, 0, 1, test_rules::ACTION_LISTING,
                output / "secondary" / "%" / "listing.bin", "",
                output / "secondary" / "a" / "%" / "listing.bin");

  EXPECT_TOTAL_FILES(plan, entries, output / "configuration.json",
                     output / "version.json", output / "routes.bin",
                     output / "primary" / "a" / "b" / "c" / "%" / "primary.bin",
                     output / "secondary" / "a" / "b" / "c" / "%" /
                         "metadata.bin",
                     output / "secondary" / "a" / "b" / "c" / "%" / "web.bin",
                     output / "secondary" / "a" / "b" / "%" / "listing.bin",
                     output / "secondary" / "a" / "%" / "listing.bin",
                     output / "secondary" / "%" / "listing.bin");
}

TEST(Build_delta, full_with_comment_emits_comment_global) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  const TestLeaves schemas;

  entries.configure(test_rules::RULES.leaves,
                    sourcemeta::one::rules_fingerprint<test_rules::RULES>(),
                    test_rules::RULES.sentinel);
  const auto plan{sourcemeta::one::delta<test_rules::RULES>(
      sourcemeta::one::BuildPhase::Produce, test_rules::MODE_FULL, entries,
      output, schemas, "1.0.0", false, "hello world", "Full", {})};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, test_rules::MODE_FULL, 3, 5);

  EXPECT_ACTION(plan, 0, 0, 3, test_rules::ACTION_COMMENT,
                output / "comment.json", "hello world");
  EXPECT_ACTION(plan, 0, 1, 3, test_rules::ACTION_CONFIGURATION,
                output / "configuration.json", "");
  EXPECT_ACTION(plan, 0, 2, 3, test_rules::ACTION_VERSION,
                output / "version.json", "1.0.0");

  EXPECT_ACTION(plan, 1, 0, 1, test_rules::ACTION_ROUTES, output / "routes.bin",
                "Full", output / "configuration.json");

  EXPECT_ACTION(plan, 2, 0, 1, test_rules::ACTION_LISTING,
                output / "secondary" / "%" / "listing.bin", "");

  EXPECT_TOTAL_FILES(plan, entries, output / "configuration.json",
                     output / "version.json", output / "comment.json",
                     output / "routes.bin",
                     output / "secondary" / "%" / "listing.bin");
}

TEST(Build_delta, full_without_comment_removes_stale_comment) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  const TestLeaves schemas;

  entries.emplace(output / "comment.json",
                  {.file_mark = MTIME(50), .dependencies = {}});
  entries.configure(test_rules::RULES.leaves,
                    sourcemeta::one::rules_fingerprint<test_rules::RULES>(),
                    test_rules::RULES.sentinel);
  const auto plan{sourcemeta::one::delta<test_rules::RULES>(
      sourcemeta::one::BuildPhase::Produce, test_rules::MODE_FULL, entries,
      output, schemas, "1.0.0", false, "", "Full", {})};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, test_rules::MODE_FULL, 4, 5);

  EXPECT_ACTION(plan, 0, 0, 2, test_rules::ACTION_CONFIGURATION,
                output / "configuration.json", "");
  EXPECT_ACTION(plan, 0, 1, 2, test_rules::ACTION_VERSION,
                output / "version.json", "1.0.0");

  EXPECT_ACTION(plan, 1, 0, 1, test_rules::ACTION_ROUTES, output / "routes.bin",
                "Full", output / "configuration.json");

  EXPECT_ACTION(plan, 2, 0, 1, test_rules::ACTION_LISTING,
                output / "secondary" / "%" / "listing.bin", "");

  EXPECT_ACTION(plan, 3, 0, 1, test_rules::ACTION_REMOVE,
                output / "comment.json", "");

  EXPECT_TOTAL_FILES(plan, entries, output / "configuration.json",
                     output / "version.json", output / "routes.bin",
                     output / "secondary" / "%" / "listing.bin");
}

TEST(Build_delta, full_multiple_leaves_emits_per_leaf_actions) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  const TestLeaves schemas{
      {"https://example.com/a", "/src/a.json", "a", MTIME(100)},
      {"https://example.com/b", "/src/b.json", "b", MTIME(100)}};

  entries.configure(test_rules::RULES.leaves,
                    sourcemeta::one::rules_fingerprint<test_rules::RULES>(),
                    test_rules::RULES.sentinel);
  const auto plan{sourcemeta::one::delta<test_rules::RULES>(
      sourcemeta::one::BuildPhase::Produce, test_rules::MODE_FULL, entries,
      output, schemas, "1.0.0", false, "", "Full", {})};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, test_rules::MODE_FULL, 5, 10);

  EXPECT_ACTION(plan, 0, 0, 2, test_rules::ACTION_CONFIGURATION,
                output / "configuration.json", "");
  EXPECT_ACTION(plan, 0, 1, 2, test_rules::ACTION_VERSION,
                output / "version.json", "1.0.0");

  EXPECT_ACTION(plan, 1, 0, 1, test_rules::ACTION_ROUTES, output / "routes.bin",
                "Full", output / "configuration.json");

  EXPECT_ACTION_UNORDERED(plan, 2, 0, 2, test_rules::ACTION_PRIMARY,
                          output / "primary" / "a" / "%" / "primary.bin",
                          "https://example.com/a",
                          std::filesystem::path{"/"} / "src" / "a.json",
                          output / "configuration.json");
  EXPECT_ACTION_UNORDERED(plan, 2, 1, 2, test_rules::ACTION_PRIMARY,
                          output / "primary" / "b" / "%" / "primary.bin",
                          "https://example.com/b",
                          std::filesystem::path{"/"} / "src" / "b.json",
                          output / "configuration.json");

  EXPECT_ACTION_UNORDERED(plan, 3, 0, 2, test_rules::ACTION_METADATA,
                          output / "secondary" / "a" / "%" / "metadata.bin",
                          "https://example.com/a",
                          output / "primary" / "a" / "%" / "primary.bin");
  EXPECT_ACTION_UNORDERED(plan, 3, 1, 2, test_rules::ACTION_METADATA,
                          output / "secondary" / "b" / "%" / "metadata.bin",
                          "https://example.com/b",
                          output / "primary" / "b" / "%" / "primary.bin");

  EXPECT_ACTION_UNORDERED(plan, 4, 0, 3, test_rules::ACTION_LISTING,
                          output / "secondary" / "%" / "listing.bin", "",
                          output / "secondary" / "a" / "%" / "metadata.bin",
                          output / "secondary" / "b" / "%" / "metadata.bin");
  EXPECT_ACTION_UNORDERED(plan, 4, 1, 3, test_rules::ACTION_WEB,
                          output / "secondary" / "a" / "%" / "web.bin",
                          "https://example.com/a",
                          output / "secondary" / "a" / "%" / "metadata.bin");
  EXPECT_ACTION_UNORDERED(plan, 4, 2, 3, test_rules::ACTION_WEB,
                          output / "secondary" / "b" / "%" / "web.bin",
                          "https://example.com/b",
                          output / "secondary" / "b" / "%" / "metadata.bin");

  EXPECT_TOTAL_FILES(plan, entries, output / "configuration.json",
                     output / "version.json", output / "routes.bin",
                     output / "primary" / "a" / "%" / "primary.bin",
                     output / "primary" / "b" / "%" / "primary.bin",
                     output / "secondary" / "a" / "%" / "metadata.bin",
                     output / "secondary" / "b" / "%" / "metadata.bin",
                     output / "secondary" / "a" / "%" / "web.bin",
                     output / "secondary" / "b" / "%" / "web.bin",
                     output / "secondary" / "%" / "listing.bin");
}

TEST(Build_delta, incremental_cached_globals_are_omitted) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  const TestLeaves schemas{
      {"https://example.com/foo", "/src/foo.json", "foo", MTIME(100)}};
  ADD_LEAF_ENTRIES(entries, output, "foo", true, MTIME(150));
  entries.emplace(output / "configuration.json",
                  {.file_mark = MTIME(150), .dependencies = {}});
  entries.emplace(output / "version.json",
                  {.file_mark = MTIME(150), .dependencies = {}});
  entries.emplace(output / "routes.bin",
                  {.file_mark = MTIME(150), .dependencies = {}});
  entries.emplace(output / "secondary" / "%" / "listing.bin",
                  {.file_mark = MTIME(150), .dependencies = {}});

  entries.configure(test_rules::RULES.leaves,
                    sourcemeta::one::rules_fingerprint<test_rules::RULES>(),
                    test_rules::RULES.sentinel);
  const auto plan{sourcemeta::one::delta<test_rules::RULES>(
      sourcemeta::one::BuildPhase::Produce, test_rules::MODE_FULL, entries,
      output, schemas, "1.0.0", true, "", "Full", {})};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, test_rules::MODE_FULL, 3, 4);

  EXPECT_ACTION(plan, 0, 0, 1, test_rules::ACTION_PRIMARY,
                output / "primary" / "foo" / "%" / "primary.bin",
                "https://example.com/foo",
                std::filesystem::path{"/"} / "src" / "foo.json",
                output / "configuration.json");

  EXPECT_ACTION(plan, 1, 0, 1, test_rules::ACTION_METADATA,
                output / "secondary" / "foo" / "%" / "metadata.bin",
                "https://example.com/foo",
                output / "primary" / "foo" / "%" / "primary.bin");

  EXPECT_ACTION_UNORDERED(plan, 2, 0, 2, test_rules::ACTION_LISTING,
                          output / "secondary" / "%" / "listing.bin", "",
                          output / "secondary" / "foo" / "%" / "metadata.bin");
  EXPECT_ACTION_UNORDERED(plan, 2, 1, 2, test_rules::ACTION_WEB,
                          output / "secondary" / "foo" / "%" / "web.bin",
                          "https://example.com/foo",
                          output / "secondary" / "foo" / "%" / "metadata.bin");
}

TEST(Build_delta, incremental_new_leaf_added_alongside_existing) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  const TestLeaves schemas{
      {"https://example.com/foo", "/src/foo.json", "foo", MTIME(100)},
      {"https://example.com/bar", "/src/bar.json", "bar", MTIME(200)}};
  ADD_LEAF_ENTRIES(entries, output, "foo", true, MTIME(150));
  entries.emplace(output / "configuration.json",
                  {.file_mark = MTIME(150), .dependencies = {}});
  entries.emplace(output / "version.json",
                  {.file_mark = MTIME(150), .dependencies = {}});
  entries.emplace(output / "routes.bin",
                  {.file_mark = MTIME(150), .dependencies = {}});
  entries.emplace(output / "secondary" / "%" / "listing.bin",
                  {.file_mark = MTIME(150), .dependencies = {}});

  entries.configure(test_rules::RULES.leaves,
                    sourcemeta::one::rules_fingerprint<test_rules::RULES>(),
                    test_rules::RULES.sentinel);
  const auto plan{sourcemeta::one::delta<test_rules::RULES>(
      sourcemeta::one::BuildPhase::Produce, test_rules::MODE_FULL, entries,
      output, schemas, "1.0.0", true, "", "Full", {})};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, test_rules::MODE_FULL, 3, 7);

  EXPECT_ACTION_UNORDERED(plan, 0, 0, 2, test_rules::ACTION_PRIMARY,
                          output / "primary" / "bar" / "%" / "primary.bin",
                          "https://example.com/bar",
                          std::filesystem::path{"/"} / "src" / "bar.json",
                          output / "configuration.json");
  EXPECT_ACTION_UNORDERED(plan, 0, 1, 2, test_rules::ACTION_PRIMARY,
                          output / "primary" / "foo" / "%" / "primary.bin",
                          "https://example.com/foo",
                          std::filesystem::path{"/"} / "src" / "foo.json",
                          output / "configuration.json");

  EXPECT_ACTION_UNORDERED(plan, 1, 0, 2, test_rules::ACTION_METADATA,
                          output / "secondary" / "bar" / "%" / "metadata.bin",
                          "https://example.com/bar",
                          output / "primary" / "bar" / "%" / "primary.bin");
  EXPECT_ACTION_UNORDERED(plan, 1, 1, 2, test_rules::ACTION_METADATA,
                          output / "secondary" / "foo" / "%" / "metadata.bin",
                          "https://example.com/foo",
                          output / "primary" / "foo" / "%" / "primary.bin");

  EXPECT_ACTION_UNORDERED(plan, 2, 0, 3, test_rules::ACTION_LISTING,
                          output / "secondary" / "%" / "listing.bin", "",
                          output / "secondary" / "bar" / "%" / "metadata.bin",
                          output / "secondary" / "foo" / "%" / "metadata.bin");
  EXPECT_ACTION_UNORDERED(plan, 2, 1, 3, test_rules::ACTION_WEB,
                          output / "secondary" / "bar" / "%" / "web.bin",
                          "https://example.com/bar",
                          output / "secondary" / "bar" / "%" / "metadata.bin");
  EXPECT_ACTION_UNORDERED(plan, 2, 2, 3, test_rules::ACTION_WEB,
                          output / "secondary" / "foo" / "%" / "web.bin",
                          "https://example.com/foo",
                          output / "secondary" / "foo" / "%" / "metadata.bin");
}

TEST(Build_delta, limits_zero_disables_check) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  const TestLeaves schemas{
      {"https://example.com/a", "/src/a.json", "a", MTIME(100)},
      {"https://example.com/b", "/src/b.json", "b", MTIME(100)},
      {"https://example.com/c", "/src/c.json", "c", MTIME(100)},
      {"https://example.com/d", "/src/d.json", "d", MTIME(100)}};

  entries.configure(test_rules::RULES.leaves,
                    sourcemeta::one::rules_fingerprint<test_rules::RULES>(),
                    test_rules::RULES.sentinel);
  EXPECT_NO_THROW({
    const auto plan{sourcemeta::one::delta<test_rules::RULES>(
        sourcemeta::one::BuildPhase::Produce, test_rules::MODE_FULL, entries,
        output, schemas, "1.0.0", false, "", "Full",
        {.maximum_direct_directory_entries = 0})};
    EXPECT_GT(plan.size, 0u);
  });
}

TEST(Build_delta, limits_within_threshold_succeeds) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  const TestLeaves schemas{
      {"https://example.com/a", "/src/a.json", "a", MTIME(100)},
      {"https://example.com/b", "/src/b.json", "b", MTIME(100)}};

  entries.configure(test_rules::RULES.leaves,
                    sourcemeta::one::rules_fingerprint<test_rules::RULES>(),
                    test_rules::RULES.sentinel);
  EXPECT_NO_THROW({
    const auto plan{sourcemeta::one::delta<test_rules::RULES>(
        sourcemeta::one::BuildPhase::Produce, test_rules::MODE_FULL, entries,
        output, schemas, "1.0.0", false, "", "Full",
        {.maximum_direct_directory_entries = 5})};
    EXPECT_GT(plan.size, 0u);
  });
}

TEST(Build_delta, limits_exceeded_throws) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  const TestLeaves schemas{
      {"https://example.com/a", "/src/a.json", "a", MTIME(100)},
      {"https://example.com/b", "/src/b.json", "b", MTIME(100)},
      {"https://example.com/c", "/src/c.json", "c", MTIME(100)}};

  entries.configure(test_rules::RULES.leaves,
                    sourcemeta::one::rules_fingerprint<test_rules::RULES>(),
                    test_rules::RULES.sentinel);
  EXPECT_THROW(
      {
        sourcemeta::one::delta<test_rules::RULES>(
            sourcemeta::one::BuildPhase::Produce, test_rules::MODE_FULL,
            entries, output, schemas, "1.0.0", false, "", "Full",
            {.maximum_direct_directory_entries = 2});
      },
      sourcemeta::one::BuildTooManyDirectoryEntriesError);
}
