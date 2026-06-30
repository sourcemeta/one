#ifndef SOURCEMETA_CORE_TEST_H_
#define SOURCEMETA_CORE_TEST_H_

#ifndef SOURCEMETA_CORE_TEST_EXPORT
#include <sourcemeta/core/test_export.h>
#endif

#include <functional>  // std::function
#include <ostream>     // std::ostream
#include <sstream>     // std::ostringstream
#include <string>      // std::string
#include <string_view> // std::string_view
#include <type_traits> // std::is_integral_v, std::is_same_v, std::remove_cv_t
#include <utility> // std::cmp_equal, std::cmp_not_equal, std::cmp_less, std::cmp_greater, std::cmp_less_equal, std::cmp_greater_equal

/// @defgroup test Test
/// @brief A minimal unit testing framework
///
/// This functionality is included as follows:
///
/// ```cpp
/// #include <sourcemeta/core/test.h>
/// ```

namespace sourcemeta::core {

// Exporting symbols that depends on the standard C++ library is considered
// safe.
// https://learn.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-2-c4275?view=msvc-170&redirectedfrom=MSDN
#if defined(_MSC_VER)
#pragma warning(disable : 4251 4275)
#endif

/// @ingroup test
///
/// Thrown to abort the current test on the first failed expectation. It does
/// not derive from the standard exception hierarchy so that a broad catch block
/// in a test does not accidentally swallow it.
struct SOURCEMETA_CORE_TEST_EXPORT TestAbortError {
  std::string message;
};

/// @ingroup test
///
/// Register a test by name with a callable body. Returns a dummy value so it
/// can be used to initialise a namespace-scope variable.
SOURCEMETA_CORE_TEST_EXPORT
auto test_register(std::string_view suite, std::string_view name,
                   std::string_view file, int line, std::function<void()> body)
    -> int;

/// @ingroup test
///
/// Print a failure at the given location and abort the current test.
[[noreturn]] SOURCEMETA_CORE_TEST_EXPORT auto
test_report_failure(std::string_view file, int line, std::string_view message)
    -> void;

/// @ingroup test
///
/// Derive a suite name from a source file path by taking its stem and stripping
/// a trailing `_test` suffix.
SOURCEMETA_CORE_TEST_EXPORT
auto test_suite_from_path(std::string_view path) -> std::string;

/// @ingroup test
///
/// Run every registered test, optionally filtered, and return a process exit
/// code.
SOURCEMETA_CORE_TEST_EXPORT
auto test_run(int argc, char **argv) -> int;

template <typename Type>
concept test_comparable_integer =
    std::is_integral_v<Type> && !std::is_same_v<std::remove_cv_t<Type>, bool> &&
    !std::is_same_v<std::remove_cv_t<Type>, char> &&
    !std::is_same_v<std::remove_cv_t<Type>, wchar_t> &&
    !std::is_same_v<std::remove_cv_t<Type>, char8_t> &&
    !std::is_same_v<std::remove_cv_t<Type>, char16_t> &&
    !std::is_same_v<std::remove_cv_t<Type>, char32_t>;

template <typename Left, typename Right>
auto test_compare_equal(const Left &left, const Right &right) -> bool {
  if constexpr (test_comparable_integer<Left> &&
                test_comparable_integer<Right>) {
    return std::cmp_equal(left, right);
  } else {
    return left == right;
  }
}

template <typename Left, typename Right>
auto test_compare_not_equal(const Left &left, const Right &right) -> bool {
  if constexpr (test_comparable_integer<Left> &&
                test_comparable_integer<Right>) {
    return std::cmp_not_equal(left, right);
  } else {
    return left != right;
  }
}

template <typename Left, typename Right>
auto test_compare_less(const Left &left, const Right &right) -> bool {
  if constexpr (test_comparable_integer<Left> &&
                test_comparable_integer<Right>) {
    return std::cmp_less(left, right);
  } else {
    return left < right;
  }
}

template <typename Left, typename Right>
auto test_compare_greater(const Left &left, const Right &right) -> bool {
  if constexpr (test_comparable_integer<Left> &&
                test_comparable_integer<Right>) {
    return std::cmp_greater(left, right);
  } else {
    return left > right;
  }
}

template <typename Left, typename Right>
auto test_compare_less_equal(const Left &left, const Right &right) -> bool {
  if constexpr (test_comparable_integer<Left> &&
                test_comparable_integer<Right>) {
    return std::cmp_less_equal(left, right);
  } else {
    return left <= right;
  }
}

template <typename Left, typename Right>
auto test_compare_greater_equal(const Left &left, const Right &right) -> bool {
  if constexpr (test_comparable_integer<Left> &&
                test_comparable_integer<Right>) {
    return std::cmp_greater_equal(left, right);
  } else {
    return left >= right;
  }
}

template <typename Type> auto test_stringify(const Type &value) -> std::string {
  if constexpr (requires(std::ostream &stream) { stream << value; }) {
    std::ostringstream stream;
    stream << value;
    return stream.str();
  } else {
    return "<unprintable>";
  }
}

template <typename Left, typename Right>
auto test_describe_mismatch(std::string_view expression, const Left &left,
                            const Right &right) -> std::string {
  return std::string{"expected "} + std::string{expression} +
         "\n  actual:   " + test_stringify(left) +
         "\n  expected: " + test_stringify(right);
}

} // namespace sourcemeta::core

