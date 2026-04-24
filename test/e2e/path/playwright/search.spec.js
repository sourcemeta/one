import { test, expect } from '@playwright/test';

const BASE_PATH = '/v1/catalog';

test.describe('Search UI with base path', () => {
  test.beforeEach(async ({ page }) => {
    await page.goto(BASE_PATH);
  });

  test('search input is present', async ({ page }) => {
    const searchInput = page.locator('#search');
    await expect(searchInput).toBeVisible();
  });

  test('typing shows search results with correct paths', async ({ page }) => {
    const searchInput = page.locator('#search');
    const searchResult = page.locator('#search-result');

    await searchInput.fill('string');
    await expect(searchResult).not.toHaveClass(/d-none/, { timeout: 1000 });

    const results = searchResult.locator('.list-group-item');
    await expect(results).toHaveCount(1);

    // Results should have paths with the base path prefix
    const firstResult = results.nth(0);
    await expect(firstResult).toContainText(BASE_PATH);
    await expect(firstResult).toHaveAttribute(
      'href', /\/v1\/catalog\//);
  });

  test('clicking a search result navigates within base path',
    async ({ page }) => {
    const searchInput = page.locator('#search');
    const searchResult = page.locator('#search-result');

    await searchInput.fill('string');
    await expect(searchResult).not.toHaveClass(/d-none/, { timeout: 1000 });

    const results = searchResult.locator('.list-group-item:not(.disabled)');
    await results.first().click();

    await expect(page).toHaveURL(new RegExp(`${BASE_PATH}/`));
  });

  test('no results shows message', async ({ page }) => {
    const searchInput = page.locator('#search');
    const searchResult = page.locator('#search-result');

    await searchInput.fill('xxxxxxxxxxxx');
    await expect(searchResult).not.toHaveClass(/d-none/, { timeout: 1000 });

    const noResults = searchResult.locator('.list-group-item');
    await expect(noResults).toHaveCount(1);
    await expect(noResults).toContainText('No results');
  });
});
