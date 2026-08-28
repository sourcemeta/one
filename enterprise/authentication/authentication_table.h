#ifndef SOURCEMETA_ONE_ENTERPRISE_AUTHENTICATION_TABLE_H_
#define SOURCEMETA_ONE_ENTERPRISE_AUTHENTICATION_TABLE_H_

#include <sourcemeta/one/authentication.h>

#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/io.h>
#include <sourcemeta/core/jose.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/oauth.h>
#include <sourcemeta/core/oidc.h>

#include "authentication_claims.h"
#include "authentication_format.h"
#include "authentication_github.h"
#include "authentication_provider.h"
#include "authentication_session.h"

#include <cassert>       // assert
#include <chrono>        // std::chrono::sys_seconds, std::chrono::seconds
#include <cstddef>       // std::byte, std::size_t
#include <cstdint>       // std::uint32_t, std::uint64_t, std::uint8_t
#include <filesystem>    // std::filesystem::path
#include <map>           // std::map
#include <memory>        // std::unique_ptr, std::make_unique
#include <mutex>         // std::mutex, std::scoped_lock
#include <optional>      // std::optional, std::nullopt
#include <span>          // std::span
#include <string>        // std::string
#include <string_view>   // std::string_view
#include <unordered_set> // std::unordered_set
#include <utility>       // std::move
#include <vector>        // std::vector

// Reading a compiled table: which policies govern a location, which of them a
// credential satisfies, and what each one declares. Everything here is a
// question about the artifact, so the protocol that acts on the answers lives
// next door rather than in this file
namespace sourcemeta::one {

// A set of policy entries, one bit per entry. Entries are assigned
// monotonically increasing identifiers in configuration declaration order, so
// a governing set is a single machine word
using PolicySet = std::uint64_t;

struct Authentication::Table::Impl {
  // The indexer always emits this artifact. A missing, unreadable, or
  // malformed file means it was deleted, corrupted, or produced by an older
  // indexer. Rather than failing open and serving every path publicly, or
  // crashing the server into a restart loop, leave the policy denying
  // everything: the section pointers below stay null, so matching yields the
  // empty set and admits no one. Opening the file covers the missing and
  // unreadable cases without a separate, throwing existence check
  explicit Impl(const std::filesystem::path &path) {
    std::unique_ptr<sourcemeta::core::FileView> view;
    try {
      view = std::make_unique<sourcemeta::core::FileView>(path);
    } catch (const sourcemeta::core::FileViewError &) {
      return;
    }

    if (this->adopt({view->as<std::byte>(), view->size()})) {
      this->view_ = std::move(view);
    }
  }

  // A table compiled in this process is held rather than mapped, so the bytes
  // are copied once and never move again, which is what keeps every section
  // pointer below valid for as long as this exists
  explicit Impl(const std::span<const std::byte> bytes) {
    this->owned_.assign(bytes.begin(), bytes.end());
    if (!this->adopt(this->owned_)) {
      this->owned_.clear();
    }
  }

  // Whether the artifact could be read, and every section pointer set where it
  // could. Nothing is set on a refusal, so a caller that ignores the answer
  // still denies everything
  auto adopt(const std::span<const std::byte> bytes) -> bool {
    if (!structurally_valid(bytes)) {
      return false;
    }

    const auto *header{at_offset<AuthenticationHeader>(bytes)};

    // Claim rules are read and parsed once here rather than on each request,
    // since a request answers a rule by lookup while the bytes behind it never
    // change. A policy naming none is left null, which every rule vacuously
    // satisfies.
    //
    // Rules that are present but unreadable leave the whole artifact denying
    // everything, exactly as a malformed header does. Passing over them would
    // instead drop the restriction and admit every caller the policy would
    // otherwise have narrowed, which is the one outcome worse than refusing
    std::vector<sourcemeta::core::JSON> claims;
    claims.assign(header->policy_count, sourcemeta::core::JSON{nullptr});
    const auto *policies{
        at_offset<AuthenticationPolicyEntry>(bytes, header->policies_offset)};
    for (std::uint32_t index{0}; index < header->policy_count; index += 1) {
      const auto &entry{policies[index]};
      const auto type{static_cast<AuthenticationPolicyType>(entry.type)};
      if (entry.metadata_length == 0) {
        continue;
      }

      const std::span<const std::byte> metadata{
          at_offset<std::byte>(bytes, entry.metadata_offset),
          entry.metadata_length};

      // What a key policy declares is read only when a credential is compared
      // against it, and reading it then cannot report a failure. So it is read
      // once here instead, because a policy whose keys cannot be recovered
      // would otherwise stand for an audience of nobody, and a reference into
      // it would be permitted for want of anything to compare
      if (type == AuthenticationPolicyType::ApiKey) {
        if (!each_counted_string(metadata,
                                 [](const std::string_view) -> void {})) {
          return false;
        }

        continue;
      }
      // A GitHub policy asserts no claims, since its provider publishes none,
      // so what is read here is only that the policy can be read at all. One
      // that cannot leaves the whole artifact denying everything, exactly as a
      // malformed header does
      if (type == AuthenticationPolicyType::GitHub) {
        GitHubPolicyMetadata decoded;
        if (!decode_github_metadata(metadata, decoded)) {
          return false;
        }

        continue;
      }

      std::string_view serialized;
      if (type == AuthenticationPolicyType::JWT) {
        if (!read_jwt_claims(metadata, serialized)) {
          return false;
        }
      } else {
        OIDCPolicyMetadata decoded;
        if (!decode_oidc_metadata(metadata, decoded)) {
          return false;
        }

        serialized = decoded.claims;
      }

      if (serialized.empty()) {
        continue;
      }

      auto document{sourcemeta::core::try_parse_json(serialized)};
      if (!document.has_value() || !document.value().is_object()) {
        return false;
      }

      claims[index] = std::move(document).value();
    }

    this->nodes_ = at_offset<AuthenticationNode>(bytes, header->nodes_offset);
    // The edge section is empty when no policy declares a nested prefix, in
    // which case it sits at the end of the buffer and must not be addressed
    if (header->edge_count > 0) {
      this->edges_ = at_offset<AuthenticationEdge>(bytes, header->edges_offset);
    }

    // Every name is a range into the string section, so it is addressed
    // whenever anything was named rather than only when a prefix was nested
    if (header->strings_length > 0) {
      this->strings_ = at_offset<char>(bytes, header->strings_offset);
    }

    this->views_ =
        at_offset<AuthenticationViewEntry>(bytes, header->views_offset);
    this->view_count_ = header->view_count;

    if (header->policy_count > 0) {
      this->policies_ =
          at_offset<AuthenticationPolicyEntry>(bytes, header->policies_offset);
      this->policy_count_ = header->policy_count;
    }

    this->claims_ = std::move(claims);
    this->bytes_ = bytes;
    return true;
  }

