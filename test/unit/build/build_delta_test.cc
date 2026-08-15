#include <sourcemeta/core/json.h>
#include <sourcemeta/core/test.h>
#include <sourcemeta/one/build.h>

#include "build_test_utils.h"
#include "test_rules.h"

#include <array>      // std::array
#include <filesystem> // std::filesystem::path
#include <string>     // std::string

// A build of one unchanging configuration and version
static constexpr sourcemeta::one::BuildState::InputsFingerprint INPUTS{
    0x0123456789abcdefULL};

// The view every expectation in this file was written against, named because a
// namespaced tree always carries a segment
static constexpr std::array<std::string_view, 1> VIEWS{{"public"}};

// A registry declaring one policy is served two ways, so its namespaced tree is
// written twice and everything outside it once
static constexpr std::array<std::string_view, 2> TWO_VIEWS{
    {"public", "private"}};

// Every view holds every leaf, which is what these expectations were written
// against unless one of them says otherwise
static auto everything() -> sourcemeta::one::ViewFilter {
  return [](const std::size_t, const std::string_view) -> bool { return true; };
}

static auto delta_path(const std::string &name) -> std::filesystem::path {
  return std::filesystem::path{BINARY_DIRECTORY} / "delta" / name;
}

TEST(full_empty_registry) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  const TestLeaves schemas;

  entries.configure(test_rules::RULES.leaves, test_rules::RULES.directories,
                    sourcemeta::one::rules_fingerprint<test_rules::RULES>(),
                    INPUTS, test_rules::RULES.sentinel);
  const auto plan{sourcemeta::one::delta<test_rules::RULES>(
      sourcemeta::one::BuildPhase::Produce, test_rules::MODE_FULL, entries,
      output, schemas, "1.0.0", false, "", "Full", {}, VIEWS, everything())};

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
                output / "secondary" / "public" / "%" / "listing.bin", "");

  EXPECT_TOTAL_FILES(plan, entries, output / "configuration.json",
                     output / "version.json", output / "routes.bin",
                     output / "gate.bin",
                     output / "secondary" / "public" / "%" / "listing.bin");
}

TEST(full_single_leaf) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  const TestLeaves schemas{
      {"https://example.com/foo", "/src/foo.json", "foo", MTIME(100)}};

  entries.configure(test_rules::RULES.leaves, test_rules::RULES.directories,
                    sourcemeta::one::rules_fingerprint<test_rules::RULES>(),
                    INPUTS, test_rules::RULES.sentinel);
  const auto plan{sourcemeta::one::delta<test_rules::RULES>(
      sourcemeta::one::BuildPhase::Produce, test_rules::MODE_FULL, entries,
      output, schemas, "1.0.0", false, "", "Full", {}, VIEWS, everything())};

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
                output / "secondary" / "public" / "foo" / "%" / "metadata.bin",
                "https://example.com/foo",
                output / "primary" / "foo" / "%" / "primary.bin");

  EXPECT_ACTION_UNORDERED(
      plan, 5, 0, 2, test_rules::ACTION_LISTING,
      output / "secondary" / "public" / "%" / "listing.bin", "",
      output / "secondary" / "public" / "foo" / "%" / "metadata.bin");
  EXPECT_ACTION_UNORDERED(
      plan, 5, 1, 2, test_rules::ACTION_WEB,
      output / "secondary" / "public" / "foo" / "%" / "web.bin",
      "https://example.com/foo",
      output / "secondary" / "public" / "foo" / "%" / "metadata.bin");

  EXPECT_TOTAL_FILES(
      plan, entries, output / "configuration.json", output / "version.json",
      output / "routes.bin", output / "gate.bin",
      output / "primary" / "foo" / "%" / "primary.bin",
      output / "secondary" / "public" / "foo" / "%" / "metadata.bin",
      output / "secondary" / "public" / "foo" / "%" / "web.bin",
      output / "secondary" / "public" / "%" / "listing.bin");
}

TEST(a_leaf_no_view_holds_is_written_outside_the_namespaced_tree_alone) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  const TestLeaves schemas{
      {"https://example.com/foo", "/src/foo.json", "foo", MTIME(100)}};

  entries.configure(test_rules::RULES.leaves, test_rules::RULES.directories,
                    sourcemeta::one::rules_fingerprint<test_rules::RULES>(),
                    INPUTS, test_rules::RULES.sentinel);
  // The second view cannot see this leaf, so it holds nothing describing it
  const auto plan{sourcemeta::one::delta<test_rules::RULES>(
      sourcemeta::one::BuildPhase::Produce, test_rules::MODE_FULL, entries,
      output, schemas, "1.0.0", false, "", "Full", {}, TWO_VIEWS,
      [](const std::size_t view, const std::string_view) -> bool {
        return view == 0;
      })};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, test_rules::MODE_FULL, 6, 9);

  EXPECT_ACTION(plan, 0, 0, 2, test_rules::ACTION_CONFIGURATION,
                output / "configuration.json", "");
  EXPECT_ACTION(plan, 0, 1, 2, test_rules::ACTION_VERSION,
                output / "version.json", "1.0.0");

  EXPECT_ACTION(plan, 1, 0, 1, test_rules::ACTION_ROUTES, output / "routes.bin",
                "Full", output / "configuration.json");

  EXPECT_ACTION(plan, 2, 0, 1, test_rules::ACTION_GATE, output / "gate.bin", "",
                output / "routes.bin");

  // The listing of a view holding nothing here waits on nothing, so it is ready
  // as soon as anything is
  EXPECT_ACTION_UNORDERED(plan, 3, 0, 2, test_rules::ACTION_PRIMARY,
                          output / "primary" / "foo" / "%" / "primary.bin",
                          "https://example.com/foo",
                          std::filesystem::path{"/"} / "src" / "foo.json",
                          output / "configuration.json");
  EXPECT_ACTION_UNORDERED(
      plan, 3, 1, 2, test_rules::ACTION_LISTING,
      output / "secondary" / "private" / "%" / "listing.bin", "");

  EXPECT_ACTION(plan, 4, 0, 1, test_rules::ACTION_METADATA,
                output / "secondary" / "public" / "foo" / "%" / "metadata.bin",
                "https://example.com/foo",
                output / "primary" / "foo" / "%" / "primary.bin");

  EXPECT_ACTION_UNORDERED(
      plan, 5, 0, 2, test_rules::ACTION_LISTING,
      output / "secondary" / "public" / "%" / "listing.bin", "",
      output / "secondary" / "public" / "foo" / "%" / "metadata.bin");
  EXPECT_ACTION_UNORDERED(
      plan, 5, 1, 2, test_rules::ACTION_WEB,
      output / "secondary" / "public" / "foo" / "%" / "web.bin",
      "https://example.com/foo",
      output / "secondary" / "public" / "foo" / "%" / "metadata.bin");

  EXPECT_TOTAL_FILES(
      plan, entries, output / "configuration.json", output / "version.json",
      output / "routes.bin", output / "gate.bin",
      output / "primary" / "foo" / "%" / "primary.bin",
      output / "secondary" / "public" / "foo" / "%" / "metadata.bin",
      output / "secondary" / "public" / "foo" / "%" / "web.bin",
      output / "secondary" / "public" / "%" / "listing.bin",
      output / "secondary" / "private" / "%" / "listing.bin");
}

