#ifndef SOURCEMETA_ONE_BUILD_H_
#define SOURCEMETA_ONE_BUILD_H_

// NOLINTBEGIN(misc-include-cleaner)
#include <sourcemeta/one/build_adapter_filesystem.h>
#include <sourcemeta/one/build_types.h>
// NOLINTEND(misc-include-cleaner)

#include <algorithm> // std::ranges::none_of
#include <cassert>   // assert

namespace sourcemeta::one {

template <typename Context, typename Adapter>
auto build(Adapter &adapter,
           const BuildHandler<Context, typename Adapter::node_type> &handler,
           const typename Adapter::node_type &destination,
           const BuildDependencies<typename Adapter::node_type> &dependencies,
           const Context &context) -> bool {
  const auto destination_mark{adapter.mark(destination)};
  const auto destination_dependencies{adapter.read_dependencies(destination)};
  if (destination_mark.has_value() && destination_dependencies.has_value()) {
    const auto &cached{destination_dependencies.value()};
    std::size_t static_index{0};
    bool static_dependencies_match{true};
    for (const auto &entry : cached) {
      if (entry.first == BuildDependencyKind::Static) {
        if (static_index >= dependencies.size() ||
            dependencies[static_index].second != entry.second) {
          static_dependencies_match = false;
          break;
        }

        static_index += 1;
      }
    }

    if (static_dependencies_match && static_index != dependencies.size()) {
      static_dependencies_match = false;
    }

    if (static_dependencies_match &&
        std::ranges::none_of(
            cached, [&adapter, &destination_mark](const auto &dependency) {
              const auto dependency_mark{adapter.mark(dependency.second)};
              return !dependency_mark.has_value() ||
                     adapter.is_newer_than(dependency_mark.value(),
                                           destination_mark.value());
            })) {
      return false;
    }
  }

  BuildDependencies<typename Adapter::node_type> output_dependencies;
  for (const auto &dependency : dependencies) {
    assert(adapter.mark(dependency.second).has_value());
    output_dependencies.emplace_back(BuildDependencyKind::Static,
                                     dependency.second);
  }

  handler(
      destination, dependencies,
      // This callback is used for build handlers to dynamically populate
      // dependencies. This is very powerful for defining handlers where
      // the actual list of dependencies is only known while or after
      // processing the handler
      [&](const auto &new_dependency) {
        assert(adapter.mark(new_dependency).has_value());
        output_dependencies.emplace_back(BuildDependencyKind::Dynamic,
                                         new_dependency);
      },
      context);

  adapter.write_dependencies(destination, output_dependencies);
  return true;
}

} // namespace sourcemeta::one

#endif // SOURCEMETA_ONE_BUILD_H_
