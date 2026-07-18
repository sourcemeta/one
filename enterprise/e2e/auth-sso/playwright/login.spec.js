import { test, expect } from '@playwright/test';

// The keycloak policy governs only /private and /archive, while /public and the
// root listing are open: SSO covering a subpath, not the whole instance.

async function signIn(page) {
  await page.locator('a[data-sourcemeta-ui-login="keycloak"]').click();
  await page.locator('#username').fill('jane');
  await page.locator('#password').fill('jane-password');
  await page.locator('#kc-login').click();
}

test.describe('Interactive SSO login on a subpath', () => {
  test('open areas are browsable without any login', async ({ page }) => {
    await page.goto('/');
    await expect(page).toHaveTitle(/Schemas/);
    // The root listing is public and never fronted by a login
    await expect(page.locator('a[data-sourcemeta-ui-login]')).toHaveCount(0);

    // A public collection opens directly
    await page.locator('table a', { hasText: 'public' }).first().click();
    await expect(page).toHaveURL(/\/public\/$/);
    await expect(page.locator('a[data-sourcemeta-ui-login]')).toHaveCount(0);
  });

  test('a gated collection shows the login card in place', async ({ page }) => {
    const response = await page.goto('/private/');
    expect(response.status()).toBe(401);
    await expect(page).toHaveTitle('Sign In');

    // A real, styled card rather than the plain denial page
    await expect(page.locator('.card')).toBeVisible();
    await expect(page.locator('h1')).toHaveText('SSO Sandbox');

    const provider = page.locator('a[data-sourcemeta-ui-login="keycloak"]');
    await expect(provider).toBeVisible();
    await expect(provider).toHaveText('keycloak');
    await expect(provider).toHaveAttribute(
      'href',
      '/self/v1/auth/login/keycloak'
    );
  });

  test('a path that resolves to nothing shows the identical login', async ({
    page
  }) => {
    await page.goto('/private/does-not-exist/deeper');
    await expect(page).toHaveTitle('Sign In');
    await expect(
      page.locator('a[data-sourcemeta-ui-login="keycloak"]')
    ).toBeVisible();
  });

  test('navigating from an open page into a gated folder and authenticating', async ({
    page
  }) => {
    await page.goto('/');
    // Click into the gated collection from the open root listing
    await page.locator('table a', { hasText: 'private' }).first().click();

    // The login is shown in place, at the gated URL
    await expect(page).toHaveURL(/\/private\//);
    await expect(page).toHaveTitle('Sign In');

    await signIn(page);

    // Back in the gated collection, now authenticated: the listing renders and
    // the login card is gone
    await expect(page).toHaveURL(/\/private\b/);
    await expect(page.locator('a[data-sourcemeta-ui-login]')).toHaveCount(0);
    await expect(page.locator('table tbody tr').first()).toBeVisible();
  });

  test('authenticating from a deep gated page returns to that exact path', async ({
    page
  }) => {
    // A specific schema deep under the policy, not the collection root
    await page.goto('/private/secret');
    await expect(page).toHaveTitle('Sign In');

    await signIn(page);

    // The login page hands the endpoint the denied path through a same-origin
    // referrer, so authentication lands back on the exact schema, not the
    // governing folder
    await expect(page).toHaveURL(/\/private\/secret$/);
    await expect(page.locator('a[data-sourcemeta-ui-login]')).toHaveCount(0);
  });

  test('logging out re-gates the collection', async ({ page }) => {
    await page.goto('/private/');
    await signIn(page);
    await expect(page.locator('table tbody tr').first()).toBeVisible();

    // Logging out clears the session, so the gated collection is fronted by the
    // login again
    await page.goto('/self/v1/auth/logout');
    await page.goto('/private/');
    await expect(page).toHaveTitle('Sign In');
    await expect(
      page.locator('a[data-sourcemeta-ui-login="keycloak"]')
    ).toBeVisible();
  });
});
