import { test, expect } from '@playwright/test';

const BASE_PATH = '/v1/catalog';

test.describe('Navigation with base path', () => {
  test('root page loads with CSS and correct structure', async ({ page }) => {
    await page.goto(BASE_PATH);
    await expect(page).toHaveTitle(/Schemas/);

    // CSS loaded (navbar should have styling)
    const navbar = page.locator('nav.navbar');
    await expect(navbar).toBeVisible();

    // Directory entries are present
    const rows = page.locator('table tbody tr');
    const count = await rows.count();
    expect(count).toBeGreaterThanOrEqual(2);
    await expect(rows.first()).toContainText('example');
  });

  test('clicking a directory navigates within the base path', async ({ page }) => {
    await page.goto(BASE_PATH);

    await page.locator('a', { hasText: 'example' }).first().click();
    await expect(page).toHaveURL(new RegExp(`${BASE_PATH}/example/`));

    // The directory page should list schemas
    const rows = page.locator('table tbody tr');
    const count = await rows.count();
    expect(count).toBeGreaterThan(0);
  });

  test('clicking a schema navigates to the schema page', async ({ page }) => {
    await page.goto(`${BASE_PATH}/example/`);

    await page.locator('a', { hasText: 'string' }).first().click();
    await expect(page).toHaveURL(new RegExp(`${BASE_PATH}/example/string`));

    // The schema page should have a code editor area
    await expect(page.locator('[data-sourcemeta-ui-editor]')).toBeVisible();
  });

  test('breadcrumb home link navigates to base path root', async ({ page }) => {
    await page.goto(`${BASE_PATH}/example/string`);

    // The breadcrumb back arrow
    const homeLink = page.locator('nav[aria-label="breadcrumb"] a').first();
    await expect(homeLink).toHaveAttribute('href', `${BASE_PATH}/`);

    await homeLink.click();
    await expect(page).toHaveURL(new RegExp(`${BASE_PATH}/$`));
  });

  test('breadcrumb directory link works', async ({ page }) => {
    await page.goto(`${BASE_PATH}/example/string`);

    const breadcrumbLinks = page.locator('nav[aria-label="breadcrumb"] a');
    const exampleLink = breadcrumbLinks.filter({ hasText: 'example' });
    await expect(exampleLink).toHaveAttribute(
      'href', `${BASE_PATH}/example/`);

    await exampleLink.click();
    await expect(page).toHaveURL(new RegExp(`${BASE_PATH}/example/`));
  });

  test('renders a "Special directories" heading on the root page',
    async ({ page }) => {
    await page.goto(BASE_PATH);
    const heading = page.getByRole('heading', { name: 'Special directories' });
    await expect(heading).toBeVisible();
  });

  test('lists /self under the special directories table on the root page',
    async ({ page }) => {
    await page.goto(BASE_PATH);
    const specialTable = page.locator(
      'h6:has-text("Special directories") + table');
    const selfLink = specialTable.locator(`a[href="${BASE_PATH}/self/"]`);
    await expect(selfLink).toBeVisible();
  });

  test('does not list /self in the regular directories table on the root page',
    async ({ page }) => {
    await page.goto(BASE_PATH);
    const regularTable = page.locator('table').first();
    const selfLink = regularTable.locator(`a[href="${BASE_PATH}/self/"]`);
    await expect(selfLink).toHaveCount(0);
  });

  test('static assets load correctly', async ({ page }) => {
    const responses = [];
    page.on('response', (response) => {
      if (response.url().includes('/self/static/')) {
        responses.push({
          url: response.url(),
          status: response.status()
        });
      }
    });

    await page.goto(BASE_PATH);

    // CSS and JS should have loaded with 200
    const cssResponse = responses.find(
      (response) => response.url.includes('style.min.css'));
    expect(cssResponse).toBeDefined();
    expect(cssResponse.status).toBe(200);

    const jsResponse = responses.find(
      (response) => response.url.includes('main.min.js'));
    expect(jsResponse).toBeDefined();
    expect(jsResponse.status).toBe(200);
  });
});
