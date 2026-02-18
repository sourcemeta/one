#include <gtest/gtest.h>

#include <sourcemeta/one/configuration.h>

#define EXPECT_PAGE(configuration, path, property, value)                      \
  EXPECT_TRUE((configuration).entries.contains(path));                         \
  EXPECT_TRUE(std::holds_alternative<sourcemeta::one::Configuration::Page>(    \
      (configuration).entries.at(path)));                                      \
  EXPECT_EQ(std::get<sourcemeta::one::Configuration::Page>(                    \
                (configuration).entries.at(path))                              \
                .property,                                                     \
            value);

#define EXPECT_COLLECTION(configuration, path, property, value)                \
  EXPECT_TRUE((configuration).entries.contains(path));                         \
  EXPECT_TRUE(                                                                 \
      std::holds_alternative<sourcemeta::one::Configuration::Collection>(      \
          (configuration).entries.at(path)));                                  \
  EXPECT_EQ(std::get<sourcemeta::one::Configuration::Collection>(              \
                (configuration).entries.at(path))                              \
                .property,                                                     \
            value);

TEST(Configuration, valid_001) {
  const auto configuration_path{std::filesystem::path{STUB_DIRECTORY} /
                                "parse_valid_001.json"};
  const auto raw_configuration{sourcemeta::one::Configuration::read(
      configuration_path, COLLECTIONS_DIRECTORY)};
  const auto configuration{sourcemeta::one::Configuration::parse(
      raw_configuration, configuration_path.parent_path())};

  EXPECT_EQ(configuration.url, "http://localhost:8000");

  EXPECT_TRUE(configuration.html.has_value());
  EXPECT_EQ(configuration.html.value().name, "Title");
  EXPECT_EQ(configuration.html.value().description, "Description");
  EXPECT_FALSE(configuration.html.value().head.has_value());
  EXPECT_FALSE(configuration.html.value().hero.has_value());
  EXPECT_FALSE(configuration.html.value().action.has_value());

  EXPECT_EQ(configuration.entries.size(), 3);

  EXPECT_PAGE(configuration, "example", title, "Sourcemeta");
  EXPECT_PAGE(configuration, "example", description, "My description");
  EXPECT_PAGE(configuration, "example", email, "hello@sourcemeta.com");
  EXPECT_PAGE(configuration, "example", github, "sourcemeta");
  EXPECT_PAGE(configuration, "example", website, "https://www.sourcemeta.com");

  EXPECT_PAGE(configuration, "test", title, "A sample schema folder");
  EXPECT_PAGE(configuration, "test", description, "For testing purposes");
  EXPECT_PAGE(configuration, "test", email, std::nullopt);
  EXPECT_PAGE(configuration, "test", github, "sourcemeta/one");
  EXPECT_PAGE(configuration, "test", website, std::nullopt);

  EXPECT_COLLECTION(configuration, "example/extension", title, "Test");
  EXPECT_COLLECTION(configuration, "example/extension", description,
                    std::nullopt);
  EXPECT_COLLECTION(configuration, "example/extension", email, std::nullopt);
  EXPECT_COLLECTION(configuration, "example/extension", github, std::nullopt);
  EXPECT_COLLECTION(configuration, "example/extension", website, std::nullopt);
  EXPECT_COLLECTION(configuration, "example/extension", absolute_path,
                    std::filesystem::path{STUB_DIRECTORY} / "schemas" /
                        "example" / "extension");
  EXPECT_COLLECTION(configuration, "example/extension", base,
                    "https://example.com/extension");
  EXPECT_COLLECTION(configuration, "example/extension", default_dialect,
                    "http://json-schema.org/draft-07/schema#");
  EXPECT_COLLECTION(configuration, "example/extension", resolve.size(), 1);
  EXPECT_COLLECTION(configuration, "example/extension",
                    resolve.at("https://other.com/single.json"), "/foo.json");
  EXPECT_COLLECTION(configuration, "example/extension", lint.rules.size(), 0);
  EXPECT_COLLECTION(configuration, "example/extension", ignore.size(), 0);
  EXPECT_COLLECTION(configuration, "example/extension", extra.size(), 0);
}