  // The transaction a callback belongs to, if the request carries one. A
  // request can present several cookies under one name, since a parent domain
  // and the host itself can each set one and neither the header nor the order
  // says which is which, so every value is tried. Letting whoever controls a
  // neighbouring host decide which transaction this reads is what turns the
  // cookie from a defence against a forged callback into the way to mount one
  [[nodiscard]] auto transaction(const std::string_view policy_name,
                                 const std::string_view state,
                                 const std::string_view redirect_uri,
                                 const Credentials &credentials) const
      -> std::optional<sourcemeta::core::JSON> {
    if (state.empty()) {
      return std::nullopt;
    }

    std::vector<std::string_view> candidates;
    for (const auto field : credentials.cookies) {
      sourcemeta::core::http_cookie_values(field, TRANSACTION_COOKIE,
                                           candidates);
    }

    for (const auto sealed : candidates) {
      auto opened{this->open(policy_name, SealPurpose::Transaction, sealed)};
      if (!opened.has_value()) {
        continue;
      }

      auto document{sourcemeta::core::try_parse_json(opened.value())};
      if (!document.has_value() || !document.value().is_object()) {
        continue;
      }

      const auto *sealed_policy{document.value().try_at("policy")};
      const auto *sealed_state{document.value().try_at("state")};
      const auto *sealed_redirect{document.value().try_at("redirect_uri")};
      const auto *verifier{document.value().try_at("verifier")};
      // A nonce is not asked for here, since only a login that will receive an
      // identity token seals one. Whoever completes such a login requires it,
      // where this requires what every login carries
      if (sealed_policy == nullptr || !sealed_policy->is_string() ||
          sealed_policy->to_string() != policy_name ||
          sealed_state == nullptr || !sealed_state->is_string() ||
          sealed_state->to_string() != state || sealed_redirect == nullptr ||
          !sealed_redirect->is_string() ||
          sealed_redirect->to_string() != redirect_uri || verifier == nullptr ||
          !verifier->is_string() || verifier->to_string().empty()) {
        continue;
      }

      return document;
    }

    return std::nullopt;
  }

  // The session cookie for a login, carrying the identity token when one is
  // given. Building it is separated out so the caller can measure the result
  // and ask for a smaller one. The name and the attributes are fixed, so a
  // cookie that cannot be built at all is a session too large for one, which
  // is what a caller measuring the result is looking for anyway
  [[nodiscard]] auto session_cookie(const std::string_view policy_name,
                                    const std::string_view subject,
                                    const std::string_view id_token,
                                    const std::chrono::sys_seconds expiry,
                                    const bool secure,
                                    std::vector<std::string> &log) const
      -> std::optional<std::string> {
    auto payload{sourcemeta::core::JSON::make_object()};
    payload.assign_assume_new("policy",
                              sourcemeta::core::JSON{std::string{policy_name}});
    payload.assign_assume_new("subject",
                              sourcemeta::core::JSON{std::string{subject}});
    if (!id_token.empty()) {
      payload.assign_assume_new("id_token",
                                sourcemeta::core::JSON{std::string{id_token}});
    }

    std::ostringstream payload_text;
    sourcemeta::core::stringify(payload, payload_text);
    const auto sealed{this->seal(policy_name, SealPurpose::Session,
                                 payload_text.str(), expiry)};
    if (!sealed.has_value()) {
      log.emplace_back("No session secret is set for the policy");
      return std::nullopt;
    }

    return sourcemeta::core::http_serialize_cookie(
        {.name = SESSION_COOKIE,
         .value = sealed.value(),
         .path = COOKIE_PATH,
         .max_age = SESSION_LIFETIME,
         .http_only = true,
         .secure = secure,
         .same_site = sourcemeta::core::HTTPCookieSameSite::Lax});
  }

  // A rule compared against a claim carrying objects is compared on the `value`
  // sub-attribute alone, so a rule naming what a person sees rather than what
  // identifies them matches nothing. A denial cannot show that, and the token
  // it concerns is sealed inside a cookie where an operator cannot look, so it
  // is said here. Only a refusal reaches this, so a working policy stays quiet,
  // and each claim is named once however often somebody signs in
  auto report_object_shaped_claims(const std::string_view policy_name,
                                   const sourcemeta::core::JSON &claims,
                                   std::vector<std::string> &log) const
      -> void {
    static std::mutex mutex;
    static std::set<std::string, std::less<>> reported;
    for (const auto claim : this->object_shaped_claims(policy_name, claims)) {
      std::string subject{claim};
      subject += " of the policy ";
      subject += policy_name;
      const std::scoped_lock guard{mutex};
      if (!reported.insert(subject).second) {
        continue;
      }

      std::string message{
          "A rule names a claim the provider answers with objects, which are "
          "compared on their identifier rather than on any name shown to a "
          "person. The claim is "};
      message += subject;
      log.push_back(std::move(message));
    }
  }

  // Which policies govern a location, or nothing at all when the artifact
  // could not be read. A missing or structurally broken one leaves the section
  // pointers null and governs nothing it could answer for, which is why an
  // empty set and an unreadable table are told apart here rather than by
  // whoever asks
  [[nodiscard]] auto governing_mask(const std::string_view path) const
      -> std::optional<Authentication::PolicySet> {
    if (this->nodes_ == nullptr) {
      return std::nullopt;
    }

    return this->match(path);
  }

