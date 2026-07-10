---
hide:
  - navigation
---

# HTTP API

!!! info

    The HTTP API is enabled by default on every Sourcemeta One instance.
    It can be disabled by setting `api` to `false` in the
    [configuration file](configuration.md#api).

This API has been architected with performance as a primary consideration,
ensuring fast response times and efficient resource utilization across all
endpoints.

- **Cross-origin resource sharing (CORS)**: Full CORS support is implemented
  throughout the API, including proper handling of preflight `OPTIONS`
  requests, making it seamlessly compatible with browser-based applications and
  cross-origin requests
- **HTTP conventions**: Every `GET` request has a corresponding `HEAD` method.
  For brevity, we don't specify this every time
- **Errors**: Error responses follow the [RFC 9457 Problem
  Details](https://www.rfc-editor.org/rfc/rfc9457) specification for
  consistent, machine-readable error information
- **Schema Documentation**: While we don't provide an OpenAPI specification due
  to its current limitations with multi-fragment path support ([see OpenAPI
  Issue #2653](https://github.com/OAI/OpenAPI-Specification/issues/2653)) which
  make describing this API impossible, Sourcemeta One itself is comprehensively
  defined using JSON Schemas mounted in `/self/v1/schemas`

## General

### Health

*This endpoint can be used as a health check for infrastructure integration
such as Docker
[`HEALTHCHECK`](https://docs.docker.com/reference/dockerfile/#healthcheck) or
load balancer probes.*

```
GET /self/v1/health
```

=== "200"

    Empty response body.

=== "405"

    The HTTP method is not `GET` or `HEAD`.

### List

*This endpoint lists the contents of a directory at the specified `{path}`
parameter*.

```
GET /self/v1/api/list/{path?}
```

If no path is provided, the endpoint returns the contents of the root
directory. The response includes all schemas and subdirectories available at
the requested path, providing a hierarchical view of the instance structure for
navigation and discovery purposes.

=== "200"

    | Property                       | Type                     | Required | Description |
    |--------------------------------|--------------------------|-----|-------------------------------------|
    | `/url` | String | Yes | The absolute URL of the directory |
    | `/path` | String | Yes | The relative URL of the directory |
    | `/breadcrumb` | Array | Yes | The breadcrumb of the directory entry |
    | `/breadcrumb/*/name` | String | Yes | The breadcrumb entry URL path segment |
    | `/breadcrumb/*/url` | String | Yes | The relative URL of the breadcrumb location |
    | `/title` | String | No | The title associated with the directory |
    | `/description` | String | No | The description associated with the directory. The web explorer renders this as Markdown |
    | `/email` | String | No | The e-mail address associated with the directory |
    | `/github` | String | No | The GitHub organisation or repository associated with the directory |
    | `/website` | String | No | The external URL associated with the directory  |
    | `/schemas` | Integer | Yes | The recursive count of schemas in this directory |
    | `/policies` | Array | Yes | The names of the authentication policies that govern this directory |
    | `/entries` | Array | Yes | The entries inside the directory |
    | `/entries/*/type` | String | Yes | The type of the entry (`schema` or `directory`) |
    | `/entries/*/name` | String | Yes | The last URL path segment of the entry |
    | `/entries/*/path` | String | Yes | The relative URL of the entry |
    | `/entries/*/policies` | Array | Yes | The names of the authentication policies that govern the directory entry |
    | `/entries/*/health` | Integer | No | The aggregated health of the entry |
    | `/entries/*/schemas` | Integer | No | For `directory` entries, the recursive count of schemas in the directory |
    | `/entries/*/title` | String | No | The title associated with the entry |
    | `/entries/*/description` | String | No | The description associated with the entry. The web explorer renders this as Markdown |
    | `/entries/*/email` | String | No | For `directory` entries, the e-mail address associated with the entry |
    | `/entries/*/github` | String | No | For `directory` entries, the GitHub organisation or repository associated with the entry |
    | `/entries/*/website` | String | No | For `directory` entries, the website URL associated with the entry |
    | `/entries/*/dependencies` | Integer | Yes | For `schema` entries, the number of direct and indirect dependencies of the schema |
    | `/entries/*/bytes` | Integer | No | For `schema` entries, the bytes that the entry occupies |
    | `/entries/*/bytesBundled` | Integer | No | For `schema` entries, the bytes that the entry occupies when bundled |
    | `/entries/*/baseDialect` | String | No | For `schema` entries, the base dialect URI of the entry |
    | `/entries/*/dialect` | String | No | For `schema` entries, the dialect URI of the entry |
    | `/entries/*/identifier` | String | No | For `schema` entries, the absolute URI of the entry |
    | `/entries/*/alert` | String / Null | No | For `schema` entries, the human readable alert message for the schema collection. The web explorer renders this as Markdown |
    | `/entries/*/priority` | Integer | No | For `schema` entries, an importance hint from `0` (least important) to `100` (most important), inherited from the schema collection's [`x-sourcemeta-one:priority`](configuration.md) configuration value |

=== "404"

    The directory does not exist.

### Static

*This endpoint serves static assets such as stylesheets, scripts, icons, and
the web manifest used by the HTML web explorer.*

```
GET /self/v1/static/{path}
```

This endpoint is always mounted, but web assets are only served when the
[`html`](configuration.md#html) configuration option is enabled.

=== "200"

    The static asset contents with its corresponding `Content-Type` header.

=== "404"

    The static asset does not exist, or the instance is running in headless
    mode.

=== "405"

    The HTTP method is not `GET` or `HEAD`.

## Schemas

### Fetch

*This endpoint fetches the JSON Schema located at the `{path}` parameter.*

```
GET /{path}[.json][?bundle=1]
```

The `.json` extension is optional unless the HTML UI is enabled and the
`Accept` header is set to prefer an HTML response. The extension and the
`{path}` segment are matched case-insensitively within the catalog's URL
namespace, so `foo.json`, `foo.JSON`, and `FOO.json` all resolve to the same
schema. The instance's configured base path (see
[`url`](configuration.md#onejson)) remains case-sensitive per
[RFC 3986 §6.2.2.1](https://datatracker.ietf.org/doc/html/rfc3986#section-6.2.2.1).
If the `bundle` query parameter is set, the schema references are embedded
using the standard [JSON Schema
Bundling](https://json-schema.org/blog/posts/bundling-json-schema-compound-documents)
process. Meta-schemas are not embedded, as the registry serves every
meta-schema that a schema may declare.

!!! warning "`bundle` is a presence flag, not a boolean"

    Bundling is triggered by the presence of the `bundle` query parameter
    regardless of its value. `?bundle`, `?bundle=1`, `?bundle=true`,
    `?bundle=0`, and `?bundle=false` all trigger bundling. To disable
    bundling, omit the parameter entirely.


=== "200"

    The schema as JSON.

=== "404"

    The schema does not exist.

=== "405"

    The [configuration file](configuration.md) marks the schema collection as listed but not served.

### Evaluate

*This endpoint takes a JSON instance as a request body and evaluates it against
the JSON Schema located at the `{path}` parameter.*

```
POST /self/v1/api/schemas/evaluate/{path}
```

Perform exhaustive JSON Schema evaluation (including annotation collection) and
respond back using the *Basic* [JSON Schema Standard Output
Format](https://json-schema.org/draft/2020-12/json-schema-core#name-output-structure).

=== "200"

    See [JSON Schema Standard Output Formats](https://json-schema.org/draft/2020-12/json-schema-core#name-output-structure).

=== "400"

    You must pass an instance to validate against.

=== "404"

    The schema does not exist.

=== "405"

    The [configuration file](configuration.md) excludes evaluation for this schema, or the [configuration file](configuration.md) marks the schema collection as listed but not served.

=== "413"

    The request body is too large. See [RFC 9110 §15.5.14](https://datatracker.ietf.org/doc/html/rfc9110#section-15.5.14).

=== "417"

    The `Expect` header carries an expectation other than `100-continue`. See [RFC 9110 §10.1.1](https://datatracker.ietf.org/doc/html/rfc9110#section-10.1.1).

### Trace

*This endpoint takes a JSON instance as a request body and evaluates it against
the JSON Schema located at the `{path}` parameter.*

```
POST /self/v1/api/schemas/trace/{path}
```

Unlike standard schema validation, this endpoint performs a detailed trace
evaluation that exposes the complete internal validation process, including
each step, rule application, and decision point that occurs during schema
processing. This granular visibility into the validation workflow makes it
particularly valuable for debugging complex schema issues, understanding
validation failures, and for developers building JSON Schema tooling who need
insight into the validation engine's behavior and logic flow.

=== "200"

    | Property                       | Type                     | Required | Description |
    |--------------------------------|--------------------------|-----|-------------------------------------|
    | `/valid`                       | Boolean                  | Yes | Whether evaluation succeeded or not |
    | `/steps`                       | Array                    | Yes | The evaluation steps that took place |
    | `/steps/*`                     | Object                   | Yes | Each evaluation step that took place |
    | `/steps/*/type`                | `push` / `pass` / `fail` | Yes | The type of the step entry |
    | `/steps/*/message`             | String                   | Yes | A description of the step |
    | `/steps/*/evaluatePath`        | String                   | Yes | The evaluate path as a JSON Pointer |
    | `/steps/*/keywordLocation`     | String                   | Yes | The absolute keyword location as a URI |
    | `/steps/*/instanceLocation`    | String                   | Yes | The instance location as a JSON Pointer |
    | `/steps/*/name`                | String                   | Yes | The internal name of the step |
    | `/steps/*/vocabulary`          | String / Null            | Yes | The vocabulary URI that defines the keyword, if any |
    | `/steps/*/annotation`          | JSON / Null              | Yes | The annotation value produced by the step, if any |
    | `/steps/*/instancePositions`   | Array                    | Yes | The instance line positions associated with the step |
    | `/steps/*/instancePositions/0` | Integer | Yes | Starting line number |
    | `/steps/*/instancePositions/1` | Integer | Yes | Starting column number |
    | `/steps/*/instancePositions/2` | Integer | Yes | Ending line number |
    | `/steps/*/instancePositions/3` | Integer | Yes | Ending column number |

=== "400"

    You must pass an instance to validate against.

=== "404"

    The schema does not exist.

=== "405"

    The [configuration file](configuration.md) excludes evaluation for this schema, or the [configuration file](configuration.md) marks the schema collection as listed but not served.

=== "413"

    The request body is too large. See [RFC 9110 §15.5.14](https://datatracker.ietf.org/doc/html/rfc9110#section-15.5.14).

=== "417"

    The `Expect` header carries an expectation other than `100-continue`. See [RFC 9110 §10.1.1](https://datatracker.ietf.org/doc/html/rfc9110#section-10.1.1).

### RDF

!!! success "Enterprise"

    This endpoint is only available in the [Enterprise](commercial.md)
    edition. Learn more about [commercial licensing](commercial.md).

*This endpoint takes a JSON object wrapping an instance as its request body,
validates the instance against the JSON Schema located at the `{path}`
parameter, and promotes it to
[JSON-LD](https://www.w3.org/TR/json-ld11/) (and therefore RDF) using the
schema's `x-jsonld-*` annotations.*

```
POST /self/v1/api/schemas/rdf/{path}
```

The same schema that validates the instance declares how it maps to Linked
Data, keeping a single source of truth for both concerns. The annotation
vocabulary is documented in the [JSON Schema CLI RDF
documentation](https://github.com/sourcemeta/jsonschema/blob/main/docs/rdf.markdown).
A schema without such annotations (including schemas on dialects older than
2019-09, which do not support annotation collection) produces an empty
JSON-LD document. Availability follows the evaluate flag: schemas excluded
from evaluation in the [configuration file](configuration.md) cannot be
promoted.

Unlike the evaluate and trace endpoints, the request body is not the bare
instance, but a JSON object with the following properties:

| Property | Type | Required | Description |
|----------|------|----------|-------------|
| `/instance` | JSON | Yes | The instance to validate and promote |
| `/flatten` | Boolean | No | Whether to flatten the resulting document. Defaults to `false` |
| `/context` | Object | No | A JSON-LD context to compact (or, with `flatten`, flatten) the resulting document against |

Without a `context`, the response is the document in [expanded
form](https://www.w3.org/TR/json-ld11/#expanded-document-form) (or [flattened
form](https://www.w3.org/TR/json-ld11/#flattened-document-form) with
`flatten`). With a `context`, the response is in [compacted
form](https://www.w3.org/TR/json-ld11/#compacted-document-form). Remote
contexts are not fetched.

=== "200"

    The instance promoted to JSON-LD, served as `application/ld+json`.

=== "400"

    You must pass a request body that matches the request schema, or the
    provided `context` cannot be processed (for example because it references
    a remote context).

=== "404"

    The schema does not exist.

=== "405"

    The [configuration file](configuration.md) excludes evaluation for this
    schema, or the [configuration file](configuration.md) marks the schema
    collection as listed but not served.

=== "413"

    The request body is too large. See [RFC 9110 §15.5.14](https://datatracker.ietf.org/doc/html/rfc9110#section-15.5.14).

=== "417"

    The `Expect` header carries an expectation other than `100-continue`. See [RFC 9110 §10.1.1](https://datatracker.ietf.org/doc/html/rfc9110#section-10.1.1).

=== "422"

    The instance does not conform to the schema (the problem details carry
    the standard [JSON Schema output
    errors](https://json-schema.org/draft/2020-12/json-schema-core#name-output-structure)),
    or the schema's JSON-LD annotations cannot be resolved for this instance
    (the problem details carry the offending annotation facet and instance
    location).

### Metadata

*This endpoint retrieves metadata information about the JSON Schema located at
the `{path}` parameter.*

```
GET /self/v1/api/schemas/metadata/{path}
```

=== "200"

    | Property                       | Type                     | Required | Description |
    |--------------------------------|--------------------------|-----|-------------------------------------|
    | `/path` | String | Yes | The relative URL of the schema |
    | `/identifier` | String | Yes | The absolute URI of the schema |
    | `/dialect` | String | Yes | The dialect URI of the schema |
    | `/baseDialect` | String | Yes | The base dialect URI of the schema |
    | `/health` | Integer | Yes | The health score of the schema |
    | `/priority` | Integer | Yes | An importance hint from `0` (least important) to `100` (most important), inherited from the schema collection's [`x-sourcemeta-one:priority`](configuration.md) configuration value |
    | `/dependencies` | Integer | Yes | The number of direct and indirect dependencies of the schema |
    | `/bytes` | Integer | Yes | The bytes that the schema occupies |
    | `/bytesBundled` | Integer | Yes | The bytes that the schema occupies when bundled |
    | `/alert` | String / Null | No | The human readable alert message for the schema collection, if any. The web explorer renders this as Markdown |
    | `/title` | String | No | The title of the schema, if any |
    | `/description` | String | No | The description of the schema, if any. The web explorer renders this as Markdown |
    | `/examples` | Array | Yes | Up to 10 of the schema examples, if any |
    | `/breadcrumb` | Array | Yes | The breadcrumb of the schema |
    | `/breadcrumb/*/name` | String | Yes | The breadcrumb entry URL path |
    | `/breadcrumb/*/path` | String | Yes | The relative URL of the breadcrumb location |

=== "404"

    The schema does not exist.

### Search

*This endpoint searches for JSON Schemas based on the provided query `{term}`.*

```
GET /self/v1/api/schemas/search?q={term}[&limit={n}][&scope={fields}]
```

| Parameter | Type | Required | Default | Description |
|-----------|------|----------|---------|-------------|
| `q` | String | Yes | - | Search term (case-insensitive substring, at most 256 characters) |
| `limit` | Integer | No | 10 | Maximum number of results, between 1 and 100 |
| `scope` | String | No | `path,title,description` | Comma-separated fields to match against |

The valid values for `scope` are `path`, `title`, and `description`, which can
be combined in any order (i.e. `scope=title,description`).

=== "200"

    | Property | Type | Required | Description |
    |----------|------|----------|-------------|
    | `/*/path` | String | Yes | The relative URL of the schema |
    | `/*/identifier` | String | Yes | The absolute URL of the schema |
    | `/*/title` | String | No | The title of the schema (may be an empty string) |
    | `/*/description` | String | No | The description of the schema (may be an empty string). The web explorer renders this as Markdown |
    | `/*/priority` | Integer | Yes | An importance hint from `0` (least important) to `100` (most important), inherited from the schema collection's [`x-sourcemeta-one:priority`](configuration.md) configuration value |
    | `/*/health` | Integer | Yes | The health score of the schema (0 to 100) |

=== "400"

    Returned when `q` is missing or empty, when `q` or `limit` do not match
    their corresponding ranges, or any query parameter has an invalid type.

### Dependencies

*This endpoint retrieves all direct and indirect dependencies of the JSON
Schema located at the specified `{path}` parameter.*

```
GET /self/v1/api/schemas/dependencies/{path}
```

=== "200"

    | Property | Type | Required | Description |
    |----------|------|----------|-------------|
    | `/*/from` | String | Yes | The absolute URL of the schema that originates the dependency |
    | `/*/to` | String | Yes | The absolute URL of the schema being referenced |
    | `/*/at` | String | Yes | The JSON Pointer to the schema location where the dependency originates |

=== "404"

    The schema does not exist.

### Dependents

*This endpoint retrieves all direct and transitive dependents of the JSON
Schema located at the specified `{path}` parameter. A dependent is a schema
that references this schema, either directly or indirectly through a chain of
references.*

```
GET /self/v1/api/schemas/dependents/{path}
```

=== "200"

    | Property | Type | Required | Description |
    |----------|------|----------|-------------|
    | `/*/from` | String | Yes | The absolute URL of the schema that originates the dependency |
    | `/*/to` | String | Yes | The absolute URL of the schema being referenced |
    | `/*/at` | String | Yes | The JSON Pointer to the schema location where the dependency originates |

=== "404"

    The schema does not exist.

### Health

*This endpoint retrieves the health analysis and score for the JSON Schema located at the specified `{path}` parameter.*

```
GET /self/v1/api/schemas/health/{path}
```

=== "200"

    | Property | Type | Required | Description |
    |----------|------|----------|-------------|
    | `/score` | Integer | Yes | The overall health score of the schema (0 to 100) |
    | `/errors` | Array | Yes | Array of health issues found in the schema |
    | `/errors/*/pointers` | Array | Yes | The paths where the issue occurs |
    | `/errors/*/pointers/*` | String | Yes | A JSON Pointer path of where the issue occurs |
    | `/errors/*/name` | String | Yes | The identifier name of the health issue |
    | `/errors/*/message` | String | Yes | Human-readable description of the issue. The web explorer renders this as Markdown |
    | `/errors/*/description` | String / Null | No | Additional description (may be null). The web explorer renders this as Markdown |

=== "404"

    The schema does not exist.

### Locations

*This endpoint retrieves metadata about every URI associated with the JSON
Schema located at the specified `{path}` parameter, including schema resources,
subschemas, anchors, and more.*

```
GET /self/v1/api/schemas/locations/{path}
```

=== "200"

    | Property | Type | Required | Description |
    |----------|------|----------|-------------|
    | `/static` | Object | Yes | Static URI locations within the schema |
    | `/static/*` | Object | Yes | Metadata for a specific URI location |
    | `/static/*/base` | String | Yes | The base URI of the location |
    | `/static/*/baseDialect` | String | Yes | The base dialect of the schema |
    | `/static/*/dialect` | String | Yes | The JSON Schema dialect URI |
    | `/static/*/parent` | String / Null | No | The parent JSON Pointer (if any) |
    | `/static/*/pointer` | String | Yes | The JSON Pointer to this location from the root of the schema |
    | `/static/*/relativePointer` | String | Yes | The relative JSON Pointer from its nearest base URI |
    | `/static/*/root` | String | Yes | The root URI of the schema |
    | `/static/*/type` | String | Yes | The type of location |
    | `/static/*/position` | Array | Yes | The entry location positions |
    | `/static/*/position/0` | Integer | Yes | Starting line number |
    | `/static/*/position/1` | Integer | Yes | Starting column number |
    | `/static/*/position/2` | Integer | Yes | Ending line number |
    | `/static/*/position/3` | Integer | Yes | Ending column number |
    | `/static/*/propertyName` | Boolean | Yes | Whether the location applies to a property name |
    | `/static/*/orphan` | Boolean | Yes | Whether the location is inside a definitions container |
    | `/dynamic` | Object | Yes | Dynamic URI locations within the schema |
    | `/dynamic/*` | Object | Yes | Metadata for a specific URI location |
    | `/dynamic/*/base` | String | Yes | The base URI of the location |
    | `/dynamic/*/baseDialect` | String | Yes | The base dialect of the schema |
    | `/dynamic/*/dialect` | String | Yes | The JSON Schema dialect URI |
    | `/dynamic/*/parent` | String / Null | No | The parent URI (if any) |
    | `/dynamic/*/pointer` | String | Yes | The JSON Pointer to this location from the root of the schema |
    | `/dynamic/*/relativePointer` | String | Yes | The relative JSON Pointer from its nearest base URI |
    | `/dynamic/*/root` | String | Yes | The root URI of the schema |
    | `/dynamic/*/type` | String | Yes | The type of location |
    | `/dynamic/*/position` | Array | Yes | The entry location positions |
    | `/dynamic/*/position/0` | Integer | Yes | Starting line number |
    | `/dynamic/*/position/1` | Integer | Yes | Starting column number |
    | `/dynamic/*/position/2` | Integer | Yes | Ending line number |
    | `/dynamic/*/position/3` | Integer | Yes | Ending column number |
    | `/dynamic/*/propertyName` | Boolean | Yes | Whether the location applies to a property name |
    | `/dynamic/*/orphan` | Boolean | Yes | Whether the location is inside a definitions container |

=== "404"

    The schema does not exist.

=== "405"

    The [configuration file](configuration.md) marks the schema collection as listed but not served.

### Stats

*This endpoint retrieves keyword statistics, by vocabulary, about every URI
associated with the JSON Schema located at the specified `{path}` parameter.*

```
GET /self/v1/api/schemas/stats/{path}
```

=== "200"

    | Property | Type | Required | Description |
    |----------|------|----------|-------------|
    | `/<vocabulary>/<keyword>` | Integer | Yes | The number of occurrences keywords of a specific keyword name from a specific vocabulary (or `unknown` otherwise) |

=== "404"

    The schema does not exist.

=== "405"

    The [configuration file](configuration.md) marks the schema collection as listed but not served.

### Positions

*This endpoint retrieves line and column position information for every token
in the JSON Schema located at the specified `{path}` parameter.*

```
GET /self/v1/api/schemas/positions/{path}
```

The result is a JSON object where every property is JSON Pointer to the given
schema.

=== "200"

    | Property | Type | Required | Description |
    |----------|------|----------|-------------|
    | `/*/0` | Integer | Yes | Starting line number |
    | `/*/1` | Integer | Yes | Starting column number |
    | `/*/2` | Integer | Yes | Ending line number |
    | `/*/3` | Integer | Yes | Ending column number |

=== "404"

    The schema does not exist.

=== "405"

    The [configuration file](configuration.md) marks the schema collection as listed but not served.

## Model Context Protocol

!!! success "Enterprise"

    The Model Context Protocol server is only available in the
    [Enterprise](commercial.md) edition. Learn more about [commercial
    licensing](commercial.md).

*This endpoint exposes the entire instance as a [Model Context
Protocol](https://modelcontextprotocol.io) server, offering every JSON Schema
in the catalog as an MCP resource and every action documented above as an
MCP tool.*

```
POST /self/v1/mcp
```

Sourcemeta One implements the [Streamable
HTTP](https://modelcontextprotocol.io/specification/2025-06-18/basic/transports#streamable-http)
transport with full CORS support, and enforces the [`Origin` header
validation](https://modelcontextprotocol.io/specification/2025-06-18/basic/transports#security-warning)
principle recommended by the specification to protect against DNS rebinding
attacks.

**Protocol versions.** The server simultaneously supports three revisions of
the MCP specification:
[`2025-03-26`](https://modelcontextprotocol.io/specification/2025-03-26),
[`2025-06-18`](https://modelcontextprotocol.io/specification/2025-06-18), and
[`2025-11-25`](https://modelcontextprotocol.io/specification/2025-11-25).
Clients pick a revision on every request via the `MCP-Protocol-Version`
header, falling back to `2025-03-26` when omitted as the [specification
requires](https://modelcontextprotocol.io/specification/2025-06-18/basic/transports#protocol-version-header).
We recommend connecting with the latest revision to take advantage of all
available features, but the server gracefully degrades to support older
clients as needed.

**Resources.** Every JSON Schema in the catalog is offered as an MCP resource
addressable by its canonical absolute URI, served in paginated form. The
configured [`x-sourcemeta-one:priority`](configuration.md) hint of each
schema is mapped to the standard MCP `annotations.priority` value, so that
AI clients can rank schemas by their declared importance.

**Tools.** Every action documented in this page is also exposed as an MCP
tool that performs the same work over JSON-RPC. The server supports the full
range of advanced MCP tool features applicable to each revision, including
per-tool
[`outputSchema`](https://modelcontextprotocol.io/specification/2025-11-25/server/tools#output-schema)
declarations, [structured
content](https://modelcontextprotocol.io/specification/2025-11-25/server/tools#structured-content)
responses,
[`resource_link`](https://modelcontextprotocol.io/specification/2025-11-25/server/tools#resource-links)
content blocks, [JSON-RPC
batching](https://www.jsonrpc.org/specification#batch), and the standard
[tool
hints](https://modelcontextprotocol.io/specification/2025-11-25/server/tools#tool)
(`readOnlyHint`, `destructiveHint`, `idempotentHint`, `openWorldHint`).

!!! warning "Tool argument stringification"

    Note several widely-deployed MCP clients incorrectly stringify non-string
    tool arguments before transmission. See the following open tickets:
    [`anthropics/claude-code#24599`](https://github.com/anthropics/claude-code/issues/24599)
    and
    [`modelcontextprotocol/python-sdk#1112`](https://github.com/modelcontextprotocol/python-sdk/issues/1112).
    We work around this on a per-tool basis as needed so that affected clients
    remain fully interoperable with Sourcemeta One. We aim to eventually revert
    these workarounds once the client ecosystem stabilises.

!!! warning "MCP client schema compliance"

    The MCP ecosystem is young and several widely-deployed clients get confused
    by any tool `inputSchema` beyond the most trivial shape, rejecting or
    silently degrading legal JSON Schema constructs such as `$ref` (including
    the schemas-as-resources pattern this server relies on). OpenAI Codex is
    currently among the most affected. See for example
    [`openai/codex#13746`](https://github.com/openai/codex/issues/13746),
    [`openai/codex#3152`](https://github.com/openai/codex/issues/3152), and
    [`openai/codex#4176`](https://github.com/openai/codex/issues/4176).  Our
    stance on broad compliance gaps like these is to *not* work around them on
    our side. Mangling the MCP surface to fit each non-compliant client would
    compromise the integrity of the specification for every other client. We
    will instead keep the server faithful to the specification and put pressure
    on MCP client providers to reach a reasonable level of compliance.

=== "200"

    A JSON-RPC response envelope, or, on `2025-03-26`, a batch of envelopes.

=== "202"

    Acknowledged notification with no body.

=== "400"

    The `MCP-Protocol-Version` header is not one of the supported revisions.

=== "403"

    The request `Origin` does not match the configured instance URL.

=== "405"

    The HTTP method is not `POST` or `OPTIONS`.

=== "413"

    The request body exceeds the maximum allowed size.
