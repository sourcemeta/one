#ifndef SOURCEMETA_ONE_AUTHENTICATION_H
#define SOURCEMETA_ONE_AUTHENTICATION_H

#ifndef SOURCEMETA_ONE_AUTHENTICATION_EXPORT
#include <sourcemeta/one/authentication_export.h>
#endif

#include <sourcemeta/one/authentication_error.h>

#include <sourcemeta/core/crypto.h>
#include <sourcemeta/core/jose.h>

#include <chrono>      // std::chrono::sys_seconds
#include <cstddef>     // std::size_t
#include <cstdint>     // std::uint8_t, std::uint64_t
#include <filesystem>  // std::filesystem::path
#include <functional>  // std::function
#include <memory>      // std::unique_ptr
#include <optional>    // std::optional
#include <span>        // std::span
#include <string>      // std::string
#include <string_view> // std::string_view
#include <utility>     // std::move
#include <variant>     // std::variant, std::get_if, std::holds_alternative
#include <vector>      // std::vector

namespace sourcemeta::one {

// The view a caller holding nothing is served. Named for the same reason a
// policy may not take that name, since an empty policy array on a listing
// already means public. Spelled once so that what a build writes and what a
// server reads cannot disagree
inline constexpr std::string_view VIEW_PUBLIC{"public"};

class SOURCEMETA_ONE_AUTHENTICATION_EXPORT Authentication {
private:
  // A set of policies, one bit per declaration index, which is why there can
  // only ever be as many policies as this has bits
  using PolicySet = std::uint64_t;

public:
  /// Where a resource lives within an instance, in the single spelling every
  /// part of the system agrees on: no leading or trailing separator, no empty
  /// or relative segments, and lowercase throughout. The instance root is the
  /// empty value.
  ///
  /// An extension is kept, because it names a representation of a resource
  /// rather than a different resource, and a policy may gate one
  /// representation on its own. The artifact tree stores a resource once, so
  /// locating an artifact drops the extension while the gate keeps it.
  ///
  /// Policies, artifact locations, and directory listings are all keyed by
  /// this, so the spelling has to be decided in exactly one place. That is
  /// what this type is for: a value can only come from `parse`, which means a
  /// caller cannot hand a path to one part of the system that another part
  /// would read differently.
  class SOURCEMETA_ONE_AUTHENTICATION_EXPORT Path {
  public:
    /// Canonicalise a request path or a URL against an instance, returning
    /// nothing when the input names somewhere outside it
    [[nodiscard]] static auto parse(const std::string_view input,
                                    const std::string_view instance_url)
        -> std::optional<Path>;

    /// Canonicalise input that is already relative to the instance root, such
    /// as a configured policy path or a path the indexer composed. It names
    /// nowhere outside the instance, so unlike `parse` it always yields a
    /// value
    [[nodiscard]] static auto relative(const std::string_view input) -> Path;

    [[nodiscard]] auto value() const noexcept -> std::string_view {
      return this->value_;
    }

    [[nodiscard]] auto empty() const noexcept -> bool {
      return this->value_.empty();
    }

    [[nodiscard]] auto operator==(const Path &other) const noexcept -> bool {
      return this->value_ == other.value_;
    }

  private:
    Path() = default;
    explicit Path(std::string value) : value_{std::move(value)} {}
    std::string value_;
  };

  // A route as the router matched it, taken by the spelling it arrived in
  // rather than by where it resolves to. A target reaching past a governed
  // prefix is still governed by it, which is why it is not canonicalised, and
  // why it is not a registry path
  class RouteTarget {
  public:
    explicit RouteTarget(const std::string_view value) noexcept
        : value_{value} {}

    [[nodiscard]] auto value() const noexcept -> std::string_view {
      return this->value_;
    }

  private:
    std::string_view value_;
  };

  // How a presented credential is compared against a policy's stored keys.
  // Identity stores the key verbatim, every other algorithm stores it hashed
  enum class Algorithm : std::uint8_t { Identity = 0, Sha256 = 1 };

