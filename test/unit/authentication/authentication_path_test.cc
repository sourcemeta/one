#include <sourcemeta/core/test.h>
#include <sourcemeta/one/authentication.h>

#include <optional>    // std::optional
#include <string>      // std::string
#include <string_view> // std::string_view

static constexpr std::string_view INSTANCE{"http://localhost:8000"};
static constexpr std::string_view BASED{"http://localhost:8000/registry"};

static auto parse(const std::string_view input,
                  const std::string_view instance = INSTANCE,
                  const std::string_view base_path = "")
    -> std::optional<sourcemeta::one::Authentication::Path> {
  return sourcemeta::one::Authentication::Path::parse(input, instance,
                                                      base_path);
}

// Returned by value: a view would dangle into the parsed path, which lives
// only for this call
static auto value(const std::string_view input,
                  const std::string_view instance = INSTANCE,
                  const std::string_view base_path = "") -> std::string {
  const auto result{parse(input, instance, base_path)};
  return result.has_value() ? std::string{result.value().value()}
                            : std::string{"<none>"};
}

TEST(the_instance_root_is_the_empty_path) {
  EXPECT_TRUE(parse("").has_value());
  EXPECT_TRUE(parse("").value().empty());
  EXPECT_EQ(value(""), "");
  EXPECT_EQ(value("/"), "");
}

TEST(an_ordinary_path_keeps_its_segments) {
  EXPECT_EQ(value("/private/secret"), "private/secret");
  EXPECT_EQ(value("private/secret"), "private/secret");
  EXPECT_EQ(value("/a/b/c/d"), "a/b/c/d");
}

TEST(an_extension_is_kept_because_it_names_a_representation) {
  // A policy may gate one representation on its own, so the extension has to
  // survive canonicalisation even though the artifact tree ignores it
  EXPECT_EQ(value("/private/secret.json"), "private/secret.json");
  EXPECT_EQ(value("/private/secret.JSON"), "private/secret.json");
  EXPECT_EQ(value("/private/secret.yaml"), "private/secret.yaml");
  EXPECT_EQ(value("/private/secret"), "private/secret");
}

TEST(spelling_is_lowercased) {
  EXPECT_EQ(value("/PRIVATE/SECRET"), "private/secret");
  EXPECT_EQ(value("/Private/Secret.json"), "private/secret.json");
}

TEST(empty_segments_collapse) {
  EXPECT_EQ(value("//private/secret"), "private/secret");
  EXPECT_EQ(value("/private//secret"), "private/secret");
  EXPECT_EQ(value("///private///secret///"), "private/secret");
  EXPECT_EQ(value("/private/secret/"), "private/secret");
}

TEST(relative_segments_are_resolved) {
  EXPECT_EQ(value("/private/./secret"), "private/secret");
  EXPECT_EQ(value("/./private/secret"), "private/secret");
  EXPECT_EQ(value("/private/../private/secret"), "private/secret");
  EXPECT_EQ(value("/public/../private/secret"), "private/secret");
}

TEST(climbing_above_the_instance_root_stays_at_the_root) {
  EXPECT_EQ(value("/../private/secret"), "private/secret");
  EXPECT_EQ(value("/../../private/secret"), "private/secret");
  EXPECT_EQ(value(".."), "");
}

TEST(an_escaped_unreserved_character_is_decoded) {
  EXPECT_EQ(value("/%70rivate/secret"), "private/secret");
  EXPECT_EQ(value("/PRIVATE/%53ECRET"), "private/secret");
  EXPECT_EQ(value("/public/%2e%2e/private/secret"), "private/secret");
}

TEST(an_escaped_reserved_character_keeps_its_escape) {
  // Decoding these would either invent a separator or rename the location,
  // and the artifact tree stores the escaped spelling
  EXPECT_EQ(value("/test/schemas/%25/test.json"), "test/schemas/%25/test.json");
  EXPECT_EQ(value("/private%2Fsecret"), "private%2fsecret");
  EXPECT_EQ(value("/a/b%3Fc"), "a/b%3fc");
}

TEST(an_escaped_separator_never_becomes_a_separator) {
  // Otherwise one location could be spelled as another
  EXPECT_FALSE(parse("/private%2Fsecret").value() ==
               parse("/private/secret").value());
}

