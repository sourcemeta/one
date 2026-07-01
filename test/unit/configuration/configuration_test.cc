#include <sourcemeta/core/test.h>
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

#define EXPECT_PRIORITY(configuration, path, value)                            \
  EXPECT_TRUE((configuration).entries.contains(path));                         \
  EXPECT_TRUE(                                                                 \
      std::holds_alternative<sourcemeta::one::Configuration::Collection>(      \
          (configuration).entries.at(path)));                                  \
  EXPECT_EQ(sourcemeta::one::Configuration::priority(                          \
                std::get<sourcemeta::one::Configuration::Collection>(          \
                    (configuration).entries.at(path))),                        \
            value);

TEST(valid_001) {
  const auto configuration_path{std::filesystem::path{STUB_DIRECTORY} /
                                "parse_valid_001.json"};
  const auto raw_configuration{
      sourcemeta::one::Configuration::read(configuration_path, SELF_DIRECTORY)};
  const auto configuration{sourcemeta::one::Configuration::parse(
      raw_configuration, configuration_path, configuration_path.parent_path())};

  EXPECT_EQ(configuration.path, configuration_path);

  EXPECT_EQ(configuration.url, "http://localhost:8000");
  EXPECT_TRUE(configuration.api);

  EXPECT_TRUE(configuration.html.has_value());
  EXPECT_EQ(configuration.html.value().name, "Title");
  EXPECT_EQ(configuration.html.value().description, "Description");
  EXPECT_FALSE(configuration.html.value().head.has_value());
  EXPECT_FALSE(configuration.html.value().hero.has_value());
  EXPECT_FALSE(configuration.html.value().action.has_value());

  EXPECT_EQ(configuration.entries.size(), 6);

  EXPECT_PAGE(configuration, "self", title, "Self");
  EXPECT_PAGE(configuration, "self", description,
              "The schemas that define the current version of this instance");
  EXPECT_PAGE(configuration, "self", email, "hello@sourcemeta.com");
  EXPECT_PAGE(configuration, "self", github, "sourcemeta/one");
  EXPECT_PAGE(configuration, "self", website, "https://www.sourcemeta.com");
  EXPECT_PAGE(configuration, "self/v1", title, std::nullopt);
  EXPECT_PAGE(configuration, "self/v1", description, std::nullopt);
  EXPECT_PAGE(configuration, "self/v1", email, std::nullopt);
  EXPECT_PAGE(configuration, "self/v1", github, std::nullopt);
  EXPECT_PAGE(configuration, "self/v1", website, std::nullopt);
  EXPECT_COLLECTION(configuration, "self/v1/schemas", absolute_path,
                    std::filesystem::path{SELF_DIRECTORY} / "v1" / "schemas");

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
  EXPECT_COLLECTION(configuration, "example/extension", extra.size(), 1);
  EXPECT_COLLECTION(configuration, "example/extension",
                    extra.defines("x-sourcemeta-one:path"), true);
  EXPECT_COLLECTION(configuration, "example/extension",
                    extra.at("x-sourcemeta-one:path"),
                    sourcemeta::core::JSON{configuration_path.string()});
  EXPECT_PRIORITY(configuration, "self/v1/schemas", 0);
  EXPECT_PRIORITY(configuration, "example/extension", 50);
}

TEST(valid_002) {
  const auto configuration_path{std::filesystem::path{STUB_DIRECTORY} /
                                "parse_valid_002.json"};
  const auto raw_configuration{
      sourcemeta::one::Configuration::read(configuration_path, SELF_DIRECTORY)};
  const auto configuration{sourcemeta::one::Configuration::parse(
      raw_configuration, configuration_path, configuration_path.parent_path())};

  EXPECT_EQ(configuration.path, configuration_path);

  EXPECT_EQ(configuration.url, "http://localhost:8000");
  EXPECT_TRUE(configuration.api);

  EXPECT_FALSE(configuration.html.has_value());

  EXPECT_EQ(configuration.entries.size(), 4);

  EXPECT_PAGE(configuration, "self", title, "Self");
  EXPECT_PAGE(configuration, "self/v1", title, std::nullopt);
  EXPECT_COLLECTION(configuration, "self/v1/schemas", absolute_path,
                    std::filesystem::path{SELF_DIRECTORY} / "v1" / "schemas");

  EXPECT_PAGE(configuration, "test", title, "A sample schema folder");
  EXPECT_PAGE(configuration, "test", description, "For testing purposes");
  EXPECT_PAGE(configuration, "test", github, "sourcemeta/one");

  EXPECT_PRIORITY(configuration, "self/v1/schemas", 0);
}

TEST(valid_003) {
  const auto configuration_path{std::filesystem::path{STUB_DIRECTORY} /
                                "parse_valid_003.json"};
  const auto raw_configuration{
      sourcemeta::one::Configuration::read(configuration_path, SELF_DIRECTORY)};
  const auto configuration{sourcemeta::one::Configuration::parse(
      raw_configuration, configuration_path, configuration_path.parent_path())};

  EXPECT_EQ(configuration.path, configuration_path);

  EXPECT_EQ(configuration.url, "http://localhost:8000");
  EXPECT_TRUE(configuration.api);

  EXPECT_TRUE(configuration.html.has_value());
  EXPECT_EQ(configuration.html.value().name, "Title");
  EXPECT_EQ(configuration.html.value().description, "Description");
  EXPECT_FALSE(configuration.html.value().head.has_value());
  EXPECT_FALSE(configuration.html.value().hero.has_value());
  EXPECT_FALSE(configuration.html.value().action.has_value());

  EXPECT_EQ(configuration.entries.size(), 4);

  EXPECT_PAGE(configuration, "self", title, "Self");
  EXPECT_PAGE(configuration, "self/v1", title, std::nullopt);
  EXPECT_COLLECTION(configuration, "self/v1/schemas", absolute_path,
                    std::filesystem::path{SELF_DIRECTORY} / "v1" / "schemas");

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

  EXPECT_PRIORITY(configuration, "self/v1/schemas", 0);
  EXPECT_PRIORITY(configuration, "example", 50);
}

