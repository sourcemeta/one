#include <sourcemeta/core/json.h>
#include <sourcemeta/core/test.h>
#include <sourcemeta/one/build.h>

#include "build_test_utils.h"
#include "test_rules.h"

#include <filesystem> // std::filesystem::path
#include <string>     // std::string

// A build of one unchanging configuration and version
static constexpr sourcemeta::one::BuildState::InputsFingerprint INPUTS{
    0x0123456789abcdefULL};

static auto delta_path(const std::string &name) -> std::filesystem::path {
  return std::filesystem::path{BINARY_DIRECTORY} / "delta" / name;
}

TEST(full_empty_registry) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  const TestLeaves schemas;

  entries.configure(test_rules::RULES.leaves,
                    sourcemeta::one::rules_fingerprint<test_rules::RULES>(),
                    INPUTS, test_rules::RULES.sentinel);
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

  EXPECT_ACTION(plan, 2, 0, 1, test_rules::ACTION_GATE, output / "gate.bin", "",
                output / "routes.bin");

  EXPECT_ACTION(plan, 3, 0, 1, test_rules::ACTION_LISTING,
                output / "secondary" / "%" / "listing.bin", "");

  EXPECT_TOTAL_FILES(plan, entries, output / "configuration.json",
                     output / "version.json", output / "routes.bin",
                     output / "gate.bin",
                     output / "secondary" / "%" / "listing.bin");
}

TEST(full_single_leaf) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  const TestLeaves schemas{
      {"https://example.com/foo", "/src/foo.json", "foo", MTIME(100)}};

  entries.configure(test_rules::RULES.leaves,
                    sourcemeta::one::rules_fingerprint<test_rules::RULES>(),
                    INPUTS, test_rules::RULES.sentinel);
  const auto plan{sourcemeta::one::delta<test_rules::RULES>(
      sourcemeta::one::BuildPhase::Produce, test_rules::MODE_FULL, entries,
      output, schemas, "1.0.0", false, "", "Full", {})};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, test_rules::MODE_FULL, 6, 8);

  EXPECT_ACTION(plan, 0, 0, 2, test_rules::ACTION_CONFIGURATION,
                output / "configuration.json", "");
  EXPECT_ACTION(plan, 0, 1, 2, test_rules::ACTION_VERSION,
                output / "version.json", "1.0.0");

  EXPECT_ACTION(plan, 1, 0, 1, test_rules::ACTION_ROUTES, output / "routes.bin",
                "Full", output / "configuration.json");

  EXPECT_ACTION(plan, 2, 0, 1, test_rules::ACTION_GATE, output / "gate.bin", "",
                output / "routes.bin");

  EXPECT_ACTION(plan, 3, 0, 1, test_rules::ACTION_PRIMARY,
                output / "primary" / "foo" / "%" / "primary.bin",
                "https://example.com/foo",
                std::filesystem::path{"/"} / "src" / "foo.json",
                output / "configuration.json");

  EXPECT_ACTION(plan, 4, 0, 1, test_rules::ACTION_METADATA,
                output / "secondary" / "foo" / "%" / "metadata.bin",
                "https://example.com/foo",
                output / "primary" / "foo" / "%" / "primary.bin");

  EXPECT_ACTION_UNORDERED(plan, 5, 0, 2, test_rules::ACTION_LISTING,
                          output / "secondary" / "%" / "listing.bin", "",
                          output / "secondary" / "foo" / "%" / "metadata.bin");
  EXPECT_ACTION_UNORDERED(plan, 5, 1, 2, test_rules::ACTION_WEB,
                          output / "secondary" / "foo" / "%" / "web.bin",
                          "https://example.com/foo",
                          output / "secondary" / "foo" / "%" / "metadata.bin");

  EXPECT_TOTAL_FILES(plan, entries, output / "configuration.json",
                     output / "version.json", output / "routes.bin",
                     output / "gate.bin",
                     output / "primary" / "foo" / "%" / "primary.bin",
                     output / "secondary" / "foo" / "%" / "metadata.bin",
                     output / "secondary" / "foo" / "%" / "web.bin",
                     output / "secondary" / "%" / "listing.bin");
}

TEST(full_single_leaf_headless_skips_full_only) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  const TestLeaves schemas{
      {"https://example.com/foo", "/src/foo.json", "foo", MTIME(100)}};

  entries.configure(test_rules::RULES.leaves,
                    sourcemeta::one::rules_fingerprint<test_rules::RULES>(),
                    INPUTS, test_rules::RULES.sentinel);
  const auto plan{sourcemeta::one::delta<test_rules::RULES>(
      sourcemeta::one::BuildPhase::Produce, test_rules::MODE_HEADLESS, entries,
      output, schemas, "1.0.0", false, "", "Headless", {})};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, test_rules::MODE_HEADLESS, 6,
                         7);

  EXPECT_ACTION(plan, 0, 0, 2, test_rules::ACTION_CONFIGURATION,
                output / "configuration.json", "");
  EXPECT_ACTION(plan, 0, 1, 2, test_rules::ACTION_VERSION,
                output / "version.json", "1.0.0");

  EXPECT_ACTION(plan, 1, 0, 1, test_rules::ACTION_ROUTES, output / "routes.bin",
                "Headless", output / "configuration.json");

  EXPECT_ACTION(plan, 2, 0, 1, test_rules::ACTION_GATE, output / "gate.bin", "",
                output / "routes.bin");

  EXPECT_ACTION(plan, 3, 0, 1, test_rules::ACTION_PRIMARY,
                output / "primary" / "foo" / "%" / "primary.bin",
                "https://example.com/foo",
                std::filesystem::path{"/"} / "src" / "foo.json",
                output / "configuration.json");

  EXPECT_ACTION(plan, 4, 0, 1, test_rules::ACTION_METADATA,
                output / "secondary" / "foo" / "%" / "metadata.bin",
                "https://example.com/foo",
                output / "primary" / "foo" / "%" / "primary.bin");

  EXPECT_ACTION(plan, 5, 0, 1, test_rules::ACTION_LISTING,
                output / "secondary" / "%" / "listing.bin", "",
                output / "secondary" / "foo" / "%" / "metadata.bin");

  EXPECT_TOTAL_FILES(plan, entries, output / "configuration.json",
                     output / "version.json", output / "routes.bin",
                     output / "gate.bin",
                     output / "primary" / "foo" / "%" / "primary.bin",
                     output / "secondary" / "foo" / "%" / "metadata.bin",
                     output / "secondary" / "%" / "listing.bin");
}

