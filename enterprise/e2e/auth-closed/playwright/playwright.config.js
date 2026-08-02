import { defineConfig, devices } from '@playwright/test';

// See https://playwright.dev/docs/test-configuration
// Chromium is told to resolve the sandbox's container names, but requests made
// outside the browser go through Node, which has no equivalent, so they dial
// the mapped port directly. Either way the certificate chains to a
// sandbox-local authority, which is what `ignoreHTTPSErrors` below tolerates
const target = (process.env.PLAYWRIGHT_BASE_URL ?? '').replace(
  '//registry:',
  '//localhost:'
);

export default defineConfig({
  testDir: '.',
  fullyParallel: false,
  forbidOnly: !!process.env.CI,
  retries: process.env.CI ? 2 : 0,
  workers: 1,
  reporter: 'list',
  outputDir: '../../../../build/test-results',
  use: {
    baseURL: target,
    trace: 'on-first-retry',
    // The identity provider's certificate chains to a sandbox-local authority
    // the browser does not know, so certificate errors are tolerated here
    // while the registry container verifies the chain for real
    ignoreHTTPSErrors: true
  },
  // Chromium only: the OIDC redirect chain relies on a Chromium-specific
  // host resolver rule, so the suite never runs under Firefox or WebKit
  projects: [
    {
      name: 'chromium',
      use: {
        ...devices['Desktop Chrome'],
        // Keycloak advertises itself as `keycloak:8443` (its KC_HOSTNAME) and
        // the registry as `registry:8000`, so the browser must resolve both
        // container names to the mapped local ports, exactly as a developer
        // would via /etc/hosts
        launchOptions: {
          args: ['--host-resolver-rules=MAP keycloak 127.0.0.1, MAP registry 127.0.0.1']
        }
      }
    }
  ]
});