TEST(full_single_leaf_across_two_views) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  const TestLeaves schemas{
      {"https://example.com/foo", "/src/foo.json", "foo", MTIME(100)}};

  entries.configure(test_rules::RULES.leaves, test_rules::RULES.directories,
                    sourcemeta::one::rules_fingerprint<test_rules::RULES>(),
                    INPUTS, test_rules::RULES.sentinel);
  const auto plan{sourcemeta::one::delta<test_rules::RULES>(
      sourcemeta::one::BuildPhase::Produce, test_rules::MODE_FULL, entries,
      output, schemas, "1.0.0", false, "", "Full", {}, TWO_VIEWS,
      everything())};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, test_rules::MODE_FULL, 6, 11);

  EXPECT_ACTION(plan, 0, 0, 2, test_rules::ACTION_CONFIGURATION,
                output / "configuration.json", "");
  EXPECT_ACTION(plan, 0, 1, 2, test_rules::ACTION_VERSION,
                output / "version.json", "1.0.0");

  EXPECT_ACTION(plan, 1, 0, 1, test_rules::ACTION_ROUTES, output / "routes.bin",
                "Full", output / "configuration.json");

  EXPECT_ACTION(plan, 2, 0, 1, test_rules::ACTION_GATE, output / "gate.bin", "",
                output / "routes.bin");

  // The unit tree is written once whatever the view count, since nothing in it
  // depends on who is asking
  EXPECT_ACTION(plan, 3, 0, 1, test_rules::ACTION_PRIMARY,
                output / "primary" / "foo" / "%" / "primary.bin",
                "https://example.com/foo",
                std::filesystem::path{"/"} / "src" / "foo.json",
                output / "configuration.json");

  EXPECT_ACTION_UNORDERED(plan, 4, 0, 2, test_rules::ACTION_METADATA,
                          output / "secondary" / "private" / "foo" / "%" /
                              "metadata.bin",
                          "https://example.com/foo",
                          output / "primary" / "foo" / "%" / "primary.bin");
  EXPECT_ACTION_UNORDERED(plan, 4, 1, 2, test_rules::ACTION_METADATA,
                          output / "secondary" / "public" / "foo" / "%" /
                              "metadata.bin",
                          "https://example.com/foo",
                          output / "primary" / "foo" / "%" / "primary.bin");

  // Each view's listing reads its own view's children rather than another's
  EXPECT_ACTION_UNORDERED(
      plan, 5, 0, 4, test_rules::ACTION_LISTING,
      output / "secondary" / "private" / "%" / "listing.bin", "",
      output / "secondary" / "private" / "foo" / "%" / "metadata.bin");
  EXPECT_ACTION_UNORDERED(
      plan, 5, 1, 4, test_rules::ACTION_WEB,
      output / "secondary" / "private" / "foo" / "%" / "web.bin",
      "https://example.com/foo",
      output / "secondary" / "private" / "foo" / "%" / "metadata.bin");
  EXPECT_ACTION_UNORDERED(
      plan, 5, 2, 4, test_rules::ACTION_LISTING,
      output / "secondary" / "public" / "%" / "listing.bin", "",
      output / "secondary" / "public" / "foo" / "%" / "metadata.bin");
  EXPECT_ACTION_UNORDERED(
      plan, 5, 3, 4, test_rules::ACTION_WEB,
      output / "secondary" / "public" / "foo" / "%" / "web.bin",
      "https://example.com/foo",
      output / "secondary" / "public" / "foo" / "%" / "metadata.bin");

  EXPECT_TOTAL_FILES(
      plan, entries, output / "configuration.json", output / "version.json",
      output / "routes.bin", output / "gate.bin",
      output / "primary" / "foo" / "%" / "primary.bin",
      output / "secondary" / "public" / "foo" / "%" / "metadata.bin",
      output / "secondary" / "public" / "foo" / "%" / "web.bin",
      output / "secondary" / "public" / "%" / "listing.bin",
      output / "secondary" / "private" / "foo" / "%" / "metadata.bin",
      output / "secondary" / "private" / "foo" / "%" / "web.bin",
      output / "secondary" / "private" / "%" / "listing.bin");
}