TEST(full_nested_leaf_path) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  const TestLeaves schemas{
      {"https://example.com/a/b/c", "/src/abc.json", "a/b/c", MTIME(100)}};

  entries.configure(test_rules::RULES.leaves,
                    sourcemeta::one::rules_fingerprint<test_rules::RULES>(),
                    INPUTS, test_rules::RULES.sentinel);
  const auto plan{sourcemeta::one::delta<test_rules::RULES>(
      sourcemeta::one::BuildPhase::Produce, test_rules::MODE_FULL, entries,
      output, schemas, "1.0.0", false, "", "Full", {})};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, test_rules::MODE_FULL, 8, 10);

  EXPECT_ACTION(plan, 0, 0, 2, test_rules::ACTION_CONFIGURATION,
                output / "configuration.json", "");
  EXPECT_ACTION(plan, 0, 1, 2, test_rules::ACTION_VERSION,
                output / "version.json", "1.0.0");

  EXPECT_ACTION(plan, 1, 0, 1, test_rules::ACTION_ROUTES, output / "routes.bin",
                "Full", output / "configuration.json");

  EXPECT_ACTION(plan, 2, 0, 1, test_rules::ACTION_GATE, output / "gate.bin", "",
                output / "routes.bin");

  EXPECT_ACTION(plan, 3, 0, 1, test_rules::ACTION_PRIMARY,
                output / "primary" / "a" / "b" / "c" / "%" / "primary.bin",
                "https://example.com/a/b/c",
                std::filesystem::path{"/"} / "src" / "abc.json",
                output / "configuration.json");

  EXPECT_ACTION(plan, 4, 0, 1, test_rules::ACTION_METADATA,
                output / "secondary" / "a" / "b" / "c" / "%" / "metadata.bin",
                "https://example.com/a/b/c",
                output / "primary" / "a" / "b" / "c" / "%" / "primary.bin");

  EXPECT_ACTION_UNORDERED(
      plan, 5, 0, 2, test_rules::ACTION_LISTING,
      output / "secondary" / "a" / "b" / "%" / "listing.bin", "",
      output / "secondary" / "a" / "b" / "c" / "%" / "metadata.bin");
  EXPECT_ACTION_UNORDERED(
      plan, 5, 1, 2, test_rules::ACTION_WEB,
      output / "secondary" / "a" / "b" / "c" / "%" / "web.bin",
      "https://example.com/a/b/c",
      output / "secondary" / "a" / "b" / "c" / "%" / "metadata.bin");

  EXPECT_ACTION(plan, 6, 0, 1, test_rules::ACTION_LISTING,
                output / "secondary" / "a" / "%" / "listing.bin", "",
                output / "secondary" / "a" / "b" / "%" / "listing.bin");

  EXPECT_ACTION(plan, 7, 0, 1, test_rules::ACTION_LISTING,
                output / "secondary" / "%" / "listing.bin", "",
                output / "secondary" / "a" / "%" / "listing.bin");

  EXPECT_TOTAL_FILES(
      plan, entries, output / "configuration.json", output / "version.json",
      output / "routes.bin", output / "gate.bin",
      output / "primary" / "a" / "b" / "c" / "%" / "primary.bin",
      output / "secondary" / "a" / "b" / "c" / "%" / "metadata.bin",
      output / "secondary" / "a" / "b" / "c" / "%" / "web.bin",
      output / "secondary" / "a" / "b" / "%" / "listing.bin",
      output / "secondary" / "a" / "%" / "listing.bin",
      output / "secondary" / "%" / "listing.bin");
}

TEST(full_with_comment_emits_comment_global) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  const TestLeaves schemas;

  entries.configure(test_rules::RULES.leaves,
                    sourcemeta::one::rules_fingerprint<test_rules::RULES>(),
                    INPUTS, test_rules::RULES.sentinel);
  const auto plan{sourcemeta::one::delta<test_rules::RULES>(
      sourcemeta::one::BuildPhase::Produce, test_rules::MODE_FULL, entries,
      output, schemas, "1.0.0", false, "hello world", "Full", {})};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, test_rules::MODE_FULL, 4, 6);

  EXPECT_ACTION(plan, 0, 0, 3, test_rules::ACTION_COMMENT,
                output / "comment.json", "hello world");
  EXPECT_ACTION(plan, 0, 1, 3, test_rules::ACTION_CONFIGURATION,
                output / "configuration.json", "");
  EXPECT_ACTION(plan, 0, 2, 3, test_rules::ACTION_VERSION,
                output / "version.json", "1.0.0");

  EXPECT_ACTION(plan, 1, 0, 1, test_rules::ACTION_ROUTES, output / "routes.bin",
                "Full", output / "configuration.json");

  EXPECT_ACTION(plan, 2, 0, 1, test_rules::ACTION_GATE, output / "gate.bin", "",
                output / "routes.bin");

  EXPECT_ACTION(plan, 3, 0, 1, test_rules::ACTION_LISTING,
                output / "secondary" / "%" / "listing.bin", "");

  EXPECT_TOTAL_FILES(plan, entries, output / "configuration.json",
                     output / "version.json", output / "comment.json",
                     output / "routes.bin", output / "gate.bin",
                     output / "secondary" / "%" / "listing.bin");
}