  // A policy gates a set of path prefixes. A path covered by no policy is
  // public.
  //
  // What a policy needs depends entirely on what it authenticates, and the
  // three have almost nothing in common beyond where they apply and what they
  // are called. Keeping them apart is what stops a key policy carrying a key
  // set location, or a machine policy carrying a client secret: a configuration
  // that means nothing cannot be written down here at all
  struct Policy {
    // A credential presented verbatim and compared against what an operator
    // configured, which authenticates a program rather than a person
    struct ApiKey {
      // The environment variable names holding the keys this admits
      std::span<const std::string_view> keys{};
      Algorithm algorithm{Algorithm::Identity};
    };

    // A token obtained elsewhere and presented in an authorization header,
    // which this validates without ever helping to obtain one
    struct Token {
      std::string_view issuer{};
      std::string_view audience{};
      // Pinning this bypasses discovery entirely
      std::string_view jwks_uri{};
      std::span<const sourcemeta::core::JWSAlgorithm> algorithms{};
      // The `typ` header a presented token must carry, empty to accept any
      std::string_view token_type{};
      // The claims a credential must carry, serialised as the member map of an
      // OpenID Connect claims request parameter. It arrives canonical, so that
      // two policies admitting the same callers carry identical bytes. Empty
      // where a policy names no rule
      std::string_view claims{};
    };

    // A person who signs in through an identity provider, which is the only
    // kind that establishes a session
    struct Interactive {
      std::string_view issuer{};
      std::string_view client_id{};
      // The environment variable name holding the client secret
      std::string_view client_secret_variable{};
      std::string_view claims{};
      std::span<const std::string_view> email_domains{};
      // The environment variable names holding the secrets that sign this
      // policy's session and transaction cookies, newest first. A value is
      // signed under the first and accepted under any, so a secret can be
      // replaced without ending the sessions signed under the one before it
      std::span<const std::string_view> session_secrets{};
    };

    std::span<const std::string_view> paths{};
    // The policy name, which interactive policies carry so their session
    // cookies can be recognised at the gate, and which every policy carries so
    // that a view can be named after what it comprises
    std::string_view name{};
    std::variant<ApiKey, Token, Interactive> credential{ApiKey{}};
  };

  // What this asks a provider for. Every outbound call goes through one of
  // these, so whoever constructs an instance decides what reaches a network,
  // and a test answers without one
  struct ProviderRequest {
    std::string_view url{};
    // Empty for a retrieval. Otherwise the form body to post, which carries a
    // client secret and so never leaves wiping storage
    sourcemeta::core::SecureString body{};
    // Empty where the request authenticates by other means, or not at all
    sourcemeta::core::SecureString authorization{};
  };

  struct ProviderResponse {
    std::uint32_t status{0};
    std::string body{};
    // What the response said about how long it stays fresh, where it said
    // anything
    std::optional<std::chrono::seconds> max_age{};
  };

  // The one way this reaches a provider, for discovery, for a key set, for
  // redeeming an authorization code, and for asking who somebody is. Answering
  // nothing is how a caller says the provider could not be reached
  using Fetcher =
      std::function<std::optional<ProviderResponse>(ProviderRequest &&)>;

  // Everything a request presented for authentication: the bearer value from
  // the authorization header and every cookie field it carried, either of which
  // may admit the caller under a covering policy. The cookie fields are kept as
  // they arrived rather than joined, since a request may carry more than one
  // and what a cookie means is decided by whoever reads it
  struct Credentials {
    std::string_view bearer{};
    std::span<const std::string_view> cookies{};
  };

  // Who is asking, read from what a request presented exactly once. Everything
  // decided afterwards is a comparison against what this holds rather than a
  // second reading of a credential
  class SOURCEMETA_ONE_AUTHENTICATION_EXPORT Caller {
  public:
    // The view this caller is served, which is the directory their artifacts
    // were written under
    [[nodiscard]] auto view() const noexcept -> std::string_view {
      return this->view_;
    }

  private:
    friend class Authentication;
    std::string_view view_{};
    PolicySet policies_{0};
    // What was presented in the authorization header, kept so that a route
    // requiring an audience can read one more claim from a token already
    // verified. It points at the request, as the credentials it came from do
    std::string_view bearer_{};
  };