TEST(full_single_leaf_headless_skips_full_only) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  const TestLeaves schemas{
      {"https://example.com/foo", "/src/foo.json", "foo", MTIME(100)}};

  entries.configure(test_rules::RULES.leaves, test_rules::RULES.directories,
                    sourcemeta::one::rules_fingerprint<test_rules::RULES>(),
                    INPUTS, test_rules::RULES.sentinel);
  const auto plan{sourcemeta::one::delta<test_rules::RULES>(
      sourcemeta::one::BuildPhase::Produce, test_rules::MODE_HEADLESS, entries,
      output, schemas, "1.0.0", false, "", "Headless", {}, VIEWS,
      everything())};

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
                output / "secondary" / "public" / "foo" / "%" / "metadata.bin",
                "https://example.com/foo",
                output / "primary" / "foo" / "%" / "primary.bin");

  EXPECT_ACTION(plan, 5, 0, 1, test_rules::ACTION_LISTING,
                output / "secondary" / "public" / "%" / "listing.bin", "",
                output / "secondary" / "public" / "foo" / "%" / "metadata.bin");

  EXPECT_TOTAL_FILES(
      plan, entries, output / "configuration.json", output / "version.json",
      output / "routes.bin", output / "gate.bin",
      output / "primary" / "foo" / "%" / "primary.bin",
      output / "secondary" / "public" / "foo" / "%" / "metadata.bin",
      output / "secondary" / "public" / "%" / "listing.bin");
}

TEST(full_nested_leaf_path) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  const TestLeaves schemas{
      {"https://example.com/a/b/c", "/src/abc.json", "a/b/c", MTIME(100)}};

  entries.configure(test_rules::RULES.leaves, test_rules::RULES.directories,
                    sourcemeta::one::rules_fingerprint<test_rules::RULES>(),
                    INPUTS, test_rules::RULES.sentinel);
  const auto plan{sourcemeta::one::delta<test_rules::RULES>(
      sourcemeta::one::BuildPhase::Produce, test_rules::MODE_FULL, entries,
      output, schemas, "1.0.0", false, "", "Full", {}, VIEWS, everything())};

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
                output / "secondary" / "public" / "a" / "b" / "c" / "%" /
                    "metadata.bin",
                "https://example.com/a/b/c",
                output / "primary" / "a" / "b" / "c" / "%" / "primary.bin");

  EXPECT_ACTION_UNORDERED(
      plan, 5, 0, 2, test_rules::ACTION_LISTING,
      output / "secondary" / "public" / "a" / "b" / "%" / "listing.bin", "",
      output / "secondary" / "public" / "a" / "b" / "c" / "%" / "metadata.bin");
  EXPECT_ACTION_UNORDERED(
      plan, 5, 1, 2, test_rules::ACTION_WEB,
      output / "secondary" / "public" / "a" / "b" / "c" / "%" / "web.bin",
      "https://example.com/a/b/c",
      output / "secondary" / "public" / "a" / "b" / "c" / "%" / "metadata.bin");

  EXPECT_ACTION(plan, 6, 0, 1, test_rules::ACTION_LISTING,
                output / "secondary" / "public" / "a" / "%" / "listing.bin", "",
                output / "secondary" / "public" / "a" / "b" / "%" /
                    "listing.bin");

  EXPECT_ACTION(plan, 7, 0, 1, test_rules::ACTION_LISTING,
                output / "secondary" / "public" / "%" / "listing.bin", "",
                output / "secondary" / "public" / "a" / "%" / "listing.bin");

  EXPECT_TOTAL_FILES(
      plan, entries, output / "configuration.json", output / "version.json",
      output / "routes.bin", output / "gate.bin",
      output / "primary" / "a" / "b" / "c" / "%" / "primary.bin",
      output / "secondary" / "public" / "a" / "b" / "c" / "%" / "metadata.bin",
      output / "secondary" / "public" / "a" / "b" / "c" / "%" / "web.bin",
      output / "secondary" / "public" / "a" / "b" / "%" / "listing.bin",
      output / "secondary" / "public" / "a" / "%" / "listing.bin",
      output / "secondary" / "public" / "%" / "listing.bin");
}

TEST(full_with_comment_emits_comment_global) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  const TestLeaves schemas;

  entries.configure(test_rules::RULES.leaves, test_rules::RULES.directories,
                    sourcemeta::one::rules_fingerprint<test_rules::RULES>(),
                    INPUTS, test_rules::RULES.sentinel);
  const auto plan{sourcemeta::one::delta<test_rules::RULES>(
      sourcemeta::one::BuildPhase::Produce, test_rules::MODE_FULL, entries,
      output, schemas, "1.0.0", false, "hello world", "Full", {}, VIEWS,
      everything())};

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
                output / "secondary" / "public" / "%" / "listing.bin", "");

  EXPECT_TOTAL_FILES(plan, entries, output / "configuration.json",
                     output / "version.json", output / "comment.json",
                     output / "routes.bin", output / "gate.bin",
                     output / "secondary" / "public" / "%" / "listing.bin");
}

TEST(full_without_comment_removes_stale_comment) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  const TestLeaves schemas;

  entries.emplace(output / "comment.json",
                  {.file_mark = MTIME(50), .dependencies = {}});
  entries.configure(test_rules::RULES.leaves, test_rules::RULES.directories,
                    sourcemeta::one::rules_fingerprint<test_rules::RULES>(),
                    INPUTS, test_rules::RULES.sentinel);
  const auto plan{sourcemeta::one::delta<test_rules::RULES>(
      sourcemeta::one::BuildPhase::Produce, test_rules::MODE_FULL, entries,
      output, schemas, "1.0.0", false, "", "Full", {}, VIEWS, everything())};

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
                output / "secondary" / "public" / "%" / "listing.bin", "");

  EXPECT_ACTION(plan, 4, 0, 1, test_rules::ACTION_REMOVE,
                output / "comment.json", "");

  EXPECT_TOTAL_FILES(plan, entries, output / "configuration.json",
                     output / "version.json", output / "routes.bin",
                     output / "gate.bin",
                     output / "secondary" / "public" / "%" / "listing.bin");
}

