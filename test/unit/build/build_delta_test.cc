#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/one/build.h>

#include "build_test_utils.h"

#include <filesystem>    // std::filesystem::path
#include <string>        // std::string
#include <unordered_map> // std::unordered_map
#include <vector>        // std::vector

TEST(Build_delta, full_empty_registry) {
  const std::filesystem::path output{"/output"};
  const sourcemeta::one::BuildState entries;
  const sourcemeta::one::Resolver::Views schemas;
  const std::vector<std::filesystem::path> changed;
  const std::vector<std::filesystem::path> removed;
  const auto plan{sourcemeta::one::delta(sourcemeta::one::BuildPhase::Produce,
                                         sourcemeta::one::BuildPlan::Type::Full,
                                         entries, output, schemas, "1.0.0",
                                         false, "", changed, removed)};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, Full, 4, 7);

  EXPECT_ACTION(plan, 0, 0, 2, Configuration, output / "configuration.json",
                "");
  EXPECT_ACTION(plan, 0, 1, 2, Version, output / "version.json", "1.0.0");

  EXPECT_ACTION(plan, 1, 0, 3, WebNotFound,
                output / "explorer" / "%" / "404.metapack", "",
                output / "configuration.json");
  EXPECT_ACTION(plan, 1, 1, 3, DirectoryList,
                output / "explorer" / "%" / "directory.metapack", "");
  EXPECT_ACTION(plan, 1, 2, 3, SearchIndex,
                output / "explorer" / "%" / "search.metapack", "");

  EXPECT_ACTION(plan, 2, 0, 1, WebIndex,
                output / "explorer" / "%" / "directory-html.metapack", "",
                output / "explorer" / "%" / "directory.metapack");

  EXPECT_ACTION(plan, 3, 0, 1, Routes, output / "routes.bin", "Full",
                output / "configuration.json");

  EXPECT_TOTAL_FILES(plan, entries, output / "configuration.json",
                     output / "version.json",
                     output / "explorer" / "%" / "search.metapack",
                     output / "explorer" / "%" / "directory.metapack",
                     output / "explorer" / "%" / "404.metapack",
                     output / "explorer" / "%" / "directory-html.metapack",
                     output / "routes.bin");
}

