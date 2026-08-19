import { test, expect } from '@playwright/test';
import { createHmac, hkdfSync } from 'node:crypto';

// A policy names one environment variable per secret it accepts, newest first.
// The sandbox gates /rotated with two, so a session signed under either is
// admitted while a fresh one is signed under the first alone. That is what
// lets a secret be replaced without ending the sessions signed under the one
// it replaces.
const NEW_SECRET = 'the-session-secret-signing-from-now-on';
const OLD_SECRET = 'the-session-secret-it-replaced-but-honours';

// A secret this instance would refuse to sign with, and which no policy under
// /rotated names, so a cookie signed under it stands for any forgery
const FOREIGN_SECRET = 'a-secret-belonging-to-nobody-in-this-test';

const SESSION_LABEL = 'sourcemeta/one/session';
// The key comes from the secret through HKDF, under a fixed salt and that
// label, which is what version 2 of the format says
const SESSION_SALT = 'sourcemeta/one/seal';

function sealSession(secret, payload, lifetime = 3600) {
  const issued = Math.floor(Date.now() / 1000);
  const expiry = issued + lifetime;
  const encoded = Buffer.from(JSON.stringify(payload)).toString('base64url');
  const prefix = `2.${issued}.${expiry}.${encoded}`;
  const key = Buffer.from(
    hkdfSync('sha256', secret, SESSION_SALT, SESSION_LABEL, 32)
  );
  const signature = createHmac('sha256', key).update(prefix).digest('base64url');
  return `${prefix}.${signature}`;
}

async function getWithSession(request, value) {
  // A schema rather than the collection root, since a directory listing is
  // rendered for a browser while this asks as a client would
  return request.get('/rotated/thing.json', {
    headers: { cookie: `sourcemeta_one_session=${value}` },
    maxRedirects: 0
  });
}

test.describe('Session secret rotation', () => {
  test('a session signed under the newest secret is admitted', async ({
    request
  }) => {
    const response = await getWithSession(
      request,
      sealSession(NEW_SECRET, { policy: 'rotated', subject: 'jane' })
    );
    expect(response.status()).toBe(200);
  });

  test('a session signed under the secret being replaced is still admitted', async ({
    request
  }) => {
    // The whole point of naming more than one: an operator puts the new secret
    // first and leaves the old one in place, and nobody is signed out while
    // the sessions signed under it age away
    const response = await getWithSession(
      request,
      sealSession(OLD_SECRET, { policy: 'rotated', subject: 'jane' })
    );
    expect(response.status()).toBe(200);
  });

  test('a session signed under a secret the policy does not name is refused', async ({
    request
  }) => {
    // Retiring a secret is dropping it from the list, so this is also what an
    // already-retired secret buys once the rotation is finished
    const response = await getWithSession(
      request,
      sealSession(FOREIGN_SECRET, { policy: 'rotated', subject: 'jane' })
    );
    expect(response.status()).toBe(404);
  });

  test('a session naming another policy is refused under these secrets', async ({
    request
  }) => {
    // A sealed value names the policy that minted it, so holding a secret this
    // policy accepts does not let a session be presented as another's
    const response = await getWithSession(
      request,
      sealSession(NEW_SECRET, { policy: 'keycloak', subject: 'jane' })
    );
    expect(response.status()).toBe(404);
  });

  test('an expired session is refused whichever secret signed it', async ({
    request
  }) => {
    // Rotation decides which signatures verify, never how long a value lives,
    // so a secret still in the list does not revive an expired session
    for (const secret of [NEW_SECRET, OLD_SECRET]) {
      const response = await getWithSession(
        request,
        sealSession(secret, { policy: 'rotated', subject: 'jane' }, -60)
      );
      expect(response.status()).toBe(404);
    }
  });
});