TEST(full_without_comment_removes_stale_comment) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  const TestLeaves schemas;

  entries.emplace(output / "comment.json",
                  {.file_mark = MTIME(50), .dependencies = {}});
  entries.configure(test_rules::RULES.leaves,
                    sourcemeta::one::rules_fingerprint<test_rules::RULES>(),
                    INPUTS, test_rules::RULES.sentinel);
  const auto plan{sourcemeta::one::delta<test_rules::RULES>(
      sourcemeta::one::BuildPhase::Produce, test_rules::MODE_FULL, entries,
      output, schemas, "1.0.0", false, "", "Full", {})};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, test_rules::MODE_FULL, 5, 6);

  EXPECT_ACTION(plan, 0, 0, 2, test_rules::ACTION_CONFIGURATION,
                output / "configuration.json", "");
  EXPECT_ACTION(plan, 0, 1, 2, test_rules::ACTION_VERSION,
                output / "version.json", "1.0.0");

  EXPECT_ACTION(plan, 1, 0, 1, test_rules::ACTION_ROUTES, output / "routes.bin",
                "Full", output / "configuration.json");

  EXPECT_ACTION(plan, 2, 0, 1, test_rules::ACTION_GATE, output / "gate.bin", "",
                output / "routes.bin");

  EXPECT_ACTION(plan, 3, 0, 1, test_rules::ACTION_LISTING,
                output / "secondary" / "%" / "listing.bin", "");

  EXPECT_ACTION(plan, 4, 0, 1, test_rules::ACTION_REMOVE,
                output / "comment.json", "");

  EXPECT_TOTAL_FILES(plan, entries, output / "configuration.json",
                     output / "version.json", output / "routes.bin",
                     output / "gate.bin",
                     output / "secondary" / "%" / "listing.bin");
}

TEST(full_multiple_leaves_emits_per_leaf_actions) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  const TestLeaves schemas{
      {"https://example.com/a", "/src/a.json", "a", MTIME(100)},
      {"https://example.com/b", "/src/b.json", "b", MTIME(100)}};

  entries.configure(test_rules::RULES.leaves,
                    sourcemeta::one::rules_fingerprint<test_rules::RULES>(),
                    INPUTS, test_rules::RULES.sentinel);
  const auto plan{sourcemeta::one::delta<test_rules::RULES>(
      sourcemeta::one::BuildPhase::Produce, test_rules::MODE_FULL, entries,
      output, schemas, "1.0.0", false, "", "Full", {})};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, test_rules::MODE_FULL, 6, 11);

  EXPECT_ACTION(plan, 0, 0, 2, test_rules::ACTION_CONFIGURATION,
                output / "configuration.json", "");
  EXPECT_ACTION(plan, 0, 1, 2, test_rules::ACTION_VERSION,
                output / "version.json", "1.0.0");

  EXPECT_ACTION(plan, 1, 0, 1, test_rules::ACTION_ROUTES, output / "routes.bin",
                "Full", output / "configuration.json");

  EXPECT_ACTION(plan, 2, 0, 1, test_rules::ACTION_GATE, output / "gate.bin", "",
                output / "routes.bin");

  EXPECT_ACTION_UNORDERED(plan, 3, 0, 2, test_rules::ACTION_PRIMARY,
                          output / "primary" / "a" / "%" / "primary.bin",
                          "https://example.com/a",
                          std::filesystem::path{"/"} / "src" / "a.json",
                          output / "configuration.json");
  EXPECT_ACTION_UNORDERED(plan, 3, 1, 2, test_rules::ACTION_PRIMARY,
                          output / "primary" / "b" / "%" / "primary.bin",
                          "https://example.com/b",
                          std::filesystem::path{"/"} / "src" / "b.json",
                          output / "configuration.json");

  EXPECT_ACTION_UNORDERED(plan, 4, 0, 2, test_rules::ACTION_METADATA,
                          output / "secondary" / "a" / "%" / "metadata.bin",
                          "https://example.com/a",
                          output / "primary" / "a" / "%" / "primary.bin");
  EXPECT_ACTION_UNORDERED(plan, 4, 1, 2, test_rules::ACTION_METADATA,
                          output / "secondary" / "b" / "%" / "metadata.bin",
                          "https://example.com/b",
                          output / "primary" / "b" / "%" / "primary.bin");

  EXPECT_ACTION_UNORDERED(plan, 5, 0, 3, test_rules::ACTION_LISTING,
                          output / "secondary" / "%" / "listing.bin", "",
                          output / "secondary" / "a" / "%" / "metadata.bin",
                          output / "secondary" / "b" / "%" / "metadata.bin");
  EXPECT_ACTION_UNORDERED(plan, 5, 1, 3, test_rules::ACTION_WEB,
                          output / "secondary" / "a" / "%" / "web.bin",
                          "https://example.com/a",
                          output / "secondary" / "a" / "%" / "metadata.bin");
  EXPECT_ACTION_UNORDERED(plan, 5, 2, 3, test_rules::ACTION_WEB,
                          output / "secondary" / "b" / "%" / "web.bin",
                          "https://example.com/b",
                          output / "secondary" / "b" / "%" / "metadata.bin");

  EXPECT_TOTAL_FILES(plan, entries, output / "configuration.json",
                     output / "version.json", output / "routes.bin",
                     output / "gate.bin",
                     output / "primary" / "a" / "%" / "primary.bin",
                     output / "primary" / "b" / "%" / "primary.bin",
                     output / "secondary" / "a" / "%" / "metadata.bin",
                     output / "secondary" / "b" / "%" / "metadata.bin",
                     output / "secondary" / "a" / "%" / "web.bin",
                     output / "secondary" / "b" / "%" / "web.bin",
                     output / "secondary" / "%" / "listing.bin");
}

