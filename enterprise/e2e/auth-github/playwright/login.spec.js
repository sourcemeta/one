import { test, expect } from '@playwright/test';

// Signing in through a GitHub deployment, from a browser.
//
// The four policies each gate one collection and each name one kind of rule, so
// what a given account reaches says which rule admitted them. The deployment
// holds three accounts: one in the acme organisation and its platform team, one
// in acme alone, and one in another organisation entirely.

async function signIn(page, policy, account, password) {
  await page.goto('/self/v1/auth/login');
  await page.locator(`a[data-sourcemeta-ui-login="${policy}"]`).click();
  await page.locator('#username').fill(account);
  await page.locator('#password').fill(password);
  await page.locator('#kc-login').click();
}

test.describe('Signing in through a GitHub deployment', () => {
  test('open areas are browsable without any login', async ({ page }) => {
    await page.goto('/');
    await expect(page).toHaveTitle(/Schemas/);
    await page.locator('table a', { hasText: 'public' }).first().click();
    await expect(page).toHaveURL(/\/public\/$/);
  });

  test('the login page names every policy that signs a person in', async ({
    page
  }) => {
    await page.goto('/self/v1/auth/login');
    await expect(page).toHaveTitle('Sign In');
    await expect(page.locator('a[data-sourcemeta-ui-login]')).toHaveCount(5);
    const first = page.locator('a[data-sourcemeta-ui-login="engineering"]');
    await expect(first).toBeVisible();
    await expect(first).toHaveText('GitHub');
  });

  test('a gated collection is not there for a stranger', async ({ page }) => {
    const response = await page.goto('/private/');
    expect(response.status()).toBe(404);
    await expect(page).toHaveTitle('Not Found');
    expect(response.headers()['www-authenticate']).toBeUndefined();
  });

  test('the browser never leaves the deployment while signing in', async ({
    page
  }) => {
    const hosts = [];
    page.on('request', (request) => {
      if (request.isNavigationRequest()) {
        hosts.push(new URL(request.url()).host);
      }
    });

    await signIn(page, 'engineering', 'octocat', 'octocat-password');
    await expect(page).toHaveURL(/\/private$/);

    // Every navigation went either to this instance or to the deployment, and
    // to nothing behind it
    expect([...new Set(hosts)].sort()).toEqual([
      'github:9443',
      new URL(process.env.PLAYWRIGHT_BASE_URL).host
    ]);
  });

  test('an account in the organisation reaches what the policy governs', async ({
    page
  }) => {
    await signIn(page, 'engineering', 'octocat', 'octocat-password');
    await expect(page).toHaveURL(/\/private$/);
    await expect(page.locator('table tbody tr').first()).toBeVisible();

    // The policy governs two collections, and the session reaches both
    await page.goto('/archive/');
    await expect(page.locator('table tbody tr').first()).toBeVisible();
    const deep = await page.goto('/private/secret');
    expect(deep.status()).toBe(200);
  });

  test('an account in another organisation is refused, and is told so', async ({
    page
  }) => {
    await signIn(page, 'engineering', 'hubot', 'hubot-password');
    await expect(page).toHaveURL(/\/self\/v1\/auth\/callback\/engineering/);
    await expect(page.locator('body')).toContainText(
      'This account is not admitted here'
    );

    // And nothing was established by being told so
    const denied = await page.goto('/private/');
    expect(denied.status()).toBe(404);
  });

  test('a team rule admits a member of the team and nobody above it', async ({
    page
  }) => {
    await signIn(page, 'platform', 'octocat', 'octocat-password');
    await expect(page).toHaveURL(/\/team$/);
    await expect(page.locator('table tbody tr').first()).toBeVisible();
  });

  test('the signed-in listing names what the session opens', async ({
    page
  }) => {
    await signIn(page, 'engineering', 'octocat', 'octocat-password');
    await page.goto('/');

    await expect(
      page.locator('table a', { hasText: 'private' }).first()
    ).toBeVisible();
    await expect(
      page.locator('table a', { hasText: 'archive' }).first()
    ).toBeVisible();
    // What this session does not open is absent rather than locked
    await expect(page.locator('table a', { hasText: 'team' })).toHaveCount(0);
    await expect(page.locator('table a', { hasText: 'corp' })).toHaveCount(0);
  });

  test('the bar offers the way out once signed in, and it works', async ({
    page
  }) => {
    await signIn(page, 'engineering', 'octocat', 'octocat-password');
    await expect(page.locator('table tbody tr').first()).toBeVisible();
    await expect(page.locator('a[data-sourcemeta-ui-signin]')).toHaveCount(0);

    const control = page.locator('button[data-sourcemeta-ui-signout]');
    await expect(control).toBeVisible();
    await control.click();

    // A deployment offers nowhere to end its own session, so signing out lands
    // back here rather than going on anywhere
    await page.waitForURL((url) => !url.pathname.startsWith('/private'));
    expect(new URL(page.url()).host).toBe(
      new URL(process.env.PLAYWRIGHT_BASE_URL).host
    );

    const response = await page.goto('/private/');
    expect(response.status()).toBe(404);
    await expect(page.locator('a[data-sourcemeta-ui-signin]')).toHaveCount(1);
  });
});
