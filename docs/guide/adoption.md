# Incrementally Adopting Schema Governance

At this point, the model makes sense, and now you are looking at your actual
situation: hundreds or thousands of APIs in various states of documentation,
owned by different teams, some with OpenAPI specs and some without.

The key principle: **you do not need to clean up your existing mess before you
start. You start alongside it, one API at a time, while development continues
uninterrupted.**

## Step 0: Know what APIs you have

[Axway's 2024 State of Enterprise API Maturity
report](https://resources.axway.com/build-api-marketplace/rpt-state-of-enterprise-api-maturity-in-2024-en)
found that 78% of enterprise leaders do not know how many APIs their
organisation has. If you are in that category, start here. The goal is not a
perfect catalog, just a working list good enough to identify where to begin.

**Ask the teams.** A quick conversation with platform and engineering leads,
plus a shared spreadsheet of API name, owner, traffic, and spec status, will
surface the most important APIs quickly.

**Check your API gateways.** Kong, AWS API Gateway, Azure API Management, and
similar tools hold route configurations that are often the most reliable
inventory of what is actually running in production.

**Scan your infrastructure.** [Optic](https://www.useoptic.com/) can discover
APIs passively from network traffic, which is often the only way to find shadow
APIs.

Once you have enough visibility to identify a first candidate API, move on. The
inventory can be refined in parallel.

## Step 1: Get OpenAPI specs, however you can

If your APIs already have OpenAPI specs, skip to Step 2. If not, the goal is a
working first draft. Quality comes later.

**Use an LLM.** Point [Claude Code](https://claude.ai/download), [GitHub
Copilot](https://github.com/features/copilot), or any other AI code assistant
at your codebase and ask it to generate an OpenAPI spec. Faster than writing
from scratch, and usually good enough to start.

**Generate from your framework.** Most frameworks have OpenAPI generators built
in or available as plugins. [OpenAPI
Generator](https://openapi-generator.tech/) covers a wide range of server
frameworks.

**Infer from gateway traffic.** [Optic](https://www.useoptic.com/) can generate
and maintain OpenAPI specs from observed traffic. Kong and AWS API Gateway have
supported tooling for this as well.

The result will have inline schemas, inconsistent naming, and missing
annotations. That is fine to start. Quality will come later.

## Step 2: Verify the spec matches your running API

Before extracting anything, verify the spec actually reflects what the API
does. [APIContext's
research](https://apicontext.com/resources/api-drift-white-paper/), based on
validating billions of API calls, found **75% of production APIs have variances
to their published OpenAPI specifications**. Extracting schemas from a spec
that does not match reality means canonicalising fiction.

We recommend you add one of the following tools to your workflow:

- **[Prism](https://stoplight.io/open-source/prism)**. A popular validation
  proxy that checks live API responses against the spec; also serves as a mock
  server
- **[Schemathesis](https://schemathesis.readthedocs.io/)**. A fantastic tool
  that automatically generates test cases from the spec and fires them at the
  running API
- **[Optic](https://www.useoptic.com/)**. A tool that diffs actual traffic
  against the spec in CI, flagging drift as it accumulates

Fix mismatches before moving on. If the spec is wrong, everything built on top
of it is wrong too.

## Step 3: Pick one API to start with

Pick a single API to work through the full cycle first. The best choice is
actively maintained, reasonably well understood, and likely to share concepts
with other APIs so the schemas you extract pay dividends immediately.

A good first candidate is usually a core domain API: something defining
`Customer`, `Order`, or `Product` that you already suspect exists in multiple
forms across your landscape.

## Step 4: Set up your schema repository and deploy the registry

Unlike many registries that enforce a rigid two-level namespace, Sourcemeta One
lets you define the full directory hierarchy to any depth, mirroring your
domain model exactly. While convenient, that flexibility can initially feel
daunting.

The answer: start simple. Structure matters less than you think, because you
can always reorganise later in a backwards-compatible way. For example:

```
/shared
  /primitives
    /money
      v1.json
/customers
  /identity
    /customer
      v1.json
/finance
  /invoicing
    /invoice
      v1.json
```


*The only thing we strongly recommend at this stage is the use of a version in
every schema path.* A JSON Schema is uniquely identified by its URI, which
means the version must be part of the file path. If you create
`/customers/customer.json` today and dozens of APIs point to it, you have no
room for breaking changes later.  `/customers/customer/v1.json` gives you that
room.

Sourcemeta One supports arbitrary versioning strategies, so do not overthink
it: a single number is enough to start. Versioning and schema evolution are
covered later in this guide.

!!! NOTE "Let us handle `$id`"

    When a schema has an explicit identifier,
    [`$ref`](https://www.learnjsonschema.com/2020-12/core/ref/)s inside it
    resolve against the
    [`$id`](https://www.learnjsonschema.com/2020-12/core/id/) rather than the
    file path, which is confusing during local development where files are just
    files on disk. Without
    [`$id`](https://www.learnjsonschema.com/2020-12/core/id/), relative
    [`$ref`](https://www.learnjsonschema.com/2020-12/core/ref/)s resolve
    predictably against the file system, which is more intuitive for local
    development. The registry assigns
    [`$id`](https://www.learnjsonschema.com/2020-12/core/id/)s automatically
    based on each schema's path and deployment configuration, so you get the
    best of both worlds.

Now deploy Sourcemeta One pointing at this repository. It will be mostly empty
at this stage, but you need it running to assess the quality of everything you
extract next.

!!! SUCCESS "Enterprise"

    If you have an [Enterprise](../commercial.md) license, your registry comes
    pre-loaded with the Standard Library: thousands of schemas mapped to ISO,
    IETF, W3C, and other open standards. Before extracting schemas from your
    first API, browse the Standard Library first. You may find that a
    significant portion of what you were about to extract already exists:
    dates, currencies, country codes, addresses, financial data models, and
    more. What you need to add is only what is genuinely specific to your
    organisation.

## Step 5: Extract the schemas from your first API

Split the OpenAPI spec into separate files using a tool such as [`redocly
split`](https://redocly.com/docs/cli/commands/split/):

```bash
redocly split openapi.yaml --outDir ./split
```

Review the output, place the schema files into your central repository under
the appropriate versioned paths, and commit.

Now check out the registry's health analysis. This is where the process becomes
self-guiding: the health report will flag missing descriptions, overly
permissive types, inconsistent patterns, and much more. Work through the
issues, commit improvements, and watch the score rise. You do not need to
decide what good looks like in the abstract. The health checks tell you. Trust
the process: the closer you get to 100%, the better your schemas become.

!!! SUCCESS "Enterprise"

    The [Enterprise](../commercial.md) edition lets you configure the registry
    with custom linting rules that encode your own organisation's specific
    guidelines: naming conventions, required annotations, structural patterns,
    or any constraint that matters to your governance standards. Each rule can
    include your own explanation and remediation guidance, so when a schema
    fails a check, the developer sees exactly what is expected and why. This
    turns the registry's health analysis into a living style guide for your
    data contracts.

## Step 6: Update the OpenAPI spec to reference the registry

Replace inline schema definitions with
[`$ref`](https://www.learnjsonschema.com/2020-12/core/ref/)s pointing at the
registry:

```yaml
# Before
components:
  schemas:
    Customer:
      type: object
      properties:
        id:
          type: string

# After
components:
  schemas:
    Customer:
      $ref: 'https://schemas.yourcompany.com/customers/identity/customer/v1.json'
```

!!! TIP

    Run [`redocly bundle`](https://redocly.com/docs/cli/commands/bundle/) to
    produce the distributable single-file spec for tooling that requires it.

If you prefer schemas on disk rather than live references, [`jsonschema
install`](https://github.com/sourcemeta/jsonschema/blob/main/docs/install.markdown)
fetches schemas with integrity verification into your own repository, removing
the runtime dependency on the registry.

Finally, don't forget to re-run your conformance tests from Step 2 to confirm
the API still matches the updated spec.

## Step 7: Move to the next API

Before extracting schemas from your second API, check what is already in the
registry. Anything that already exists should be referenced, not re-extracted.

For each schema in the second API's spec: if it already exists in the registry,
replace the inline definition with a
[`$ref`](https://www.learnjsonschema.com/2020-12/core/ref/). If not, extract it
first, then reference it.

This is where the fragmentation cycle stops. The effort for the second API is
less than the first. The third less than the second. Each iteration makes the
registry more comprehensive and the next iteration faster.

## The cycle compounds

Early on, most schemas in each new API are novel. Over time, most already exist
in the registry. The proportion of new work shrinks with each iteration.

The platform team monitors what is being extracted: which schemas appear across
APIs in different forms, which local definitions duplicate canonical ones,
which newly extracted schemas should be promoted organisation-wide. The
registry health analysis and web explorer provide that visibility without
requiring manual review of every API.

There is no finish line. The goal is not to centralise everything by a given
date. It is to make centralisation the path of least resistance for new work,
and to gradually draw existing schemas toward the canonical layer over time.

!!! NOTE "Need help getting there?"

    [Enterprise](../commercial.md) customers can engage Sourcemeta for hands-on
    professional services: API discovery, spec generation, schema extraction,
    registry setup, and team onboarding. [Get in
    touch](mailto:hello@sourcemeta.com).
