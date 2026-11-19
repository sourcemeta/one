# HTTP/1.1 wire tests

The CTest target `one.http.wire` starts a test-only echo handler using
One's HTTPServer, HTTPRequest, and HTTPResponse. It binds IPv4 loopback
at an OS-selected port and announces the port on stdout. The runner waits
for the announcement, bounds socket and process operations, and shuts down
the child on success and failure. The existing Docker CTest run picks up
this test when ONE_TESTS and ONE_SERVER are enabled.

Run from a One build configured with `ONE_TESTS=ON` and `ONE_SERVER=ON`,
with Node available:

```sh
cmake --build build --target sourcemeta_one_http_wire_echo
ctest --test-dir build -R '^one.http.wire$' --output-on-failure
```

The cases are independently written from RFC 9112 and motivated by the
reviewed categories in the [h1spec revision pinned by uWebSockets v20.81.0](https://github.com/uNetworking/h1spec/tree/f0a5650a20c575fbea0f7179a3a9cfa50f20ba6e).
The test takes no runtime dependency on h1spec or Deno.

| Case | Expected outcome | Basis |
| --- | --- | --- |
| Origin-form GET and a request split across writes | 200 and exact path | RFC 9112 sections 2.2 and 3.2.1 |
| Fixed-length binary body | 200 and exact bytes | RFC 9112 section 6.3 |
| Chunked body | 200 and decoded bytes | RFC 9112 section 7.1 |
| Missing Host | 400 | RFC 9112 section 3.2 |
| Bodies of 64 and 65 bytes | Exact echo at the cap and 413 above it | One HTTPRequest::body fixture policy |

The remaining h1spec cases are outside this initial subset. In particular,
this gate excludes timeout-only cases, special method or interim-response
handling, and cases whose broad status ranges need separate RFC review.
The split-write checks complete the requests and verify the response.
TCP is a byte stream, so the test does not assume that each client write
maps to a separate server read.
