#ifndef SOURCEMETA_ONE_BUILD_TEST_UTILSH_
#define SOURCEMETA_ONE_BUILD_TEST_UTILSH_

#include <cassert>
#include <fstream>
#include <string>

inline auto READ_FILE(const std::filesystem::path &path) -> std::string {
  std::ifstream stream{path};
  std::ostringstream buffer;
  buffer << stream.rdbuf();
  return buffer.str();
}

inline auto WRITE_FILE(const std::filesystem::path &path,
                       const std::string &contents) -> void {
  std::filesystem::create_directories(path.parent_path());
  std::ofstream stream{path};
  assert(!stream.fail());
  stream << contents;
  stream.flush();
  stream.close();
}

#endif
