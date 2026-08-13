import { test, expect } from '@playwright/test';

// The keycloak policy governs only /private and /archive, while /public and the
// root listing are open: SSO covering a subpath, not the whole instance.
//
// Signing in is somewhere a person goes rather than something a gated path
// hands them, so every flow here starts at the login page rather than at the
// path that turned out not to be there.

async function signIn(page) {
  await page.goto('/self/v1/auth/login');
  await page.locator('a[data-sourcemeta-ui-login="keycloak"]').click();
  await page.locator('#username').fill('jane');
  await page.locator('#password').fill('jane-password');
  await page.locator('#kc-login').click();
}

test.describe('Interactive SSO login on a subpath', () => {
  test('open areas are browsable without any login', async ({ page }) => {
    await page.goto('/');
    await expect(page).toHaveTitle(/Schemas/);
    // Nothing on an open page advertises a way in
    await expect(page.locator('a[data-sourcemeta-ui-login]')).toHaveCount(0);

    // A public collection opens directly
    await page.locator('table a', { hasText: 'public' }).first().click();
    await expect(page).toHaveURL(/\/public\/$/);
    await expect(page.locator('a[data-sourcemeta-ui-login]')).toHaveCount(0);
  });

  test('the root listing names only what an anonymous reader can open', async ({
    page
  }) => {
    await page.goto('/');

    // The gated collections are absent rather than shown as doors, so there is
    // nothing here to click into and be refused at
    await expect(
      page.locator('table a', { hasText: 'public' }).first()
    ).toBeVisible();
    await expect(page.locator('table a', { hasText: 'private' })).toHaveCount(
      0
    );
    await expect(page.locator('table a', { hasText: 'archive' })).toHaveCount(
      0
    );
  });

  test('a gated collection is not there, and says nothing else', async ({
    page
  }) => {
    const response = await page.goto('/private/');
    expect(response.status()).toBe(404);
    await expect(page).toHaveTitle('Not Found');

    // No card, no provider, no challenge: the answer a stranger gets for a
    // name this instance does not have
    await expect(page.locator('a[data-sourcemeta-ui-login]')).toHaveCount(0);
    expect(response.headers()['www-authenticate']).toBeUndefined();
  });

  test('a path that resolves to nothing is answered identically', async ({
    page
  }) => {
    const gated = await page.goto('/private/');
    const nothing = await page.goto('/private/does-not-exist/deeper');
    expect(nothing.status()).toBe(404);
    await expect(page).toHaveTitle('Not Found');

    // The ETag is the content hash, so an equal one proves an equal body
    expect(nothing.headers()['etag']).toBe(gated.headers()['etag']);
  });

  test('a name no policy governs is answered identically too', async ({
    page
  }) => {
    const gated = await page.goto('/private/');
    const invented = await page.goto('/no-such-collection/');
    expect(invented.status()).toBe(404);
    expect(invented.headers()['etag']).toBe(gated.headers()['etag']);
  });

  test('signing in lands on what the policy governs', async ({ page }) => {
    await signIn(page);

    // The login page names no return target and sends no referrer, so the
    // endpoint falls back to the first path the policy declares. Landing back
    // on the login page would mean a caller signs in and is asked to sign in
    await expect(page).toHaveURL(/\/private$/);
    await expect(page.locator('table tbody tr').first()).toBeVisible();
  });

  test('the session reaches every path the policy governs', async ({
    page
  }) => {
    await signIn(page);

    // The policy governs two collections, and the login landed on one of them
    await page.goto('/archive/');
    await expect(page).toHaveURL(/\/archive\/$/);
    await expect(page.locator('table tbody tr').first()).toBeVisible();

    // Including a schema deep under it
    const deep = await page.goto('/private/secret');
    expect(deep.status()).toBe(200);
  });

  test('a signed-in reader sees the gated collections in the root listing', async ({
    page
  }) => {
    await signIn(page);
    await page.goto('/');

    // The listing is read from the view the session resolves to, so what was
    // absent for a stranger is present here
    await expect(
      page.locator('table a', { hasText: 'private' }).first()
    ).toBeVisible();
    await expect(
      page.locator('table a', { hasText: 'archive' }).first()
    ).toBeVisible();
  });

  test('the bar offers the way in, and it goes to the login page', async ({
    page
  }) => {
    await page.goto('/');
    const control = page.locator('a[data-sourcemeta-ui-signin]');
    await expect(control).toBeVisible();
    await expect(control).toHaveText('Sign In');

    await control.click();
    await expect(page).toHaveURL(/\/self\/v1\/auth\/login$/);
    await expect(page).toHaveTitle('Sign In');
    await expect(
      page.locator('a[data-sourcemeta-ui-login="keycloak"]')
    ).toBeVisible();
  });

  test('the bar offers the way out once signed in, and it works', async ({
    page
  }) => {
    await signIn(page);
    await expect(page.locator('table tbody tr').first()).toBeVisible();

    // The way in is gone and the way out is there, which is the whole of what
    // the bar has to say about a session
    await expect(page.locator('a[data-sourcemeta-ui-signin]')).toHaveCount(0);
    const control = page.locator('button[data-sourcemeta-ui-signout]');
    await expect(control).toBeVisible();
    await expect(control).toHaveText('Sign Out');

    // Submitting it ends the session at the provider too, which is why it is
    // a form rather than a link
    await control.click();
    await page.waitForURL((url) => !url.pathname.startsWith('/private'));

    const response = await page.goto('/private/');
    expect(response.status()).toBe(404);
    await expect(page).toHaveTitle('Not Found');
    await expect(page.locator('button[data-sourcemeta-ui-signout]')).toHaveCount(
      0
    );
    await expect(page.locator('a[data-sourcemeta-ui-signin]')).toHaveCount(1);
  });
});