  // Whether a caller satisfying these policies is shown a location
  [[nodiscard]] auto permits(const std::string_view path,
                             const Authentication::PolicySet policies) const
      -> bool {
    const auto governing{this->governing_mask(path)};
    return governing.has_value() &&
           (governing.value() == 0 || (governing.value() & policies) != 0);
  }

  // The policies that govern a registry path, accumulated from every prefix
  // covering it. An unconfigured instance yields the empty set
  [[nodiscard]] auto match(const std::string_view registry_path) const noexcept
      -> PolicySet {
    if (this->nodes_ == nullptr) {
      return 0;
    }

    const auto *nodes{static_cast<const AuthenticationNode *>(this->nodes_)};
    const auto *edges{static_cast<const AuthenticationEdge *>(this->edges_)};

    PolicySet result{nodes[0].mask};
    std::uint32_t current{0};
    std::size_t cursor{0};
    auto segment{authentication_next_segment(registry_path, cursor)};
    while (!segment.empty()) {
      auto lookahead{cursor};
      const auto next{authentication_next_segment(registry_path, lookahead)};
      const auto &node{nodes[current]};

      const auto exact{find_child(node, edges, this->strings_, segment)};
      // An extension is content negotiation on the resource itself, so only the
      // terminal segment also matches the extensionless resource policy, with
      // union semantics admitting both. An intermediate dotted segment is a
      // distinct directory and must not inherit its stem's policies
      auto stem{NO_CHILD};
      if (next.empty()) {
        const auto extension{segment.rfind('.')};
        if (extension != std::string_view::npos && extension > 0) {
          stem = find_child(
              node, edges, this->strings_,
              std::string_view{segment.begin(), segment.begin() + extension});
        }
      }

      if (exact != NO_CHILD) {
        result |= nodes[exact].mask;
      }

      if (stem != NO_CHILD) {
        result |= nodes[stem].mask;
      }

      if (exact != NO_CHILD) {
        current = exact;
      } else if (stem != NO_CHILD) {
        current = stem;
      } else {
        break;
      }

      segment = next;
      cursor = lookahead;
    }

    return result;
  }

  // Whether one policy accepts what a request presented, which is the whole of
  // reading a credential and is asked both of a policy governing a path and of
  // every policy when placing a caller. Two answers to this could disagree,
  // so there is one
  [[nodiscard]] auto
  admits_policy(const std::uint32_t index, const std::string_view credential,
                const std::span<const std::string_view> cookies,
                const std::optional<sourcemeta::core::JWT> &token,
                const std::string_view required_audience) const -> bool {
    const auto &entry{
        static_cast<const AuthenticationPolicyEntry *>(this->policies_)[index]};
    std::span<const std::byte> metadata;
    if (entry.metadata_length > 0) {
      metadata = {at_offset<std::byte>(this->bytes_, entry.metadata_offset),
                  entry.metadata_length};
    }

    const auto type{static_cast<AuthenticationPolicyType>(entry.type)};
    if (type == AuthenticationPolicyType::JWT) {
      return token.has_value() &&
             this->admits_jwt(metadata, token.value(), required_audience,
                              this->claims_[index]);
    }

    // An interactive policy authenticates a person through the session its
    // browser login established, never a presented credential. A request that
    // presented one is asking to be read as that credential, so its session is
    // not consulted at all rather than quietly widening what it reaches
    if (is_interactive_type(type)) {
      return credential.empty() &&
             this->admits_session(type, metadata, cookies);
    }

    return admits_apikey(
        metadata, credential,
        static_cast<Authentication::Algorithm>(entry.algorithm));
  }

  // Which policies a caller satisfies, asked of every policy rather than of
  // those governing one path, since a view describes the whole registry
  [[nodiscard]] auto
  classify(const std::string_view credential,
           const std::span<const std::string_view> cookies) const
      -> Authentication::PolicySet {
    // Presenting nothing satisfies nothing, and this is the common request, so
    // it answers without reading a policy at all
    if (this->nodes_ == nullptr || (credential.empty() && cookies.empty())) {
      return 0;
    }

    const auto token{sourcemeta::core::JWT::from(credential)};
    const auto *policies{
        static_cast<const AuthenticationPolicyEntry *>(this->policies_)};
    Authentication::PolicySet result{0};
    for (std::uint32_t index{0}; index < this->policy_count_; index += 1) {
      if (!this->admits_policy(index, credential, cookies, token, {})) {
        continue;
      }

      // Only token policies combine, so anything else stands for the caller on
      // its own and the first one reached is the one read
      if (static_cast<AuthenticationPolicyType>(policies[index].type) !=
          AuthenticationPolicyType::JWT) {
        if (result == 0) {
          return Authentication::PolicySet{1} << index;
        }

        continue;
      }

      result |= Authentication::PolicySet{1} << index;
    }

    return result;
  }

  [[nodiscard]] auto view_count() const noexcept -> std::size_t {
    return this->view_count_;
  }

  [[nodiscard]] auto view_at(const std::size_t index) const
      -> Authentication::RecordedView {
    assert(index < this->view_count_);
    const auto &entry{
        static_cast<const AuthenticationViewEntry *>(this->views_)[index]};
    Authentication::RecordedView result;
    result.name_ = {this->strings_ + entry.name_offset, entry.name_length};
    result.policies_ = entry.policies;
    return result;
  }

  // The name a set of policies is served under, read from the recorded table
  // rather than spelled again here
  [[nodiscard]] auto view_name(const Authentication::PolicySet policies) const
      -> std::string_view {
    const auto *views{
        static_cast<const AuthenticationViewEntry *>(this->views_)};
    for (std::uint32_t index{0}; index < this->view_count_; index += 1) {
      if (views[index].policies == policies) {
        return {this->strings_ + views[index].name_offset,
                views[index].name_length};
      }
    }

    // Every set a caller can be placed in is named, since the table holds one
    // entry per policy and one per combination that can be satisfied at once.
    // An unconfigured instance has no table at all, and serves the one view
    // everything is served under
    return VIEW_PUBLIC;
  }