  // What an interactive operation came to. A refusal says nothing beyond
  // having failed, since both endpoints answer uniformly whatever went wrong,
  // so what happened reaches the log and never a response
  struct Outcome {
    enum class Result : std::uint8_t {
      // The browser is sent onwards, carrying whatever cookies came with it.
      // A silent attempt that came to nothing ends here too, put back where it
      // started rather than shown any of it
      Redirect,
      // Nothing here answers to that name, which is the same answer a typo
      // gets
      Missing,
      // A login could not be started. Every reason reads alike, since telling
      // them apart would say how this deployment is doing to whoever asked
      Unavailable,
      // What came back does not belong to a login this instance started
      Invalid,
      // The provider refused, which it named itself
      Declined,
      // A callback that belongs to a real login, which could not end in a
      // session. Every reason reads alike, for the reason above
      Incomplete,
      // Somebody the provider vouched for, whom this policy does not admit.
      // That is the end of the road rather than something to try again
      NotAdmitted
    };

    Result result{Result::Unavailable};
    // Where to send the browser afterwards
    std::string location{};
    // Serialised, ready to be written as they are. Whoever measures a value
    // against what a browser will keep is whoever knows the attributes, so they
    // are composed here rather than by the surface that writes them
    std::vector<std::string> cookies{};
    // What an operator would want to know, which never reaches a response
    std::vector<std::string> log{};
  };

  // Whether a path is one this instance could gate. A policy scoped to
  // anything else would gate nothing at all, so the answer comes from whoever
  // knows what this instance serves
  using PathGuard = std::function<bool(std::string_view)>;

  class Table;

  // One view as the artifact records it: the directory its artifacts live
  // under, and the policies a caller satisfies to be served from it.
  //
  // The set is not something a caller can state, only something a table
  // answers with, because what it shows is decided by which policies a build
  // recorded together rather than by whoever asks. A set assembled elsewhere
  // would name a view no build ever wrote and be taken for one that admits
  // whatever it happens to hold
  class RecordedView {
  public:
    [[nodiscard]] auto name() const noexcept -> std::string_view {
      return this->name_;
    }

  private:
    friend Table;
    friend Authentication;
    std::string_view name_{};
    PolicySet policies_{0};
  };

  /// The compiled policy table: what a build wrote and what every question
  /// about who governs what is answered from.
  ///
  /// A pure function of its bytes. It holds no client, reads no secret and
  /// reaches no network, which is why whoever only needs these answers takes
  /// one of these rather than the serving class that owns one.
  class SOURCEMETA_ONE_AUTHENTICATION_EXPORT Table {
  public:
    // Build the artifact the gate reads, refusing any policy scoped to a path
    // the guard does not recognise. The configuration names where a refusal
    // came from
    [[nodiscard]] static auto
    compile(const std::span<const Policy> policies,
            const std::filesystem::path &configuration,
            const PathGuard &gateable) -> std::vector<std::byte>;

    // Persist what was compiled, so that a later process can map it back
    static auto write(const std::span<const std::byte> bytes,
                      const std::filesystem::path &destination) -> void;

    /// Map a table a build wrote. A missing, unreadable or malformed artifact
    /// yields a table that governs nothing it could answer for, rather than one
    /// that serves every path publicly.
    explicit Table(const std::filesystem::path &path);

    /// Adopt a table compiled in this process rather than mapped from a file,
    /// so that reading a policy set never depends on a filesystem to do it
    explicit Table(const std::span<const std::byte> bytes);

    ~Table();

    // The bytes are owned for the lifetime of the table, either as a mapping or
    // as a copy, so one is handed on rather than duplicated
    Table(Table &&) noexcept;
    auto operator=(Table &&) noexcept -> Table &;
    Table(const Table &) = delete;
    auto operator=(const Table &) -> Table & = delete;

    /// Every view this table serves, which is every copy of the view tree a
    /// build of it produced, ordered as the artifact records them. A table that
    /// could not be read serves none.
    [[nodiscard]] auto views() const -> std::vector<RecordedView>;

    /// The view served under a name. A name this table does not serve answers
    /// the anonymous view, which is what a build stamped before a policy was
    /// withdrawn leaves behind, and which reaches only what nobody governs.
    [[nodiscard]] auto view(const std::string_view name) const -> RecordedView;

    /// Whether a view shows a path, which is the rule a build filters by and
    /// the only place it is stated. A path nobody governs is shown to
    /// everybody, and a governed one is shown to a view satisfying any policy
    /// governing it, exactly as the gate admits a caller under any policy
    /// covering the path they asked for.
    [[nodiscard]] auto visible(const Path &path, const RecordedView &view) const
        -> bool;

