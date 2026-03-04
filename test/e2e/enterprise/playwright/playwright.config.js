import base from '../../html/playwright/playwright.config.js';
import { defineConfig } from '@playwright/test';

export default defineConfig({
  ...base,
  testDir: '../../html/playwright'
});
