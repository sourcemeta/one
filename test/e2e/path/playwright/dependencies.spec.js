import { test, expect } from '@playwright/test';

const BASE_PATH = '/v1/catalog';

test.describe('Dependencies and dependents with base path', () => {
  test('with-ref schema shows dependency on string',
    async ({ page }) => {
    await page.goto(`${BASE_PATH}/example/with-ref?tab=dependencies`);
    const dependenciesPanel = page.locator(
      '[data-sourcemeta-ui-tab-id="dependencies"]');
    await expect(dependenciesPanel).not.toHaveClass(/d-none/);

    await expect(dependenciesPanel).toContainText('1 direct dependency');
    await expect(dependenciesPanel).toContainText('0 indirect dependencies');

    const dependencyRows = dependenciesPanel.locator('tbody tr');
    await expect(dependencyRows).toHaveCount(1);
    await expect(dependencyRows.first()).toContainText(
      `${BASE_PATH}/example/string`);
  });

  test('string schema shows with-ref as a dependent',
    async ({ page }) => {
    await page.goto(`${BASE_PATH}/example/string?tab=dependents`);
    const dependentsPanel = page.locator(
      '[data-sourcemeta-ui-tab-id="dependents"]');
    await expect(dependentsPanel).not.toHaveClass(/d-none/);

    await expect(dependentsPanel).toContainText('1 direct dependent');

    const dependentRows = dependentsPanel.locator('tbody tr');
    const withRefRow = dependentRows.filter({
      hasText: `${BASE_PATH}/example/with-ref`
    });
    await expect(withRefRow).toHaveCount(1);
    await expect(withRefRow.locator('.badge')).toContainText('Direct');
  });

  test('rebased with-ref shows dependency on with-id',
    async ({ page }) => {
    await page.goto(`${BASE_PATH}/rebased/with-ref?tab=dependencies`);
    const dependenciesPanel = page.locator(
      '[data-sourcemeta-ui-tab-id="dependencies"]');
    await expect(dependenciesPanel).not.toHaveClass(/d-none/);

    await expect(dependenciesPanel).toContainText('1 direct dependency');

    const dependencyRows = dependenciesPanel.locator('tbody tr');
    await expect(dependencyRows).toHaveCount(1);
    await expect(dependencyRows.first()).toContainText(
      `${BASE_PATH}/rebased/with-id`);
  });

  test('dependency links point to correct base path URLs',
    async ({ page }) => {
    await page.goto(`${BASE_PATH}/example/with-ref?tab=dependencies`);
    const dependenciesPanel = page.locator(
      '[data-sourcemeta-ui-tab-id="dependencies"]');
    await expect(dependenciesPanel).not.toHaveClass(/d-none/);

    // The dependency cell contains a code > a element with the schema link
    const depLink = dependenciesPanel.locator('tbody code a').first();
    await expect(depLink).toHaveAttribute(
      'href', `${BASE_PATH}/example/string`);
  });

  test('dependents badge shows correct count',
    async ({ page }) => {
    await page.goto(`${BASE_PATH}/example/string`);

    const dependentsTabButton = page.locator(
      '[data-sourcemeta-ui-tab-target="dependents"]');
    const badge = dependentsTabButton.locator('.badge');
    await expect(badge).toContainText('1');
  });
});
