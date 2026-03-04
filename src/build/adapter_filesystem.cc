#include <sourcemeta/one/build_adapter_filesystem.h>

#include <sourcemeta/core/io.h>

#include <cassert>     // assert
#include <fstream>     // std::ofstream, std::ifstream
#include <mutex>       // std::unique_lock
#include <string>      // std::string
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

  std::ifstream stream{dependencies_path};
  if (!stream.is_open()) {
    return std::nullopt;
  }

  std::string contents{std::istreambuf_iterator<char>(stream),
                       std::istreambuf_iterator<char>()};

  BuildDependencies<node_type> result;
  std::size_t position{0};
  while (position < contents.size()) {
    auto newline{contents.find('\n', position)};
    if (newline == std::string::npos) {
      newline = contents.size();
    }

    auto end{newline};
    // Prevent CRLF on Windows
    if (end > position && contents[end - 1] == '\r') {
      end -= 1;
    }

    if (end > position) {
      auto kind{BuildDependencyKind::Static};
      std::filesystem::path dependency;
      const auto length{end - position};
      if (length >= 2 && contents[position + 1] == ' ' &&
          (contents[position] == 's' || contents[position] == 'd')) {
        kind = (contents[position] == 'd') ? BuildDependencyKind::Dynamic
                                           : BuildDependencyKind::Static;
        dependency = contents.substr(position + 2, end - position - 2);
      } else {
        dependency = contents.substr(position, length);
      }
      if (!dependency.is_absolute()) {
        dependency = (this->root / dependency).lexically_normal();
      }

      result.emplace_back(kind, std::move(dependency));
    }

    position = newline + 1;
  }

  if (result.empty()) {
    return std::nullopt;
  } else {
    return result;
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
    const auto value{std::filesystem::last_write_time(path)};
    // Within a single run, if we didn't build this file, its mtime won't
    // change. If we did build it, refreshing already set a synthetic timestamp
    // that the cache lookup above would have returned instead
    std::unique_lock lock{this->mutex};
    this->marks.emplace(path, value);
    return value;
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
