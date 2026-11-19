const assert = require("node:assert/strict");
const { spawn } = require("node:child_process");
const { once } = require("node:events");
const { createConnection } = require("node:net");
const { createInterface } = require("node:readline");
const { setTimeout: delay } = require("node:timers/promises");

const HOST = "127.0.0.1";
const DEADLINE = 3000;

function request(port, parts) {
  return new Promise((resolve, reject) => {
    const socket = createConnection({ host: HOST, port });
    const chunks = [];
    const deadline = setTimeout(() => socket.destroy(new Error("Response deadline exceeded")), DEADLINE);
    socket.on("data", (chunk) => chunks.push(chunk));
    socket.once("error", reject);
    socket.once("close", (failed) => {
      clearTimeout(deadline);
      if (!failed) resolve(Buffer.concat(chunks));
    });
    socket.once("connect", async () => {
      try {
        for (const [index, part] of parts.entries()) {
          if (index > 0) {
            await delay(40);
            assert.equal(chunks.length, 0, "response arrived before the request was complete");
            assert.equal(socket.destroyed, false, "connection closed before the request was complete");
          }
          socket.write(part);
        }
      } catch (error) {
        socket.destroy(error);
      }
    });
  });
}

function checkResponse(raw, status, expectedBody) {
  const split = raw.indexOf("\r\n\r\n");
  assert.ok(split > 0, "expected a complete response header block");
  const [line, ...headers] = raw.subarray(0, split).toString("latin1").split("\r\n");
  assert.equal(Number(/^HTTP\/1\.1 (\d{3})(?: |$)/.exec(line)?.[1]), status, line);
  if (expectedBody !== undefined) {
    const body = Buffer.from(expectedBody);
    assert.deepEqual(raw.subarray(split + 4), body);
    assert.deepEqual(headers.filter((header) => /^content-length:/i.test(header))
      .map((header) => header.split(":")[1].trim()), [String(body.length)]);
  }
}

async function run(port) {
  const get = "GET /example?query=1 HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n";
  const post = "POST /echo HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n";
  const binary = Buffer.from([0, 0x41, 0xff, 0x0a]);
  // These cases re-express behaviors covered by h1spec. See README.md.
  const cases = [
    ["GET request target", [get], 200, "/example"],
    ["request line and header split across writes", [get.slice(0, 9), get.slice(9, 30), get.slice(30)], 200, "/example"],
    ["fixed-length binary body", [post + "Content-Length: 4\r\n\r\n", binary], 200, binary],
    ["chunked body", [post + "Transfer-Encoding: chunked\r\n\r\n2\r\nhe\r\n3\r\nllo\r\n0\r\n\r\n"], 200, "hello"],
    ["missing Host", ["GET /example HTTP/1.1\r\nConnection: close\r\n\r\n"], 400],
    // The body limit is specific to the test fixture and exercises One's wrapper.
    ["body at the limit", [post + "Content-Length: 64\r\n\r\n" + "a".repeat(64)], 200, "a".repeat(64)],
    ["body above the limit", [post + "Content-Length: 65\r\n\r\n" + "a".repeat(65)], 413],
  ];
  for (const [name, parts, status, body] of cases) {
    try {
      checkResponse(await request(port, parts), status, body);
      console.log("PASS " + name);
    } catch (error) {
      throw new Error(name + ": " + error.message, { cause: error });
    }
  }
}

async function main() {
  const child = spawn(process.argv[2], [], { stdio: ["ignore", "pipe", "inherit"] });
  const lines = createInterface({ input: child.stdout });
  const ready = once(lines, "line").then(([line]) => Number(line));
  const closed = once(child, "close");
  let timeout;
  let failed = false;
  const readinessDeadline = new Promise((_, reject) => {
    timeout = setTimeout(() => reject(new Error(
      "Echo server did not announce readiness before the deadline"
    )), DEADLINE);
  });
  try {
    const port = await Promise.race([
      ready,
      readinessDeadline,
      closed.then(([code, signal]) => { throw new Error("Echo server exited before readiness: " + (signal || code)); }),
    ]);
    clearTimeout(timeout);
    assert.ok(Number.isInteger(port) && port > 0 && port <= 65535, "expected an ephemeral port");
    await run(port);
  } catch (error) {
    failed = true;
    throw error;
  } finally {
    clearTimeout(timeout);
    lines.close();
    child.kill("SIGTERM");
    const kill = setTimeout(() => child.kill("SIGKILL"), DEADLINE);
    try {
      const [code, signal] = await closed;
      assert.equal(signal, null, "echo server terminated by " + signal);
      assert.equal(code, 0, "expected a graceful server exit");
    } catch (shutdownError) {
      if (!failed) throw shutdownError;
      console.error("Echo server cleanup also failed:", shutdownError.message);
    } finally {
      clearTimeout(kill);
    }
  }
}

main().catch((error) => {
  console.error(error);
  process.exitCode = 1;
});