TEST(incremental_cached_globals_are_omitted) {
  const auto output{delta_path("cached_globals")};
  WRITE_GLOBAL_OUTPUTS(output);
  sourcemeta::one::BuildState entries;
  const TestLeaves schemas{
      {"https://example.com/foo", "/src/foo.json", "foo", MTIME(100)}};
  ADD_LEAF_ENTRIES(entries, output, "foo", true, MTIME(150));
  ADD_GLOBAL_ENTRIES(entries, output, MTIME(150));
  entries.emplace(output / "secondary" / "%" / "listing.bin",
                  {.file_mark = MTIME(150), .dependencies = {}});

  entries.configure(test_rules::RULES.leaves,
                    sourcemeta::one::rules_fingerprint<test_rules::RULES>(),
                    INPUTS, test_rules::RULES.sentinel);
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

TEST(incremental_new_leaf_added_alongside_existing) {
  const auto output{delta_path("new_leaf_alongside")};
  WRITE_GLOBAL_OUTPUTS(output);
  sourcemeta::one::BuildState entries;
  const TestLeaves schemas{
      {"https://example.com/foo", "/src/foo.json", "foo", MTIME(100)},
      {"https://example.com/bar", "/src/bar.json", "bar", MTIME(200)}};
  ADD_LEAF_ENTRIES(entries, output, "foo", true, MTIME(150));
  ADD_GLOBAL_ENTRIES(entries, output, MTIME(150));
  entries.emplace(output / "secondary" / "%" / "listing.bin",
                  {.file_mark = MTIME(150), .dependencies = {}});

  entries.configure(test_rules::RULES.leaves,
                    sourcemeta::one::rules_fingerprint<test_rules::RULES>(),
                    INPUTS, test_rules::RULES.sentinel);
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

TEST(incremental_missing_version_global_is_repaired) {
  const auto output{delta_path("missing_version")};
  WRITE_GLOBAL_OUTPUTS(output);
  std::filesystem::remove(output / "version.json");
  sourcemeta::one::BuildState entries;
  const TestLeaves schemas{
      {"https://example.com/foo", "/src/foo.json", "foo", MTIME(100)}};
  ADD_LEAF_ENTRIES(entries, output, "foo", true, MTIME(150));
  ADD_GLOBAL_ENTRIES(entries, output, MTIME(150));
  entries.emplace(output / "secondary" / "%" / "listing.bin",
                  {.file_mark = MTIME(150), .dependencies = {}});

  entries.configure(test_rules::RULES.leaves,
                    sourcemeta::one::rules_fingerprint<test_rules::RULES>(),
                    INPUTS, test_rules::RULES.sentinel);
  const auto plan{sourcemeta::one::delta<test_rules::RULES>(
      sourcemeta::one::BuildPhase::Produce, test_rules::MODE_FULL, entries,
      output, schemas, "1.0.0", true, "", "Full", {})};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, test_rules::MODE_FULL, 4, 5);

  EXPECT_ACTION(plan, 0, 0, 1, test_rules::ACTION_VERSION,
                output / "version.json", "1.0.0");

  EXPECT_ACTION(plan, 1, 0, 1, test_rules::ACTION_PRIMARY,
                output / "primary" / "foo" / "%" / "primary.bin",
                "https://example.com/foo",
                std::filesystem::path{"/"} / "src" / "foo.json",
                output / "configuration.json");

  EXPECT_ACTION(plan, 2, 0, 1, test_rules::ACTION_METADATA,
                output / "secondary" / "foo" / "%" / "metadata.bin",
                "https://example.com/foo",
                output / "primary" / "foo" / "%" / "primary.bin");

  EXPECT_ACTION_UNORDERED(plan, 3, 0, 2, test_rules::ACTION_LISTING,
                          output / "secondary" / "%" / "listing.bin", "",
                          output / "secondary" / "foo" / "%" / "metadata.bin");
  EXPECT_ACTION_UNORDERED(plan, 3, 1, 2, test_rules::ACTION_WEB,
                          output / "secondary" / "foo" / "%" / "web.bin",
                          "https://example.com/foo",
                          output / "secondary" / "foo" / "%" / "metadata.bin");
}

TEST(incremental_missing_configuration_anchor_is_repaired) {
  const auto output{delta_path("missing_configuration")};
  WRITE_GLOBAL_OUTPUTS(output);
  std::filesystem::remove(output / "configuration.json");
  sourcemeta::one::BuildState entries;
  const TestLeaves schemas{
      {"https://example.com/foo", "/src/foo.json", "foo", MTIME(100)}};
  ADD_LEAF_ENTRIES(entries, output, "foo", true, MTIME(150));
  ADD_GLOBAL_ENTRIES(entries, output, MTIME(150));
  entries.emplace(output / "secondary" / "%" / "listing.bin",
                  {.file_mark = MTIME(150), .dependencies = {}});

  entries.configure(test_rules::RULES.leaves,
                    sourcemeta::one::rules_fingerprint<test_rules::RULES>(),
                    INPUTS, test_rules::RULES.sentinel);
  const auto plan{sourcemeta::one::delta<test_rules::RULES>(
      sourcemeta::one::BuildPhase::Produce, test_rules::MODE_FULL, entries,
      output, schemas, "1.0.0", true, "", "Full", {})};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, test_rules::MODE_FULL, 4, 5);

  EXPECT_ACTION(plan, 0, 0, 1, test_rules::ACTION_CONFIGURATION,
                output / "configuration.json", "");

  EXPECT_ACTION(plan, 1, 0, 1, test_rules::ACTION_PRIMARY,
                output / "primary" / "foo" / "%" / "primary.bin",
                "https://example.com/foo",
                std::filesystem::path{"/"} / "src" / "foo.json",
                output / "configuration.json");

  EXPECT_ACTION(plan, 2, 0, 1, test_rules::ACTION_METADATA,
                output / "secondary" / "foo" / "%" / "metadata.bin",
                "https://example.com/foo",
                output / "primary" / "foo" / "%" / "primary.bin");

  EXPECT_ACTION_UNORDERED(plan, 3, 0, 2, test_rules::ACTION_LISTING,
                          output / "secondary" / "%" / "listing.bin", "",
                          output / "secondary" / "foo" / "%" / "metadata.bin");
  EXPECT_ACTION_UNORDERED(plan, 3, 1, 2, test_rules::ACTION_WEB,
                          output / "secondary" / "foo" / "%" / "web.bin",
                          "https://example.com/foo",
                          output / "secondary" / "foo" / "%" / "metadata.bin");
}