TEST(valid_004) {
  const auto configuration_path{std::filesystem::path{STUB_DIRECTORY} /
                                "parse_valid_004.json"};
  const auto raw_configuration{
      sourcemeta::one::Configuration::read(configuration_path, SELF_DIRECTORY)};
  const auto configuration{sourcemeta::one::Configuration::parse(
      raw_configuration, configuration_path, configuration_path.parent_path())};

  EXPECT_EQ(configuration.path, configuration_path);

  EXPECT_EQ(configuration.url, "http://localhost:8000");
  EXPECT_TRUE(configuration.api);

  EXPECT_TRUE(configuration.html.has_value());
  EXPECT_EQ(configuration.html.value().name, "Title");
  EXPECT_EQ(configuration.html.value().description, "Description");
  EXPECT_FALSE(configuration.html.value().head.has_value());
  EXPECT_FALSE(configuration.html.value().hero.has_value());
  EXPECT_FALSE(configuration.html.value().action.has_value());

  EXPECT_EQ(configuration.entries.size(), 4);

  EXPECT_PAGE(configuration, "self", title, "Self");
  EXPECT_PAGE(configuration, "self/v1", title, std::nullopt);
  EXPECT_COLLECTION(configuration, "self/v1/schemas", absolute_path,
                    std::filesystem::path{SELF_DIRECTORY} / "v1" / "schemas");

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
  EXPECT_COLLECTION(configuration, "example", extra.size(), 1);
  EXPECT_COLLECTION(configuration, "example",
                    extra.defines("x-sourcemeta-one:path"), true);
  EXPECT_COLLECTION(configuration, "example", extra.at("x-sourcemeta-one:path"),
                    sourcemeta::core::JSON{configuration_path.string()});

  EXPECT_PRIORITY(configuration, "self/v1/schemas", 0);
  EXPECT_PRIORITY(configuration, "example", 50);
}

TEST(valid_005) {
  const auto configuration_path{std::filesystem::path{STUB_DIRECTORY} /
                                "parse_valid_005.json"};
  const auto raw_configuration{
      sourcemeta::one::Configuration::read(configuration_path, SELF_DIRECTORY)};
  const auto configuration{sourcemeta::one::Configuration::parse(
      raw_configuration, configuration_path, configuration_path.parent_path())};

  EXPECT_EQ(configuration.path, configuration_path);

  EXPECT_EQ(configuration.url, "http://localhost:8000");
  EXPECT_TRUE(configuration.api);

  EXPECT_TRUE(configuration.html.has_value());
  EXPECT_EQ(configuration.html.value().name, "Title");
  EXPECT_EQ(configuration.html.value().description, "Description");

  EXPECT_EQ(configuration.entries.size(), 4);

  EXPECT_PAGE(configuration, "self", title, "Self");
  EXPECT_PAGE(configuration, "self/v1", title, std::nullopt);
  EXPECT_COLLECTION(configuration, "self/v1/schemas", absolute_path,
                    std::filesystem::path{SELF_DIRECTORY} / "v1" / "schemas");

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
  EXPECT_COLLECTION(configuration, "example", extra.size(), 1);
  EXPECT_COLLECTION(configuration, "example",
                    extra.defines("x-sourcemeta-one:path"), true);
  EXPECT_COLLECTION(configuration, "example", extra.at("x-sourcemeta-one:path"),
                    sourcemeta::core::JSON{configuration_path.string()});

  EXPECT_PRIORITY(configuration, "self/v1/schemas", 0);
  EXPECT_PRIORITY(configuration, "example", 50);
}

TEST(valid_006) {
  const auto configuration_path{std::filesystem::path{STUB_DIRECTORY} /
                                "parse_valid_006.json"};
  const auto raw_configuration{
      sourcemeta::one::Configuration::read(configuration_path, SELF_DIRECTORY)};
  const auto configuration{sourcemeta::one::Configuration::parse(
      raw_configuration, configuration_path, configuration_path.parent_path())};

  EXPECT_EQ(configuration.path, configuration_path);

  EXPECT_EQ(configuration.url, "http://localhost:8000");
  EXPECT_TRUE(configuration.api);

  EXPECT_TRUE(configuration.html.has_value());
  EXPECT_EQ(configuration.html.value().name, "Title");
  EXPECT_EQ(configuration.html.value().description, "Description");

  EXPECT_EQ(configuration.entries.size(), 4);

  EXPECT_PAGE(configuration, "self", title, "Self");
  EXPECT_PAGE(configuration, "self/v1", title, std::nullopt);
  EXPECT_COLLECTION(configuration, "self/v1/schemas", absolute_path,
                    std::filesystem::path{SELF_DIRECTORY} / "v1" / "schemas");

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
  EXPECT_COLLECTION(configuration, "example", extra.size(), 1);
  EXPECT_COLLECTION(configuration, "example",
                    extra.defines("x-sourcemeta-one:path"), true);
  EXPECT_COLLECTION(configuration, "example", extra.at("x-sourcemeta-one:path"),
                    sourcemeta::core::JSON{configuration_path.string()});

  EXPECT_PRIORITY(configuration, "self/v1/schemas", 0);
  EXPECT_PRIORITY(configuration, "example", 50);
}

