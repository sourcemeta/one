import { test, expect } from '@playwright/test';

// Chain: test/bundling/double -> test/bundling/single -> test/v2.0/schema
test.describe('Dependencies and Dependents relationship', () => {
  test('single has a direct dependency on v2.0/schema visible in its ' +
    'dependencies tab, and v2.0/schema lists single as a direct dependent',
    async ({ page }) => {
    // Navigate to test/bundling/single and open the dependencies tab
    await page.goto('/test/bundling/single?tab=dependencies');
    const dependenciesPanel = page.locator(
      '[data-sourcemeta-ui-tab-id="dependencies"]');
    await expect(dependenciesPanel).not.toHaveClass(/d-none/);

    // It should show 1 direct dependency and 0 indirect
    await expect(dependenciesPanel).toContainText('1 direct dependency');
    await expect(dependenciesPanel).toContainText('0 indirect dependencies');

    // The dependency table should list v2.0/schema
    const dependencyRows = dependenciesPanel.locator('tbody tr');
    await expect(dependencyRows).toHaveCount(1);
    await expect(dependencyRows.first()).toContainText('/test/v2.0/schema');

    // Now navigate to that dependency and check its dependents tab
    await page.goto('/test/v2.0/schema?tab=dependents');
    const dependentsPanel = page.locator(
      '[data-sourcemeta-ui-tab-id="dependents"]');
    await expect(dependentsPanel).not.toHaveClass(/d-none/);

    // single should appear as a direct dependent
    await expect(dependentsPanel).toContainText('1 direct dependent');
    const dependentRows = dependentsPanel.locator('tbody tr');
    const singleRow = dependentRows.filter({
      hasText: '/test/bundling/single'
    });
    await expect(singleRow).toHaveCount(1);
    await expect(singleRow.locator('.badge')).toContainText('Direct');
  });

  test('double has direct and indirect dependencies, and single lists ' +
    'double as a direct dependent',
    async ({ page }) => {
    // Navigate to test/bundling/double and open the dependencies tab
    await page.goto('/test/bundling/double?tab=dependencies');
    const dependenciesPanel = page.locator(
      '[data-sourcemeta-ui-tab-id="dependencies"]');
    await expect(dependenciesPanel).not.toHaveClass(/d-none/);

    // double has 2 direct dependencies (single + draft-07 metaschema)
    // and 1 indirect dependency (v2.0/schema via single)
    await expect(dependenciesPanel).toContainText('2 direct dependencies');
    await expect(dependenciesPanel).toContainText('1 indirect dependency');

    // The dependency table should contain single as a direct dependency
    const directRows = dependenciesPanel.locator('tbody tr').filter({
      hasText: '/test/bundling/single'
    });
    await expect(directRows).toHaveCount(1);
    // The row should contain the JSON Pointer, not the "Indirect" badge
    await expect(directRows).toContainText('/properties/foo/$ref');

    // The table should also contain v2.0/schema as indirect
    const indirectRows = dependenciesPanel.locator('tbody tr').filter({
      hasText: '/test/v2.0/schema'
    });
    await expect(indirectRows).toHaveCount(1);
    await expect(indirectRows).toContainText('Indirect');

    // Now navigate to single and check that double is listed as a dependent
    await page.goto('/test/bundling/single?tab=dependents');
    const dependentsPanel = page.locator(
      '[data-sourcemeta-ui-tab-id="dependents"]');
    await expect(dependentsPanel).not.toHaveClass(/d-none/);

    // double should appear as a direct dependent of single
    const doubleRow = dependentsPanel.locator('tbody tr').filter({
      hasText: '/test/bundling/double'
    });
    await expect(doubleRow).toHaveCount(1);
    await expect(doubleRow.locator('.badge')).toContainText('Direct');
  });

  test('v2.0/schema shows transitive dependents with correct types',
    async ({ page }) => {
    await page.goto('/test/v2.0/schema?tab=dependents');
    const dependentsPanel = page.locator(
      '[data-sourcemeta-ui-tab-id="dependents"]');
    await expect(dependentsPanel).not.toHaveClass(/d-none/);

    // single is a direct dependent, while double, with-rebase, and
    // with-rebase-same-host are indirect (they depend on single, not
    // directly on v2.0/schema)
    await expect(dependentsPanel).toContainText('1 direct dependent');
    await expect(dependentsPanel).toContainText('3 indirect dependents');

    const dependentRows = dependentsPanel.locator('tbody tr');
    await expect(dependentRows).toHaveCount(4);

    // single is the direct dependent
    const singleRow = dependentRows.filter({
      hasText: '/test/bundling/single'
    });
    await expect(singleRow).toHaveCount(1);
    await expect(singleRow.locator('.badge')).toContainText('Direct');

    // double is an indirect dependent
    const doubleRow = dependentRows.filter({
      hasText: '/test/bundling/double'
    });
    await expect(doubleRow).toHaveCount(1);
    await expect(doubleRow.locator('.badge')).toContainText('Indirect');
  });

  test('dependents tab badge shows the correct deduplicated count',
    async ({ page }) => {
    await page.goto('/test/v2.0/schema');

    const dependentsTabButton = page.locator(
      '[data-sourcemeta-ui-tab-target="dependents"]');
    // 1 direct + 3 indirect = 4 unique dependents
    const badge = dependentsTabButton.locator('.badge');
    await expect(badge).toContainText('4');
  });
});
