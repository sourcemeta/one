# The Problem With API Governance Today

APIs are the connective tissue of modern software. Most organisations know
this. Most have also, at some point, decided to "govern their APIs."

And yet the same problems keep coming back.

## The symptoms are familiar

Pick any reasonably large engineering organisation and you tend to find the
same problems, regardless of how much has been invested in API tooling.

**Duplication.** The payments team has a `Customer` object. So does the
notifications service, the billing system, and the analytics pipeline. Each
definition made sense to the team that wrote it. None of them were being
careless. There was no shared definition to reach for, so they wrote their own.
Now you have five definitions of `Customer`, subtly inconsistent. An `address`
that is a string in one API and a nested object in another. `dateOfBirth` here,
`dob` there. `email` required in three specs, optional in two, and only
validated in one.

**Discoverability.** A sixth team arrives. They search for existing
definitions, find nothing organised or trustworthy, and write their own. The
problem does not just persist, it compounds. According to [Postman's 2025 State
of the API Report](https://www.postman.com/state-of-api/2025/), **34% of
developers cannot find the APIs they need within their own organisation**, and
**55% struggle with inconsistent documentation**. Teams are rebuilding work
that already exists because there is no reliable way to know what already
exists and to actually reuse it.

**Integration pain.** No two APIs in the organisation agree on how to represent
the same concept. Some of this is unavoidable as your business domain has
concepts unique to you, and without a shared internal definition every team
invents their own version independently. But a significant part is entirely
avoidable. Standards for many common concepts already exist: [ISO
8601](https://www.iso.org/iso-8601-date-and-time-format.html) for dates, [ISO
3166](https://www.iso.org/iso-3166-country-codes.html) for country codes, [ISO
4217](https://www.iso.org/iso-4217-currency-codes.html) for currencies, [ISO
20022](https://www.iso20022.org/) for financial data models used by SWIFT and
SEPA, and many more. For those concepts, the thinking has already been done and
your teams are spending effort reinventing it, most probably to lesser quality.
The consequence either way is the same: every integration boundary in your
system, internal or external, requires a translation layer that would not exist
if everyone had agreed on the same definitions to begin with.

## The data confirms it

This pattern is not a niche complaint. It became the norm.

[SmartBear's 2023 State of Software Quality: API
report](https://smartbear.com/state-of-software-quality/api/), surveying over
1,100 API practitioners across 17 industries, found that **API standardisation
is the top challenge cited by 51% of organisations**, ahead of security,
tooling, and performance. It has held the top spot in every edition of the
survey since 2016.

[Axway's 2024 State of Enterprise API Maturity
report](https://resources.axway.com/build-api-marketplace/rpt-state-of-enterprise-api-maturity-in-2024-en),
based on 600 senior IT and business decision-makers across nine countries,
found that **78% of enterprise leaders do not know how many APIs their
organisation has**, and **74% acknowledge that more than 20% of their APIs are
entirely unmanaged**. If organisations cannot account for their APIs, the
things that are at least visible as endpoints, as documentation pages, as
things that break in production, consider how much less visible the schema
layer inside those APIs must be. APIs surface in logs, in incidents, in partner
complaints. Schemas are embedded silently inside specs, never inventoried,
never compared across teams. The governance gap at the API level is actually
the optimistic number.

These are not symptoms of bad engineers or underinvestment in tooling. Most of
these organisations already have tooling. They are symptoms of a missing layer.

## Why this keeps happening? The industry standardised on OpenAPI and stopped one layer too high

These are not random failures. They have a common structural cause.

OpenAPI became the industry standard for describing APIs. That was genuinely
good. Teams started writing specs, tooling matured, design-first workflows
became real. An entire ecosystem of editors, linters, gateways, and
documentation portals emerged around the format.

But almost all of that tooling shares the same blind spot: it treats each
OpenAPI spec as the unit of work. It helps you design a better spec in
isolation, with consistent naming within it, well-structured responses, and
clear documentation. What it does not do is help you share anything *across*
specs. The OpenAPI spec is always the starting point, and it is never
decomposed further.

This is not a criticism of API spec first as a practice. Designing the contract
before writing code is genuinely better than the alternative. But API spec
first still takes the OpenAPI specification as its atom, which means the
schemas inside it remain ungoverned, uninventoried, and unshared across teams.
You can be entirely rigorous about your OpenAPI spec and still end up with five
definitions of `Customer`. The discipline is real. The level of abstraction is
wrong.

What makes this particularly hard to notice is that the tooling actively
reinforces the false confidence. A team runs their OpenAPI spec through a
linter. It passes. Naming conventions: consistent. Response codes: correct.
Required fields: present. But the linter checked the structure of the spec, not
the quality of the schemas inside it. It did not ask whether `Customer` was
well-modelled, whether it duplicated something three other teams had already
defined, or whether it diverged from an industry standard. The spec looks
clean. The schema layer is still a mess. Most OpenAPI linters treat schema
content as largely opaque, because the schema layer is not what they were
designed to govern. You can pass every lint rule and still have a fundamental
governance problem.

This matters because a non-trivial OpenAPI spec is, in terms of raw content,
mostly schemas. The endpoints, the HTTP verbs, the status codes: that is
structural boilerplate. The substance is the definitions. What a `Customer`
looks like. What an `Invoice` contains. What an `Address` requires. In any
real-world API, well over 80% of what matters lives in the schema layer.

And nobody talked about sharing that layer.

Teams write OpenAPI specs in isolation. Each spec defines its own schemas
inline, because that is what every tutorial shows and because there was nowhere
central to put them even if a team wanted to. The result: every OpenAPI spec is
an island. Individually excellent. Collectively incoherent.

## AI makes this more urgent, not less

[Gartner predicts that by 2026, more than 30% of the increase in API demand
will come from AI tools and large language
models](https://www.gartner.com/en/newsroom/press-releases/2024-03-20-gartner-predicts-more-than-30-percent-of-the-increase-in-demand-for-apis-will-come-from-ai-and-tools-using-llms-by-2026).
Separately, [40% of enterprise applications will be integrated with
task-specific AI agents by end of
2026](https://www.gartner.com/en/newsroom/press-releases/2025-08-26-gartner-predicts-40-percent-of-enterprise-apps-will-feature-task-specific-ai-agents-by-2026-up-from-less-than-5-percent-in-2025),
up from less than 5% today. AI agents consume interface definitions literally
and at scale. The quality of those definitions, and the documentation generated
out of them, has never mattered more.

But it is worth being precise about what "interface definition" means here.
Most engineers currently think of the OpenAPI spec as the interface. That is
understandable: it is what gets shared with consumers, what gets published to
documentation portals, what gets handed to a new developer on day one. *But
OpenAPI is a wrapper format*. It makes schemas usable in the context of APIs,
describing which schemas appear at which endpoints and under which HTTP
methods. *The actual interfaces, the data contracts themselves, are the schemas
underneath.*

An AI agent consuming a well-defined, rich shared schema behaves consistently
and predictably. An AI agent consuming five slightly different inline schemas
with loose descriptions and no metadata five slightly different
interpretations, all potentially incorrect.

## The solution is to treat schemas as their own layer

The problem is not that teams write bad APIs. It is that the schema layer, the
layer that defines what data *means* across your organisation, has never been
treated as infrastructure in its own right.

Every other layer has been addressed. You have source control for code. You
have registries for container images. You have package managers for
dependencies. *The schema layer, the shared vocabulary of your entire API
landscape, has been left as an afterthought embedded inside individual specs:
invisible and ungoverned.*

That is what this guide addresses. Not another linter for your OpenAPI specs. A
foundation for the layer beneath them.
