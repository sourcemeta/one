import { createServer } from "node:http";
import { mkdtemp, writeFile, rm } from "node:fs/promises";
import { tmpdir } from "node:os";
import { join } from "node:path";
import { spawn as spawnCli } from "@sourcemeta/jsonschema";

const PORT = process.env.PORT ? Number(process.env.PORT) : 4545;

const parseTraceOutput = (text) => {
  const blocks = text
    .split(/\n\s*\n/)
    .map((block) => block.trim())
    .filter(Boolean);

  const steps = [];
  for (const block of blocks) {
    const lines = block.split("\n").map((line) => line.trim());
    const head = lines[0].match(
      /^(?:->|<-)\s*\((push|pass|fail)\)\s*"(.*)"\s*\((.+)\)$/
    );
    if (!head) continue;

    const [, type, evaluatePath, name] = head;
    let instanceLocation = "";
    let keywordLocation = "";
    let vocabulary = null;

    for (const line of lines.slice(1)) {
      const inst = line.match(/^at instance location "(.*)"/);
      if (inst) instanceLocation = inst[1];
      const kw = line.match(/^at keyword location "(.*)"$/);
      if (kw) keywordLocation = kw[1];
      const voc = line.match(/^at vocabulary "(.*)"$/);
      if (voc) vocabulary = voc[1];
    }

    steps.push({
      type,
      name,
      evaluatePath,
      instanceLocation,
      keywordLocation,
      vocabulary,
      annotation: null,
      message: null,
    });
  }
  return steps;
};

const parseErrorMessages = (text) => {
  const messages = {};
  const re =
    /^ {2}(.+)\n {4}at instance location "(.*)" \(line \d+, column \d+\)\n {4}at evaluate path "(.*)"$/gm;
  let match;
  while ((match = re.exec(text))) {
    const [, message, , evaluatePath] = match;
    messages[evaluatePath] = message;
  }
  return messages;
};

const server = createServer((req, res) => {
  res.setHeader("Access-Control-Allow-Origin", "*");
  res.setHeader("Access-Control-Allow-Methods", "POST, OPTIONS");
  res.setHeader("Access-Control-Allow-Headers", "Content-Type");

  if (req.method === "OPTIONS") {
    res.writeHead(204);
    res.end();
    return;
  }

  if (req.method !== "POST" || req.url !== "/trace") {
    res.writeHead(404, { "Content-Type": "application/json" });
    res.end(JSON.stringify({ error: "Not found" }));
    return;
  }

  let body = "";
  req.on("data", (chunk) => (body += chunk));
  req.on("end", async () => {
    let dir;
    try {
      const { schema, instance } = JSON.parse(body);
      dir = await mkdtemp(join(tmpdir(), "one-ui-trace-"));
      const schemaPath = join(dir, "schema.json");
      const instancePath = join(dir, "instance.json");
      await writeFile(schemaPath, schema, "utf-8");
      await writeFile(instancePath, instance, "utf-8");

      const [traceResult, plainResult] = await Promise.all([
        spawnCli(["validate", schemaPath, instancePath, "--trace"]),
        spawnCli(["validate", schemaPath, instancePath]),
      ]);

      if (traceResult.code !== 0 && traceResult.code !== 2) {
        res.writeHead(422, { "Content-Type": "application/json" });
        res.end(
          JSON.stringify({
            error: traceResult.stderr.trim() || "Failed to compile schema",
          })
        );
        return;
      }

      const steps = parseTraceOutput(traceResult.stdout);
      const messages = parseErrorMessages(plainResult.stderr);
      for (const step of steps) {
        if (step.type === "fail" && messages[step.evaluatePath]) {
          step.message = messages[step.evaluatePath];
        }
      }

      res.writeHead(200, { "Content-Type": "application/json" });
      res.end(JSON.stringify({ valid: plainResult.code === 0, steps }));
    } catch (error) {
      res.writeHead(500, { "Content-Type": "application/json" });
      res.end(JSON.stringify({ error: error.message }));
    } finally {
      if (dir) await rm(dir, { recursive: true, force: true }).catch(() => {});
    }
  });
});

server.listen(PORT, () => {
  console.log(`one-ui compile/trace server listening on http://localhost:${PORT}`);
});