TEST(full_multiple_leaves_emits_per_leaf_actions) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  const TestLeaves schemas{
      {"https://example.com/a", "/src/a.json", "a", MTIME(100)},
      {"https://example.com/b", "/src/b.json", "b", MTIME(100)}};

  entries.configure(test_rules::RULES.leaves, test_rules::RULES.directories,
                    sourcemeta::one::rules_fingerprint<test_rules::RULES>(),
                    INPUTS, test_rules::RULES.sentinel);
  const auto plan{sourcemeta::one::delta<test_rules::RULES>(
      sourcemeta::one::BuildPhase::Produce, test_rules::MODE_FULL, entries,
      output, schemas, "1.0.0", false, "", "Full", {}, VIEWS, everything())};

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

  EXPECT_ACTION_UNORDERED(
      plan, 4, 0, 2, test_rules::ACTION_METADATA,
      output / "secondary" / "public" / "a" / "%" / "metadata.bin",
      "https://example.com/a", output / "primary" / "a" / "%" / "primary.bin");
  EXPECT_ACTION_UNORDERED(
      plan, 4, 1, 2, test_rules::ACTION_METADATA,
      output / "secondary" / "public" / "b" / "%" / "metadata.bin",
      "https://example.com/b", output / "primary" / "b" / "%" / "primary.bin");

  EXPECT_ACTION_UNORDERED(
      plan, 5, 0, 3, test_rules::ACTION_LISTING,
      output / "secondary" / "public" / "%" / "listing.bin", "",
      output / "secondary" / "public" / "a" / "%" / "metadata.bin",
      output / "secondary" / "public" / "b" / "%" / "metadata.bin");
  EXPECT_ACTION_UNORDERED(
      plan, 5, 1, 3, test_rules::ACTION_WEB,
      output / "secondary" / "public" / "a" / "%" / "web.bin",
      "https://example.com/a",
      output / "secondary" / "public" / "a" / "%" / "metadata.bin");
  EXPECT_ACTION_UNORDERED(
      plan, 5, 2, 3, test_rules::ACTION_WEB,
      output / "secondary" / "public" / "b" / "%" / "web.bin",
      "https://example.com/b",
      output / "secondary" / "public" / "b" / "%" / "metadata.bin");

  EXPECT_TOTAL_FILES(
      plan, entries, output / "configuration.json", output / "version.json",
      output / "routes.bin", output / "gate.bin",
      output / "primary" / "a" / "%" / "primary.bin",
      output / "primary" / "b" / "%" / "primary.bin",
      output / "secondary" / "public" / "a" / "%" / "metadata.bin",
      output / "secondary" / "public" / "b" / "%" / "metadata.bin",
      output / "secondary" / "public" / "a" / "%" / "web.bin",
      output / "secondary" / "public" / "b" / "%" / "web.bin",
      output / "secondary" / "public" / "%" / "listing.bin");
}

TEST(incremental_cached_globals_are_omitted) {
  const auto output{delta_path("cached_globals")};
  WRITE_GLOBAL_OUTPUTS(output);
  sourcemeta::one::BuildState entries;
  const TestLeaves schemas{
      {"https://example.com/foo", "/src/foo.json", "foo", MTIME(100)}};
  ADD_LEAF_ENTRIES(entries, output, "foo", true, MTIME(50));
  ADD_GLOBAL_ENTRIES(entries, output, MTIME(150));
  entries.emplace(output / "secondary" / "public" / "%" / "listing.bin",
                  {.file_mark = MTIME(150), .dependencies = {}});

  entries.configure(test_rules::RULES.leaves, test_rules::RULES.directories,
                    sourcemeta::one::rules_fingerprint<test_rules::RULES>(),
                    INPUTS, test_rules::RULES.sentinel);
  const auto plan{sourcemeta::one::delta<test_rules::RULES>(
      sourcemeta::one::BuildPhase::Produce, test_rules::MODE_FULL, entries,
      output, schemas, "1.0.0", true, "", "Full", {}, VIEWS, everything())};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, test_rules::MODE_FULL, 3, 4);

  EXPECT_ACTION(plan, 0, 0, 1, test_rules::ACTION_PRIMARY,
                output / "primary" / "foo" / "%" / "primary.bin",
                "https://example.com/foo",
                std::filesystem::path{"/"} / "src" / "foo.json",
                output / "configuration.json");

  EXPECT_ACTION(plan, 1, 0, 1, test_rules::ACTION_METADATA,
                output / "secondary" / "public" / "foo" / "%" / "metadata.bin",
                "https://example.com/foo",
                output / "primary" / "foo" / "%" / "primary.bin");

  EXPECT_ACTION_UNORDERED(
      plan, 2, 0, 2, test_rules::ACTION_LISTING,
      output / "secondary" / "public" / "%" / "listing.bin", "",
      output / "secondary" / "public" / "foo" / "%" / "metadata.bin");
  EXPECT_ACTION_UNORDERED(
      plan, 2, 1, 2, test_rules::ACTION_WEB,
      output / "secondary" / "public" / "foo" / "%" / "web.bin",
      "https://example.com/foo",
      output / "secondary" / "public" / "foo" / "%" / "metadata.bin");
}

