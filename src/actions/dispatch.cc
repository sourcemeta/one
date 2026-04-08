#include <sourcemeta/one/actions.h>

#include "action_default.h"
#include "action_health_check.h"
#include "action_jsonschema_evaluate.h"
#include "action_not_found.h"
#include "action_schema_search.h"
#include "action_serve_explorer_artifact.h"
#include "action_serve_schema_artifact.h"
#include "action_serve_static.h"

#include <array>   // std::array
#include <cstddef> // std::size_t
#include <memory>  // std::unique_ptr, std::make_unique
#include <mutex>   // std::once_flag, std::call_once
#include <string>  // std::string

template <typename T>
static auto
make_action(const std::filesystem::path &base,
            const sourcemeta::core::URITemplateRouterView &router,
            const sourcemeta::core::URITemplateRouter::Identifier identifier)
    -> std::unique_ptr<sourcemeta::one::Action> {
  return std::make_unique<T>(base, router, identifier);
}

using ActionConstructFunction =
    auto (*)(const std::filesystem::path &,
             const sourcemeta::core::URITemplateRouterView &,
             sourcemeta::core::URITemplateRouter::Identifier)
        -> std::unique_ptr<sourcemeta::one::Action>;

#define SOURCEMETA_ONE_MAKE_CONSTRUCTOR_ENTRY(Name, Class)                     \
  table[sourcemeta::one::ACTION_TYPE_##Name] = &make_action<Class>;

static constexpr std::array<ActionConstructFunction,
                            sourcemeta::one::ACTION_TYPE_COUNT>
    CONSTRUCTORS{[] {
      std::array<ActionConstructFunction, sourcemeta::one::ACTION_TYPE_COUNT>
          table{};
      SOURCEMETA_ONE_FOR_EACH_ACTION(SOURCEMETA_ONE_MAKE_CONSTRUCTOR_ENTRY)
      return table;
    }()};

#undef SOURCEMETA_ONE_MAKE_CONSTRUCTOR_ENTRY

struct Slot {
  std::unique_ptr<sourcemeta::one::Action> instance;
  std::once_flag flag;
};

// Heap array because Slot contains a non-movable std::once_flag
// NOLINTNEXTLINE(modernize-avoid-c-arrays)
static std::unique_ptr<Slot[]> SLOTS;
static std::size_t SLOTS_SIZE{0};

auto sourcemeta::one::actions_initialize(
    const sourcemeta::core::URITemplateRouterView &router) -> void {
  SLOTS_SIZE = router.size() + 1;
  // NOLINTNEXTLINE(modernize-avoid-c-arrays)
  SLOTS = std::make_unique<Slot[]>(SLOTS_SIZE);
}

auto sourcemeta::one::actions_dispatch(
    const sourcemeta::core::URITemplateRouter::Identifier identifier,
    const sourcemeta::core::URITemplateRouter::Identifier context,
    const sourcemeta::core::URITemplateRouterView &router,
    const std::filesystem::path &base,
    const std::span<std::string_view> matches,
    sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response) -> void {
  if (identifier >= SLOTS_SIZE || context >= CONSTRUCTORS.size()) [[unlikely]] {
    sourcemeta::one::json_error(
        request, response, sourcemeta::one::STATUS_NOT_IMPLEMENTED,
        "unknown-handler-code",
        "This server version does not implement the handler for "
        "this URL",
        // TODO: This implies the API is mounted
        std::string{router.base_path()} + "/self/v1/schemas/api/error");
    return;
  }

  auto &slot{SLOTS[identifier]};
  std::call_once(slot.flag, [&] {
    slot.instance = CONSTRUCTORS[context](base, router, identifier);
  });

  slot.instance->run(matches, request, response);
}
