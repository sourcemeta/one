# Using Custom Meta-Schemas

JSON Schema is extensible at its core. A meta-schema is a JSON Schema that
describes what a valid schema looks like. Every keyword you know and use is
defined as part of a JSON Schema meta-schema. For example, see the [2020-12
validation meta-schema](https://json-schema.org/draft/2020-12/meta/validation),
which defines common keywords like
[`type`](https://www.learnjsonschema.com/2020-12/validation/type/) and
[`maxLength`](https://www.learnjsonschema.com/2020-12/validation/maxlength/).

In other words, the syntax of schemas is defined using JSON Schema itself. And
you can use this same mechanism to:

- **Define your own custom keywords**, typically to define the syntax of your
  own annotation keywords, such as ownership, lineage, data sensitivity, or
  lifecycle metadata, instead of letting them be free form and ungoverned

- **Define modelling guidelines**, such as enforcing the presence of certain
  keywords, rejecting the parts of JSON Schema you do not want your
  schema-authors to use, or prohibiting specific usage patterns

Sourcemeta One already ships with a JSON Schema linter, and the [Enterprise
edition](../commercial.md) lets you define [custom linter
rules](../configuration.md#linter) of your own. However, linting is
deliberately soft. The instance still ingests offending schemas and surfaces
the problems through its health scores.

A custom meta-schema is the hard counterpart, analogous to a syntax error.
**Any schema that does not match its meta-schema is rejected at indexing time
and never makes it to the registry**. Many organisations rely on this when a
modelling guideline is a deal breaker that cannot be violated at all.

!!! note

    Nothing about this approach is vendor-specific. While other products
    enforce modelling guidelines through proprietary mechanisms, meta-schemas
    are a core feature of the JSON Schema specification. There is no lock-in
    involved. The custom meta-schemas you will write on this page carry over
    to any compliant JSON Schema implementation, within Sourcemeta One or far
    beyond it.

## Overview

In this tutorial, we will, step by step:

- Serve a custom meta-schema that starts as a faithful copy of the official
  [JSON Schema 2020-12](https://www.learnjsonschema.com/2020-12/) dialect
- Validate a collection of schemas against it
- Prohibit the [`if`](https://www.learnjsonschema.com/2020-12/applicator/if/),
  [`then`](https://www.learnjsonschema.com/2020-12/applicator/then/), and
  [`else`](https://www.learnjsonschema.com/2020-12/applicator/else/) keywords
- Force object types to declare
  [`additionalProperties`](https://www.learnjsonschema.com/2020-12/applicator/additionalproperties/)
- Introduce a well-defined `x-author` keyword and require it at the top level
  of every schema
- Reject any keyword that our dialect does not define
- Remove an official vocabulary from the dialect
- Unit test the meta-schema to make sure we got all of the above right

All you need is [Docker](https://www.docker.com), plus the [Sourcemeta JSON
Schema CLI](https://github.com/sourcemeta/jsonschema) for the final step.

## Step 1: Serving a Custom Meta-Schema

First, some terminology. In JSON Schema, a *vocabulary* is a collection of
keywords, and a *dialect* is a collection of vocabularies. The official
[2020-12](https://www.learnjsonschema.com/2020-12/) dialect consists of seven
vocabularies: [core](https://www.learnjsonschema.com/2020-12/core/),
[applicator](https://www.learnjsonschema.com/2020-12/applicator/),
[unevaluated](https://www.learnjsonschema.com/2020-12/unevaluated/),
[validation](https://www.learnjsonschema.com/2020-12/validation/),
[meta-data](https://www.learnjsonschema.com/2020-12/meta-data/),
[format-annotation](https://www.learnjsonschema.com/2020-12/format-annotation/),
and [content](https://www.learnjsonschema.com/2020-12/content/).

A meta-schema is a schema, so the instance serves it like any other. Let's
create the entry point of our custom dialect. For now, it merely reassembles
[JSON Schema 2020-12](https://www.learnjsonschema.com/2020-12/), and there are
three parts to it. The
[`$vocabulary`](https://www.learnjsonschema.com/2020-12/core/vocabulary/)
keyword declares which vocabularies our dialect uses, but on its own it does
not enforce any syntax. The syntax comes from the
[`$ref`](https://www.learnjsonschema.com/2020-12/core/ref/) references to the
official meta-schema of each vocabulary, combined through the
[`allOf`](https://www.learnjsonschema.com/2020-12/applicator/allof/)
applicator. Finally, the
[`$dynamicAnchor`](https://www.learnjsonschema.com/2020-12/core/dynamicanchor/)
keyword: the official vocabulary meta-schemas descend into nested subschemas
through a [dynamic
reference](https://www.learnjsonschema.com/2020-12/core/dynamicref/) to an
anchor named `meta`, and the official dialect declares that anchor at its top
level, so that the entire dialect, and not just an individual vocabulary,
re-applies to every nested subschema. Our copy must do the same:

```json title="schemas/meta/custom.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$vocabulary": {
    "https://json-schema.org/draft/2020-12/vocab/core": true,
    "https://json-schema.org/draft/2020-12/vocab/applicator": true,
    "https://json-schema.org/draft/2020-12/vocab/unevaluated": true,
    "https://json-schema.org/draft/2020-12/vocab/validation": true,
    "https://json-schema.org/draft/2020-12/vocab/meta-data": true,
    "https://json-schema.org/draft/2020-12/vocab/format-annotation": true,
    "https://json-schema.org/draft/2020-12/vocab/content": true
  },
  "$dynamicAnchor": "meta",
  "title": "Custom Meta-Schema",
  "description": "An extension of JSON Schema 2020-12 with custom keywords and rules",
  "allOf": [
    { "$ref": "https://json-schema.org/draft/2020-12/meta/core" },
    { "$ref": "https://json-schema.org/draft/2020-12/meta/applicator" },
    { "$ref": "https://json-schema.org/draft/2020-12/meta/unevaluated" },
    { "$ref": "https://json-schema.org/draft/2020-12/meta/validation" },
    { "$ref": "https://json-schema.org/draft/2020-12/meta/meta-data" },
    { "$ref": "https://json-schema.org/draft/2020-12/meta/format-annotation" },
    { "$ref": "https://json-schema.org/draft/2020-12/meta/content" }
  ]
}
```

!!! info

    Dynamic references are one of the most advanced corners of JSON Schema.
    You do not need to fully master them to follow along, but if you want to
    dig deeper, refer to
    [`$dynamicRef`](https://www.learnjsonschema.com/2020-12/core/dynamicref/)
    and
    [`$dynamicAnchor`](https://www.learnjsonschema.com/2020-12/core/dynamicanchor/)
    on [Learn JSON Schema](https://www.learnjsonschema.com).

Next, a [configuration file](../configuration.md) that serves our instance at
`http://localhost:8000` and ingests the meta-schema directory as a collection
called `meta`:

```json title="one.json"
{
  "url": "http://localhost:8000",
  "html": {
    "name": "My Registry"
  },
  "contents": {
    "meta": {
      "title": "Meta",
      "description": "A collection of custom meta-schemas",
      "path": "./schemas/meta"
    }
  }
}
```

The `meta` collection name is arbitrary. Call it `metaschemas` or anything
else you like, or skip the dedicated collection entirely and keep meta-schemas
mixed with your other schemas. We do recommend a separate collection though,
as it makes their purpose clear.

Finally, a `Dockerfile` that extends the official base image and indexes the
schemas at build time. Replace `<version>` with the latest release from
[GitHub Packages](https://github.com/orgs/sourcemeta/packages?repo_name=one),
for example `6.4.0`:

```docker title="Dockerfile"
FROM ghcr.io/sourcemeta/one:<version>
COPY one.json .
COPY schemas schemas
RUN sourcemeta one.json
```

Build the image and run it:

```sh
$ docker build --tag my-registry . --file Dockerfile
$ docker run --publish 8000:8000 my-registry
```

You can now browse the instance at
[http://localhost:8000](http://localhost:8000) and fetch the meta-schema at
[http://localhost:8000/meta/custom.json](http://localhost:8000/meta/custom.json).
Note that the instance content-negotiates, so to see the HTML pages from the
terminal rather than a browser, pass the corresponding `Accept` header:

```sh
$ curl --header 'Accept: text/html' http://localhost:8000
```

Keep an eye on the `RUN sourcemeta one.json` step of the build. That is where
indexing happens, and where our meta-schema will start rejecting schemas later
on. We will rebuild and re-run the instance many times throughout this
tutorial, so remember to stop the running container (with Ctrl-C or
`docker stop`) before starting it again, otherwise the port will still be
taken. Your project should look like this so far:

```
├── Dockerfile
├── one.json
└── schemas
    └── meta
        └── custom.json
```

## Step 2: Validating Schemas Against It

Now let's create a collection of schemas governed by our new dialect. Here is
a person schema that uses a conditional to require US-style postal codes (we
will come back to that conditional in the next step). Note that it does not
declare the
[`$schema`](https://www.learnjsonschema.com/2020-12/core/schema/) keyword:

```json title="schemas/example/person.json"
{
  "title": "Person",
  "description": "A simple schema for describing a person",
  "examples": [
    {
      "name": "John Doe",
      "country": "United States of America",
      "postalCode": "10001"
    }
  ],
  "type": "object",
  "required": [ "name" ],
  "properties": {
    "name": {
      "type": "string"
    },
    "country": {
      "type": "string"
    },
    "postalCode": {
      "type": "string"
    }
  },
  "if": {
    "properties": {
      "country": {
        "const": "United States of America"
      }
    }
  },
  "then": {
    "properties": {
      "postalCode": {
        "pattern": "^[0-9]{5}(-[0-9]{4})?$"
      }
    }
  }
}
```

!!! note

    JSON Schema requires
    [`$schema`](https://www.learnjsonschema.com/2020-12/core/schema/) to be an
    absolute URI, which makes pointing
    at custom meta-schemas from within schema files a bit trickier: schemas
    would have to declare some absolute URI for it, and the instance must be
    taught how to route that URI back to your meta-schema using the
    [`resolve`](../configuration.md#resolve) property. That works, but
    omitting
    [`$schema`](https://www.learnjsonschema.com/2020-12/core/schema/) and
    declaring the dialect at the configuration level, like we do here, is
    simpler, so it is what we recommend.

Register it as an `example` collection whose
[`defaultDialect`](../configuration.md#collections) points at our meta-schema.
This property assigns a dialect to every schema in the collection that does
not declare
[`$schema`](https://www.learnjsonschema.com/2020-12/core/schema/). It takes a
URI reference that is resolved against the
URL the collection is served from, so `../meta/custom` from
`http://localhost:8000/example/` lands on `http://localhost:8000/meta/custom`:

```json title="one.json" hl_lines="12-17"
{
  "url": "http://localhost:8000",
  "html": {
    "name": "My Registry"
  },
  "contents": {
    "meta": {
      "title": "Meta",
      "description": "A collection of custom meta-schemas",
      "path": "./schemas/meta"
    },
    "example": {
      "title": "Example Schemas",
      "description": "Schemas that must conform to our custom meta-schema",
      "path": "./schemas/example",
      "defaultDialect": "../meta/custom"
    }
  }
}
```

Rebuild the image, run it again, and fetch the person schema. It is now
stamped with our custom dialect:

```sh
$ curl http://localhost:8000/example/person.json
```

```json hl_lines="2"
{
  "$schema": "http://localhost:8000/meta/custom",
  "$id": "http://localhost:8000/example/person",
  "title": "Person",
  "description": "A simple schema for describing a person",
  "examples": [
    {
      "name": "John Doe",
      "country": "United States of America",
      "postalCode": "10001"
    }
  ],
  "type": "object",
  "if": {
    "properties": {
      "country": {
        "const": "United States of America"
      }
    }
  },
  "then": {
    "properties": {
      "postalCode": {
        "pattern": "^[0-9]{5}(-[0-9]{4})?$"
      }
    }
  },
  "required": [ "name" ],
  "properties": {
    "name": {
      "type": "string"
    },
    "country": {
      "type": "string"
    },
    "postalCode": {
      "type": "string"
    }
  }
}
```

That is all the boilerplate. At this point, our dialect accepts exactly what
the official [2020-12](https://www.learnjsonschema.com/2020-12/) dialect
accepts, nothing more and nothing less. Time to
change that. Your project should now look like this:

```
├── Dockerfile
├── one.json
└── schemas
    ├── example
    │   └── person.json
    └── meta
        └── custom.json
```

## Step 3: Prohibiting Keywords

Suppose our organisation considers
[`if`](https://www.learnjsonschema.com/2020-12/applicator/if/),
[`then`](https://www.learnjsonschema.com/2020-12/applicator/then/), and
[`else`](https://www.learnjsonschema.com/2020-12/applicator/else/) too
confusing to allow. To forbid a keyword, declare it as the `false` schema, the
impossible schema that nothing validates against. We recommend keeping every
rule in its own file from the start, as meta-schemas otherwise tend to grow
huge. Any simple convention works. We will store rule schemas in a `rules`
directory, where the `circular` subdirectory holds the rules that apply to any
part of a schema (the name will make sense in a moment):

```json title="schemas/meta/rules/circular/no-conditionals.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$vocabulary": {
    "https://json-schema.org/draft/2020-12/vocab/core": true
  },
  "title": "No Conditionals Rule",
  "description": "You must not use the if, then, and else applicator keywords",
  "properties": {
    "if": false,
    "then": false,
    "else": false
  }
}
```

We want this rule to apply at every nesting level. An
[`if`](https://www.learnjsonschema.com/2020-12/applicator/if/) buried inside
[`properties`](https://www.learnjsonschema.com/2020-12/applicator/properties/)
is just as forbidden as a top level one. Remember the `meta` dynamic anchor
from step 1? Because our meta-schema declares it, everything in its
[`allOf`](https://www.learnjsonschema.com/2020-12/applicator/allof/)
re-applies to every nested subschema, so all it takes is hooking the rule in.
Note that the reference to the rule is a plain relative path, which the
instance resolves and rewrites at indexing time:

```json title="schemas/meta/custom.json" hl_lines="23"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$vocabulary": {
    "https://json-schema.org/draft/2020-12/vocab/core": true,
    "https://json-schema.org/draft/2020-12/vocab/applicator": true,
    "https://json-schema.org/draft/2020-12/vocab/unevaluated": true,
    "https://json-schema.org/draft/2020-12/vocab/validation": true,
    "https://json-schema.org/draft/2020-12/vocab/meta-data": true,
    "https://json-schema.org/draft/2020-12/vocab/format-annotation": true,
    "https://json-schema.org/draft/2020-12/vocab/content": true
  },
  "$dynamicAnchor": "meta",
  "title": "Custom Meta-Schema",
  "description": "An extension of JSON Schema 2020-12 with custom keywords and rules",
  "allOf": [
    { "$ref": "https://json-schema.org/draft/2020-12/meta/core" },
    { "$ref": "https://json-schema.org/draft/2020-12/meta/applicator" },
    { "$ref": "https://json-schema.org/draft/2020-12/meta/unevaluated" },
    { "$ref": "https://json-schema.org/draft/2020-12/meta/validation" },
    { "$ref": "https://json-schema.org/draft/2020-12/meta/meta-data" },
    { "$ref": "https://json-schema.org/draft/2020-12/meta/format-annotation" },
    { "$ref": "https://json-schema.org/draft/2020-12/meta/content" },
    { "$ref": "rules/circular/no-conditionals.json" }
  ]
}
```

Remember the conditional in `person.json`? Rebuild the image and indexing now
refuses it:

```text hl_lines="4"
$ docker build --tag my-registry . --file Dockerfile
...
error: The schema does not adhere to its metaschema
The object value was expected to validate against the defined properties subschemas
  at instance location ""
  at evaluate path "/allOf/7/$ref/properties"
The object value was expected to validate against the referenced schema
  at instance location ""
  at evaluate path "/allOf/7/$ref"
The object value was expected to validate against the 8 given subschemas
  at instance location ""
  at evaluate path "/allOf"
```

The instance location is the offending spot in the schema, here the root, and
the evaluate path walks through the meta-schema: `/allOf/7/$ref` is the eighth
entry of our
[`allOf`](https://www.learnjsonschema.com/2020-12/applicator/allof/), the
`no-conditionals` rule. Remove the
[`if`](https://www.learnjsonschema.com/2020-12/applicator/if/) and
[`then`](https://www.learnjsonschema.com/2020-12/applicator/then/) keywords
from `person.json` and the build succeeds again. Your project should now look
like this:

```
├── Dockerfile
├── one.json
└── schemas
    ├── example
    │   └── person.json
    └── meta
        ├── custom.json
        └── rules
            └── circular
                └── no-conditionals.json
```

## Step 4: Enforcing Usage Patterns

Many people find it confusing that JSON Schema accepts object properties not
listed in
[`properties`](https://www.learnjsonschema.com/2020-12/applicator/properties/)
by default. If your organisation feels that way, a meta-schema rule can force
every schema to take an explicit stance. Rules can be more elaborate than
banning keywords: this one requires
[`additionalProperties`](https://www.learnjsonschema.com/2020-12/applicator/additionalproperties/)
whenever [`type`](https://www.learnjsonschema.com/2020-12/validation/type/) is
or contains `object`:

```json title="schemas/meta/rules/circular/object-additional-properties.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$vocabulary": {
    "https://json-schema.org/draft/2020-12/vocab/core": true
  },
  "title": "Object Additional Properties Rule",
  "description": "You must declare additionalProperties when type is or contains object",
  "if": {
    "required": [ "type" ],
    "properties": {
      "type": {
        "anyOf": [
          { "const": "object" },
          { "type": "array", "contains": { "const": "object" } }
        ]
      }
    }
  },
  "then": {
    "required": [ "additionalProperties" ],
    "properties": {
      "additionalProperties": true
    }
  }
}
```

Note that the rule itself uses
[`if`](https://www.learnjsonschema.com/2020-12/applicator/if/) and
[`then`](https://www.learnjsonschema.com/2020-12/applicator/then/). Rule files
are written in the official
[2020-12](https://www.learnjsonschema.com/2020-12/) dialect, where
conditionals are perfectly fine. Our restrictions only apply to schemas written in our custom dialect.
Hook the rule into the meta-schema:

```json title="schemas/meta/custom.json" hl_lines="23"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$vocabulary": {
    "https://json-schema.org/draft/2020-12/vocab/core": true,
    "https://json-schema.org/draft/2020-12/vocab/applicator": true,
    "https://json-schema.org/draft/2020-12/vocab/unevaluated": true,
    "https://json-schema.org/draft/2020-12/vocab/validation": true,
    "https://json-schema.org/draft/2020-12/vocab/meta-data": true,
    "https://json-schema.org/draft/2020-12/vocab/format-annotation": true,
    "https://json-schema.org/draft/2020-12/vocab/content": true
  },
  "$dynamicAnchor": "meta",
  "title": "Custom Meta-Schema",
  "description": "An extension of JSON Schema 2020-12 with custom keywords and rules",
  "allOf": [
    { "$ref": "https://json-schema.org/draft/2020-12/meta/core" },
    { "$ref": "https://json-schema.org/draft/2020-12/meta/applicator" },
    { "$ref": "https://json-schema.org/draft/2020-12/meta/unevaluated" },
    { "$ref": "https://json-schema.org/draft/2020-12/meta/validation" },
    { "$ref": "https://json-schema.org/draft/2020-12/meta/meta-data" },
    { "$ref": "https://json-schema.org/draft/2020-12/meta/format-annotation" },
    { "$ref": "https://json-schema.org/draft/2020-12/meta/content" },
    { "$ref": "rules/circular/object-additional-properties.json" },
    { "$ref": "rules/circular/no-conditionals.json" }
  ]
}
```

Our person schema declares an `object`
[`type`](https://www.learnjsonschema.com/2020-12/validation/type/) but no
[`additionalProperties`](https://www.learnjsonschema.com/2020-12/applicator/additionalproperties/),
so rebuilding fails once more:

```text hl_lines="4"
$ docker build --tag my-registry . --file Dockerfile
...
error: The schema does not adhere to its metaschema
The object value was expected to define the property "additionalProperties"
  at instance location ""
  at evaluate path "/allOf/7/$ref/then/required"
The object value was expected to validate against the referenced schema
  at instance location ""
  at evaluate path "/allOf/7/$ref"
The object value was expected to validate against the 9 given subschemas
  at instance location ""
  at evaluate path "/allOf"
```

Add `"additionalProperties": false` at the bottom of `person.json` to
comply.
Your project should now look like this:

```
├── Dockerfile
├── one.json
└── schemas
    ├── example
    │   └── person.json
    └── meta
        ├── custom.json
        └── rules
            └── circular
                ├── no-conditionals.json
                └── object-additional-properties.json
```

## Step 5: Defining Custom Keywords

Schemas often carry custom metadata, and JSON Schema permits unknown keywords
by default, so nothing stops you from annotating schemas with an `x-author`
keyword right now. But nothing defines it either: `"x-author": 1` would be
just as acceptable as a proper name. With a meta-schema, we can pin its syntax
down to a string. This is exactly how JSON Schema defines the standard
keywords too: remember the [2020-12 validation
meta-schema](https://json-schema.org/draft/2020-12/meta/validation) from the
top of this page, which declares
[`type`](https://www.learnjsonschema.com/2020-12/validation/type/) and
[`maxLength`](https://www.learnjsonschema.com/2020-12/validation/maxlength/)
the same way we are about to declare `x-author`. We will keep custom keyword definitions in a
`keywords` directory, separate from the rules:

```json title="schemas/meta/keywords/x-author.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$vocabulary": {
    "https://json-schema.org/draft/2020-12/vocab/core": true
  },
  "title": "Author Keyword",
  "description": "Keyword for attributing schemas to their authors",
  "properties": {
    "x-author": {
      "description": "The author of the schema",
      "type": "string"
    }
  }
}
```

Hook it into the meta-schema, so the keyword is available at every nesting
level:

```json title="schemas/meta/custom.json" hl_lines="23"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$vocabulary": {
    "https://json-schema.org/draft/2020-12/vocab/core": true,
    "https://json-schema.org/draft/2020-12/vocab/applicator": true,
    "https://json-schema.org/draft/2020-12/vocab/unevaluated": true,
    "https://json-schema.org/draft/2020-12/vocab/validation": true,
    "https://json-schema.org/draft/2020-12/vocab/meta-data": true,
    "https://json-schema.org/draft/2020-12/vocab/format-annotation": true,
    "https://json-schema.org/draft/2020-12/vocab/content": true
  },
  "$dynamicAnchor": "meta",
  "title": "Custom Meta-Schema",
  "description": "An extension of JSON Schema 2020-12 with custom keywords and rules",
  "allOf": [
    { "$ref": "https://json-schema.org/draft/2020-12/meta/core" },
    { "$ref": "https://json-schema.org/draft/2020-12/meta/applicator" },
    { "$ref": "https://json-schema.org/draft/2020-12/meta/unevaluated" },
    { "$ref": "https://json-schema.org/draft/2020-12/meta/validation" },
    { "$ref": "https://json-schema.org/draft/2020-12/meta/meta-data" },
    { "$ref": "https://json-schema.org/draft/2020-12/meta/format-annotation" },
    { "$ref": "https://json-schema.org/draft/2020-12/meta/content" },
    { "$ref": "keywords/x-author.json" },
    { "$ref": "rules/circular/object-additional-properties.json" },
    { "$ref": "rules/circular/no-conditionals.json" }
  ]
}
```

To prove the syntax is enforced, declare `"x-author": 1` at the top of
`person.json` and rebuild:

```text hl_lines="4"
$ docker build --tag my-registry . --file Dockerfile
...
error: The schema does not adhere to its metaschema
The value was expected to be of type string but it was of type integer
  at instance location "/x-author"
  at evaluate path "/allOf/7/$ref/properties/x-author/type"
The object value was expected to validate against the single defined property subschema
  at instance location ""
  at evaluate path "/allOf/7/$ref/properties"
The object value was expected to validate against the referenced schema
  at instance location ""
  at evaluate path "/allOf/7/$ref"
The object value was expected to validate against the 10 given subschemas
  at instance location ""
  at evaluate path "/allOf"
```

Declare it as a string instead and the build passes:

```json title="schemas/example/person.json" hl_lines="2"
{
  "x-author": "Jane Doe",
  "title": "Person",
  "description": "A simple schema for describing a person",
  "examples": [
    {
      "name": "John Doe",
      "country": "United States of America",
      "postalCode": "10001"
    }
  ],
  "type": "object",
  "required": [ "name" ],
  "properties": {
    "name": {
      "type": "string"
    },
    "country": {
      "type": "string"
    },
    "postalCode": {
      "type": "string"
    }
  },
  "additionalProperties": false
}
```

Your project should now look like this:

```
├── Dockerfile
├── one.json
└── schemas
    ├── example
    │   └── person.json
    └── meta
        ├── custom.json
        ├── keywords
        │   └── x-author.json
        └── rules
            └── circular
                ├── no-conditionals.json
                └── object-additional-properties.json
```

## Step 6: Requiring Keywords at the Top Level

There is a catch: `x-author` is still optional. Delete it from `person.json`
and the build happily passes. We want every schema to declare its author at
the top level. Similar to `rules/circular`, we will keep rules that apply to
the top level of schemas in a `rules/top` directory:

```json title="schemas/meta/rules/top/x-author-required.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$vocabulary": {
    "https://json-schema.org/draft/2020-12/vocab/core": true
  },
  "title": "Author Required Rule",
  "description": "You must declare the x-author keyword at the top level",
  "required": [ "x-author" ],
  "properties": {
    "x-author": true
  }
}
```

Where do we hook it? Not where the previous rules went. Because of the dynamic
anchor, everything in the
[`allOf`](https://www.learnjsonschema.com/2020-12/applicator/allof/) of
`custom.json` re-applies to every nested subschema. Try it: put `x-author` back at the top of `person.json`, append
`{ "$ref": "rules/top/x-author-required.json" }` to that
[`allOf`](https://www.learnjsonschema.com/2020-12/applicator/allof/), and
rebuild:

```text hl_lines="4-5"
$ docker build --tag my-registry . --file Dockerfile
...
error: The schema does not adhere to its metaschema
The object value was expected to define the property "x-author"
  at instance location "/properties/name"
  at evaluate path "/allOf/1/$ref/properties/properties/additionalProperties/$dynamicRef/allOf/10/$ref/required"
...
```

The instance location says it all: the little subschema at `/properties/name`
is now expected to declare an author of its own, and the
[`$dynamicRef`](https://www.learnjsonschema.com/2020-12/core/dynamicref/) in
the evaluate path shows the recursion at work. That is too much. The fix is to
split the meta-schema in two: a *circular* schema that carries the dynamic
anchor and everything that must re-apply recursively (hence the directory name
we picked in step 3), and the entry point, which keeps the
[`$vocabulary`](https://www.learnjsonschema.com/2020-12/core/vocabulary/)
declaration and applies the circular schema plus any top level rules, exactly
once:

```json title="schemas/meta/circular.json" hl_lines="3 14-16"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$dynamicAnchor": "meta",
  "title": "Circular Schema",
  "description": "The recursive definition that applies to every nested subschema",
  "allOf": [
    { "$ref": "https://json-schema.org/draft/2020-12/meta/core" },
    { "$ref": "https://json-schema.org/draft/2020-12/meta/applicator" },
    { "$ref": "https://json-schema.org/draft/2020-12/meta/unevaluated" },
    { "$ref": "https://json-schema.org/draft/2020-12/meta/validation" },
    { "$ref": "https://json-schema.org/draft/2020-12/meta/meta-data" },
    { "$ref": "https://json-schema.org/draft/2020-12/meta/format-annotation" },
    { "$ref": "https://json-schema.org/draft/2020-12/meta/content" },
    { "$ref": "keywords/x-author.json" },
    { "$ref": "rules/circular/object-additional-properties.json" },
    { "$ref": "rules/circular/no-conditionals.json" }
  ]
}
```

```json title="schemas/meta/custom.json" hl_lines="15-16"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$vocabulary": {
    "https://json-schema.org/draft/2020-12/vocab/core": true,
    "https://json-schema.org/draft/2020-12/vocab/applicator": true,
    "https://json-schema.org/draft/2020-12/vocab/unevaluated": true,
    "https://json-schema.org/draft/2020-12/vocab/validation": true,
    "https://json-schema.org/draft/2020-12/vocab/meta-data": true,
    "https://json-schema.org/draft/2020-12/vocab/format-annotation": true,
    "https://json-schema.org/draft/2020-12/vocab/content": true
  },
  "title": "Custom Meta-Schema",
  "description": "An extension of JSON Schema 2020-12 with custom keywords and rules",
  "allOf": [
    { "$ref": "circular.json" },
    { "$ref": "rules/top/x-author-required.json" }
  ]
}
```

Note the dynamic anchor moved to the circular schema. References there
re-apply everywhere, while references in the entry point run only once,
against the schema as a whole. Rebuild and the build passes: `person.json`
declares its author at the top, and none of its nested subschemas need to.
Delete the `x-author` line to see the rule kick in:

```text hl_lines="4"
$ docker build --tag my-registry . --file Dockerfile
...
error: The schema does not adhere to its metaschema
The object value was expected to define the property "x-author"
  at instance location ""
  at evaluate path "/allOf/1/$ref/required"
The object value was expected to validate against the referenced schema
  at instance location ""
  at evaluate path "/allOf/1/$ref"
The object value was expected to validate against the 2 given subschemas
  at instance location ""
  at evaluate path "/allOf"
```

Put it back before moving on. Your project should now look like this:

```
├── Dockerfile
├── one.json
└── schemas
    ├── example
    │   └── person.json
    └── meta
        ├── circular.json
        ├── custom.json
        ├── keywords
        │   └── x-author.json
        └── rules
            ├── circular
            │   ├── no-conditionals.json
            │   └── object-additional-properties.json
            └── top
                └── x-author-required.json
```

## Step 7: Closing the Dialect

One gap remains. Our dialect defines `x-author`, but any keyword it does not
define is silently ignored, so a typo like `x-autor` or an improvised
annotation would sail through. To close the dialect, end the circular schema
with
[`unevaluatedProperties`](https://www.learnjsonschema.com/2020-12/unevaluated/unevaluatedproperties/),
which rejects every property that none of the referenced meta-schemas
evaluated:

```json title="schemas/meta/circular.json" hl_lines="18"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$dynamicAnchor": "meta",
  "title": "Circular Schema",
  "description": "The recursive definition that applies to every nested subschema",
  "allOf": [
    { "$ref": "https://json-schema.org/draft/2020-12/meta/core" },
    { "$ref": "https://json-schema.org/draft/2020-12/meta/applicator" },
    { "$ref": "https://json-schema.org/draft/2020-12/meta/unevaluated" },
    { "$ref": "https://json-schema.org/draft/2020-12/meta/validation" },
    { "$ref": "https://json-schema.org/draft/2020-12/meta/meta-data" },
    { "$ref": "https://json-schema.org/draft/2020-12/meta/format-annotation" },
    { "$ref": "https://json-schema.org/draft/2020-12/meta/content" },
    { "$ref": "keywords/x-author.json" },
    { "$ref": "rules/circular/object-additional-properties.json" },
    { "$ref": "rules/circular/no-conditionals.json" }
  ],
  "unevaluatedProperties": false
}
```

Try adding `"x-team": "Growth"` to `person.json` and rebuild:

```text hl_lines="4"
$ docker build --tag my-registry . --file Dockerfile
...
error: The schema does not adhere to its metaschema
The object value was not expected to define the property "x-team"
  at instance location "/x-team"
  at evaluate path "/allOf/0/$ref/unevaluatedProperties"
The object value was not expected to define unevaluated properties
  at instance location ""
  at evaluate path "/allOf/0/$ref/unevaluatedProperties"
The object value was expected to validate against the referenced schema
  at instance location ""
  at evaluate path "/allOf/0/$ref"
The object value was expected to validate against the 2 given subschemas
  at instance location ""
  at evaluate path "/allOf"
```

Remove it and the build passes again.

## Step 8: Removing Official Vocabularies

So far we have only added to [JSON Schema
2020-12](https://www.learnjsonschema.com/2020-12/), but a custom dialect can
also take parts away. Our dialect already bans
[`if`](https://www.learnjsonschema.com/2020-12/applicator/if/),
[`then`](https://www.learnjsonschema.com/2020-12/applicator/then/), and
[`else`](https://www.learnjsonschema.com/2020-12/applicator/else/)
individually. Suppose we now want to drop the
[unevaluated](https://www.learnjsonschema.com/2020-12/unevaluated/) vocabulary
altogether, taking the
[`unevaluatedProperties`](https://www.learnjsonschema.com/2020-12/unevaluated/unevaluatedproperties/)
and
[`unevaluatedItems`](https://www.learnjsonschema.com/2020-12/unevaluated/unevaluateditems/)
keywords away in one go. Two deletions are needed. First, the
`https://json-schema.org/draft/2020-12/vocab/unevaluated` URI from the
[`$vocabulary`](https://www.learnjsonschema.com/2020-12/core/vocabulary/)
declaration of the entry point, highlighted below:

```json title="schemas/meta/custom.json" hl_lines="6"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$vocabulary": {
    "https://json-schema.org/draft/2020-12/vocab/core": true,
    "https://json-schema.org/draft/2020-12/vocab/applicator": true,
    "https://json-schema.org/draft/2020-12/vocab/unevaluated": true,
    "https://json-schema.org/draft/2020-12/vocab/validation": true,
    "https://json-schema.org/draft/2020-12/vocab/meta-data": true,
    "https://json-schema.org/draft/2020-12/vocab/format-annotation": true,
    "https://json-schema.org/draft/2020-12/vocab/content": true
  },
  "title": "Custom Meta-Schema",
  "description": "An extension of JSON Schema 2020-12 with custom keywords and rules",
  "allOf": [
    { "$ref": "circular.json" },
    { "$ref": "rules/top/x-author-required.json" }
  ]
}
```

Deleting it leaves the entry point as:

```json title="schemas/meta/custom.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$vocabulary": {
    "https://json-schema.org/draft/2020-12/vocab/core": true,
    "https://json-schema.org/draft/2020-12/vocab/applicator": true,
    "https://json-schema.org/draft/2020-12/vocab/validation": true,
    "https://json-schema.org/draft/2020-12/vocab/meta-data": true,
    "https://json-schema.org/draft/2020-12/vocab/format-annotation": true,
    "https://json-schema.org/draft/2020-12/vocab/content": true
  },
  "title": "Custom Meta-Schema",
  "description": "An extension of JSON Schema 2020-12 with custom keywords and rules",
  "allOf": [
    { "$ref": "circular.json" },
    { "$ref": "rules/top/x-author-required.json" }
  ]
}
```

Second, the [`$ref`](https://www.learnjsonschema.com/2020-12/core/ref/) to the
[unevaluated](https://www.learnjsonschema.com/2020-12/unevaluated/) vocabulary
meta-schema (`https://json-schema.org/draft/2020-12/meta/unevaluated`) from
the circular schema, highlighted below:

```json title="schemas/meta/circular.json" hl_lines="9"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$dynamicAnchor": "meta",
  "title": "Circular Schema",
  "description": "The recursive definition that applies to every nested subschema",
  "allOf": [
    { "$ref": "https://json-schema.org/draft/2020-12/meta/core" },
    { "$ref": "https://json-schema.org/draft/2020-12/meta/applicator" },
    { "$ref": "https://json-schema.org/draft/2020-12/meta/unevaluated" },
    { "$ref": "https://json-schema.org/draft/2020-12/meta/validation" },
    { "$ref": "https://json-schema.org/draft/2020-12/meta/meta-data" },
    { "$ref": "https://json-schema.org/draft/2020-12/meta/format-annotation" },
    { "$ref": "https://json-schema.org/draft/2020-12/meta/content" },
    { "$ref": "keywords/x-author.json" },
    { "$ref": "rules/circular/object-additional-properties.json" },
    { "$ref": "rules/circular/no-conditionals.json" }
  ],
  "unevaluatedProperties": false
}
```

Deleting it leaves the circular schema as:

```json title="schemas/meta/circular.json"
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "$dynamicAnchor": "meta",
  "title": "Circular Schema",
  "description": "The recursive definition that applies to every nested subschema",
  "allOf": [
    { "$ref": "https://json-schema.org/draft/2020-12/meta/core" },
    { "$ref": "https://json-schema.org/draft/2020-12/meta/applicator" },
    { "$ref": "https://json-schema.org/draft/2020-12/meta/validation" },
    { "$ref": "https://json-schema.org/draft/2020-12/meta/meta-data" },
    { "$ref": "https://json-schema.org/draft/2020-12/meta/format-annotation" },
    { "$ref": "https://json-schema.org/draft/2020-12/meta/content" },
    { "$ref": "keywords/x-author.json" },
    { "$ref": "rules/circular/object-additional-properties.json" },
    { "$ref": "rules/circular/no-conditionals.json" }
  ],
  "unevaluatedProperties": false
}
```

Because we closed the dialect in the previous step, the keywords of the
removed vocabulary are now rejected like any other unknown keyword. Add
`"unevaluatedProperties": false` to `person.json` and rebuild:

```text hl_lines="4"
$ docker build --tag my-registry . --file Dockerfile
...
error: The schema does not adhere to its metaschema
The object value was not expected to define the property "unevaluatedProperties"
  at instance location "/unevaluatedProperties"
  at evaluate path "/allOf/0/$ref/unevaluatedProperties"
The object value was not expected to define unevaluated properties
  at instance location ""
  at evaluate path "/allOf/0/$ref/unevaluatedProperties"
The object value was expected to validate against the referenced schema
  at instance location ""
  at evaluate path "/allOf/0/$ref"
The object value was expected to validate against the 2 given subschemas
  at instance location ""
  at evaluate path "/allOf"
```

There is some irony in that the circular schema itself still ends with
[`unevaluatedProperties`](https://www.learnjsonschema.com/2020-12/unevaluated/unevaluatedproperties/).
Like the rule files, it is written in the official
[2020-12](https://www.learnjsonschema.com/2020-12/) dialect, where the keyword
exists just fine. Only schemas written in
our custom dialect lose access to it.

Remove the keyword from `person.json` and rebuild one last time. Our dialect
is now complete.

## Step 9: Unit Testing the Meta-Schema

As the previous steps hint, meta-schema authoring can be tricky. Dynamic
anchors, where each rule is hooked, and closed dialects all interact in subtle
ways, and discovering a mistake through a failed registry build is a slow
feedback loop. The [Sourcemeta JSON Schema
CLI](https://github.com/sourcemeta/jsonschema) gives us a faster one,
including proper unit testing.

First, a small refactor. The collection definitions we have been writing in
`one.json` follow the same format as the configuration files of the JSON
Schema CLI, which are called `jsonschema.json`. By extracting the `example`
collection into such a file that lives alongside the schemas, the CLI learns
about our schemas too. The `path` property is relative to the manifest, so `.`
means its own directory:

```json title="schemas/example/jsonschema.json"
{
  "title": "Example Schemas",
  "description": "Schemas that must conform to our custom meta-schema",
  "path": ".",
  "defaultDialect": "../meta/custom"
}
```

Point the `example` collection at it using the
[`include`](../configuration.md#include) property. When `include` points to a
directory, the instance looks for a `jsonschema.json` file inside it:

```json title="one.json" hl_lines="13"
{
  "url": "http://localhost:8000",
  "html": {
    "name": "My Registry"
  },
  "contents": {
    "meta": {
      "title": "Meta",
      "description": "A collection of custom meta-schemas",
      "path": "./schemas/meta"
    },
    "example": {
      "include": "./schemas/example"
    }
  }
}
```

Rebuild the image to confirm the instance behaves exactly as before. The
difference is on the command line. For example, the CLI now knows the dialect
of our example schemas, so its `jsonschema metaschema` command can check them
against our meta-schema without any registry build involved. The CLI is
distributed in [several ways](https://github.com/sourcemeta/jsonschema), and
if you have Node.js, you can skip installation entirely by invoking
`jsonschema` as `npx --yes @sourcemeta/jsonschema` in the commands below:

```sh
$ jsonschema metaschema schemas/example/person.json --verbose
...
ok: /path/to/my-registry/schemas/example/person.json
  matches file:///path/to/my-registry/schemas/meta/custom.json
```

Note how `defaultDialect` does double duty: the registry resolves
`../meta/custom` against the URL the collection is served from, while the CLI
resolves it against the file system, and both land on our custom meta-schema.

Now for the unit tests. The CLI ships with a `jsonschema test` command that
takes files declaring a `target` schema and a list of instances that must or
must not validate against it. A meta-schema is just a schema whose instances
are schemas themselves, so we can pin down every behaviour of our dialect:

```json title="test/meta/custom.test.json"
{
  "target": "../../schemas/meta/custom.json",
  "tests": [
    {
      "description": "The empty object is invalid as the author keyword is required",
      "valid": false,
      "data": {}
    },
    {
      "description": "An object that only declares an author is valid",
      "valid": true,
      "data": {
        "x-author": "Jane Doe"
      }
    },
    {
      "description": "The author keyword must be a string",
      "valid": false,
      "data": {
        "x-author": 1
      }
    },
    {
      "description": "Nested subschemas do not require an author",
      "valid": true,
      "data": {
        "x-author": "Jane Doe",
        "properties": {
          "name": {
            "type": "string"
          }
        }
      }
    },
    {
      "description": "The conditional keywords are disallowed in nested subschemas",
      "valid": false,
      "data": {
        "x-author": "Jane Doe",
        "properties": {
          "name": {
            "if": true
          }
        }
      }
    },
    {
      "description": "An object type schema must declare additionalProperties",
      "valid": false,
      "data": {
        "x-author": "Jane Doe",
        "type": "object"
      }
    },
    {
      "description": "Keywords not defined by the dialect are disallowed",
      "valid": false,
      "data": {
        "x-author": "Jane Doe",
        "x-team": "Growth"
      }
    },
    {
      "description": "The unevaluated applicator keywords are not part of the dialect",
      "valid": false,
      "data": {
        "x-author": "Jane Doe",
        "unevaluatedProperties": false
      }
    }
  ]
}
```

```sh
$ jsonschema test ./test
/path/to/my-registry/test/meta/custom.test.json: PASS 8/8
```

Each of these cases guards one of the previous steps, so if you ever
restructure the meta-schema files, like we did when extracting the circular
schema, the tests will catch any behavioural difference. They also make great
continuous integration material. You can even run them as an earlier stage of
the Docker image build, so that a broken meta-schema can never make it into
the instance. Copying the schemas from the test stage is what forces that
stage, and therefore the tests, to run as part of the build:

```docker title="Dockerfile"
FROM ghcr.io/sourcemeta/one:<version> AS tests
WORKDIR /test
COPY schemas schemas
COPY test test
RUN jsonschema test ./test

FROM ghcr.io/sourcemeta/one:<version>
COPY one.json .
COPY --from=tests /test/schemas schemas
RUN sourcemeta one.json
```

The finished project looks like this:

```
├── Dockerfile
├── one.json
├── schemas
│   ├── example
│   │   ├── jsonschema.json
│   │   └── person.json
│   └── meta
│       ├── circular.json
│       ├── custom.json
│       ├── keywords
│       │   └── x-author.json
│       └── rules
│           ├── circular
│           │   ├── no-conditionals.json
│           │   └── object-additional-properties.json
│           └── top
│               └── x-author-required.json
└── test
    └── meta
        └── custom.test.json
```

From here, you can keep growing the dialect with more keywords, rules, and
test cases!
