import { test, expect } from '@playwright/test';

test.describe('Schema Tabs', () => {
  test('defaults to examples tab on load without adding query parameter',
    async ({ page }) => {
    await page.goto('/test/schemas/string');

    // The examples tab button should be active
    const examplesTab = page.locator(
      '[data-sourcemeta-ui-tab-target="examples"]');
    await expect(examplesTab).toHaveClass(/active/);

    // The examples panel should be visible
    const examplesPanel = page.locator(
      '[data-sourcemeta-ui-tab-id="examples"]');
    await expect(examplesPanel).not.toHaveClass(/d-none/);

    // Other panels should be hidden
    const dependenciesPanel = page.locator(
      '[data-sourcemeta-ui-tab-id="dependencies"]');
    await expect(dependenciesPanel).toHaveClass(/d-none/);
    const healthPanel = page.locator(
      '[data-sourcemeta-ui-tab-id="health"]');
    await expect(healthPanel).toHaveClass(/d-none/);

    // The URL should NOT have a tab query parameter
    expect(page.url()).not.toContain('tab=');
  });

  test('clicking a tab updates the query parameter and panel visibility',
    async ({ page }) => {
    await page.goto('/test/schemas/string');

    // Wait for the default tab to be active
    const examplesTab = page.locator(
      '[data-sourcemeta-ui-tab-target="examples"]');
    await expect(examplesTab).toHaveClass(/active/);

    // Click on the dependencies tab
    const dependenciesTab = page.locator(
      '[data-sourcemeta-ui-tab-target="dependencies"]');
    await dependenciesTab.click();

    // Query parameter should update
    await expect(page).toHaveURL(/tab=dependencies/);

    // The dependencies tab should be active, others not
    await expect(dependenciesTab).toHaveClass(/active/);
    await expect(examplesTab).not.toHaveClass(/active/);

    // The dependencies panel should be visible, others hidden
    const dependenciesPanel = page.locator(
      '[data-sourcemeta-ui-tab-id="dependencies"]');
    await expect(dependenciesPanel).not.toHaveClass(/d-none/);
    const examplesPanel = page.locator(
      '[data-sourcemeta-ui-tab-id="examples"]');
    await expect(examplesPanel).toHaveClass(/d-none/);

    // Click on the health tab
    const healthTab = page.locator(
      '[data-sourcemeta-ui-tab-target="health"]');
    await healthTab.click();

    await expect(page).toHaveURL(/tab=health/);
    await expect(healthTab).toHaveClass(/active/);
    await expect(dependenciesTab).not.toHaveClass(/active/);
    const healthPanel = page.locator(
      '[data-sourcemeta-ui-tab-id="health"]');
    await expect(healthPanel).not.toHaveClass(/d-none/);
    await expect(dependenciesPanel).toHaveClass(/d-none/);
  });

  test('clicking back to the first tab also sets the query parameter',
    async ({ page }) => {
    await page.goto('/test/schemas/string');

    const examplesTab = page.locator(
      '[data-sourcemeta-ui-tab-target="examples"]');
    await expect(examplesTab).toHaveClass(/active/);

    // Click away from the default tab
    const dependenciesTab = page.locator(
      '[data-sourcemeta-ui-tab-target="dependencies"]');
    await dependenciesTab.click();
    await expect(page).toHaveURL(/tab=dependencies/);

    // Click back to examples
    await examplesTab.click();

    // Now the query parameter should be present
    await expect(page).toHaveURL(/tab=examples/);
    await expect(examplesTab).toHaveClass(/active/);
    await expect(dependenciesTab).not.toHaveClass(/active/);
  });

  test('navigating with an invalid tab query parameter falls back to first tab',
    async ({ page }) => {
    await page.goto('/test/schemas/string?tab=nonexistent');

    // The first tab (examples) should be active as a fallback
    const examplesTab = page.locator(
      '[data-sourcemeta-ui-tab-target="examples"]');
    await expect(examplesTab).toHaveClass(/active/);

    // The examples panel should be visible
    const examplesPanel = page.locator(
      '[data-sourcemeta-ui-tab-id="examples"]');
    await expect(examplesPanel).not.toHaveClass(/d-none/);

    // Other panels should be hidden
    const dependenciesPanel = page.locator(
      '[data-sourcemeta-ui-tab-id="dependencies"]');
    await expect(dependenciesPanel).toHaveClass(/d-none/);
  });

  test('navigating with a tab query parameter selects that tab',
    async ({ page }) => {
    await page.goto('/test/schemas/string?tab=health');

    // The health tab should be active
    const healthTab = page.locator(
      '[data-sourcemeta-ui-tab-target="health"]');
    await expect(healthTab).toHaveClass(/active/);

    // The health panel should be visible
    const healthPanel = page.locator(
      '[data-sourcemeta-ui-tab-id="health"]');
    await expect(healthPanel).not.toHaveClass(/d-none/);

    // The URL should keep the tab parameter
    await expect(page).toHaveURL(/tab=health/);

    // Other tabs and panels should not be active/visible
    const examplesTab = page.locator(
      '[data-sourcemeta-ui-tab-target="examples"]');
    await expect(examplesTab).not.toHaveClass(/active/);
    const examplesPanel = page.locator(
      '[data-sourcemeta-ui-tab-id="examples"]');
    await expect(examplesPanel).toHaveClass(/d-none/);
  });
});