  [[nodiscard]] auto admits_jwt(const std::span<const std::byte> metadata,
                                const sourcemeta::core::JWT &token,
                                const std::string_view required_audience,
                                const sourcemeta::core::JSON &claims) const
      -> bool {
    JWTPolicy policy;
    if (!decode_jwt_metadata(metadata, policy)) {
      return false;
    }

    auto *provider{this->provider_for(policy.issuer, policy.jwks_uri)};
    if (provider == nullptr) {
      return false;
    }

    // RFC 9068 Section 4 has a resource server refuse a token whose `typ` is
    // not the access token profile's, which is what keeps an identity token
    // from being spent as an API credential. A provider that does not stamp
    // the header at all cannot be told apart that way, so the policy says
    // which type it requires rather than one being assumed
    const auto expected_type{
        policy.token_type.empty()
            ? std::optional<std::string_view>{std::nullopt}
            : std::optional<std::string_view>{policy.token_type}};
    const auto error{provider->verify(token, policy.algorithms, policy.issuer,
                                      policy.audience, std::nullopt,
                                      expected_type)};
    if (error.has_value()) {
      return false;
    }

    // The signature and the policy's own audience are already established, so
    // the route's requirement is one more claim read from a token that has
    // been verified rather than a second verification
    if (!required_audience.empty() && !token.has_audience(required_audience)) {
      return false;
    }

    return admits_claims(token.payload(), claims) == Admission::Admitted;
  }

  [[nodiscard]] auto
  admits_session(const AuthenticationPolicyType type,
                 const std::span<const std::byte> metadata,
                 const std::span<const std::string_view> cookies) const
      -> bool {
    if (cookies.empty()) {
      return false;
    }

    SessionPolicyMetadata decoded;
    if (!decode_session_metadata(type, metadata, decoded) ||
        decoded.name.empty()) {
      return false;
    }
    const auto policy_name{decoded.name};

    // A request can carry several cookies under one name, since a parent
    // domain and the host itself can each set one and neither the header nor
    // the order says which is which. Taking any single one lets whoever
    // controls a neighbouring host decide which session this instance reads,
    // so each is carried through the whole check and the caller is admitted
    // when any of them is a session for this policy
    std::vector<std::string_view> candidates;
    for (const auto field : cookies) {
      sourcemeta::core::http_cookie_values(field, SESSION_COOKIE, candidates);
    }
    // Every policy governing this location reads the one session the browser
    // holds, so what the payload names is what keeps a session established
    // under one policy from admitting its holder under another. A value that
    // is not for this policy is passed over rather than ending the search
    for (const auto sealed : candidates) {
      if (this->seal_names_policy(decoded.session_secrets, SealPurpose::Session,
                                  sealed, policy_name)) {
        return true;
      }
    }

    return false;
  }

  // The payload of a session value, whichever interactive policy minted it.
  // The policy travels inside the sealed value, so a caller learns which one
  // established the session rather than nominating one, and a value is only
  // ever accepted under the policy it names. Deciding that here rather than at
  // a call site keeps one answer to what a session is
  [[nodiscard]] auto open_session(const std::string_view value) const
      -> std::optional<std::string> {
    if (this->policy_count_ == 0) {
      return std::nullopt;
    }

    const auto *policies{
        static_cast<const AuthenticationPolicyEntry *>(this->policies_)};
    for (std::uint32_t index{0}; index < this->policy_count_; index += 1) {
      const auto &entry{policies[index]};
      const auto type{static_cast<AuthenticationPolicyType>(entry.type)};
      if (!is_interactive_type(type) || entry.metadata_length == 0) {
        continue;
      }

      const std::span<const std::byte> metadata{
          at_offset<std::byte>(this->bytes_, entry.metadata_offset),
          entry.metadata_length};
      SessionPolicyMetadata decoded;
      if (!decode_session_metadata(type, metadata, decoded) ||
          decoded.name.empty()) {
        continue;
      }

      const auto payload{this->session_open(decoded.session_secrets,
                                            SealPurpose::Session, value)};
      if (!payload.has_value()) {
        continue;
      }

      const auto document{sourcemeta::core::try_parse_json(payload.value())};
      if (!document.has_value() || !document.value().is_object()) {
        continue;
      }

      const auto *minted_for{document.value().try_at("policy")};
      if (minted_for == nullptr || !minted_for->is_string() ||
          minted_for->to_string() != decoded.name) {
        continue;
      }

      return payload;
    }

    return std::nullopt;
  }

  // The decoded metadata of the OIDC policy declared under the given name,
  // scanned out of the artifact
  [[nodiscard]] auto find_interactive(const std::string_view name,
                                      OIDCPolicyMetadata &result) const
      -> bool {
    if (this->policy_count_ == 0 || name.empty()) {
      return false;
    }

    const auto *policies{
        static_cast<const AuthenticationPolicyEntry *>(this->policies_)};
    for (std::uint32_t index{0}; index < this->policy_count_; index += 1) {
      const auto &entry{policies[index]};
      if (static_cast<AuthenticationPolicyType>(entry.type) !=
              AuthenticationPolicyType::OIDC ||
          entry.metadata_length == 0) {
        continue;
      }

      const std::span<const std::byte> metadata{
          at_offset<std::byte>(this->bytes_, entry.metadata_offset),
          entry.metadata_length};
      if (decode_oidc_metadata(metadata, result) && result.name == name) {
        return true;
      }
    }

    return false;
  }

