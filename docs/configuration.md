---
hide:
  - navigation
---

# Configuration

Sourcemeta One is designed around a GitOps workflow: all of its behavior is
determined by the configuration file documented here, and runtime changes are
not permitted. *This ensures that your instances are fully reproducible,
auditable, and version-controlled, just like any other part of your
infrastructure.*

!!! success

    Because Sourcemeta One is entirely configured at build time (with changes
    applied only via a redeployment), it achieves significant performance
    advantages. Schemas are pre-optimized at build time, and the service itself
    is fully stateless, enabling effortless horizontal scaling and predictable
    performance under load.

This configuration file is designed to give you complete freedom to structure
your instance in a way that best suits your organization. Compared to many
other solutions, it imposes no artificial constraints on hierarchy, versioning,
or schema organization. You can version and arrange your schemas however you
like: by department, by function, in a flat structure, or in any other way you
can think of. This allows your instance to reflect your company's needs rather
than a pre-defined model.

!!! note

    By convention, the name of the configuration file is `one.json`.

The JSON Schema that defines `one.json` is always available at `/self`. You can
explore the latest version at
[https://schemas.sourcemeta.com/self](https://schemas.sourcemeta.com/self).

!!! tip

    A great way to learn what's possible is to explore the configuration file
    of the [schemas.sourcemeta.com](https://schemas.sourcemeta.com) public
    example instance, which you can find [on
    GitHub](https://github.com/sourcemeta/one/blob/main/enterprise/e2e/public/one.json)

## `one.json`

The configuration file controls your entire instance through various top-level
properties that define both global settings and content structure.  For
representing the contents of the instance, this file uses a hierarchical tree
approach where you organise the contents of your instance using nested nodes.
Each node in this tree serves as either a [Collection](#collections)
(containing actual schemas) or a [Page](#pages) (acting as a directory that
groups other pages and schema collections), giving you complete flexibility in
structuring your instance.

!!! note

    The `/self` namespace is reserved for the built-in [HTTP API](api.md)
    and internal functionality. It is always present and cannot be
    overridden by user content.

| Property        | Type | Required | Default | Description |
|-----------------|------|----------|---------|-------------|
| `/url`          | String  | :red_circle: **Yes** | N/A | The absolute URL on which the instance will be served, which must name an origin and nothing more: a scheme, a host, and optionally a port, with no path and no trailing slash. Sourcemeta One will automatically add URI identifiers relative to this URL for every ingested schema. Inside the instance's URL namespace (schema URIs the catalog owns), path lookups are case-insensitive, even though [RFC 3986 §6.2.2.1](https://datatracker.ietf.org/doc/html/rfc3986#section-6.2.2.1) makes the path component of a URI case-sensitive in general |
| `/extends`      | Array   | No  | None | One or more configuration files to extend from. See the [Extends](#extends) section for more information |
| `/contents`     | Object  | No  | None | The top-level [Collections](#collections) and [Pages](#pages) that compose the instance |
| `/html`        | Object or Boolean  | No  | `{}` | Settings for the HTML explorer. If set to `false`, the instance runs in headless mode. Enabling the HTML explorer implies the API must also be enabled. See the [HTML](#html) section for more details |
| `/api`         | Object or Boolean  | No  | `{}` | Controls whether the HTTP API is accessible. If set to `false`, the JSON API is disabled. Can only be set to `false` when `/html` is also set to `false` |
| `/authentication` (**Enterprise**) | Array  | No  | None | A list of authentication policies that govern this instance. Anything not covered by a policy remains public. See the [Authentication](#authentication) section for more details |

!!! note "Why the instance URL cannot have a path"

    An instance owns a whole origin, so `https://schemas.example.com` is
    accepted while `https://example.com/schemas` and even
    `https://example.com/` are refused at indexing time.

    This is not a limitation we chose so much as one the web's discovery
    mechanisms impose. Standards publish what a client needs to learn about
    a server under `/.well-known/`, and
    [RFC 8615 §3](https://www.rfc-editor.org/rfc/rfc8615.html#section-3)
    states that well-known URIs "are rooted in the top of the path's
    hierarchy", giving `/foo/.well-known/example` as an example of a path
    that is _not_ a well-known URI.
    [RFC 9728 §3.1](https://www.rfc-editor.org/rfc/rfc9728.html#section-3.1),
    which OAuth clients follow to discover how to authenticate against this
    instance, makes the same choice explicitly: even when the resource
    identifier carries a path, the metadata is inserted _between the host and
    the path_, and so is served from the top of the origin. An instance
    served under a path would have to publish these documents at an origin
    root it does not own, and a client is under no obligation to look
    anywhere else.

    If your schema URIs need a directory prefix, do not put it in the
    instance URL. Declare a [Collection](#collections) or a
    [Page](#pages) in `one.json` and put the schemas inside it. A collection
    named `my-first-collection` gives every schema below it a
    `https://schemas.example.com/my-first-collection/` prefix while the
    instance stays at its origin, which is exactly what the example below
    does.

For example, a minimal configuration that mounts a single schema collection
(`./schemas`) at URL `https://schemas.example.com/my-first-collection` may look
like this, and a schema at `./schemas/foo.json` will be available at
`https://schemas.example.com/my-first-collection/foo.json`:

```json title="one.json"
{
  "url": "https://schemas.example.com",
  "contents": {
    "my-first-collection": {
      "path": "./schemas"
    }
  }
}
```

### HTML

When enabled through the optional `html` top-level property, Sourcemeta One
generates an HTML explorer interface. Unlike the [JSON API](api.md), this
explorer provides a user-friendly web interface for browsing and examining your
schemas.  You can customize the explorer's appearance and behavior using the
configuration options detailed below.

| Property        | Type | Required | Default | Description |
|-----------------|------|----------|---------|-------------|
| `/name`        | String  | No  | *Sourcemeta* | The concise name of the instance. For example, the name of your organisation. This will be shown in the navigation bar in the HTML explorer |
| `/description`  | String  | No  | *The next-generation JSON Schema platform* | A longer description of the instance. This will be shown in HTML meta tags |
| `/head`         | String  | No  | None | An HTML snippet to include in the `<head>` section of the HTML explorer. Useful for website analytics purposes or for custom styles |
| `/hero`         | String  | No  | None | An HTML snippet to render in the front page. Try to make this snippet as standalone as possible using `style` HTML attributes |
| `/action`       | Object  | No  | None | A call-to-action button to render in the navigation bar of the HTML explorer |
| `/action/title` | String  | Yes | N/A | The text of the call-to-action button |
| `/action/icon`  | String  | Yes | N/A | The icon name of the call-to-action button, which must match the name of an icon in the [Bootstrap Icons](https://icons.getbootstrap.com) collection |
| `/action/url`   | String  | Yes | N/A | The absolute URL of the call-to-action button |

## Collections

A schema collection functions as a curated set of schemas that the instance
ingests and serves at a specified location. Unlike pages, schema collections
contain the actual schema definitions that power your instance.

*Sourcemeta One supports JSON Schema Draft 3, Draft 4, Draft 6, Draft 7,
2019-09, and 2020-12, and custom meta-schemas based on those dialects.*

!!! warning

    Sourcemeta One maintains data integrity by rejecting any schemas that fail
    against their meta-schemas or that cannot be fully resolved during the
    ingestion process. For this reason, you may need to explicitly inform the
    instance about default dialects, base URIs, or custom overrides for schema
    reference resolution.

    If you are facing any difficulties with this, don't hesitate in asking for
    help using [GitHub
    Discussions](https://github.com/sourcemeta/one/discussions). We are here to
    help!

!!! note

    To consolidate differences across operating systems, Sourcemeta One assumes
    the file system is case-insensitive and will not distinguish between two
    schema URIs that only differ in casing.  Furthermore, URI paths will be
    turned into lowercase.

| Property        | Type | Required | Default | Description |
|-----------------|------|----------|---------|-------------|
| `/path`         | String  | :red_circle: **Yes** (unless `include` is set) | N/A | The path (relative to the location of the configuration file) to the directory which includes the schemas for this collection. The directory will be recursively traversed in search of `.json`, `.yaml`, or `.yml` schemas |
| `/baseUri`         | String  | No  | *The top-level `url`* | The base URI of every schema file that is part of this collection, for rebasing purposes. If a schema defines an explicit identifier that is not relative to this base URI, the generation of the instance will fail |
| `/defaultDialect` | String  | No  | None | The default JSON Schema dialect URI Reference to use for schemas that do not declare the `$schema` keyword. Accepts a URI reference, which is rebased against the collection's `baseUri` (or the top-level `url` joined with the collection path) unless it names an [official JSON Schema dialect](https://json-schema.org/specification-links) |
| `/title`        | String  | No  | None | The concise title of the schema collection |
| `/description`  | String  | No  | None | A longer description of the schema collection. The web explorer renders this as Markdown |
| `/email`        | String  | No  | None | The e-mail address associated with the schema collection |
| `/github`       | String  | No  | None | The GitHub organisation or `organisation/repository` identifier associated with the schema collection |
| `/website`      | String  | No  | None | The absolute URL to the website associated with the schema collection |
| `/include`     | String  | No  | None | A `jsonschema.json` manifest definition to include in-place. See the [Include](#include) section for more information. **If this property is set, none of the other properties can be set (including `path`)** |
| `/resolve`      | Object  | No  | None | A URI-to-URI map to hook into the schema reference resolution process. See the [Resolve](#resolve) section for more information |
| `/lint`      | Object  | No  | None | Linting configuration for this schema collection. See the [JSON Schema CLI configuration](https://github.com/sourcemeta/jsonschema/blob/main/docs/configuration.markdown) for more information |
| `/lint/rules` (**Enterprise**) | Array  | No  | None | An array of file paths (relative to the configuration file location) to custom linting rule definitions. See the [Linter](#linter) section for more information |
| `/ignore`      | Array  | No  | None | An array of file paths (relative to the configuration file location) to exclude from the schema collection. See the [JSON Schema CLI configuration](https://github.com/sourcemeta/jsonschema/blob/main/docs/configuration.markdown) for more information |
| `/x-sourcemeta-one:evaluate`      | Boolean  | No  | `true` | When set to `false`, disable the evaluation API for this schema collection. This is useful if you will never make use of the [evaluation API](api.md) and want to speed up the generation of the instance |
| `/x-sourcemeta-one:alert`      | String  | No  | N/A | When set, provide a human-readable alert on both the API and the HTML explorer for every schema in the collection. This is useful to provide any important message to consumers. The web explorer renders this as Markdown |
| `/x-sourcemeta-one:priority`      | Integer  | No  | `50` | A hint, from `0` (least important) to `100` (most important), that signals the relative importance of this collection compared to others in the same instance. Consumers may use this to rank or filter collections |

!!! warning

    To preserve scalability and encourage best practices in schema
    organisation, Sourcemeta One caps the number of immediate entries (schemas
    or subdirectories) within a single registry directory to 1,000 by default.
    Instead of placing a large number of schemas in a single flat directory,
    organise them into a tree of nested subdirectories. A large number of
    schemas spread across multiple levels of directories scales well.

### Include

The `include` property enables modular schema collection management by
allowing you to extract collection definitions into separate `jsonschema.json`
files and reference them in-place. Unlike inline definitions, this approach
promotes reusability across multiple configuration files while maintaining
clean separation of concerns. Each included `jsonschema.json` file contains the
same properties as a standard schema collection definition, with Sourcemeta One
seamlessly integrating the external file's contents at the specified location
during processing. For example:

```json hl_lines="5" title="one.json"
{
  "url": "https://schemas.example.com",
  "contents": {
    "my-first-collection": {
      "include": "./jsonschema.json"
    }
  }
}
```

```json title="jsonschema.json"
{
  "title": "My Schema Collection",
  "path": "./schemas"
}
```

If a directory path is provided to the `include` property, the instance will
look for a file called `jsonschema.json` inside such directory.

If the included manifest does not declare the `path` property (nor `contents`
or a nested `include`), the collection defaults to the directory containing
the manifest itself, mirroring how the [JSON Schema
CLI](https://github.com/sourcemeta/jsonschema/blob/main/docs/configuration.markdown)
interprets such configuration files.

### Resolve

The `resolve` property is an advanced feature to hook into the schema reference
resolution process. When set, the object translates any reference that equals a
property name in the object to the corresponding property value.

This is useful when mounting two schema collections where one references the
other through an absolute URL. For example, IPTC's [News in
JSON](https://www.iptc.org/std/ninjs/) schemas contain `$ref` references to
`https://geojson.org/schema/GeoJSON.json`. If you also host a vendored copy of
the [GeoJSON](https://geojson.org) schemas, you can use `resolve` to route
those external references back into your instance instead of depending on a
resource outside your control:

```json hl_lines="8" title="one.json"
{
  "url": "https://schemas.example.com",
  "contents": {
    "geojson": {
      "baseUri": "https://geojson.org/schema",
      "path": "./vendor/geojson"
    },
    "ninjs": {
      "baseUri": "http://www.iptc.org/std/ninjs",
      "path": "./vendor/ninjs",
      "resolve": {
        "https://geojson.org/schema/GeoJSON.json": "/geojson/GeoJSON.json"
      }
    }
  }
}
```

### Linter

!!! success "Enterprise"

    Custom linter rules are only available in the
    [Enterprise](commercial.md) edition. Learn more about [commercial
    licensing](commercial.md).

Sourcemeta One ships with a growing comprehensive set of built-in universal
linting rules. The `lint/rules` property lets you extend that default set with
rules specific to your organisation: naming conventions, required annotations,
structural patterns, or any other constraint that matters to your governance
standards. Violations are surfaced alongside the built-in checks in the
registry's health analysis.

A custom rule is a JSON Schema file written in any supported dialect. During
indexing, each rule is evaluated against every subschema in every schema of the
collection. The rule must declare a `title` (used as the unique rule
identifier) and should include a `description` (shown to developers when a
violation is reported).

!!! note

    Rules apply to every subschema individually, not to the top-level schema
    document as a whole. For example, a rule that requires every subschema to
    define `title` will be checked against every nested subschema too, not only
    the root.

For example, say your organisation requires all schema property names to follow
camelCase. Create a rule file like this:

```json title="rules/camelcase.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "title": "custom/all_properties_camelcase",
  "description": "Ensure camelCase properties",
  "properties": {
    "properties": {
      "propertyNames": {
        "pattern": "^[a-z][a-zA-Z0-9]*$"
      }
    }
  }
}
```

This rule targets subschemas that define `properties` and asserts that all
property names within must match the camelCase pattern. Because the rule is
evaluated against every subschema, it catches violations at every nesting
level. Then register it in your configuration file:

```json hl_lines="6-8" title="one.json"
{
  "url": "https://schemas.example.com",
  "contents": {
    "my-collection": {
      "path": "./schemas",
      "lint": {
        "rules": [ "./rules/camelcase.json" ]
      }
    }
  }
}
```

Rule file paths are relative to the configuration file location. You can list
multiple rules in the array to enforce several constraints at once. Rule names
must be unique across all rules in a collection.

## Pages

A page functions as an organizational container within the instance.  Unlike
schema collections, pages don't contain schemas directly—instead, they group
other pages or schema collections together. For instance, you might create a
hierarchy of pages representing your organization's teams, where each team page
contains the schema collections they own.

| Property        | Type | Required | Default | Description |
|-----------------|------|----------|---------|-------------|
| `/title`        | String  | No  | None | The concise title of the page |
| `/description`  | String  | No  | None | A longer description of the page |
| `/email`        | String  | No  | None | The e-mail address associated with the page |
| `/github`       | String  | No  | None | The GitHub organisation or `organisation/repository` identifier associated with the page |
| `/website`      | String  | No  | None | The absolute URL to the website associated with the page |
| `/contents`     | Object  | No  | None | The nested [Collections](#collections) and [Pages](#pages) inside this page |

## Authentication

!!! success "Enterprise"

    Authentication is only available in the [Enterprise](commercial.md)
    edition. Learn more about [commercial licensing](commercial.md).

Authentication supports three policy types. An `apiKey` policy grants access to a
consumer that presents a pre-shared key, a `jwt` policy grants access to a
consumer that presents a signed JSON Web Token, verified against the issuer's
published key set, and an `oidc` policy grants access to a user who signs in
through their identity provider in the browser. The first two admit machines that
present a credential on every request, while the third authenticates a user once
and then rides a session the instance establishes. Anything not covered by a
policy stays public, so the configuration only ever describes what to protect,
never what to expose. When a path is governed by more than one policy, the
policies are unioned, so a single collection can admit both a machine presenting a
credential and a user carrying a session.

None of it is stored. Every credential is checked as it arrives, against the
environment or against the issuer's published key set, and a session is a sealed
value the instance keeps no record of, so any replica verifies any of them on
its own, with no session or state to look up and no coordination with the
others. Public key material is still fetched from an issuer and cached, which
is a lookup of what an issuer publishes rather than of who is signed in. That
is what lets an instance scale horizontally, and it is why taking access away
works differently for each policy type, described under each below. Sessions
belong to `oidc` alone: a `jwt` policy has no session and no cookie, whether or
not it names the same provider.

**The UNIX model**: Visibility and access are kept separate, following the UNIX
filesystem model. A policy that governs a directory does not erase it from its
parent's listing.  Just as `ls` reveals a directory you cannot `cd` into, a
consumer browsing the instance can tell that a governed directory exists, and
can see the names of the policies that govern it, much like UNIX shows the
owning group of a file you are not allowed to read. What stays hidden is the
content: the directory cannot be opened, nor its schemas read, without a valid
key. The policy names are disclosed on purpose, so that a consumer knows who to
ask for access. The keys themselves, and the environment variables behind them,
are never exposed.

!!! tip

    If the descriptive metadata of a governed directory, such as its title and
    description, is itself sensitive, wrap it: put the policy on a deliberately
    generic outer container that carries little metadata, and nest the
    sensitive directories inside it. Outsiders then see only the bland
    container, while the inner names and descriptions stay behind the gate.

A policy governs a [Collection](#collections) or [Page](#pages), or a namespace
above them (the instance root governs everything). It cannot gate an individual
path inside a collection: a collection is either public or private as a whole.

Every policy declares its `type`, a `name`, and the `paths` it governs,
regardless of type:

| Property        | Type | Required | Default | Description |
|-----------------|------|----------|---------|-------------|
| `/type`         | String  | :red_circle: **Yes** | N/A | The policy type, one of `apiKey`, `jwt`, or `oidc` |
| `/name`         | String  | :red_circle: **Yes** | N/A | The policy name, surfaced in directory listings. Must consist of lowercase letters, digits, and hyphens. The name `public` is reserved |
| `/paths`        | Array   | :red_circle: **Yes** | N/A | The registry paths this policy governs, each rooted at `/`. Every path must be `/` itself (governing the whole instance) or name a known collection, page, or route |

### API Key

Consumers present a key through the `Authorization` header using the `Bearer`
scheme ([RFC 6750](https://datatracker.ietf.org/doc/html/rfc6750)). Revoking one
means dropping its variable from `keys` and restarting, which leaves every other
key in the policy working.

An `apiKey` policy declares the following additional properties:

| Property        | Type | Required | Default | Description |
|-----------------|------|----------|---------|-------------|
| `/algorithm`    | String  | :red_circle: **Yes** | N/A | How a presented key is compared against the stored keys. Either `identity` (the environment variable holds the key verbatim) or `sha256` (the environment variable holds the lowercase hexadecimal SHA-256 digest of the key) |
| `/keys`         | Array   | :red_circle: **Yes** | N/A | The keys this policy accepts, each read from an environment variable so that secrets never live in the configuration file |
| `/keys/*/environmentVariable` | String | :red_circle: **Yes** | N/A | The name of the environment variable that holds the key, or its hash when `algorithm` is not `identity` |

!!! tip

    To consume schemas from a gated instance in your projects, take a look at
    our [JSON Schema CLI](https://github.com/sourcemeta/jsonschema) and its
    [`install`](https://github.com/sourcemeta/jsonschema/blob/main/docs/install.markdown)
    command, which supports authenticating against a registry using API keys
    through the `--header` option.

For example, the following instance keeps `/docs` public, gates `/partners`
behind a single key, and protects `/billing` with both a plaintext key and a
pre-hashed one:

```json title="one.json"
{
  "url": "https://schemas.example.com",
  "authentication": [
    {
      "type": "apiKey",
      "algorithm": "identity",
      "name": "partners",
      "paths": [ "/partners" ],
      "keys": [ { "environmentVariable": "ONE_PARTNERS_KEY" } ]
    },
    {
      "type": "apiKey",
      "algorithm": "identity",
      "name": "billing-plain",
      "paths": [ "/billing" ],
      "keys": [ { "environmentVariable": "ONE_BILLING_KEY" } ]
    },
    {
      "type": "apiKey",
      "algorithm": "sha256",
      "name": "billing-hashed",
      "paths": [ "/billing" ],
      "keys": [ { "environmentVariable": "ONE_BILLING_HASHED_KEY" } ]
    }
  ],
  "contents": {
    "docs": { "path": "./schemas/docs" },
    "partners": { "path": "./schemas/partners" },
    "billing": { "path": "./schemas/billing" }
  }
}
```

!!! tip

    For a `sha256` policy, store the lowercase hexadecimal digest of the key
    rather than the key itself. For example, `printf '%s' "your-key" | openssl
    dgst -sha256 | awk '{print $NF}'` prints the exact value to place in the
    environment variable.

!!! note

    Sourcemeta One intentionally does not rate limit authentication. Provided
    keys are high-entropy, guessing one is computationally infeasible, so
    throttling does not meaningfully strengthen key protection, and per-client
    counters would require the shared state that the stateless design avoids. That
    reasoning assumes strong keys, so generate them with ample entropy. Rate
    limiting still has real value as operational abuse control, against online
    guessing of a weak key, resource exhaustion, or a leaked key, but it belongs
    at the deployment edge rather than in the catalog. Comparable components take
    the same stance: the Confluent Schema Registry ships no built-in rate limiting
    and is [fronted by a reverse
    proxy](https://www.networknt.com/tutorial/proxy/schema-registry/) for it, and
    container registries throttle at the CDN edge. Put that protection in a
    [reverse proxy, API gateway, or
    WAF](https://www.gravitee.io/blog/rate-limiting-throttling-with-an-api-gateway-why-it-matters)
    in front of the instance.

### JWT

A `jwt` policy grants access to machine consumers that present a signed JSON Web
Token ([RFC 7519](https://datatracker.ietf.org/doc/html/rfc7519)) through the
`Authorization` header using the `Bearer` scheme ([RFC
6750](https://datatracker.ietf.org/doc/html/rfc6750)), as issued by an OAuth 2.0
or OpenID Connect provider. Unlike an `apiKey` policy, no shared secret lives in
the configuration or the instance: the policy names a trusted issuer, and the
instance verifies each token against the public key set that issuer publishes,
fetched over HTTP at request time and cached.

A token is admitted only when its signature verifies against the issuer's key
set, its `iss` claim matches the policy's `issuer`, its `aud` claim includes the
policy's `audience`, its signature algorithm is one the policy allows, and it is
within its validity period. A token that fails any of these is denied, with the
same response as any other unauthenticated request. Nothing here asks the
issuer whether a subject is still welcome, and no token is ever recorded as
withdrawn, so a valid token is accepted until it expires. The issuer can cut
that short by retiring the key its tokens were signed with, which reaches every
token signed under it once the cached key set refreshes. Short token lifetimes
are what bound exposure otherwise.

| Property        | Type | Required | Default | Description |
|-----------------|------|----------|---------|-------------|
| `/issuer`       | String  | :red_circle: **Yes** | N/A | The token issuer to trust, matched against the `iss` claim. When `jwksUri` is omitted, the key set is discovered from this issuer, which must then be a valid issuer identifier as per [OpenID Connect Discovery 1.0](https://openid.net/specs/openid-connect-discovery-1_0.html): an `https` URL, with any trailing slash dropped |
| `/audience`     | String  | :red_circle: **Yes** | N/A | The audience this instance identifies as. A token is accepted when its `aud` claim includes this value, so a token minted for several audiences at once is accepted as long as this one is among them. The [Model Context Protocol endpoint](api.md#model-context-protocol) is the exception: the specification has an MCP server accept only tokens issued for the endpoint itself, so a token naming the registry as a whole reaches everything this policy governs except that endpoint. Discovery for that endpoint additionally requires the instance to be served over `https`, since a resource identifier admits no other scheme |
| `/algorithms`   | Array   | :red_circle: **Yes** | N/A | The JSON Web Signature algorithms the policy accepts. One or more of `RS256`, `RS384`, `RS512`, `PS256`, `PS384`, `PS512`, `ES256`, `ES384`, `ES512`, and `EdDSA` |
| `/tokenType`    | String  | No | Any type is accepted | The `typ` header a presented token must carry, such as `at+jwt` for the [RFC 9068](https://www.rfc-editor.org/rfc/rfc9068) JSON Web Token access token profile. Set it whenever the issuer stamps one. An identity token is signed by the same issuer under the same key, and where this policy's `audience` matches the `clientId` of an `oidc` policy on that issuer, the type is the only thing distinguishing the two, so without it an identity token is accepted as an API credential |
| `/jwksUri`      | String  | No | Discovered from the issuer | The URL of the issuer's JSON Web Key Set. When omitted, it is discovered from the issuer's OpenID Connect metadata at `{issuer}/.well-known/openid-configuration`, which requires the issuer to be an `https` URL that publishes a valid OpenID Provider metadata document. Set it explicitly for an issuer that does not meet that bar |
| `/claims`       | Object  | No | Any verified token is admitted | The claims a token must carry beyond being valid, as a map from claim name to the values that admit. A token satisfies a rule by carrying any one of its values, and must satisfy every rule declared. The `scope` claim is read as the space-delimited set [RFC 6749](https://www.rfc-editor.org/rfc/rfc6749) defines, so a value matches only as a whole token within it. An array claim matches on any member, and a member that is an object is compared on its `value` sub-attribute, which is the shape [RFC 9068](https://www.rfc-editor.org/rfc/rfc9068) gives group, role, and entitlement claims. Rules on `iss`, `aud`, `exp`, `nbf`, and `iat` are refused, as the policy verifies those itself |

For example, the following instance keeps `/docs` public, gates `/partners`
behind an API key, and protects `/internal` with a JWT policy that trusts a
single issuer and audience:

```json title="one.json"
{
  "url": "https://schemas.example.com",
  "authentication": [
    {
      "type": "apiKey",
      "algorithm": "identity",
      "name": "partners",
      "paths": [ "/partners" ],
      "keys": [ { "environmentVariable": "ONE_PARTNERS_KEY" } ]
    },
    {
      "type": "jwt",
      "name": "internal",
      "paths": [ "/internal" ],
      "issuer": "https://accounts.example.com",
      "audience": "https://schemas.example.com",
      "algorithms": [ "RS256" ]
    }
  ],
  "contents": {
    "docs": { "path": "./schemas/docs" },
    "partners": { "path": "./schemas/partners" },
    "internal": { "path": "./schemas/internal" }
  }
}
```

!!! note

    The key set is fetched from the issuer over HTTP and cached as soft state,
    honouring the response's `Cache-Control` and refreshed when a token presents
    an unrecognised key identifier, so that issuer key rotation is picked up
    without restarting the instance.

To narrow that policy to the machines your issuer marks as belonging to the
platform group and granting a read scope, declare both as claim rules:

```json title="one.json"
{
  "type": "jwt",
  "name": "internal",
  "paths": [ "/internal" ],
  "issuer": "https://accounts.example.com",
  "audience": "https://schemas.example.com",
  "algorithms": [ "RS256" ],
  "claims": {
    "groups": [ "platform", "oncall" ],
    "scope": [ "registry:read" ]
  }
}
```

A token is admitted when it belongs to `platform` **or** `oncall`, **and**
carries the `registry:read` scope. Values within a rule are alternatives, and
separate rules all have to hold, so widening who a policy admits means adding
values to one rule rather than adding another.

### OIDC

An `oidc` policy grants access to a user who signs in through an OpenID Connect
provider in the browser. Where an `apiKey` or `jwt` policy admits a machine that
presents a credential on every request, an `oidc` policy authenticates a user
once at their provider and then relies on a session the instance establishes and
signs itself. Until that session exists, a browser that navigates to a governed
page is sent to begin a login, while a request for the raw schema, or from a
machine, is denied like any other unauthenticated request.

The instance registers with the provider as a client, identified by its
`clientId` and the client secret shared with it. It trusts the `issuer` both as
the value of the token's `iss` claim and as the source of the provider's OpenID
Connect metadata at `{issuer}/.well-known/openid-configuration`, from which it
discovers the signing key set that verifies each identity token. The session that
follows is signed with a secret of the instance's own, unrelated to the provider.

| Property        | Type | Required | Default | Description |
|-----------------|------|----------|---------|-------------|
| `/title`        | String  | No | The policy name | A human readable version of the policy name |
| `/issuer`       | String  | :red_circle: **Yes** | N/A | The OpenID Connect issuer to trust, matched against the identity token's `iss` claim and used to discover the provider's metadata, including the signing key set that verifies tokens. It must be a valid issuer identifier as per [OpenID Connect Discovery 1.0](https://openid.net/specs/openid-connect-discovery-1_0.html): an `https` URL, with any trailing slash dropped. Front a provider that only speaks plain HTTP with TLS termination and trust its certificate authority |
| `/clientId`     | String  | :red_circle: **Yes** | N/A | The client identifier registered with the provider for this instance |
| `/clientSecret` | Object  | :red_circle: **Yes** | N/A | The client secret shared with the provider, read from an environment variable so that it never lives in the configuration file |
| `/clientSecret/environmentVariable` | String | :red_circle: **Yes** | N/A | The name of the environment variable that holds the client secret |
| `/sessionSecrets` | Array | :red_circle: **Yes** | N/A | The secrets used to sign the session cookies this instance mints, newest first. These are the instance's own secrets, unrelated to the provider. A cookie is signed under the first and accepted under any, so adding a new secret first and dropping the old one once the sessions signed under it have expired rotates without signing anybody out. Unlike a `jwt` key set, these are read from the environment once at startup, so a change to them takes effect on restart |
| `/sessionSecrets/*` | Object | :red_circle: **Yes** | N/A | A single session signing secret |
| `/sessionSecrets/*/environmentVariable` | String | :red_circle: **Yes** | N/A | The name of the environment variable that holds the session signing secret. Generate it at random, with at least 32 characters, as with `openssl rand -base64 32`. Everything a session cookie carries but its signature travels in the open, so a secret that can be guessed is one that anybody holding a single cookie can find, after which they can mint sessions of their own |
| `/claims` | Object | No | Anybody the provider authenticates is admitted | The claims a person must carry, read exactly as on a `jwt` policy. A rule is answered against the identity token, so **every claim a rule names has to reach that token**. The login asks for them through the [OpenID Connect Core 1.0](https://openid.net/specs/openid-connect-core-1_0.html) claims request parameter where the provider supports it, which asks for them there specifically. Otherwise it asks through the scope that carries the claim, and Section 5.4 has a provider return those from the UserInfo endpoint by default under the authorization code flow, so the provider has to be configured to release them into the identity token as well. A claim no standard scope carries, such as `groups`, has to be arranged at the provider either way, since inventing a scope name risks the request being refused outright |
| `/emailDomains` | Array | No | An address is not consulted | The domains an admitted address sits at, compared against everything after the address's last `@`. Case is folded across ASCII, so a domain reaching beyond it is written either as the provider spells it or in its punycode form. An address only counts when the provider marks it verified, since [OpenID Connect Core 1.0](https://openid.net/specs/openid-connect-core-1_0.html) has a provider assert it checked ownership only then, and without that the address is whatever its holder typed. An address can also change, so this admits and nothing more: no part of the system keys identity off it |

!!! tip

    Two URLs must be registered with the provider, both derived from the
    instance's `url` and the policy's name: the redirect URI
    `{url}/self/v1/auth/callback/{name}`, and the post-logout redirect URI
    `{url}`.

!!! note

    A login that names no page to return to lands on the **first** path the
    policy declares. The order of `paths` therefore decides where signing in
    leaves somebody, though it never changes what the policy gates.

A browser holds one session per instance, whichever interactive policy
established it, so signing in with a second one ends the first. A session lasts
an hour and renews without anybody noticing, by sending the browser back to the
provider, which answers without displaying anything where the sign-in still
stands. Only a navigation renews: a script calling the API with an expired
session is denied plainly rather than redirected, since a redirect chain to an
identity provider is not something it can follow.

To narrow a policy from everyone the provider will authenticate to the people
you mean, name what they must carry:

```json title="one.json"
{
  "type": "oidc",
  "name": "corporate",
  "paths": [ "/internal" ],
  "issuer": "https://accounts.example.com",
  "clientId": "registry",
  "clientSecret": { "environmentVariable": "ONE_CLIENT_SECRET" },
  "sessionSecrets": [ { "environmentVariable": "ONE_SESSION_SECRET" } ],
  "emailDomains": [ "example.com" ],
  "claims": { "groups": [ "platform", "oncall" ] }
}
```

Somebody is admitted when their verified address sits at `example.com`, **and**
they belong to `platform` **or** `oncall`. Values within a rule are
alternatives, and separate rules all have to hold, exactly as on a `jwt` policy.

A rule is answered when somebody signs in, not on every request afterwards. So
a person the policy will never admit is told once, rather than holding a valid
session that is refused everywhere, and losing a group at the provider takes
effect within a session lifetime rather than at once. Tightening a rule in
`one.json` behaves the same way: existing sessions keep what they had until
they expire. Removing the policy outright, or rotating its session secrets,
are the immediate levers.

Because a session is a sealed value rather than a record, nothing can be struck
out. Signing out takes the session from the browser, so a copy taken beforehand
stays usable until it expires. Ending every session at once means leaving a
single new secret in `sessionSecrets`, dropping the ones sessions were signed
under, and restarting. That is the only immediate lever there is, and note that
merely adding a secret does the opposite: existing sessions keep working, which
is what makes rotation invisible. There is deliberately no OpenID Connect
Back-Channel Logout, since acting on one means keeping a record of which
sessions are dead and consulting it on every request, which is the round trip
statelessness avoids.

!!! warning

    The instance's own `url` must be an origin a browser treats as trustworthy,
    since the session cookie is only marked `Secure` on one and otherwise
    reaches the browser in the clear. That means `https`, or plain HTTP on a
    loopback address such as `http://127.0.0.1:8000`, with the special-use
    `localhost` name accepted alongside it. Indexing refuses anything else.
    Note that this is about the instance and not the provider: an `https`
    issuer does not help if the browser reaches the instance over plain HTTP.

For example, the following instance keeps `/docs` public, gates `/partners`
behind an API key, and protects `/console` with an `oidc` policy so that users
sign in through their identity provider to reach it:

```json title="one.json"
{
  "url": "https://schemas.example.com",
  "authentication": [
    {
      "type": "apiKey",
      "algorithm": "identity",
      "name": "partners",
      "paths": [ "/partners" ],
      "keys": [ { "environmentVariable": "ONE_PARTNERS_KEY" } ]
    },
    {
      "type": "oidc",
      "name": "console",
      "title": "Acme Single Sign-On",
      "paths": [ "/console" ],
      "issuer": "https://accounts.example.com",
      "clientId": "schemas-registry",
      "clientSecret": { "environmentVariable": "ONE_CONSOLE_CLIENT_SECRET" },
      "sessionSecrets": [ { "environmentVariable": "ONE_CONSOLE_SESSION_SECRET" } ]
    }
  ],
  "contents": {
    "docs": { "path": "./schemas/docs" },
    "partners": { "path": "./schemas/partners" },
    "console": { "path": "./schemas/console" }
  }
}
```

!!! tip

    Because the policies covering a path are unioned, an `oidc` policy can sit
    alongside an `apiKey` or `jwt` policy on the same path. The collection then
    admits both a machine that presents a credential and a user who signs in, so
    one endpoint can serve continuous integration and users at once.

## Extends

The `extends` property enables configuration inheritance, allowing you to build
upon existing configuration files for enhanced reusability and modularity. This
property accepts an array of file paths (relative from the configuration file
location). For example:

```json hl_lines="3" title="one.json"
{
  "url": "https://schemas.example.com",
  "extends": [ "../path/to/my/other/config/one.json" ]
}
```

If a directory path is provided to the `extends` property, the instance will
look for a file called `one.json` inside such directory.

!!! note

    Sourcemeta One processes these extensions through deep-merging, where each
    extended configuration file merges into the previous one in sequence, with
    your top-level configuration file taking final precedence over the combined
    result.

