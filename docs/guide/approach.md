# The Schema-First Mental Model

On the previous page we described the problem: a schema layer that exists in
every organisation but is governed by nobody, invisible below the OpenAPI
surface, duplicated across teams, and never treated as shared infrastructure.
The solution is not a new process or a new tool. It is a change in how you
think about schemas.

Schemas are code. They define the structure and meaning of your data, determine
what is valid and what is not, and represent the contracts that every API in
your organisation either honours or violates. They deserve the same discipline
you apply to any other critical piece of code: version control, review, and a
single authoritative source.

## The architecture in a nutshell

- A single Git repository holds your organisation's canonical schemas
- A registry sits on top of that repository and exposes its contents over HTTP:
  searchable, browsable, and queryable by any tooling or pipeline that needs
  it
- Changes go through Git, the same pull request workflow your organisation
  already uses for everything else
- Consumers reference schemas either directly via
  [`$ref`](https://www.learnjsonschema.com/2020-12/core/ref/) pointing at the
  registry, or by running [`jsonschema
  install`](https://github.com/sourcemeta/jsonschema/blob/main/docs/install.markdown)
  to pull schemas into their own repository and reference them locally
- API teams keep their OpenAPI specs wherever makes sense for them, pointing
  their schema references at the central source

That is the entire model. Everything that follows is grounded on those steps.

## This model is already proven in practice

Maintaining a Git repository of schemas and serving it over HTTP is not a novel
proposal. Most organisations doing it are closed-source, but several public
examples are instructive.

[SchemaStore](https://github.com/SchemaStore/schemastore) is the most visible
instance: a Git repository of JSON Schemas for popular tools, maintained
through pull requests and served over HTTP. When your editor autocompletes a
GitHub Actions workflow or a Kubernetes manifest, it is reading from a schema
reviewed and merged through a PR there.

[KrakenD](https://github.com/krakend/krakend-schema) maintains its
configuration schemas the same way. So does [NASA's General Coordinates
Network](https://github.com/nasa-gcn/gcn-schema/tree/main/gcn) for astronomical
alert schemas, and the [Human Cell
Atlas](https://github.com/HumanCellAtlas/metadata-schema/tree/master/json_schema)
for biological metadata.

Ikenna Nwaiwu's [Automating API Delivery: APIOps with
OpenAPI](https://www.amazon.com/Automating-API-Delivery-APIOps-OpenAPI/dp/1633438783)
(Manning, 2024) formalises the same approach for API definitions. This guide
extends that thinking one layer deeper, to the schema layer those definitions
depend on.

Enterprise organisations are already adopting this exact workflow for OpenAPI
specifications.
[Afosto](https://www.krakend.io/case-study/afosto/) maintains OpenAPI
definitions in per-service Git repositories, "committed to a central repository
during the CI process," which "triggers a new gateway build."
[Medibank](https://www.postman.com/customer-stories/medibank/) adopted
contract-first development where "API endpoints, data structures,
request/response formats" are defined and reviewed before implementation.
[Paymenttools](https://tyk.io/case-studies/paymenttools/) routes every API
change through Git, where it gets "automatically validated" against "PCI and
ISO compliance" rules. These organisations have proven the GitOps model works
for API specifications. Extending it one layer deeper, to the schemas inside
those specifications, is the natural next step.

Fran Méndez, creator of [AsyncAPI](https://www.asyncapi.com/), arrives at the
same architecture from the event-driven side in [Shift: The Playbook for
Event-Driven Architecture Advocacy](https://leadtheshift.co) (2026). The
chapter _Strategy 12, "Implementing Incremental Schema Governance,"_ prescribes
a central Git repository of contracts, CI-enforced validation, federated review
via pull requests, and a human-friendly event catalog on top. The pattern is
the same whether the contracts are JSON Schemas powering REST APIs or AsyncAPI
files powering event streams.

In [Crafting Great APIs with Domain-Driven
Design](https://www.amazon.com/Crafting-Great-APIs-Domain-Driven-Design/dp/B0DYNMWP67)
(Apress, 2025), Annegret Junker and Fabrizio Lazzaretti also advocate for
APIOps as a practice that brings API design, implementation, and operation
closer to each other, doing for APIs what DevOps does for code. At its core,
the process is rooted in storing API definitions and
their declarative configurations in a central version-controlled repository, and
reconciling the desired state of the API against it.

## Why Git? Governance by construction

*This approach is a direct application of [GitOps](https://opengitops.dev/):
using Git as the single source of truth for system state, where every change
goes through a version-controlled, reviewable workflow.*

In comparison, a stateful registry like Apicurio or Confluent accepts writes
directly to itself over its API. This means that schemas can enter your
canonical layer outside any review workflow. Some solutions bolt approval flows
on top, but in doing so they are rebuilding what Git already provides natively:
audit history, access controls, branch-based proposals, CI integration. They
are rebuilding those capabilities with less maturity, less flexibility, and
less familiarity than infrastructure your engineers have used for years.

!!! NOTE

    It is worth noting that stateful registries are the right tool for certain
    problems. In event streaming architectures, sharing a schema out-of-band
    for deserialisation is a coordination problem, and a stateful push model
    fits. API governance is a different problem.

With a Git-native approach, the registry has no write path. The only way to
change what it serves is to change what is in Git, which means going through a
pull request: proposable, reviewable, reversible, audited by default.
Governance is not a policy people follow. It is a structural property of the
system.

Git's extensibility compounds this advantage. You can attach anything to the
workflow: AI reviewers, webhooks to notify downstream systems, automated
duplicate detection, Slack notifications. The entire CI ecosystem the industry
has built around Git works here without additional integration. No schema
registry plugin system required.

## Why a registry on top of Git, and not just Git

A raw file tree is not enough for everyone who needs to interact with the
schema layer. A product manager should not need a terminal and a git client to
understand what a `Transaction` schema contains. A governance team needs a
health view across hundreds of definitions, not file diffs. A pipeline
resolving a [`$ref`](https://www.learnjsonschema.com/2020-12/core/ref/) will
prefer a stable HTTP endpoint, not a repository clone. A compliance stakeholder
needs rendered documentation, not raw JSON.

A read-only registry, like Sourcemeta One, turns the source of truth into
something the whole organisation can use, without changing what the source of
truth is. The registry is the interface. Git is the authority.

Because the registry holds no state, its operational profile is fundamentally
simpler: no database, no sync process, no stateful service. A stateless
registry is a single container, trivial to deploy and scale horizontally.
Reliability increases precisely because the most common source of failure in
distributed systems, stateful persistence, has been removed.

## `$ref` versus `jsonschema install`

Referencing schemas via
[`$ref`](https://www.learnjsonschema.com/2020-12/core/ref/) is convenient for
tooling, OpenAPI authoring, and local development. The tradeoff is a runtime
dependency on the registry being available, which is fine for most internal
workflows but a risk for production systems or airgapped environments.

The [`jsonschema
install`](https://github.com/sourcemeta/jsonschema/blob/main/docs/install.markdown)
command fetches schemas with integrity verification and writes them to disk,
where they can be committed and used with no network dependency. The pattern is
identical to npm: you depend on the local copy, not the registry being live.
Many teams use [`$ref`](https://www.learnjsonschema.com/2020-12/core/ref/)
during development and [`jsonschema
install`](https://github.com/sourcemeta/jsonschema/blob/main/docs/install.markdown)
for production builds and CI.

## One source of truth, but never a blocker

A source of truth requires one place, because two authoritative repositories
means teams must choose, and that choice is where inconsistency begins. But the
path there runs through local iteration, not around it.

A common fear about centralised governance is that it becomes a bottleneck.
That is a misunderstanding of what this model is trying to achieve.

The goal is not to gate development behind a registry. It is to centralise and
make official what teams are already building, in parallel, without ever
blocking them. Local schemas are a staging area. If the registry does not have
what you need, define it locally, ship, and keep building. Upstream proposals
happen as a separate concern.

## What the flow looks like

A developer on the payments team needs a `PaymentMethod` definition. The
registry does not have one. They define it locally, reference it from their
OpenAPI spec, and ship. Development is not blocked.

When the PR lands, the platform team notices the `currency` field uses a
freeform string where [ISO
4217](https://www.iso.org/iso-4217-currency-codes.html) already defines a
controlled vocabulary, and suggests internally referencing it. The schema
becomes stronger through review. The PR merges, triggering a CI action that
redeploys Sourcemeta One. On startup the registry reads from the updated
repository and `PaymentMethod` is now available canonically.

The payments team replaces their local definition with a
[`$ref`](https://www.learnjsonschema.com/2020-12/core/ref/) to the registry.
The next team finds it through the registry and skips defining their own. A
third team runs [`jsonschema
install`](https://github.com/sourcemeta/jsonschema/blob/main/docs/install.markdown)
to pull it locally. The local schema served its purpose and is gone.

!!! NOTE

    In some organisations the developer then proposes it upstream directly. In
    larger organisations, an API platform team continuously monitors what
    schemas are being defined locally across teams, identifies patterns worth
    standardising, and drives centralisation over time, replacing local
    definitions with canonical ones as adoption spreads. Either way, the schema
    moves upstream without blocking anyone.

The fragmentation cycle is always governed and prevented.