  // What the policy declared under a name carries about the session it holds,
  // whichever kind establishes it. Sealing, opening and reading a client secret
  // ask this rather than each kind in turn
  [[nodiscard]] auto find_session(const std::string_view name,
                                  SessionPolicyMetadata &result) const -> bool {
    if (this->policy_count_ == 0 || name.empty()) {
      return false;
    }

    const auto *policies{
        static_cast<const AuthenticationPolicyEntry *>(this->policies_)};
    for (std::uint32_t index{0}; index < this->policy_count_; index += 1) {
      const auto &entry{policies[index]};
      const auto type{static_cast<AuthenticationPolicyType>(entry.type)};
      if (!is_interactive_type(type) || entry.metadata_length == 0) {
        continue;
      }

      const std::span<const std::byte> metadata{
          at_offset<std::byte>(this->bytes_, entry.metadata_offset),
          entry.metadata_length};
      if (decode_session_metadata(type, metadata, result) &&
          result.name == name) {
        return true;
      }
    }

    return false;
  }

  // The decoded metadata of the GitHub policy declared under the given name,
  // scanned out of the artifact
  [[nodiscard]] auto find_github(const std::string_view name,
                                 GitHubPolicyMetadata &result) const -> bool {
    if (this->policy_count_ == 0 || name.empty()) {
      return false;
    }

    const auto *policies{
        static_cast<const AuthenticationPolicyEntry *>(this->policies_)};
    for (std::uint32_t index{0}; index < this->policy_count_; index += 1) {
      const auto &entry{policies[index]};
      if (static_cast<AuthenticationPolicyType>(entry.type) !=
              AuthenticationPolicyType::GitHub ||
          entry.metadata_length == 0) {
        continue;
      }

      const std::span<const std::byte> metadata{
          at_offset<std::byte>(this->bytes_, entry.metadata_offset),
          entry.metadata_length};
      if (decode_github_metadata(metadata, result) && result.name == name) {
        return true;
      }
    }

    return false;
  }

  [[nodiscard]] auto github(const std::string_view name) const
      -> std::optional<GitHubPolicy> {
    GitHubPolicyMetadata decoded;
    if (!this->find_github(name, decoded)) {
      return std::nullopt;
    }

    return github_policy(decoded);
  }

  [[nodiscard]] auto interactive(const std::string_view name) const
      -> std::optional<InteractivePolicy> {
    OIDCPolicyMetadata decoded;
    if (!this->find_interactive(name, decoded)) {
      return std::nullopt;
    }

    return interactive_policy(decoded);
  }

  [[nodiscard]] auto
  object_shaped_claims(const std::string_view policy,
                       const sourcemeta::core::JSON &claims) const
      -> std::vector<std::string_view> {
    std::vector<std::string_view> result;
    const auto *policies{
        static_cast<const AuthenticationPolicyEntry *>(this->policies_)};
    for (std::uint32_t index{0}; index < this->policy_count_; index += 1) {
      const auto &entry{policies[index]};
      if (static_cast<AuthenticationPolicyType>(entry.type) !=
              AuthenticationPolicyType::OIDC ||
          entry.metadata_length == 0) {
        continue;
      }

      OIDCPolicyMetadata decoded;
      if (!decode_oidc_metadata(
              {at_offset<std::byte>(this->bytes_, entry.metadata_offset),
               entry.metadata_length},
              decoded) ||
          decoded.name != policy) {
        continue;
      }

      const auto &rules{this->claims_[index]};
      if (!rules.is_object()) {
        return result;
      }

      for (const auto &rule : rules.as_object()) {
        // A scope is read as one space-delimited string rather than compared
        // member by member, so one arriving as anything else is refused rather
        // than compared on an identifier, and saying otherwise would send an
        // operator looking for the wrong mistake
        if (rule.first == "scope") {
          continue;
        }

        const auto *value{claims.try_at(rule.first)};
        if (value != nullptr && carries_objects(*value)) {
          result.push_back(rule.first);
        }
      }

      return result;
    }

    return result;
  }

  [[nodiscard]] auto admits_identity(const std::string_view policy,
                                     const sourcemeta::core::JSON &claims) const
      -> Admission {
    const auto *policies{
        static_cast<const AuthenticationPolicyEntry *>(this->policies_)};
    for (std::uint32_t index{0}; index < this->policy_count_; index += 1) {
      const auto &entry{policies[index]};
      if (static_cast<AuthenticationPolicyType>(entry.type) !=
              AuthenticationPolicyType::OIDC ||
          entry.metadata_length == 0) {
        continue;
      }

      OIDCPolicyMetadata decoded;
      if (!decode_oidc_metadata(
              {at_offset<std::byte>(this->bytes_, entry.metadata_offset),
               entry.metadata_length},
              decoded) ||
          decoded.name != policy) {
        continue;
      }

      // A policy naming domains admits nobody whose address it cannot place,
      // so one it cannot read denies rather than being passed over
      std::uint32_t domains{0};
      std::size_t cursor{0};
      if (!read_u32(decoded.email_domains, cursor, domains)) {
        return Admission::Refused;
      }

      const auto address{
          domains > 0 ? admits_email_domain(claims, decoded.email_domains)
                      : Admission::Admitted};
      if (address == Admission::Refused) {
        return address;
      }

      const auto rules{admits_claims(claims, this->claims_[index])};
      if (rules == Admission::Refused) {
        return rules;
      }

      // Either half wanting more is the whole wanting more
      return rules == Admission::Incomplete ? rules : address;
    }

    // A name that no interactive policy answers to could never have minted a
    // session, so nothing it asserts is admitted
    return Admission::Refused;
  }

