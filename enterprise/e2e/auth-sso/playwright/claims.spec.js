import { test, expect } from '@playwright/test';

// The team policy admits a person the provider places in a group, and the corp
// policy admits one whose verified address sits at a domain. Jane satisfies
// both, Bob authenticates perfectly well and satisfies neither.

async function signIn(page, policy, username, password) {
  await page.locator(`a[data-sourcemeta-ui-login="${policy}"]`).click();
  await page.locator('#username').fill(username);
  await page.locator('#password').fill(password);
  await page.locator('#kc-login').click();
}

test.describe('Admission by the claims a provider asserts', () => {
  test('a member of the group reaches what the group gates', async ({
    page,
  }) => {
    const denied = await page.goto('/team/');
    expect(denied.status()).toBe(401);
    await expect(page).toHaveTitle('Sign In');

    await signIn(page, 'team', 'jane', 'jane-password');
    await expect(page).toHaveURL(/\/team\/$/);
    await expect(page.locator('table a', { hasText: 'roster' })).toBeVisible();

    // The session admits its holder on every later request without asking the
    // provider again
    const again = await page.goto('/team/roster');
    expect(again.status()).toBe(200);
  });

  test('somebody outside the group is told so rather than looped', async ({
    page,
  }) => {
    await page.goto('/team/');
    await signIn(page, 'team', 'bob', 'bob-password');

    // Authenticated by the provider, refused by the policy, which is a
    // different answer from being refused a login
    await expect(page.locator('body')).toContainText(
      'This account is not admitted here',
    );

    // No session was minted, so the gated path is still closed, and asking for
    // it again shows the login rather than bouncing back to the provider
    const denied = await page.goto('/team/');
    expect(denied.status()).toBe(401);
    await expect(page).toHaveTitle('Sign In');
  });

  test('an address at the domain reaches what the domain gates', async ({
    page,
  }) => {
    await page.goto('/corp/');
    await signIn(page, 'corp', 'jane', 'jane-password');
    await expect(page).toHaveURL(/\/corp\/$/);
    await expect(page.locator('table a', { hasText: 'policy' })).toBeVisible();
  });

  test('an address at another domain is refused', async ({ page }) => {
    await page.goto('/corp/');
    await signIn(page, 'corp', 'bob', 'bob-password');
    await expect(page.locator('body')).toContainText(
      'This account is not admitted here',
    );

    const denied = await page.goto('/corp/');
    expect(denied.status()).toBe(401);
    await expect(page).toHaveTitle('Sign In');
  });

  test('a policy naming no rule still admits whoever signs in', async ({
    page,
  }) => {
    await page.goto('/private/');
    await signIn(page, 'keycloak', 'bob', 'bob-password');
    await expect(page).toHaveURL(/\/private\/$/);
  });
});
