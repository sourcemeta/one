// A GitHub deployment, as far as a registry signing people in against one can
// tell. It is served under one name over TLS, exactly as the real thing is, and
// it is the only host the browser or the registry ever reaches.
//
// Everything about authenticating a person is a real OAuth 2.0 authorization
// code exchange against the identity provider behind this, sign-in page and
// PKCE included. What is emulated is the surface GitHub puts in front of that:
// the two endpoint paths, the REST API the identity is assembled from, and the
// three places where GitHub departs from what a standard would have.
//
// Those three departures are the point of this file, since they are what an
// implementation is most likely to get wrong:
//
//   1. The API refuses a request that names no user agent, with a 403.
//   2. The token endpoint answers in a form encoding unless a request asks for
//      JSON, where RFC 6749 Section 5.1 mandates JSON.
//   3. The token endpoint answers a failure with a 200 carrying an `error`
//      member, where RFC 6749 Section 5.2 has a failure carry a 400.
//
// An organisation is a group at the provider, and a team is a group below one,
// so who belongs to what is declared in the realm rather than here.

import { createServer } from "node:https";
import { request as httpRequest } from "node:http";
import { readFileSync } from "node:fs";
import { createHash } from "node:crypto";

const PORT = Number(process.env.GITHUB_PORT ?? 9443);
const PROVIDER = process.env.GITHUB_PROVIDER ?? "http://keycloak:8080";
const REALM = process.env.GITHUB_REALM ?? "main";
const PUBLIC_ORIGIN = process.env.GITHUB_PUBLIC_ORIGIN ?? `https://github:${PORT}`;

const AUTHORIZATION_ENDPOINT = `/realms/${REALM}/protocol/openid-connect/auth`;
const TOKEN_ENDPOINT = `/realms/${REALM}/protocol/openid-connect/token`;
const USERINFO_ENDPOINT = `/realms/${REALM}/protocol/openid-connect/userinfo`;

// One call to the identity provider behind this, which is the only thing here
// that is not emulated
function upstream(path, options, body) {
  const target = new URL(path, PROVIDER);
  return new Promise((resolve) => {
    const outgoing = httpRequest(
      {
        hostname: target.hostname,
        port: target.port,
        path: target.pathname + target.search,
        method: options.method ?? "GET",
        headers: {
          ...options.headers,
          host: new URL(PUBLIC_ORIGIN).host,
          "x-forwarded-proto": "https",
          "x-forwarded-host": new URL(PUBLIC_ORIGIN).host
        }
      },
      (answer) => {
        const chunks = [];
        answer.on("data", (chunk) => chunks.push(chunk));
        answer.on("end", () =>
          resolve({
            status: answer.statusCode,
            headers: answer.headers,
            body: Buffer.concat(chunks)
          })
        );
      }
    );

    outgoing.on("error", () => resolve(null));
    if (body !== undefined) {
      outgoing.write(body);
    }

    outgoing.end();
  });
}

function readBody(incoming) {
  return new Promise((resolve) => {
    const chunks = [];
    incoming.on("data", (chunk) => chunks.push(chunk));
    incoming.on("end", () => resolve(Buffer.concat(chunks)));
  });
}

// An account identifier is a number that outlives a rename, so one is derived
// from the identifier the provider assigns rather than from the handle
function accountIdentifier(subject) {
  return parseInt(createHash("sha256").update(subject).digest("hex").slice(0, 8), 16);
}

// A group at the provider names an organisation, and a group below one names a
// team within it, which is the whole of the mapping between the two models
function membership(claims) {
  const groups = Array.isArray(claims.groups) ? claims.groups : [];
  const organizations = [];
  const teams = [];
  for (const group of groups) {
    const segments = group.split("/").filter((segment) => segment.length > 0);
    if (segments.length === 1) {
      organizations.push({ id: accountIdentifier(segments[0]), login: segments[0] });
    } else if (segments.length === 2) {
      teams.push({
        id: accountIdentifier(group),
        name: segments[1],
        slug: segments[1],
        organization: { id: accountIdentifier(segments[0]), login: segments[0] }
      });
    }
  }

  return { organizations, teams };
}

function send(response, status, headers, body) {
  response.writeHead(status, headers);
  response.end(body);
}

function sendJSON(response, status, document) {
  send(response, status, { "content-type": "application/json; charset=utf-8" },
       JSON.stringify(document));
}

// A listing is answered a page at a time, honouring what a request asked for,
// so that whoever reads one has to walk it rather than assume it arrives whole
function paginate(entries, url) {
  const size = Math.min(Number(url.searchParams.get("per_page") ?? 30) || 30, 100);
  const page = Math.max(Number(url.searchParams.get("page") ?? 1) || 1, 1);
  return entries.slice((page - 1) * size, page * size);
}

async function claimsFor(authorization) {
  const answer = await upstream(USERINFO_ENDPOINT, {
    headers: { authorization, accept: "application/json" }
  });
  if (answer === null || answer.status < 200 || answer.status >= 300) {
    return null;
  }

  try {
    return JSON.parse(answer.body.toString("utf-8"));
  } catch {
    return null;
  }
}

