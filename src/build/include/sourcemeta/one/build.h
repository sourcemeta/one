#ifndef SOURCEMETA_ONE_BUILD_H_
#define SOURCEMETA_ONE_BUILD_H_

#ifndef SOURCEMETA_ONE_BUILD_EXPORT
#include <sourcemeta/one/build_export.h>
#endif

#include <sourcemeta/core/json.h>
#include <sourcemeta/one/resolver.h>

#include <sourcemeta/one/build_state.h>

#include <cstdint>     // std::uint8_t
#include <filesystem>  // std::filesystem::path
#include <functional>  // std::function
#include <string_view> // std::string_view
#include <vector>      // std::vector

namespace sourcemeta::one {

struct BuildPlan {
  enum class Type : std::uint8_t { Headless, Full };

  struct Action {
    enum class Type : std::uint8_t {
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

    using Dependencies = std::vector<std::filesystem::path>;

    Type type;
    std::filesystem::path destination;
    Dependencies dependencies;
    std::string_view data;
  };

  std::filesystem::path output;
  Type type;
  std::vector<std::vector<Action>> waves;
  std::size_t size{0};
};

using BuildDynamicCallback = std::function<void(const std::filesystem::path &)>;

SOURCEMETA_ONE_BUILD_EXPORT
auto delta(const BuildPlan::Type build_type, const BuildState &entries,
           const std::filesystem::path &output, const Resolver::Views &schemas,
           const std::string_view version,
           const std::string_view current_version,
           const std::string_view comment,
           const sourcemeta::core::JSON &configuration,
           const sourcemeta::core::JSON &current_configuration,
           const std::vector<std::filesystem::path> &changed,
           const std::vector<std::filesystem::path> &removed) -> BuildPlan;

} // namespace sourcemeta::one

#endif // SOURCEMETA_ONE_BUILD_H_
