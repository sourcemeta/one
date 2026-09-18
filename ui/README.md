The ui For the sourcemeta one project

## Custom Debugger

The "Custom Debugger" lets you paste any JSON Schema and instance and step
through the real Sourcemeta Blaze evaluation, without needing a connected
registry. It relies on a small local server that shells out to the
`@sourcemeta/jsonschema` CLI to compile and trace the schema.

Start it before opening the Custom Debugger:

```sh
npm run compile-server
```

It listens on `http://localhost:4545` by default (override with `PORT`). The
Custom Debugger UI lets you point at a different URL if you're running it
elsewhere.
