#include <sourcemeta/one/actions.h>

#include "action_default_v1.h"
#include "action_health_check_v1.h"
#include "action_jsonschema_evaluate_v1.h"
#include "action_mcp_v1.h"
#include "action_not_found_v1.h"
#include "action_schema_search_v1.h"
#include "action_serve_explorer_artifact_v1.h"
#include "action_serve_schema_artifact_v1.h"
#include "action_serve_static_v1.h"

#include <array>  // std::array
#include <memory> // std::unique_ptr, std::make_unique
#include <mutex>  // std::once_flag, std::call_once
#include <string> // std::string

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

sourcemeta::one::ActionDispatcher::ActionDispatcher(
    const std::filesystem::path &base,
    const sourcemeta::core::URITemplateRouterView &router)
    : base_{base}, router_{router},
      // NOLINTNEXTLINE(modernize-avoid-c-arrays)
      slots_{std::make_unique<Slot[]>(router.size() + 1)},
      slots_size_{router.size() + 1} {
  router.arguments(0, [this](const auto &key, const auto &value) {
    if (key == "errorSchema") {
      this->default_error_schema_ = std::get<std::string_view>(value);
    }
  });
}

auto sourcemeta::one::ActionDispatcher::error(
    const sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response, const char *const code,
    std::string &&identifier, std::string &&message) const -> void {
  sourcemeta::one::json_error(request, response, code, std::move(identifier),
                              std::move(message), this->default_error_schema_);
}

auto sourcemeta::one::ActionDispatcher::dispatch(
    const sourcemeta::core::URITemplateRouter::Identifier identifier,
    const sourcemeta::core::URITemplateRouter::Identifier context,
    const std::span<std::string_view> matches,
    sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response) -> void {
  if (identifier >= this->slots_size_ || context >= CONSTRUCTORS.size())
      [[unlikely]] {
    this->error(request, response, sourcemeta::one::STATUS_NOT_IMPLEMENTED,
                "unknown-handler-code",
                "This server version does not implement the handler for "
                "this URL");
    return;
  }

  auto &slot{this->slots_[identifier]};
  std::call_once(slot.flag, [this, &slot, context, identifier] {
    slot.instance =
        CONSTRUCTORS[context](this->base_, this->router_, identifier);
  });

  slot.instance->run(matches, request, response);
}
