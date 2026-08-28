import { test, expect } from '@playwright/test';

// What the browser is left holding after signing in through a GitHub
// deployment, which is where this differs visibly from an OpenID Connect
// provider.

test.describe('What a session through a deployment holds', () => {
  test('the session carries the account and never the credential', async ({
    page,
    context
  }) => {
    await page.goto('/self/v1/auth/login');
    await page.locator('a[data-sourcemeta-ui-login="engineering"]').click();
    await page.locator('#username').fill('octocat');
    await page.locator('#password').fill('octocat-password');
    await page.locator('#kc-login').click();
    await expect(page).toHaveURL(/\/private$/);

    const cookies = await context.cookies();
    const names = cookies
      .filter((cookie) => cookie.name.startsWith('sourcemeta_one_'))
      .map((cookie) => cookie.name);

    // A deployment cannot be asked whether a sign-in still stands without
    // showing the person its own pages, so nothing here earns a browser the
    // marker that would send it back there on its own
    expect(names).toEqual(['sourcemeta_one_session']);

    const session = cookies.find(
      (cookie) => cookie.name === 'sourcemeta_one_session'
    );
    expect(session.httpOnly).toBe(true);

    // The sealed payload names the policy and the account identifier, and
    // carries no access token: that credential reaches the person's own
    // repositories and is spent during the callback rather than stored
    const payload = JSON.parse(
      Buffer.from(session.value.split('.')[3], 'base64url').toString('utf-8')
    );
    expect(Object.keys(payload).sort()).toEqual(['policy', 'subject']);
    expect(payload.policy).toBe('engineering');
    expect(payload.subject).toMatch(/^[0-9]+$/);
  });
});