TEST(incremental_missing_mode_global_is_repaired) {
  const auto output{delta_path("missing_routes")};
  WRITE_GLOBAL_OUTPUTS(output);
  std::filesystem::remove(output / "routes.bin");
  sourcemeta::one::BuildState entries;
  const TestLeaves schemas{
      {"https://example.com/foo", "/src/foo.json", "foo", MTIME(100)}};
  ADD_LEAF_ENTRIES(entries, output, "foo", true, MTIME(150));
  ADD_GLOBAL_ENTRIES(entries, output, MTIME(150));
  entries.emplace(output / "secondary" / "%" / "listing.bin",
                  {.file_mark = MTIME(150), .dependencies = {}});

  entries.configure(test_rules::RULES.leaves,
                    sourcemeta::one::rules_fingerprint<test_rules::RULES>(),
                    INPUTS, test_rules::RULES.sentinel);
  const auto plan{sourcemeta::one::delta<test_rules::RULES>(
      sourcemeta::one::BuildPhase::Produce, test_rules::MODE_FULL, entries,
      output, schemas, "1.0.0", true, "", "Full", {})};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, test_rules::MODE_FULL, 4, 5);

  EXPECT_ACTION(plan, 0, 0, 1, test_rules::ACTION_ROUTES, output / "routes.bin",
                "Full", output / "configuration.json");

  EXPECT_ACTION(plan, 1, 0, 1, test_rules::ACTION_PRIMARY,
                output / "primary" / "foo" / "%" / "primary.bin",
                "https://example.com/foo",
                std::filesystem::path{"/"} / "src" / "foo.json",
                output / "configuration.json");

  EXPECT_ACTION(plan, 2, 0, 1, test_rules::ACTION_METADATA,
                output / "secondary" / "foo" / "%" / "metadata.bin",
                "https://example.com/foo",
                output / "primary" / "foo" / "%" / "primary.bin");

  EXPECT_ACTION_UNORDERED(plan, 3, 0, 2, test_rules::ACTION_LISTING,
                          output / "secondary" / "%" / "listing.bin", "",
                          output / "secondary" / "foo" / "%" / "metadata.bin");
  EXPECT_ACTION_UNORDERED(plan, 3, 1, 2, test_rules::ACTION_WEB,
                          output / "secondary" / "foo" / "%" / "web.bin",
                          "https://example.com/foo",
                          output / "secondary" / "foo" / "%" / "metadata.bin");
}

TEST(incremental_missing_dependent_global_is_repaired) {
  const auto output{delta_path("missing_gate")};
  WRITE_GLOBAL_OUTPUTS(output);
  std::filesystem::remove(output / "gate.bin");
  sourcemeta::one::BuildState entries;
  const TestLeaves schemas{
      {"https://example.com/foo", "/src/foo.json", "foo", MTIME(100)}};
  ADD_LEAF_ENTRIES(entries, output, "foo", true, MTIME(150));
  ADD_GLOBAL_ENTRIES(entries, output, MTIME(150));
  entries.emplace(output / "secondary" / "%" / "listing.bin",
                  {.file_mark = MTIME(150), .dependencies = {}});

  entries.configure(test_rules::RULES.leaves,
                    sourcemeta::one::rules_fingerprint<test_rules::RULES>(),
                    INPUTS, test_rules::RULES.sentinel);
  const auto plan{sourcemeta::one::delta<test_rules::RULES>(
      sourcemeta::one::BuildPhase::Produce, test_rules::MODE_FULL, entries,
      output, schemas, "1.0.0", true, "", "Full", {})};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, test_rules::MODE_FULL, 4, 5);

  EXPECT_ACTION(plan, 0, 0, 1, test_rules::ACTION_GATE, output / "gate.bin", "",
                output / "routes.bin");

  EXPECT_ACTION(plan, 1, 0, 1, test_rules::ACTION_PRIMARY,
                output / "primary" / "foo" / "%" / "primary.bin",
                "https://example.com/foo",
                std::filesystem::path{"/"} / "src" / "foo.json",
                output / "configuration.json");

  EXPECT_ACTION(plan, 2, 0, 1, test_rules::ACTION_METADATA,
                output / "secondary" / "foo" / "%" / "metadata.bin",
                "https://example.com/foo",
                output / "primary" / "foo" / "%" / "primary.bin");

  EXPECT_ACTION_UNORDERED(plan, 3, 0, 2, test_rules::ACTION_LISTING,
                          output / "secondary" / "%" / "listing.bin", "",
                          output / "secondary" / "foo" / "%" / "metadata.bin");
  EXPECT_ACTION_UNORDERED(plan, 3, 1, 2, test_rules::ACTION_WEB,
                          output / "secondary" / "foo" / "%" / "web.bin",
                          "https://example.com/foo",
                          output / "secondary" / "foo" / "%" / "metadata.bin");
}

