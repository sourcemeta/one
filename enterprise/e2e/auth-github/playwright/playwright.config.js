import { defineConfig, devices } from '@playwright/test';

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
    // The deployment's certificate chains to a sandbox-local authority the
    // browser does not know, so certificate errors are tolerated here while the
    // registry container verifies the chain for real
    ignoreHTTPSErrors: true
  },
  // Chromium only: the redirect chain relies on a Chromium-specific host
  // resolver rule, so the suite never runs under Firefox or WebKit
  projects: [
    {
      name: 'chromium',
      use: {
        ...devices['Desktop Chrome'],
        // The deployment is served under `github:9443`, so the browser must
        // resolve that container name to the mapped local port to follow the
        // redirect, exactly as a developer would via /etc/hosts
        launchOptions: {
          args: ['--host-resolver-rules=MAP github 127.0.0.1']
        }
      }
    }
  ]
});