  // Which interactive policy governing this location sealed this marker. The
  // marker is opened rather than read, so naming a policy is not by itself a
  // way to learn whether that policy governs anywhere: only a browser this
  // instance signed in holds one that opens
  [[nodiscard]] auto renewal_marker(const std::string_view path,
                                    const std::string_view value) const
      -> std::optional<std::string_view> {
    const auto mask{this->match(path)};
    if (mask == 0 || this->policy_count_ == 0 || value.empty()) {
      return std::nullopt;
    }

    const auto *policies{
        static_cast<const AuthenticationPolicyEntry *>(this->policies_)};
    for (std::uint32_t index{0}; index < this->policy_count_; index += 1) {
      if ((mask & (PolicySet{1} << index)) == 0) {
        continue;
      }

      // Only a policy whose provider can be asked whether a sign-in still
      // stands without showing the person anything is named here, which a
      // GitHub deployment cannot be, so a browser signed in through one is
      // never sent back to it on its own
      const auto &entry{policies[index]};
      if (static_cast<AuthenticationPolicyType>(entry.type) !=
              AuthenticationPolicyType::OIDC ||
          entry.metadata_length == 0) {
        continue;
      }

      const std::span<const std::byte> metadata{
          at_offset<std::byte>(this->bytes_, entry.metadata_offset),
          entry.metadata_length};
      OIDCPolicyMetadata decoded;
      if (!decode_oidc_metadata(metadata, decoded) || decoded.name.empty()) {
        continue;
      }

      if (this->seal_names_policy(decoded.session_secrets, SealPurpose::Renewal,
                                  value, decoded.name)) {
        return decoded.name;
      }
    }

    return std::nullopt;
  }

  [[nodiscard]] auto interactive(const std::string_view path,
                                 const std::string_view name) const
      -> std::optional<InteractivePolicy> {
    const auto mask{this->match(path)};
    if (mask == 0 || this->policy_count_ == 0 || name.empty()) {
      return std::nullopt;
    }

    const auto *policies{
        static_cast<const AuthenticationPolicyEntry *>(this->policies_)};
    for (std::uint32_t index{0}; index < this->policy_count_; index += 1) {
      if ((mask & (PolicySet{1} << index)) == 0) {
        continue;
      }

      const auto &entry{policies[index]};
      if (static_cast<AuthenticationPolicyType>(entry.type) !=
              AuthenticationPolicyType::OIDC ||
          entry.metadata_length == 0) {
        continue;
      }

      const std::span<const std::byte> metadata{
          at_offset<std::byte>(this->bytes_, entry.metadata_offset),
          entry.metadata_length};
      OIDCPolicyMetadata decoded;
      if (decode_oidc_metadata(metadata, decoded) && decoded.name == name) {
        return interactive_policy(decoded);
      }
    }

    return std::nullopt;
  }

  [[nodiscard]] auto client_secret(const std::string_view policy) const
      -> std::optional<sourcemeta::core::SecureString> {
    SessionPolicyMetadata decoded;
    if (!this->find_session(policy, decoded)) {
      return std::nullopt;
    }

    return resolve_environment(decoded.client_secret_variable);
  }

  // The resolved values back the views handed to the sealing primitive, so
  // they are returned to the caller to keep alive alongside them. A run that
  // cannot be read to its end says nothing about which secrets a policy
  // accepts, so none of it is trusted rather than the part read before it
  static auto session_secrets(const std::span<const std::byte> variables)
      -> std::vector<sourcemeta::core::SecureString> {
    std::vector<sourcemeta::core::SecureString> result;
    if (!each_counted_string(variables, [&result](const auto variable) -> void {
          auto resolved{resolve_environment(variable)};
          if (resolved.has_value()) {
            result.push_back(std::move(resolved.value()));
          }
        })) {
      return {};
    }

    return result;
  }

  // A value is signed under the newest secret and accepted under any of them,
  // so a secret can be replaced by putting the new one first and dropping the
  // old once every value signed under it has expired
  [[nodiscard]] auto session_seal(const std::span<const std::byte> variables,
                                  const SealPurpose purpose,
                                  const std::string_view payload,
                                  const std::chrono::sys_seconds expiry) const
      -> std::optional<std::string> {
    const auto resolved{session_secrets(variables)};
    if (resolved.empty()) {
      return std::nullopt;
    }

    const auto issued{std::chrono::time_point_cast<std::chrono::seconds>(
        std::chrono::system_clock::now())};
    return seal_value(payload, purpose, resolved.front(), issued, expiry);
  }

  // Whether a sealed value opens under these secrets for this purpose and
  // names this policy. Two policies may share a secret, in which case a value
  // minted elsewhere opens cleanly here and nothing but the payload tells them
  // apart. It is the control rather than a belt on top of one, so every kind
  // of sealed value asks it the same way
  [[nodiscard]] auto seal_names_policy(const std::span<const std::byte> secrets,
                                       const SealPurpose purpose,
                                       const std::string_view value,
                                       const std::string_view name) const
      -> bool {
    const auto payload{this->session_open(secrets, purpose, value)};
    if (!payload.has_value()) {
      return false;
    }

    const auto document{sourcemeta::core::try_parse_json(payload.value())};
    if (!document.has_value() || !document.value().is_object()) {
      return false;
    }

    const auto *minted_for{document.value().try_at("policy")};
    return minted_for != nullptr && minted_for->is_string() &&
           minted_for->to_string() == name;
  }

  [[nodiscard]] auto session_open(const std::span<const std::byte> variables,
                                  const SealPurpose purpose,
                                  const std::string_view value) const
      -> std::optional<std::string> {
    const auto resolved{session_secrets(variables)};
    if (resolved.empty()) {
      return std::nullopt;
    }

    // The views are taken once the values have stopped moving
    std::vector<std::string_view> secrets;
    secrets.reserve(resolved.size());
    for (const auto &secret : resolved) {
      secrets.emplace_back(secret);
    }

    const auto now{std::chrono::time_point_cast<std::chrono::seconds>(
        std::chrono::system_clock::now())};
    return open_value(value, purpose, secrets, now);
  }

  [[nodiscard]] auto seal(const std::string_view policy,
                          const SealPurpose purpose,
                          const std::string_view payload,
                          const std::chrono::sys_seconds expiry) const
      -> std::optional<std::string> {
    SessionPolicyMetadata decoded;
    if (!this->find_session(policy, decoded)) {
      return std::nullopt;
    }

    return this->session_seal(decoded.session_secrets, purpose, payload,
                              expiry);
  }