TEST(incremental_missing_globals_repair_in_dependency_order) {
  const auto output{delta_path("missing_several")};
  WRITE_GLOBAL_OUTPUTS(output);
  std::filesystem::remove(output / "version.json");
  std::filesystem::remove(output / "routes.bin");
  std::filesystem::remove(output / "gate.bin");
  sourcemeta::one::BuildState entries;
  const TestLeaves schemas{
      {"https://example.com/foo", "/src/foo.json", "foo", MTIME(100)}};
  ADD_LEAF_ENTRIES(entries, output, "foo", true, MTIME(150));
  ADD_GLOBAL_ENTRIES(entries, output, MTIME(150));
  entries.emplace(output / "secondary" / "%" / "listing.bin",
                  {.file_mark = MTIME(150), .dependencies = {}});

  entries.configure(test_rules::RULES.leaves,
                    sourcemeta::one::rules_fingerprint<test_rules::RULES>(),
                    INPUTS, test_rules::RULES.sentinel);
  const auto plan{sourcemeta::one::delta<test_rules::RULES>(
      sourcemeta::one::BuildPhase::Produce, test_rules::MODE_FULL, entries,
      output, schemas, "1.0.0", true, "", "Full", {})};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, test_rules::MODE_FULL, 6, 7);

  EXPECT_ACTION(plan, 0, 0, 1, test_rules::ACTION_VERSION,
                output / "version.json", "1.0.0");

  EXPECT_ACTION(plan, 1, 0, 1, test_rules::ACTION_ROUTES, output / "routes.bin",
                "Full", output / "configuration.json");

  EXPECT_ACTION(plan, 2, 0, 1, test_rules::ACTION_GATE, output / "gate.bin", "",
                output / "routes.bin");

  EXPECT_ACTION(plan, 3, 0, 1, test_rules::ACTION_PRIMARY,
                output / "primary" / "foo" / "%" / "primary.bin",
                "https://example.com/foo",
                std::filesystem::path{"/"} / "src" / "foo.json",
                output / "configuration.json");

  EXPECT_ACTION(plan, 4, 0, 1, test_rules::ACTION_METADATA,
                output / "secondary" / "foo" / "%" / "metadata.bin",
                "https://example.com/foo",
                output / "primary" / "foo" / "%" / "primary.bin");

  EXPECT_ACTION_UNORDERED(plan, 5, 0, 2, test_rules::ACTION_LISTING,
                          output / "secondary" / "%" / "listing.bin", "",
                          output / "secondary" / "foo" / "%" / "metadata.bin");
  EXPECT_ACTION_UNORDERED(plan, 5, 1, 2, test_rules::ACTION_WEB,
                          output / "secondary" / "foo" / "%" / "web.bin",
                          "https://example.com/foo",
                          output / "secondary" / "foo" / "%" / "metadata.bin");
}

TEST(incremental_unrecorded_missing_global_is_not_demanded) {
  const auto output{delta_path("unrecorded_gate")};
  WRITE_GLOBAL_OUTPUTS(output);
  std::filesystem::remove(output / "gate.bin");
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
                    INPUTS, test_rules::RULES.sentinel);
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

TEST(incremental_missing_global_repairs_alone_when_nothing_else_changed) {
  const auto output{delta_path("missing_alone")};
  WRITE_GLOBAL_OUTPUTS(output);
  sourcemeta::one::BuildState entries;
  const TestLeaves schemas;
  ADD_GLOBAL_ENTRIES(entries, output, MTIME(150));

  entries.configure(test_rules::RULES.leaves,
                    sourcemeta::one::rules_fingerprint<test_rules::RULES>(),
                    INPUTS, test_rules::RULES.sentinel);
  const auto clean_plan{sourcemeta::one::delta<test_rules::RULES>(
      sourcemeta::one::BuildPhase::Produce, test_rules::MODE_FULL, entries,
      output, schemas, "1.0.0", true, "", "Full", {})};
  EXPECT_EQ(clean_plan.waves.size(), 0);
  EXPECT_EQ(clean_plan.size, 0);

  std::filesystem::remove(output / "version.json");
  const auto plan{sourcemeta::one::delta<test_rules::RULES>(
      sourcemeta::one::BuildPhase::Produce, test_rules::MODE_FULL, entries,
      output, schemas, "1.0.0", true, "", "Full", {})};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, test_rules::MODE_FULL, 1, 1);
  EXPECT_ACTION(plan, 0, 0, 1, test_rules::ACTION_VERSION,
                output / "version.json", "1.0.0");
}

TEST(limits_zero_disables_check) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  const TestLeaves schemas{
      {"https://example.com/a", "/src/a.json", "a", MTIME(100)},
      {"https://example.com/b", "/src/b.json", "b", MTIME(100)},
      {"https://example.com/c", "/src/c.json", "c", MTIME(100)},
      {"https://example.com/d", "/src/d.json", "d", MTIME(100)}};

  entries.configure(test_rules::RULES.leaves,
                    sourcemeta::one::rules_fingerprint<test_rules::RULES>(),
                    INPUTS, test_rules::RULES.sentinel);
  const auto plan{sourcemeta::one::delta<test_rules::RULES>(
      sourcemeta::one::BuildPhase::Produce, test_rules::MODE_FULL, entries,
      output, schemas, "1.0.0", false, "", "Full",
      {.maximum_direct_directory_entries = 0})};
  EXPECT_EQ(plan.size, 17u);
}

