#include <sourcemeta/one/build_adapter_filesystem.h>

#include <sourcemeta/core/io.h>

#include <cassert>     // assert
#include <fstream>     // std::ofstream
#include <mutex>       // std::unique_lock
#include <string_view> // std::string_view

namespace sourcemeta::one {

constexpr std::string_view DEPENDENCIES_EXTENSION{".deps"};

BuildAdapterFilesystem::BuildAdapterFilesystem(
    const std::filesystem::path &output_root)
    : root{std::filesystem::canonical(output_root)} {}

auto BuildAdapterFilesystem::dependencies_path(const node_type &path) const
    -> node_type {
  assert(path.is_absolute());
  return path.string() + std::string{DEPENDENCIES_EXTENSION};
}

auto BuildAdapterFilesystem::read_dependencies(const node_type &path) const
    -> std::optional<BuildDependencies<node_type>> {
  assert(path.is_absolute());
  const auto dependencies_path{this->dependencies_path(path)};
  if (std::filesystem::exists(dependencies_path)) {
    auto stream{sourcemeta::core::read_file(dependencies_path)};
    assert(stream.is_open());

    BuildDependencies<node_type> result;
    std::string line;
    while (std::getline(stream, line)) {
      // Prevent CRLF on Windows
      if (!line.empty() && line.back() == '\r') {
        line.pop_back();
      }

      if (!line.empty()) {
        auto kind{BuildDependencyKind::Static};
        std::filesystem::path dependency;
        if (line.size() >= 2 && line[1] == ' ' &&
            (line[0] == 's' || line[0] == 'd')) {
          kind = (line[0] == 'd') ? BuildDependencyKind::Dynamic
                                  : BuildDependencyKind::Static;
          dependency = line.substr(2);
        } else {
          dependency = line;
        }
        if (!dependency.is_absolute()) {
          dependency =
              std::filesystem::weakly_canonical(this->root / dependency);
        }

        result.emplace_back(kind, std::move(dependency));
      }
    }

    if (result.empty()) {
      return std::nullopt;
    } else {
      return result;
    }
  } else {
    return std::nullopt;
  }
}

auto BuildAdapterFilesystem::write_dependencies(
    const node_type &path, const BuildDependencies<node_type> &dependencies)
    -> void {
  assert(path.is_absolute());
  assert(std::filesystem::exists(path));
  // Try to make sure as much as we can that any write operation made to disk
  sourcemeta::core::flush(path);
  this->refresh(path);
  const auto dependencies_path{this->dependencies_path(path)};
  std::filesystem::create_directories(dependencies_path.parent_path());
  std::ofstream dependencies_stream{dependencies_path};
  assert(!dependencies_stream.fail());
  for (const auto &dependency : dependencies) {
    const auto prefix{dependency.first == BuildDependencyKind::Dynamic ? "d "
                                                                       : "s "};
    const auto relative{dependency.second.lexically_relative(this->root)};
    if (!relative.empty() && *relative.begin() != "..") {
      dependencies_stream << prefix << relative.string() << "\n";
    } else {
      dependencies_stream << prefix << dependency.second.string() << "\n";
    }
  }

  dependencies_stream.flush();
  dependencies_stream.close();
  sourcemeta::core::flush(dependencies_path);
}

auto BuildAdapterFilesystem::refresh(const node_type &path) -> void {
  // We prefer our own computed marks so that we don't have to rely
  // too much on file-system specific non-sense
  const auto value{std::filesystem::file_time_type::clock::now()};
  // Because builds are typically ran in parallel
  std::unique_lock lock{this->mutex};
  this->marks.insert_or_assign(path, value);
}

auto BuildAdapterFilesystem::mark(const node_type &path)
    -> std::optional<mark_type> {
  assert(path.is_absolute());

  {
    // Because a read operation while a write operation is taking place is
    // undefined behaviour
    std::shared_lock lock{this->mutex};
    const auto match{this->marks.find(path)};
    if (match != this->marks.end()) {
      return match->second;
    }
  }

  try {
    // Keep in mind that depending on the OS, filesystem, and even standard
    // library implementation, this value might not be very reliable. In fact,
    // in many cases it can be outdated. Therefore, we never cache this value
    return std::filesystem::last_write_time(path);
  } catch (const std::filesystem::filesystem_error &) {
    return std::nullopt;
  }
}

auto BuildAdapterFilesystem::is_newer_than(const mark_type left,
                                           const mark_type right) const
    -> bool {
  return left > right;
}

} // namespace sourcemeta::one