TEST(valid_007) {
  const auto configuration_path{std::filesystem::path{STUB_DIRECTORY} /
                                "parse_valid_007.json"};
  const auto raw_configuration{
      sourcemeta::one::Configuration::read(configuration_path, SELF_DIRECTORY)};
  const auto configuration{sourcemeta::one::Configuration::parse(
      raw_configuration, configuration_path, configuration_path.parent_path())};

  EXPECT_EQ(configuration.path, configuration_path);

  EXPECT_EQ(configuration.url, "http://localhost:8000");
  EXPECT_TRUE(configuration.api);

  EXPECT_TRUE(configuration.html.has_value());
  EXPECT_EQ(configuration.html.value().name, "Title");
  EXPECT_EQ(configuration.html.value().description, "Description");

  EXPECT_EQ(configuration.entries.size(), 4);

  EXPECT_PAGE(configuration, "self", title, "Self");
  EXPECT_PAGE(configuration, "self/v1", title, std::nullopt);
  EXPECT_COLLECTION(configuration, "self/v1/schemas", absolute_path,
                    std::filesystem::path{SELF_DIRECTORY} / "v1" / "schemas");

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
  EXPECT_COLLECTION(configuration, "example", extra.size(), 1);
  EXPECT_COLLECTION(configuration, "example",
                    extra.defines("x-sourcemeta-one:path"), true);
  EXPECT_COLLECTION(configuration, "example", extra.at("x-sourcemeta-one:path"),
                    sourcemeta::core::JSON{configuration_path.string()});

  EXPECT_PRIORITY(configuration, "self/v1/schemas", 0);
  EXPECT_PRIORITY(configuration, "example", 50);
}

TEST(valid_008) {
  const auto configuration_path{std::filesystem::path{STUB_DIRECTORY} /
                                "parse_valid_008.json"};
  const auto raw_configuration{
      sourcemeta::one::Configuration::read(configuration_path, SELF_DIRECTORY)};
  const auto configuration{sourcemeta::one::Configuration::parse(
      raw_configuration, configuration_path, configuration_path.parent_path())};

  EXPECT_EQ(configuration.path, configuration_path);

  EXPECT_EQ(configuration.url, "http://localhost:8000");
  EXPECT_TRUE(configuration.api);

  EXPECT_TRUE(configuration.html.has_value());
  EXPECT_EQ(configuration.html.value().name, "Title");
  EXPECT_EQ(configuration.html.value().description, "Description");

  EXPECT_EQ(configuration.entries.size(), 4);

  EXPECT_PAGE(configuration, "self", title, "Self");
  EXPECT_PAGE(configuration, "self/v1", title, std::nullopt);
  EXPECT_COLLECTION(configuration, "self/v1/schemas", absolute_path,
                    std::filesystem::path{SELF_DIRECTORY} / "v1" / "schemas");

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

  EXPECT_PRIORITY(configuration, "self/v1/schemas", 0);
  EXPECT_PRIORITY(configuration, "example", 50);
}

TEST(base_path_none) {
  const auto configuration_path{std::filesystem::path{STUB_DIRECTORY} /
                                "base_path_none.json"};
  const auto raw_configuration{
      sourcemeta::one::Configuration::read(configuration_path, SELF_DIRECTORY)};
  const auto configuration{sourcemeta::one::Configuration::parse(
      raw_configuration, configuration_path, configuration_path.parent_path())};

  EXPECT_EQ(configuration.path, configuration_path);
  EXPECT_EQ(configuration.base_path, "");
  EXPECT_EQ(configuration.origin, "http://localhost:8000");
}

TEST(base_path_slash) {
  const auto configuration_path{std::filesystem::path{STUB_DIRECTORY} /
                                "base_path_slash.json"};
  const auto raw_configuration{
      sourcemeta::one::Configuration::read(configuration_path, SELF_DIRECTORY)};
  const auto configuration{sourcemeta::one::Configuration::parse(
      raw_configuration, configuration_path, configuration_path.parent_path())};

  EXPECT_EQ(configuration.path, configuration_path);
  EXPECT_EQ(configuration.base_path, "");
  EXPECT_EQ(configuration.origin, "http://localhost:8000");
}

TEST(base_path_simple) {
  const auto configuration_path{std::filesystem::path{STUB_DIRECTORY} /
                                "base_path_simple.json"};
  const auto raw_configuration{
      sourcemeta::one::Configuration::read(configuration_path, SELF_DIRECTORY)};
  const auto configuration{sourcemeta::one::Configuration::parse(
      raw_configuration, configuration_path, configuration_path.parent_path())};

  EXPECT_EQ(configuration.path, configuration_path);
  EXPECT_EQ(configuration.base_path, "/v1/catalog");
  EXPECT_EQ(configuration.origin, "http://localhost:8000");
}

TEST(base_path_trailing_slash) {
  const auto configuration_path{std::filesystem::path{STUB_DIRECTORY} /
                                "base_path_trailing_slash.json"};
  const auto raw_configuration{
      sourcemeta::one::Configuration::read(configuration_path, SELF_DIRECTORY)};
  const auto configuration{sourcemeta::one::Configuration::parse(
      raw_configuration, configuration_path, configuration_path.parent_path())};

  EXPECT_EQ(configuration.path, configuration_path);
  EXPECT_EQ(configuration.base_path, "/v1/catalog");
  EXPECT_EQ(configuration.origin, "http://localhost:8000");
}

TEST(base_path_deep) {
  const auto configuration_path{std::filesystem::path{STUB_DIRECTORY} /
                                "base_path_deep.json"};
  const auto raw_configuration{
      sourcemeta::one::Configuration::read(configuration_path, SELF_DIRECTORY)};
  const auto configuration{sourcemeta::one::Configuration::parse(
      raw_configuration, configuration_path, configuration_path.parent_path())};

  EXPECT_EQ(configuration.path, configuration_path);
  EXPECT_EQ(configuration.base_path, "/api/v2/schemas");
  EXPECT_EQ(configuration.origin, "http://localhost:8000");
}

TEST(origin_https_default_port) {
  const auto configuration_path{std::filesystem::path{STUB_DIRECTORY} /
                                "origin_https_default_port.json"};
  const auto raw_configuration{
      sourcemeta::one::Configuration::read(configuration_path, SELF_DIRECTORY)};
  const auto configuration{sourcemeta::one::Configuration::parse(
      raw_configuration, configuration_path, configuration_path.parent_path())};

  EXPECT_EQ(configuration.path, configuration_path);
  EXPECT_EQ(configuration.url, "https://example.com/schemas");
  EXPECT_EQ(configuration.base_path, "/schemas");
  EXPECT_EQ(configuration.origin, "https://example.com");
}