TEST(limits_within_threshold_succeeds) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  const TestLeaves schemas{
      {"https://example.com/a", "/src/a.json", "a", MTIME(100)},
      {"https://example.com/b", "/src/b.json", "b", MTIME(100)}};

  entries.configure(test_rules::RULES.leaves,
                    sourcemeta::one::rules_fingerprint<test_rules::RULES>(),
                    INPUTS, test_rules::RULES.sentinel);
  const auto plan{sourcemeta::one::delta<test_rules::RULES>(
      sourcemeta::one::BuildPhase::Produce, test_rules::MODE_FULL, entries,
      output, schemas, "1.0.0", false, "", "Full",
      {.maximum_direct_directory_entries = 5})};
  EXPECT_EQ(plan.size, 11u);
}

TEST(combine_leaf_without_previous_references_rebuilds_its_own_reverse) {
  const auto output{delta_path("combine_new_leaf")};
  std::filesystem::remove_all(output);
  std::filesystem::create_directories(output);
  sourcemeta::one::BuildState entries;
  const TestLeaves schemas{
      {"https://example.com/foo", "/src/foo.json", "foo", MTIME(100)}};
  entries.emplace(output / "primary" / "foo" / "%" / "references.bin",
                  {.file_mark = MTIME(150), .dependencies = {}});

  entries.configure(
      test_rules::COMBINE_RULES.leaves,
      sourcemeta::one::rules_fingerprint<test_rules::COMBINE_RULES>(), INPUTS,
      test_rules::COMBINE_RULES.sentinel);
  const auto plan{sourcemeta::one::delta<test_rules::COMBINE_RULES>(
      sourcemeta::one::BuildPhase::Combine, test_rules::MODE_FULL, entries,
      output, schemas, "1.0.0", true, "", "Full", {})};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, test_rules::MODE_FULL, 1, 1);

  EXPECT_ACTION(plan, 0, 0, 1, test_rules::ACTION_REVERSE,
                output / "primary" / "foo" / "%" / "reverse.bin",
                "https://example.com/foo");

  EXPECT_TOTAL_FILES(plan, entries,
                     output / "primary" / "foo" / "%" / "references.bin",
                     output / "primary" / "foo" / "%" / "reverse.bin");
}

TEST(combine_new_reference_rebuilds_the_reverse_of_what_it_points_at) {
  const auto output{delta_path("combine_added_reference")};
  std::filesystem::remove_all(output);
  std::filesystem::create_directories(output);
  const auto state{output / "state.bin"};
  const TestLeaves schemas{
      {"https://example.com/foo", "/src/foo.json", "foo", MTIME(100)},
      {"https://example.com/bar", "/src/bar.json", "bar", MTIME(100)}};

  sourcemeta::one::BuildState previous;
  previous.emplace(output / "primary" / "foo" / "%" / "references.bin",
                   {.file_mark = MTIME(150), .dependencies = {}});
  previous.emplace(output / "primary" / "bar" / "%" / "references.bin",
                   {.file_mark = MTIME(150), .dependencies = {}});
  previous.configure(
      test_rules::COMBINE_RULES.leaves,
      sourcemeta::one::rules_fingerprint<test_rules::COMBINE_RULES>(), INPUTS,
      test_rules::COMBINE_RULES.sentinel);
  previous.save(state);

  sourcemeta::one::BuildState entries;
  entries.load(state, test_rules::COMBINE_RULES.leaves,
               sourcemeta::one::rules_fingerprint<test_rules::COMBINE_RULES>(),
               INPUTS, test_rules::COMBINE_RULES.sentinel);
  entries.commit(output / "primary" / "bar" / "%" / "references.bin",
                 {output / "primary" / "foo" / "%" / "primary.bin"});

  const auto plan{sourcemeta::one::delta<test_rules::COMBINE_RULES>(
      sourcemeta::one::BuildPhase::Combine, test_rules::MODE_FULL, entries,
      output, schemas, "1.0.0", true, "", "Full", {})};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, test_rules::MODE_FULL, 1, 1);

  EXPECT_ACTION(plan, 0, 0, 1, test_rules::ACTION_REVERSE,
                output / "primary" / "foo" / "%" / "reverse.bin",
                "https://example.com/foo",
                output / "primary" / "bar" / "%" / "references.bin");

  EXPECT_TOTAL_FILES(plan, entries,
                     output / "primary" / "foo" / "%" / "references.bin",
                     output / "primary" / "bar" / "%" / "references.bin",
                     output / "primary" / "foo" / "%" / "reverse.bin");
}

TEST(combine_unchanged_references_rebuild_nothing) {
  const auto output{delta_path("combine_unchanged_reference")};
  std::filesystem::remove_all(output);
  std::filesystem::create_directories(output);
  const auto state{output / "state.bin"};
  const TestLeaves schemas{
      {"https://example.com/foo", "/src/foo.json", "foo", MTIME(100)},
      {"https://example.com/bar", "/src/bar.json", "bar", MTIME(100)}};

  sourcemeta::one::BuildState previous;
  previous.emplace(output / "primary" / "foo" / "%" / "references.bin",
                   {.file_mark = MTIME(150), .dependencies = {}});
  previous.emplace(
      output / "primary" / "bar" / "%" / "references.bin",
      {.file_mark = MTIME(150),
       .dependencies = {output / "primary" / "foo" / "%" / "primary.bin"}});
  previous.configure(
      test_rules::COMBINE_RULES.leaves,
      sourcemeta::one::rules_fingerprint<test_rules::COMBINE_RULES>(), INPUTS,
      test_rules::COMBINE_RULES.sentinel);
  previous.save(state);

  sourcemeta::one::BuildState entries;
  entries.load(state, test_rules::COMBINE_RULES.leaves,
               sourcemeta::one::rules_fingerprint<test_rules::COMBINE_RULES>(),
               INPUTS, test_rules::COMBINE_RULES.sentinel);
  entries.commit(output / "primary" / "bar" / "%" / "references.bin",
                 {output / "primary" / "foo" / "%" / "primary.bin"});

  const auto plan{sourcemeta::one::delta<test_rules::COMBINE_RULES>(
      sourcemeta::one::BuildPhase::Combine, test_rules::MODE_FULL, entries,
      output, schemas, "1.0.0", true, "", "Full", {})};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, test_rules::MODE_FULL, 0, 0);

  EXPECT_TOTAL_FILES(plan, entries,
                     output / "primary" / "foo" / "%" / "references.bin",
                     output / "primary" / "bar" / "%" / "references.bin");
}