  [[nodiscard]] auto open(const std::string_view policy,
                          const SealPurpose purpose,
                          const std::string_view value) const
      -> std::optional<std::string> {
    SessionPolicyMetadata decoded;
    if (!this->find_session(policy, decoded)) {
      return std::nullopt;
    }

    return this->session_open(decoded.session_secrets, purpose, value);
  }

  // The provider's description of itself is fetched once and refreshed on the
  // freshness its own response advertises, rather than on every login. The
  // caching resolver hands back a snapshot of what it last read, so the OpenID
  // Connect layer is applied again only when that snapshot changes rather than
  // once per request
  [[nodiscard]] auto endpoints(const std::string_view policy) const
      -> std::optional<ProviderEndpoints> {
    OIDCPolicyMetadata decoded;
    if (!this->find_interactive(policy, decoded) || !this->fetcher_) {
      return std::nullopt;
    }

    std::string issuer{decoded.issuer};
    sourcemeta::core::OAuthMetadataProvider *provider{nullptr};
    {
      const std::scoped_lock lock{this->metadata_mutex_};
      const auto existing{this->metadata_providers_.find(issuer)};
      if (existing == this->metadata_providers_.cend()) {
        // The two resolvers describe a retrieval with their own result type,
        // so the one transport this instance was given is adapted rather than
        // a second one being introduced
        auto transport{
            [fetcher = this->key_fetcher()](const std::string_view url)
                -> std::optional<
                    sourcemeta::core::OAuthMetadataProvider::FetchResult> {
              auto result{fetcher(url)};
              if (!result.has_value()) {
                return std::nullopt;
              }

              const auto max_age{result.value().max_age};
              return sourcemeta::core::OAuthMetadataProvider::FetchResult{
                  .body = std::move(result).value().body, .max_age = max_age};
            }};
        auto fresh{std::make_unique<sourcemeta::core::OAuthMetadataProvider>(
            issuer,
            sourcemeta::core::OAuthWellKnownKind::OpenIDConfigurationAppended,
            std::move(transport))};
        provider = fresh.get();
        this->metadata_providers_.emplace(issuer, std::move(fresh));
      } else {
        provider = existing->second.get();
      }
    }

    const auto server{provider->metadata()};
    if (!server) {
      return std::nullopt;
    }

    const std::scoped_lock lock{this->metadata_mutex_};
    auto &cached{this->endpoints_[issuer]};
    if (cached.source != server) {
      auto document{sourcemeta::core::OIDCProviderMetadata::from(
          sourcemeta::core::OAuthServerMetadata{*server})};
      if (!document.has_value()) {
        return std::nullopt;
      }

      ProviderEndpoints resolved;
      if (document.value().authorization_endpoint().has_value()) {
        resolved.authorization =
            document.value().authorization_endpoint().value();
      }

      if (document.value().token_endpoint().has_value()) {
        resolved.token = document.value().token_endpoint().value();
      }

      resolved.jwks_uri = document.value().jwks_uri();
      if (document.value().end_session_endpoint().has_value()) {
        resolved.end_session = document.value().end_session_endpoint().value();
      }

      if (document.value().userinfo_endpoint().has_value()) {
        resolved.userinfo = document.value().userinfo_endpoint().value();
      }

      // RFC 6749 Section 2.3.1 requires every server to accept the client
      // secret in an authorization header and discourages carrying it in the
      // request body, so the body is used only where the header is refused.
      // A provider that lists nothing is taken to accept the header, which is
      // what the specification assigns to saying nothing
      resolved.token_endpoint_basic_auth =
          document.value().supports_token_endpoint_auth_method(
              "client_secret_basic");

      // OpenID Connect Discovery 1.0 Section 3 defaults this to false when a
      // provider says nothing, so a login falls back to asking through the
      // scopes that carry a claim rather than sending a parameter that would
      // be ignored, leaving it with no claims at all
      resolved.claims_parameter_supported =
          document.value().supports_claims_parameter();

      const auto *advertised{
          document.value().data().try_at("claims_supported")};
      if (advertised != nullptr && advertised->is_array()) {
        for (const auto &claim : advertised->as_array()) {
          if (claim.is_string()) {
            resolved.claims_supported.push_back(claim.to_string());
          }
        }
      }

      cached.source = server;
      cached.resolved = std::move(resolved);
    }

    return cached.resolved;
  }

  [[nodiscard]] auto provider_for(const std::string_view issuer,
                                  const std::string_view jwks_uri) const
      -> sourcemeta::core::JWKSProvider * {
    if (!this->fetcher_) {
      return nullptr;
    }

    std::pair<std::string, std::string> key{issuer, jwks_uri};

    {
      const std::scoped_lock lock{this->jwks_mutex_};
      const auto existing{this->jwks_providers_.find(key)};
      if (existing != this->jwks_providers_.cend()) {
        return existing->second.get();
      }
    }

    std::string location;
    if (jwks_uri.empty()) {
      const auto url{sourcemeta::core::oidc_discovery_url(issuer)};
      if (!url.has_value()) {
        return nullptr;
      }

      const auto metadata{this->retrieve(url.value())};
      if (!metadata.has_value()) {
        return nullptr;
      }

      auto parsed{sourcemeta::core::try_parse_json(metadata.value().body)};
      if (!parsed.has_value()) {
        return nullptr;
      }

      const auto document{sourcemeta::core::OIDCProviderMetadata::from(
          std::move(parsed).value(), issuer)};
      if (!document.has_value()) {
        return nullptr;
      }

      location = document.value().jwks_uri();
    } else {
      location = jwks_uri;
    }

    sourcemeta::core::JWKSProvider::Options options;
    options.clock_skew = JWT_CLOCK_SKEW;
    auto provider{std::make_unique<sourcemeta::core::JWKSProvider>(
        std::move(location), this->key_fetcher(), options)};

    // A concurrent caller may have installed this key while the lock was
    // released, so its provider wins and ours is discarded
    const std::scoped_lock lock{this->jwks_mutex_};
    const auto existing{this->jwks_providers_.find(key)};
    if (existing != this->jwks_providers_.cend()) {
      return existing->second.get();
    }

    auto *raw{provider.get()};
    this->jwks_providers_.emplace(std::move(key), std::move(provider));
    return raw;
  }