TEST(origin_custom_port) {
  const auto configuration_path{std::filesystem::path{STUB_DIRECTORY} /
                                "origin_custom_port.json"};
  const auto raw_configuration{
      sourcemeta::one::Configuration::read(configuration_path, SELF_DIRECTORY)};
  const auto configuration{sourcemeta::one::Configuration::parse(
      raw_configuration, configuration_path, configuration_path.parent_path())};

  EXPECT_EQ(configuration.path, configuration_path);
  EXPECT_EQ(configuration.url, "https://example.com:9443/api");
  EXPECT_EQ(configuration.base_path, "/api");
  EXPECT_EQ(configuration.origin, "https://example.com:9443");
}

TEST(origin_with_userinfo) {
  const auto configuration_path{std::filesystem::path{STUB_DIRECTORY} /
                                "origin_with_userinfo.json"};
  const auto raw_configuration{
      sourcemeta::one::Configuration::read(configuration_path, SELF_DIRECTORY)};
  const auto configuration{sourcemeta::one::Configuration::parse(
      raw_configuration, configuration_path, configuration_path.parent_path())};

  EXPECT_EQ(configuration.path, configuration_path);
  EXPECT_EQ(configuration.origin, "http://example.com");
}

TEST(origin_with_query) {
  const auto configuration_path{std::filesystem::path{STUB_DIRECTORY} /
                                "origin_with_query.json"};
  const auto raw_configuration{
      sourcemeta::one::Configuration::read(configuration_path, SELF_DIRECTORY)};
  const auto configuration{sourcemeta::one::Configuration::parse(
      raw_configuration, configuration_path, configuration_path.parent_path())};

  EXPECT_EQ(configuration.path, configuration_path);
  EXPECT_EQ(configuration.origin, "http://example.com");
}

TEST(origin_with_fragment) {
  const auto configuration_path{std::filesystem::path{STUB_DIRECTORY} /
                                "origin_with_fragment.json"};
  const auto raw_configuration{
      sourcemeta::one::Configuration::read(configuration_path, SELF_DIRECTORY)};
  const auto configuration{sourcemeta::one::Configuration::parse(
      raw_configuration, configuration_path, configuration_path.parent_path())};

  EXPECT_EQ(configuration.path, configuration_path);
  EXPECT_EQ(configuration.origin, "http://example.com");
}

TEST(origin_with_userinfo_query_fragment_port) {
  const auto configuration_path{
      std::filesystem::path{STUB_DIRECTORY} /
      "origin_with_userinfo_query_fragment_port.json"};
  const auto raw_configuration{
      sourcemeta::one::Configuration::read(configuration_path, SELF_DIRECTORY)};
  const auto configuration{sourcemeta::one::Configuration::parse(
      raw_configuration, configuration_path, configuration_path.parent_path())};

  EXPECT_EQ(configuration.path, configuration_path);
  EXPECT_EQ(configuration.origin, "http://example.com:8000");
}

TEST(is_collection_base_true) {
  const auto configuration_path{std::filesystem::path{STUB_DIRECTORY} /
                                "parse_valid_001.json"};
  const auto raw_configuration{
      sourcemeta::one::Configuration::read(configuration_path, SELF_DIRECTORY)};
  const auto configuration{sourcemeta::one::Configuration::parse(
      raw_configuration, configuration_path, configuration_path.parent_path())};

  EXPECT_EQ(configuration.path, configuration_path);
  EXPECT_TRUE(configuration.is_collection_base("example/extension"));
}

TEST(is_collection_base_false_for_page) {
  const auto configuration_path{std::filesystem::path{STUB_DIRECTORY} /
                                "parse_valid_001.json"};
  const auto raw_configuration{
      sourcemeta::one::Configuration::read(configuration_path, SELF_DIRECTORY)};
  const auto configuration{sourcemeta::one::Configuration::parse(
      raw_configuration, configuration_path, configuration_path.parent_path())};

  EXPECT_EQ(configuration.path, configuration_path);
  EXPECT_FALSE(configuration.is_collection_base("example"));
  EXPECT_FALSE(configuration.is_collection_base("test"));
}

TEST(is_collection_base_false_for_unknown) {
  const auto configuration_path{std::filesystem::path{STUB_DIRECTORY} /
                                "parse_valid_001.json"};
  const auto raw_configuration{
      sourcemeta::one::Configuration::read(configuration_path, SELF_DIRECTORY)};
  const auto configuration{sourcemeta::one::Configuration::parse(
      raw_configuration, configuration_path, configuration_path.parent_path())};

  EXPECT_EQ(configuration.path, configuration_path);
  EXPECT_FALSE(configuration.is_collection_base("nonexistent"));
  EXPECT_FALSE(configuration.is_collection_base("example/nonexistent"));
}

TEST(valid_009_api_enabled) {
  const auto configuration_path{std::filesystem::path{STUB_DIRECTORY} /
                                "parse_valid_009.json"};
  const auto raw_configuration{
      sourcemeta::one::Configuration::read(configuration_path, SELF_DIRECTORY)};
  const auto configuration{sourcemeta::one::Configuration::parse(
      raw_configuration, configuration_path, configuration_path.parent_path())};

  EXPECT_EQ(configuration.path, configuration_path);

  EXPECT_EQ(configuration.url, "http://localhost:8000");
  EXPECT_EQ(configuration.base_path, "");
  EXPECT_TRUE(configuration.api);

  EXPECT_TRUE(configuration.html.has_value());
  EXPECT_EQ(configuration.html.value().name, "Title");
  EXPECT_EQ(configuration.html.value().description, "Description");
  EXPECT_FALSE(configuration.html.value().head.has_value());
  EXPECT_FALSE(configuration.html.value().hero.has_value());
  EXPECT_FALSE(configuration.html.value().action.has_value());

  EXPECT_EQ(configuration.entries.size(), 3);

  EXPECT_PAGE(configuration, "self", title, "Self");
  EXPECT_PAGE(configuration, "self", description,
              "The schemas that define the current version of this instance");
  EXPECT_PAGE(configuration, "self", email, "hello@sourcemeta.com");
  EXPECT_PAGE(configuration, "self", github, "sourcemeta/one");
  EXPECT_PAGE(configuration, "self", website, "https://www.sourcemeta.com");
  EXPECT_PAGE(configuration, "self/v1", title, std::nullopt);
  EXPECT_PAGE(configuration, "self/v1", description, std::nullopt);
  EXPECT_PAGE(configuration, "self/v1", email, std::nullopt);
  EXPECT_PAGE(configuration, "self/v1", github, std::nullopt);
  EXPECT_PAGE(configuration, "self/v1", website, std::nullopt);
  EXPECT_COLLECTION(configuration, "self/v1/schemas", absolute_path,
                    std::filesystem::path{SELF_DIRECTORY} / "v1" / "schemas");

  EXPECT_PRIORITY(configuration, "self/v1/schemas", 0);
}

