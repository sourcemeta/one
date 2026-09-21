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

// https://vite.dev/config/
export default defineConfig({
  base: '/one-ui/',
  plugins: [react(), tailwindcss(), inlineLinkedStylesheets()],
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
