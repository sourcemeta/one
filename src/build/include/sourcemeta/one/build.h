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
  std::vector<std::filesystem::path> dependencies;
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
using BuildState = std::unordered_map<std::string, BuildEntry>;

SOURCEMETA_ONE_BUILD_EXPORT
auto delta(const BuildType build_type, const BuildState &entries,
           const std::filesystem::path &output, const Resolver::Views &schemas,
           const std::string_view version,
           const std::string_view current_version,
           const std::string_view comment,
           const sourcemeta::core::JSON &configuration,
           const sourcemeta::core::JSON &current_configuration,
           const std::vector<std::filesystem::path> &changed,
           const std::vector<std::filesystem::path> &removed) -> BuildPlan;

SOURCEMETA_ONE_BUILD_EXPORT
auto build_state_load(const std::filesystem::path &path, BuildState &entries)
    -> bool;

SOURCEMETA_ONE_BUILD_EXPORT
auto build_state_save(const std::filesystem::path &path,
                      const BuildState &entries) -> void;

} // namespace sourcemeta::one

#endif // SOURCEMETA_ONE_BUILD_H_
