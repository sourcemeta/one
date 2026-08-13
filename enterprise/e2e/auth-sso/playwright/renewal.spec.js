import { test, expect } from '@playwright/test';

// A session lasts an hour. Rather than ask somebody to click their provider
// again when it ends, a browser that has signed in before is sent back to the
// provider to be asked whether that sign-in still stands. When it does, the
// answer arrives without anything being shown and the hour passes unnoticed.
//
// An expired session is reproduced by deleting the session cookie and leaving
// the marker, which is exactly what a browser holds once the session's own
// lifetime has run out.

const MARKER = 'sourcemeta_one_renewal';
const SESSION = 'sourcemeta_one_session';

async function signIn(page) {
  await page.goto('/self/v1/auth/login');
  await page.locator('a[data-sourcemeta-ui-login="keycloak"]').click();
  await page.locator('#username').fill('jane');
  await page.locator('#password').fill('jane-password');
  await page.locator('#kc-login').click();
}

async function cookieNamed(context, name) {
  return (await context.cookies()).find((entry) => entry.name === name);
}

async function expireSession(context) {
  const kept = (await context.cookies()).filter(
    (entry) => entry.name !== SESSION
  );
  await context.clearCookies();
  await context.addCookies(kept);
}

test.describe('Silent session renewal', () => {
  test('signing in leaves the marker that makes renewal possible', async ({
    page,
    context
  }) => {
    await signIn(page);
    await expect(page).toHaveURL(/\/private\b/);

    const marker = await cookieNamed(context, MARKER);
    expect(marker).toBeDefined();
    expect(marker.value).toBe('keycloak');
    expect(marker.httpOnly).toBe(true);
    // It is only of use once the session has expired, so it has to outlive it
    const session = await cookieNamed(context, SESSION);
    expect(marker.expires).toBeGreaterThan(session.expires);
  });

  test('an expired session renews without the person seeing anything', async ({
    page,
    context
  }) => {
    await signIn(page);
    await expect(page.locator('table tbody tr').first()).toBeVisible();

    await expireSession(context);
    expect(await cookieNamed(context, SESSION)).toBeUndefined();

    // The gated page is simply browsed to again. Nothing is shown along the
    // way, and the listing renders as though the session had never lapsed
    await page.goto('/private/');
    await expect(page).not.toHaveTitle('Not Found');
    await expect(page.locator('table tbody tr').first()).toBeVisible();
    expect(await cookieNamed(context, SESSION)).toBeDefined();
  });

  test('renewal lands on the exact page it was asked for', async ({
    page,
    context
  }) => {
    await signIn(page);
    await page.goto('/private/secret');
    await expect(page).toHaveURL(/\/private\/secret$/);

    // The renewal carries the page along, so an expired session is replaced
    // without the person losing their place
    await expireSession(context);
    await page.goto('/private/secret');
    await expect(page).toHaveURL(/\/private\/secret$/);
    await expect(page).not.toHaveTitle('Not Found');
  });

  test('a marker without a provider session gives up quietly', async ({
    page,
    context
  }) => {
    // A browser that never signed in at the provider, carrying only the
    // marker. The provider is asked and says it cannot answer without
    // interaction, which is the ordinary end of a silent attempt rather than a
    // failure, so the person is left where they were and told what anybody
    // else would be told
    await context.addCookies([
      {
        name: MARKER,
        value: 'keycloak',
        url: process.env.PLAYWRIGHT_BASE_URL
      }
    ]);

    await page.goto('/private/');
    await expect(page).toHaveTitle('Not Found');
    await expect(page.locator('a[data-sourcemeta-ui-login]')).toHaveCount(0);

    // The marker is gone, so the next attempt does not go round again. Without
    // this the browser would be sent to the provider on every navigation, for
    // an answer that is never going to change
    expect(await cookieNamed(context, MARKER)).toBeUndefined();
  });

  test('a failed renewal does not repeat itself', async ({ page, context }) => {
    await context.addCookies([
      {
        name: MARKER,
        value: 'keycloak',
        url: process.env.PLAYWRIGHT_BASE_URL
      }
    ]);

    await page.goto('/private/');
    await expect(page).toHaveTitle('Not Found');

    // A second navigation is answered directly. If the marker had survived,
    // this would be another round trip through the provider, and every
    // navigation after it too
    const responses = [];
    page.on('response', (response) => responses.push(response.url()));
    await page.goto('/private/');
    await expect(page).toHaveTitle('Not Found');
    expect(responses.filter((url) => url.includes('keycloak:8443'))).toHaveLength(
      0
    );
  });

  test('signing out stops the renewal it would otherwise trigger', async ({
    page,
    context
  }) => {
    await signIn(page);
    await expect(page.locator('table tbody tr').first()).toBeVisible();
    expect(await cookieNamed(context, MARKER)).toBeDefined();

    // Signing out is a form submit rather than a navigation, since it ends a
    // session at the provider. This is the shape the sign-out control will
    // take once the explorer renders one.
    await page.evaluate(() => {
      const form = document.createElement('form');
      form.method = 'POST';
      form.action = '/self/v1/auth/logout';
      document.body.appendChild(form);
      form.submit();
    });
    await page.waitForURL((url) => !url.pathname.startsWith('/private'));

    // The marker goes with the session. Somebody who has signed out is asking
    // not to be signed in, and leaving it would undo that at the very next
    // navigation without them doing anything
    expect(await cookieNamed(context, MARKER)).toBeUndefined();
    await page.goto('/private/');
    await expect(page).toHaveTitle('Not Found');
  });
});
