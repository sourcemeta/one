#ifndef SOURCEMETA_ONE_BUILD_H_
#define SOURCEMETA_ONE_BUILD_H_

#ifndef SOURCEMETA_ONE_BUILD_EXPORT
#include <sourcemeta/one/build_export.h>
#endif

#include <sourcemeta/core/json.h>
#include <sourcemeta/one/resolver.h>

#include <cstdint>     // std::uint8_t
#include <filesystem>  // std::filesystem::path, std::filesystem::file_time_type
#include <functional>  // std::function
#include <string>      // std::string
#include <string_view> // std::string_view
#include <unordered_map> // std::unordered_map
#include <vector>        // std::vector

namespace sourcemeta::one {

struct BuildEntry {
  std::filesystem::file_time_type file_mark;
  // TODO: Do we still need this distinction now?
  std::vector<std::filesystem::path> static_dependencies;
  std::vector<std::filesystem::path> dynamic_dependencies;
};

enum class BuildType : std::uint8_t { Headless, Full };

enum class BuildAction : std::uint8_t {
  Materialise,
  Positions,
  Locations,
  Dependencies,
  Stats,
  Health,
  Bundle,
  Editor,
  BlazeExhaustive,
  BlazeFast,
  SchemaMetadata,
  DependencyTree,
  Dependents,
  SearchIndex,
  DirectoryList,
  WebIndex,
  WebNotFound,
  WebDirectory,
  WebSchema,
  Comment,
  Configuration,
  Version,
  Routes,
  Remove
};

struct BuildActionEntry {
  BuildAction type;
  std::filesystem::path destination;
  std::vector<std::filesystem::path> dependencies;
  std::string_view data;
};

struct BuildPlan {
  std::filesystem::path output;
  BuildType type;
  std::vector<std::vector<BuildActionEntry>> waves;
  std::size_t size{0};
};

using BuildDependencies = std::vector<std::filesystem::path>;

using BuildDynamicCallback = std::function<void(const std::filesystem::path &)>;

// TODO: Maybe this should be its own class?
using BuildEntries = std::unordered_map<std::string, BuildEntry>;

SOURCEMETA_ONE_BUILD_EXPORT
auto delta(const BuildType build_type, const BuildEntries &entries,
           const std::filesystem::path &output,
           const std::unordered_map<std::string, Resolver::Entry> &schemas,
           const std::string_view version,
           const std::string_view current_version,
           const std::string_view comment,
           const sourcemeta::core::JSON &configuration,
           const sourcemeta::core::JSON &current_configuration,
           const std::vector<std::filesystem::path> &changed,
           const std::vector<std::filesystem::path> &removed) -> BuildPlan;

SOURCEMETA_ONE_BUILD_EXPORT
auto load_state(const std::filesystem::path &path, BuildEntries &entries)
    -> bool;

SOURCEMETA_ONE_BUILD_EXPORT
auto save_state(const std::filesystem::path &path, const BuildEntries &entries)
    -> void;

} // namespace sourcemeta::one

#endif // SOURCEMETA_ONE_BUILD_H_
