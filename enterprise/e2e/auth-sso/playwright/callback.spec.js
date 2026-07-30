import { test, expect } from '@playwright/test';
import { createHmac } from 'node:crypto';

// The sandbox's session signing secret, from its environment file. Knowing it
// lets these tests mint correctly signed transaction cookies whose payloads
// the login endpoint would never produce, proving the callback rejects a
// hollow transaction on its own rather than downstream.
const SESSION_SECRET = 'a-session-signing-secret-for-the-sso-sandbox';

// A sealed value is signed under a key derived from the secret and what the
// value is for, so forging one means naming its purpose too. That is what
// stops a transaction from being presented as a session.
const TRANSACTION_LABEL = 'sourcemeta/one/transaction';

const STATE = 'e2e-forged-state-value-for-callback-tests-1';
const NONCE = 'e2e-forged-nonce-value-for-callback-tests-1';
const VERIFIER = 'e2e-forged-verifier-value-for-callback-12';

const CALLBACK = /\/self\/v1\/auth\/callback\//;

function sealTransaction(payload) {
  // A sealed value carries the instant it was minted alongside its expiry, so
  // that the interval it claims is one the instance would have produced
  const issued = Math.floor(Date.now() / 1000);
  const expiry = issued + 600;
  const encoded = Buffer.from(JSON.stringify(payload)).toString('base64url');
  const prefix = `1.${issued}.${expiry}.${encoded}`;
  const key = createHmac('sha256', SESSION_SECRET)
    .update(TRANSACTION_LABEL)
    .digest();
  const signature = createHmac('sha256', key).update(prefix).digest('base64url');
  return `${prefix}.${signature}`;
}

// A sealed value is signed rather than encrypted, so whoever holds one can
// read what it carries. That is what lets these tests reproduce a real login's
// transaction and alter one field of it.
function openSealed(value) {
  return JSON.parse(Buffer.from(value.split('.')[3], 'base64url').toString());
}

// The whole of a `Set-Cookie` a response carries under a given name,
// attributes and all, so that what a browser would store can be measured
// rather than only what it would send back.
async function setCookieFrom(response, name) {
  const header = (await response.headersArray())
    .filter((entry) => entry.name.toLowerCase() === 'set-cookie')
    .map((entry) => entry.value)
    .find((value) => value.startsWith(`${name}=`));
  expect(header).toBeDefined();
  return header;
}

function valueOf(setCookie) {
  return setCookie.slice(setCookie.indexOf('=') + 1).split(';')[0];
}

async function callback(request, transaction) {
  return request.get(
    `/self/v1/auth/callback/keycloak?code=a-code-nobody-issued&state=${STATE}`,
    {
      headers: {
        Cookie: `sourcemeta_one_transaction=${sealTransaction(transaction)}`
      }
    }
  );
}

// Signs in for real, having first replaced the transaction the login minted
// with one differing from it in whatever the caller alters. The authorization
// request has already gone to the provider by then, so the provider answers
// the login that was actually started while this instance reads the altered
// account of it. That is what puts a genuine, unspent code in front of a
// transaction that does not match it.
async function signInAlteringTheTransaction(page, context, alteration) {
  await page.goto('/self/v1/auth/login/keycloak');
  const carried = (await context.cookies()).find(
    (cookie) => cookie.name === 'sourcemeta_one_transaction'
  );
  expect(carried).toBeDefined();

  const sealed = sealTransaction({
    ...openSealed(carried.value),
    ...alteration
  });
  await context.addCookies([
    {
      name: 'sourcemeta_one_transaction',
      value: sealed,
      url: process.env.PLAYWRIGHT_BASE_URL
    }
  ]);

  await page.locator('#username').fill('jane');
  await page.locator('#password').fill('jane-password');
  const answer = page.waitForResponse(CALLBACK);
  await page.locator('#kc-login').click();
  return { response: await answer, sealed };
}