TEST(valid_010_api_disabled) {
  const auto configuration_path{std::filesystem::path{STUB_DIRECTORY} /
                                "parse_valid_010.json"};
  const auto raw_configuration{
      sourcemeta::one::Configuration::read(configuration_path, SELF_DIRECTORY)};
  const auto configuration{sourcemeta::one::Configuration::parse(
      raw_configuration, configuration_path, configuration_path.parent_path())};

  EXPECT_EQ(configuration.path, configuration_path);

  EXPECT_EQ(configuration.url, "http://localhost:8000");
  EXPECT_EQ(configuration.base_path, "");
  EXPECT_FALSE(configuration.api);

  EXPECT_FALSE(configuration.html.has_value());

  EXPECT_EQ(configuration.entries.size(), 0);
}

TEST(valid_011_priority_explicit) {
  const auto configuration_path{std::filesystem::path{STUB_DIRECTORY} /
                                "parse_valid_011_priority_explicit.json"};
  const auto raw_configuration{
      sourcemeta::one::Configuration::read(configuration_path, SELF_DIRECTORY)};
  const auto configuration{sourcemeta::one::Configuration::parse(
      raw_configuration, configuration_path, configuration_path.parent_path())};

  EXPECT_EQ(configuration.path, configuration_path);

  EXPECT_PRIORITY(configuration, "self/v1/schemas", 0);
  EXPECT_PRIORITY(configuration, "low/schemas", 0);
  EXPECT_PRIORITY(configuration, "mid/schemas", 50);
  EXPECT_PRIORITY(configuration, "high/schemas", 100);
}

TEST(invalid_priority_above_maximum) {
  const auto configuration_path{std::filesystem::path{STUB_DIRECTORY} /
                                "parse_invalid_priority_above.json"};
  const auto raw_configuration{
      sourcemeta::one::Configuration::read(configuration_path, SELF_DIRECTORY)};
  try {
    sourcemeta::one::Configuration::parse(raw_configuration, configuration_path,
                                          configuration_path.parent_path());
    FAIL();
  } catch (const sourcemeta::one::ConfigurationValidationError &error) {
    EXPECT_STREQ(error.what(), "Invalid configuration");
  }
}

TEST(invalid_priority_below_minimum) {
  const auto configuration_path{std::filesystem::path{STUB_DIRECTORY} /
                                "parse_invalid_priority_below.json"};
  const auto raw_configuration{
      sourcemeta::one::Configuration::read(configuration_path, SELF_DIRECTORY)};
  try {
    sourcemeta::one::Configuration::parse(raw_configuration, configuration_path,
                                          configuration_path.parent_path());
    FAIL();
  } catch (const sourcemeta::one::ConfigurationValidationError &error) {
    EXPECT_STREQ(error.what(), "Invalid configuration");
  }
}

TEST(priority_helper_clamps_out_of_range) {
  sourcemeta::one::Configuration::Collection collection;
  collection.extra.assign(
      "x-sourcemeta-one:priority",
      sourcemeta::core::JSON{
          static_cast<sourcemeta::core::JSON::Integer>(200)});
  EXPECT_EQ(sourcemeta::one::Configuration::priority(collection), 100);

  collection.extra.assign(
      "x-sourcemeta-one:priority",
      sourcemeta::core::JSON{static_cast<sourcemeta::core::JSON::Integer>(-5)});
  EXPECT_EQ(sourcemeta::one::Configuration::priority(collection), 0);
}

TEST(priority_helper_defaults_when_missing_or_wrong_type) {
  sourcemeta::one::Configuration::Collection collection;
  EXPECT_EQ(sourcemeta::one::Configuration::priority(collection), 50);

  collection.extra.assign("x-sourcemeta-one:priority",
                          sourcemeta::core::JSON{"high"});
  EXPECT_EQ(sourcemeta::one::Configuration::priority(collection), 50);
}

TEST(authentication_defaults_to_empty_when_absent) {
  const auto configuration_path{std::filesystem::path{STUB_DIRECTORY} /
                                "parse_valid_001.json"};
  const auto raw_configuration{
      sourcemeta::one::Configuration::read(configuration_path, SELF_DIRECTORY)};
  const auto configuration{sourcemeta::one::Configuration::parse(
      raw_configuration, configuration_path, configuration_path.parent_path())};

  EXPECT_EQ(configuration.path, configuration_path);

  // Absent authentication leaves no policies
  EXPECT_EQ(configuration.authentication.size(), 0);
}

TEST(authentication_inherited_from_extends) {
  const auto configuration_path{std::filesystem::path{STUB_DIRECTORY} /
                                "parse_valid_authentication_extends.json"};
  const auto raw_configuration{
      sourcemeta::one::Configuration::read(configuration_path, SELF_DIRECTORY)};
  const auto configuration{sourcemeta::one::Configuration::parse(
      raw_configuration, configuration_path, configuration_path.parent_path())};

  EXPECT_EQ(configuration.path, configuration_path);

  EXPECT_EQ(configuration.authentication.size(), 1);
  EXPECT_EQ(configuration.authentication.at(0).name, "internal");
  EXPECT_EQ(configuration.authentication.at(0).paths,
            (std::vector<sourcemeta::core::JSON::String>{"/internal"}));
}