TEST(Configuration, valid_002) {
  const auto configuration_path{std::filesystem::path{STUB_DIRECTORY} /
                                "parse_valid_002.json"};
  const auto raw_configuration{sourcemeta::one::Configuration::read(
      configuration_path, COLLECTIONS_DIRECTORY)};
  const auto configuration{sourcemeta::one::Configuration::parse(
      raw_configuration, configuration_path.parent_path())};

  EXPECT_EQ(configuration.url, "http://localhost:8000");

  EXPECT_FALSE(configuration.html.has_value());

  EXPECT_EQ(configuration.entries.size(), 1);

  EXPECT_PAGE(configuration, "test", title, "A sample schema folder");
  EXPECT_PAGE(configuration, "test", description, "For testing purposes");
  EXPECT_PAGE(configuration, "test", github, "sourcemeta/one");
}

TEST(Configuration, valid_003) {
  const auto configuration_path{std::filesystem::path{STUB_DIRECTORY} /
                                "parse_valid_003.json"};
  const auto raw_configuration{sourcemeta::one::Configuration::read(
      configuration_path, COLLECTIONS_DIRECTORY)};
  const auto configuration{sourcemeta::one::Configuration::parse(
      raw_configuration, configuration_path.parent_path())};

  EXPECT_EQ(configuration.url, "http://localhost:8000");

  EXPECT_TRUE(configuration.html.has_value());
  EXPECT_EQ(configuration.html.value().name, "Title");
  EXPECT_EQ(configuration.html.value().description, "Description");
  EXPECT_FALSE(configuration.html.value().head.has_value());
  EXPECT_FALSE(configuration.html.value().hero.has_value());
  EXPECT_FALSE(configuration.html.value().action.has_value());

  EXPECT_EQ(configuration.entries.size(), 1);

  EXPECT_COLLECTION(configuration, "example", title, std::nullopt);
  EXPECT_COLLECTION(configuration, "example", description, std::nullopt);
  EXPECT_COLLECTION(configuration, "example", email, std::nullopt);
  EXPECT_COLLECTION(configuration, "example", github, std::nullopt);
  EXPECT_COLLECTION(configuration, "example", website, std::nullopt);
  EXPECT_COLLECTION(configuration, "example", absolute_path,
                    std::filesystem::path{STUB_DIRECTORY} / "schemas" /
                        "example" / "extension");
  // Note that the base URI is turned into lowercase and canonicalised
  EXPECT_COLLECTION(configuration, "example", base, "https://example.com/foo");
  EXPECT_COLLECTION(configuration, "example", default_dialect, std::nullopt);
  EXPECT_COLLECTION(configuration, "example", resolve.size(), 0);
  EXPECT_COLLECTION(configuration, "example", lint.rules.size(), 0);
  EXPECT_COLLECTION(configuration, "example", ignore.size(), 0);
}

TEST(Configuration, valid_004) {
  const auto configuration_path{std::filesystem::path{STUB_DIRECTORY} /
                                "parse_valid_004.json"};
  const auto raw_configuration{sourcemeta::one::Configuration::read(
      configuration_path, COLLECTIONS_DIRECTORY)};
  const auto configuration{sourcemeta::one::Configuration::parse(
      raw_configuration, configuration_path.parent_path())};

  EXPECT_EQ(configuration.url, "http://localhost:8000");

  EXPECT_TRUE(configuration.html.has_value());
  EXPECT_EQ(configuration.html.value().name, "Title");
  EXPECT_EQ(configuration.html.value().description, "Description");
  EXPECT_FALSE(configuration.html.value().head.has_value());
  EXPECT_FALSE(configuration.html.value().hero.has_value());
  EXPECT_FALSE(configuration.html.value().action.has_value());

  EXPECT_EQ(configuration.entries.size(), 1);

  EXPECT_COLLECTION(configuration, "example", title, std::nullopt);
  EXPECT_COLLECTION(configuration, "example", description, std::nullopt);
  EXPECT_COLLECTION(configuration, "example", email, std::nullopt);
  EXPECT_COLLECTION(configuration, "example", github, std::nullopt);
  EXPECT_COLLECTION(configuration, "example", website, std::nullopt);
  EXPECT_COLLECTION(configuration, "example", absolute_path,
                    std::filesystem::path{STUB_DIRECTORY} / "schemas" /
                        "example" / "extension");
  EXPECT_COLLECTION(configuration, "example", base, "http://localhost:8000");
  EXPECT_COLLECTION(configuration, "example", default_dialect, std::nullopt);
  EXPECT_COLLECTION(configuration, "example", resolve.size(), 0);
  EXPECT_COLLECTION(configuration, "example", extension.size(), 3);
  EXPECT_COLLECTION(configuration, "example", extension.contains(".json"),
                    true);
  EXPECT_COLLECTION(configuration, "example", extension.contains(".yml"), true);
  EXPECT_COLLECTION(configuration, "example", extension.contains(".yaml"),
                    true);
  EXPECT_COLLECTION(configuration, "example", lint.rules.size(), 0);
  EXPECT_COLLECTION(configuration, "example", ignore.size(), 0);
  EXPECT_COLLECTION(configuration, "example", extra.size(), 0);
}