TEST(incremental_only_the_new_leaf_is_built_beside_an_unchanged_one) {
  const auto output{delta_path("new_leaf_alongside")};
  WRITE_GLOBAL_OUTPUTS(output);
  sourcemeta::one::BuildState entries;
  const TestLeaves schemas{
      {"https://example.com/foo", "/src/foo.json", "foo", MTIME(100)},
      {"https://example.com/bar", "/src/bar.json", "bar", MTIME(200)}};
  ADD_LEAF_ENTRIES(entries, output, "foo", true, MTIME(150));
  ADD_GLOBAL_ENTRIES(entries, output, MTIME(150));
  entries.emplace(output / "secondary" / "public" / "%" / "listing.bin",
                  {.file_mark = MTIME(150), .dependencies = {}});

  entries.configure(test_rules::RULES.leaves, test_rules::RULES.directories,
                    sourcemeta::one::rules_fingerprint<test_rules::RULES>(),
                    INPUTS, test_rules::RULES.sentinel);
  const auto plan{sourcemeta::one::delta<test_rules::RULES>(
      sourcemeta::one::BuildPhase::Produce, test_rules::MODE_FULL, entries,
      output, schemas, "1.0.0", true, "", "Full", {}, VIEWS, everything())};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, test_rules::MODE_FULL, 3, 4);

  // Only the leaf that is new is built. The one already recorded is left alone,
  // which is the whole of what an incremental build buys
  EXPECT_ACTION(plan, 0, 0, 1, test_rules::ACTION_PRIMARY,
                output / "primary" / "bar" / "%" / "primary.bin",
                "https://example.com/bar",
                std::filesystem::path{"/"} / "src" / "bar.json",
                output / "configuration.json");

  EXPECT_ACTION(plan, 1, 0, 1, test_rules::ACTION_METADATA,
                output / "secondary" / "public" / "bar" / "%" / "metadata.bin",
                "https://example.com/bar",
                output / "primary" / "bar" / "%" / "primary.bin");

  // The listing still names both, since what a directory holds is not a
  // function of which of its leaves were rebuilt
  EXPECT_ACTION_UNORDERED(
      plan, 2, 0, 2, test_rules::ACTION_LISTING,
      output / "secondary" / "public" / "%" / "listing.bin", "",
      output / "secondary" / "public" / "bar" / "%" / "metadata.bin",
      output / "secondary" / "public" / "foo" / "%" / "metadata.bin");
  EXPECT_ACTION_UNORDERED(
      plan, 2, 1, 2, test_rules::ACTION_WEB,
      output / "secondary" / "public" / "bar" / "%" / "web.bin",
      "https://example.com/bar",
      output / "secondary" / "public" / "bar" / "%" / "metadata.bin");

  EXPECT_TOTAL_FILES(
      plan, entries, output / "configuration.json", output / "version.json",
      output / "routes.bin", output / "gate.bin",
      output / "primary" / "foo" / "%" / "primary.bin",
      output / "secondary" / "public" / "foo" / "%" / "metadata.bin",
      output / "secondary" / "public" / "foo" / "%" / "web.bin",
      output / "primary" / "bar" / "%" / "primary.bin",
      output / "secondary" / "public" / "bar" / "%" / "metadata.bin",
      output / "secondary" / "public" / "bar" / "%" / "web.bin",
      output / "secondary" / "public" / "%" / "listing.bin");
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
  entries.emplace(output / "secondary" / "public" / "%" / "listing.bin",
                  {.file_mark = MTIME(150), .dependencies = {}});

  entries.configure(test_rules::RULES.leaves, test_rules::RULES.directories,
                    sourcemeta::one::rules_fingerprint<test_rules::RULES>(),
                    INPUTS, test_rules::RULES.sentinel);
  const auto plan{sourcemeta::one::delta<test_rules::RULES>(
      sourcemeta::one::BuildPhase::Produce, test_rules::MODE_FULL, entries,
      output, schemas, "1.0.0", true, "", "Full", {}, VIEWS, everything())};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, test_rules::MODE_FULL, 1, 1);

  // The repair is the whole plan, since nothing else fell behind
  EXPECT_ACTION(plan, 0, 0, 1, test_rules::ACTION_VERSION,
                output / "version.json", "1.0.0");
  EXPECT_TOTAL_FILES(
      plan, entries, output / "configuration.json", output / "version.json",
      output / "routes.bin", output / "gate.bin",
      output / "primary" / "foo" / "%" / "primary.bin",
      output / "secondary" / "public" / "foo" / "%" / "metadata.bin",
      output / "secondary" / "public" / "foo" / "%" / "web.bin",
      output / "secondary" / "public" / "%" / "listing.bin");
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
  entries.emplace(output / "secondary" / "public" / "%" / "listing.bin",
                  {.file_mark = MTIME(150), .dependencies = {}});

  entries.configure(test_rules::RULES.leaves, test_rules::RULES.directories,
                    sourcemeta::one::rules_fingerprint<test_rules::RULES>(),
                    INPUTS, test_rules::RULES.sentinel);
  const auto plan{sourcemeta::one::delta<test_rules::RULES>(
      sourcemeta::one::BuildPhase::Produce, test_rules::MODE_FULL, entries,
      output, schemas, "1.0.0", true, "", "Full", {}, VIEWS, everything())};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, test_rules::MODE_FULL, 1, 1);

  // The repair is the whole plan, since nothing else fell behind
  EXPECT_ACTION(plan, 0, 0, 1, test_rules::ACTION_CONFIGURATION,
                output / "configuration.json", "");
  EXPECT_TOTAL_FILES(
      plan, entries, output / "configuration.json", output / "version.json",
      output / "routes.bin", output / "gate.bin",
      output / "primary" / "foo" / "%" / "primary.bin",
      output / "secondary" / "public" / "foo" / "%" / "metadata.bin",
      output / "secondary" / "public" / "foo" / "%" / "web.bin",
      output / "secondary" / "public" / "%" / "listing.bin");
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
  entries.emplace(output / "secondary" / "public" / "%" / "listing.bin",
                  {.file_mark = MTIME(150), .dependencies = {}});

  entries.configure(test_rules::RULES.leaves, test_rules::RULES.directories,
                    sourcemeta::one::rules_fingerprint<test_rules::RULES>(),
                    INPUTS, test_rules::RULES.sentinel);
  const auto plan{sourcemeta::one::delta<test_rules::RULES>(
      sourcemeta::one::BuildPhase::Produce, test_rules::MODE_FULL, entries,
      output, schemas, "1.0.0", true, "", "Full", {}, VIEWS, everything())};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, test_rules::MODE_FULL, 1, 1);

  // The repair is the whole plan, since nothing else fell behind
  EXPECT_ACTION(plan, 0, 0, 1, test_rules::ACTION_ROUTES, output / "routes.bin",
                "Full", output / "configuration.json");
  EXPECT_TOTAL_FILES(
      plan, entries, output / "configuration.json", output / "version.json",
      output / "routes.bin", output / "gate.bin",
      output / "primary" / "foo" / "%" / "primary.bin",
      output / "secondary" / "public" / "foo" / "%" / "metadata.bin",
      output / "secondary" / "public" / "foo" / "%" / "web.bin",
      output / "secondary" / "public" / "%" / "listing.bin");
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
  entries.emplace(output / "secondary" / "public" / "%" / "listing.bin",
                  {.file_mark = MTIME(150), .dependencies = {}});

  entries.configure(test_rules::RULES.leaves, test_rules::RULES.directories,
                    sourcemeta::one::rules_fingerprint<test_rules::RULES>(),
                    INPUTS, test_rules::RULES.sentinel);
  const auto plan{sourcemeta::one::delta<test_rules::RULES>(
      sourcemeta::one::BuildPhase::Produce, test_rules::MODE_FULL, entries,
      output, schemas, "1.0.0", true, "", "Full", {}, VIEWS, everything())};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, test_rules::MODE_FULL, 1, 1);

  // The repair is the whole plan, since nothing else fell behind
  EXPECT_ACTION(plan, 0, 0, 1, test_rules::ACTION_GATE, output / "gate.bin", "",
                output / "routes.bin");
  EXPECT_TOTAL_FILES(
      plan, entries, output / "configuration.json", output / "version.json",
      output / "routes.bin", output / "gate.bin",
      output / "primary" / "foo" / "%" / "primary.bin",
      output / "secondary" / "public" / "foo" / "%" / "metadata.bin",
      output / "secondary" / "public" / "foo" / "%" / "web.bin",
      output / "secondary" / "public" / "%" / "listing.bin");
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
  entries.emplace(output / "secondary" / "public" / "%" / "listing.bin",
                  {.file_mark = MTIME(150), .dependencies = {}});

  entries.configure(test_rules::RULES.leaves, test_rules::RULES.directories,
                    sourcemeta::one::rules_fingerprint<test_rules::RULES>(),
                    INPUTS, test_rules::RULES.sentinel);
  const auto plan{sourcemeta::one::delta<test_rules::RULES>(
      sourcemeta::one::BuildPhase::Produce, test_rules::MODE_FULL, entries,
      output, schemas, "1.0.0", true, "", "Full", {}, VIEWS, everything())};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, test_rules::MODE_FULL, 3, 3);

  // Each repair waits for the one it reads, so the order is the dependency
  // order rather than the order they were found missing in
  EXPECT_ACTION(plan, 0, 0, 1, test_rules::ACTION_VERSION,
                output / "version.json", "1.0.0");

  EXPECT_ACTION(plan, 1, 0, 1, test_rules::ACTION_ROUTES, output / "routes.bin",
                "Full", output / "configuration.json");

  EXPECT_ACTION(plan, 2, 0, 1, test_rules::ACTION_GATE, output / "gate.bin", "",
                output / "routes.bin");

  EXPECT_TOTAL_FILES(
      plan, entries, output / "configuration.json", output / "version.json",
      output / "routes.bin", output / "gate.bin",
      output / "primary" / "foo" / "%" / "primary.bin",
      output / "secondary" / "public" / "foo" / "%" / "metadata.bin",
      output / "secondary" / "public" / "foo" / "%" / "web.bin",
      output / "secondary" / "public" / "%" / "listing.bin");
}

