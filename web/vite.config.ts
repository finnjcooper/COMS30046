import { defineConfig } from 'vite'
import react from '@vitejs/plugin-react'
import tailwindcss from '@tailwindcss/vite'
import path from 'node:path'

// https://vite.dev/config/
export default defineConfig({
  base: '/risc-v-sim/',
  plugins: [react(), tailwindcss()],
  resolve: {
    alias: {
      '@components': path.resolve(import.meta.dirname, 'src/components'),
      '@hooks': path.resolve(import.meta.dirname, 'src/hooks'),
      '@styles': path.resolve(import.meta.dirname, 'src/styles'),
    },
  },
})