    /// The names of the policies that govern a path, in the order they were
    /// declared. A name is what a configuration and the artifact built from it
    /// agree on, so a caller holding the configuration can find what it
    /// declared without depending on the order it declared it in. The names
    /// point into the artifact and stay valid for the lifetime of this table.
    [[nodiscard]] auto governing(const Path &path) const
        -> std::vector<std::string_view>;

    /// Whether a schema at one path may reference a schema at another, which is
    /// whether the second is reachable by everybody the first is.
    [[nodiscard]] auto reference_permitted(const Path &referrer,
                                           const Path &referent) const -> bool;

  private:
    friend Authentication;

    // One way the registry looks to somebody: the name its artifacts live under
    // and the policies a caller of it satisfies. The anonymous view comprises
    // none, which is what makes it the base every other view adds to
    struct View {
      std::string name;
      std::vector<std::size_t> policies;

      [[nodiscard]] auto operator==(const View &other) const -> bool = default;
    };

    // How many policies may name one issuer. The combinations over a group
    // double with every policy added to it, so where to stop is a choice rather
    // than a discovery: this sits far above anything a configuration has reason
    // to declare and far below where enumerating them becomes a burden. Raising
    // it is a decision about what a build should attempt, bounded only in that
    // it can never reach the ceiling on policies, where the enumeration stops
    // being expressible at all. What a number of views costs is decided where
    // they are built rather than where they are named
    static constexpr std::size_t MAXIMUM_COMBINABLE_POLICIES{16};

    /// Every view over a registry declaring these policies, which is what
    /// segments its output. A pure function of what was declared: the anonymous
    /// view first, then the rest ordered by name.
    ///
    /// A credential carries one issuer and is checked against it before any
    /// rule, so only token policies declared against the same issuer can be
    /// satisfied together, and only those combine. Every other policy stands
    /// alone, since a caller presents one key or holds one session.
    ///
    /// The count is one, plus one per policy that stands alone, plus two to the
    /// power of each issuer group's size less one.
    ///
    /// Whether that many views is affordable is not decided here, since what
    /// they cost is known where they are built rather than where they are
    /// named. What is decided here is only that an enumeration doubling with
    /// every policy has to stop somewhere, which the ceiling below picks a
    /// point for.
    ///
    /// Every policy name must be distinct, which is what keeps one view from
    /// taking another's name.
    [[nodiscard]] static auto enumerate(const std::span<const Policy> policies)
        -> std::vector<View>;

    // The implementation differs by edition and owns the memory-mapped
    // artifact, so it is hidden behind a pointer to keep the binary format out
    // of the shared interface
    struct Impl;
    std::unique_ptr<Impl> impl_;
  };

  /// Serve a table, which is what turns answering who governs what into
  /// admitting somebody: the fetcher is how a provider is reached, and nothing
  /// but this class ever holds one.
  Authentication(Table &&table, Fetcher fetcher);

  ~Authentication();

  // The artifact is memory-mapped and owned for the lifetime of the view
  Authentication(const Authentication &) = delete;
  Authentication(Authentication &&) = delete;
  auto operator=(const Authentication &) -> Authentication & = delete;
  auto operator=(Authentication &&) -> Authentication & = delete;

  /// The table this serves, which answers everything derived from what a build
  /// wrote rather than from what a request presented.
  [[nodiscard]] auto table() const noexcept -> const Table &;

  /// Who is asking, read from what a request presented. This is the one place a
  /// credential is verified, so every question asked of the result afterwards
  /// is a comparison rather than a second verification.
  ///
  /// A caller presenting nothing satisfies nothing, which is the anonymous
  /// answer and costs nothing to reach.
  ///
  /// Token policies naming one issuer are satisfied together, since a token
  /// carrying several claims satisfies several of them at once and each is an
  /// area its holder reaches. Every other policy stands alone, and where a
  /// credential satisfies more than one, the first declared is the one read.
  [[nodiscard]] auto caller(const Credentials &credentials) const -> Caller;

  /// Whether a caller is shown a registry path, which is whether the view they
  /// were placed in holds it. A path nobody governs is shown to everybody, and
  /// a governed one to a view satisfying any policy governing it.
  ///
  /// The path arrives canonical, so every surface asks about the same location
  /// rather than about whichever spelling it happened to receive.
  [[nodiscard]] auto permits(const Path &path, const Caller &caller) const
      -> bool;

