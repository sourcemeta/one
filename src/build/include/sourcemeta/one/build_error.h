#ifndef SOURCEMETA_ONE_BUILD_ERROR_H_
#define SOURCEMETA_ONE_BUILD_ERROR_H_

#include <cassert>    // assert
#include <cstdint>    // std::uint64_t
#include <exception>  // std::exception
#include <filesystem> // std::filesystem::path
#include <utility>    // std::move

namespace sourcemeta::one {

class BuildTooManyDirectoryEntriesError : public std::exception {
public:
  BuildTooManyDirectoryEntriesError(std::filesystem::path path,
                                    std::uint64_t count)
      : path_{std::move(path)}, count_{count} {
    assert(this->path_.is_absolute());
  }

  [[nodiscard]] auto what() const noexcept -> const char * override {
    return "Too many entries in a single directory";
  }

  [[nodiscard]] auto path() const noexcept -> const std::filesystem::path & {
    return this->path_;
  }

  [[nodiscard]] auto count() const noexcept -> std::uint64_t {
    return this->count_;
  }

private:
  std::filesystem::path path_;
  std::uint64_t count_;
};

} // namespace sourcemeta::one

#endif