TEST(authentication_explicit_paths) {
  const auto raw_configuration{sourcemeta::core::parse_json(R"JSON({
    "url": "https://example.com",
    "authentication": [
      {
        "type": "apiKey",
        "algorithm": "identity",
        "name": "internal",
        "paths": [ "/internal", "/vendor" ],
        "keys": [ { "environmentVariable": "ONE_KEY" } ]
      }
    ]
  })JSON")};
  const auto configuration{sourcemeta::one::Configuration::parse(
      raw_configuration, "/tmp/one.json", ".")};

  EXPECT_EQ(configuration.path, "/tmp/one.json");
  EXPECT_EQ(configuration.authentication.size(), 1);
  EXPECT_EQ(
      configuration.authentication.at(0).paths,
      (std::vector<sourcemeta::core::JSON::String>{"/internal", "/vendor"}));
}

TEST(authentication_apikey_identity) {
  const auto raw_configuration{sourcemeta::core::parse_json(R"JSON({
    "url": "https://example.com",
    "authentication": [
      {
        "type": "apiKey",
        "algorithm": "identity",
        "name": "internal",
        "paths": [ "/internal" ],
        "keys": [
          { "environmentVariable": "ONE_KEY_A" },
          { "environmentVariable": "ONE_KEY_B" }
        ]
      }
    ]
  })JSON")};
  const auto configuration{sourcemeta::one::Configuration::parse(
      raw_configuration, "/tmp/one.json", ".")};

  EXPECT_EQ(configuration.path, "/tmp/one.json");
  EXPECT_EQ(configuration.authentication.size(), 1);
  EXPECT_EQ(configuration.authentication.at(0).name, "internal");
  EXPECT_EQ(configuration.authentication.at(0).paths,
            (std::vector<sourcemeta::core::JSON::String>{"/internal"}));
  EXPECT_EQ(
      configuration.authentication.at(0).keys,
      (std::vector<sourcemeta::core::JSON::String>{"ONE_KEY_A", "ONE_KEY_B"}));
}

TEST(authentication_rejects_apikey_without_keys) {
  const auto raw_configuration{sourcemeta::core::parse_json(R"JSON({
    "url": "https://example.com",
    "authentication": [
      { "type": "apiKey", "algorithm": "identity", "paths": [ "/internal" ] }
    ]
  })JSON")};
  try {
    sourcemeta::one::Configuration::parse(raw_configuration, "/tmp/one.json",
                                          ".");
    FAIL();
  } catch (const sourcemeta::one::ConfigurationValidationError &error) {
    EXPECT_STREQ(error.what(), "Invalid configuration");
  }
}

TEST(authentication_rejects_duplicate_name) {
  const auto raw_configuration{sourcemeta::core::parse_json(R"JSON({
    "url": "https://example.com",
    "authentication": [
      {
        "type": "apiKey",
        "algorithm": "identity",
        "name": "data-team",
        "paths": [ "/alpha" ],
        "keys": [ { "environmentVariable": "ONE_KEY_A" } ]
      },
      {
        "type": "apiKey",
        "algorithm": "identity",
        "name": "data-team",
        "paths": [ "/beta" ],
        "keys": [ { "environmentVariable": "ONE_KEY_B" } ]
      }
    ]
  })JSON")};
  try {
    sourcemeta::one::Configuration::parse(raw_configuration, "/tmp/one.json",
                                          ".");
    FAIL();
  } catch (const sourcemeta::one::ConfigurationDuplicateAuthenticationNameError
               &error) {
    EXPECT_STREQ(error.what(),
                 "An authentication policy name is used more than once");
  }
}

TEST(authentication_rejects_reserved_name) {
  const auto raw_configuration{sourcemeta::core::parse_json(R"JSON({
    "url": "https://example.com",
    "authentication": [
      {
        "type": "apiKey",
        "algorithm": "identity",
        "name": "public",
        "paths": [ "/internal" ],
        "keys": [ { "environmentVariable": "ONE_KEY" } ]
      }
    ]
  })JSON")};
  try {
    sourcemeta::one::Configuration::parse(raw_configuration, "/tmp/one.json",
                                          ".");
    FAIL();
  } catch (const sourcemeta::one::ConfigurationReservedAuthenticationNameError
               &error) {
    EXPECT_STREQ(error.what(),
                 "An authentication policy may not use a reserved name");
  }
}

TEST(covers_entry_root_is_above_everything) {
  const auto raw_configuration{sourcemeta::core::parse_json(R"JSON({
    "url": "https://example.com",
    "contents": {
      "alpha": { "path": "./alpha" },
      "team": {
        "title": "Team",
        "contents": {
          "private": { "path": "./team/private" },
          "public": { "path": "./team/public" }
        }
      }
    }
  })JSON")};
  const auto configuration{sourcemeta::one::Configuration::parse(
      raw_configuration, "/tmp/one.json", "/tmp")};
  EXPECT_TRUE(configuration.covers_entry("/"));
}

TEST(covers_entry_an_exact_collection) {
  const auto raw_configuration{sourcemeta::core::parse_json(R"JSON({
    "url": "https://example.com",
    "contents": {
      "alpha": { "path": "./alpha" },
      "team": {
        "title": "Team",
        "contents": {
          "private": { "path": "./team/private" },
          "public": { "path": "./team/public" }
        }
      }
    }
  })JSON")};
  const auto configuration{sourcemeta::one::Configuration::parse(
      raw_configuration, "/tmp/one.json", "/tmp")};
  EXPECT_TRUE(configuration.covers_entry("/alpha"));
}

TEST(covers_entry_an_exact_page) {
  const auto raw_configuration{sourcemeta::core::parse_json(R"JSON({
    "url": "https://example.com",
    "contents": {
      "alpha": { "path": "./alpha" },
      "team": {
        "title": "Team",
        "contents": {
          "private": { "path": "./team/private" },
          "public": { "path": "./team/public" }
        }
      }
    }
  })JSON")};
  const auto configuration{sourcemeta::one::Configuration::parse(
      raw_configuration, "/tmp/one.json", "/tmp")};
  EXPECT_TRUE(configuration.covers_entry("/team"));
}

