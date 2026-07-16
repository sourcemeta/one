#include <sourcemeta/core/test.h>
#include <sourcemeta/one/http_helpers.h>

TEST(is_local_path_accepts_a_rooted_path) {
  EXPECT_TRUE(sourcemeta::one::is_local_path("/"));
  EXPECT_TRUE(sourcemeta::one::is_local_path("/private/secret"));
  EXPECT_TRUE(sourcemeta::one::is_local_path("/registry/console/"));
  EXPECT_TRUE(sourcemeta::one::is_local_path("/a/b?c=d"));
}

TEST(is_local_path_rejects_a_path_that_is_not_rooted) {
  EXPECT_FALSE(sourcemeta::one::is_local_path(""));
  EXPECT_FALSE(sourcemeta::one::is_local_path("private/secret"));
  EXPECT_FALSE(sourcemeta::one::is_local_path("https://evil.example.com"));
  EXPECT_FALSE(sourcemeta::one::is_local_path("javascript:alert(1)"));
}

TEST(is_local_path_rejects_protocol_relative_and_backslash_forms) {
  EXPECT_FALSE(sourcemeta::one::is_local_path("//evil.example.com"));
  EXPECT_FALSE(sourcemeta::one::is_local_path("/\\evil.example.com"));
  EXPECT_FALSE(sourcemeta::one::is_local_path("/foo\\bar"));
}

TEST(is_local_path_rejects_control_characters) {
  EXPECT_FALSE(sourcemeta::one::is_local_path("/foo\nbar"));
  EXPECT_FALSE(sourcemeta::one::is_local_path("/foo\tbar"));
  EXPECT_FALSE(sourcemeta::one::is_local_path("/foo\rbar"));
  EXPECT_FALSE(sourcemeta::one::is_local_path("/foo\x7f"));
}