TEST(combine_dropped_reference_rebuilds_the_reverse_of_what_it_left) {
  const auto output{delta_path("combine_dropped_reference")};
  std::filesystem::remove_all(output);
  std::filesystem::create_directories(output);
  const auto state{output / "state.bin"};
  const TestLeaves schemas{
      {"https://example.com/foo", "/src/foo.json", "foo", MTIME(100)},
      {"https://example.com/bar", "/src/bar.json", "bar", MTIME(100)}};

  sourcemeta::one::BuildState previous;
  previous.emplace(output / "primary" / "foo" / "%" / "references.bin",
                   {.file_mark = MTIME(150), .dependencies = {}});
  previous.emplace(
      output / "primary" / "bar" / "%" / "references.bin",
      {.file_mark = MTIME(150),
       .dependencies = {output / "primary" / "foo" / "%" / "primary.bin"}});
  previous.configure(
      test_rules::COMBINE_RULES.leaves,
      sourcemeta::one::rules_fingerprint<test_rules::COMBINE_RULES>(), INPUTS,
      test_rules::COMBINE_RULES.sentinel);
  previous.save(state);

  sourcemeta::one::BuildState entries;
  entries.load(state, test_rules::COMBINE_RULES.leaves,
               sourcemeta::one::rules_fingerprint<test_rules::COMBINE_RULES>(),
               INPUTS, test_rules::COMBINE_RULES.sentinel);
  entries.commit(output / "primary" / "bar" / "%" / "references.bin", {});

  const auto plan{sourcemeta::one::delta<test_rules::COMBINE_RULES>(
      sourcemeta::one::BuildPhase::Combine, test_rules::MODE_FULL, entries,
      output, schemas, "1.0.0", true, "", "Full", {})};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, test_rules::MODE_FULL, 1, 1);

  EXPECT_ACTION(plan, 0, 0, 1, test_rules::ACTION_REVERSE,
                output / "primary" / "foo" / "%" / "reverse.bin",
                "https://example.com/foo");

  EXPECT_TOTAL_FILES(plan, entries,
                     output / "primary" / "foo" / "%" / "references.bin",
                     output / "primary" / "bar" / "%" / "references.bin",
                     output / "primary" / "foo" / "%" / "reverse.bin");
}

TEST(combine_destination_follows_the_tree_the_rule_names) {
  const auto output{delta_path("combine_secondary_destination")};
  std::filesystem::remove_all(output);
  std::filesystem::create_directories(output);
  sourcemeta::one::BuildState entries;
  const TestLeaves schemas{
      {"https://example.com/foo", "/src/foo.json", "foo", MTIME(100)}};
  entries.emplace(output / "primary" / "foo" / "%" / "references.bin",
                  {.file_mark = MTIME(150), .dependencies = {}});

  entries.configure(
      test_rules::COMBINE_RULES_SECONDARY.leaves,
      sourcemeta::one::rules_fingerprint<test_rules::COMBINE_RULES_SECONDARY>(),
      INPUTS, test_rules::COMBINE_RULES_SECONDARY.sentinel);
  const auto plan{sourcemeta::one::delta<test_rules::COMBINE_RULES_SECONDARY>(
      sourcemeta::one::BuildPhase::Combine, test_rules::MODE_FULL, entries,
      output, schemas, "1.0.0", true, "", "Full", {})};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, test_rules::MODE_FULL, 1, 1);

  EXPECT_ACTION(plan, 0, 0, 1, test_rules::ACTION_REVERSE,
                output / "secondary" / "foo" / "%" / "reverse.bin",
                "https://example.com/foo");

  EXPECT_TOTAL_FILES(plan, entries,
                     output / "primary" / "foo" / "%" / "references.bin",
                     output / "secondary" / "foo" / "%" / "reverse.bin");
}

TEST(limits_exceeded_throws) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  const TestLeaves schemas{
      {"https://example.com/a", "/src/a.json", "a", MTIME(100)},
      {"https://example.com/b", "/src/b.json", "b", MTIME(100)},
      {"https://example.com/c", "/src/c.json", "c", MTIME(100)}};

  entries.configure(test_rules::RULES.leaves,
                    sourcemeta::one::rules_fingerprint<test_rules::RULES>(),
                    INPUTS, test_rules::RULES.sentinel);
  try {
    sourcemeta::one::delta<test_rules::RULES>(
        sourcemeta::one::BuildPhase::Produce, test_rules::MODE_FULL, entries,
        output, schemas, "1.0.0", false, "", "Full",
        {.maximum_direct_directory_entries = 2});
    FAIL();
  } catch (const sourcemeta::one::BuildTooManyDirectoryEntriesError &error) {
    EXPECT_STREQ(error.what(), "Too many entries in a single directory");
  }
}
