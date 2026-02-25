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
  if (destination_mark.has_value() && destination_dependencies.has_value() &&
      std::ranges::none_of(
          destination_dependencies.value(),
          [&adapter, &destination_mark](const auto &dependency) {
            const auto dependency_mark = adapter.mark(dependency);
            return !dependency_mark.has_value() ||
                   adapter.is_newer_than(dependency_mark.value(),
                                         destination_mark.value());
          })) {
    return false;
  }

  BuildDependencies<typename Adapter::node_type> deps;
  for (const auto &dependency : dependencies) {
    assert(adapter.mark(dependency).has_value());
    deps.emplace_back(dependency);
  }

  handler(
      destination, dependencies,
      // This callback is used for build handlers to dynamically populate
      // dependencies. This is very powerful for defining handlers where
      // the actual list of dependencies is only known while or after
      // processing the handler
      [&](const auto &new_dependency) {
        assert(adapter.mark(new_dependency).has_value());
        deps.emplace_back(new_dependency);
      },
      context);

  adapter.write_dependencies(destination, deps);
  return true;
}

} // namespace sourcemeta::one

#endif // SOURCEMETA_ONE_BUILD_H_