TEST(incremental_unrecorded_missing_global_is_not_demanded) {
  const auto output{delta_path("unrecorded_gate")};
  WRITE_GLOBAL_OUTPUTS(output);
  std::filesystem::remove(output / "gate.bin");
  sourcemeta::one::BuildState entries;
  const TestLeaves schemas{
      {"https://example.com/foo", "/src/foo.json", "foo", MTIME(100)}};
  ADD_LEAF_ENTRIES(entries, output, "foo", true, MTIME(50));
  entries.emplace(output / "configuration.json",
                  {.file_mark = MTIME(150), .dependencies = {}});
  entries.emplace(output / "version.json",
                  {.file_mark = MTIME(150), .dependencies = {}});
  entries.emplace(output / "routes.bin",
                  {.file_mark = MTIME(150), .dependencies = {}});
  entries.emplace(output / "secondary" / "public" / "%" / "listing.bin",
                  {.file_mark = MTIME(150), .dependencies = {}});

  entries.configure(test_rules::RULES.leaves, test_rules::RULES.directories,
                    sourcemeta::one::rules_fingerprint<test_rules::RULES>(),
                    INPUTS, test_rules::RULES.sentinel);
  const auto plan{sourcemeta::one::delta<test_rules::RULES>(
      sourcemeta::one::BuildPhase::Produce, test_rules::MODE_FULL, entries,
      output, schemas, "1.0.0", true, "", "Full", {}, VIEWS, everything())};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, test_rules::MODE_FULL, 3, 4);

  EXPECT_ACTION(plan, 0, 0, 1, test_rules::ACTION_PRIMARY,
                output / "primary" / "foo" / "%" / "primary.bin",
                "https://example.com/foo",
                std::filesystem::path{"/"} / "src" / "foo.json",
                output / "configuration.json");

  EXPECT_ACTION(plan, 1, 0, 1, test_rules::ACTION_METADATA,
                output / "secondary" / "public" / "foo" / "%" / "metadata.bin",
                "https://example.com/foo",
                output / "primary" / "foo" / "%" / "primary.bin");

  EXPECT_ACTION_UNORDERED(
      plan, 2, 0, 2, test_rules::ACTION_LISTING,
      output / "secondary" / "public" / "%" / "listing.bin", "",
      output / "secondary" / "public" / "foo" / "%" / "metadata.bin");
  EXPECT_ACTION_UNORDERED(
      plan, 2, 1, 2, test_rules::ACTION_WEB,
      output / "secondary" / "public" / "foo" / "%" / "web.bin",
      "https://example.com/foo",
      output / "secondary" / "public" / "foo" / "%" / "metadata.bin");
}