  /// The same question asked of an explicit route, which the router matched on
  /// the request target literally rather than on the location that target
  /// resolves to. The spelling is taken as matched, so a target reaching past a
  /// governed prefix is still governed by it.
  ///
  /// A route may additionally require that a presented token names the route
  /// itself, rather than only whatever wider audience the policy admitting the
  /// caller was configured with. An empty requirement asks for nothing extra,
  /// and a caller that presented no token is unaffected, since only a token
  /// carries an audience to check.
  [[nodiscard]] auto
  permits(const RouteTarget &target, const Caller &caller,
          const std::string_view required_audience = {}) const -> bool;

  /// End the browser's session, and the provider's where it named a policy
  /// whose provider offers to end one.
  ///
  /// Every cookie this instance mints expires on every path this can take,
  /// including the one where nothing opened at all. A cookie is withheld on
  /// plenty of navigations while the browser still holds it, so clearing only
  /// what arrived would leave a session behind while telling the person they
  /// are signed out.
  /// Where the browser lands afterwards is named by whoever asked, since a
  /// route is a fact about the surface that received the request rather than
  /// about authentication. Nothing here composes a URL of its own.
  [[nodiscard]] auto logout(const Credentials &credentials,
                            const std::string_view instance_url,
                            const std::string_view return_to) const -> Outcome;

  /// Start a login, sealing the transaction the callback will complete and
  /// naming where the provider should send the browser.
  ///
  /// A silent attempt asks the provider whether an existing sign-in still
  /// stands, and is answered either way without the person seeing anything.
  /// The transaction carries which kind it is, since a provider refusing to
  /// answer without interaction is an ordinary outcome for one and a failure
  /// for the other.
  ///
  /// The return target is where the browser goes once this completes. It
  /// arrives already checked against what this instance serves, since only the
  /// surface that received the request knows what it asked for. Where it is
  /// empty, what the policy governs stands in.
  /// The redirect URI is passed in rather than composed, for the same reason:
  /// it names a route, and which routes exist is not something this knows. It
  /// is what the provider is told to come back to, and what the callback will
  /// be checked against.
  [[nodiscard]] auto login(const std::string_view policy,
                           const std::string_view instance_url,
                           const std::string_view redirect_uri,
                           const bool silent,
                           const std::string_view return_to) const -> Outcome;

  /// Which interactive policy should be asked whether a lapsed sign-in still
  /// stands, for a caller that reached this path and nothing else.
  ///
  /// Two things have to hold, and both matter. The caller must carry the marker
  /// a previous sign-in left, or every stranger would be sent to a provider
  /// they have no account with. And the policy that marker names must govern
  /// the path they reached, or a stale marker would send somebody to a provider
  /// whose answer could not admit them here, leaving them where they started
  /// and going round again.
  ///
  /// The name is answered rather than a URL, since where a login begins is a
  /// route and this does not know any.
  [[nodiscard]] auto renewal(const Path &path,
                             const Credentials &credentials) const
      -> std::optional<std::string_view>;

  // What a provider sent back, read from wherever the surface found it
  struct CallbackRequest {
    std::string_view state{};
    std::string_view code{};
    // A decline names itself with an error code, so one that is present and
    // empty is a different thing from one that is absent
    bool has_error{false};
    std::string_view error{};
    // A provider naming no issuer cannot be checked against the one addressed,
    // so the check runs only when one arrived
    bool has_issuer{false};
    std::string_view issuer{};
  };

  /// Complete a login, minting the session when everything holds.
  ///
  /// The transaction the browser carries is the only proof that this belongs
  /// to a login this instance started, and it is opened before either a
  /// success or a decline is honoured, so a cross-site callback cannot trigger
  /// an error on somebody's behalf.
  ///
  /// The redirect URI must be the one the login was started with, since the
  /// provider checks it and so does this. It is passed in for the same reason
  /// it is passed to `login`: it names a route.
  [[nodiscard]] auto callback(const std::string_view policy,
                              const std::string_view instance_url,
                              const std::string_view redirect_uri,
                              const CallbackRequest &incoming,
                              const Credentials &credentials) const -> Outcome;

private:
  // The table this serves, held rather than borrowed so that nothing can
  // outlive the artifact every answer is read from
  Table table_;
};

} // namespace sourcemeta::one

#endif