TEST(covers_entry_a_nested_collection) {
  const auto raw_configuration{sourcemeta::core::parse_json(R"JSON({
    "url": "https://example.com",
    "contents": {
      "alpha": { "path": "./alpha" },
      "team": {
        "title": "Team",
        "contents": {
          "private": { "path": "./team/private" },
          "public": { "path": "./team/public" }
        }
      }
    }
  })JSON")};
  const auto configuration{sourcemeta::one::Configuration::parse(
      raw_configuration, "/tmp/one.json", "/tmp")};
  EXPECT_TRUE(configuration.covers_entry("/team/private"));
  EXPECT_TRUE(configuration.covers_entry("/team/public"));
}

TEST(covers_entry_rejects_a_trailing_slash) {
  const auto raw_configuration{sourcemeta::core::parse_json(R"JSON({
    "url": "https://example.com",
    "contents": {
      "alpha": { "path": "./alpha" },
      "team": {
        "title": "Team",
        "contents": {
          "private": { "path": "./team/private" },
          "public": { "path": "./team/public" }
        }
      }
    }
  })JSON")};
  const auto configuration{sourcemeta::one::Configuration::parse(
      raw_configuration, "/tmp/one.json", "/tmp")};
  EXPECT_FALSE(configuration.covers_entry("/alpha/"));
  EXPECT_FALSE(configuration.covers_entry("/team/private/"));
}

TEST(covers_entry_rejects_a_path_inside_a_collection) {
  const auto raw_configuration{sourcemeta::core::parse_json(R"JSON({
    "url": "https://example.com",
    "contents": {
      "alpha": { "path": "./alpha" },
      "team": {
        "title": "Team",
        "contents": {
          "private": { "path": "./team/private" },
          "public": { "path": "./team/public" }
        }
      }
    }
  })JSON")};
  const auto configuration{sourcemeta::one::Configuration::parse(
      raw_configuration, "/tmp/one.json", "/tmp")};
  EXPECT_FALSE(configuration.covers_entry("/alpha/schema"));
  EXPECT_FALSE(configuration.covers_entry("/team/private/secret"));
}

TEST(covers_entry_rejects_an_unknown_top_level_path) {
  const auto raw_configuration{sourcemeta::core::parse_json(R"JSON({
    "url": "https://example.com",
    "contents": {
      "alpha": { "path": "./alpha" },
      "team": {
        "title": "Team",
        "contents": {
          "private": { "path": "./team/private" },
          "public": { "path": "./team/public" }
        }
      }
    }
  })JSON")};
  const auto configuration{sourcemeta::one::Configuration::parse(
      raw_configuration, "/tmp/one.json", "/tmp")};
  EXPECT_FALSE(configuration.covers_entry("/beta"));
}

TEST(covers_entry_rejects_a_partial_segment) {
  const auto raw_configuration{sourcemeta::core::parse_json(R"JSON({
    "url": "https://example.com",
    "contents": {
      "alpha": { "path": "./alpha" },
      "team": {
        "title": "Team",
        "contents": {
          "private": { "path": "./team/private" },
          "public": { "path": "./team/public" }
        }
      }
    }
  })JSON")};
  const auto configuration{sourcemeta::one::Configuration::parse(
      raw_configuration, "/tmp/one.json", "/tmp")};
  EXPECT_FALSE(configuration.covers_entry("/alph"));
  EXPECT_FALSE(configuration.covers_entry("/team/priv"));
}

TEST(covers_entry_with_no_entries_only_covers_the_root) {
  const auto raw_configuration{sourcemeta::core::parse_json(R"JSON({
    "url": "https://example.com"
  })JSON")};
  const auto configuration{sourcemeta::one::Configuration::parse(
      raw_configuration, "/tmp/one.json", ".")};
  EXPECT_TRUE(configuration.covers_entry("/"));
  EXPECT_FALSE(configuration.covers_entry("/alpha"));
}

TEST(authentication_rejects_apikey_without_algorithm) {
  const auto raw_configuration{sourcemeta::core::parse_json(R"JSON({
    "url": "https://example.com",
    "authentication": [
      {
        "type": "apiKey",
        "paths": [ "/internal" ],
        "keys": [ { "environmentVariable": "ONE_KEY" } ]
      }
    ]
  })JSON")};
  try {
    sourcemeta::one::Configuration::parse(raw_configuration, "/tmp/one.json",
                                          ".");
    FAIL();
  } catch (const sourcemeta::one::ConfigurationValidationError &error) {
    EXPECT_STREQ(error.what(), "Invalid configuration");
  }
}

TEST(authentication_rejects_apikey_non_identity_algorithm) {
  const auto raw_configuration{sourcemeta::core::parse_json(R"JSON({
    "url": "https://example.com",
    "authentication": [
      {
        "type": "apiKey",
        "algorithm": "sha256",
        "paths": [ "/internal" ],
        "keys": [ { "environmentVariable": "ONE_KEY" } ]
      }
    ]
  })JSON")};
  try {
    sourcemeta::one::Configuration::parse(raw_configuration, "/tmp/one.json",
                                          ".");
    FAIL();
  } catch (const sourcemeta::one::ConfigurationValidationError &error) {
    EXPECT_STREQ(error.what(), "Invalid configuration");
  }
}

TEST(authentication_accepts_empty_array) {
  const auto raw_configuration{sourcemeta::core::parse_json(R"JSON({
    "url": "https://example.com",
    "authentication": []
  })JSON")};
  const auto configuration{sourcemeta::one::Configuration::parse(
      raw_configuration, "/tmp/one.json", ".")};
  EXPECT_EQ(configuration.authentication.size(), 0);
}

TEST(authentication_rejects_duplicate_entries) {
  const auto raw_configuration{sourcemeta::core::parse_json(R"JSON({
    "url": "https://example.com",
    "authentication": [
      {
        "type": "apiKey",
        "algorithm": "identity",
        "name": "team",
        "paths": [ "/a" ],
        "keys": [ { "environmentVariable": "K" } ]
      },
      {
        "type": "apiKey",
        "algorithm": "identity",
        "name": "team",
        "paths": [ "/a" ],
        "keys": [ { "environmentVariable": "K" } ]
      }
    ]
  })JSON")};
  try {
    sourcemeta::one::Configuration::parse(raw_configuration, "/tmp/one.json",
                                          ".");
    FAIL();
  } catch (const sourcemeta::one::ConfigurationValidationError &error) {
    EXPECT_STREQ(error.what(), "Invalid configuration");
  }
}

