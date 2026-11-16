import { defineConfig } from "vite";
import { svelte } from "@sveltejs/vite-plugin-svelte";
import tailwindcss from "@tailwindcss/vite";
import { viteSingleFile } from "vite-plugin-singlefile";
import { VitePWA } from "vite-plugin-pwa";

export default defineConfig({
  plugins: [
    svelte(),
    tailwindcss(),
    viteSingleFile(),
    VitePWA({
      registerType: "autoUpdate",
      includeAssets: ["favicon.ico", "apple-touch-icon.png"],
      manifest: {
        name: "Curling Timer",
        short_name: "Curling",
        description: "Live countdown, schedule, and relay control for curling clubs",
        display: "standalone",
        orientation: "portrait-primary",
        theme_color: "#06141f",
        background_color: "#06141f",
        start_url: "/",
        scope: "/",
        icons: [
          { src: "/icons/icon-192.png", sizes: "192x192", type: "image/png", purpose: "any" },
          { src: "/icons/icon-512.png", sizes: "512x512", type: "image/png", purpose: "any" },
          { src: "/icons/icon-512-maskable.png", sizes: "512x512", type: "image/png", purpose: "maskable" },
        ],
        categories: ["sports", "utilities"],
      },
      workbox: {
        globPatterns: ["**/*.{js,css,html,ico,png,svg,woff2}"],
        runtimeCaching: [
          { urlPattern: /^https?.*\/api\/.*$/, handler: "NetworkFirst", options: { networkTimeoutSeconds: 5, cacheName: "api-cache", expiration: { maxEntries: 50, maxAgeSeconds: 60 * 60 * 24 } } },
          { urlPattern: /^https?.*\.(png|jpg|jpeg|svg|gif|webp|woff2)$/, handler: "CacheFirst", options: { cacheName: "static-assets", expiration: { maxEntries: 100, maxAgeSeconds: 60 * 60 * 24 * 30 } } },
        ],
      },
      devOptions: { enabled: true, type: "module" },
    }),
  ],
  build: {
    outDir: "dist",
    assetsInlineLimit: 100000000,
    cssCodeSplit: false,
  },
  server: {
    proxy: {
      "/api": {
        target: process.env.VITE_API_TARGET || "http://localhost:8080",
        changeOrigin: true,
      },
    },
  },
});