TEST(Configuration, valid_005) {
  const auto configuration_path{std::filesystem::path{STUB_DIRECTORY} /
                                "parse_valid_005.json"};
  const auto raw_configuration{sourcemeta::one::Configuration::read(
      configuration_path, COLLECTIONS_DIRECTORY)};
  const auto configuration{sourcemeta::one::Configuration::parse(
      raw_configuration, configuration_path.parent_path())};

  EXPECT_EQ(configuration.url, "http://localhost:8000");

  EXPECT_TRUE(configuration.html.has_value());
  EXPECT_EQ(configuration.html.value().name, "Title");
  EXPECT_EQ(configuration.html.value().description, "Description");

  EXPECT_EQ(configuration.entries.size(), 1);

  EXPECT_COLLECTION(configuration, "example", title, std::nullopt);
  EXPECT_COLLECTION(configuration, "example", description, std::nullopt);
  EXPECT_COLLECTION(configuration, "example", email, std::nullopt);
  EXPECT_COLLECTION(configuration, "example", github, std::nullopt);
  EXPECT_COLLECTION(configuration, "example", website, std::nullopt);
  EXPECT_COLLECTION(configuration, "example", absolute_path,
                    std::filesystem::path{STUB_DIRECTORY} / "schemas" /
                        "example" / "extension");
  EXPECT_COLLECTION(configuration, "example", base,
                    "https://example.com/schemas");
  EXPECT_COLLECTION(configuration, "example", default_dialect, std::nullopt);
  EXPECT_COLLECTION(configuration, "example", resolve.size(), 0);
  EXPECT_COLLECTION(configuration, "example", lint.rules.size(), 2);
  EXPECT_COLLECTION(
      configuration, "example", lint.rules.at(0),
      std::filesystem::weakly_canonical(std::filesystem::path{STUB_DIRECTORY} /
                                        "rules" / "my_rule.py"));
  EXPECT_COLLECTION(
      configuration, "example", lint.rules.at(1),
      std::filesystem::weakly_canonical(std::filesystem::path{STUB_DIRECTORY} /
                                        "rules" / "another_rule.py"));
  EXPECT_COLLECTION(configuration, "example", ignore.size(), 0);
  EXPECT_COLLECTION(configuration, "example", extra.size(), 0);
}

TEST(Configuration, valid_006) {
  const auto configuration_path{std::filesystem::path{STUB_DIRECTORY} /
                                "parse_valid_006.json"};
  const auto raw_configuration{sourcemeta::one::Configuration::read(
      configuration_path, COLLECTIONS_DIRECTORY)};
  const auto configuration{sourcemeta::one::Configuration::parse(
      raw_configuration, configuration_path.parent_path())};

  EXPECT_EQ(configuration.url, "http://localhost:8000");

  EXPECT_TRUE(configuration.html.has_value());
  EXPECT_EQ(configuration.html.value().name, "Title");
  EXPECT_EQ(configuration.html.value().description, "Description");

  EXPECT_EQ(configuration.entries.size(), 1);

  EXPECT_COLLECTION(configuration, "example", title, std::nullopt);
  EXPECT_COLLECTION(configuration, "example", description, std::nullopt);
  EXPECT_COLLECTION(configuration, "example", email, std::nullopt);
  EXPECT_COLLECTION(configuration, "example", github, std::nullopt);
  EXPECT_COLLECTION(configuration, "example", website, std::nullopt);
  EXPECT_COLLECTION(configuration, "example", absolute_path,
                    std::filesystem::path{STUB_DIRECTORY} / "schemas" /
                        "example" / "extension");
  EXPECT_COLLECTION(configuration, "example", base,
                    "https://example.com/schemas");
  EXPECT_COLLECTION(configuration, "example", default_dialect, std::nullopt);
  EXPECT_COLLECTION(configuration, "example", resolve.size(), 0);
  EXPECT_COLLECTION(configuration, "example", lint.rules.size(), 0);
  EXPECT_COLLECTION(configuration, "example", ignore.size(), 2);
  EXPECT_COLLECTION(configuration, "example", ignore.at(0),
                    std::filesystem::weakly_canonical(
                        std::filesystem::path{STUB_DIRECTORY} / "schemas" /
                        "example" / "extension" / "draft"));
  EXPECT_COLLECTION(configuration, "example", ignore.at(1),
                    std::filesystem::weakly_canonical(
                        std::filesystem::path{STUB_DIRECTORY} / "schemas" /
                        "example" / "extension" / "deprecated"));
  EXPECT_COLLECTION(configuration, "example", extra.size(), 0);
}

