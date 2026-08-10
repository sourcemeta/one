import { test, expect } from '@playwright/test';

// A schema page ships a shell and loads its graph panels itself, after the page
// has rendered, from endpoints governed exactly as the schema is. So the panels
// only fill in if those requests carry the session the navigation carried.
//
// This is worth pinning rather than assuming, because the page is written this
// way deliberately: keeping the caller-dependent parts out of the built page is
// what lets one page serve everybody. If the requests ever stopped carrying the
// session, the page would still render and only the panels would fail, which is
// the kind of breakage that reaches production.
//
// Both directions are driven, because they are separate requests to separate
// endpoints and only one of them is a function of the schema's own bytes.

async function signIn(page) {
  await page.locator('a[data-sourcemeta-ui-login="keycloak"]').click();
  await page.locator('#username').fill('jane');
  await page.locator('#password').fill('jane-password');
  await page.locator('#kc-login').click();
}

test.describe('Schema page panels under a session', () => {
  test('the outgoing graph fills in for a signed-in reader', async ({
    page
  }) => {
    await page.goto('/private/secret');
    await expect(page).toHaveTitle('Sign In');
    await signIn(page);
    await expect(page).toHaveURL(/\/private\/secret$/);

    await page.goto('/private/secret?tab=dependencies');
    const panel = page.locator('[data-sourcemeta-ui-tab-id="dependencies"]');
    await expect(panel).not.toHaveClass(/d-none/);

    // The panel reports rather than fails, so the request behind it was
    // admitted rather than refused
    await expect(panel).not.toContainText('Failed to load dependencies.');
    await expect(panel).toContainText('1 direct dependency');
    await expect(panel).toContainText('0 indirect dependencies');

    // And it carries the answer itself, so what arrived was the graph rather
    // than merely a status code
    const rows = panel.locator('tbody tr');
    await expect(rows).toHaveCount(1);
    await expect(rows.first()).toContainText('/public/string');
    await expect(rows.first()).toContainText('/properties/label/$ref');
  });

  test('the incoming graph fills in for a signed-in reader', async ({
    page
  }) => {
    await page.goto('/private/secret');
    await signIn(page);
    await expect(page).toHaveURL(/\/private\/secret$/);

    await page.goto('/private/secret?tab=dependents');
    const panel = page.locator('[data-sourcemeta-ui-tab-id="dependents"]');
    await expect(panel).not.toHaveClass(/d-none/);

    await expect(panel).not.toContainText('Failed to load dependents.');
    await expect(panel).toContainText('1 direct dependent');

    // The schema pointing at this one is governed too, so naming it is
    // something only an admitted reader may be told
    const row = panel.locator('tbody tr').filter({
      hasText: '/archive/record'
    });
    await expect(row).toHaveCount(1);
    await expect(row.locator('.badge')).toContainText('Direct');
  });

  // The control for both of the above. They assert that a panel filled in, and
  // that means nothing unless filling in was something the session bought. Each
  // endpoint is asked here without one, and refuses
  test('the same endpoints refuse a reader holding no session', async ({
    request
  }) => {
    for (const endpoint of [
      '/self/v1/api/schemas/dependencies/private/secret',
      '/self/v1/api/schemas/dependents/private/secret'
    ]) {
      const denied = await request.get(endpoint);
      expect(denied.status()).toBe(401);
      expect(await denied.json()).toEqual({
        type: 'urn:sourcemeta:one:authentication-required',
        title: 'Unauthorized',
        status: 401,
        detail: 'This resource requires authentication'
      });
    }
  });

  // The page itself is not what is being tested, but if it stopped rendering
  // for a signed-in reader the panels above would fail for a reason that has
  // nothing to do with what they are pinning
  test('the page renders before its panels load', async ({ page }) => {
    await page.goto('/private/secret');
    await signIn(page);
    await expect(page).toHaveURL(/\/private\/secret$/);
    await expect(
      page.locator('[data-sourcemeta-ui-tab-target="dependencies"]')
    ).toBeVisible();
    await expect(
      page.locator('[data-sourcemeta-ui-tab-target="dependents"]')
    ).toBeVisible();
  });
});
