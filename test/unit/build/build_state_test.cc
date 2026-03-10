#include <gtest/gtest.h>

#include <sourcemeta/one/build.h>

#include <chrono>     // std::chrono::nanoseconds, std::chrono::duration_cast
#include <filesystem> // std::filesystem::path
#include <string>     // std::string

static auto state_path(const std::string &name) -> std::filesystem::path {
  return std::filesystem::path{BINARY_DIRECTORY} / "state" / name;
}

TEST(Build_state, round_trip_empty) {
  const auto path{state_path("empty")};
  std::filesystem::create_directories(path.parent_path());

  const sourcemeta::one::BuildEntries original_entries;
  sourcemeta::one::save_state(path, original_entries);

  sourcemeta::one::BuildEntries loaded_entries;
  EXPECT_TRUE(sourcemeta::one::load_state(path, loaded_entries));
  EXPECT_TRUE(loaded_entries.empty());
}

TEST(Build_state, round_trip_single_entry_no_deps_no_mark) {
  const auto path{state_path("single_no_deps")};
  std::filesystem::create_directories(path.parent_path());

  sourcemeta::one::BuildEntries original_entries;
  original_entries["/output/schemas/foo/%/schema.metapack"] = {
      .file_mark = std::nullopt,
      .static_dependencies = {},
      .dynamic_dependencies = {},
      .tracked = true};

  sourcemeta::one::save_state(path, original_entries);

  sourcemeta::one::BuildEntries loaded_entries;
  EXPECT_TRUE(sourcemeta::one::load_state(path, loaded_entries));
  EXPECT_EQ(loaded_entries.size(), 1);
  EXPECT_TRUE(loaded_entries.contains("/output/schemas/foo/%/schema.metapack"));

  const auto &entry{loaded_entries.at("/output/schemas/foo/%/schema.metapack")};
  EXPECT_FALSE(entry.file_mark.has_value());
  EXPECT_TRUE(entry.static_dependencies.empty());
  EXPECT_TRUE(entry.dynamic_dependencies.empty());
}

TEST(Build_state, round_trip_with_file_mark) {
  const auto path{state_path("with_mark")};
  std::filesystem::create_directories(path.parent_path());

  const auto now{std::filesystem::file_time_type::clock::now()};
  sourcemeta::one::BuildEntries original_entries;
  original_entries["/output/schemas/foo/%/schema.metapack"] = {
      .file_mark = now,
      .static_dependencies = {},
      .dynamic_dependencies = {},
      .tracked = true};

  sourcemeta::one::save_state(path, original_entries);

  sourcemeta::one::BuildEntries loaded_entries;
  EXPECT_TRUE(sourcemeta::one::load_state(path, loaded_entries));
  EXPECT_EQ(loaded_entries.size(), 1);

  const auto &entry{loaded_entries.at("/output/schemas/foo/%/schema.metapack")};
  EXPECT_TRUE(entry.file_mark.has_value());

  const auto original_ns{std::chrono::duration_cast<std::chrono::nanoseconds>(
                             now.time_since_epoch())
                             .count()};
  const auto loaded_ns{std::chrono::duration_cast<std::chrono::nanoseconds>(
                           entry.file_mark.value().time_since_epoch())
                           .count()};
  EXPECT_EQ(original_ns, loaded_ns);
}

TEST(Build_state, round_trip_with_static_dependencies) {
  const auto path{state_path("with_static_deps")};
  std::filesystem::create_directories(path.parent_path());

  sourcemeta::one::BuildEntries original_entries;
  original_entries["/output/schemas/foo/%/dependencies.metapack"] = {
      .file_mark = std::nullopt,
      .static_dependencies = {"/output/schemas/bar/%/schema.metapack",
                              "/output/schemas/baz/%/schema.metapack"},
      .dynamic_dependencies = {},
      .tracked = true};

  sourcemeta::one::save_state(path, original_entries);

  sourcemeta::one::BuildEntries loaded_entries;
  EXPECT_TRUE(sourcemeta::one::load_state(path, loaded_entries));
  EXPECT_EQ(loaded_entries.size(), 1);

  const auto &entry{
      loaded_entries.at("/output/schemas/foo/%/dependencies.metapack")};
  EXPECT_EQ(entry.static_dependencies.size(), 2);
  EXPECT_EQ(entry.static_dependencies[0],
            "/output/schemas/bar/%/schema.metapack");
  EXPECT_EQ(entry.static_dependencies[1],
            "/output/schemas/baz/%/schema.metapack");
  EXPECT_TRUE(entry.dynamic_dependencies.empty());
}