  struct Audience {
    bool is_public;
    std::unordered_set<std::string_view> keys;
  };

  [[nodiscard]] auto audience(const std::string_view registry_path) const
      -> Audience {
    Audience result{.is_public = false, .keys = {}};
    // A missing or broken artifact is not public, keeping the reference check
    // conservative
    if (this->nodes_ == nullptr) {
      return result;
    }

    const auto governing{this->match(registry_path)};
    if (governing == 0) {
      // No policy covers this path, so it is public
      result.is_public = true;
      return result;
    }

    const auto *policies{
        static_cast<const AuthenticationPolicyEntry *>(this->policies_)};
    for (std::uint32_t index{0}; index < this->policy_count_; index += 1) {
      if ((governing & (PolicySet{1} << index)) == 0) {
        continue;
      }

      const auto &entry{policies[index]};
      if (entry.metadata_length > 0) {
        const std::span<const std::byte> metadata{
            at_offset<std::byte>(this->bytes_, entry.metadata_offset),
            entry.metadata_length};
        const auto type{static_cast<AuthenticationPolicyType>(entry.type)};
        if (type == AuthenticationPolicyType::JWT) {
          collect_jwt_identifiers(metadata, result.keys);
        } else if (type == AuthenticationPolicyType::OIDC) {
          collect_oidc_identifiers(metadata, result.keys);
        } else if (type == AuthenticationPolicyType::GitHub) {
          collect_github_identifiers(metadata, result.keys);
        } else {
          collect_keys(metadata, result.keys);
        }
      }
    }

    return result;
  }

  [[nodiscard]] auto
  reference_permitted(const std::string_view referrer_path,
                      const std::string_view referent_path) const -> bool {
    // A missing or broken artifact denies every reference, since an empty key
    // set would otherwise vacuously satisfy the subset check below
    if (this->nodes_ == nullptr) {
      return false;
    }

    const auto referent{this->audience(referent_path)};
    if (referent.is_public) {
      return true;
    }

    const auto referrer{this->audience(referrer_path)};
    if (referrer.is_public) {
      return false;
    }

    // A governed location whose policies name nothing admits nobody, so the
    // subset below would hold for want of anything to compare rather than
    // because the referent is as reachable as the referrer. That is the one
    // direction this must not fail in, so an empty set refuses here instead
    if (referrer.keys.empty()) {
      return false;
    }

    return std::ranges::all_of(referrer.keys,
                               [&referent](const auto key) -> bool {
                                 return referent.keys.contains(key);
                               });
  }

  // The trie section bases, resolved from the header once at construction so
  // that matching never re-reads it. They are typed as the internal serialized
  // structures and point into the memory-mapped buffer below, remaining valid
  // for the lifetime of the view. All are null when the instance is
  // unconfigured, and the edge and string bases are null when no policy
  // declares a nested prefix
  const void *nodes_{nullptr};
  const void *edges_{nullptr};
  const char *strings_{nullptr};

  // The policy table, resolved once at construction, locating each policy's
  // keys. Null when the instance is unconfigured or declares no policies
  const void *policies_{nullptr};
  std::uint32_t policy_count_{0};

  // The view table, computed where the policies were read and only looked up
  // here, so that what a build wrote and what this serves cannot disagree
  const void *views_{nullptr};
  std::uint32_t view_count_{0};

  // The parsed claim rules of each policy, in the same order as the table
  // above, null where a policy declares none
  std::vector<sourcemeta::core::JSON> claims_;

  // Whichever of the two holds the bytes every section above points into. A
  // mapped table keeps the mapping, a compiled one keeps its own copy, and the
  // span is how anything reads them without caring which
  std::unique_ptr<sourcemeta::core::FileView> view_;
  std::vector<std::byte> owned_;
  std::span<const std::byte> bytes_{};

  Authentication::Fetcher fetcher_;

  [[nodiscard]] auto fetcher() const -> const Authentication::Fetcher & {
    return this->fetcher_;
  }

  // A plain retrieval, which is what discovery and a key set both are
  [[nodiscard]] auto retrieve(const std::string_view url) const
      -> std::optional<Authentication::ProviderResponse> {
    if (!this->fetcher_) {
      return std::nullopt;
    }

    auto response{this->fetcher_({.url = url})};
    if (!response.has_value() || response.value().status < 200 ||
        response.value().status >= 300) {
      return std::nullopt;
    }

    return response;
  }

  // What a key set is retrieved with, which is the one fetcher asked for a
  // plain retrieval. Adapting here rather than holding two keeps every
  // outbound call this makes going through the same place
  [[nodiscard]] auto key_fetcher() const
      -> sourcemeta::core::JWKSProvider::Fetcher {
    return [fetcher = this->fetcher_](const std::string_view url)
               -> std::optional<sourcemeta::core::JWKSProvider::FetchResult> {
      auto response{fetcher({.url = url})};
      if (!response.has_value() || response.value().status < 200 ||
          response.value().status >= 300) {
        return std::nullopt;
      }

      const auto max_age{response.value().max_age};
      return sourcemeta::core::JWKSProvider::FetchResult{
          .body = std::move(response).value().body, .max_age = max_age};
    };
  }

  mutable std::mutex jwks_mutex_;
  mutable std::map<std::pair<std::string, std::string>,
                   std::unique_ptr<sourcemeta::core::JWKSProvider>>
      jwks_providers_;

  struct ResolvedEndpoints {
    std::shared_ptr<const sourcemeta::core::OAuthServerMetadata> source;
    ProviderEndpoints resolved;
  };

  mutable std::mutex metadata_mutex_;
  mutable std::map<std::string,
                   std::unique_ptr<sourcemeta::core::OAuthMetadataProvider>>
      metadata_providers_;
  mutable std::map<std::string, ResolvedEndpoints> endpoints_;
};

} // namespace sourcemeta::one

#endif