async function api(incoming, response, url) {
  // GitHub refuses a request that names no user agent outright, and says so in
  // prose rather than in the representation the rest of the API answers with
  if (!incoming.headers["user-agent"]) {
    send(response, 403, { "content-type": "text/plain; charset=utf-8" },
         "Request forbidden by administrative rules. Please make sure your " +
         "request has a User-Agent header.");
    return;
  }

  const authorization = incoming.headers.authorization;
  if (!authorization) {
    sendJSON(response, 401, { message: "Requires authentication" });
    return;
  }

  const claims = await claimsFor(authorization);
  if (claims === null) {
    sendJSON(response, 401, { message: "Bad credentials" });
    return;
  }

  const { organizations, teams } = membership(claims);
  const path = url.pathname.replace(/^\/api\/v3/, "");

  if (path === "/user") {
    sendJSON(response, 200, {
      login: claims.preferred_username,
      id: accountIdentifier(claims.sub),
      type: "User",
      name: claims.name ?? null,
      // The address on an account is the public one, which is unset far more
      // often than not, so a policy asking about one is made to go and look
      email: null
    });
    return;
  }

  if (path === "/user/emails") {
    const address = claims.email;
    sendJSON(response, 200,
             address === undefined
               ? []
               : paginate([{
                   email: address,
                   primary: true,
                   verified: claims.email_verified === true,
                   visibility: "private"
                 }], url));
    return;
  }

  if (path === "/user/orgs") {
    sendJSON(response, 200, paginate(organizations, url));
    return;
  }

  if (path === "/user/teams") {
    sendJSON(response, 200, paginate(teams, url));
    return;
  }

  sendJSON(response, 404, { message: "Not Found" });
}

async function token(incoming, response) {
  const body = await readBody(incoming);
  const answer = await upstream(TOKEN_ENDPOINT, {
    method: "POST",
    headers: {
      "content-type": "application/x-www-form-urlencoded",
      "content-length": Buffer.byteLength(body),
      accept: "application/json"
    }
  }, body);

  // What GitHub answers with, rather than what the provider behind this
  // answered with: every outcome carries a 200, and a failure names itself in
  // the body
  let payload;
  if (answer === null) {
    payload = { error: "server_error" };
  } else {
    let document;
    try {
      document = JSON.parse(answer.body.toString("utf-8"));
    } catch {
      document = { error: "server_error" };
    }

    payload = answer.status >= 200 && answer.status < 300
      ? {
          access_token: document.access_token,
          token_type: "bearer",
          scope: document.scope ?? ""
        }
      : {
          error: document.error ?? "bad_verification_code",
          error_description: document.error_description ?? "",
          error_uri: "https://docs.github.com/apps/oauth"
        };
  }

  // The default representation is a form encoding, which is what a client that
  // did not ask for JSON is answered with
  const wants = String(incoming.headers.accept ?? "");
  if (wants.includes("application/json")) {
    send(response, 200, { "content-type": "application/json; charset=utf-8" },
         JSON.stringify(payload));
    return;
  }

  const encoded = new URLSearchParams();
  for (const [name, value] of Object.entries(payload)) {
    encoded.set(name, String(value ?? ""));
  }

  send(response, 200,
       { "content-type": "application/x-www-form-urlencoded; charset=utf-8" },
       encoded.toString());
}

// Everything else is the identity provider itself, served under this name so
// that the browser only ever sees one deployment
async function proxy(incoming, response, url) {
  const body = incoming.method === "POST" ? await readBody(incoming) : undefined;
  const headers = { ...incoming.headers };
  delete headers.host;
  delete headers["accept-encoding"];
  if (body !== undefined) {
    headers["content-length"] = Buffer.byteLength(body);
  }

  const answer = await upstream(url.pathname + url.search,
                                { method: incoming.method, headers }, body);
  if (answer === null) {
    send(response, 502, { "content-type": "text/plain" }, "Bad Gateway");
    return;
  }

  send(response, answer.status, answer.headers, answer.body);
}

const server = createServer(
  {
    cert: readFileSync("/etc/github/tls/github.crt"),
    key: readFileSync("/etc/github/tls/github.key")
  },
  (incoming, response) => {
    const url = new URL(incoming.url, PUBLIC_ORIGIN);

    // The two endpoint paths GitHub serves, which are the only part of the
    // protocol surface that differs from where the provider behind this
    // serves them
    if (url.pathname === "/login/oauth/authorize") {
      const target = new URL(AUTHORIZATION_ENDPOINT, PUBLIC_ORIGIN);
      target.search = url.search;
      // What the deployment needs of the provider behind it to answer for who
      // signed in, which is its own business rather than anything its client
      // asked for
      const scope = (url.searchParams.get("scope") ?? "").split(" ")
        .filter((entry) => entry.length > 0);
      target.searchParams.set("scope", ["openid", ...scope].join(" "));
      send(response, 302, { location: target.toString() }, "");
      return;
    }

    if (url.pathname === "/login/oauth/access_token") {
      token(incoming, response);
      return;
    }

    if (url.pathname.startsWith("/api/v3/")) {
      api(incoming, response, url);
      return;
    }

    proxy(incoming, response, url);
  }
);

server.listen(PORT, "0.0.0.0");