test.describe('Callback transaction validation', () => {
  test('a signed transaction with every field is accepted', async ({
    request
  }) => {
    const response = await callback(request, {
      policy: 'keycloak',
      state: STATE,
      nonce: NONCE,
      verifier: VERIFIER
    });
    // A transaction that is rejected outright is refused as a bad request, so
    // getting past that to a server-side failure is what shows this one was
    // accepted. Which step then failed is deliberately not said
    expect(response.status()).toBe(500);
    expect(await response.json()).toEqual({
      type: 'urn:sourcemeta:one:auth-incomplete',
      title: 'Internal Server Error',
      status: 500,
      detail: 'The session could not be established'
    });
  });

  test('a signed transaction with an empty verifier is rejected outright', async ({
    request
  }) => {
    const response = await callback(request, {
      policy: 'keycloak',
      state: STATE,
      nonce: NONCE,
      verifier: ''
    });
    expect(response.status()).toBe(400);
    expect(await response.json()).toEqual({
      type: 'urn:sourcemeta:one:auth-invalid-callback',
      title: 'Bad Request',
      status: 400,
      detail: 'The login could not be completed'
    });
  });

  test('a signed transaction with an empty nonce is rejected outright', async ({
    request
  }) => {
    const response = await callback(request, {
      policy: 'keycloak',
      state: STATE,
      nonce: '',
      verifier: VERIFIER
    });
    expect(response.status()).toBe(400);
    expect(await response.json()).toEqual({
      type: 'urn:sourcemeta:one:auth-invalid-callback',
      title: 'Bad Request',
      status: 400,
      detail: 'The login could not be completed'
    });
  });

  test('a signed transaction naming no state at all matches nothing', async ({
    request
  }) => {
    // An empty state is not a state a provider could echo, so it is refused
    // rather than read as there being nothing to compare
    const response = await request.get(
      '/self/v1/auth/callback/keycloak?code=a-code-nobody-issued&state=',
      {
        headers: {
          Cookie: `sourcemeta_one_transaction=${sealTransaction({
            policy: 'keycloak',
            state: '',
            nonce: NONCE,
            verifier: VERIFIER
          })}`
        }
      }
    );
    expect(response.status()).toBe(400);
    expect(await response.json()).toEqual({
      type: 'urn:sourcemeta:one:auth-invalid-callback',
      title: 'Bad Request',
      status: 400,
      detail: 'The login could not be completed'
    });
  });

  test('a transaction is found among several the browser carries', async ({
    request
  }) => {
    // A browser can carry several cookies under one name, because a parent
    // domain and the host itself can each set one and neither the header nor
    // the order says which is which. Letting whoever set the other one decide
    // which login this is turns the cookie from a defence against a forged
    // callback into the way to mount one, so every value is tried.
    const other = sealTransaction({
      policy: 'keycloak',
      state: 'a-state-belonging-to-some-other-login-1',
      nonce: NONCE,
      verifier: VERIFIER
    });
    const mine = sealTransaction({
      policy: 'keycloak',
      state: STATE,
      nonce: NONCE,
      verifier: VERIFIER
    });
    const declined = {
      type: 'urn:sourcemeta:one:auth-login-declined',
      title: 'Forbidden',
      status: 403,
      detail: 'The identity provider declined the login'
    };

    const behind = await request.get(
      `/self/v1/auth/callback/keycloak?error=access_denied&state=${STATE}`,
      {
        headers: {
          Cookie: `sourcemeta_one_transaction=${other}; sourcemeta_one_transaction=${mine}`
        }
      }
    );
    expect(behind.status()).toBe(403);
    expect(await behind.json()).toEqual(declined);

    // Mirrored, so neither position is the one that decides
    const ahead = await request.get(
      `/self/v1/auth/callback/keycloak?error=access_denied&state=${STATE}`,
      {
        headers: {
          Cookie: `sourcemeta_one_transaction=${mine}; sourcemeta_one_transaction=${other}`
        }
      }
    );
    expect(ahead.status()).toBe(403);
    expect(await ahead.json()).toEqual(declined);
  });
});

