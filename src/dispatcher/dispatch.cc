#include <sourcemeta/one/dispatcher.h>

#include <memory>  // std::make_unique
#include <mutex>   // std::call_once
#include <string>  // std::string
#include <utility> // std::move

namespace sourcemeta::one {

ActionDispatcher::ActionDispatcher(
    const std::filesystem::path &base,
    const sourcemeta::core::URITemplateRouterView &router,
    const std::span<const ActionConstructor> constructors,
    const std::span<const std::string_view> descriptions)
    : base_{base}, router_{router}, constructors_{constructors},
      descriptions_{descriptions},
      // NOLINTNEXTLINE(modernize-avoid-c-arrays)
      slots_{std::make_unique<Slot[]>(router.size() + 1)},
      slots_size_{router.size() + 1} {
  router.arguments(0, [this](const auto &key, const auto &value) {
    if (key == "errorSchema") {
      this->default_error_schema_ = std::get<std::string_view>(value);
    }
  });
}

auto ActionDispatcher::description(
    const sourcemeta::core::URITemplateRouter::Identifier context)
    const noexcept -> std::string_view {
  if (context >= this->descriptions_.size()) {
    return {};
  }
  return this->descriptions_[context];
}

auto ActionDispatcher::error(const sourcemeta::one::HTTPRequest &request,
                             sourcemeta::one::HTTPResponse &response,
                             const char *const code, std::string &&identifier,
                             std::string &&message) const -> void {
  sourcemeta::one::json_error(request, response, code, std::move(identifier),
                              std::move(message), this->default_error_schema_);
}

auto ActionDispatcher::action(
    const sourcemeta::core::URITemplateRouter::Identifier identifier,
    const sourcemeta::core::URITemplateRouter::Identifier context) -> Action * {
  if (identifier >= this->slots_size_ || context >= this->constructors_.size())
      [[unlikely]] {
    return nullptr;
  }

  auto &slot{this->slots_[identifier]};
  std::call_once(slot.flag, [this, &slot, context, identifier] {
    slot.instance =
        this->constructors_[context](this->base_, this->router_, identifier);
    slot.instance->attach_dispatcher(*this);
  });

  return slot.instance.get();
}

auto ActionDispatcher::action(
    const sourcemeta::core::URITemplateRouter::Identifier identifier)
    -> Action * {
  if (identifier >= this->slots_size_) [[unlikely]] {
    return nullptr;
  }
  return this->action(identifier, this->router_.context(identifier));
}

auto ActionDispatcher::dispatch(
    const sourcemeta::core::URITemplateRouter::Identifier identifier,
    const sourcemeta::core::URITemplateRouter::Identifier context,
    const std::span<std::string_view> matches,
    sourcemeta::one::HTTPRequest &request,
    sourcemeta::one::HTTPResponse &response) -> void {
  auto *instance{this->action(identifier, context)};
  if (instance == nullptr) [[unlikely]] {
    this->error(request, response, sourcemeta::one::STATUS_NOT_IMPLEMENTED,
                "unknown-handler-code",
                "This server version does not implement the handler for "
                "this URL");
    return;
  }

  instance->rest(matches, request, response);
}

} // namespace sourcemeta::one
