import { test, expect } from '@playwright/test';

test.describe('Schema Convert Dropdown', () => {
  test('starts collapsed and lists every dialect the schema can be asked for',
    async ({ page }) => {
    await page.goto('/test/draft3/string');

    const toggle = page.locator('[data-sourcemeta-ui-dropdown-toggle]');
    const menu = page.locator('[data-sourcemeta-ui-dropdown-menu]');

    // The menu is closed until asked for
    await expect(toggle).toHaveAttribute('aria-expanded', 'false');
    await expect(menu).not.toBeVisible();

    await toggle.click();

    await expect(toggle).toHaveAttribute('aria-expanded', 'true');
    await expect(menu).toBeVisible();

    // A Draft 3 schema can be asked for in every newer official dialect
    const items = menu.locator('a');
    await expect(items).toHaveText([
      'Draft4', 'Draft6', 'Draft7', '2019-09', '2020-12'
    ]);
    await expect(items.nth(0)).toHaveAttribute(
      'href', '/test/draft3/string.json?as=draft4');
    await expect(items.nth(1)).toHaveAttribute(
      'href', '/test/draft3/string.json?as=draft6');
    await expect(items.nth(2)).toHaveAttribute(
      'href', '/test/draft3/string.json?as=draft7');
    await expect(items.nth(3)).toHaveAttribute(
      'href', '/test/draft3/string.json?as=2019-09');
    await expect(items.nth(4)).toHaveAttribute(
      'href', '/test/draft3/string.json?as=2020-12');
  });

  test('offers the dialect the schema already declares', async ({ page }) => {
    await page.goto('/test/object');

    const toggle = page.locator('[data-sourcemeta-ui-dropdown-toggle]');
    const menu = page.locator('[data-sourcemeta-ui-dropdown-menu]');
    await toggle.click();

    const items = menu.locator('a');
    await expect(items).toHaveText([ '2020-12' ]);
    await expect(items.nth(0)).toHaveAttribute(
      'href', '/test/object.json?as=2020-12');
  });

  test('the menu sits below the button it belongs to', async ({ page }) => {
    await page.goto('/test/draft3/string');

    const toggle = page.locator('[data-sourcemeta-ui-dropdown-toggle]');
    const menu = page.locator('[data-sourcemeta-ui-dropdown-menu]');
    await toggle.click();

    const toggleBox = await toggle.boundingBox();
    const menuBox = await menu.boundingBox();
    expect(menuBox.y).toBeGreaterThanOrEqual(toggleBox.y + toggleBox.height);
    expect(menuBox.x).toBe(toggleBox.x);
  });

  test('clicking outside the dropdown closes it', async ({ page }) => {
    await page.goto('/test/draft3/string');

    const toggle = page.locator('[data-sourcemeta-ui-dropdown-toggle]');
    const menu = page.locator('[data-sourcemeta-ui-dropdown-menu]');
    await toggle.click();
    await expect(menu).toBeVisible();

    await page.locator('footer').click();

    await expect(toggle).toHaveAttribute('aria-expanded', 'false');
    await expect(menu).not.toBeVisible();
  });

  test('pressing escape closes it', async ({ page }) => {
    await page.goto('/test/draft3/string');

    const toggle = page.locator('[data-sourcemeta-ui-dropdown-toggle]');
    const menu = page.locator('[data-sourcemeta-ui-dropdown-menu]');
    await toggle.click();
    await expect(menu).toBeVisible();

    await page.keyboard.press('Escape');

    await expect(toggle).toHaveAttribute('aria-expanded', 'false');
    await expect(menu).not.toBeVisible();
  });

  test('clicking the button again closes it', async ({ page }) => {
    await page.goto('/test/draft3/string');

    const toggle = page.locator('[data-sourcemeta-ui-dropdown-toggle]');
    const menu = page.locator('[data-sourcemeta-ui-dropdown-menu]');
    await toggle.click();
    await expect(menu).toBeVisible();

    await toggle.click();

    await expect(toggle).toHaveAttribute('aria-expanded', 'false');
    await expect(menu).not.toBeVisible();
  });
});
