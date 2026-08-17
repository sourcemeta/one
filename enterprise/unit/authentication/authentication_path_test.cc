#include <sourcemeta/core/test.h>
#include <sourcemeta/one/authentication.h>

#include <optional>    // std::optional
#include <string>      // std::string
#include <string_view> // std::string_view

static constexpr std::string_view INSTANCE{"http://localhost:8000"};

static auto parse(const std::string_view input,
                  const std::string_view instance = INSTANCE)
    -> std::optional<sourcemeta::one::Authentication::Path> {
  return sourcemeta::one::Authentication::Path::parse(input, instance);
}

// Returned by value: a view would dangle into the parsed path, which lives
// only for this call
static auto value(const std::string_view input,
                  const std::string_view instance = INSTANCE) -> std::string {
  const auto result{parse(input, instance)};
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

TEST(canonicalising_a_representation_twice_changes_nothing) {
  EXPECT_EQ(value(value("/private/secret.json")), "private/secret.json");
}

TEST(canonicalising_repeated_separators_twice_changes_nothing) {
  EXPECT_EQ(value(value("//PRIVATE//secret")), "private/secret");
}

TEST(canonicalising_relative_segments_twice_changes_nothing) {
  EXPECT_EQ(value(value("/./a/../b/c.JSON")), "b/c.json");
}

TEST(canonicalising_the_root_twice_changes_nothing) {
  EXPECT_EQ(value(value("/")), "");
}

TEST(canonicalising_a_trailing_separator_twice_changes_nothing) {
  EXPECT_EQ(value(value("/a/b/")), "a/b");
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

TEST(a_repeated_leading_separator_with_upper_case_names_the_location) {
  EXPECT_EQ(value("//PRIVATE/secret"), "private/secret");
}

TEST(an_upper_cased_prefix_names_the_location) {
  EXPECT_EQ(value("/PRIVATE/secret"), "private/secret");
}

TEST(a_repeated_leading_separator_names_the_location) {
  EXPECT_EQ(value("//private/secret"), "private/secret");
}

TEST(repeated_separators_throughout_with_upper_case_name_the_location) {
  EXPECT_EQ(value("///PRIVATE///SECRET"), "private/secret");
}

TEST(a_leading_dot_segment_with_upper_case_names_the_location) {
  EXPECT_EQ(value("/./PRIVATE/secret"), "private/secret");
}

TEST(climbing_back_into_an_upper_cased_prefix_names_the_location) {
  EXPECT_EQ(value("/private/../PRIVATE/secret"), "private/secret");
}

TEST(an_escaped_letter_in_the_prefix_names_the_location) {
  EXPECT_EQ(value("/%50RIVATE/secret"), "private/secret");
}

TEST(climbing_from_a_public_prefix_into_an_upper_cased_one_names_the_location) {
  EXPECT_EQ(value("/public/../PRIVATE/secret"), "private/secret");
}

TEST(two_paths_compare_by_their_canonical_spelling) {
  EXPECT_TRUE(parse("/private/secret").value() ==
              parse("//PRIVATE/secret").value());
  EXPECT_FALSE(parse("/private/secret").value() ==
               parse("/private/other").value());
}

TEST(input_already_relative_to_the_root_is_canonicalised_as_is) {
  // What the indexer composes names nowhere outside the instance, so it
  // always yields a value
  EXPECT_EQ(sourcemeta::one::Authentication::Path::relative("/private/secret")
                .value(),
            "private/secret");
  EXPECT_EQ(
      sourcemeta::one::Authentication::Path::relative("private/secret").value(),
      "private/secret");
  EXPECT_EQ(sourcemeta::one::Authentication::Path::relative("/").value(), "");
  EXPECT_EQ(sourcemeta::one::Authentication::Path::relative("").value(), "");
}

TEST(a_relative_upper_cased_path_reads_as_the_request_form_does) {
  EXPECT_EQ(sourcemeta::one::Authentication::Path::relative("/PRIVATE/secret")
                .value(),
            "private/secret");
}

TEST(a_relative_path_with_repeated_separators_reads_as_the_request_form_does) {
  EXPECT_EQ(sourcemeta::one::Authentication::Path::relative("//private//secret")
                .value(),
            "private/secret");
}

TEST(a_relative_path_with_a_dot_segment_reads_as_the_request_form_does) {
  EXPECT_EQ(sourcemeta::one::Authentication::Path::relative("/private/./secret")
                .value(),
            "private/secret");
}

TEST(a_relative_path_that_climbs_reads_as_the_request_form_does) {
  EXPECT_EQ(sourcemeta::one::Authentication::Path::relative(
                "/public/../private/secret")
                .value(),
            "private/secret");
}
