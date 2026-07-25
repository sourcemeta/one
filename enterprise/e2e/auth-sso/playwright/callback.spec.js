import { test, expect } from '@playwright/test';
import { createHmac } from 'node:crypto';

// The sandbox's session signing secret, from its environment file. Knowing it
// lets these tests mint correctly signed transaction cookies whose payloads
// the login endpoint would never produce, proving the callback rejects a
// hollow transaction on its own rather than downstream.
const SESSION_SECRET = 'a-session-signing-secret-for-the-sso-sandbox';

const STATE = 'e2e-forged-state-value-for-callback-tests-1';
const NONCE = 'e2e-forged-nonce-value-for-callback-tests-1';
const VERIFIER = 'e2e-forged-verifier-value-for-callback-12';

function sealTransaction(payload) {
  const expiry = Math.floor(Date.now() / 1000) + 600;
  const encoded = Buffer.from(JSON.stringify(payload)).toString('base64url');
  const prefix = `1.${expiry}.${encoded}`;
  const signature = createHmac('sha256', SESSION_SECRET)
    .update(prefix)
    .digest('base64url');
  return `${prefix}.${signature}`;
}

async function callback(request, transaction) {
  return request.get(
    `/self/v1/auth/callback/keycloak?code=a-code-nobody-issued&state=${STATE}`,
    {
      headers: {
        Cookie: `sourcemeta_one_transaction_keycloak=${sealTransaction(transaction)}`
      }
    }
  );
}

test.describe('Callback transaction validation', () => {
  test('a signed transaction with every field reaches the code exchange', async ({
    request
  }) => {
    const response = await callback(request, {
      policy: 'keycloak',
      state: STATE,
      nonce: NONCE,
      verifier: VERIFIER
    });
    // The provider refuses the made-up code, which proves the transaction
    // itself was accepted and the flow progressed to the exchange
    expect(response.status()).toBe(502);
    expect(await response.json()).toEqual({
      type: 'urn:sourcemeta:one:auth-exchange-failed',
      title: 'Bad Gateway',
      status: 502,
      detail: 'The authorization code could not be redeemed'
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
});
