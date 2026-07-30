import { test, expect } from '@playwright/test';
import { createHmac } from 'node:crypto';

// This instance is gated at the root by all three policy types at once, so
// every path is covered, including the `/self` schemas the MCP endpoint reads
// to do its own work. That is what makes it the case worth testing: a browser
// admitted by its session cookie reaches a tool, and the tool then has to
// resolve its own envelope schemas on that same caller's behalf. Carrying only
// the bearer that far would leave a caller who passed the gate refused by the
// work done behind it, for a credential the gate had already accepted.
//
// The session is minted here rather than obtained by signing in, since what is
// under test is what a session admits once held, not how one is obtained.

const SESSION_SECRET = 'a-session-signing-secret-for-the-auth-closed-sandbox';
const SESSION_LABEL = 'sourcemeta/one/session';

function sealSession(payload) {
  const issued = Math.floor(Date.now() / 1000);
  const expiry = issued + 3600;
  const encoded = Buffer.from(JSON.stringify(payload)).toString('base64url');
  const prefix = `1.${issued}.${expiry}.${encoded}`;
  const key = createHmac('sha256', SESSION_SECRET).update(SESSION_LABEL).digest();
  const signature = createHmac('sha256', key).update(prefix).digest('base64url');
  return `${prefix}.${signature}`;
}

const SESSION = sealSession({ policy: 'keycloak', subject: 'jane' });

async function mcp(request, body, cookie) {
  return request.post('/self/v1/mcp', {
    headers: {
      'content-type': 'application/json',
      accept: 'application/json, text/event-stream',
      'mcp-protocol-version': '2025-11-25',
      ...(cookie ? { cookie: `sourcemeta_one_session=${cookie}` } : {})
    },
    data: body,
    failOnStatusCode: false
  });
}

test.describe('MCP under a browser session', () => {
  test('a session admits a tool call, arguments and all', async ({
    request
  }) => {
    const response = await mcp(
      request,
      {
        jsonrpc: '2.0',
        id: 1,
        method: 'tools/call',
        params: { name: 'search_schemas', arguments: { q: 'object' } }
      },
      SESSION
    );

    expect(response.status()).toBe(200);
    const body = await response.json();
    // A tool that ran. Not an authentication refusal, and not the internal
    // error that resolving its own schemas on a bearer-only credential used to
    // produce for a caller holding a cookie instead
    expect(body.error).toBeUndefined();
    expect(body.result).toBeDefined();
    expect(body.result.isError).toBeFalsy();
    expect(body.result.structuredContent).toBeDefined();
  });

  test('a session reads a resource', async ({ request }) => {
    // `resources/read` resolves an artifact on the caller's behalf, which is
    // the other place the credential has to reach
    const listed = await mcp(
      request,
      { jsonrpc: '2.0', id: 2, method: 'resources/list' },
      SESSION
    );
    expect(listed.status()).toBe(200);
    const resources = (await listed.json()).result.resources;
    expect(resources.length).toBeGreaterThan(0);

    const read = await mcp(
      request,
      {
        jsonrpc: '2.0',
        id: 3,
        method: 'resources/read',
        params: { uri: resources[0].uri }
      },
      SESSION
    );
    expect(read.status()).toBe(200);
    const body = await read.json();
    expect(body.error).toBeUndefined();
    expect(body.result.contents.length).toBeGreaterThan(0);
  });

  test('a caller with no credential is still refused', async ({ request }) => {
    // The gate is what this instance is for, so admitting a cookie must not
    // have admitted everybody
    const response = await mcp(request, {
      jsonrpc: '2.0',
      id: 4,
      method: 'tools/call',
      params: { name: 'search_schemas', arguments: { q: 'object' } }
    });
    expect(response.status()).toBe(401);
  });

  test('a session forged under another secret is refused', async ({
    request
  }) => {
    const forged = (() => {
      const issued = Math.floor(Date.now() / 1000);
      const encoded = Buffer.from(
        JSON.stringify({ policy: 'keycloak', subject: 'jane' })
      ).toString('base64url');
      const prefix = `1.${issued}.${issued + 3600}.${encoded}`;
      const key = createHmac('sha256', 'a-secret-nobody-here-signs-with')
        .update(SESSION_LABEL)
        .digest();
      return `${prefix}.${createHmac('sha256', key)
        .update(prefix)
        .digest('base64url')}`;
    })();

    const response = await mcp(
      request,
      {
        jsonrpc: '2.0',
        id: 5,
        method: 'tools/call',
        params: { name: 'search_schemas', arguments: { q: 'object' } }
      },
      forged
    );
    expect(response.status()).toBe(401);
  });
});