TEST(Build_state, round_trip_with_dynamic_dependencies) {
  const auto path{state_path("with_dynamic_deps")};
  std::filesystem::create_directories(path.parent_path());

  sourcemeta::one::BuildEntries original_entries;
  original_entries["/output/schemas/foo/%/dependencies.metapack"] = {
      .file_mark = std::nullopt,
      .static_dependencies = {"/output/schemas/bar/%/schema.metapack"},
      .dynamic_dependencies = {"/output/schemas/qux/%/schema.metapack"},
      .tracked = true};

  sourcemeta::one::save_state(path, original_entries);

  sourcemeta::one::BuildEntries loaded_entries;
  EXPECT_TRUE(sourcemeta::one::load_state(path, loaded_entries));
  EXPECT_EQ(loaded_entries.size(), 1);

  const auto &entry{
      loaded_entries.at("/output/schemas/foo/%/dependencies.metapack")};
  EXPECT_EQ(entry.static_dependencies.size(), 1);
  EXPECT_EQ(entry.static_dependencies[0],
            "/output/schemas/bar/%/schema.metapack");
  EXPECT_EQ(entry.dynamic_dependencies.size(), 1);
  EXPECT_EQ(entry.dynamic_dependencies[0],
            "/output/schemas/qux/%/schema.metapack");
}

TEST(Build_state, round_trip_multiple_entries) {
  const auto path{state_path("multiple")};
  std::filesystem::create_directories(path.parent_path());

  const auto now{std::filesystem::file_time_type::clock::now()};
  sourcemeta::one::BuildEntries original_entries;
  original_entries["/output/schemas/foo/%/schema.metapack"] = {
      .file_mark = now,
      .static_dependencies = {},
      .dynamic_dependencies = {},
      .tracked = true};
  original_entries["/output/schemas/foo/%/dependencies.metapack"] = {
      .file_mark = std::nullopt,
      .static_dependencies = {"/output/schemas/bar/%/schema.metapack"},
      .dynamic_dependencies = {},
      .tracked = true};
  original_entries["/output/configuration.json"] = {.file_mark = std::nullopt,
                                                    .static_dependencies = {},
                                                    .dynamic_dependencies = {},
                                                    .tracked = true};

  sourcemeta::one::save_state(path, original_entries);

  sourcemeta::one::BuildEntries loaded_entries;
  EXPECT_TRUE(sourcemeta::one::load_state(path, loaded_entries));
  EXPECT_EQ(loaded_entries.size(), 3);
  EXPECT_TRUE(loaded_entries.contains("/output/schemas/foo/%/schema.metapack"));
  EXPECT_TRUE(
      loaded_entries.contains("/output/schemas/foo/%/dependencies.metapack"));
  EXPECT_TRUE(loaded_entries.contains("/output/configuration.json"));

  EXPECT_TRUE(loaded_entries.at("/output/schemas/foo/%/schema.metapack")
                  .file_mark.has_value());
  EXPECT_EQ(loaded_entries.at("/output/schemas/foo/%/dependencies.metapack")
                .static_dependencies.size(),
            1);
  EXPECT_FALSE(
      loaded_entries.at("/output/configuration.json").file_mark.has_value());
}

TEST(Build_state, untracked_entries_not_saved) {
  const auto path{state_path("untracked")};
  std::filesystem::create_directories(path.parent_path());

  sourcemeta::one::BuildEntries original_entries;
  original_entries["/output/schemas/foo/%/schema.metapack"] = {
      .file_mark = std::nullopt,
      .static_dependencies = {},
      .dynamic_dependencies = {},
      .tracked = true};
  original_entries["/output/schemas/bar/%/schema.metapack"] = {
      .file_mark = std::nullopt,
      .static_dependencies = {},
      .dynamic_dependencies = {},
      .tracked = false};

  sourcemeta::one::save_state(path, original_entries);

  sourcemeta::one::BuildEntries loaded_entries;
  EXPECT_TRUE(sourcemeta::one::load_state(path, loaded_entries));
  EXPECT_EQ(loaded_entries.size(), 1);
  EXPECT_TRUE(loaded_entries.contains("/output/schemas/foo/%/schema.metapack"));
  EXPECT_FALSE(
      loaded_entries.contains("/output/schemas/bar/%/schema.metapack"));
}