TEST(Configuration, valid_007) {
  const auto configuration_path{std::filesystem::path{STUB_DIRECTORY} /
                                "parse_valid_007.json"};
  const auto raw_configuration{sourcemeta::one::Configuration::read(
      configuration_path, COLLECTIONS_DIRECTORY)};
  const auto configuration{sourcemeta::one::Configuration::parse(
      raw_configuration, configuration_path.parent_path())};

  EXPECT_EQ(configuration.url, "http://localhost:8000");

  EXPECT_TRUE(configuration.html.has_value());
  EXPECT_EQ(configuration.html.value().name, "Title");
  EXPECT_EQ(configuration.html.value().description, "Description");

  EXPECT_EQ(configuration.entries.size(), 1);

  EXPECT_COLLECTION(configuration, "example", title, std::nullopt);
  EXPECT_COLLECTION(configuration, "example", description, std::nullopt);
  EXPECT_COLLECTION(configuration, "example", email, std::nullopt);
  EXPECT_COLLECTION(configuration, "example", github, std::nullopt);
  EXPECT_COLLECTION(configuration, "example", website, std::nullopt);
  EXPECT_COLLECTION(configuration, "example", absolute_path,
                    std::filesystem::path{STUB_DIRECTORY} / "schemas" /
                        "example" / "extension");
  EXPECT_COLLECTION(configuration, "example", base,
                    "https://example.com/schemas");
  EXPECT_COLLECTION(configuration, "example", default_dialect, std::nullopt);
  EXPECT_COLLECTION(configuration, "example", resolve.size(), 0);
  EXPECT_COLLECTION(configuration, "example", lint.rules.size(), 1);
  EXPECT_COLLECTION(
      configuration, "example", lint.rules.at(0),
      std::filesystem::weakly_canonical(std::filesystem::path{STUB_DIRECTORY} /
                                        "rules" / "my_rule.py"));
  EXPECT_COLLECTION(configuration, "example", ignore.size(), 1);
  EXPECT_COLLECTION(configuration, "example", ignore.at(0),
                    std::filesystem::weakly_canonical(
                        std::filesystem::path{STUB_DIRECTORY} / "schemas" /
                        "example" / "extension" / "draft"));
  EXPECT_COLLECTION(configuration, "example", extra.size(), 0);
}

TEST(Configuration, valid_008) {
  const auto configuration_path{std::filesystem::path{STUB_DIRECTORY} /
                                "parse_valid_008.json"};
  const auto raw_configuration{sourcemeta::one::Configuration::read(
      configuration_path, COLLECTIONS_DIRECTORY)};
  const auto configuration{sourcemeta::one::Configuration::parse(
      raw_configuration, configuration_path.parent_path())};

  EXPECT_EQ(configuration.url, "http://localhost:8000");

  EXPECT_TRUE(configuration.html.has_value());
  EXPECT_EQ(configuration.html.value().name, "Title");
  EXPECT_EQ(configuration.html.value().description, "Description");

  EXPECT_EQ(configuration.entries.size(), 1);

  EXPECT_COLLECTION(configuration, "example", title, std::nullopt);
  EXPECT_COLLECTION(configuration, "example", absolute_path,
                    std::filesystem::path{STUB_DIRECTORY} / "folder" /
                        "schemas" / "example" / "extension");
  EXPECT_COLLECTION(configuration, "example", base,
                    "https://example.com/schemas");
  EXPECT_COLLECTION(configuration, "example", lint.rules.size(), 1);
  EXPECT_COLLECTION(
      configuration, "example", lint.rules.at(0),
      std::filesystem::weakly_canonical(std::filesystem::path{STUB_DIRECTORY} /
                                        "folder" / "rules" / "my_rule.py"));
  EXPECT_COLLECTION(configuration, "example", ignore.size(), 0);
}
