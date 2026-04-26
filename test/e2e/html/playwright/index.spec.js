import { test, expect } from '@playwright/test';

test.describe('Home page directory listing', () => {
  test.beforeEach(async ({ page }) => {
    await page.goto('/');
  });

  test('renders a "Special directories" heading', async ({ page }) => {
    const heading = page.getByRole('heading', { name: 'Special directories' });
    await expect(heading).toBeVisible();
  });

  test('lists /self under the special directories table', async ({ page }) => {
    const specialTable = page.locator(
      'h6:has-text("Special directories") + table');
    const selfLink = specialTable.locator('a[href="/self/"]');
    await expect(selfLink).toBeVisible();
  });

  test('does not list /self in the regular directories table',
    async ({ page }) => {
    const regularTable = page.locator('table').first();
    const selfLink = regularTable.locator('a[href="/self/"]');
    await expect(selfLink).toHaveCount(0);
  });

  test('lists /test in the regular directories table, not the special one',
    async ({ page }) => {
    const regularTable = page.locator('table').first();
    const testLink = regularTable.locator('a[href="/test/"]');
    await expect(testLink).toBeVisible();

    const specialTable = page.locator(
      'h6:has-text("Special directories") + table');
    const testInSpecial = specialTable.locator('a[href="/test/"]');
    await expect(testInSpecial).toHaveCount(0);
  });

  test('can navigate to /self/v1/schemas and see configuration schemas',
    async ({ page }) => {
    await page.goto('/self/v1/schemas/');
    const heading = page.locator('td a[href*="configuration"]');
    await expect(heading).toBeVisible();
  });
});
