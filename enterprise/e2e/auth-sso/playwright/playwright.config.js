import { defineConfig } from '@playwright/test';

// See https://playwright.dev/docs/test-configuration
export default defineConfig({
  testDir: '.',
  fullyParallel: false,
  forbidOnly: !!process.env.CI,
  retries: process.env.CI ? 2 : 0,
  workers: 1,
  reporter: 'list',
  outputDir: '../../../../build/test-results',
  use: {
    baseURL: process.env.PLAYWRIGHT_BASE_URL,
    trace: 'on-first-retry',
    // Keycloak advertises itself as `keycloak:8080` (its KC_HOSTNAME), so the
    // browser must resolve that container name to the mapped local port to
    // follow the OIDC redirect, exactly as a developer would via /etc/hosts
    launchOptions: {
      args: ['--host-resolver-rules=MAP keycloak 127.0.0.1']
    }
  }
});