TEST(Build_delta, full_single_schema) {
  const std::filesystem::path output{"/output"};
  const sourcemeta::one::BuildState entries;
  const sourcemeta::one::Resolver::Views schemas{
      {"https://example.com/foo", {"/src/foo.json", "foo", MTIME(100)}}};
  const std::vector<std::filesystem::path> changed;
  const std::vector<std::filesystem::path> removed;
  const auto plan{sourcemeta::one::delta(sourcemeta::one::BuildPhase::Produce,
                                         sourcemeta::one::BuildPlan::Type::Full,
                                         entries, output, schemas, "1.0.0",
                                         false, "", changed, removed)};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, Full, 8, 19);

  EXPECT_ACTION(plan, 0, 0, 2, Configuration, output / "configuration.json",
                "");
  EXPECT_ACTION(plan, 0, 1, 2, Version, output / "version.json", "1.0.0");

  EXPECT_ACTION(plan, 1, 0, 2, WebNotFound,
                output / "explorer" / "%" / "404.metapack", "",
                output / "configuration.json");
  EXPECT_ACTION(plan, 1, 1, 2, Materialise,
                output / "schemas" / "foo" / "%" / "schema.metapack",
                "https://example.com/foo",
                std::filesystem::path{"/"} / "src" / "foo.json",
                output / "configuration.json");

  EXPECT_ACTION(plan, 2, 0, 4, Dependencies,
                output / "schemas" / "foo" / "%" / "dependencies.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 2, 1, 4, Locations,
                output / "schemas" / "foo" / "%" / "locations.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 2, 2, 4, Positions,
                output / "schemas" / "foo" / "%" / "positions.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 2, 3, 4, Stats,
                output / "schemas" / "foo" / "%" / "stats.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");

  EXPECT_ACTION(plan, 3, 0, 2, Bundle,
                output / "schemas" / "foo" / "%" / "bundle.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "dependencies.metapack");
  EXPECT_ACTION(plan, 3, 1, 2, Health,
                output / "schemas" / "foo" / "%" / "health.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "dependencies.metapack");

  EXPECT_ACTION(plan, 4, 0, 4, SchemaMetadata,
                output / "explorer" / "foo" / "%" / "schema.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "health.metapack",
                output / "schemas" / "foo" / "%" / "dependencies.metapack");
  EXPECT_ACTION(plan, 4, 1, 4, BlazeExhaustive,
                output / "schemas" / "foo" / "%" / "blaze-exhaustive.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "bundle.metapack");
  EXPECT_ACTION(plan, 4, 2, 4, BlazeFast,
                output / "schemas" / "foo" / "%" / "blaze-fast.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "bundle.metapack");
  EXPECT_ACTION(plan, 4, 3, 4, Editor,
                output / "schemas" / "foo" / "%" / "editor.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "bundle.metapack");

  EXPECT_ACTION(plan, 5, 0, 3, DirectoryList,
                output / "explorer" / "%" / "directory.metapack", "",
                output / "explorer" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 5, 1, 3, SearchIndex,
                output / "explorer" / "%" / "search.metapack", "",
                output / "explorer" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 5, 2, 3, WebSchema,
                output / "explorer" / "foo" / "%" / "schema-html.metapack",
                "https://example.com/foo",
                output / "explorer" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "health.metapack");

  EXPECT_ACTION(plan, 6, 0, 1, WebIndex,
                output / "explorer" / "%" / "directory-html.metapack", "",
                output / "explorer" / "%" / "directory.metapack");

  EXPECT_ACTION(plan, 7, 0, 1, Routes, output / "routes.bin", "Full",
                output / "configuration.json");

  EXPECT_TOTAL_FILES(
      plan, entries, output / "configuration.json", output / "version.json",
      output / "schemas" / "foo" / "%" / "schema.metapack",
      output / "schemas" / "foo" / "%" / "dependencies.metapack",
      output / "schemas" / "foo" / "%" / "locations.metapack",
      output / "schemas" / "foo" / "%" / "positions.metapack",
      output / "schemas" / "foo" / "%" / "stats.metapack",
      output / "schemas" / "foo" / "%" / "bundle.metapack",
      output / "schemas" / "foo" / "%" / "health.metapack",
      output / "schemas" / "foo" / "%" / "blaze-exhaustive.metapack",
      output / "schemas" / "foo" / "%" / "blaze-fast.metapack",
      output / "schemas" / "foo" / "%" / "editor.metapack",
      output / "explorer" / "foo" / "%" / "schema.metapack",
      output / "explorer" / "foo" / "%" / "schema-html.metapack",
      output / "explorer" / "%" / "search.metapack",
      output / "explorer" / "%" / "directory.metapack",
      output / "explorer" / "%" / "directory-html.metapack",
      output / "explorer" / "%" / "404.metapack", output / "routes.bin");
}

TEST(Build_delta, incremental_changed_same_mtime) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  entries.emplace(output / "version.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "configuration.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  ADD_SCHEMA_ENTRIES(entries, output, "foo", true, true, MTIME(100));
  entries.emplace(output / "explorer" / "%" / "search.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "foo" / "%" / "directory.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "directory.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "directory-html.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "404.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "routes.bin",
                  {.file_mark = MTIME(100), .dependencies = {}});
  const sourcemeta::one::Resolver::Views schemas{
      {"https://example.com/foo", {"/src/foo.json", "foo", MTIME(100)}}};
  const std::vector<std::filesystem::path> changed{"/src/foo.json"};
  const std::vector<std::filesystem::path> removed;

  const auto plan{sourcemeta::one::delta(sourcemeta::one::BuildPhase::Produce,
                                         sourcemeta::one::BuildPlan::Type::Full,
                                         entries, output, schemas, "1.0.0",
                                         true, "", changed, removed)};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, Full, 0, 0);

  EXPECT_TOTAL_FILES(
      plan, entries, output / "version.json", output / "configuration.json",
      output / "schemas" / "foo" / "%" / "schema.metapack",
      output / "schemas" / "foo" / "%" / "dependencies.metapack",
      output / "schemas" / "foo" / "%" / "locations.metapack",
      output / "schemas" / "foo" / "%" / "positions.metapack",
      output / "schemas" / "foo" / "%" / "stats.metapack",
      output / "schemas" / "foo" / "%" / "bundle.metapack",
      output / "schemas" / "foo" / "%" / "health.metapack",
      output / "schemas" / "foo" / "%" / "blaze-exhaustive.metapack",
      output / "schemas" / "foo" / "%" / "blaze-fast.metapack",
      output / "schemas" / "foo" / "%" / "editor.metapack",
      output / "explorer" / "foo" / "%" / "schema.metapack",
      output / "explorer" / "foo" / "%" / "schema-html.metapack",
      output / "explorer" / "foo" / "%" / "directory.metapack",
      output / "explorer" / "foo" / "%" / "directory-html.metapack",
      output / "explorer" / "%" / "search.metapack",
      output / "explorer" / "%" / "directory.metapack",
      output / "explorer" / "%" / "directory-html.metapack",
      output / "explorer" / "%" / "404.metapack", output / "routes.bin");
}

TEST(Build_delta, incremental_missing_schema_metapack) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  entries.emplace(output / "version.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "schemas" / "bar" / "%" / "dependencies.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "bar" / "%" / "schema.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "configuration.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  const sourcemeta::one::Resolver::Views schemas{
      {"https://example.com/bar", {"/src/bar.json", "bar", MTIME(100)}}};
  const std::vector<std::filesystem::path> changed;
  const std::vector<std::filesystem::path> removed;

  const auto plan{sourcemeta::one::delta(sourcemeta::one::BuildPhase::Produce,
                                         sourcemeta::one::BuildPlan::Type::Full,
                                         entries, output, schemas, "1.0.0",
                                         true, "", changed, removed)};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, Full, 7, 16);

  EXPECT_ACTION(plan, 0, 0, 1, Materialise,
                output / "schemas" / "bar" / "%" / "schema.metapack",
                "https://example.com/bar",
                std::filesystem::path{"/"} / "src" / "bar.json",
                output / "configuration.json");

  EXPECT_ACTION(plan, 1, 0, 4, Dependencies,
                output / "schemas" / "bar" / "%" / "dependencies.metapack",
                "https://example.com/bar",
                output / "schemas" / "bar" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 1, 1, 4, Locations,
                output / "schemas" / "bar" / "%" / "locations.metapack",
                "https://example.com/bar",
                output / "schemas" / "bar" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 1, 2, 4, Positions,
                output / "schemas" / "bar" / "%" / "positions.metapack",
                "https://example.com/bar",
                output / "schemas" / "bar" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 1, 3, 4, Stats,
                output / "schemas" / "bar" / "%" / "stats.metapack",
                "https://example.com/bar",
                output / "schemas" / "bar" / "%" / "schema.metapack");

  EXPECT_ACTION(plan, 2, 0, 2, Bundle,
                output / "schemas" / "bar" / "%" / "bundle.metapack",
                "https://example.com/bar",
                output / "schemas" / "bar" / "%" / "schema.metapack",
                output / "schemas" / "bar" / "%" / "dependencies.metapack");
  EXPECT_ACTION(plan, 2, 1, 2, Health,
                output / "schemas" / "bar" / "%" / "health.metapack",
                "https://example.com/bar",
                output / "schemas" / "bar" / "%" / "schema.metapack",
                output / "schemas" / "bar" / "%" / "dependencies.metapack");

  EXPECT_ACTION(plan, 3, 0, 4, SchemaMetadata,
                output / "explorer" / "bar" / "%" / "schema.metapack",
                "https://example.com/bar",
                output / "schemas" / "bar" / "%" / "schema.metapack",
                output / "schemas" / "bar" / "%" / "health.metapack",
                output / "schemas" / "bar" / "%" / "dependencies.metapack");
  EXPECT_ACTION(plan, 3, 1, 4, BlazeExhaustive,
                output / "schemas" / "bar" / "%" / "blaze-exhaustive.metapack",
                "https://example.com/bar",
                output / "schemas" / "bar" / "%" / "bundle.metapack");
  EXPECT_ACTION(plan, 3, 2, 4, BlazeFast,
                output / "schemas" / "bar" / "%" / "blaze-fast.metapack",
                "https://example.com/bar",
                output / "schemas" / "bar" / "%" / "bundle.metapack");
  EXPECT_ACTION(plan, 3, 3, 4, Editor,
                output / "schemas" / "bar" / "%" / "editor.metapack",
                "https://example.com/bar",
                output / "schemas" / "bar" / "%" / "bundle.metapack");

  EXPECT_ACTION(plan, 4, 0, 3, DirectoryList,
                output / "explorer" / "%" / "directory.metapack", "",
                output / "explorer" / "bar" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 4, 1, 3, SearchIndex,
                output / "explorer" / "%" / "search.metapack", "",
                output / "explorer" / "bar" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 4, 2, 3, WebSchema,
                output / "explorer" / "bar" / "%" / "schema-html.metapack",
                "https://example.com/bar",
                output / "explorer" / "bar" / "%" / "schema.metapack",
                output / "schemas" / "bar" / "%" / "health.metapack");

  EXPECT_ACTION(plan, 5, 0, 1, WebIndex,
                output / "explorer" / "%" / "directory-html.metapack", "",
                output / "explorer" / "%" / "directory.metapack");

  EXPECT_ACTION(plan, 6, 0, 1, Routes, output / "routes.bin", "Full",
                output / "configuration.json");

  EXPECT_TOTAL_FILES(
      plan, entries, output / "version.json", output / "configuration.json",
      output / "schemas" / "bar" / "%" / "schema.metapack",
      output / "schemas" / "bar" / "%" / "dependencies.metapack",
      output / "schemas" / "bar" / "%" / "locations.metapack",
      output / "schemas" / "bar" / "%" / "positions.metapack",
      output / "schemas" / "bar" / "%" / "stats.metapack",
      output / "schemas" / "bar" / "%" / "bundle.metapack",
      output / "schemas" / "bar" / "%" / "health.metapack",
      output / "schemas" / "bar" / "%" / "blaze-exhaustive.metapack",
      output / "schemas" / "bar" / "%" / "blaze-fast.metapack",
      output / "schemas" / "bar" / "%" / "editor.metapack",
      output / "explorer" / "bar" / "%" / "schema.metapack",
      output / "explorer" / "bar" / "%" / "schema-html.metapack",
      output / "explorer" / "%" / "search.metapack",
      output / "explorer" / "%" / "directory.metapack",
      output / "explorer" / "%" / "directory-html.metapack",
      output / "routes.bin");
}

TEST(Build_delta, incremental_one_schema_added) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  entries.emplace(output / "version.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "configuration.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "schemas" / "bar" / "%" / "schema.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "schemas" / "bar" / "%" / "dependencies.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "schemas" / "bar" / "%" / "locations.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "schemas" / "bar" / "%" / "positions.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "schemas" / "bar" / "%" / "stats.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "schemas" / "bar" / "%" / "bundle.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "schemas" / "bar" / "%" / "health.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "schemas" / "bar" / "%" /
                      "blaze-exhaustive.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "schemas" / "bar" / "%" / "blaze-fast.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "schemas" / "bar" / "%" / "editor.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "bar" / "%" / "schema.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "bar" / "%" / "directory.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "bar" / "%" / "directory-html.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "bar" / "%" / "schema-html.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "search.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "directory.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "directory-html.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "404.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "routes.bin",
                  {.file_mark = MTIME(100), .dependencies = {}});
  const sourcemeta::one::Resolver::Views schemas{
      {"https://example.com/foo", {"/src/foo.json", "foo", MTIME(200)}},
      {"https://example.com/bar", {"/src/bar.json", "bar", MTIME(100)}}};
  const std::vector<std::filesystem::path> changed{"/src/foo.json"};
  const std::vector<std::filesystem::path> removed;

  const auto plan{sourcemeta::one::delta(sourcemeta::one::BuildPhase::Produce,
                                         sourcemeta::one::BuildPlan::Type::Full,
                                         entries, output, schemas, "1.0.0",
                                         true, "", changed, removed)};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, Full, 6, 15);

  EXPECT_ACTION(plan, 0, 0, 1, Materialise,
                output / "schemas" / "foo" / "%" / "schema.metapack",
                "https://example.com/foo",
                std::filesystem::path{"/"} / "src" / "foo.json",
                output / "configuration.json");

  EXPECT_ACTION(plan, 1, 0, 4, Dependencies,
                output / "schemas" / "foo" / "%" / "dependencies.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 1, 1, 4, Locations,
                output / "schemas" / "foo" / "%" / "locations.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 1, 2, 4, Positions,
                output / "schemas" / "foo" / "%" / "positions.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 1, 3, 4, Stats,
                output / "schemas" / "foo" / "%" / "stats.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");

  EXPECT_ACTION(plan, 2, 0, 2, Bundle,
                output / "schemas" / "foo" / "%" / "bundle.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "dependencies.metapack");
  EXPECT_ACTION(plan, 2, 1, 2, Health,
                output / "schemas" / "foo" / "%" / "health.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "dependencies.metapack");

  EXPECT_ACTION(plan, 3, 0, 4, SchemaMetadata,
                output / "explorer" / "foo" / "%" / "schema.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "health.metapack",
                output / "schemas" / "foo" / "%" / "dependencies.metapack");
  EXPECT_ACTION(plan, 3, 1, 4, BlazeExhaustive,
                output / "schemas" / "foo" / "%" / "blaze-exhaustive.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "bundle.metapack");
  EXPECT_ACTION(plan, 3, 2, 4, BlazeFast,
                output / "schemas" / "foo" / "%" / "blaze-fast.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "bundle.metapack");
  EXPECT_ACTION(plan, 3, 3, 4, Editor,
                output / "schemas" / "foo" / "%" / "editor.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "bundle.metapack");

  EXPECT_ACTION_UNORDERED(plan, 4, 0, 3, DirectoryList,
                          output / "explorer" / "%" / "directory.metapack", "",
                          output / "explorer" / "foo" / "%" / "schema.metapack",
                          output / "explorer" / "bar" / "%" /
                              "schema.metapack");
  EXPECT_ACTION_UNORDERED(
      plan, 4, 1, 3, SearchIndex, output / "explorer" / "%" / "search.metapack",
      "", output / "explorer" / "foo" / "%" / "schema.metapack",
      output / "explorer" / "bar" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 4, 2, 3, WebSchema,
                output / "explorer" / "foo" / "%" / "schema-html.metapack",
                "https://example.com/foo",
                output / "explorer" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "health.metapack");

  EXPECT_ACTION(plan, 5, 0, 1, WebIndex,
                output / "explorer" / "%" / "directory-html.metapack", "",
                output / "explorer" / "%" / "directory.metapack");

  EXPECT_TOTAL_FILES(
      plan, entries, output / "version.json", output / "configuration.json",
      output / "schemas" / "bar" / "%" / "schema.metapack",
      output / "schemas" / "bar" / "%" / "dependencies.metapack",
      output / "schemas" / "bar" / "%" / "locations.metapack",
      output / "schemas" / "bar" / "%" / "positions.metapack",
      output / "schemas" / "bar" / "%" / "stats.metapack",
      output / "schemas" / "bar" / "%" / "bundle.metapack",
      output / "schemas" / "bar" / "%" / "health.metapack",
      output / "schemas" / "bar" / "%" / "blaze-exhaustive.metapack",
      output / "schemas" / "bar" / "%" / "blaze-fast.metapack",
      output / "schemas" / "bar" / "%" / "editor.metapack",
      output / "explorer" / "bar" / "%" / "schema.metapack",
      output / "explorer" / "bar" / "%" / "schema-html.metapack",
      output / "explorer" / "bar" / "%" / "directory.metapack",
      output / "explorer" / "bar" / "%" / "directory-html.metapack",
      output / "schemas" / "foo" / "%" / "schema.metapack",
      output / "schemas" / "foo" / "%" / "dependencies.metapack",
      output / "schemas" / "foo" / "%" / "locations.metapack",
      output / "schemas" / "foo" / "%" / "positions.metapack",
      output / "schemas" / "foo" / "%" / "stats.metapack",
      output / "schemas" / "foo" / "%" / "bundle.metapack",
      output / "schemas" / "foo" / "%" / "health.metapack",
      output / "schemas" / "foo" / "%" / "blaze-exhaustive.metapack",
      output / "schemas" / "foo" / "%" / "blaze-fast.metapack",
      output / "schemas" / "foo" / "%" / "editor.metapack",
      output / "explorer" / "foo" / "%" / "schema.metapack",
      output / "explorer" / "foo" / "%" / "schema-html.metapack",
      output / "explorer" / "%" / "search.metapack",
      output / "explorer" / "%" / "directory.metapack",
      output / "explorer" / "%" / "directory-html.metapack",
      output / "explorer" / "%" / "404.metapack", output / "routes.bin");
}

TEST(Build_delta, incremental_removed_schema) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  entries.emplace(output / "version.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "configuration.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  const sourcemeta::one::Resolver::Views schemas{
      {"https://example.com/foo", {"/src/foo.json", "foo", MTIME(100)}}};
  const std::vector<std::filesystem::path> changed;
  const std::vector<std::filesystem::path> removed{"/src/foo.json"};

  const auto plan{sourcemeta::one::delta(sourcemeta::one::BuildPhase::Produce,
                                         sourcemeta::one::BuildPlan::Type::Full,
                                         entries, output, schemas, "1.0.0",
                                         true, "", changed, removed)};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, Full, 4, 6);

  EXPECT_ACTION(plan, 0, 0, 2, DirectoryList,
                output / "explorer" / "%" / "directory.metapack", "");
  EXPECT_ACTION(plan, 0, 1, 2, SearchIndex,
                output / "explorer" / "%" / "search.metapack", "");

  EXPECT_ACTION(plan, 1, 0, 1, WebIndex,
                output / "explorer" / "%" / "directory-html.metapack", "",
                output / "explorer" / "%" / "directory.metapack");

  EXPECT_ACTION(plan, 2, 0, 1, Routes, output / "routes.bin", "Full",
                output / "configuration.json");

  EXPECT_ACTION(plan, 3, 0, 2, Remove, output / "explorer" / "foo" / "%", "");
  EXPECT_ACTION(plan, 3, 1, 2, Remove, output / "schemas" / "foo" / "%", "");

  EXPECT_TOTAL_FILES(plan, entries, output / "version.json",
                     output / "configuration.json",
                     output / "explorer" / "%" / "search.metapack",
                     output / "explorer" / "%" / "directory.metapack",
                     output / "explorer" / "%" / "directory-html.metapack",
                     output / "routes.bin");
}

TEST(Build_delta, full_stale_file_in_entries) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  entries.emplace(output / "schemas" / "ghost" / "%" / "schema.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  const sourcemeta::one::Resolver::Views schemas{
      {"https://example.com/foo", {"/src/foo.json", "foo", MTIME(100)}}};
  const std::vector<std::filesystem::path> changed;
  const std::vector<std::filesystem::path> removed;
  const auto plan{sourcemeta::one::delta(sourcemeta::one::BuildPhase::Produce,
                                         sourcemeta::one::BuildPlan::Type::Full,
                                         entries, output, schemas, "1.0.0",
                                         false, "", changed, removed)};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, Full, 9, 20);

  EXPECT_ACTION(plan, 0, 0, 2, Configuration, output / "configuration.json",
                "");
  EXPECT_ACTION(plan, 0, 1, 2, Version, output / "version.json", "1.0.0");

  EXPECT_ACTION(plan, 1, 0, 2, WebNotFound,
                output / "explorer" / "%" / "404.metapack", "",
                output / "configuration.json");
  EXPECT_ACTION(plan, 1, 1, 2, Materialise,
                output / "schemas" / "foo" / "%" / "schema.metapack",
                "https://example.com/foo",
                std::filesystem::path{"/"} / "src" / "foo.json",
                output / "configuration.json");

  EXPECT_ACTION(plan, 2, 0, 4, Dependencies,
                output / "schemas" / "foo" / "%" / "dependencies.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 2, 1, 4, Locations,
                output / "schemas" / "foo" / "%" / "locations.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 2, 2, 4, Positions,
                output / "schemas" / "foo" / "%" / "positions.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 2, 3, 4, Stats,
                output / "schemas" / "foo" / "%" / "stats.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");

  EXPECT_ACTION(plan, 3, 0, 2, Bundle,
                output / "schemas" / "foo" / "%" / "bundle.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "dependencies.metapack");
  EXPECT_ACTION(plan, 3, 1, 2, Health,
                output / "schemas" / "foo" / "%" / "health.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "dependencies.metapack");

  EXPECT_ACTION(plan, 4, 0, 4, SchemaMetadata,
                output / "explorer" / "foo" / "%" / "schema.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "health.metapack",
                output / "schemas" / "foo" / "%" / "dependencies.metapack");
  EXPECT_ACTION(plan, 4, 1, 4, BlazeExhaustive,
                output / "schemas" / "foo" / "%" / "blaze-exhaustive.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "bundle.metapack");
  EXPECT_ACTION(plan, 4, 2, 4, BlazeFast,
                output / "schemas" / "foo" / "%" / "blaze-fast.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "bundle.metapack");
  EXPECT_ACTION(plan, 4, 3, 4, Editor,
                output / "schemas" / "foo" / "%" / "editor.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "bundle.metapack");

  EXPECT_ACTION(plan, 5, 0, 3, DirectoryList,
                output / "explorer" / "%" / "directory.metapack", "",
                output / "explorer" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 5, 1, 3, SearchIndex,
                output / "explorer" / "%" / "search.metapack", "",
                output / "explorer" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 5, 2, 3, WebSchema,
                output / "explorer" / "foo" / "%" / "schema-html.metapack",
                "https://example.com/foo",
                output / "explorer" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "health.metapack");

  EXPECT_ACTION(plan, 6, 0, 1, WebIndex,
                output / "explorer" / "%" / "directory-html.metapack", "",
                output / "explorer" / "%" / "directory.metapack");

  EXPECT_ACTION(plan, 7, 0, 1, Routes, output / "routes.bin", "Full",
                output / "configuration.json");

  EXPECT_ACTION(plan, 8, 0, 1, Remove, output / "schemas" / "ghost", "");

  EXPECT_TOTAL_FILES(
      plan, entries, output / "configuration.json", output / "version.json",
      output / "schemas" / "foo" / "%" / "schema.metapack",
      output / "schemas" / "foo" / "%" / "dependencies.metapack",
      output / "schemas" / "foo" / "%" / "locations.metapack",
      output / "schemas" / "foo" / "%" / "positions.metapack",
      output / "schemas" / "foo" / "%" / "stats.metapack",
      output / "schemas" / "foo" / "%" / "bundle.metapack",
      output / "schemas" / "foo" / "%" / "health.metapack",
      output / "schemas" / "foo" / "%" / "blaze-exhaustive.metapack",
      output / "schemas" / "foo" / "%" / "blaze-fast.metapack",
      output / "schemas" / "foo" / "%" / "editor.metapack",
      output / "explorer" / "foo" / "%" / "schema.metapack",
      output / "explorer" / "foo" / "%" / "schema-html.metapack",
      output / "explorer" / "%" / "search.metapack",
      output / "explorer" / "%" / "directory.metapack",
      output / "explorer" / "%" / "directory-html.metapack",
      output / "explorer" / "%" / "404.metapack", output / "routes.bin");
}

TEST(Build_delta, full_stale_directory_in_entries) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  entries.emplace(output / "schemas" / "ghost" / "%",
                  {.file_mark = MTIME(100), .dependencies = {}});
  const sourcemeta::one::Resolver::Views schemas{
      {"https://example.com/foo", {"/src/foo.json", "foo", MTIME(100)}}};
  const std::vector<std::filesystem::path> changed;
  const std::vector<std::filesystem::path> removed;
  const auto plan{sourcemeta::one::delta(sourcemeta::one::BuildPhase::Produce,
                                         sourcemeta::one::BuildPlan::Type::Full,
                                         entries, output, schemas, "1.0.0",
                                         false, "", changed, removed)};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, Full, 9, 20);

  EXPECT_ACTION(plan, 0, 0, 2, Configuration, output / "configuration.json",
                "");
  EXPECT_ACTION(plan, 0, 1, 2, Version, output / "version.json", "1.0.0");

  EXPECT_ACTION(plan, 1, 0, 2, WebNotFound,
                output / "explorer" / "%" / "404.metapack", "",
                output / "configuration.json");
  EXPECT_ACTION(plan, 1, 1, 2, Materialise,
                output / "schemas" / "foo" / "%" / "schema.metapack",
                "https://example.com/foo",
                std::filesystem::path{"/"} / "src" / "foo.json",
                output / "configuration.json");

  EXPECT_ACTION(plan, 2, 0, 4, Dependencies,
                output / "schemas" / "foo" / "%" / "dependencies.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 2, 1, 4, Locations,
                output / "schemas" / "foo" / "%" / "locations.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 2, 2, 4, Positions,
                output / "schemas" / "foo" / "%" / "positions.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 2, 3, 4, Stats,
                output / "schemas" / "foo" / "%" / "stats.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");

  EXPECT_ACTION(plan, 3, 0, 2, Bundle,
                output / "schemas" / "foo" / "%" / "bundle.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "dependencies.metapack");
  EXPECT_ACTION(plan, 3, 1, 2, Health,
                output / "schemas" / "foo" / "%" / "health.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "dependencies.metapack");

  EXPECT_ACTION(plan, 4, 0, 4, SchemaMetadata,
                output / "explorer" / "foo" / "%" / "schema.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "health.metapack",
                output / "schemas" / "foo" / "%" / "dependencies.metapack");
  EXPECT_ACTION(plan, 4, 1, 4, BlazeExhaustive,
                output / "schemas" / "foo" / "%" / "blaze-exhaustive.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "bundle.metapack");
  EXPECT_ACTION(plan, 4, 2, 4, BlazeFast,
                output / "schemas" / "foo" / "%" / "blaze-fast.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "bundle.metapack");
  EXPECT_ACTION(plan, 4, 3, 4, Editor,
                output / "schemas" / "foo" / "%" / "editor.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "bundle.metapack");

  EXPECT_ACTION(plan, 5, 0, 3, DirectoryList,
                output / "explorer" / "%" / "directory.metapack", "",
                output / "explorer" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 5, 1, 3, SearchIndex,
                output / "explorer" / "%" / "search.metapack", "",
                output / "explorer" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 5, 2, 3, WebSchema,
                output / "explorer" / "foo" / "%" / "schema-html.metapack",
                "https://example.com/foo",
                output / "explorer" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "health.metapack");

  EXPECT_ACTION(plan, 6, 0, 1, WebIndex,
                output / "explorer" / "%" / "directory-html.metapack", "",
                output / "explorer" / "%" / "directory.metapack");

  EXPECT_ACTION(plan, 7, 0, 1, Routes, output / "routes.bin", "Full",
                output / "configuration.json");

  EXPECT_ACTION(plan, 8, 0, 1, Remove, output / "schemas" / "ghost", "");

  EXPECT_TOTAL_FILES(
      plan, entries, output / "configuration.json", output / "version.json",
      output / "schemas" / "foo" / "%" / "schema.metapack",
      output / "schemas" / "foo" / "%" / "dependencies.metapack",
      output / "schemas" / "foo" / "%" / "locations.metapack",
      output / "schemas" / "foo" / "%" / "positions.metapack",
      output / "schemas" / "foo" / "%" / "stats.metapack",
      output / "schemas" / "foo" / "%" / "bundle.metapack",
      output / "schemas" / "foo" / "%" / "health.metapack",
      output / "schemas" / "foo" / "%" / "blaze-exhaustive.metapack",
      output / "schemas" / "foo" / "%" / "blaze-fast.metapack",
      output / "schemas" / "foo" / "%" / "editor.metapack",
      output / "explorer" / "foo" / "%" / "schema.metapack",
      output / "explorer" / "foo" / "%" / "schema-html.metapack",
      output / "explorer" / "%" / "search.metapack",
      output / "explorer" / "%" / "directory.metapack",
      output / "explorer" / "%" / "directory-html.metapack",
      output / "explorer" / "%" / "404.metapack", output / "routes.bin");
}

TEST(Build_delta, full_with_comment) {
  const std::filesystem::path output{"/output"};
  const sourcemeta::one::BuildState entries;
  const sourcemeta::one::Resolver::Views schemas;
  const std::vector<std::filesystem::path> changed;
  const std::vector<std::filesystem::path> removed;
  const auto plan{sourcemeta::one::delta(
      sourcemeta::one::BuildPhase::Produce,
      sourcemeta::one::BuildPlan::Type::Full, entries, output, schemas, "1.0.0",
      false, "Hello world", changed, removed)};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, Full, 4, 8);

  EXPECT_ACTION(plan, 0, 0, 3, Comment, output / "comment.json", "Hello world");
  EXPECT_ACTION(plan, 0, 1, 3, Configuration, output / "configuration.json",
                "");
  EXPECT_ACTION(plan, 0, 2, 3, Version, output / "version.json", "1.0.0");

  EXPECT_ACTION(plan, 1, 0, 3, WebNotFound,
                output / "explorer" / "%" / "404.metapack", "",
                output / "configuration.json");
  EXPECT_ACTION(plan, 1, 1, 3, DirectoryList,
                output / "explorer" / "%" / "directory.metapack", "");
  EXPECT_ACTION(plan, 1, 2, 3, SearchIndex,
                output / "explorer" / "%" / "search.metapack", "");

  EXPECT_ACTION(plan, 2, 0, 1, WebIndex,
                output / "explorer" / "%" / "directory-html.metapack", "",
                output / "explorer" / "%" / "directory.metapack");

  EXPECT_ACTION(plan, 3, 0, 1, Routes, output / "routes.bin", "Full",
                output / "configuration.json");

  EXPECT_TOTAL_FILES(plan, entries, output / "comment.json",
                     output / "configuration.json", output / "version.json",
                     output / "explorer" / "%" / "search.metapack",
                     output / "explorer" / "%" / "directory.metapack",
                     output / "explorer" / "%" / "404.metapack",
                     output / "explorer" / "%" / "directory-html.metapack",
                     output / "routes.bin");
}

TEST(Build_delta, full_without_comment_removes_existing) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  entries.emplace(output / "comment.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  const sourcemeta::one::Resolver::Views schemas;
  const std::vector<std::filesystem::path> changed;
  const std::vector<std::filesystem::path> removed;
  const auto plan{sourcemeta::one::delta(sourcemeta::one::BuildPhase::Produce,
                                         sourcemeta::one::BuildPlan::Type::Full,
                                         entries, output, schemas, "1.0.0",
                                         false, "", changed, removed)};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, Full, 5, 8);

  EXPECT_ACTION(plan, 0, 0, 2, Configuration, output / "configuration.json",
                "");
  EXPECT_ACTION(plan, 0, 1, 2, Version, output / "version.json", "1.0.0");

  EXPECT_ACTION(plan, 1, 0, 3, WebNotFound,
                output / "explorer" / "%" / "404.metapack", "",
                output / "configuration.json");
  EXPECT_ACTION(plan, 1, 1, 3, DirectoryList,
                output / "explorer" / "%" / "directory.metapack", "");
  EXPECT_ACTION(plan, 1, 2, 3, SearchIndex,
                output / "explorer" / "%" / "search.metapack", "");

  EXPECT_ACTION(plan, 2, 0, 1, WebIndex,
                output / "explorer" / "%" / "directory-html.metapack", "",
                output / "explorer" / "%" / "directory.metapack");

  EXPECT_ACTION(plan, 3, 0, 1, Routes, output / "routes.bin", "Full",
                output / "configuration.json");

  EXPECT_ACTION(plan, 4, 0, 1, Remove, output / "comment.json", "");

  EXPECT_TOTAL_FILES(plan, entries, output / "configuration.json",
                     output / "version.json",
                     output / "explorer" / "%" / "search.metapack",
                     output / "explorer" / "%" / "directory.metapack",
                     output / "explorer" / "%" / "404.metapack",
                     output / "explorer" / "%" / "directory-html.metapack",
                     output / "routes.bin");
}

TEST(Build_delta, incremental_with_comment) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  entries.emplace(output / "version.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "configuration.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  const sourcemeta::one::Resolver::Views schemas{
      {"https://example.com/foo", {"/src/foo.json", "foo", MTIME(100)}}};
  const std::vector<std::filesystem::path> changed{"/src/foo.json"};
  const std::vector<std::filesystem::path> removed;
  const auto plan{sourcemeta::one::delta(
      sourcemeta::one::BuildPhase::Produce,
      sourcemeta::one::BuildPlan::Type::Full, entries, output, schemas, "1.0.0",
      true, "Hello world", changed, removed)};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, Full, 8, 17);

  EXPECT_ACTION(plan, 0, 0, 1, Comment, output / "comment.json", "Hello world");

  EXPECT_ACTION(plan, 1, 0, 1, Materialise,
                output / "schemas" / "foo" / "%" / "schema.metapack",
                "https://example.com/foo",
                std::filesystem::path{"/"} / "src" / "foo.json",
                output / "configuration.json");

  EXPECT_ACTION(plan, 2, 0, 4, Dependencies,
                output / "schemas" / "foo" / "%" / "dependencies.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 2, 1, 4, Locations,
                output / "schemas" / "foo" / "%" / "locations.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 2, 2, 4, Positions,
                output / "schemas" / "foo" / "%" / "positions.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 2, 3, 4, Stats,
                output / "schemas" / "foo" / "%" / "stats.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");

  EXPECT_ACTION(plan, 3, 0, 2, Bundle,
                output / "schemas" / "foo" / "%" / "bundle.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "dependencies.metapack");
  EXPECT_ACTION(plan, 3, 1, 2, Health,
                output / "schemas" / "foo" / "%" / "health.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "dependencies.metapack");

  EXPECT_ACTION(plan, 4, 0, 4, SchemaMetadata,
                output / "explorer" / "foo" / "%" / "schema.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "health.metapack",
                output / "schemas" / "foo" / "%" / "dependencies.metapack");
  EXPECT_ACTION(plan, 4, 1, 4, BlazeExhaustive,
                output / "schemas" / "foo" / "%" / "blaze-exhaustive.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "bundle.metapack");
  EXPECT_ACTION(plan, 4, 2, 4, BlazeFast,
                output / "schemas" / "foo" / "%" / "blaze-fast.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "bundle.metapack");
  EXPECT_ACTION(plan, 4, 3, 4, Editor,
                output / "schemas" / "foo" / "%" / "editor.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "bundle.metapack");

  EXPECT_ACTION(plan, 5, 0, 3, DirectoryList,
                output / "explorer" / "%" / "directory.metapack", "",
                output / "explorer" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 5, 1, 3, SearchIndex,
                output / "explorer" / "%" / "search.metapack", "",
                output / "explorer" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 5, 2, 3, WebSchema,
                output / "explorer" / "foo" / "%" / "schema-html.metapack",
                "https://example.com/foo",
                output / "explorer" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "health.metapack");

  EXPECT_ACTION(plan, 6, 0, 1, WebIndex,
                output / "explorer" / "%" / "directory-html.metapack", "",
                output / "explorer" / "%" / "directory.metapack");

  EXPECT_ACTION(plan, 7, 0, 1, Routes, output / "routes.bin", "Full",
                output / "configuration.json");

  EXPECT_TOTAL_FILES(plan, entries, output / "version.json",
                     output / "configuration.json", output / "comment.json",
                     output / "schemas" / "foo" / "%" / "schema.metapack",
                     output / "schemas" / "foo" / "%" / "dependencies.metapack",
                     output / "schemas" / "foo" / "%" / "locations.metapack",
                     output / "schemas" / "foo" / "%" / "positions.metapack",
                     output / "schemas" / "foo" / "%" / "stats.metapack",
                     output / "schemas" / "foo" / "%" / "bundle.metapack",
                     output / "schemas" / "foo" / "%" / "health.metapack",
                     output / "schemas" / "foo" / "%" /
                         "blaze-exhaustive.metapack",
                     output / "schemas" / "foo" / "%" / "blaze-fast.metapack",
                     output / "schemas" / "foo" / "%" / "editor.metapack",
                     output / "explorer" / "foo" / "%" / "schema.metapack",
                     output / "explorer" / "foo" / "%" / "schema-html.metapack",
                     output / "explorer" / "%" / "search.metapack",
                     output / "explorer" / "%" / "directory.metapack",
                     output / "explorer" / "%" / "directory-html.metapack",
                     output / "routes.bin");
}

TEST(Build_delta, incremental_empty_comment_removes_existing) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  entries.emplace(output / "version.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "comment.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "configuration.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  const sourcemeta::one::Resolver::Views schemas{
      {"https://example.com/foo", {"/src/foo.json", "foo", MTIME(100)}}};
  const std::vector<std::filesystem::path> changed{"/src/foo.json"};
  const std::vector<std::filesystem::path> removed;
  const auto plan{sourcemeta::one::delta(sourcemeta::one::BuildPhase::Produce,
                                         sourcemeta::one::BuildPlan::Type::Full,
                                         entries, output, schemas, "1.0.0",
                                         true, "", changed, removed)};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, Full, 8, 17);

  EXPECT_ACTION(plan, 0, 0, 1, Materialise,
                output / "schemas" / "foo" / "%" / "schema.metapack",
                "https://example.com/foo",
                std::filesystem::path{"/"} / "src" / "foo.json",
                output / "configuration.json");

  EXPECT_ACTION(plan, 1, 0, 4, Dependencies,
                output / "schemas" / "foo" / "%" / "dependencies.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 1, 1, 4, Locations,
                output / "schemas" / "foo" / "%" / "locations.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 1, 2, 4, Positions,
                output / "schemas" / "foo" / "%" / "positions.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 1, 3, 4, Stats,
                output / "schemas" / "foo" / "%" / "stats.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");

  EXPECT_ACTION(plan, 2, 0, 2, Bundle,
                output / "schemas" / "foo" / "%" / "bundle.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "dependencies.metapack");
  EXPECT_ACTION(plan, 2, 1, 2, Health,
                output / "schemas" / "foo" / "%" / "health.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "dependencies.metapack");

  EXPECT_ACTION(plan, 3, 0, 4, SchemaMetadata,
                output / "explorer" / "foo" / "%" / "schema.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "health.metapack",
                output / "schemas" / "foo" / "%" / "dependencies.metapack");
  EXPECT_ACTION(plan, 3, 1, 4, BlazeExhaustive,
                output / "schemas" / "foo" / "%" / "blaze-exhaustive.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "bundle.metapack");
  EXPECT_ACTION(plan, 3, 2, 4, BlazeFast,
                output / "schemas" / "foo" / "%" / "blaze-fast.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "bundle.metapack");
  EXPECT_ACTION(plan, 3, 3, 4, Editor,
                output / "schemas" / "foo" / "%" / "editor.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "bundle.metapack");

  EXPECT_ACTION(plan, 4, 0, 3, DirectoryList,
                output / "explorer" / "%" / "directory.metapack", "",
                output / "explorer" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 4, 1, 3, SearchIndex,
                output / "explorer" / "%" / "search.metapack", "",
                output / "explorer" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 4, 2, 3, WebSchema,
                output / "explorer" / "foo" / "%" / "schema-html.metapack",
                "https://example.com/foo",
                output / "explorer" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "health.metapack");

  EXPECT_ACTION(plan, 5, 0, 1, WebIndex,
                output / "explorer" / "%" / "directory-html.metapack", "",
                output / "explorer" / "%" / "directory.metapack");

  EXPECT_ACTION(plan, 6, 0, 1, Routes, output / "routes.bin", "Full",
                output / "configuration.json");

  EXPECT_ACTION(plan, 7, 0, 1, Remove, output / "comment.json", "");

  EXPECT_TOTAL_FILES(
      plan, entries, output / "version.json", output / "configuration.json",
      output / "schemas" / "foo" / "%" / "schema.metapack",
      output / "schemas" / "foo" / "%" / "dependencies.metapack",
      output / "schemas" / "foo" / "%" / "locations.metapack",
      output / "schemas" / "foo" / "%" / "positions.metapack",
      output / "schemas" / "foo" / "%" / "stats.metapack",
      output / "schemas" / "foo" / "%" / "bundle.metapack",
      output / "schemas" / "foo" / "%" / "health.metapack",
      output / "schemas" / "foo" / "%" / "blaze-exhaustive.metapack",
      output / "schemas" / "foo" / "%" / "blaze-fast.metapack",
      output / "schemas" / "foo" / "%" / "editor.metapack",
      output / "explorer" / "foo" / "%" / "schema.metapack",
      output / "explorer" / "foo" / "%" / "schema-html.metapack",
      output / "explorer" / "%" / "search.metapack",
      output / "explorer" / "%" / "directory.metapack",
      output / "explorer" / "%" / "directory-html.metapack",
      output / "routes.bin");
}

TEST(Build_delta, incremental_no_changes_adds_comment) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  entries.emplace(output / "version.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "configuration.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "routes.bin",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "search.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "directory.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "404.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "directory-html.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  const sourcemeta::one::Resolver::Views schemas;
  const std::vector<std::filesystem::path> changed;
  const std::vector<std::filesystem::path> removed;
  const auto plan{sourcemeta::one::delta(sourcemeta::one::BuildPhase::Produce,
                                         sourcemeta::one::BuildPlan::Type::Full,
                                         entries, output, schemas, "1.0.0",
                                         true, "hello", changed, removed)};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, Full, 1, 1);

  EXPECT_ACTION(plan, 0, 0, 1, Comment, output / "comment.json", "hello");

  EXPECT_TOTAL_FILES(plan, entries, output / "version.json",
                     output / "configuration.json", output / "comment.json",
                     output / "routes.bin",
                     output / "explorer" / "%" / "search.metapack",
                     output / "explorer" / "%" / "directory.metapack",
                     output / "explorer" / "%" / "404.metapack",
                     output / "explorer" / "%" / "directory-html.metapack");
}

TEST(Build_delta, incremental_no_changes_removes_comment) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  entries.emplace(output / "version.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "configuration.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "comment.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "routes.bin",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "search.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "directory.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "404.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "directory-html.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  const sourcemeta::one::Resolver::Views schemas;
  const std::vector<std::filesystem::path> changed;
  const std::vector<std::filesystem::path> removed;
  const auto plan{sourcemeta::one::delta(sourcemeta::one::BuildPhase::Produce,
                                         sourcemeta::one::BuildPlan::Type::Full,
                                         entries, output, schemas, "1.0.0",
                                         true, "", changed, removed)};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, Full, 1, 1);

  EXPECT_ACTION(plan, 0, 0, 1, Remove, output / "comment.json", "");

  EXPECT_TOTAL_FILES(plan, entries, output / "version.json",
                     output / "configuration.json", output / "routes.bin",
                     output / "explorer" / "%" / "search.metapack",
                     output / "explorer" / "%" / "directory.metapack",
                     output / "explorer" / "%" / "404.metapack",
                     output / "explorer" / "%" / "directory-html.metapack");
}

TEST(Build_delta, incremental_schema_removed_cleans_stale_entries) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  entries.emplace(output / "version.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "configuration.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "routes.bin",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "search.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "directory.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "404.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "directory-html.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  ADD_SCHEMA_ENTRIES(entries, output, "foo", true, true, MTIME(100));
  const sourcemeta::one::Resolver::Views schemas;
  const std::vector<std::filesystem::path> changed;
  const std::vector<std::filesystem::path> removed;
  const auto plan{sourcemeta::one::delta(sourcemeta::one::BuildPhase::Produce,
                                         sourcemeta::one::BuildPlan::Type::Full,
                                         entries, output, schemas, "1.0.0",
                                         true, "", changed, removed)};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, Full, 3, 5);

  EXPECT_ACTION(plan, 0, 0, 2, DirectoryList,
                output / "explorer" / "%" / "directory.metapack", "");
  EXPECT_ACTION(plan, 0, 1, 2, SearchIndex,
                output / "explorer" / "%" / "search.metapack", "");

  EXPECT_ACTION(plan, 1, 0, 1, WebIndex,
                output / "explorer" / "%" / "directory-html.metapack", "",
                output / "explorer" / "%" / "directory.metapack");

  EXPECT_ACTION(plan, 2, 0, 2, Remove, output / "explorer" / "foo", "");
  EXPECT_ACTION(plan, 2, 1, 2, Remove, output / "schemas", "");

  EXPECT_TOTAL_FILES(plan, entries, output / "version.json",
                     output / "configuration.json", output / "routes.bin",
                     output / "explorer" / "%" / "search.metapack",
                     output / "explorer" / "%" / "directory.metapack",
                     output / "explorer" / "%" / "404.metapack",
                     output / "explorer" / "%" / "directory-html.metapack");
}

TEST(Build_delta, remove_wave_deduplicates_children_of_removed_directories) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  entries.emplace(output / "version.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "configuration.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "routes.bin",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "search.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "directory.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "404.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "directory-html.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  ADD_SCHEMA_ENTRIES(entries, output, "foo", true, true, MTIME(100));
  const sourcemeta::one::Resolver::Views schemas;
  const std::vector<std::filesystem::path> changed;
  const std::vector<std::filesystem::path> removed;
  const auto plan{sourcemeta::one::delta(sourcemeta::one::BuildPhase::Produce,
                                         sourcemeta::one::BuildPlan::Type::Full,
                                         entries, output, schemas, "1.0.0",
                                         true, "", changed, removed)};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, Full, 3, 5);

  EXPECT_ACTION(plan, 0, 0, 2, DirectoryList,
                output / "explorer" / "%" / "directory.metapack", "");
  EXPECT_ACTION(plan, 0, 1, 2, SearchIndex,
                output / "explorer" / "%" / "search.metapack", "");

  EXPECT_ACTION(plan, 1, 0, 1, WebIndex,
                output / "explorer" / "%" / "directory-html.metapack", "",
                output / "explorer" / "%" / "directory.metapack");

  EXPECT_ACTION(plan, 2, 0, 2, Remove, output / "explorer" / "foo", "");
  EXPECT_ACTION(plan, 2, 1, 2, Remove, output / "schemas", "");

  EXPECT_TOTAL_FILES(plan, entries, output / "version.json",
                     output / "configuration.json", output / "routes.bin",
                     output / "explorer" / "%" / "search.metapack",
                     output / "explorer" / "%" / "directory.metapack",
                     output / "explorer" / "%" / "404.metapack",
                     output / "explorer" / "%" / "directory-html.metapack");
}

TEST(Build_delta, full_config_change_to_empty_schemas) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  entries.emplace(output / "version.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "configuration.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "routes.bin",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "search.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "directory.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "404.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "directory-html.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  ADD_SCHEMA_ENTRIES(entries, output, "foo", true, true, MTIME(100));
  const sourcemeta::one::Resolver::Views schemas;
  const std::vector<std::filesystem::path> changed;
  const std::vector<std::filesystem::path> removed;
  const auto plan{sourcemeta::one::delta(sourcemeta::one::BuildPhase::Produce,
                                         sourcemeta::one::BuildPlan::Type::Full,
                                         entries, output, schemas, "1.0.0",
                                         false, "", changed, removed)};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, Full, 5, 9);

  EXPECT_ACTION(plan, 0, 0, 2, Configuration, output / "configuration.json",
                "");
  EXPECT_ACTION(plan, 0, 1, 2, Version, output / "version.json", "1.0.0");

  EXPECT_ACTION(plan, 1, 0, 3, WebNotFound,
                output / "explorer" / "%" / "404.metapack", "",
                output / "configuration.json");
  EXPECT_ACTION(plan, 1, 1, 3, DirectoryList,
                output / "explorer" / "%" / "directory.metapack", "");
  EXPECT_ACTION(plan, 1, 2, 3, SearchIndex,
                output / "explorer" / "%" / "search.metapack", "");

  EXPECT_ACTION(plan, 2, 0, 1, WebIndex,
                output / "explorer" / "%" / "directory-html.metapack", "",
                output / "explorer" / "%" / "directory.metapack");

  EXPECT_ACTION(plan, 3, 0, 1, Routes, output / "routes.bin", "Full",
                output / "configuration.json");

  EXPECT_ACTION(plan, 4, 0, 2, Remove, output / "explorer" / "foo", "");
  EXPECT_ACTION(plan, 4, 1, 2, Remove, output / "schemas", "");

  EXPECT_TOTAL_FILES(plan, entries, output / "version.json",
                     output / "configuration.json", output / "routes.bin",
                     output / "explorer" / "%" / "search.metapack",
                     output / "explorer" / "%" / "directory.metapack",
                     output / "explorer" / "%" / "404.metapack",
                     output / "explorer" / "%" / "directory-html.metapack");
}

TEST(Build_delta, full_single_schema_evaluate_false) {
  const std::filesystem::path output{"/output"};
  const sourcemeta::one::BuildState entries;
  const sourcemeta::one::Resolver::Views schemas{
      {"https://example.com/foo", {"/src/foo.json", "foo", MTIME(100), false}}};
  const std::vector<std::filesystem::path> changed;
  const std::vector<std::filesystem::path> removed;
  const auto plan{sourcemeta::one::delta(sourcemeta::one::BuildPhase::Produce,
                                         sourcemeta::one::BuildPlan::Type::Full,
                                         entries, output, schemas, "1.0.0",
                                         false, "", changed, removed)};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, Full, 8, 17);

  EXPECT_ACTION(plan, 0, 0, 2, Configuration, output / "configuration.json",
                "");
  EXPECT_ACTION(plan, 0, 1, 2, Version, output / "version.json", "1.0.0");

  EXPECT_ACTION(plan, 1, 0, 2, WebNotFound,
                output / "explorer" / "%" / "404.metapack", "",
                output / "configuration.json");
  EXPECT_ACTION(plan, 1, 1, 2, Materialise,
                output / "schemas" / "foo" / "%" / "schema.metapack",
                "https://example.com/foo",
                std::filesystem::path{"/"} / "src" / "foo.json",
                output / "configuration.json");

  EXPECT_ACTION(plan, 2, 0, 4, Dependencies,
                output / "schemas" / "foo" / "%" / "dependencies.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 2, 1, 4, Locations,
                output / "schemas" / "foo" / "%" / "locations.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 2, 2, 4, Positions,
                output / "schemas" / "foo" / "%" / "positions.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 2, 3, 4, Stats,
                output / "schemas" / "foo" / "%" / "stats.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");

  EXPECT_ACTION(plan, 3, 0, 2, Bundle,
                output / "schemas" / "foo" / "%" / "bundle.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "dependencies.metapack");
  EXPECT_ACTION(plan, 3, 1, 2, Health,
                output / "schemas" / "foo" / "%" / "health.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "dependencies.metapack");

  EXPECT_ACTION(plan, 4, 0, 2, SchemaMetadata,
                output / "explorer" / "foo" / "%" / "schema.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "health.metapack",
                output / "schemas" / "foo" / "%" / "dependencies.metapack");
  EXPECT_ACTION(plan, 4, 1, 2, Editor,
                output / "schemas" / "foo" / "%" / "editor.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "bundle.metapack");

  EXPECT_ACTION(plan, 5, 0, 3, DirectoryList,
                output / "explorer" / "%" / "directory.metapack", "",
                output / "explorer" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 5, 1, 3, SearchIndex,
                output / "explorer" / "%" / "search.metapack", "",
                output / "explorer" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 5, 2, 3, WebSchema,
                output / "explorer" / "foo" / "%" / "schema-html.metapack",
                "https://example.com/foo",
                output / "explorer" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "health.metapack");

  EXPECT_ACTION(plan, 6, 0, 1, WebIndex,
                output / "explorer" / "%" / "directory-html.metapack", "",
                output / "explorer" / "%" / "directory.metapack");

  EXPECT_ACTION(plan, 7, 0, 1, Routes, output / "routes.bin", "Full",
                output / "configuration.json");

  EXPECT_TOTAL_FILES(
      plan, entries, output / "configuration.json", output / "version.json",
      output / "schemas" / "foo" / "%" / "schema.metapack",
      output / "schemas" / "foo" / "%" / "dependencies.metapack",
      output / "schemas" / "foo" / "%" / "locations.metapack",
      output / "schemas" / "foo" / "%" / "positions.metapack",
      output / "schemas" / "foo" / "%" / "stats.metapack",
      output / "schemas" / "foo" / "%" / "bundle.metapack",
      output / "schemas" / "foo" / "%" / "health.metapack",
      output / "schemas" / "foo" / "%" / "editor.metapack",
      output / "explorer" / "foo" / "%" / "schema.metapack",
      output / "explorer" / "foo" / "%" / "schema-html.metapack",
      output / "explorer" / "%" / "search.metapack",
      output / "explorer" / "%" / "directory.metapack",
      output / "explorer" / "%" / "directory-html.metapack",
      output / "explorer" / "%" / "404.metapack", output / "routes.bin");
}

TEST(Build_delta, full_evaluate_false_removes_existing_blaze) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  entries.emplace(output / "schemas" / "foo" / "%" /
                      "blaze-exhaustive.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "schemas" / "foo" / "%" / "blaze-fast.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  const sourcemeta::one::Resolver::Views schemas{
      {"https://example.com/foo", {"/src/foo.json", "foo", MTIME(100), false}}};
  const std::vector<std::filesystem::path> changed;
  const std::vector<std::filesystem::path> removed;
  const auto plan{sourcemeta::one::delta(sourcemeta::one::BuildPhase::Produce,
                                         sourcemeta::one::BuildPlan::Type::Full,
                                         entries, output, schemas, "1.0.0",
                                         false, "", changed, removed)};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, Full, 9, 19);

  EXPECT_ACTION(plan, 0, 0, 2, Configuration, output / "configuration.json",
                "");
  EXPECT_ACTION(plan, 0, 1, 2, Version, output / "version.json", "1.0.0");

  EXPECT_ACTION(plan, 1, 0, 2, WebNotFound,
                output / "explorer" / "%" / "404.metapack", "",
                output / "configuration.json");
  EXPECT_ACTION(plan, 1, 1, 2, Materialise,
                output / "schemas" / "foo" / "%" / "schema.metapack",
                "https://example.com/foo",
                std::filesystem::path{"/"} / "src" / "foo.json",
                output / "configuration.json");

  EXPECT_ACTION(plan, 2, 0, 4, Dependencies,
                output / "schemas" / "foo" / "%" / "dependencies.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 2, 1, 4, Locations,
                output / "schemas" / "foo" / "%" / "locations.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 2, 2, 4, Positions,
                output / "schemas" / "foo" / "%" / "positions.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 2, 3, 4, Stats,
                output / "schemas" / "foo" / "%" / "stats.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");

  EXPECT_ACTION(plan, 3, 0, 2, Bundle,
                output / "schemas" / "foo" / "%" / "bundle.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "dependencies.metapack");
  EXPECT_ACTION(plan, 3, 1, 2, Health,
                output / "schemas" / "foo" / "%" / "health.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "dependencies.metapack");

  EXPECT_ACTION(plan, 4, 0, 2, SchemaMetadata,
                output / "explorer" / "foo" / "%" / "schema.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "health.metapack",
                output / "schemas" / "foo" / "%" / "dependencies.metapack");
  EXPECT_ACTION(plan, 4, 1, 2, Editor,
                output / "schemas" / "foo" / "%" / "editor.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "bundle.metapack");

  EXPECT_ACTION(plan, 5, 0, 3, DirectoryList,
                output / "explorer" / "%" / "directory.metapack", "",
                output / "explorer" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 5, 1, 3, SearchIndex,
                output / "explorer" / "%" / "search.metapack", "",
                output / "explorer" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 5, 2, 3, WebSchema,
                output / "explorer" / "foo" / "%" / "schema-html.metapack",
                "https://example.com/foo",
                output / "explorer" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "health.metapack");

  EXPECT_ACTION(plan, 6, 0, 1, WebIndex,
                output / "explorer" / "%" / "directory-html.metapack", "",
                output / "explorer" / "%" / "directory.metapack");

  EXPECT_ACTION(plan, 7, 0, 1, Routes, output / "routes.bin", "Full",
                output / "configuration.json");

  EXPECT_ACTION(plan, 8, 0, 2, Remove,
                output / "schemas" / "foo" / "%" / "blaze-exhaustive.metapack",
                "");
  EXPECT_ACTION(plan, 8, 1, 2, Remove,
                output / "schemas" / "foo" / "%" / "blaze-fast.metapack", "");

  EXPECT_TOTAL_FILES(
      plan, entries, output / "configuration.json", output / "version.json",
      output / "schemas" / "foo" / "%" / "schema.metapack",
      output / "schemas" / "foo" / "%" / "dependencies.metapack",
      output / "schemas" / "foo" / "%" / "locations.metapack",
      output / "schemas" / "foo" / "%" / "positions.metapack",
      output / "schemas" / "foo" / "%" / "stats.metapack",
      output / "schemas" / "foo" / "%" / "bundle.metapack",
      output / "schemas" / "foo" / "%" / "health.metapack",
      output / "schemas" / "foo" / "%" / "editor.metapack",
      output / "explorer" / "foo" / "%" / "schema.metapack",
      output / "explorer" / "foo" / "%" / "schema-html.metapack",
      output / "explorer" / "%" / "search.metapack",
      output / "explorer" / "%" / "directory.metapack",
      output / "explorer" / "%" / "directory-html.metapack",
      output / "explorer" / "%" / "404.metapack", output / "routes.bin");
}

TEST(Build_delta, incremental_evaluate_false) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  entries.emplace(output / "version.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "configuration.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  const sourcemeta::one::Resolver::Views schemas{
      {"https://example.com/foo", {"/src/foo.json", "foo", MTIME(100), false}}};
  const std::vector<std::filesystem::path> changed{"/src/foo.json"};
  const std::vector<std::filesystem::path> removed;
  const auto plan{sourcemeta::one::delta(sourcemeta::one::BuildPhase::Produce,
                                         sourcemeta::one::BuildPlan::Type::Full,
                                         entries, output, schemas, "1.0.0",
                                         true, "", changed, removed)};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, Full, 7, 14);

  EXPECT_ACTION(plan, 0, 0, 1, Materialise,
                output / "schemas" / "foo" / "%" / "schema.metapack",
                "https://example.com/foo",
                std::filesystem::path{"/"} / "src" / "foo.json",
                output / "configuration.json");

  EXPECT_ACTION(plan, 1, 0, 4, Dependencies,
                output / "schemas" / "foo" / "%" / "dependencies.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 1, 1, 4, Locations,
                output / "schemas" / "foo" / "%" / "locations.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 1, 2, 4, Positions,
                output / "schemas" / "foo" / "%" / "positions.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 1, 3, 4, Stats,
                output / "schemas" / "foo" / "%" / "stats.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");

  EXPECT_ACTION(plan, 2, 0, 2, Bundle,
                output / "schemas" / "foo" / "%" / "bundle.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "dependencies.metapack");
  EXPECT_ACTION(plan, 2, 1, 2, Health,
                output / "schemas" / "foo" / "%" / "health.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "dependencies.metapack");

  EXPECT_ACTION(plan, 3, 0, 2, SchemaMetadata,
                output / "explorer" / "foo" / "%" / "schema.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "health.metapack",
                output / "schemas" / "foo" / "%" / "dependencies.metapack");
  EXPECT_ACTION(plan, 3, 1, 2, Editor,
                output / "schemas" / "foo" / "%" / "editor.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "bundle.metapack");

  EXPECT_ACTION(plan, 4, 0, 3, DirectoryList,
                output / "explorer" / "%" / "directory.metapack", "",
                output / "explorer" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 4, 1, 3, SearchIndex,
                output / "explorer" / "%" / "search.metapack", "",
                output / "explorer" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 4, 2, 3, WebSchema,
                output / "explorer" / "foo" / "%" / "schema-html.metapack",
                "https://example.com/foo",
                output / "explorer" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "health.metapack");

  EXPECT_ACTION(plan, 5, 0, 1, WebIndex,
                output / "explorer" / "%" / "directory-html.metapack", "",
                output / "explorer" / "%" / "directory.metapack");

  EXPECT_ACTION(plan, 6, 0, 1, Routes, output / "routes.bin", "Full",
                output / "configuration.json");

  EXPECT_TOTAL_FILES(plan, entries, output / "version.json",
                     output / "configuration.json",
                     output / "schemas" / "foo" / "%" / "schema.metapack",
                     output / "schemas" / "foo" / "%" / "dependencies.metapack",
                     output / "schemas" / "foo" / "%" / "locations.metapack",
                     output / "schemas" / "foo" / "%" / "positions.metapack",
                     output / "schemas" / "foo" / "%" / "stats.metapack",
                     output / "schemas" / "foo" / "%" / "bundle.metapack",
                     output / "schemas" / "foo" / "%" / "health.metapack",
                     output / "schemas" / "foo" / "%" / "editor.metapack",
                     output / "explorer" / "foo" / "%" / "schema.metapack",
                     output / "explorer" / "foo" / "%" / "schema-html.metapack",
                     output / "explorer" / "%" / "search.metapack",
                     output / "explorer" / "%" / "directory.metapack",
                     output / "explorer" / "%" / "directory-html.metapack",
                     output / "routes.bin");
}

TEST(Build_delta, incremental_missing_blaze_exhaustive) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  entries.emplace(output / "version.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "configuration.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  ADD_SCHEMA_ENTRIES(entries, output, "foo", true, true, MTIME(100));
  entries.forget(
      (output / "schemas" / "foo" / "%" / "blaze-exhaustive.metapack")
          .string());
  entries.emplace(output / "explorer" / "%" / "search.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "foo" / "%" / "directory.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "directory.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "directory-html.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "404.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "routes.bin",
                  {.file_mark = MTIME(100), .dependencies = {}});
  const sourcemeta::one::Resolver::Views schemas{
      {"https://example.com/foo", {"/src/foo.json", "foo", MTIME(40)}}};
  const std::vector<std::filesystem::path> changed;
  const std::vector<std::filesystem::path> removed;
  const auto plan{sourcemeta::one::delta(sourcemeta::one::BuildPhase::Produce,
                                         sourcemeta::one::BuildPlan::Type::Full,
                                         entries, output, schemas, "1.0.0",
                                         true, "", changed, removed)};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, Full, 2, 4);

  EXPECT_ACTION(plan, 0, 0, 3, DirectoryList,
                output / "explorer" / "%" / "directory.metapack", "",
                output / "explorer" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 0, 1, 3, SearchIndex,
                output / "explorer" / "%" / "search.metapack", "",
                output / "explorer" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 0, 2, 3, BlazeExhaustive,
                output / "schemas" / "foo" / "%" / "blaze-exhaustive.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "bundle.metapack");

  EXPECT_ACTION(plan, 1, 0, 1, WebIndex,
                output / "explorer" / "%" / "directory-html.metapack", "",
                output / "explorer" / "%" / "directory.metapack");

  EXPECT_TOTAL_FILES(
      plan, entries, output / "version.json", output / "configuration.json",
      output / "schemas" / "foo" / "%" / "schema.metapack",
      output / "schemas" / "foo" / "%" / "dependencies.metapack",
      output / "schemas" / "foo" / "%" / "locations.metapack",
      output / "schemas" / "foo" / "%" / "positions.metapack",
      output / "schemas" / "foo" / "%" / "stats.metapack",
      output / "schemas" / "foo" / "%" / "bundle.metapack",
      output / "schemas" / "foo" / "%" / "health.metapack",
      output / "schemas" / "foo" / "%" / "blaze-exhaustive.metapack",
      output / "schemas" / "foo" / "%" / "blaze-fast.metapack",
      output / "schemas" / "foo" / "%" / "editor.metapack",
      output / "explorer" / "foo" / "%" / "schema.metapack",
      output / "explorer" / "foo" / "%" / "schema-html.metapack",
      output / "explorer" / "foo" / "%" / "directory.metapack",
      output / "explorer" / "foo" / "%" / "directory-html.metapack",
      output / "explorer" / "%" / "search.metapack",
      output / "explorer" / "%" / "directory.metapack",
      output / "explorer" / "%" / "directory-html.metapack",
      output / "explorer" / "%" / "404.metapack", output / "routes.bin");
}

TEST(Build_delta, incremental_missing_bundle) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  entries.emplace(output / "version.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "configuration.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  ADD_SCHEMA_ENTRIES(entries, output, "foo", true, true, MTIME(100));
  entries.forget(
      (output / "schemas" / "foo" / "%" / "bundle.metapack").string());
  entries.emplace(output / "explorer" / "%" / "search.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "foo" / "%" / "directory.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "directory.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "directory-html.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "404.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "routes.bin",
                  {.file_mark = MTIME(100), .dependencies = {}});
  const sourcemeta::one::Resolver::Views schemas{
      {"https://example.com/foo", {"/src/foo.json", "foo", MTIME(40)}}};
  const std::vector<std::filesystem::path> changed;
  const std::vector<std::filesystem::path> removed;
  const auto plan{sourcemeta::one::delta(sourcemeta::one::BuildPhase::Produce,
                                         sourcemeta::one::BuildPlan::Type::Full,
                                         entries, output, schemas, "1.0.0",
                                         true, "", changed, removed)};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, Full, 2, 7);

  EXPECT_ACTION(plan, 0, 0, 3, DirectoryList,
                output / "explorer" / "%" / "directory.metapack", "",
                output / "explorer" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 0, 1, 3, SearchIndex,
                output / "explorer" / "%" / "search.metapack", "",
                output / "explorer" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 0, 2, 3, Bundle,
                output / "schemas" / "foo" / "%" / "bundle.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "dependencies.metapack");

  EXPECT_ACTION(plan, 1, 0, 4, WebIndex,
                output / "explorer" / "%" / "directory-html.metapack", "",
                output / "explorer" / "%" / "directory.metapack");
  EXPECT_ACTION(plan, 1, 1, 4, BlazeExhaustive,
                output / "schemas" / "foo" / "%" / "blaze-exhaustive.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "bundle.metapack");
  EXPECT_ACTION(plan, 1, 2, 4, BlazeFast,
                output / "schemas" / "foo" / "%" / "blaze-fast.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "bundle.metapack");
  EXPECT_ACTION(plan, 1, 3, 4, Editor,
                output / "schemas" / "foo" / "%" / "editor.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "bundle.metapack");

  EXPECT_TOTAL_FILES(
      plan, entries, output / "version.json", output / "configuration.json",
      output / "schemas" / "foo" / "%" / "schema.metapack",
      output / "schemas" / "foo" / "%" / "dependencies.metapack",
      output / "schemas" / "foo" / "%" / "locations.metapack",
      output / "schemas" / "foo" / "%" / "positions.metapack",
      output / "schemas" / "foo" / "%" / "stats.metapack",
      output / "schemas" / "foo" / "%" / "bundle.metapack",
      output / "schemas" / "foo" / "%" / "health.metapack",
      output / "schemas" / "foo" / "%" / "blaze-exhaustive.metapack",
      output / "schemas" / "foo" / "%" / "blaze-fast.metapack",
      output / "schemas" / "foo" / "%" / "editor.metapack",
      output / "explorer" / "foo" / "%" / "schema.metapack",
      output / "explorer" / "foo" / "%" / "schema-html.metapack",
      output / "explorer" / "foo" / "%" / "directory.metapack",
      output / "explorer" / "foo" / "%" / "directory-html.metapack",
      output / "explorer" / "%" / "search.metapack",
      output / "explorer" / "%" / "directory.metapack",
      output / "explorer" / "%" / "directory-html.metapack",
      output / "explorer" / "%" / "404.metapack", output / "routes.bin");
}

TEST(Build_delta, incremental_missing_web_schema) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  entries.emplace(output / "version.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "configuration.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  ADD_SCHEMA_ENTRIES(entries, output, "foo", true, true, MTIME(100));
  entries.forget(
      (output / "explorer" / "foo" / "%" / "schema-html.metapack").string());
  entries.emplace(output / "explorer" / "%" / "search.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "foo" / "%" / "directory.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "directory.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "directory-html.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "404.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "routes.bin",
                  {.file_mark = MTIME(100), .dependencies = {}});
  const sourcemeta::one::Resolver::Views schemas{
      {"https://example.com/foo", {"/src/foo.json", "foo", MTIME(40)}}};
  const std::vector<std::filesystem::path> changed;
  const std::vector<std::filesystem::path> removed;
  const auto plan{sourcemeta::one::delta(sourcemeta::one::BuildPhase::Produce,
                                         sourcemeta::one::BuildPlan::Type::Full,
                                         entries, output, schemas, "1.0.0",
                                         true, "", changed, removed)};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, Full, 2, 4);

  EXPECT_ACTION(plan, 0, 0, 3, DirectoryList,
                output / "explorer" / "%" / "directory.metapack", "",
                output / "explorer" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 0, 1, 3, SearchIndex,
                output / "explorer" / "%" / "search.metapack", "",
                output / "explorer" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 0, 2, 3, WebSchema,
                output / "explorer" / "foo" / "%" / "schema-html.metapack",
                "https://example.com/foo",
                output / "explorer" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "health.metapack");

  EXPECT_ACTION(plan, 1, 0, 1, WebIndex,
                output / "explorer" / "%" / "directory-html.metapack", "",
                output / "explorer" / "%" / "directory.metapack");

  EXPECT_TOTAL_FILES(
      plan, entries, output / "version.json", output / "configuration.json",
      output / "schemas" / "foo" / "%" / "schema.metapack",
      output / "schemas" / "foo" / "%" / "dependencies.metapack",
      output / "schemas" / "foo" / "%" / "locations.metapack",
      output / "schemas" / "foo" / "%" / "positions.metapack",
      output / "schemas" / "foo" / "%" / "stats.metapack",
      output / "schemas" / "foo" / "%" / "bundle.metapack",
      output / "schemas" / "foo" / "%" / "health.metapack",
      output / "schemas" / "foo" / "%" / "blaze-exhaustive.metapack",
      output / "schemas" / "foo" / "%" / "blaze-fast.metapack",
      output / "schemas" / "foo" / "%" / "editor.metapack",
      output / "explorer" / "foo" / "%" / "schema.metapack",
      output / "explorer" / "foo" / "%" / "schema-html.metapack",
      output / "explorer" / "foo" / "%" / "directory.metapack",
      output / "explorer" / "foo" / "%" / "directory-html.metapack",
      output / "explorer" / "%" / "search.metapack",
      output / "explorer" / "%" / "directory.metapack",
      output / "explorer" / "%" / "directory-html.metapack",
      output / "explorer" / "%" / "404.metapack", output / "routes.bin");
}

TEST(Build_delta, incremental_missing_web_not_checked_headless) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  entries.emplace(output / "version.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "configuration.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  ADD_SCHEMA_ENTRIES(entries, output, "foo", true, false, MTIME(100));
  entries.emplace(output / "explorer" / "%" / "search.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "foo" / "%" / "directory.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "directory.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "routes.bin",
                  {.file_mark = MTIME(100), .dependencies = {}});
  const sourcemeta::one::Resolver::Views schemas{
      {"https://example.com/foo", {"/src/foo.json", "foo", MTIME(40)}}};
  const std::vector<std::filesystem::path> changed;
  const std::vector<std::filesystem::path> removed;
  const auto plan{sourcemeta::one::delta(
      sourcemeta::one::BuildPhase::Produce,
      sourcemeta::one::BuildPlan::Type::Headless, entries, output, schemas,
      "1.0.0", true, "", changed, removed)};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, Headless, 0, 0);

  EXPECT_TOTAL_FILES(
      plan, entries, output / "version.json", output / "configuration.json",
      output / "schemas" / "foo" / "%" / "schema.metapack",
      output / "schemas" / "foo" / "%" / "dependencies.metapack",
      output / "schemas" / "foo" / "%" / "locations.metapack",
      output / "schemas" / "foo" / "%" / "positions.metapack",
      output / "schemas" / "foo" / "%" / "stats.metapack",
      output / "schemas" / "foo" / "%" / "bundle.metapack",
      output / "schemas" / "foo" / "%" / "health.metapack",
      output / "schemas" / "foo" / "%" / "blaze-exhaustive.metapack",
      output / "schemas" / "foo" / "%" / "blaze-fast.metapack",
      output / "schemas" / "foo" / "%" / "editor.metapack",
      output / "explorer" / "foo" / "%" / "schema.metapack",
      output / "explorer" / "foo" / "%" / "directory.metapack",
      output / "explorer" / "%" / "search.metapack",
      output / "explorer" / "%" / "directory.metapack", output / "routes.bin");
}

TEST(Build_delta, mtime_nothing_changed) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  entries.emplace(output / "version.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "configuration.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  ADD_SCHEMA_ENTRIES(entries, output, "foo", true, true, MTIME(50));
  entries.emplace(output / "explorer" / "%" / "search.metapack",
                  {.file_mark = MTIME(50), .dependencies = {}});
  entries.emplace(output / "explorer" / "foo" / "%" / "directory.metapack",
                  {.file_mark = MTIME(50), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "directory.metapack",
                  {.file_mark = MTIME(50), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "directory-html.metapack",
                  {.file_mark = MTIME(50), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "404.metapack",
                  {.file_mark = MTIME(50), .dependencies = {}});
  entries.emplace(output / "routes.bin",
                  {.file_mark = MTIME(50), .dependencies = {}});
  const sourcemeta::one::Resolver::Views schemas{
      {"https://example.com/foo", {"/src/foo.json", "foo", MTIME(40)}}};
  const std::vector<std::filesystem::path> changed;
  const std::vector<std::filesystem::path> removed;
  const auto plan{sourcemeta::one::delta(sourcemeta::one::BuildPhase::Produce,
                                         sourcemeta::one::BuildPlan::Type::Full,
                                         entries, output, schemas, "1.0.0",
                                         true, "", changed, removed)};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, Full, 0, 0);

  EXPECT_TOTAL_FILES(
      plan, entries, output / "version.json", output / "configuration.json",
      output / "schemas" / "foo" / "%" / "schema.metapack",
      output / "schemas" / "foo" / "%" / "dependencies.metapack",
      output / "schemas" / "foo" / "%" / "locations.metapack",
      output / "schemas" / "foo" / "%" / "positions.metapack",
      output / "schemas" / "foo" / "%" / "stats.metapack",
      output / "schemas" / "foo" / "%" / "bundle.metapack",
      output / "schemas" / "foo" / "%" / "health.metapack",
      output / "schemas" / "foo" / "%" / "blaze-exhaustive.metapack",
      output / "schemas" / "foo" / "%" / "blaze-fast.metapack",
      output / "schemas" / "foo" / "%" / "editor.metapack",
      output / "explorer" / "foo" / "%" / "schema.metapack",
      output / "explorer" / "foo" / "%" / "schema-html.metapack",
      output / "explorer" / "foo" / "%" / "directory.metapack",
      output / "explorer" / "foo" / "%" / "directory-html.metapack",
      output / "explorer" / "%" / "search.metapack",
      output / "explorer" / "%" / "directory.metapack",
      output / "explorer" / "%" / "directory-html.metapack",
      output / "explorer" / "%" / "404.metapack", output / "routes.bin");
}

TEST(Build_delta, mtime_source_newer) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  entries.emplace(output / "version.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "configuration.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  ADD_SCHEMA_ENTRIES(entries, output, "foo", true, true, MTIME(10));
  ADD_SCHEMA_ENTRIES(entries, output, "bar", true, true, MTIME(50));
  entries.emplace(output / "explorer" / "%" / "search.metapack",
                  {.file_mark = MTIME(50), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "directory.metapack",
                  {.file_mark = MTIME(50), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "directory-html.metapack",
                  {.file_mark = MTIME(50), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "404.metapack",
                  {.file_mark = MTIME(50), .dependencies = {}});
  entries.emplace(output / "routes.bin",
                  {.file_mark = MTIME(50), .dependencies = {}});
  const sourcemeta::one::Resolver::Views schemas{
      {"https://example.com/foo", {"/src/foo.json", "foo", MTIME(50)}},
      {"https://example.com/bar", {"/src/bar.json", "bar", MTIME(40)}}};
  const std::vector<std::filesystem::path> changed;
  const std::vector<std::filesystem::path> removed;

  const auto plan{sourcemeta::one::delta(sourcemeta::one::BuildPhase::Produce,
                                         sourcemeta::one::BuildPlan::Type::Full,
                                         entries, output, schemas, "1.0.0",
                                         true, "", changed, removed)};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, Full, 6, 15);

  EXPECT_ACTION(plan, 0, 0, 1, Materialise,
                output / "schemas" / "foo" / "%" / "schema.metapack",
                "https://example.com/foo",
                std::filesystem::path{"/"} / "src" / "foo.json",
                output / "configuration.json");

  EXPECT_ACTION(plan, 1, 0, 4, Dependencies,
                output / "schemas" / "foo" / "%" / "dependencies.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 1, 1, 4, Locations,
                output / "schemas" / "foo" / "%" / "locations.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 1, 2, 4, Positions,
                output / "schemas" / "foo" / "%" / "positions.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 1, 3, 4, Stats,
                output / "schemas" / "foo" / "%" / "stats.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");

  EXPECT_ACTION(plan, 2, 0, 2, Bundle,
                output / "schemas" / "foo" / "%" / "bundle.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "dependencies.metapack");
  EXPECT_ACTION(plan, 2, 1, 2, Health,
                output / "schemas" / "foo" / "%" / "health.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "dependencies.metapack");

  EXPECT_ACTION(plan, 3, 0, 4, SchemaMetadata,
                output / "explorer" / "foo" / "%" / "schema.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "health.metapack",
                output / "schemas" / "foo" / "%" / "dependencies.metapack");
  EXPECT_ACTION(plan, 3, 1, 4, BlazeExhaustive,
                output / "schemas" / "foo" / "%" / "blaze-exhaustive.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "bundle.metapack");
  EXPECT_ACTION(plan, 3, 2, 4, BlazeFast,
                output / "schemas" / "foo" / "%" / "blaze-fast.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "bundle.metapack");
  EXPECT_ACTION(plan, 3, 3, 4, Editor,
                output / "schemas" / "foo" / "%" / "editor.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "bundle.metapack");

  EXPECT_ACTION_UNORDERED(plan, 4, 0, 3, DirectoryList,
                          output / "explorer" / "%" / "directory.metapack", "",
                          output / "explorer" / "foo" / "%" / "schema.metapack",
                          output / "explorer" / "bar" / "%" /
                              "schema.metapack");
  EXPECT_ACTION_UNORDERED(
      plan, 4, 1, 3, SearchIndex, output / "explorer" / "%" / "search.metapack",
      "", output / "explorer" / "foo" / "%" / "schema.metapack",
      output / "explorer" / "bar" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 4, 2, 3, WebSchema,
                output / "explorer" / "foo" / "%" / "schema-html.metapack",
                "https://example.com/foo",
                output / "explorer" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "health.metapack");

  EXPECT_ACTION(plan, 5, 0, 1, WebIndex,
                output / "explorer" / "%" / "directory-html.metapack", "",
                output / "explorer" / "%" / "directory.metapack");

  EXPECT_TOTAL_FILES(
      plan, entries, output / "version.json", output / "configuration.json",
      output / "schemas" / "foo" / "%" / "schema.metapack",
      output / "schemas" / "foo" / "%" / "dependencies.metapack",
      output / "schemas" / "foo" / "%" / "locations.metapack",
      output / "schemas" / "foo" / "%" / "positions.metapack",
      output / "schemas" / "foo" / "%" / "stats.metapack",
      output / "schemas" / "foo" / "%" / "bundle.metapack",
      output / "schemas" / "foo" / "%" / "health.metapack",
      output / "schemas" / "foo" / "%" / "blaze-exhaustive.metapack",
      output / "schemas" / "foo" / "%" / "blaze-fast.metapack",
      output / "schemas" / "foo" / "%" / "editor.metapack",
      output / "explorer" / "foo" / "%" / "schema.metapack",
      output / "explorer" / "foo" / "%" / "schema-html.metapack",
      output / "explorer" / "foo" / "%" / "directory-html.metapack",
      output / "schemas" / "bar" / "%" / "schema.metapack",
      output / "schemas" / "bar" / "%" / "dependencies.metapack",
      output / "schemas" / "bar" / "%" / "locations.metapack",
      output / "schemas" / "bar" / "%" / "positions.metapack",
      output / "schemas" / "bar" / "%" / "stats.metapack",
      output / "schemas" / "bar" / "%" / "bundle.metapack",
      output / "schemas" / "bar" / "%" / "health.metapack",
      output / "schemas" / "bar" / "%" / "blaze-exhaustive.metapack",
      output / "schemas" / "bar" / "%" / "blaze-fast.metapack",
      output / "schemas" / "bar" / "%" / "editor.metapack",
      output / "explorer" / "bar" / "%" / "schema.metapack",
      output / "explorer" / "bar" / "%" / "schema-html.metapack",
      output / "explorer" / "bar" / "%" / "directory-html.metapack",
      output / "explorer" / "%" / "search.metapack",
      output / "explorer" / "%" / "directory.metapack",
      output / "explorer" / "%" / "directory-html.metapack",
      output / "explorer" / "%" / "404.metapack", output / "routes.bin");
}

TEST(Build_delta, mtime_no_entry) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  entries.emplace(output / "version.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "configuration.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  ADD_SCHEMA_ENTRIES(entries, output, "bar", true, true, MTIME(50));
  entries.emplace(output / "explorer" / "%" / "search.metapack",
                  {.file_mark = MTIME(50), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "directory.metapack",
                  {.file_mark = MTIME(50), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "directory-html.metapack",
                  {.file_mark = MTIME(50), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "404.metapack",
                  {.file_mark = MTIME(50), .dependencies = {}});
  entries.emplace(output / "routes.bin",
                  {.file_mark = MTIME(50), .dependencies = {}});
  const sourcemeta::one::Resolver::Views schemas{
      {"https://example.com/foo", {"/src/foo.json", "foo", MTIME(40)}},
      {"https://example.com/bar", {"/src/bar.json", "bar", MTIME(40)}}};
  const std::vector<std::filesystem::path> changed;
  const std::vector<std::filesystem::path> removed;

  const auto plan{sourcemeta::one::delta(sourcemeta::one::BuildPhase::Produce,
                                         sourcemeta::one::BuildPlan::Type::Full,
                                         entries, output, schemas, "1.0.0",
                                         true, "", changed, removed)};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, Full, 6, 15);

  EXPECT_ACTION(plan, 0, 0, 1, Materialise,
                output / "schemas" / "foo" / "%" / "schema.metapack",
                "https://example.com/foo",
                std::filesystem::path{"/"} / "src" / "foo.json",
                output / "configuration.json");

  EXPECT_ACTION(plan, 1, 0, 4, Dependencies,
                output / "schemas" / "foo" / "%" / "dependencies.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 1, 1, 4, Locations,
                output / "schemas" / "foo" / "%" / "locations.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 1, 2, 4, Positions,
                output / "schemas" / "foo" / "%" / "positions.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 1, 3, 4, Stats,
                output / "schemas" / "foo" / "%" / "stats.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");

  EXPECT_ACTION(plan, 2, 0, 2, Bundle,
                output / "schemas" / "foo" / "%" / "bundle.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "dependencies.metapack");
  EXPECT_ACTION(plan, 2, 1, 2, Health,
                output / "schemas" / "foo" / "%" / "health.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "dependencies.metapack");

  EXPECT_ACTION(plan, 3, 0, 4, SchemaMetadata,
                output / "explorer" / "foo" / "%" / "schema.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "health.metapack",
                output / "schemas" / "foo" / "%" / "dependencies.metapack");
  EXPECT_ACTION(plan, 3, 1, 4, BlazeExhaustive,
                output / "schemas" / "foo" / "%" / "blaze-exhaustive.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "bundle.metapack");
  EXPECT_ACTION(plan, 3, 2, 4, BlazeFast,
                output / "schemas" / "foo" / "%" / "blaze-fast.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "bundle.metapack");
  EXPECT_ACTION(plan, 3, 3, 4, Editor,
                output / "schemas" / "foo" / "%" / "editor.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "bundle.metapack");

  EXPECT_ACTION_UNORDERED(plan, 4, 0, 3, DirectoryList,
                          output / "explorer" / "%" / "directory.metapack", "",
                          output / "explorer" / "foo" / "%" / "schema.metapack",
                          output / "explorer" / "bar" / "%" /
                              "schema.metapack");
  EXPECT_ACTION_UNORDERED(
      plan, 4, 1, 3, SearchIndex, output / "explorer" / "%" / "search.metapack",
      "", output / "explorer" / "foo" / "%" / "schema.metapack",
      output / "explorer" / "bar" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 4, 2, 3, WebSchema,
                output / "explorer" / "foo" / "%" / "schema-html.metapack",
                "https://example.com/foo",
                output / "explorer" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "health.metapack");

  EXPECT_ACTION(plan, 5, 0, 1, WebIndex,
                output / "explorer" / "%" / "directory-html.metapack", "",
                output / "explorer" / "%" / "directory.metapack");

  EXPECT_TOTAL_FILES(
      plan, entries, output / "version.json", output / "configuration.json",
      output / "schemas" / "foo" / "%" / "schema.metapack",
      output / "schemas" / "foo" / "%" / "dependencies.metapack",
      output / "schemas" / "foo" / "%" / "locations.metapack",
      output / "schemas" / "foo" / "%" / "positions.metapack",
      output / "schemas" / "foo" / "%" / "stats.metapack",
      output / "schemas" / "foo" / "%" / "bundle.metapack",
      output / "schemas" / "foo" / "%" / "health.metapack",
      output / "schemas" / "foo" / "%" / "blaze-exhaustive.metapack",
      output / "schemas" / "foo" / "%" / "blaze-fast.metapack",
      output / "schemas" / "foo" / "%" / "editor.metapack",
      output / "explorer" / "foo" / "%" / "schema.metapack",
      output / "explorer" / "foo" / "%" / "schema-html.metapack",
      output / "schemas" / "bar" / "%" / "schema.metapack",
      output / "schemas" / "bar" / "%" / "dependencies.metapack",
      output / "schemas" / "bar" / "%" / "locations.metapack",
      output / "schemas" / "bar" / "%" / "positions.metapack",
      output / "schemas" / "bar" / "%" / "stats.metapack",
      output / "schemas" / "bar" / "%" / "bundle.metapack",
      output / "schemas" / "bar" / "%" / "health.metapack",
      output / "schemas" / "bar" / "%" / "blaze-exhaustive.metapack",
      output / "schemas" / "bar" / "%" / "blaze-fast.metapack",
      output / "schemas" / "bar" / "%" / "editor.metapack",
      output / "explorer" / "bar" / "%" / "schema.metapack",
      output / "explorer" / "bar" / "%" / "schema-html.metapack",
      output / "explorer" / "bar" / "%" / "directory-html.metapack",
      output / "explorer" / "%" / "search.metapack",
      output / "explorer" / "%" / "directory.metapack",
      output / "explorer" / "%" / "directory-html.metapack",
      output / "explorer" / "%" / "404.metapack", output / "routes.bin");
}

TEST(Build_delta, mtime_no_file_mark) {
  const std::filesystem::path output{"/output"};

  sourcemeta::one::BuildState entries;
  entries.emplace(output / "version.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "configuration.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  ADD_SCHEMA_ENTRIES(entries, output, "foo", true, true, MTIME(100));
  entries.emplace(output / "explorer" / "%" / "search.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "directory.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "directory-html.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "404.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "routes.bin",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.forget(
      (output / "schemas" / "foo" / "%" / "schema.metapack").string());
  const sourcemeta::one::Resolver::Views schemas{
      {"https://example.com/foo", {"/src/foo.json", "foo", MTIME(40)}}};
  const std::vector<std::filesystem::path> changed;
  const std::vector<std::filesystem::path> removed;

  const auto plan{sourcemeta::one::delta(sourcemeta::one::BuildPhase::Produce,
                                         sourcemeta::one::BuildPlan::Type::Full,
                                         entries, output, schemas, "1.0.0",
                                         true, "", changed, removed)};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, Full, 6, 15);

  EXPECT_ACTION(plan, 0, 0, 1, Materialise,
                output / "schemas" / "foo" / "%" / "schema.metapack",
                "https://example.com/foo",
                std::filesystem::path{"/"} / "src" / "foo.json",
                output / "configuration.json");

  EXPECT_ACTION(plan, 1, 0, 4, Dependencies,
                output / "schemas" / "foo" / "%" / "dependencies.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 1, 1, 4, Locations,
                output / "schemas" / "foo" / "%" / "locations.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 1, 2, 4, Positions,
                output / "schemas" / "foo" / "%" / "positions.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 1, 3, 4, Stats,
                output / "schemas" / "foo" / "%" / "stats.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");

  EXPECT_ACTION(plan, 2, 0, 2, Bundle,
                output / "schemas" / "foo" / "%" / "bundle.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "dependencies.metapack");
  EXPECT_ACTION(plan, 2, 1, 2, Health,
                output / "schemas" / "foo" / "%" / "health.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "dependencies.metapack");

  EXPECT_ACTION(plan, 3, 0, 4, SchemaMetadata,
                output / "explorer" / "foo" / "%" / "schema.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "health.metapack",
                output / "schemas" / "foo" / "%" / "dependencies.metapack");
  EXPECT_ACTION(plan, 3, 1, 4, BlazeExhaustive,
                output / "schemas" / "foo" / "%" / "blaze-exhaustive.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "bundle.metapack");
  EXPECT_ACTION(plan, 3, 2, 4, BlazeFast,
                output / "schemas" / "foo" / "%" / "blaze-fast.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "bundle.metapack");
  EXPECT_ACTION(plan, 3, 3, 4, Editor,
                output / "schemas" / "foo" / "%" / "editor.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "bundle.metapack");

  EXPECT_ACTION(plan, 4, 0, 3, DirectoryList,
                output / "explorer" / "%" / "directory.metapack", "",
                output / "explorer" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 4, 1, 3, SearchIndex,
                output / "explorer" / "%" / "search.metapack", "",
                output / "explorer" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 4, 2, 3, WebSchema,
                output / "explorer" / "foo" / "%" / "schema-html.metapack",
                "https://example.com/foo",
                output / "explorer" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "health.metapack");

  EXPECT_ACTION(plan, 5, 0, 1, WebIndex,
                output / "explorer" / "%" / "directory-html.metapack", "",
                output / "explorer" / "%" / "directory.metapack");

  EXPECT_TOTAL_FILES(
      plan, entries, output / "version.json", output / "configuration.json",
      output / "schemas" / "foo" / "%" / "schema.metapack",
      output / "schemas" / "foo" / "%" / "dependencies.metapack",
      output / "schemas" / "foo" / "%" / "locations.metapack",
      output / "schemas" / "foo" / "%" / "positions.metapack",
      output / "schemas" / "foo" / "%" / "stats.metapack",
      output / "schemas" / "foo" / "%" / "bundle.metapack",
      output / "schemas" / "foo" / "%" / "health.metapack",
      output / "schemas" / "foo" / "%" / "blaze-exhaustive.metapack",
      output / "schemas" / "foo" / "%" / "blaze-fast.metapack",
      output / "schemas" / "foo" / "%" / "editor.metapack",
      output / "explorer" / "foo" / "%" / "schema.metapack",
      output / "explorer" / "foo" / "%" / "schema-html.metapack",
      output / "explorer" / "foo" / "%" / "directory-html.metapack",
      output / "explorer" / "%" / "search.metapack",
      output / "explorer" / "%" / "directory.metapack",
      output / "explorer" / "%" / "directory-html.metapack",
      output / "explorer" / "%" / "404.metapack", output / "routes.bin");
}

TEST(Build_delta, incremental_reverse_dep_direct) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  entries.emplace(output / "version.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(
      output / "schemas" / "b" / "%" / "dependencies.metapack",
      {.file_mark = MTIME(100),
       .dependencies = {output / "schemas" / "a" / "%" / "schema.metapack"}});
  entries.emplace(output / "explorer" / "a" / "%" / "schema.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "b" / "%" / "schema.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "configuration.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  const sourcemeta::one::Resolver::Views schemas{
      {"https://example.com/a", {"/src/a.json", "a", MTIME(100)}},
      {"https://example.com/b", {"/src/b.json", "b", MTIME(100)}}};
  const std::vector<std::filesystem::path> changed{"/src/a.json"};
  const std::vector<std::filesystem::path> removed;

  const auto plan{sourcemeta::one::delta(sourcemeta::one::BuildPhase::Produce,
                                         sourcemeta::one::BuildPlan::Type::Full,
                                         entries, output, schemas, "1.0.0",
                                         true, "", changed, removed)};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, Full, 7, 28);

  EXPECT_ACTION(plan, 0, 0, 2, Materialise,
                output / "schemas" / "a" / "%" / "schema.metapack",
                "https://example.com/a",
                std::filesystem::path{"/"} / "src" / "a.json",
                output / "configuration.json");
  EXPECT_ACTION(plan, 0, 1, 2, Materialise,
                output / "schemas" / "b" / "%" / "schema.metapack",
                "https://example.com/b",
                std::filesystem::path{"/"} / "src" / "b.json",
                output / "configuration.json");

  EXPECT_ACTION(plan, 1, 0, 8, Dependencies,
                output / "schemas" / "a" / "%" / "dependencies.metapack",
                "https://example.com/a",
                output / "schemas" / "a" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 1, 1, 8, Locations,
                output / "schemas" / "a" / "%" / "locations.metapack",
                "https://example.com/a",
                output / "schemas" / "a" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 1, 2, 8, Positions,
                output / "schemas" / "a" / "%" / "positions.metapack",
                "https://example.com/a",
                output / "schemas" / "a" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 1, 3, 8, Stats,
                output / "schemas" / "a" / "%" / "stats.metapack",
                "https://example.com/a",
                output / "schemas" / "a" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 1, 4, 8, Dependencies,
                output / "schemas" / "b" / "%" / "dependencies.metapack",
                "https://example.com/b",
                output / "schemas" / "b" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 1, 5, 8, Locations,
                output / "schemas" / "b" / "%" / "locations.metapack",
                "https://example.com/b",
                output / "schemas" / "b" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 1, 6, 8, Positions,
                output / "schemas" / "b" / "%" / "positions.metapack",
                "https://example.com/b",
                output / "schemas" / "b" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 1, 7, 8, Stats,
                output / "schemas" / "b" / "%" / "stats.metapack",
                "https://example.com/b",
                output / "schemas" / "b" / "%" / "schema.metapack");

  EXPECT_ACTION(plan, 2, 0, 4, Bundle,
                output / "schemas" / "a" / "%" / "bundle.metapack",
                "https://example.com/a",
                output / "schemas" / "a" / "%" / "schema.metapack",
                output / "schemas" / "a" / "%" / "dependencies.metapack");
  EXPECT_ACTION(plan, 2, 1, 4, Health,
                output / "schemas" / "a" / "%" / "health.metapack",
                "https://example.com/a",
                output / "schemas" / "a" / "%" / "schema.metapack",
                output / "schemas" / "a" / "%" / "dependencies.metapack");
  EXPECT_ACTION(plan, 2, 2, 4, Bundle,
                output / "schemas" / "b" / "%" / "bundle.metapack",
                "https://example.com/b",
                output / "schemas" / "b" / "%" / "schema.metapack",
                output / "schemas" / "b" / "%" / "dependencies.metapack");
  EXPECT_ACTION(plan, 2, 3, 4, Health,
                output / "schemas" / "b" / "%" / "health.metapack",
                "https://example.com/b",
                output / "schemas" / "b" / "%" / "schema.metapack",
                output / "schemas" / "b" / "%" / "dependencies.metapack");

  EXPECT_ACTION(plan, 3, 0, 8, SchemaMetadata,
                output / "explorer" / "a" / "%" / "schema.metapack",
                "https://example.com/a",
                output / "schemas" / "a" / "%" / "schema.metapack",
                output / "schemas" / "a" / "%" / "health.metapack",
                output / "schemas" / "a" / "%" / "dependencies.metapack");
  EXPECT_ACTION(plan, 3, 1, 8, SchemaMetadata,
                output / "explorer" / "b" / "%" / "schema.metapack",
                "https://example.com/b",
                output / "schemas" / "b" / "%" / "schema.metapack",
                output / "schemas" / "b" / "%" / "health.metapack",
                output / "schemas" / "b" / "%" / "dependencies.metapack");
  EXPECT_ACTION(plan, 3, 2, 8, BlazeExhaustive,
                output / "schemas" / "a" / "%" / "blaze-exhaustive.metapack",
                "https://example.com/a",
                output / "schemas" / "a" / "%" / "bundle.metapack");
  EXPECT_ACTION(plan, 3, 3, 8, BlazeFast,
                output / "schemas" / "a" / "%" / "blaze-fast.metapack",
                "https://example.com/a",
                output / "schemas" / "a" / "%" / "bundle.metapack");
  EXPECT_ACTION(plan, 3, 4, 8, Editor,
                output / "schemas" / "a" / "%" / "editor.metapack",
                "https://example.com/a",
                output / "schemas" / "a" / "%" / "bundle.metapack");
  EXPECT_ACTION(plan, 3, 5, 8, BlazeExhaustive,
                output / "schemas" / "b" / "%" / "blaze-exhaustive.metapack",
                "https://example.com/b",
                output / "schemas" / "b" / "%" / "bundle.metapack");
  EXPECT_ACTION(plan, 3, 6, 8, BlazeFast,
                output / "schemas" / "b" / "%" / "blaze-fast.metapack",
                "https://example.com/b",
                output / "schemas" / "b" / "%" / "bundle.metapack");
  EXPECT_ACTION(plan, 3, 7, 8, Editor,
                output / "schemas" / "b" / "%" / "editor.metapack",
                "https://example.com/b",
                output / "schemas" / "b" / "%" / "bundle.metapack");

  EXPECT_ACTION_UNORDERED(plan, 4, 0, 4, DirectoryList,
                          output / "explorer" / "%" / "directory.metapack", "",
                          output / "explorer" / "a" / "%" / "schema.metapack",
                          output / "explorer" / "b" / "%" / "schema.metapack");
  EXPECT_ACTION_UNORDERED(plan, 4, 1, 4, SearchIndex,
                          output / "explorer" / "%" / "search.metapack", "",
                          output / "explorer" / "a" / "%" / "schema.metapack",
                          output / "explorer" / "b" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 4, 2, 4, WebSchema,
                output / "explorer" / "a" / "%" / "schema-html.metapack",
                "https://example.com/a",
                output / "explorer" / "a" / "%" / "schema.metapack",
                output / "schemas" / "a" / "%" / "health.metapack");
  EXPECT_ACTION(plan, 4, 3, 4, WebSchema,
                output / "explorer" / "b" / "%" / "schema-html.metapack",
                "https://example.com/b",
                output / "explorer" / "b" / "%" / "schema.metapack",
                output / "schemas" / "b" / "%" / "health.metapack");

  EXPECT_ACTION(plan, 5, 0, 1, WebIndex,
                output / "explorer" / "%" / "directory-html.metapack", "",
                output / "explorer" / "%" / "directory.metapack");

  EXPECT_ACTION(plan, 6, 0, 1, Routes, output / "routes.bin", "Full",
                output / "configuration.json");

  EXPECT_TOTAL_FILES(
      plan, entries, output / "version.json", output / "configuration.json",
      output / "schemas" / "a" / "%" / "schema.metapack",
      output / "schemas" / "a" / "%" / "dependencies.metapack",
      output / "schemas" / "a" / "%" / "locations.metapack",
      output / "schemas" / "a" / "%" / "positions.metapack",
      output / "schemas" / "a" / "%" / "stats.metapack",
      output / "schemas" / "a" / "%" / "bundle.metapack",
      output / "schemas" / "a" / "%" / "health.metapack",
      output / "schemas" / "a" / "%" / "blaze-exhaustive.metapack",
      output / "schemas" / "a" / "%" / "blaze-fast.metapack",
      output / "schemas" / "a" / "%" / "editor.metapack",
      output / "explorer" / "a" / "%" / "schema.metapack",
      output / "explorer" / "a" / "%" / "schema-html.metapack",
      output / "schemas" / "b" / "%" / "schema.metapack",
      output / "schemas" / "b" / "%" / "dependencies.metapack",
      output / "schemas" / "b" / "%" / "locations.metapack",
      output / "schemas" / "b" / "%" / "positions.metapack",
      output / "schemas" / "b" / "%" / "stats.metapack",
      output / "schemas" / "b" / "%" / "bundle.metapack",
      output / "schemas" / "b" / "%" / "health.metapack",
      output / "schemas" / "b" / "%" / "blaze-exhaustive.metapack",
      output / "schemas" / "b" / "%" / "blaze-fast.metapack",
      output / "schemas" / "b" / "%" / "editor.metapack",
      output / "explorer" / "b" / "%" / "schema.metapack",
      output / "explorer" / "b" / "%" / "schema-html.metapack",
      output / "explorer" / "%" / "search.metapack",
      output / "explorer" / "%" / "directory.metapack",
      output / "explorer" / "%" / "directory-html.metapack",
      output / "routes.bin");
}

TEST(Build_delta, incremental_reverse_dep_transitive) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  entries.emplace(output / "version.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(
      output / "schemas" / "b" / "%" / "dependencies.metapack",
      {.file_mark = MTIME(100),
       .dependencies = {output / "schemas" / "a" / "%" / "schema.metapack"}});
  entries.emplace(
      output / "schemas" / "c" / "%" / "dependencies.metapack",
      {.file_mark = MTIME(100),
       .dependencies = {output / "schemas" / "b" / "%" / "schema.metapack"}});
  entries.emplace(output / "explorer" / "a" / "%" / "schema.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "b" / "%" / "schema.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "c" / "%" / "schema.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "configuration.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  const sourcemeta::one::Resolver::Views schemas{
      {"https://example.com/a", {"/src/a.json", "a", MTIME(100)}},
      {"https://example.com/b", {"/src/b.json", "b", MTIME(100)}},
      {"https://example.com/c", {"/src/c.json", "c", MTIME(100)}}};
  const std::vector<std::filesystem::path> changed{"/src/a.json"};
  const std::vector<std::filesystem::path> removed;

  const auto plan{sourcemeta::one::delta(sourcemeta::one::BuildPhase::Produce,
                                         sourcemeta::one::BuildPlan::Type::Full,
                                         entries, output, schemas, "1.0.0",
                                         true, "", changed, removed)};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, Full, 7, 40);

  EXPECT_ACTION(plan, 0, 0, 3, Materialise,
                output / "schemas" / "a" / "%" / "schema.metapack",
                "https://example.com/a",
                std::filesystem::path{"/"} / "src" / "a.json",
                output / "configuration.json");
  EXPECT_ACTION(plan, 0, 1, 3, Materialise,
                output / "schemas" / "b" / "%" / "schema.metapack",
                "https://example.com/b",
                std::filesystem::path{"/"} / "src" / "b.json",
                output / "configuration.json");
  EXPECT_ACTION(plan, 0, 2, 3, Materialise,
                output / "schemas" / "c" / "%" / "schema.metapack",
                "https://example.com/c",
                std::filesystem::path{"/"} / "src" / "c.json",
                output / "configuration.json");

  EXPECT_ACTION(plan, 1, 0, 12, Dependencies,
                output / "schemas" / "a" / "%" / "dependencies.metapack",
                "https://example.com/a",
                output / "schemas" / "a" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 1, 1, 12, Locations,
                output / "schemas" / "a" / "%" / "locations.metapack",
                "https://example.com/a",
                output / "schemas" / "a" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 1, 2, 12, Positions,
                output / "schemas" / "a" / "%" / "positions.metapack",
                "https://example.com/a",
                output / "schemas" / "a" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 1, 3, 12, Stats,
                output / "schemas" / "a" / "%" / "stats.metapack",
                "https://example.com/a",
                output / "schemas" / "a" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 1, 4, 12, Dependencies,
                output / "schemas" / "b" / "%" / "dependencies.metapack",
                "https://example.com/b",
                output / "schemas" / "b" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 1, 5, 12, Locations,
                output / "schemas" / "b" / "%" / "locations.metapack",
                "https://example.com/b",
                output / "schemas" / "b" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 1, 6, 12, Positions,
                output / "schemas" / "b" / "%" / "positions.metapack",
                "https://example.com/b",
                output / "schemas" / "b" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 1, 7, 12, Stats,
                output / "schemas" / "b" / "%" / "stats.metapack",
                "https://example.com/b",
                output / "schemas" / "b" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 1, 8, 12, Dependencies,
                output / "schemas" / "c" / "%" / "dependencies.metapack",
                "https://example.com/c",
                output / "schemas" / "c" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 1, 9, 12, Locations,
                output / "schemas" / "c" / "%" / "locations.metapack",
                "https://example.com/c",
                output / "schemas" / "c" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 1, 10, 12, Positions,
                output / "schemas" / "c" / "%" / "positions.metapack",
                "https://example.com/c",
                output / "schemas" / "c" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 1, 11, 12, Stats,
                output / "schemas" / "c" / "%" / "stats.metapack",
                "https://example.com/c",
                output / "schemas" / "c" / "%" / "schema.metapack");

  EXPECT_ACTION(plan, 2, 0, 6, Bundle,
                output / "schemas" / "a" / "%" / "bundle.metapack",
                "https://example.com/a",
                output / "schemas" / "a" / "%" / "schema.metapack",
                output / "schemas" / "a" / "%" / "dependencies.metapack");
  EXPECT_ACTION(plan, 2, 1, 6, Health,
                output / "schemas" / "a" / "%" / "health.metapack",
                "https://example.com/a",
                output / "schemas" / "a" / "%" / "schema.metapack",
                output / "schemas" / "a" / "%" / "dependencies.metapack");
  EXPECT_ACTION(plan, 2, 2, 6, Bundle,
                output / "schemas" / "b" / "%" / "bundle.metapack",
                "https://example.com/b",
                output / "schemas" / "b" / "%" / "schema.metapack",
                output / "schemas" / "b" / "%" / "dependencies.metapack");
  EXPECT_ACTION(plan, 2, 3, 6, Health,
                output / "schemas" / "b" / "%" / "health.metapack",
                "https://example.com/b",
                output / "schemas" / "b" / "%" / "schema.metapack",
                output / "schemas" / "b" / "%" / "dependencies.metapack");
  EXPECT_ACTION(plan, 2, 4, 6, Bundle,
                output / "schemas" / "c" / "%" / "bundle.metapack",
                "https://example.com/c",
                output / "schemas" / "c" / "%" / "schema.metapack",
                output / "schemas" / "c" / "%" / "dependencies.metapack");
  EXPECT_ACTION(plan, 2, 5, 6, Health,
                output / "schemas" / "c" / "%" / "health.metapack",
                "https://example.com/c",
                output / "schemas" / "c" / "%" / "schema.metapack",
                output / "schemas" / "c" / "%" / "dependencies.metapack");

  EXPECT_ACTION(plan, 3, 0, 12, SchemaMetadata,
                output / "explorer" / "a" / "%" / "schema.metapack",
                "https://example.com/a",
                output / "schemas" / "a" / "%" / "schema.metapack",
                output / "schemas" / "a" / "%" / "health.metapack",
                output / "schemas" / "a" / "%" / "dependencies.metapack");
  EXPECT_ACTION(plan, 3, 1, 12, SchemaMetadata,
                output / "explorer" / "b" / "%" / "schema.metapack",
                "https://example.com/b",
                output / "schemas" / "b" / "%" / "schema.metapack",
                output / "schemas" / "b" / "%" / "health.metapack",
                output / "schemas" / "b" / "%" / "dependencies.metapack");
  EXPECT_ACTION(plan, 3, 2, 12, SchemaMetadata,
                output / "explorer" / "c" / "%" / "schema.metapack",
                "https://example.com/c",
                output / "schemas" / "c" / "%" / "schema.metapack",
                output / "schemas" / "c" / "%" / "health.metapack",
                output / "schemas" / "c" / "%" / "dependencies.metapack");
  EXPECT_ACTION(plan, 3, 3, 12, BlazeExhaustive,
                output / "schemas" / "a" / "%" / "blaze-exhaustive.metapack",
                "https://example.com/a",
                output / "schemas" / "a" / "%" / "bundle.metapack");
  EXPECT_ACTION(plan, 3, 4, 12, BlazeFast,
                output / "schemas" / "a" / "%" / "blaze-fast.metapack",
                "https://example.com/a",
                output / "schemas" / "a" / "%" / "bundle.metapack");
  EXPECT_ACTION(plan, 3, 5, 12, Editor,
                output / "schemas" / "a" / "%" / "editor.metapack",
                "https://example.com/a",
                output / "schemas" / "a" / "%" / "bundle.metapack");
  EXPECT_ACTION(plan, 3, 6, 12, BlazeExhaustive,
                output / "schemas" / "b" / "%" / "blaze-exhaustive.metapack",
                "https://example.com/b",
                output / "schemas" / "b" / "%" / "bundle.metapack");
  EXPECT_ACTION(plan, 3, 7, 12, BlazeFast,
                output / "schemas" / "b" / "%" / "blaze-fast.metapack",
                "https://example.com/b",
                output / "schemas" / "b" / "%" / "bundle.metapack");
  EXPECT_ACTION(plan, 3, 8, 12, Editor,
                output / "schemas" / "b" / "%" / "editor.metapack",
                "https://example.com/b",
                output / "schemas" / "b" / "%" / "bundle.metapack");
  EXPECT_ACTION(plan, 3, 9, 12, BlazeExhaustive,
                output / "schemas" / "c" / "%" / "blaze-exhaustive.metapack",
                "https://example.com/c",
                output / "schemas" / "c" / "%" / "bundle.metapack");
  EXPECT_ACTION(plan, 3, 10, 12, BlazeFast,
                output / "schemas" / "c" / "%" / "blaze-fast.metapack",
                "https://example.com/c",
                output / "schemas" / "c" / "%" / "bundle.metapack");
  EXPECT_ACTION(plan, 3, 11, 12, Editor,
                output / "schemas" / "c" / "%" / "editor.metapack",
                "https://example.com/c",
                output / "schemas" / "c" / "%" / "bundle.metapack");

  EXPECT_ACTION_UNORDERED(plan, 4, 0, 5, DirectoryList,
                          output / "explorer" / "%" / "directory.metapack", "",
                          output / "explorer" / "a" / "%" / "schema.metapack",
                          output / "explorer" / "b" / "%" / "schema.metapack",
                          output / "explorer" / "c" / "%" / "schema.metapack");
  EXPECT_ACTION_UNORDERED(plan, 4, 1, 5, SearchIndex,
                          output / "explorer" / "%" / "search.metapack", "",
                          output / "explorer" / "a" / "%" / "schema.metapack",
                          output / "explorer" / "b" / "%" / "schema.metapack",
                          output / "explorer" / "c" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 4, 2, 5, WebSchema,
                output / "explorer" / "a" / "%" / "schema-html.metapack",
                "https://example.com/a",
                output / "explorer" / "a" / "%" / "schema.metapack",
                output / "schemas" / "a" / "%" / "health.metapack");
  EXPECT_ACTION(plan, 4, 3, 5, WebSchema,
                output / "explorer" / "b" / "%" / "schema-html.metapack",
                "https://example.com/b",
                output / "explorer" / "b" / "%" / "schema.metapack",
                output / "schemas" / "b" / "%" / "health.metapack");
  EXPECT_ACTION(plan, 4, 4, 5, WebSchema,
                output / "explorer" / "c" / "%" / "schema-html.metapack",
                "https://example.com/c",
                output / "explorer" / "c" / "%" / "schema.metapack",
                output / "schemas" / "c" / "%" / "health.metapack");

  EXPECT_ACTION(plan, 5, 0, 1, WebIndex,
                output / "explorer" / "%" / "directory-html.metapack", "",
                output / "explorer" / "%" / "directory.metapack");

  EXPECT_ACTION(plan, 6, 0, 1, Routes, output / "routes.bin", "Full",
                output / "configuration.json");

  EXPECT_TOTAL_FILES(
      plan, entries, output / "version.json", output / "configuration.json",
      output / "schemas" / "a" / "%" / "schema.metapack",
      output / "schemas" / "a" / "%" / "dependencies.metapack",
      output / "schemas" / "a" / "%" / "locations.metapack",
      output / "schemas" / "a" / "%" / "positions.metapack",
      output / "schemas" / "a" / "%" / "stats.metapack",
      output / "schemas" / "a" / "%" / "bundle.metapack",
      output / "schemas" / "a" / "%" / "health.metapack",
      output / "schemas" / "a" / "%" / "blaze-exhaustive.metapack",
      output / "schemas" / "a" / "%" / "blaze-fast.metapack",
      output / "schemas" / "a" / "%" / "editor.metapack",
      output / "explorer" / "a" / "%" / "schema.metapack",
      output / "explorer" / "a" / "%" / "schema-html.metapack",
      output / "schemas" / "b" / "%" / "schema.metapack",
      output / "schemas" / "b" / "%" / "dependencies.metapack",
      output / "schemas" / "b" / "%" / "locations.metapack",
      output / "schemas" / "b" / "%" / "positions.metapack",
      output / "schemas" / "b" / "%" / "stats.metapack",
      output / "schemas" / "b" / "%" / "bundle.metapack",
      output / "schemas" / "b" / "%" / "health.metapack",
      output / "schemas" / "b" / "%" / "blaze-exhaustive.metapack",
      output / "schemas" / "b" / "%" / "blaze-fast.metapack",
      output / "schemas" / "b" / "%" / "editor.metapack",
      output / "explorer" / "b" / "%" / "schema.metapack",
      output / "explorer" / "b" / "%" / "schema-html.metapack",
      output / "schemas" / "c" / "%" / "schema.metapack",
      output / "schemas" / "c" / "%" / "dependencies.metapack",
      output / "schemas" / "c" / "%" / "locations.metapack",
      output / "schemas" / "c" / "%" / "positions.metapack",
      output / "schemas" / "c" / "%" / "stats.metapack",
      output / "schemas" / "c" / "%" / "bundle.metapack",
      output / "schemas" / "c" / "%" / "health.metapack",
      output / "schemas" / "c" / "%" / "blaze-exhaustive.metapack",
      output / "schemas" / "c" / "%" / "blaze-fast.metapack",
      output / "schemas" / "c" / "%" / "editor.metapack",
      output / "explorer" / "c" / "%" / "schema.metapack",
      output / "explorer" / "c" / "%" / "schema-html.metapack",
      output / "explorer" / "%" / "search.metapack",
      output / "explorer" / "%" / "directory.metapack",
      output / "explorer" / "%" / "directory-html.metapack",
      output / "routes.bin");
}

TEST(Build_delta, mtime_reverse_dep) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  entries.emplace(output / "version.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "schemas" / "a" / "%" / "schema.metapack",
                  {.file_mark = MTIME(10), .dependencies = {}});
  entries.emplace(output / "schemas" / "b" / "%" / "schema.metapack",
                  {.file_mark = MTIME(50), .dependencies = {}});
  entries.emplace(
      output / "schemas" / "b" / "%" / "dependencies.metapack",
      {.file_mark = MTIME(100),
       .dependencies = {output / "schemas" / "a" / "%" / "schema.metapack"}});
  entries.emplace(output / "explorer" / "a" / "%" / "schema.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "b" / "%" / "schema.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "configuration.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  const sourcemeta::one::Resolver::Views schemas{
      {"https://example.com/a", {"/src/a.json", "a", MTIME(30)}},
      {"https://example.com/b", {"/src/b.json", "b", MTIME(30)}}};
  const std::vector<std::filesystem::path> changed;
  const std::vector<std::filesystem::path> removed;

  const auto plan{sourcemeta::one::delta(sourcemeta::one::BuildPhase::Produce,
                                         sourcemeta::one::BuildPlan::Type::Full,
                                         entries, output, schemas, "1.0.0",
                                         true, "", changed, removed)};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, Full, 7, 27);

  EXPECT_ACTION(plan, 0, 0, 5, Materialise,
                output / "schemas" / "a" / "%" / "schema.metapack",
                "https://example.com/a",
                std::filesystem::path{"/"} / "src" / "a.json",
                output / "configuration.json");
  EXPECT_ACTION(plan, 0, 1, 5, Dependencies,
                output / "schemas" / "b" / "%" / "dependencies.metapack",
                "https://example.com/b",
                output / "schemas" / "b" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 0, 2, 5, Locations,
                output / "schemas" / "b" / "%" / "locations.metapack",
                "https://example.com/b",
                output / "schemas" / "b" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 0, 3, 5, Positions,
                output / "schemas" / "b" / "%" / "positions.metapack",
                "https://example.com/b",
                output / "schemas" / "b" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 0, 4, 5, Stats,
                output / "schemas" / "b" / "%" / "stats.metapack",
                "https://example.com/b",
                output / "schemas" / "b" / "%" / "schema.metapack");

  EXPECT_ACTION(plan, 1, 0, 6, Dependencies,
                output / "schemas" / "a" / "%" / "dependencies.metapack",
                "https://example.com/a",
                output / "schemas" / "a" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 1, 1, 6, Locations,
                output / "schemas" / "a" / "%" / "locations.metapack",
                "https://example.com/a",
                output / "schemas" / "a" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 1, 2, 6, Positions,
                output / "schemas" / "a" / "%" / "positions.metapack",
                "https://example.com/a",
                output / "schemas" / "a" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 1, 3, 6, Stats,
                output / "schemas" / "a" / "%" / "stats.metapack",
                "https://example.com/a",
                output / "schemas" / "a" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 1, 4, 6, Bundle,
                output / "schemas" / "b" / "%" / "bundle.metapack",
                "https://example.com/b",
                output / "schemas" / "b" / "%" / "schema.metapack",
                output / "schemas" / "b" / "%" / "dependencies.metapack");
  EXPECT_ACTION(plan, 1, 5, 6, Health,
                output / "schemas" / "b" / "%" / "health.metapack",
                "https://example.com/b",
                output / "schemas" / "b" / "%" / "schema.metapack",
                output / "schemas" / "b" / "%" / "dependencies.metapack");

  EXPECT_ACTION(plan, 2, 0, 6, SchemaMetadata,
                output / "explorer" / "b" / "%" / "schema.metapack",
                "https://example.com/b",
                output / "schemas" / "b" / "%" / "schema.metapack",
                output / "schemas" / "b" / "%" / "health.metapack",
                output / "schemas" / "b" / "%" / "dependencies.metapack");
  EXPECT_ACTION(plan, 2, 1, 6, Bundle,
                output / "schemas" / "a" / "%" / "bundle.metapack",
                "https://example.com/a",
                output / "schemas" / "a" / "%" / "schema.metapack",
                output / "schemas" / "a" / "%" / "dependencies.metapack");
  EXPECT_ACTION(plan, 2, 2, 6, Health,
                output / "schemas" / "a" / "%" / "health.metapack",
                "https://example.com/a",
                output / "schemas" / "a" / "%" / "schema.metapack",
                output / "schemas" / "a" / "%" / "dependencies.metapack");
  EXPECT_ACTION(plan, 2, 3, 6, BlazeExhaustive,
                output / "schemas" / "b" / "%" / "blaze-exhaustive.metapack",
                "https://example.com/b",
                output / "schemas" / "b" / "%" / "bundle.metapack");
  EXPECT_ACTION(plan, 2, 4, 6, BlazeFast,
                output / "schemas" / "b" / "%" / "blaze-fast.metapack",
                "https://example.com/b",
                output / "schemas" / "b" / "%" / "bundle.metapack");
  EXPECT_ACTION(plan, 2, 5, 6, Editor,
                output / "schemas" / "b" / "%" / "editor.metapack",
                "https://example.com/b",
                output / "schemas" / "b" / "%" / "bundle.metapack");

  EXPECT_ACTION(plan, 3, 0, 5, SchemaMetadata,
                output / "explorer" / "a" / "%" / "schema.metapack",
                "https://example.com/a",
                output / "schemas" / "a" / "%" / "schema.metapack",
                output / "schemas" / "a" / "%" / "health.metapack",
                output / "schemas" / "a" / "%" / "dependencies.metapack");
  EXPECT_ACTION(plan, 3, 1, 5, WebSchema,
                output / "explorer" / "b" / "%" / "schema-html.metapack",
                "https://example.com/b",
                output / "explorer" / "b" / "%" / "schema.metapack",
                output / "schemas" / "b" / "%" / "health.metapack");
  EXPECT_ACTION(plan, 3, 2, 5, BlazeExhaustive,
                output / "schemas" / "a" / "%" / "blaze-exhaustive.metapack",
                "https://example.com/a",
                output / "schemas" / "a" / "%" / "bundle.metapack");
  EXPECT_ACTION(plan, 3, 3, 5, BlazeFast,
                output / "schemas" / "a" / "%" / "blaze-fast.metapack",
                "https://example.com/a",
                output / "schemas" / "a" / "%" / "bundle.metapack");
  EXPECT_ACTION(plan, 3, 4, 5, Editor,
                output / "schemas" / "a" / "%" / "editor.metapack",
                "https://example.com/a",
                output / "schemas" / "a" / "%" / "bundle.metapack");

  EXPECT_ACTION_UNORDERED(plan, 4, 0, 3, DirectoryList,
                          output / "explorer" / "%" / "directory.metapack", "",
                          output / "explorer" / "a" / "%" / "schema.metapack",
                          output / "explorer" / "b" / "%" / "schema.metapack");
  EXPECT_ACTION_UNORDERED(plan, 4, 1, 3, SearchIndex,
                          output / "explorer" / "%" / "search.metapack", "",
                          output / "explorer" / "a" / "%" / "schema.metapack",
                          output / "explorer" / "b" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 4, 2, 3, WebSchema,
                output / "explorer" / "a" / "%" / "schema-html.metapack",
                "https://example.com/a",
                output / "explorer" / "a" / "%" / "schema.metapack",
                output / "schemas" / "a" / "%" / "health.metapack");

  EXPECT_ACTION(plan, 5, 0, 1, WebIndex,
                output / "explorer" / "%" / "directory-html.metapack", "",
                output / "explorer" / "%" / "directory.metapack");

  EXPECT_ACTION(plan, 6, 0, 1, Routes, output / "routes.bin", "Full",
                output / "configuration.json");

  EXPECT_TOTAL_FILES(
      plan, entries, output / "version.json", output / "configuration.json",
      output / "schemas" / "a" / "%" / "schema.metapack",
      output / "schemas" / "a" / "%" / "dependencies.metapack",
      output / "schemas" / "a" / "%" / "locations.metapack",
      output / "schemas" / "a" / "%" / "positions.metapack",
      output / "schemas" / "a" / "%" / "stats.metapack",
      output / "schemas" / "a" / "%" / "bundle.metapack",
      output / "schemas" / "a" / "%" / "health.metapack",
      output / "schemas" / "a" / "%" / "blaze-exhaustive.metapack",
      output / "schemas" / "a" / "%" / "blaze-fast.metapack",
      output / "schemas" / "a" / "%" / "editor.metapack",
      output / "explorer" / "a" / "%" / "schema.metapack",
      output / "explorer" / "a" / "%" / "schema-html.metapack",
      output / "schemas" / "b" / "%" / "schema.metapack",
      output / "schemas" / "b" / "%" / "dependencies.metapack",
      output / "schemas" / "b" / "%" / "locations.metapack",
      output / "schemas" / "b" / "%" / "positions.metapack",
      output / "schemas" / "b" / "%" / "stats.metapack",
      output / "schemas" / "b" / "%" / "bundle.metapack",
      output / "schemas" / "b" / "%" / "health.metapack",
      output / "schemas" / "b" / "%" / "blaze-exhaustive.metapack",
      output / "schemas" / "b" / "%" / "blaze-fast.metapack",
      output / "schemas" / "b" / "%" / "editor.metapack",
      output / "explorer" / "b" / "%" / "schema.metapack",
      output / "explorer" / "b" / "%" / "schema-html.metapack",
      output / "explorer" / "%" / "search.metapack",
      output / "explorer" / "%" / "directory.metapack",
      output / "explorer" / "%" / "directory-html.metapack",
      output / "routes.bin");
}

TEST(Build_delta, incremental_evaluate_false_removes_existing_blaze) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  entries.emplace(output / "version.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "schemas" / "foo" / "%" /
                      "blaze-exhaustive.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "schemas" / "foo" / "%" / "blaze-fast.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "configuration.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  const sourcemeta::one::Resolver::Views schemas{
      {"https://example.com/foo", {"/src/foo.json", "foo", MTIME(100), false}}};
  const std::vector<std::filesystem::path> changed{"/src/foo.json"};
  const std::vector<std::filesystem::path> removed;
  const auto plan{sourcemeta::one::delta(sourcemeta::one::BuildPhase::Produce,
                                         sourcemeta::one::BuildPlan::Type::Full,
                                         entries, output, schemas, "1.0.0",
                                         true, "", changed, removed)};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, Full, 8, 16);

  EXPECT_ACTION(plan, 0, 0, 1, Materialise,
                output / "schemas" / "foo" / "%" / "schema.metapack",
                "https://example.com/foo",
                std::filesystem::path{"/"} / "src" / "foo.json",
                output / "configuration.json");

  EXPECT_ACTION(plan, 1, 0, 4, Dependencies,
                output / "schemas" / "foo" / "%" / "dependencies.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 1, 1, 4, Locations,
                output / "schemas" / "foo" / "%" / "locations.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 1, 2, 4, Positions,
                output / "schemas" / "foo" / "%" / "positions.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 1, 3, 4, Stats,
                output / "schemas" / "foo" / "%" / "stats.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");

  EXPECT_ACTION(plan, 2, 0, 2, Bundle,
                output / "schemas" / "foo" / "%" / "bundle.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "dependencies.metapack");
  EXPECT_ACTION(plan, 2, 1, 2, Health,
                output / "schemas" / "foo" / "%" / "health.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "dependencies.metapack");

  EXPECT_ACTION(plan, 3, 0, 2, SchemaMetadata,
                output / "explorer" / "foo" / "%" / "schema.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "health.metapack",
                output / "schemas" / "foo" / "%" / "dependencies.metapack");
  EXPECT_ACTION(plan, 3, 1, 2, Editor,
                output / "schemas" / "foo" / "%" / "editor.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "bundle.metapack");

  EXPECT_ACTION(plan, 4, 0, 3, DirectoryList,
                output / "explorer" / "%" / "directory.metapack", "",
                output / "explorer" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 4, 1, 3, SearchIndex,
                output / "explorer" / "%" / "search.metapack", "",
                output / "explorer" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 4, 2, 3, WebSchema,
                output / "explorer" / "foo" / "%" / "schema-html.metapack",
                "https://example.com/foo",
                output / "explorer" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "health.metapack");

  EXPECT_ACTION(plan, 5, 0, 1, WebIndex,
                output / "explorer" / "%" / "directory-html.metapack", "",
                output / "explorer" / "%" / "directory.metapack");

  EXPECT_ACTION(plan, 6, 0, 1, Routes, output / "routes.bin", "Full",
                output / "configuration.json");

  EXPECT_ACTION(plan, 7, 0, 2, Remove,
                output / "schemas" / "foo" / "%" / "blaze-exhaustive.metapack",
                "");
  EXPECT_ACTION(plan, 7, 1, 2, Remove,
                output / "schemas" / "foo" / "%" / "blaze-fast.metapack", "");

  EXPECT_TOTAL_FILES(plan, entries, output / "version.json",
                     output / "configuration.json",
                     output / "schemas" / "foo" / "%" / "schema.metapack",
                     output / "schemas" / "foo" / "%" / "dependencies.metapack",
                     output / "schemas" / "foo" / "%" / "locations.metapack",
                     output / "schemas" / "foo" / "%" / "positions.metapack",
                     output / "schemas" / "foo" / "%" / "stats.metapack",
                     output / "schemas" / "foo" / "%" / "bundle.metapack",
                     output / "schemas" / "foo" / "%" / "health.metapack",
                     output / "schemas" / "foo" / "%" / "editor.metapack",
                     output / "explorer" / "foo" / "%" / "schema.metapack",
                     output / "explorer" / "foo" / "%" / "schema-html.metapack",
                     output / "explorer" / "%" / "search.metapack",
                     output / "explorer" / "%" / "directory.metapack",
                     output / "explorer" / "%" / "directory-html.metapack",
                     output / "routes.bin");
}

TEST(Build_delta, headless_full_empty_registry) {
  const std::filesystem::path output{"/output"};
  const sourcemeta::one::BuildState entries;
  const sourcemeta::one::Resolver::Views schemas;
  const std::vector<std::filesystem::path> changed;
  const std::vector<std::filesystem::path> removed;
  const auto plan{sourcemeta::one::delta(
      sourcemeta::one::BuildPhase::Produce,
      sourcemeta::one::BuildPlan::Type::Headless, entries, output, schemas,
      "1.0.0", false, "", changed, removed)};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, Headless, 3, 5);

  EXPECT_ACTION(plan, 0, 0, 2, Configuration, output / "configuration.json",
                "");
  EXPECT_ACTION(plan, 0, 1, 2, Version, output / "version.json", "1.0.0");

  EXPECT_ACTION(plan, 1, 0, 2, DirectoryList,
                output / "explorer" / "%" / "directory.metapack", "");
  EXPECT_ACTION(plan, 1, 1, 2, SearchIndex,
                output / "explorer" / "%" / "search.metapack", "");

  EXPECT_ACTION(plan, 2, 0, 1, Routes, output / "routes.bin", "Headless",
                output / "configuration.json");

  EXPECT_TOTAL_FILES(
      plan, entries, output / "configuration.json", output / "version.json",
      output / "explorer" / "%" / "search.metapack",
      output / "explorer" / "%" / "directory.metapack", output / "routes.bin");
}

TEST(Build_delta, headless_full_single_schema) {
  const std::filesystem::path output{"/output"};
  const sourcemeta::one::BuildState entries;
  const sourcemeta::one::Resolver::Views schemas{
      {"https://example.com/foo", {"/src/foo.json", "foo", MTIME(100)}}};
  const std::vector<std::filesystem::path> changed;
  const std::vector<std::filesystem::path> removed;
  const auto plan{sourcemeta::one::delta(
      sourcemeta::one::BuildPhase::Produce,
      sourcemeta::one::BuildPlan::Type::Headless, entries, output, schemas,
      "1.0.0", false, "", changed, removed)};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, Headless, 7, 16);

  EXPECT_ACTION(plan, 0, 0, 2, Configuration, output / "configuration.json",
                "");
  EXPECT_ACTION(plan, 0, 1, 2, Version, output / "version.json", "1.0.0");

  EXPECT_ACTION(plan, 1, 0, 1, Materialise,
                output / "schemas" / "foo" / "%" / "schema.metapack",
                "https://example.com/foo",
                std::filesystem::path{"/"} / "src" / "foo.json",
                output / "configuration.json");

  EXPECT_ACTION(plan, 2, 0, 4, Dependencies,
                output / "schemas" / "foo" / "%" / "dependencies.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 2, 1, 4, Locations,
                output / "schemas" / "foo" / "%" / "locations.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 2, 2, 4, Positions,
                output / "schemas" / "foo" / "%" / "positions.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 2, 3, 4, Stats,
                output / "schemas" / "foo" / "%" / "stats.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");

  EXPECT_ACTION(plan, 3, 0, 2, Bundle,
                output / "schemas" / "foo" / "%" / "bundle.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "dependencies.metapack");
  EXPECT_ACTION(plan, 3, 1, 2, Health,
                output / "schemas" / "foo" / "%" / "health.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "dependencies.metapack");

  EXPECT_ACTION(plan, 4, 0, 4, SchemaMetadata,
                output / "explorer" / "foo" / "%" / "schema.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "health.metapack",
                output / "schemas" / "foo" / "%" / "dependencies.metapack");
  EXPECT_ACTION(plan, 4, 1, 4, BlazeExhaustive,
                output / "schemas" / "foo" / "%" / "blaze-exhaustive.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "bundle.metapack");
  EXPECT_ACTION(plan, 4, 2, 4, BlazeFast,
                output / "schemas" / "foo" / "%" / "blaze-fast.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "bundle.metapack");
  EXPECT_ACTION(plan, 4, 3, 4, Editor,
                output / "schemas" / "foo" / "%" / "editor.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "bundle.metapack");

  EXPECT_ACTION(plan, 5, 0, 2, DirectoryList,
                output / "explorer" / "%" / "directory.metapack", "",
                output / "explorer" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 5, 1, 2, SearchIndex,
                output / "explorer" / "%" / "search.metapack", "",
                output / "explorer" / "foo" / "%" / "schema.metapack");

  EXPECT_ACTION(plan, 6, 0, 1, Routes, output / "routes.bin", "Headless",
                output / "configuration.json");

  EXPECT_TOTAL_FILES(
      plan, entries, output / "configuration.json", output / "version.json",
      output / "schemas" / "foo" / "%" / "schema.metapack",
      output / "schemas" / "foo" / "%" / "dependencies.metapack",
      output / "schemas" / "foo" / "%" / "locations.metapack",
      output / "schemas" / "foo" / "%" / "positions.metapack",
      output / "schemas" / "foo" / "%" / "stats.metapack",
      output / "schemas" / "foo" / "%" / "bundle.metapack",
      output / "schemas" / "foo" / "%" / "health.metapack",
      output / "schemas" / "foo" / "%" / "blaze-exhaustive.metapack",
      output / "schemas" / "foo" / "%" / "blaze-fast.metapack",
      output / "schemas" / "foo" / "%" / "editor.metapack",
      output / "explorer" / "foo" / "%" / "schema.metapack",
      output / "explorer" / "%" / "search.metapack",
      output / "explorer" / "%" / "directory.metapack", output / "routes.bin");
}

TEST(Build_delta, headless_incremental) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  entries.emplace(output / "version.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "configuration.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  const sourcemeta::one::Resolver::Views schemas{
      {"https://example.com/foo", {"/src/foo.json", "foo", MTIME(100)}}};
  const std::vector<std::filesystem::path> changed{"/src/foo.json"};
  const std::vector<std::filesystem::path> removed;
  const auto plan{sourcemeta::one::delta(
      sourcemeta::one::BuildPhase::Produce,
      sourcemeta::one::BuildPlan::Type::Headless, entries, output, schemas,
      "1.0.0", true, "", changed, removed)};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, Headless, 5, 13);

  EXPECT_ACTION(plan, 0, 0, 1, Materialise,
                output / "schemas" / "foo" / "%" / "schema.metapack",
                "https://example.com/foo",
                std::filesystem::path{"/"} / "src" / "foo.json",
                output / "configuration.json");

  EXPECT_ACTION(plan, 1, 0, 4, Dependencies,
                output / "schemas" / "foo" / "%" / "dependencies.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 1, 1, 4, Locations,
                output / "schemas" / "foo" / "%" / "locations.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 1, 2, 4, Positions,
                output / "schemas" / "foo" / "%" / "positions.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 1, 3, 4, Stats,
                output / "schemas" / "foo" / "%" / "stats.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");

  EXPECT_ACTION(plan, 2, 0, 2, Bundle,
                output / "schemas" / "foo" / "%" / "bundle.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "dependencies.metapack");
  EXPECT_ACTION(plan, 2, 1, 2, Health,
                output / "schemas" / "foo" / "%" / "health.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "dependencies.metapack");

  EXPECT_ACTION(plan, 3, 0, 4, SchemaMetadata,
                output / "explorer" / "foo" / "%" / "schema.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "health.metapack",
                output / "schemas" / "foo" / "%" / "dependencies.metapack");
  EXPECT_ACTION(plan, 3, 1, 4, BlazeExhaustive,
                output / "schemas" / "foo" / "%" / "blaze-exhaustive.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "bundle.metapack");
  EXPECT_ACTION(plan, 3, 2, 4, BlazeFast,
                output / "schemas" / "foo" / "%" / "blaze-fast.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "bundle.metapack");
  EXPECT_ACTION(plan, 3, 3, 4, Editor,
                output / "schemas" / "foo" / "%" / "editor.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "bundle.metapack");

  EXPECT_ACTION(plan, 4, 0, 2, DirectoryList,
                output / "explorer" / "%" / "directory.metapack", "",
                output / "explorer" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 4, 1, 2, SearchIndex,
                output / "explorer" / "%" / "search.metapack", "",
                output / "explorer" / "foo" / "%" / "schema.metapack");

  EXPECT_TOTAL_FILES(
      plan, entries, output / "version.json", output / "configuration.json",
      output / "schemas" / "foo" / "%" / "schema.metapack",
      output / "schemas" / "foo" / "%" / "dependencies.metapack",
      output / "schemas" / "foo" / "%" / "locations.metapack",
      output / "schemas" / "foo" / "%" / "positions.metapack",
      output / "schemas" / "foo" / "%" / "stats.metapack",
      output / "schemas" / "foo" / "%" / "bundle.metapack",
      output / "schemas" / "foo" / "%" / "health.metapack",
      output / "schemas" / "foo" / "%" / "blaze-exhaustive.metapack",
      output / "schemas" / "foo" / "%" / "blaze-fast.metapack",
      output / "schemas" / "foo" / "%" / "editor.metapack",
      output / "explorer" / "foo" / "%" / "schema.metapack",
      output / "explorer" / "%" / "search.metapack",
      output / "explorer" / "%" / "directory.metapack");
}

TEST(Build_delta, full_to_headless_removes_web) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  entries.emplace(output / "version.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "schemas" / "foo" / "%" / "schema.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "directory-html.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "404.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "foo" / "%" / "directory-html.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "foo" / "%" / "schema-html.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "routes.bin",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "configuration.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  const sourcemeta::one::Resolver::Views schemas{
      {"https://example.com/foo", {"/src/foo.json", "foo", MTIME(200)}}};
  const std::vector<std::filesystem::path> changed{"/src/foo.json"};
  const std::vector<std::filesystem::path> removed;
  const auto plan{sourcemeta::one::delta(
      sourcemeta::one::BuildPhase::Produce,
      sourcemeta::one::BuildPlan::Type::Headless, entries, output, schemas,
      "1.0.0", true, "", changed, removed)};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, Headless, 7, 18);

  EXPECT_ACTION(plan, 0, 0, 1, Materialise,
                output / "schemas" / "foo" / "%" / "schema.metapack",
                "https://example.com/foo",
                std::filesystem::path{"/"} / "src" / "foo.json",
                output / "configuration.json");

  EXPECT_ACTION(plan, 1, 0, 4, Dependencies,
                output / "schemas" / "foo" / "%" / "dependencies.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 1, 1, 4, Locations,
                output / "schemas" / "foo" / "%" / "locations.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 1, 2, 4, Positions,
                output / "schemas" / "foo" / "%" / "positions.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 1, 3, 4, Stats,
                output / "schemas" / "foo" / "%" / "stats.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");

  EXPECT_ACTION(plan, 2, 0, 2, Bundle,
                output / "schemas" / "foo" / "%" / "bundle.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "dependencies.metapack");
  EXPECT_ACTION(plan, 2, 1, 2, Health,
                output / "schemas" / "foo" / "%" / "health.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "dependencies.metapack");

  EXPECT_ACTION(plan, 3, 0, 4, SchemaMetadata,
                output / "explorer" / "foo" / "%" / "schema.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "health.metapack",
                output / "schemas" / "foo" / "%" / "dependencies.metapack");
  EXPECT_ACTION(plan, 3, 1, 4, BlazeExhaustive,
                output / "schemas" / "foo" / "%" / "blaze-exhaustive.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "bundle.metapack");
  EXPECT_ACTION(plan, 3, 2, 4, BlazeFast,
                output / "schemas" / "foo" / "%" / "blaze-fast.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "bundle.metapack");
  EXPECT_ACTION(plan, 3, 3, 4, Editor,
                output / "schemas" / "foo" / "%" / "editor.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "bundle.metapack");

  EXPECT_ACTION(plan, 4, 0, 2, DirectoryList,
                output / "explorer" / "%" / "directory.metapack", "",
                output / "explorer" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 4, 1, 2, SearchIndex,
                output / "explorer" / "%" / "search.metapack", "",
                output / "explorer" / "foo" / "%" / "schema.metapack");

  EXPECT_ACTION(plan, 5, 0, 1, Routes, output / "routes.bin", "Headless",
                output / "configuration.json");

  EXPECT_ACTION(plan, 6, 0, 4, Remove,
                output / "explorer" / "%" / "404.metapack", "");
  EXPECT_ACTION(plan, 6, 1, 4, Remove,
                output / "explorer" / "%" / "directory-html.metapack", "");
  EXPECT_ACTION(plan, 6, 2, 4, Remove,
                output / "explorer" / "foo" / "%" / "directory-html.metapack",
                "");
  EXPECT_ACTION(plan, 6, 3, 4, Remove,
                output / "explorer" / "foo" / "%" / "schema-html.metapack", "");

  EXPECT_TOTAL_FILES(
      plan, entries, output / "version.json", output / "configuration.json",
      output / "schemas" / "foo" / "%" / "schema.metapack",
      output / "schemas" / "foo" / "%" / "dependencies.metapack",
      output / "schemas" / "foo" / "%" / "locations.metapack",
      output / "schemas" / "foo" / "%" / "positions.metapack",
      output / "schemas" / "foo" / "%" / "stats.metapack",
      output / "schemas" / "foo" / "%" / "bundle.metapack",
      output / "schemas" / "foo" / "%" / "health.metapack",
      output / "schemas" / "foo" / "%" / "blaze-exhaustive.metapack",
      output / "schemas" / "foo" / "%" / "blaze-fast.metapack",
      output / "schemas" / "foo" / "%" / "editor.metapack",
      output / "explorer" / "foo" / "%" / "schema.metapack",
      output / "explorer" / "%" / "search.metapack",
      output / "explorer" / "%" / "directory.metapack", output / "routes.bin");
}

TEST(Build_delta, full_to_headless_no_change_removes_web) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  entries.emplace(output / "version.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "configuration.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "routes.bin",
                  {.file_mark = MTIME(100), .dependencies = {}});
  ADD_SCHEMA_ENTRIES(entries, output, "foo", true, true, MTIME(100));
  entries.emplace(output / "explorer" / "%" / "directory.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "directory-html.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "search.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "404.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  const sourcemeta::one::Resolver::Views schemas{
      {"https://example.com/foo",
       {.path = "/src/foo.json", .relative_path = "foo", .mtime = MTIME(100)}}};
  const std::vector<std::filesystem::path> changed;
  const std::vector<std::filesystem::path> removed;
  const auto plan{sourcemeta::one::delta(
      sourcemeta::one::BuildPhase::Produce,
      sourcemeta::one::BuildPlan::Type::Headless, entries, output, schemas,
      "1.0.0", true, "", changed, removed)};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, Headless, 2, 5);

  EXPECT_ACTION(plan, 0, 0, 1, Routes, output / "routes.bin", "Headless",
                output / "configuration.json");

  EXPECT_ACTION_UNORDERED(plan, 1, 0, 4, Remove,
                          output / "explorer" / "%" / "404.metapack", "");
  EXPECT_ACTION_UNORDERED(plan, 1, 1, 4, Remove,
                          output / "explorer" / "%" / "directory-html.metapack",
                          "");
  EXPECT_ACTION_UNORDERED(
      plan, 1, 2, 4, Remove,
      output / "explorer" / "foo" / "%" / "directory-html.metapack", "");
  EXPECT_ACTION_UNORDERED(
      plan, 1, 3, 4, Remove,
      output / "explorer" / "foo" / "%" / "schema-html.metapack", "");

  EXPECT_TOTAL_FILES(
      plan, entries, output / "version.json", output / "configuration.json",
      output / "schemas" / "foo" / "%" / "schema.metapack",
      output / "schemas" / "foo" / "%" / "dependencies.metapack",
      output / "schemas" / "foo" / "%" / "locations.metapack",
      output / "schemas" / "foo" / "%" / "positions.metapack",
      output / "schemas" / "foo" / "%" / "stats.metapack",
      output / "schemas" / "foo" / "%" / "bundle.metapack",
      output / "schemas" / "foo" / "%" / "health.metapack",
      output / "schemas" / "foo" / "%" / "blaze-exhaustive.metapack",
      output / "schemas" / "foo" / "%" / "blaze-fast.metapack",
      output / "schemas" / "foo" / "%" / "editor.metapack",
      output / "explorer" / "foo" / "%" / "schema.metapack",
      output / "explorer" / "%" / "search.metapack",
      output / "explorer" / "%" / "directory.metapack", output / "routes.bin");
}

TEST(Build_delta, headless_to_full_incremental) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  entries.emplace(output / "version.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "configuration.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  const sourcemeta::one::Resolver::Views schemas{
      {"https://example.com/foo", {"/src/foo.json", "foo", MTIME(100)}}};
  const std::vector<std::filesystem::path> changed{"/src/foo.json"};
  const std::vector<std::filesystem::path> removed;
  const auto plan{sourcemeta::one::delta(sourcemeta::one::BuildPhase::Produce,
                                         sourcemeta::one::BuildPlan::Type::Full,
                                         entries, output, schemas, "1.0.0",
                                         true, "", changed, removed)};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, Full, 7, 16);

  EXPECT_ACTION(plan, 0, 0, 1, Materialise,
                output / "schemas" / "foo" / "%" / "schema.metapack",
                "https://example.com/foo",
                std::filesystem::path{"/"} / "src" / "foo.json",
                output / "configuration.json");

  EXPECT_ACTION(plan, 1, 0, 4, Dependencies,
                output / "schemas" / "foo" / "%" / "dependencies.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 1, 1, 4, Locations,
                output / "schemas" / "foo" / "%" / "locations.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 1, 2, 4, Positions,
                output / "schemas" / "foo" / "%" / "positions.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 1, 3, 4, Stats,
                output / "schemas" / "foo" / "%" / "stats.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");

  EXPECT_ACTION(plan, 2, 0, 2, Bundle,
                output / "schemas" / "foo" / "%" / "bundle.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "dependencies.metapack");
  EXPECT_ACTION(plan, 2, 1, 2, Health,
                output / "schemas" / "foo" / "%" / "health.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "dependencies.metapack");

  EXPECT_ACTION(plan, 3, 0, 4, SchemaMetadata,
                output / "explorer" / "foo" / "%" / "schema.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "health.metapack",
                output / "schemas" / "foo" / "%" / "dependencies.metapack");
  EXPECT_ACTION(plan, 3, 1, 4, BlazeExhaustive,
                output / "schemas" / "foo" / "%" / "blaze-exhaustive.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "bundle.metapack");
  EXPECT_ACTION(plan, 3, 2, 4, BlazeFast,
                output / "schemas" / "foo" / "%" / "blaze-fast.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "bundle.metapack");
  EXPECT_ACTION(plan, 3, 3, 4, Editor,
                output / "schemas" / "foo" / "%" / "editor.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "bundle.metapack");

  EXPECT_ACTION(plan, 4, 0, 3, DirectoryList,
                output / "explorer" / "%" / "directory.metapack", "",
                output / "explorer" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 4, 1, 3, SearchIndex,
                output / "explorer" / "%" / "search.metapack", "",
                output / "explorer" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 4, 2, 3, WebSchema,
                output / "explorer" / "foo" / "%" / "schema-html.metapack",
                "https://example.com/foo",
                output / "explorer" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "health.metapack");

  EXPECT_ACTION(plan, 5, 0, 1, WebIndex,
                output / "explorer" / "%" / "directory-html.metapack", "",
                output / "explorer" / "%" / "directory.metapack");

  EXPECT_ACTION(plan, 6, 0, 1, Routes, output / "routes.bin", "Full",
                output / "configuration.json");

  EXPECT_TOTAL_FILES(
      plan, entries, output / "version.json", output / "configuration.json",
      output / "schemas" / "foo" / "%" / "schema.metapack",
      output / "schemas" / "foo" / "%" / "dependencies.metapack",
      output / "schemas" / "foo" / "%" / "locations.metapack",
      output / "schemas" / "foo" / "%" / "positions.metapack",
      output / "schemas" / "foo" / "%" / "stats.metapack",
      output / "schemas" / "foo" / "%" / "bundle.metapack",
      output / "schemas" / "foo" / "%" / "health.metapack",
      output / "schemas" / "foo" / "%" / "blaze-exhaustive.metapack",
      output / "schemas" / "foo" / "%" / "blaze-fast.metapack",
      output / "schemas" / "foo" / "%" / "editor.metapack",
      output / "explorer" / "foo" / "%" / "schema.metapack",
      output / "explorer" / "foo" / "%" / "schema-html.metapack",
      output / "explorer" / "%" / "search.metapack",
      output / "explorer" / "%" / "directory.metapack",
      output / "explorer" / "%" / "directory-html.metapack",
      output / "routes.bin");
}

TEST(Build_delta, headless_to_full_full_rebuild) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  entries.emplace(output / "configuration.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "version.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "schemas" / "foo" / "%" / "schema.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "schemas" / "foo" / "%" / "dependencies.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "schemas" / "foo" / "%" / "locations.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "schemas" / "foo" / "%" / "positions.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "schemas" / "foo" / "%" / "stats.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "schemas" / "foo" / "%" / "bundle.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "schemas" / "foo" / "%" / "health.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "schemas" / "foo" / "%" / "editor.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "schemas" / "foo" / "%" /
                      "blaze-exhaustive.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "schemas" / "foo" / "%" / "blaze-fast.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "foo" / "%" / "schema.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "search.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "foo" / "%" / "directory.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "directory.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "routes.bin",
                  {.file_mark = MTIME(100), .dependencies = {}});
  const sourcemeta::one::Resolver::Views schemas{
      {"https://example.com/foo", {"/src/foo.json", "foo", MTIME(200)}}};
  const std::vector<std::filesystem::path> changed;
  const std::vector<std::filesystem::path> removed;
  const auto plan{sourcemeta::one::delta(sourcemeta::one::BuildPhase::Produce,
                                         sourcemeta::one::BuildPlan::Type::Full,
                                         entries, output, schemas, "1.0.0",
                                         true, "", changed, removed)};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, Full, 7, 16);

  EXPECT_ACTION(plan, 0, 0, 1, Materialise,
                output / "schemas" / "foo" / "%" / "schema.metapack",
                "https://example.com/foo",
                std::filesystem::path{"/"} / "src" / "foo.json",
                output / "configuration.json");

  EXPECT_ACTION(plan, 1, 0, 4, Dependencies,
                output / "schemas" / "foo" / "%" / "dependencies.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 1, 1, 4, Locations,
                output / "schemas" / "foo" / "%" / "locations.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 1, 2, 4, Positions,
                output / "schemas" / "foo" / "%" / "positions.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 1, 3, 4, Stats,
                output / "schemas" / "foo" / "%" / "stats.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");

  EXPECT_ACTION(plan, 2, 0, 2, Bundle,
                output / "schemas" / "foo" / "%" / "bundle.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "dependencies.metapack");
  EXPECT_ACTION(plan, 2, 1, 2, Health,
                output / "schemas" / "foo" / "%" / "health.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "dependencies.metapack");

  EXPECT_ACTION(plan, 3, 0, 4, SchemaMetadata,
                output / "explorer" / "foo" / "%" / "schema.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "health.metapack",
                output / "schemas" / "foo" / "%" / "dependencies.metapack");
  EXPECT_ACTION(plan, 3, 1, 4, BlazeExhaustive,
                output / "schemas" / "foo" / "%" / "blaze-exhaustive.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "bundle.metapack");
  EXPECT_ACTION(plan, 3, 2, 4, BlazeFast,
                output / "schemas" / "foo" / "%" / "blaze-fast.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "bundle.metapack");
  EXPECT_ACTION(plan, 3, 3, 4, Editor,
                output / "schemas" / "foo" / "%" / "editor.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "bundle.metapack");

  EXPECT_ACTION(plan, 4, 0, 3, DirectoryList,
                output / "explorer" / "%" / "directory.metapack", "",
                output / "explorer" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 4, 1, 3, SearchIndex,
                output / "explorer" / "%" / "search.metapack", "",
                output / "explorer" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 4, 2, 3, WebSchema,
                output / "explorer" / "foo" / "%" / "schema-html.metapack",
                "https://example.com/foo",
                output / "explorer" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "health.metapack");

  EXPECT_ACTION(plan, 5, 0, 1, WebIndex,
                output / "explorer" / "%" / "directory-html.metapack", "",
                output / "explorer" / "%" / "directory.metapack");

  EXPECT_ACTION(plan, 6, 0, 1, Routes, output / "routes.bin", "Full",
                output / "configuration.json");

  EXPECT_TOTAL_FILES(
      plan, entries, output / "version.json", output / "configuration.json",
      output / "schemas" / "foo" / "%" / "schema.metapack",
      output / "schemas" / "foo" / "%" / "dependencies.metapack",
      output / "schemas" / "foo" / "%" / "locations.metapack",
      output / "schemas" / "foo" / "%" / "positions.metapack",
      output / "schemas" / "foo" / "%" / "stats.metapack",
      output / "schemas" / "foo" / "%" / "bundle.metapack",
      output / "schemas" / "foo" / "%" / "health.metapack",
      output / "schemas" / "foo" / "%" / "blaze-exhaustive.metapack",
      output / "schemas" / "foo" / "%" / "blaze-fast.metapack",
      output / "schemas" / "foo" / "%" / "editor.metapack",
      output / "explorer" / "foo" / "%" / "schema.metapack",
      output / "explorer" / "foo" / "%" / "schema-html.metapack",
      output / "explorer" / "foo" / "%" / "directory.metapack",
      output / "explorer" / "%" / "search.metapack",
      output / "explorer" / "%" / "directory.metapack",
      output / "explorer" / "%" / "directory-html.metapack",
      output / "routes.bin");
}

TEST(Build_delta, full_to_headless_full_rebuild) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;
  entries.emplace(output / "version.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "configuration.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "directory-html.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "404.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "foo" / "%" / "schema-html.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  const sourcemeta::one::Resolver::Views schemas{
      {"https://example.com/foo", {"/src/foo.json", "foo", MTIME(100)}}};
  const std::vector<std::filesystem::path> changed;
  const std::vector<std::filesystem::path> removed;
  const auto plan{sourcemeta::one::delta(
      sourcemeta::one::BuildPhase::Produce,
      sourcemeta::one::BuildPlan::Type::Headless, entries, output, schemas,
      "2.0.0", false, "", changed, removed)};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, Headless, 8, 19);

  EXPECT_ACTION(plan, 0, 0, 2, Configuration, output / "configuration.json",
                "");
  EXPECT_ACTION(plan, 0, 1, 2, Version, output / "version.json", "2.0.0");

  EXPECT_ACTION(plan, 1, 0, 1, Materialise,
                output / "schemas" / "foo" / "%" / "schema.metapack",
                "https://example.com/foo",
                std::filesystem::path{"/"} / "src" / "foo.json",
                output / "configuration.json");

  EXPECT_ACTION(plan, 2, 0, 4, Dependencies,
                output / "schemas" / "foo" / "%" / "dependencies.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 2, 1, 4, Locations,
                output / "schemas" / "foo" / "%" / "locations.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 2, 2, 4, Positions,
                output / "schemas" / "foo" / "%" / "positions.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 2, 3, 4, Stats,
                output / "schemas" / "foo" / "%" / "stats.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");

  EXPECT_ACTION(plan, 3, 0, 2, Bundle,
                output / "schemas" / "foo" / "%" / "bundle.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "dependencies.metapack");
  EXPECT_ACTION(plan, 3, 1, 2, Health,
                output / "schemas" / "foo" / "%" / "health.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "dependencies.metapack");

  EXPECT_ACTION(plan, 4, 0, 4, SchemaMetadata,
                output / "explorer" / "foo" / "%" / "schema.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "health.metapack",
                output / "schemas" / "foo" / "%" / "dependencies.metapack");
  EXPECT_ACTION(plan, 4, 1, 4, BlazeExhaustive,
                output / "schemas" / "foo" / "%" / "blaze-exhaustive.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "bundle.metapack");
  EXPECT_ACTION(plan, 4, 2, 4, BlazeFast,
                output / "schemas" / "foo" / "%" / "blaze-fast.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "bundle.metapack");
  EXPECT_ACTION(plan, 4, 3, 4, Editor,
                output / "schemas" / "foo" / "%" / "editor.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "bundle.metapack");

  EXPECT_ACTION(plan, 5, 0, 2, DirectoryList,
                output / "explorer" / "%" / "directory.metapack", "",
                output / "explorer" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 5, 1, 2, SearchIndex,
                output / "explorer" / "%" / "search.metapack", "",
                output / "explorer" / "foo" / "%" / "schema.metapack");

  EXPECT_ACTION(plan, 6, 0, 1, Routes, output / "routes.bin", "Headless",
                output / "configuration.json");

  EXPECT_ACTION(plan, 7, 0, 3, Remove,
                output / "explorer" / "%" / "404.metapack", "");
  EXPECT_ACTION(plan, 7, 1, 3, Remove,
                output / "explorer" / "%" / "directory-html.metapack", "");
  EXPECT_ACTION(plan, 7, 2, 3, Remove,
                output / "explorer" / "foo" / "%" / "schema-html.metapack", "");

  EXPECT_TOTAL_FILES(
      plan, entries, output / "configuration.json", output / "version.json",
      output / "schemas" / "foo" / "%" / "schema.metapack",
      output / "schemas" / "foo" / "%" / "dependencies.metapack",
      output / "schemas" / "foo" / "%" / "locations.metapack",
      output / "schemas" / "foo" / "%" / "positions.metapack",
      output / "schemas" / "foo" / "%" / "stats.metapack",
      output / "schemas" / "foo" / "%" / "bundle.metapack",
      output / "schemas" / "foo" / "%" / "health.metapack",
      output / "schemas" / "foo" / "%" / "blaze-exhaustive.metapack",
      output / "schemas" / "foo" / "%" / "blaze-fast.metapack",
      output / "schemas" / "foo" / "%" / "editor.metapack",
      output / "explorer" / "foo" / "%" / "schema.metapack",
      output / "explorer" / "%" / "search.metapack",
      output / "explorer" / "%" / "directory.metapack", output / "routes.bin");
}

TEST(Build_delta, full_single_schema_nested_path_headless) {
  const std::filesystem::path output{"/output"};
  const sourcemeta::one::BuildState entries;
  const sourcemeta::one::Resolver::Views schemas{
      {"https://example.com/test",
       {"/src/test.json", "example/test", MTIME(100)}}};
  const std::vector<std::filesystem::path> changed;
  const std::vector<std::filesystem::path> removed;
  const auto plan{sourcemeta::one::delta(
      sourcemeta::one::BuildPhase::Produce,
      sourcemeta::one::BuildPlan::Type::Headless, entries, output, schemas,
      "1.0.0", false, "", changed, removed)};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, Headless, 8, 17);

  EXPECT_ACTION(plan, 0, 0, 2, Configuration, output / "configuration.json",
                "");
  EXPECT_ACTION(plan, 0, 1, 2, Version, output / "version.json", "1.0.0");

  EXPECT_ACTION(plan, 1, 0, 1, Materialise,
                output / "schemas" / "example" / "test" / "%" /
                    "schema.metapack",
                "https://example.com/test",
                std::filesystem::path{"/"} / "src" / "test.json",
                output / "configuration.json");

  EXPECT_ACTION(
      plan, 2, 0, 4, Dependencies,
      output / "schemas" / "example" / "test" / "%" / "dependencies.metapack",
      "https://example.com/test",
      output / "schemas" / "example" / "test" / "%" / "schema.metapack");
  EXPECT_ACTION(
      plan, 2, 1, 4, Locations,
      output / "schemas" / "example" / "test" / "%" / "locations.metapack",
      "https://example.com/test",
      output / "schemas" / "example" / "test" / "%" / "schema.metapack");
  EXPECT_ACTION(
      plan, 2, 2, 4, Positions,
      output / "schemas" / "example" / "test" / "%" / "positions.metapack",
      "https://example.com/test",
      output / "schemas" / "example" / "test" / "%" / "schema.metapack");
  EXPECT_ACTION(
      plan, 2, 3, 4, Stats,
      output / "schemas" / "example" / "test" / "%" / "stats.metapack",
      "https://example.com/test",
      output / "schemas" / "example" / "test" / "%" / "schema.metapack");

  EXPECT_ACTION(
      plan, 3, 0, 2, Bundle,
      output / "schemas" / "example" / "test" / "%" / "bundle.metapack",
      "https://example.com/test",
      output / "schemas" / "example" / "test" / "%" / "schema.metapack",
      output / "schemas" / "example" / "test" / "%" / "dependencies.metapack");
  EXPECT_ACTION(
      plan, 3, 1, 2, Health,
      output / "schemas" / "example" / "test" / "%" / "health.metapack",
      "https://example.com/test",
      output / "schemas" / "example" / "test" / "%" / "schema.metapack",
      output / "schemas" / "example" / "test" / "%" / "dependencies.metapack");

  EXPECT_ACTION(
      plan, 4, 0, 4, SchemaMetadata,
      output / "explorer" / "example" / "test" / "%" / "schema.metapack",
      "https://example.com/test",
      output / "schemas" / "example" / "test" / "%" / "schema.metapack",
      output / "schemas" / "example" / "test" / "%" / "health.metapack",
      output / "schemas" / "example" / "test" / "%" / "dependencies.metapack");
  EXPECT_ACTION(plan, 4, 1, 4, BlazeExhaustive,
                output / "schemas" / "example" / "test" / "%" /
                    "blaze-exhaustive.metapack",
                "https://example.com/test",
                output / "schemas" / "example" / "test" / "%" /
                    "bundle.metapack");
  EXPECT_ACTION(
      plan, 4, 2, 4, BlazeFast,
      output / "schemas" / "example" / "test" / "%" / "blaze-fast.metapack",
      "https://example.com/test",
      output / "schemas" / "example" / "test" / "%" / "bundle.metapack");
  EXPECT_ACTION(
      plan, 4, 3, 4, Editor,
      output / "schemas" / "example" / "test" / "%" / "editor.metapack",
      "https://example.com/test",
      output / "schemas" / "example" / "test" / "%" / "bundle.metapack");

  EXPECT_ACTION(
      plan, 5, 0, 2, SearchIndex, output / "explorer" / "%" / "search.metapack",
      "", output / "explorer" / "example" / "test" / "%" / "schema.metapack");
  EXPECT_ACTION(
      plan, 5, 1, 2, DirectoryList,
      output / "explorer" / "example" / "%" / "directory.metapack", "",
      output / "explorer" / "example" / "test" / "%" / "schema.metapack");

  EXPECT_ACTION(plan, 6, 0, 1, DirectoryList,
                output / "explorer" / "%" / "directory.metapack", "",
                output / "explorer" / "example" / "%" / "directory.metapack");

  EXPECT_ACTION(plan, 7, 0, 1, Routes, output / "routes.bin", "Headless",
                output / "configuration.json");

  EXPECT_TOTAL_FILES(
      plan, entries, output / "configuration.json", output / "version.json",
      output / "schemas" / "example" / "test" / "%" / "schema.metapack",
      output / "schemas" / "example" / "test" / "%" / "dependencies.metapack",
      output / "schemas" / "example" / "test" / "%" / "locations.metapack",
      output / "schemas" / "example" / "test" / "%" / "positions.metapack",
      output / "schemas" / "example" / "test" / "%" / "stats.metapack",
      output / "schemas" / "example" / "test" / "%" / "bundle.metapack",
      output / "schemas" / "example" / "test" / "%" / "health.metapack",
      output / "schemas" / "example" / "test" / "%" /
          "blaze-exhaustive.metapack",
      output / "schemas" / "example" / "test" / "%" / "blaze-fast.metapack",
      output / "schemas" / "example" / "test" / "%" / "editor.metapack",
      output / "explorer" / "example" / "test" / "%" / "schema.metapack",
      output / "explorer" / "%" / "search.metapack",
      output / "explorer" / "%" / "directory.metapack",
      output / "explorer" / "example" / "%" / "directory.metapack",
      output / "routes.bin");
}

TEST(Build_delta, incremental_add_schema_preserves_intermediate_dirs) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;

  entries.emplace(output / "version.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "configuration.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "routes.bin",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "search.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "directory.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "404.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "directory-html.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "example" / "%" / "directory.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "example" / "%" /
                      "directory-html.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "example" / "schemas" / "%" /
                      "directory.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "example" / "schemas" / "%" /
                      "directory-html.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  ADD_SCHEMA_ENTRIES(entries, output, "example/schemas/a", true, true,
                     MTIME(100));
  ADD_SCHEMA_ENTRIES(entries, output, "example/schemas/b", true, true,
                     MTIME(100));

  const sourcemeta::one::Resolver::Views schemas{
      {"https://example.com/a",
       {"/src/a.json", "example/schemas/a", MTIME(100)}},
      {"https://example.com/b",
       {"/src/b.json", "example/schemas/b", MTIME(100)}},
      {"https://example.com/c",
       {"/src/c.json", "example/schemas/c", MTIME(200)}}};
  const std::vector<std::filesystem::path> changed{"/src/c.json"};
  const std::vector<std::filesystem::path> removed;
  const auto plan{sourcemeta::one::delta(sourcemeta::one::BuildPhase::Produce,
                                         sourcemeta::one::BuildPlan::Type::Full,
                                         entries, output, schemas, "1.0.0",
                                         true, "", changed, removed)};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, Full, 8, 19);

  EXPECT_ACTION(plan, 0, 0, 1, Materialise,
                output / "schemas" / "example" / "schemas" / "c" / "%" /
                    "schema.metapack",
                "https://example.com/c",
                std::filesystem::path{"/"} / "src" / "c.json",
                output / "configuration.json");

  EXPECT_ACTION(plan, 1, 0, 4, Dependencies,
                output / "schemas" / "example" / "schemas" / "c" / "%" /
                    "dependencies.metapack",
                "https://example.com/c",
                output / "schemas" / "example" / "schemas" / "c" / "%" /
                    "schema.metapack");
  EXPECT_ACTION(plan, 1, 1, 4, Locations,
                output / "schemas" / "example" / "schemas" / "c" / "%" /
                    "locations.metapack",
                "https://example.com/c",
                output / "schemas" / "example" / "schemas" / "c" / "%" /
                    "schema.metapack");
  EXPECT_ACTION(plan, 1, 2, 4, Positions,
                output / "schemas" / "example" / "schemas" / "c" / "%" /
                    "positions.metapack",
                "https://example.com/c",
                output / "schemas" / "example" / "schemas" / "c" / "%" /
                    "schema.metapack");
  EXPECT_ACTION(plan, 1, 3, 4, Stats,
                output / "schemas" / "example" / "schemas" / "c" / "%" /
                    "stats.metapack",
                "https://example.com/c",
                output / "schemas" / "example" / "schemas" / "c" / "%" /
                    "schema.metapack");

  EXPECT_ACTION(plan, 2, 0, 2, Bundle,
                output / "schemas" / "example" / "schemas" / "c" / "%" /
                    "bundle.metapack",
                "https://example.com/c",
                output / "schemas" / "example" / "schemas" / "c" / "%" /
                    "schema.metapack",
                output / "schemas" / "example" / "schemas" / "c" / "%" /
                    "dependencies.metapack");
  EXPECT_ACTION(plan, 2, 1, 2, Health,
                output / "schemas" / "example" / "schemas" / "c" / "%" /
                    "health.metapack",
                "https://example.com/c",
                output / "schemas" / "example" / "schemas" / "c" / "%" /
                    "schema.metapack",
                output / "schemas" / "example" / "schemas" / "c" / "%" /
                    "dependencies.metapack");
  // TODO(over-rebuild): a's and b's dependents rebuild unnecessarily

  EXPECT_ACTION(plan, 3, 0, 4, SchemaMetadata,
                output / "explorer" / "example" / "schemas" / "c" / "%" /
                    "schema.metapack",
                "https://example.com/c",
                output / "schemas" / "example" / "schemas" / "c" / "%" /
                    "schema.metapack",
                output / "schemas" / "example" / "schemas" / "c" / "%" /
                    "health.metapack",
                output / "schemas" / "example" / "schemas" / "c" / "%" /
                    "dependencies.metapack");
  EXPECT_ACTION(plan, 3, 1, 4, BlazeExhaustive,
                output / "schemas" / "example" / "schemas" / "c" / "%" /
                    "blaze-exhaustive.metapack",
                "https://example.com/c",
                output / "schemas" / "example" / "schemas" / "c" / "%" /
                    "bundle.metapack");
  EXPECT_ACTION(plan, 3, 2, 4, BlazeFast,
                output / "schemas" / "example" / "schemas" / "c" / "%" /
                    "blaze-fast.metapack",
                "https://example.com/c",
                output / "schemas" / "example" / "schemas" / "c" / "%" /
                    "bundle.metapack");
  EXPECT_ACTION(plan, 3, 3, 4, Editor,
                output / "schemas" / "example" / "schemas" / "c" / "%" /
                    "editor.metapack",
                "https://example.com/c",
                output / "schemas" / "example" / "schemas" / "c" / "%" /
                    "bundle.metapack");

  EXPECT_ACTION(plan, 4, 0, 3, SearchIndex,
                output / "explorer" / "%" / "search.metapack", "",
                output / "explorer" / "example" / "schemas" / "c" / "%" /
                    "schema.metapack",
                output / "explorer" / "example" / "schemas" / "b" / "%" /
                    "schema.metapack",
                output / "explorer" / "example" / "schemas" / "a" / "%" /
                    "schema.metapack");
  EXPECT_ACTION_UNORDERED(plan, 4, 1, 3, DirectoryList,
                          output / "explorer" / "example" / "schemas" / "%" /
                              "directory.metapack",
                          "",
                          output / "explorer" / "example" / "schemas" / "a" /
                              "%" / "schema.metapack",
                          output / "explorer" / "example" / "schemas" / "b" /
                              "%" / "schema.metapack",
                          output / "explorer" / "example" / "schemas" / "c" /
                              "%" / "schema.metapack");
  EXPECT_ACTION(plan, 4, 2, 3, WebSchema,
                output / "explorer" / "example" / "schemas" / "c" / "%" /
                    "schema-html.metapack",
                "https://example.com/c",
                output / "explorer" / "example" / "schemas" / "c" / "%" /
                    "schema.metapack",
                output / "schemas" / "example" / "schemas" / "c" / "%" /
                    "health.metapack");

  EXPECT_ACTION(
      plan, 5, 0, 2, DirectoryList,
      output / "explorer" / "example" / "%" / "directory.metapack", "",
      output / "explorer" / "example" / "schemas" / "%" / "directory.metapack");
  EXPECT_ACTION(plan, 5, 1, 2, WebDirectory,
                output / "explorer" / "example" / "schemas" / "%" /
                    "directory-html.metapack",
                "",
                output / "explorer" / "example" / "schemas" / "%" /
                    "directory.metapack");

  EXPECT_ACTION(plan, 6, 0, 2, DirectoryList,
                output / "explorer" / "%" / "directory.metapack", "",
                output / "explorer" / "example" / "%" / "directory.metapack");
  EXPECT_ACTION(
      plan, 6, 1, 2, WebDirectory,
      output / "explorer" / "example" / "%" / "directory-html.metapack", "",
      output / "explorer" / "example" / "%" / "directory.metapack");

  EXPECT_ACTION(plan, 7, 0, 1, WebIndex,
                output / "explorer" / "%" / "directory-html.metapack", "",
                output / "explorer" / "%" / "directory.metapack");

  EXPECT_TOTAL_FILES(
      plan, entries, output / "version.json", output / "configuration.json",
      output / "routes.bin", output / "explorer" / "%" / "search.metapack",
      output / "explorer" / "%" / "directory.metapack",
      output / "explorer" / "%" / "404.metapack",
      output / "explorer" / "%" / "directory-html.metapack",
      output / "explorer" / "example" / "%" / "directory.metapack",
      output / "explorer" / "example" / "%" / "directory-html.metapack",
      output / "explorer" / "example" / "schemas" / "%" / "directory.metapack",
      output / "explorer" / "example" / "schemas" / "%" /
          "directory-html.metapack",
      output / "schemas" / "example" / "schemas" / "a" / "%" /
          "schema.metapack",
      output / "schemas" / "example" / "schemas" / "a" / "%" /
          "dependencies.metapack",
      output / "schemas" / "example" / "schemas" / "a" / "%" /
          "locations.metapack",
      output / "schemas" / "example" / "schemas" / "a" / "%" /
          "positions.metapack",
      output / "schemas" / "example" / "schemas" / "a" / "%" / "stats.metapack",
      output / "schemas" / "example" / "schemas" / "a" / "%" /
          "bundle.metapack",
      output / "schemas" / "example" / "schemas" / "a" / "%" /
          "health.metapack",
      output / "schemas" / "example" / "schemas" / "a" / "%" /
          "editor.metapack",
      output / "schemas" / "example" / "schemas" / "a" / "%" / output /
          "explorer" / "example" / "schemas" / "a" / "%" / "schema.metapack",
      output / "schemas" / "example" / "schemas" / "a" / "%" /
          "blaze-exhaustive.metapack",
      output / "schemas" / "example" / "schemas" / "a" / "%" /
          "blaze-fast.metapack",
      output / "explorer" / "example" / "schemas" / "a" / "%" /
          "schema-html.metapack",
      output / "explorer" / "example" / "schemas" / "a" / "%" /
          "directory-html.metapack",
      output / "schemas" / "example" / "schemas" / "b" / "%" /
          "schema.metapack",
      output / "schemas" / "example" / "schemas" / "b" / "%" /
          "dependencies.metapack",
      output / "schemas" / "example" / "schemas" / "b" / "%" /
          "locations.metapack",
      output / "schemas" / "example" / "schemas" / "b" / "%" /
          "positions.metapack",
      output / "schemas" / "example" / "schemas" / "b" / "%" / "stats.metapack",
      output / "schemas" / "example" / "schemas" / "b" / "%" /
          "bundle.metapack",
      output / "schemas" / "example" / "schemas" / "b" / "%" /
          "health.metapack",
      output / "schemas" / "example" / "schemas" / "b" / "%" /
          "editor.metapack",
      output / "schemas" / "example" / "schemas" / "b" / "%" / output /
          "explorer" / "example" / "schemas" / "b" / "%" / "schema.metapack",
      output / "schemas" / "example" / "schemas" / "b" / "%" /
          "blaze-exhaustive.metapack",
      output / "schemas" / "example" / "schemas" / "b" / "%" /
          "blaze-fast.metapack",
      output / "explorer" / "example" / "schemas" / "b" / "%" /
          "schema-html.metapack",
      output / "explorer" / "example" / "schemas" / "b" / "%" /
          "directory-html.metapack",
      output / "schemas" / "example" / "schemas" / "c" / "%" /
          "schema.metapack",
      output / "schemas" / "example" / "schemas" / "c" / "%" /
          "dependencies.metapack",
      output / "schemas" / "example" / "schemas" / "c" / "%" /
          "locations.metapack",
      output / "schemas" / "example" / "schemas" / "c" / "%" /
          "positions.metapack",
      output / "schemas" / "example" / "schemas" / "c" / "%" / "stats.metapack",
      output / "schemas" / "example" / "schemas" / "c" / "%" /
          "bundle.metapack",
      output / "schemas" / "example" / "schemas" / "c" / "%" /
          "health.metapack",
      output / "schemas" / "example" / "schemas" / "c" / "%" /
          "editor.metapack",
      output / "schemas" / "example" / "schemas" / "c" / "%" / output /
          "explorer" / "example" / "schemas" / "c" / "%" / "schema.metapack",
      output / "schemas" / "example" / "schemas" / "c" / "%" /
          "blaze-exhaustive.metapack",
      output / "schemas" / "example" / "schemas" / "c" / "%" /
          "blaze-fast.metapack",
      output / "explorer" / "example" / "schemas" / "c" / "%" /
          "schema-html.metapack");
}

TEST(Build_delta, incremental_directory_listing_includes_unchanged_siblings) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;

  entries.emplace(output / "version.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "configuration.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "routes.bin",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "search.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "directory.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "directory-html.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "404.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  ADD_SCHEMA_ENTRIES(entries, output, "foo", true, true, MTIME(100));
  ADD_SCHEMA_ENTRIES(entries, output, "bar", true, true, MTIME(100));

  const sourcemeta::one::Resolver::Views schemas{
      {"https://example.com/foo", {"/src/foo.json", "foo", MTIME(200)}},
      {"https://example.com/bar", {"/src/bar.json", "bar", MTIME(100)}}};
  const std::vector<std::filesystem::path> changed{"/src/foo.json"};
  const std::vector<std::filesystem::path> removed;
  const auto plan{sourcemeta::one::delta(sourcemeta::one::BuildPhase::Produce,
                                         sourcemeta::one::BuildPlan::Type::Full,
                                         entries, output, schemas, "1.0.0",
                                         true, "", changed, removed)};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, Full, 6, 15);

  EXPECT_ACTION(plan, 0, 0, 1, Materialise,
                output / "schemas" / "foo" / "%" / "schema.metapack",
                "https://example.com/foo",
                std::filesystem::path{"/"} / "src" / "foo.json",
                output / "configuration.json");

  EXPECT_ACTION(plan, 1, 0, 4, Dependencies,
                output / "schemas" / "foo" / "%" / "dependencies.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 1, 1, 4, Locations,
                output / "schemas" / "foo" / "%" / "locations.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 1, 2, 4, Positions,
                output / "schemas" / "foo" / "%" / "positions.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 1, 3, 4, Stats,
                output / "schemas" / "foo" / "%" / "stats.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack");

  EXPECT_ACTION(plan, 2, 0, 2, Bundle,
                output / "schemas" / "foo" / "%" / "bundle.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "dependencies.metapack");
  EXPECT_ACTION(plan, 2, 1, 2, Health,
                output / "schemas" / "foo" / "%" / "health.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "dependencies.metapack");

  EXPECT_ACTION(plan, 3, 0, 4, SchemaMetadata,
                output / "explorer" / "foo" / "%" / "schema.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "health.metapack",
                output / "schemas" / "foo" / "%" / "dependencies.metapack");
  EXPECT_ACTION(plan, 3, 1, 4, BlazeExhaustive,
                output / "schemas" / "foo" / "%" / "blaze-exhaustive.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "bundle.metapack");
  EXPECT_ACTION(plan, 3, 2, 4, BlazeFast,
                output / "schemas" / "foo" / "%" / "blaze-fast.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "bundle.metapack");
  EXPECT_ACTION(plan, 3, 3, 4, Editor,
                output / "schemas" / "foo" / "%" / "editor.metapack",
                "https://example.com/foo",
                output / "schemas" / "foo" / "%" / "bundle.metapack");

  // The directory list must depend on BOTH foo AND bar
  EXPECT_ACTION_UNORDERED(plan, 4, 0, 3, DirectoryList,
                          output / "explorer" / "%" / "directory.metapack", "",
                          output / "explorer" / "foo" / "%" / "schema.metapack",
                          output / "explorer" / "bar" / "%" /
                              "schema.metapack");
  EXPECT_ACTION_UNORDERED(
      plan, 4, 1, 3, SearchIndex, output / "explorer" / "%" / "search.metapack",
      "", output / "explorer" / "foo" / "%" / "schema.metapack",
      output / "explorer" / "bar" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 4, 2, 3, WebSchema,
                output / "explorer" / "foo" / "%" / "schema-html.metapack",
                "https://example.com/foo",
                output / "explorer" / "foo" / "%" / "schema.metapack",
                output / "schemas" / "foo" / "%" / "health.metapack");

  EXPECT_ACTION(plan, 5, 0, 1, WebIndex,
                output / "explorer" / "%" / "directory-html.metapack", "",
                output / "explorer" / "%" / "directory.metapack");

  EXPECT_TOTAL_FILES(
      plan, entries, output / "version.json", output / "configuration.json",
      output / "routes.bin", output / "explorer" / "%" / "search.metapack",
      output / "explorer" / "%" / "directory.metapack",
      output / "explorer" / "%" / "directory-html.metapack",
      output / "explorer" / "%" / "404.metapack",
      output / "schemas" / "foo" / "%" / "schema.metapack",
      output / "schemas" / "foo" / "%" / "dependencies.metapack",
      output / "schemas" / "foo" / "%" / "locations.metapack",
      output / "schemas" / "foo" / "%" / "positions.metapack",
      output / "schemas" / "foo" / "%" / "stats.metapack",
      output / "schemas" / "foo" / "%" / "bundle.metapack",
      output / "schemas" / "foo" / "%" / "health.metapack",
      output / "schemas" / "foo" / "%" / "editor.metapack",
      output / "schemas" / "foo" / "%" / "blaze-exhaustive.metapack",
      output / "schemas" / "foo" / "%" / "blaze-fast.metapack",
      output / "explorer" / "foo" / "%" / "schema.metapack",
      output / "explorer" / "foo" / "%" / "schema-html.metapack",
      output / "schemas" / "bar" / "%" / "schema.metapack",
      output / "schemas" / "bar" / "%" / "dependencies.metapack",
      output / "schemas" / "bar" / "%" / "locations.metapack",
      output / "schemas" / "bar" / "%" / "positions.metapack",
      output / "schemas" / "bar" / "%" / "stats.metapack",
      output / "schemas" / "bar" / "%" / "bundle.metapack",
      output / "schemas" / "bar" / "%" / "health.metapack",
      output / "schemas" / "bar" / "%" / "editor.metapack",
      output / "schemas" / "bar" / "%" / "blaze-exhaustive.metapack",
      output / "schemas" / "bar" / "%" / "blaze-fast.metapack",
      output / "explorer" / "bar" / "%" / "schema.metapack",
      output / "explorer" / "bar" / "%" / "schema-html.metapack",
      output / "explorer" / "foo" / "%" / "directory-html.metapack",
      output / "explorer" / "bar" / "%" / "directory-html.metapack");
}

TEST(Build_delta, incremental_add_schema_rebuilds_all_dependents) {
  const std::filesystem::path output{"/output"};
  sourcemeta::one::BuildState entries;

  entries.emplace(output / "version.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "configuration.json",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "routes.bin",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "search.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "directory.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "directory-html.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  entries.emplace(output / "explorer" / "%" / "404.metapack",
                  {.file_mark = MTIME(100), .dependencies = {}});
  ADD_SCHEMA_ENTRIES(entries, output, "a", true, true, MTIME(100));
  ADD_SCHEMA_ENTRIES(entries, output, "b", true, true, MTIME(100));

  const sourcemeta::one::Resolver::Views schemas{
      {"https://example.com/a", {"/src/a.json", "a", MTIME(100)}},
      {"https://example.com/b", {"/src/b.json", "b", MTIME(100)}},
      {"https://example.com/c", {"/src/c.json", "c", MTIME(200)}}};
  const std::vector<std::filesystem::path> changed{"/src/c.json"};
  const std::vector<std::filesystem::path> removed;
  const auto plan{sourcemeta::one::delta(sourcemeta::one::BuildPhase::Produce,
                                         sourcemeta::one::BuildPlan::Type::Full,
                                         entries, output, schemas, "1.0.0",
                                         true, "", changed, removed)};

  EXPECT_CONSISTENT_PLAN(plan, entries, output, Full, 6, 15);

  EXPECT_ACTION(plan, 0, 0, 1, Materialise,
                output / "schemas" / "c" / "%" / "schema.metapack",
                "https://example.com/c",
                std::filesystem::path{"/"} / "src" / "c.json",
                output / "configuration.json");

  EXPECT_ACTION(plan, 1, 0, 4, Dependencies,
                output / "schemas" / "c" / "%" / "dependencies.metapack",
                "https://example.com/c",
                output / "schemas" / "c" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 1, 1, 4, Locations,
                output / "schemas" / "c" / "%" / "locations.metapack",
                "https://example.com/c",
                output / "schemas" / "c" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 1, 2, 4, Positions,
                output / "schemas" / "c" / "%" / "positions.metapack",
                "https://example.com/c",
                output / "schemas" / "c" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 1, 3, 4, Stats,
                output / "schemas" / "c" / "%" / "stats.metapack",
                "https://example.com/c",
                output / "schemas" / "c" / "%" / "schema.metapack");

  EXPECT_ACTION(plan, 2, 0, 2, Bundle,
                output / "schemas" / "c" / "%" / "bundle.metapack",
                "https://example.com/c",
                output / "schemas" / "c" / "%" / "schema.metapack",
                output / "schemas" / "c" / "%" / "dependencies.metapack");
  EXPECT_ACTION(plan, 2, 1, 2, Health,
                output / "schemas" / "c" / "%" / "health.metapack",
                "https://example.com/c",
                output / "schemas" / "c" / "%" / "schema.metapack",
                output / "schemas" / "c" / "%" / "dependencies.metapack");
  // TODO(over-rebuild): a's and b's dependents rebuild unnecessarily

  EXPECT_ACTION(plan, 3, 0, 4, SchemaMetadata,
                output / "explorer" / "c" / "%" / "schema.metapack",
                "https://example.com/c",
                output / "schemas" / "c" / "%" / "schema.metapack",
                output / "schemas" / "c" / "%" / "health.metapack",
                output / "schemas" / "c" / "%" / "dependencies.metapack");
  EXPECT_ACTION(plan, 3, 1, 4, BlazeExhaustive,
                output / "schemas" / "c" / "%" / "blaze-exhaustive.metapack",
                "https://example.com/c",
                output / "schemas" / "c" / "%" / "bundle.metapack");
  EXPECT_ACTION(plan, 3, 2, 4, BlazeFast,
                output / "schemas" / "c" / "%" / "blaze-fast.metapack",
                "https://example.com/c",
                output / "schemas" / "c" / "%" / "bundle.metapack");
  EXPECT_ACTION(plan, 3, 3, 4, Editor,
                output / "schemas" / "c" / "%" / "editor.metapack",
                "https://example.com/c",
                output / "schemas" / "c" / "%" / "bundle.metapack");

  EXPECT_ACTION_UNORDERED(plan, 4, 0, 3, DirectoryList,
                          output / "explorer" / "%" / "directory.metapack", "",
                          output / "explorer" / "a" / "%" / "schema.metapack",
                          output / "explorer" / "b" / "%" / "schema.metapack",
                          output / "explorer" / "c" / "%" / "schema.metapack");
  EXPECT_ACTION_UNORDERED(plan, 4, 1, 3, SearchIndex,
                          output / "explorer" / "%" / "search.metapack", "",
                          output / "explorer" / "a" / "%" / "schema.metapack",
                          output / "explorer" / "b" / "%" / "schema.metapack",
                          output / "explorer" / "c" / "%" / "schema.metapack");
  EXPECT_ACTION(plan, 4, 2, 3, WebSchema,
                output / "explorer" / "c" / "%" / "schema-html.metapack",
                "https://example.com/c",
                output / "explorer" / "c" / "%" / "schema.metapack",
                output / "schemas" / "c" / "%" / "health.metapack");

  EXPECT_ACTION(plan, 5, 0, 1, WebIndex,
                output / "explorer" / "%" / "directory-html.metapack", "",
                output / "explorer" / "%" / "directory.metapack");

  EXPECT_TOTAL_FILES(
      plan, entries, output / "version.json", output / "configuration.json",
      output / "routes.bin", output / "explorer" / "%" / "search.metapack",
      output / "explorer" / "%" / "directory.metapack",
      output / "explorer" / "%" / "directory-html.metapack",
      output / "explorer" / "%" / "404.metapack",
      output / "schemas" / "a" / "%" / "schema.metapack",
      output / "schemas" / "a" / "%" / "dependencies.metapack",
      output / "schemas" / "a" / "%" / "locations.metapack",
      output / "schemas" / "a" / "%" / "positions.metapack",
      output / "schemas" / "a" / "%" / "stats.metapack",
      output / "schemas" / "a" / "%" / "bundle.metapack",
      output / "schemas" / "a" / "%" / "health.metapack",
      output / "schemas" / "a" / "%" / "editor.metapack",
      output / "schemas" / "a" / "%" / "blaze-exhaustive.metapack",
      output / "schemas" / "a" / "%" / "blaze-fast.metapack",
      output / "explorer" / "a" / "%" / "schema.metapack",
      output / "explorer" / "a" / "%" / "schema-html.metapack",
      output / "explorer" / "a" / "%" / "directory-html.metapack",
      output / "schemas" / "b" / "%" / "schema.metapack",
      output / "schemas" / "b" / "%" / "dependencies.metapack",
      output / "schemas" / "b" / "%" / "locations.metapack",
      output / "schemas" / "b" / "%" / "positions.metapack",
      output / "schemas" / "b" / "%" / "stats.metapack",
      output / "schemas" / "b" / "%" / "bundle.metapack",
      output / "schemas" / "b" / "%" / "health.metapack",
      output / "schemas" / "b" / "%" / "editor.metapack",
      output / "schemas" / "b" / "%" / "blaze-exhaustive.metapack",
      output / "schemas" / "b" / "%" / "blaze-fast.metapack",
      output / "explorer" / "b" / "%" / "schema.metapack",
      output / "explorer" / "b" / "%" / "schema-html.metapack",
      output / "explorer" / "b" / "%" / "directory-html.metapack",
      output / "schemas" / "c" / "%" / "schema.metapack",
      output / "schemas" / "c" / "%" / "dependencies.metapack",
      output / "schemas" / "c" / "%" / "locations.metapack",
      output / "schemas" / "c" / "%" / "positions.metapack",
      output / "schemas" / "c" / "%" / "stats.metapack",
      output / "schemas" / "c" / "%" / "bundle.metapack",
      output / "schemas" / "c" / "%" / "health.metapack",
      output / "schemas" / "c" / "%" / "editor.metapack",
      output / "schemas" / "c" / "%" / "blaze-exhaustive.metapack",
      output / "schemas" / "c" / "%" / "blaze-fast.metapack",
      output / "explorer" / "c" / "%" / "schema.metapack",
      output / "explorer" / "c" / "%" / "schema-html.metapack");
}