// Everything above forges a transaction against a code no provider issued, so
// nothing there reaches an identity token. These sign in for real, altering
// one field of the transaction after the authorization request has gone out,
// so the provider answers the login that was started while this instance reads
// an account of it that differs in exactly that field.
test.describe('Callback identity validation against a real login', () => {
  test('a faithfully reproduced transaction completes the login', async ({
    page,
    context
  }) => {
    const { response, sealed } = await signInAlteringTheTransaction(
      page,
      context,
      {}
    );

    // The control the two below are read against: re-sealing the transaction
    // unchanged still signs the person in, so when an altered one fails, it
    // fails on what was altered rather than on having been re-sealed
    expect(response.status()).toBe(303);
    expect(response.headers()['location']).toBe('/private');

    const session = await setCookieFrom(response, 'sourcemeta_one_session');

    // RFC 6265 Section 6.1 asks a user agent to support at least 4096 bytes
    // per cookie, counting its name, value and attributes. A browser that
    // drops a larger one says nothing about having done so, which would look
    // like signing in and then not being signed in, so the whole of it is kept
    // inside what every browser will store
    expect(session.length).toBeLessThan(4000);

    // And it carries the identity token, which is what lets signing out prove
    // whose session the provider is being asked to end
    const payload = openSealed(valueOf(session));
    expect(payload.policy).toBe('keycloak');
    expect(payload.subject).toBeTruthy();
    expect(payload.id_token).toBeTruthy();

    // The code that session was built from is spent, so the very same answer
    // buys nothing a second time. The transaction is put back first, since the
    // callback expired it, so what is being refused here is the code alone
    await context.addCookies([
      {
        name: 'sourcemeta_one_transaction',
        value: sealed,
        url: process.env.PLAYWRIGHT_BASE_URL
      }
    ]);
    const again = await page.request.get(response.url(), { maxRedirects: 0 });
    expect(again.status()).toBe(500);
    expect(await again.json()).toEqual({
      type: 'urn:sourcemeta:one:auth-incomplete',
      title: 'Internal Server Error',
      status: 500,
      detail: 'The session could not be established'
    });
  });

  test('an identity token echoing a nonce other than the one asked for is refused', async ({
    page,
    context
  }) => {
    // The provider echoes the nonce the authorization request named, so
    // expecting a different one back is what a token minted for some other
    // login looks like. Nothing else here is altered: the code is genuine and
    // unspent, and the exchange for it succeeds before the token is read
    const { response } = await signInAlteringTheTransaction(page, context, {
      nonce: 'a-nonce-the-provider-was-never-asked-to-echo'
    });

    expect(response.status()).toBe(500);
    expect(await response.json()).toEqual({
      type: 'urn:sourcemeta:one:auth-incomplete',
      title: 'Internal Server Error',
      status: 500,
      detail: 'The session could not be established'
    });
    expect(response.headers()['set-cookie']).toBeUndefined();
  });

  test('a code redeemed with a verifier other than the one that earned it is refused', async ({
    page,
    context
  }) => {
    // The authorization request committed to the real verifier by its digest,
    // so the code is worth nothing without it. That is what keeps a code that
    // leaks through a log or a referrer from being spent by whoever reads it
    const { response } = await signInAlteringTheTransaction(page, context, {
      verifier: 'a-verifier-no-authorization-request-committed'
    });

    expect(response.status()).toBe(500);
    expect(await response.json()).toEqual({
      type: 'urn:sourcemeta:one:auth-incomplete',
      title: 'Internal Server Error',
      status: 500,
      detail: 'The session could not be established'
    });
    expect(response.headers()['set-cookie']).toBeUndefined();
  });
});

// A transaction and a session are both values this instance seals for one
// policy, and the cookie name that tells them apart travels outside the
// signature where whoever presents it chooses the name. Anybody can obtain a
// transaction, since starting a login asks for nothing, which is what once
// made presenting one as a session an unauthenticated way into every gated
// path. These carry a real value of each kind across to the other's name, in a
// request holding nothing else, so what is refused is the value rather than
// the absence of one.
test.describe('Neither sealed value passes for the other', () => {
  test('a transaction under the session name opens nothing a session opens', async ({
    page,
    context,
    request
  }) => {
    const login = await context.request.get('/self/v1/auth/login/keycloak', {
      maxRedirects: 0
    });
    expect(login.status()).toBe(303);
    const transaction = valueOf(
      await setCookieFrom(login, 'sourcemeta_one_transaction')
    );

    const { response } = await signInAlteringTheTransaction(page, context, {});
    const session = valueOf(
      await setCookieFrom(response, 'sourcemeta_one_session')
    );

    // A real session opens the gated schema, which is what makes the refusal
    // below mean the transaction was read and rejected rather than that
    // nothing was read at all
    const opened = await request.get('/private/secret.json', {
      headers: { Cookie: `sourcemeta_one_session=${session}` }
    });
    expect(opened.status()).toBe(200);

    const denied = await request.get('/private/secret.json', {
      headers: { Cookie: `sourcemeta_one_session=${transaction}` }
    });
    expect(denied.status()).toBe(401);
    expect(await denied.json()).toEqual({
      type: 'urn:sourcemeta:one:authentication-required',
      title: 'Unauthorized',
      status: 401,
      detail: 'This resource requires authentication'
    });
  });

  test('a session under the transaction name proves no login was started', async ({
    page,
    context,
    request
  }) => {
    const { response } = await signInAlteringTheTransaction(page, context, {});
    const session = valueOf(
      await setCookieFrom(response, 'sourcemeta_one_session')
    );

    const refused = await request.get(
      `/self/v1/auth/callback/keycloak?code=a-code-nobody-issued&state=${STATE}`,
      { headers: { Cookie: `sourcemeta_one_transaction=${session}` } }
    );
    expect(refused.status()).toBe(400);
    expect(await refused.json()).toEqual({
      type: 'urn:sourcemeta:one:auth-invalid-callback',
      title: 'Bad Request',
      status: 400,
      detail: 'The login could not be completed'
    });
  });
});