TEST(every_spelling_of_one_location_agrees) {
  const auto canonical{value("/private/secret")};
  EXPECT_EQ(value("/public/../private/secret/"), canonical);
  EXPECT_EQ(value("///private/./secret"), canonical);
  EXPECT_EQ(value("//PRIVATE/secret"), canonical);
  EXPECT_EQ(value("http://localhost:8000/private/secret"), canonical);

  const auto representation{value("/private/secret.json")};
  EXPECT_EQ(value("//PRIVATE/secret.json"), representation);
  EXPECT_EQ(value("/./PRIVATE//secret.JSON"), representation);
  EXPECT_EQ(value("http://localhost:8000/private/secret.json"), representation);
}

TEST(canonicalisation_is_idempotent) {
  for (const std::string_view input :
       {"/private/secret.json", "//PRIVATE//secret", "/./a/../b/c.JSON", "/",
        "/a/b/"}) {
    const auto once{parse(input)};
    EXPECT_TRUE(once.has_value());
    const auto twice{parse(once.value().value())};
    EXPECT_TRUE(twice.has_value());
    EXPECT_EQ(once.value().value(), twice.value().value());
  }
}

TEST(a_url_on_this_instance_reduces_to_its_path) {
  EXPECT_EQ(value("http://localhost:8000/private/secret"), "private/secret");
  EXPECT_EQ(value("http://localhost:8000/"), "");
  EXPECT_EQ(value("http://localhost:8000"), "");
}

TEST(a_url_on_another_origin_names_nowhere_here) {
  EXPECT_FALSE(parse("http://evil.example.com/private/secret").has_value());
  EXPECT_FALSE(parse("https://localhost:8000/private/secret").has_value());
  EXPECT_FALSE(parse("http://localhost:9999/private/secret").has_value());
}

TEST(a_base_path_is_removed) {
  EXPECT_EQ(value("/registry/private/secret", BASED, "/registry"),
            "private/secret");
  EXPECT_EQ(value("/registry/private/secret.json", BASED, "/registry"),
            "private/secret.json");
  EXPECT_EQ(value("/registry", BASED, "/registry"), "");
  EXPECT_EQ(value("/registry/", BASED, "/registry"), "");
}

TEST(a_url_under_a_base_path_reduces_the_same_way) {
  EXPECT_EQ(value("http://localhost:8000/registry/private/secret", BASED,
                  "/registry"),
            "private/secret");
}

TEST(a_path_outside_the_base_path_names_nowhere_here) {
  EXPECT_FALSE(parse("/private/secret", BASED, "/registry").has_value());
  EXPECT_FALSE(parse("/elsewhere/private", BASED, "/registry").has_value());
}

TEST(a_base_path_is_matched_on_whole_segments) {
  EXPECT_FALSE(parse("/registrynot/private", BASED, "/registry").has_value());
}

TEST(hostile_spellings_never_escape_their_location) {
  // Each of these was a gate bypass attempt, and every one has to reduce to
  // the same governed location rather than to something a policy would miss
  for (const std::string_view input :
       {"//PRIVATE/secret", "/PRIVATE/secret", "//private/secret",
        "///PRIVATE///SECRET", "/./PRIVATE/secret",
        "/private/../PRIVATE/secret", "/%50RIVATE/secret",
        "/public/../PRIVATE/secret"}) {
    const auto result{parse(input)};
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result.value().value(), "private/secret");
  }
}

TEST(two_paths_compare_by_their_canonical_spelling) {
  EXPECT_TRUE(parse("/private/secret").value() ==
              parse("//PRIVATE/secret").value());
  EXPECT_FALSE(parse("/private/secret").value() ==
               parse("/private/other").value());
}

TEST(input_already_relative_to_the_root_is_canonicalised_as_is) {
  // What the indexer composes carries no base path and names nowhere outside
  // the instance, so it always yields a value
  EXPECT_EQ(sourcemeta::one::Authentication::Path::relative("/private/secret")
                .value(),
            "private/secret");
  EXPECT_EQ(
      sourcemeta::one::Authentication::Path::relative("private/secret").value(),
      "private/secret");
  EXPECT_EQ(sourcemeta::one::Authentication::Path::relative("/").value(), "");
  EXPECT_EQ(sourcemeta::one::Authentication::Path::relative("").value(), "");
}

TEST(a_relative_path_is_canonicalised_the_same_way_a_request_is) {
  for (const std::string_view input :
       {"/PRIVATE/secret", "//private//secret", "/private/./secret",
        "/public/../private/secret"}) {
    EXPECT_EQ(sourcemeta::one::Authentication::Path::relative(input).value(),
              parse("/private/secret").value().value());
  }
}