TEST(incremental_missing_global_repairs_alone_when_nothing_else_changed) {
  const auto output{delta_path("missing_alone")};
  WRITE_GLOBAL_OUTPUTS(output);
  sourcemeta::one::BuildState entries;
  const TestLeaves schemas;
  ADD_GLOBAL_ENTRIES(entries, output, MTIME(150));

  entries.configure(test_rules::RULES.leaves, test_rules::RULES.directories,
                    sourcemeta::one::rules_fingerprint<test_rules::RULES>(),
                    INPUTS, test_rules::RULES.sentinel);
  const auto clean_plan{sourcemeta::one::delta<test_rules::RULES>(
      sourcemeta::one::BuildPhase::Produce, test_rules::MODE_FULL, entries,
      output, schemas, "1.0.0", true, "", "Full", {}, VIEWS, everything())};
  EXPECT_EQ(clean_plan.waves.size(), 0);
  EXPECT_EQ(clean_plan.size, 0);

  std::filesystem::remove(output / "version.json");
  const auto plan{sourcemeta::one::delta<test_rules::RULES>(
      sourcemeta::one::BuildPhase::Produce, test_rules::MODE_FULL, entries,
      output, schemas, "1.0.0", true, "", "Full", {}, VIEWS, everything())};

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

  entries.configure(test_rules::RULES.leaves, test_rules::RULES.directories,
                    sourcemeta::one::rules_fingerprint<test_rules::RULES>(),
                    INPUTS, test_rules::RULES.sentinel);
  const auto plan{sourcemeta::one::delta<test_rules::RULES>(
      sourcemeta::one::BuildPhase::Produce, test_rules::MODE_FULL, entries,
      output, schemas, "1.0.0", false, "", "Full",
      {.maximum_direct_directory_entries = 0}, VIEWS, everything())};
  EXPECT_EQ(plan.size, 17u);
}

TEST(limits_within_threshold_succeeds) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  const TestLeaves schemas{
      {"https://example.com/a", "/src/a.json", "a", MTIME(100)},
      {"https://example.com/b", "/src/b.json", "b", MTIME(100)}};

  entries.configure(test_rules::RULES.leaves, test_rules::RULES.directories,
                    sourcemeta::one::rules_fingerprint<test_rules::RULES>(),
                    INPUTS, test_rules::RULES.sentinel);
  const auto plan{sourcemeta::one::delta<test_rules::RULES>(
      sourcemeta::one::BuildPhase::Produce, test_rules::MODE_FULL, entries,
      output, schemas, "1.0.0", false, "", "Full",
      {.maximum_direct_directory_entries = 5}, VIEWS, everything())};
  EXPECT_EQ(plan.size, 11u);
}