test.describe('Schema Usage Snippets', () => {
  test('copy button copies CLI snippet to clipboard',
    async ({ page, context }) => {
    await context.grantPermissions(['clipboard-read', 'clipboard-write']);
    await page.goto('/test/schemas/string');

    // The CLI tab should be active by default
    const cliTab = page.locator(
      '[data-sourcemeta-ui-tab-target="usage-cli"]');
    await expect(cliTab).toHaveClass(/active/);

    // Click the copy button in the CLI panel
    const cliPanel = page.locator(
      '[data-sourcemeta-ui-tab-id="usage-cli"]');
    const copyButton = cliPanel.locator('[data-sourcemeta-ui-copy]');
    await copyButton.click();

    const clipboardText = await page.evaluate(
      () => navigator.clipboard.readText());
    expect(clipboardText).toBe(
      'jsonschema install http://localhost:8000/test/schemas/string'
      + ' schemas/string.json');
  });

  test('copy button copies OpenAPI snippet after switching tab',
    async ({ page, context }) => {
    await context.grantPermissions(['clipboard-read', 'clipboard-write']);
    await page.goto('/test/schemas/string');

    // Switch to OpenAPI tab
    const openApiTab = page.locator(
      '[data-sourcemeta-ui-tab-target="usage-openapi"]');
    await openApiTab.click();
    await expect(openApiTab).toHaveClass(/active/);

    // Click the copy button in the OpenAPI panel
    const openApiPanel = page.locator(
      '[data-sourcemeta-ui-tab-id="usage-openapi"]');
    const copyButton = openApiPanel.locator('[data-sourcemeta-ui-copy]');
    await copyButton.click();

    const clipboardText = await page.evaluate(
      () => navigator.clipboard.readText());
    expect(clipboardText).toBe(
      '$ref: "http://localhost:8000/test/schemas/string"');
  });

  test('copy button shows check icon as feedback',
    async ({ page, context }) => {
    await context.grantPermissions(['clipboard-read', 'clipboard-write']);
    await page.goto('/test/schemas/string');

    const cliPanel = page.locator(
      '[data-sourcemeta-ui-tab-id="usage-cli"]');
    await expect(cliPanel).not.toHaveClass(/d-none/);
    const copyButton = cliPanel.locator('[data-sourcemeta-ui-copy]');
    const icon = copyButton.locator('i');

    // Icon should start as clipboard
    await expect(icon).toHaveClass(/bi-clipboard/);

    await copyButton.click();

    // Icon should change to check
    await expect(icon).toHaveClass(/bi-check/);
    await expect(icon).not.toHaveClass(/bi-clipboard/);

    // Icon should revert back after 1.5 seconds
    await expect(icon).toHaveClass(/bi-clipboard/, { timeout: 3000 });
  });
});
