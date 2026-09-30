import react from '@vitejs/plugin-react'
import tailwindcss from '@tailwindcss/vite'
import { defineConfig, type Plugin } from 'vite'

// Folds every stylesheet index.html links into an inline <style> and drops the
// now-orphaned file from the bundle. Only the entry stylesheet is linked from
// the HTML, so Monaco's jsonMode CSS keeps its own file and stays lazily
// loaded alongside the chunk that asks for it.
const inlineLinkedStylesheets = (): Plugin => ({
  name: 'one-ui-inline-linked-stylesheets',
  enforce: 'post',
  transformIndexHtml: {
    order: 'post',
    handler(html, context) {
      const bundle = context.bundle
      if (!bundle) {
        return html
      }

      let result = html
      for (const [fileName, output] of Object.entries(bundle)) {
        if (output.type !== 'asset' || !fileName.endsWith('.css')) {
          continue
        }

        const link = new RegExp(
          `<link[^>]+href="[^"]*${fileName.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')}"[^>]*>`,
        )
        if (!link.test(result)) {
          continue
        }

        result = result.replace(link, `<style>${output.source as string}</style>`)
        delete bundle[fileName]
      }

      return result
    },
  },
})

// In production this app is served BY the registry it browses, so every
// fetch it makes is same-origin — there's no separate "registry URL" to
// configure. `npm run dev` has no such registry of its own, so requests for
// actual registry data (as opposed to a page route the SPA renders itself)
// are proxied to a real one here instead, overridable via VITE_DEV_REGISTRY
// for testing against a different instance.
const devRegistryTarget = process.env.VITE_DEV_REGISTRY ?? 'https://schemas.sourcemeta.com'

// A schema path segment can itself contain a dot (a version number like
// schema_0.14, or v1.1), so "does the url contain a dot" isn't a safe way
// to tell a real static asset apart from a navigation route — this checks
// for an actual known asset extension at the end of the path instead.
const ASSET_EXTENSION = /\.(?:js|mjs|css|json|svg|png|jpe?g|gif|webp|ico|woff2?|ttf|map|wasm|txt|xml|webmanifest)$/i
const isLikelyAssetPath = (url: string): boolean =>
  ASSET_EXTENSION.test(url.split('?')[0] ?? '')

// The real server serves this same index.html for ANY path, including one
// naming a schema (see test/e2e/ui-experimental/hurl/test.all.hurl) — the
// client owns routing, not the server. Vite's dev server only knows how to
// do that for paths under `base`, so a hard refresh on a schema path (e.g.
// /nasa/gcn/.../spec) 404s instead of reaching our router. This rewrites
// any such navigation to `base` before Vite's own routing sees it, so a dev
// reload behaves like the real server does.
const devSpaFallback = (): Plugin => ({
  name: 'one-ui-dev-spa-fallback',
  configureServer(server) {
    server.middlewares.use((req, _res, next) => {
      const accept = req.headers.accept ?? ''
      if (
        req.method === 'GET' &&
        accept.includes('text/html') &&
        req.url &&
        !req.url.startsWith('/self/v1/') &&
        !req.url.startsWith('/@') &&
        !isLikelyAssetPath(req.url)
      ) {
        req.url = '/self/v1/static/'
      }
      next()
    })
  },
})

// https://vite.dev/config/
export default defineConfig({
  base: '/self/v1/static/',
  plugins: [react(), tailwindcss(), inlineLinkedStylesheets(), devSpaFallback()],
  server: {
    proxy: {
      '/self/v1/api': { target: devRegistryTarget, changeOrigin: true },
      '/self/v1/health': { target: devRegistryTarget, changeOrigin: true },
      // Direct schema content fetches (e.g. /test/example.json, or
      // /test/example.json?bundle=1 for the bundled form) — anything else
      // is a page route the SPA renders itself. The match is against the
      // full url including the query string, so `$` alone (matching only
      // an unadorned .json) misses the bundled variant. Case-insensitive
      // since the registry's own routing doesn't care about .json's case.
      '^/.*\\.[jJ][sS][oO][nN](\\?.*)?$': { target: devRegistryTarget, changeOrigin: true },
    },
  },
  build: {
    // One serves these from a single flat directory, so nesting them under
    // assets/ would only add a path segment nothing reads
    assetsDir: '',
    // The font travels inside the stylesheet that the plugin above inlines,
    // which spares the browser a round trip before it can paint text
    assetsInlineLimit: (filePath) =>
      filePath.endsWith('.woff2') ? true : undefined,
  },
})