TEST(a_named_view_is_a_segment_of_the_namespaced_tree_alone) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  const TestLeaves schemas{
      {"https://example.com/foo", "/src/foo.json", "foo", MTIME(100)}};
  static constexpr std::array<std::string_view, 1> named{{"alpha"}};

  entries.configure(test_rules::RULES.leaves, test_rules::RULES.directories,
                    sourcemeta::one::rules_fingerprint<test_rules::RULES>(),
                    INPUTS, test_rules::RULES.sentinel);
  const auto plan{sourcemeta::one::delta<test_rules::RULES>(
      sourcemeta::one::BuildPhase::Produce, test_rules::MODE_FULL, entries,
      output, schemas, "1.0.0", false, "", "Full", {}, named, everything())};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, test_rules::MODE_FULL, 6, 8);

  EXPECT_ACTION(plan, 0, 0, 2, test_rules::ACTION_CONFIGURATION,
                output / "configuration.json", "");
  EXPECT_ACTION(plan, 0, 1, 2, test_rules::ACTION_VERSION,
                output / "version.json", "1.0.0");

  EXPECT_ACTION(plan, 1, 0, 1, test_rules::ACTION_ROUTES, output / "routes.bin",
                "Full", output / "configuration.json");

  EXPECT_ACTION(plan, 2, 0, 1, test_rules::ACTION_GATE, output / "gate.bin", "",
                output / "routes.bin");

  // The tree that is not namespaced is untouched by the name, which is what
  // makes the segment below a property of the tree rather than of the build
  EXPECT_ACTION(plan, 3, 0, 1, test_rules::ACTION_PRIMARY,
                output / "primary" / "foo" / "%" / "primary.bin",
                "https://example.com/foo",
                std::filesystem::path{"/"} / "src" / "foo.json",
                output / "configuration.json");

  EXPECT_ACTION(plan, 4, 0, 1, test_rules::ACTION_METADATA,
                output / "secondary" / "alpha" / "foo" / "%" / "metadata.bin",
                "https://example.com/foo",
                output / "primary" / "foo" / "%" / "primary.bin");

  EXPECT_ACTION_UNORDERED(
      plan, 5, 0, 2, test_rules::ACTION_LISTING,
      output / "secondary" / "alpha" / "%" / "listing.bin", "",
      output / "secondary" / "alpha" / "foo" / "%" / "metadata.bin");
  EXPECT_ACTION_UNORDERED(
      plan, 5, 1, 2, test_rules::ACTION_WEB,
      output / "secondary" / "alpha" / "foo" / "%" / "web.bin",
      "https://example.com/foo",
      output / "secondary" / "alpha" / "foo" / "%" / "metadata.bin");

  EXPECT_TOTAL_FILES(
      plan, entries, output / "configuration.json", output / "version.json",
      output / "routes.bin", output / "gate.bin",
      output / "primary" / "foo" / "%" / "primary.bin",
      output / "secondary" / "alpha" / "foo" / "%" / "metadata.bin",
      output / "secondary" / "alpha" / "foo" / "%" / "web.bin",
      output / "secondary" / "alpha" / "%" / "listing.bin");
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
      test_rules::COMBINE_RULES.leaves, test_rules::COMBINE_RULES.directories,
      sourcemeta::one::rules_fingerprint<test_rules::COMBINE_RULES>(), INPUTS,
      test_rules::COMBINE_RULES.sentinel);
  const auto plan{sourcemeta::one::delta<test_rules::COMBINE_RULES>(
      sourcemeta::one::BuildPhase::Combine, test_rules::MODE_FULL, entries,
      output, schemas, "1.0.0", true, "", "Full", {}, VIEWS, everything())};

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
      test_rules::COMBINE_RULES.leaves, test_rules::COMBINE_RULES.directories,
      sourcemeta::one::rules_fingerprint<test_rules::COMBINE_RULES>(), INPUTS,
      test_rules::COMBINE_RULES.sentinel);
  previous.save(state);

  sourcemeta::one::BuildState entries;
  entries.load(state, test_rules::COMBINE_RULES.leaves,
               test_rules::COMBINE_RULES.directories,
               sourcemeta::one::rules_fingerprint<test_rules::COMBINE_RULES>(),
               INPUTS, test_rules::COMBINE_RULES.sentinel);
  entries.commit(output / "primary" / "bar" / "%" / "references.bin",
                 {output / "primary" / "foo" / "%" / "primary.bin"});

  const auto plan{sourcemeta::one::delta<test_rules::COMBINE_RULES>(
      sourcemeta::one::BuildPhase::Combine, test_rules::MODE_FULL, entries,
      output, schemas, "1.0.0", true, "", "Full", {}, VIEWS, everything())};

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
      test_rules::COMBINE_RULES.leaves, test_rules::COMBINE_RULES.directories,
      sourcemeta::one::rules_fingerprint<test_rules::COMBINE_RULES>(), INPUTS,
      test_rules::COMBINE_RULES.sentinel);
  previous.save(state);

  sourcemeta::one::BuildState entries;
  entries.load(state, test_rules::COMBINE_RULES.leaves,
               test_rules::COMBINE_RULES.directories,
               sourcemeta::one::rules_fingerprint<test_rules::COMBINE_RULES>(),
               INPUTS, test_rules::COMBINE_RULES.sentinel);
  entries.commit(output / "primary" / "bar" / "%" / "references.bin",
                 {output / "primary" / "foo" / "%" / "primary.bin"});

  const auto plan{sourcemeta::one::delta<test_rules::COMBINE_RULES>(
      sourcemeta::one::BuildPhase::Combine, test_rules::MODE_FULL, entries,
      output, schemas, "1.0.0", true, "", "Full", {}, VIEWS, everything())};

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
      test_rules::COMBINE_RULES.leaves, test_rules::COMBINE_RULES.directories,
      sourcemeta::one::rules_fingerprint<test_rules::COMBINE_RULES>(), INPUTS,
      test_rules::COMBINE_RULES.sentinel);
  previous.save(state);

  sourcemeta::one::BuildState entries;
  entries.load(state, test_rules::COMBINE_RULES.leaves,
               test_rules::COMBINE_RULES.directories,
               sourcemeta::one::rules_fingerprint<test_rules::COMBINE_RULES>(),
               INPUTS, test_rules::COMBINE_RULES.sentinel);
  entries.commit(output / "primary" / "bar" / "%" / "references.bin", {});

  const auto plan{sourcemeta::one::delta<test_rules::COMBINE_RULES>(
      sourcemeta::one::BuildPhase::Combine, test_rules::MODE_FULL, entries,
      output, schemas, "1.0.0", true, "", "Full", {}, VIEWS, everything())};

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
      test_rules::COMBINE_RULES_SECONDARY.directories,
      sourcemeta::one::rules_fingerprint<test_rules::COMBINE_RULES_SECONDARY>(),
      INPUTS, test_rules::COMBINE_RULES_SECONDARY.sentinel);
  const auto plan{sourcemeta::one::delta<test_rules::COMBINE_RULES_SECONDARY>(
      sourcemeta::one::BuildPhase::Combine, test_rules::MODE_FULL, entries,
      output, schemas, "1.0.0", true, "", "Full", {}, VIEWS, everything())};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, test_rules::MODE_FULL, 1, 1);

  EXPECT_ACTION(plan, 0, 0, 1, test_rules::ACTION_REVERSE,
                output / "secondary" / "public" / "foo" / "%" / "reverse.bin",
                "https://example.com/foo");

  EXPECT_TOTAL_FILES(
      plan, entries, output / "primary" / "foo" / "%" / "references.bin",
      output / "secondary" / "public" / "foo" / "%" / "reverse.bin");
}

TEST(limits_exceeded_throws) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  const TestLeaves schemas{
      {"https://example.com/a", "/src/a.json", "a", MTIME(100)},
      {"https://example.com/b", "/src/b.json", "b", MTIME(100)},
      {"https://example.com/c", "/src/c.json", "c", MTIME(100)}};

  entries.configure(test_rules::RULES.leaves, test_rules::RULES.directories,
                    sourcemeta::one::rules_fingerprint<test_rules::RULES>(),
                    INPUTS, test_rules::RULES.sentinel);
  try {
    sourcemeta::one::delta<test_rules::RULES>(
        sourcemeta::one::BuildPhase::Produce, test_rules::MODE_FULL, entries,
        output, schemas, "1.0.0", false, "", "Full",
        {.maximum_direct_directory_entries = 2}, VIEWS, everything());
    FAIL();
  } catch (const sourcemeta::one::BuildTooManyDirectoryEntriesError &error) {
    EXPECT_STREQ(error.what(), "Too many entries in a single directory");
  }
}
