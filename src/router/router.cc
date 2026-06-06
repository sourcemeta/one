#include <sourcemeta/one/router.h>

#include <memory>  // std::make_unique
#include <mutex>   // std::call_once
#include <string>  // std::string
#include <utility> // std::move

namespace sourcemeta::one {

Router::Router(const std::filesystem::path &base,
               const sourcemeta::core::URITemplateRouterView &router,
               const std::span<const RouterActionConstructor> constructors)
    : base_{base}, router_{router}, constructors_{constructors},
      // NOLINTNEXTLINE(modernize-avoid-c-arrays)
      slots_{std::make_unique<Slot[]>(router.size() + 1)},
      slots_size_{router.size() + 1} {
  router.arguments(0, [this](const auto &key, const auto &value) {
    if (key == "errorSchema") {
      this->default_error_schema_ = std::get<std::string_view>(value);
    }
  });
}

auto Router::error(const sourcemeta::one::HTTPRequest &request,
                   sourcemeta::one::HTTPResponse &response,
                   const sourcemeta::core::HTTPStatus &status,
                   const std::string_view type, const std::string_view detail,
                   const std::string_view origin) const -> void {
  sourcemeta::one::json_error(request, response, status, type, detail,
                              this->default_error_schema_, origin);
}

auto Router::action(
    const sourcemeta::core::URITemplateRouter::Identifier identifier,
    const sourcemeta::core::URITemplateRouter::Identifier context)
    -> RouterAction * {
  if (identifier >= this->slots_size_ || context >= this->constructors_.size())
      [[unlikely]] {
    return nullptr;
  }

  auto &slot{this->slots_[identifier]};
  std::call_once(slot.flag, [this, &slot, context, identifier] {
    slot.instance = this->constructors_[context](this->base_, this->router_,
                                                 identifier, *this);
  });

  return slot.instance.get();
}

auto Router::action(
    const sourcemeta::core::URITemplateRouter::Identifier identifier)
    -> RouterAction * {
  if (identifier >= this->slots_size_) [[unlikely]] {
    return nullptr;
  }
  return this->action(identifier, this->router_.context(identifier));
}

auto Router::dispatch(
    const sourcemeta::core::URITemplateRouter::Identifier identifier,
    const sourcemeta::core::URITemplateRouter::Identifier context,
    const std::span<std::string_view> matches,
    sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response) -> void {
  auto *instance{this->action(identifier, context)};
  if (instance == nullptr) [[unlikely]] {
    this->error(request, response,
                sourcemeta::core::HTTP_STATUS_NOT_IMPLEMENTED,
                "sourcemeta:one/unknown-action",
                "This version does not implement such action handler for "
                "this URL",
                "*");
    return;
  }

  instance->rest(matches, request, response);
}

} // namespace sourcemeta::one
