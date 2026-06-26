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
| `/url`          | String  | :red_circle: **Yes** | N/A | The absolute URL on which the instance will be served. Sourcemeta One will automatically add URI identifiers relative to this URL for every ingested schema. The absolute URL _may_ have a path component. The path component of this URL is case-sensitive per [RFC 3986 §6.2.2.1](https://datatracker.ietf.org/doc/html/rfc3986#section-6.2.2.1). Inside the instance's URL namespace (schema URIs the catalog owns), path lookups are case-insensitive |
| `/extends`      | Array   | No  | None | One or more configuration files to extend from. See the [Extends](#extends) section for more information |
| `/contents`     | Object  | No  | None | The top-level [Collections](#collections) and [Pages](#pages) that compose the instance |
| `/html`        | Object or Boolean  | No  | `{}` | Settings for the HTML explorer. If set to `false`, the instance runs in headless mode. Enabling the HTML explorer implies the API must also be enabled. See the [HTML](#html) section for more details |
| `/api`         | Object or Boolean  | No  | `{}` | Controls whether the HTTP API is accessible. If set to `false`, the JSON API is disabled. Can only be set to `false` when `/html` is also set to `false` |
| `/authentication` (**Enterprise**) | Array  | No  | None | A list of authentication policies that govern this instance. Anything not covered by a policy remains public. See the [Authentication](#authentication) section for more details |

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

Authentication currently supports a single policy type, `apiKey`, where a
consumer is granted access by presenting a pre-shared key. Support for OpenID
Connect and JWT is planned. Anything not covered by a policy stays public, so
the configuration only ever describes what to protect, never what to expose.
When a path is governed by more than one policy, the policies are unioned.

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

| Property        | Type | Required | Default | Description |
|-----------------|------|----------|---------|-------------|
| `/type`         | String  | :red_circle: **Yes** | N/A | The policy type. Currently only `apiKey` is supported |
| `/name`         | String  | :red_circle: **Yes** | N/A | The policy name, surfaced in directory listings. Must consist of lowercase letters, digits, and hyphens. The name `public` is reserved |
| `/algorithm`    | String  | :red_circle: **Yes** | N/A | How a presented key is compared against the stored keys. Either `identity` (the environment variable holds the key verbatim) or `sha256` (the environment variable holds the lowercase hexadecimal SHA-256 digest of the key) |
| `/paths`        | Array   | :red_circle: **Yes** | N/A | The registry paths this policy governs, each rooted at `/`. Every path must name a known collection, page, or route |
| `/keys`         | Array   | :red_circle: **Yes** | N/A | The keys this policy accepts, each read from an environment variable so that secrets never live in the configuration file |
| `/keys/*/environmentVariable` | String | :red_circle: **Yes** | N/A | The name of the environment variable that holds the key, or its hash when `algorithm` is not `identity` |

### API Key

Consumers present a key through the `Authorization` header using the `Bearer`
scheme ([RFC 6750](https://datatracker.ietf.org/doc/html/rfc6750)).

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

