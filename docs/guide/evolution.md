# Schema Evolution and Versioning

Static analysis of schema evolution in JSON Schema is something we are actively
working on, and it is a genuinely complex problem. 

Other schema formats like Apache Avro and Protocol Buffers have solved
compatibility detection reasonably well because their type systems are
constrained and predictable.  However, JSON Schema is a different beast. Its
expressiveness is its strength, but it also makes automated analysis
significantly harder to get right in cases beyond the trivial.

This page covers what is available today, practical guidance on versioning, and
the roadmap for where automated analysis is heading.

## Versioning strategy is an organisational decision, not a tooling decision

Sourcemeta One deliberately does not impose a versioning scheme. This is an
architectural decision, not a gap, as versioning strategies across the industry
are genuinely disparate:

- **ISO standards** combine a standard number with a publication year: `ISO
  4217:2015`, `ISO 8601:2004`. The year signals the edition without implying
  any compatibility relationship with prior editions
- **The JSON Schema specification** uses date-based identifiers: `2020-12`,
  `2019-09`. Unambiguous, but carries no compatibility signal
- **[Semantic Versioning](https://semver.org/)** (`1.2.3`) encodes a
  compatibility signal in the version number itself
- **[SchemaVer](https://snowplow.io/blog/introducing-schemaver-for-semantic-versioning-of-schemas)**
  adapts SemVer for schemas with a `MODEL-REVISION-ADDITION` structure,
  replacing major/minor/patch with terms that map to data compatibility
- **Monotonic integers** (`v1`, `v2`, `v3`) make no claim about the nature of
  the change. Simple and widely used in internal API governance

A registry that forces one scheme creates friction for organisations already
using another. For this reason, Sourcemeta One treats version identifiers as
opaque path segments and accommodates any convention.

## Why SemVer is harder to apply to schemas than it looks

[Semantic Versioning](https://semver.org/) is tempting because it is familiar
and encodes useful compatibility signals. But applying it to JSON Schema
involves decisions the ecosystem has not yet settled. For example:

**Annotations.** Keywords like
[`default`](https://www.learnjsonschema.com/2020-12/meta-data/default/) produce
annotations rather than constraints. Consumers using schemas for documentation
generation or UI rendering may depend on them. Is changing a default value a
breaking change?  SchemaVer may say patch. A consumer whose UI break may
disagree.

**Anchors and definitions.** If another schema references a named anchor or
internal definition directly and you rename it during a refactor, you have
broken a consumer without touching any validation behaviour.

[SchemaVer](https://snowplow.io/blog/introducing-schemaver-for-semantic-versioning-of-schemas)
addresses some of this with MODEL/REVISION/ADDITION semantics. But even
SchemaVer acknowledges grey areas requiring author judgement. 

The fundamental challenge is that [JSON Schema is a constraint
system](https://modern-json-schema.com/json-schema-is-a-constraint-system), not
a modelling language. In most programming languages, adding something is
backwards compatible. In JSON Schema, adding more typically means permitting
less. What looks like an addition is often a restriction, and software
versioning intuitions become actively misleading.

## Version as a dedicated path component

A JSON Schema is uniquely identified by its URI. Two versions cannot share the
same URI, thus a dedicated path component is the clearest convention:
`/customers/customer/v1.json`, `/customers/customer/v2.json`. It makes versions
visible, navigable, and directly addressable, with both versions served
simultaneously and unambiguously.

In practice, we found that a simple monotonic major version is more than enough
for most organisations. A single integer, incremented when a breaking change is
needed, sidesteps SemVer ambiguity and gives you all the versioning room you
need while keeping it simple.

## Schema-level versus data model-level versioning

Should you version each schema individually, or version an entire collection
together as a data model? For example:

- **Per-schema versioning:** `/project/customer/v1.json`,
  `/project/invoice/v1.json`, `/project/customer/v2.json`. Each schema evolves
  independently
- **Data model versioning:** `/project/v1/customer.json`,
  `/project/v1/invoice.json`, `/project/v2/customer.json`. The entire namespace
  is versioned together

Data model versioning makes sense when schemas are tightly coupled and
genuinely always evolve together. For example, a wire protocol or standards
specification. However, for most API governance scenarios, it creates a
practical problem. When one schema out of twenty changes, the other nineteen
are copied unchanged into the new namespace. Consumers face multiple identical
copies with no way to know what actually changed, and diffs become noise.

For this reason, we suggest per-schema versioning: version only what changes,
only when it changes.

## Guaranteeing compatibility today: testing

This is exactly how software libraries are versioned and published throughout
the industry. 

For example, The NPM registry does not validate that your `2.0.0` is genuinely
incompatible with `1.x`. The guarantee comes from the author's discipline,
their test suite, and their changelog. Assuming we treat schemas like code,
this should feel familiar: the same model that governs every package you depend
on governs your schemas here.

The [`jsonschema
test`](https://github.com/sourcemeta/jsonschema/blob/main/docs/test.markdown)
command is the unit test runner for schemas. A compatibility test suite asserts
two things: instances that must remain valid under the new version, and
instances that must remain invalid. Running this against both versions tells
you whether a change is backwards compatible.

```bash
jsonschema test /path/to/schema/tests/
```

Run this in CI on every proposed change to a canonical schema before it merges,
just like you would do for any production code.

## Redirecting consumers when schemas move

In practice, schemas move: concepts get renamed, directories restructured,
versions superseded. Many do not realise that JSON Schema handles this natively
via [`$ref`](https://www.learnjsonschema.com/2020-12/core/ref/) forwarding.

When you need to move a schema, leave a minimal schema with a
[`$ref`](https://www.learnjsonschema.com/2020-12/core/ref/) to the new location
and if on 2019-09 and later, set the
[`deprecated`](https://www.learnjsonschema.com/2020-12/meta-data/deprecated/)
keyword to `true`.

```json
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "deprecated": true,
  "$ref": "../path/to/new/v2.json"
}
```

Consumers resolving the old URI transparently follow the reference. Tooling
that honours
[`deprecated`](https://www.learnjsonschema.com/2020-12/meta-data/deprecated/)
surfaces a migration warning, and the old URI keeps working. The platform team
can then monitor deprecated URIs still receiving traffic and track adoption of
the new version.

!!! NOTE "Coming Soon"

    Sourcemeta One will add HTTP-level `3xx` redirection at deprecated URIs so
    HTTP clients and caching layers handle the transition natively. This can be
    convenient when moving directories with a non-trivial amount of schemas.

## What is coming: breaking change detection and lineage

We are actively working on shipping the following features:

**Breaking change detection.** When Sourcemeta One is deployed, it will analyse
changes against the previously published state and refuse to complete if any
modification to an existing schema URI would break consumers: a removed
required property, a narrowed type, a tightened constraint, or a deleted
schema. The registry deployment becomes a compatibility gate, not just a
publication step.

**Schema lineage.** There is currently no formal way for a schema to declare it
is a newer version of a previous one beyond directory conventions. We are
introducing an `x-supersedes` keyword that lets a schema explicitly declare
which URI it supersedes, independent of its location. This survives
reorganisation and allows forks: a schema can supersede two ancestors,
modelling a merge of previously separate concepts. Sourcemeta One will
visualise these chains in the web explorer. Snowplow's [Iglu
registry](https://docs.snowplow.io/docs/fundamentals/schemas/versioning/) has
pioneered a similar concept with their `$supersedes` keyword. For organisations
with consistent directory-based versioning, Sourcemeta One will also support
automatic lineage inference during deployment from path patterns.

!!! NOTE "Coming Soon"

    Schema evolution tooling is actively in development and will land
    incrementally. If breaking change detection or lineage tracking are
    critical to your evaluation, [get in touch](mailto:hello@sourcemeta.com) to
    discuss timelines and priority.