TEST(authentication_rejects_missing_paths) {
  const auto raw_configuration{sourcemeta::core::parse_json(R"JSON({
    "url": "https://example.com",
    "authentication": [
      {
        "type": "apiKey",
        "algorithm": "identity",
        "name": "team",
        "keys": [ { "environmentVariable": "K" } ]
      }
    ]
  })JSON")};
  try {
    sourcemeta::one::Configuration::parse(raw_configuration, "/tmp/one.json",
                                          ".");
    FAIL();
  } catch (const sourcemeta::one::ConfigurationValidationError &error) {
    EXPECT_STREQ(error.what(), "Invalid configuration");
  }
}

TEST(authentication_rejects_empty_path) {
  const auto raw_configuration{sourcemeta::core::parse_json(R"JSON({
    "url": "https://example.com",
    "authentication": [
      {
        "type": "apiKey",
        "algorithm": "identity",
        "name": "team",
        "paths": [ "" ],
        "keys": [ { "environmentVariable": "K" } ]
      }
    ]
  })JSON")};
  try {
    sourcemeta::one::Configuration::parse(raw_configuration, "/tmp/one.json",
                                          ".");
    FAIL();
  } catch (const sourcemeta::one::ConfigurationValidationError &error) {
    EXPECT_STREQ(error.what(), "Invalid configuration");
  }
}

TEST(authentication_rejects_path_without_leading_slash) {
  const auto raw_configuration{sourcemeta::core::parse_json(R"JSON({
    "url": "https://example.com",
    "authentication": [
      {
        "type": "apiKey",
        "algorithm": "identity",
        "name": "team",
        "paths": [ "acme" ],
        "keys": [ { "environmentVariable": "K" } ]
      }
    ]
  })JSON")};
  try {
    sourcemeta::one::Configuration::parse(raw_configuration, "/tmp/one.json",
                                          ".");
    FAIL();
  } catch (const sourcemeta::one::ConfigurationValidationError &error) {
    EXPECT_STREQ(error.what(), "Invalid configuration");
  }
}

TEST(authentication_rejects_unknown_type) {
  const auto raw_configuration{sourcemeta::core::parse_json(R"JSON({
    "url": "https://example.com",
    "authentication": [
      { "type": "apiKey", "paths": [ "/" ] }
    ]
  })JSON")};
  try {
    sourcemeta::one::Configuration::parse(raw_configuration, "/tmp/one.json",
                                          ".");
    FAIL();
  } catch (const sourcemeta::one::ConfigurationValidationError &error) {
    EXPECT_STREQ(error.what(), "Invalid configuration");
  }
}

TEST(authentication_accepts_maximum_entries) {
  auto raw_configuration{sourcemeta::core::JSON::make_object()};
  raw_configuration.assign("url",
                           sourcemeta::core::JSON{"https://example.com"});
  auto entries{sourcemeta::core::JSON::make_array()};
  for (std::size_t index = 0; index < 64; ++index) {
    auto entry{sourcemeta::core::JSON::make_object()};
    entry.assign("type", sourcemeta::core::JSON{"apiKey"});
    entry.assign("algorithm", sourcemeta::core::JSON{"identity"});
    entry.assign("name", sourcemeta::core::JSON{"p" + std::to_string(index)});
    auto paths{sourcemeta::core::JSON::make_array()};
    paths.push_back(sourcemeta::core::JSON{"/p" + std::to_string(index)});
    entry.assign("paths", std::move(paths));
    auto keys{sourcemeta::core::JSON::make_array()};
    auto key{sourcemeta::core::JSON::make_object()};
    key.assign("environmentVariable",
               sourcemeta::core::JSON{"K" + std::to_string(index)});
    keys.push_back(std::move(key));
    entry.assign("keys", std::move(keys));
    entries.push_back(std::move(entry));
  }
  raw_configuration.assign("authentication", std::move(entries));

  const auto configuration{sourcemeta::one::Configuration::parse(
      raw_configuration, "/tmp/one.json", ".")};
  EXPECT_EQ(configuration.path, "/tmp/one.json");
  EXPECT_EQ(configuration.authentication.size(), 64);
}

TEST(authentication_rejects_above_maximum_entries) {
  auto raw_configuration{sourcemeta::core::JSON::make_object()};
  raw_configuration.assign("url",
                           sourcemeta::core::JSON{"https://example.com"});
  auto entries{sourcemeta::core::JSON::make_array()};
  for (std::size_t index = 0; index < 65; ++index) {
    auto entry{sourcemeta::core::JSON::make_object()};
    entry.assign("type", sourcemeta::core::JSON{"apiKey"});
    entry.assign("algorithm", sourcemeta::core::JSON{"identity"});
    entry.assign("name", sourcemeta::core::JSON{"p" + std::to_string(index)});
    auto paths{sourcemeta::core::JSON::make_array()};
    paths.push_back(sourcemeta::core::JSON{"/p" + std::to_string(index)});
    entry.assign("paths", std::move(paths));
    auto keys{sourcemeta::core::JSON::make_array()};
    auto key{sourcemeta::core::JSON::make_object()};
    key.assign("environmentVariable",
               sourcemeta::core::JSON{"K" + std::to_string(index)});
    keys.push_back(std::move(key));
    entry.assign("keys", std::move(keys));
    entries.push_back(std::move(entry));
  }
  raw_configuration.assign("authentication", std::move(entries));

  try {
    sourcemeta::one::Configuration::parse(raw_configuration, "/tmp/one.json",
                                          ".");
    FAIL();
  } catch (const sourcemeta::one::ConfigurationValidationError &error) {
    EXPECT_STREQ(error.what(), "Invalid configuration");
  }
}