#define SOURCEMETA_CORE_TEST_REGISTER(name)                                    \
  static auto sourcemeta_test_body_##name()->void;                             \
  [[maybe_unused]] static const int sourcemeta_test_registration_##name =      \
      ::sourcemeta::core::test_register(                                       \
          ::sourcemeta::core::test_suite_from_path(__FILE__), #name, __FILE__, \
          __LINE__, &sourcemeta_test_body_##name);                             \
  static auto sourcemeta_test_body_##name()->void

#define TEST(name) SOURCEMETA_CORE_TEST_REGISTER(name)

#define SOURCEMETA_CORE_TEST_COMPARE(actual, expected, comparator, operation)  \
  do {                                                                         \
    const auto &sourcemeta_test_actual{(actual)};                              \
    const auto &sourcemeta_test_expected{(expected)};                          \
    if (!::sourcemeta::core::comparator(sourcemeta_test_actual,                \
                                        sourcemeta_test_expected)) {           \
      ::sourcemeta::core::test_report_failure(                                 \
          __FILE__, __LINE__,                                                  \
          ::sourcemeta::core::test_describe_mismatch(                          \
              #actual " " operation " " #expected, sourcemeta_test_actual,     \
              sourcemeta_test_expected));                                      \
    }                                                                          \
  } while (false)

#define EXPECT_EQ(actual, expected)                                            \
  SOURCEMETA_CORE_TEST_COMPARE(actual, expected, test_compare_equal, "==")
#define EXPECT_NE(actual, expected)                                            \
  SOURCEMETA_CORE_TEST_COMPARE(actual, expected, test_compare_not_equal, "!=")
#define EXPECT_LT(actual, expected)                                            \
  SOURCEMETA_CORE_TEST_COMPARE(actual, expected, test_compare_less, "<")
#define EXPECT_GT(actual, expected)                                            \
  SOURCEMETA_CORE_TEST_COMPARE(actual, expected, test_compare_greater, ">")
#define EXPECT_LE(actual, expected)                                            \
  SOURCEMETA_CORE_TEST_COMPARE(actual, expected, test_compare_less_equal, "<=")
#define EXPECT_GE(actual, expected)                                            \
  SOURCEMETA_CORE_TEST_COMPARE(actual, expected, test_compare_greater_equal,   \
                               ">=")

#define EXPECT_TRUE(condition)                                                 \
  do {                                                                         \
    if (!(condition)) {                                                        \
      ::sourcemeta::core::test_report_failure(__FILE__, __LINE__,              \
                                              "expected true: " #condition);   \
    }                                                                          \
  } while (false)

#define EXPECT_FALSE(condition)                                                \
  do {                                                                         \
    if ((condition)) {                                                         \
      ::sourcemeta::core::test_report_failure(__FILE__, __LINE__,              \
                                              "expected false: " #condition);  \
    }                                                                          \
  } while (false)

#define FAIL()                                                                 \
  ::sourcemeta::core::test_report_failure(__FILE__, __LINE__,                  \
                                          "explicit failure")

#endif
