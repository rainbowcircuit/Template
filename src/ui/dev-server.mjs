// Static file server for Debug builds: serves public/ as-is (no bundling) so
// edits to app.js and friends are picked up on reload. lit/three are resolved
// via the import map in public/index.html (esm.sh), not from node_modules.
import esbuild from 'esbuild';
import path from 'node:path';
import { fileURLToPath } from 'node:url';

const rootDir = path.dirname(fileURLToPath(import.meta.url));
const publicDir = path.join(rootDir, 'public');
const port = 4000;

const ctx = await esbuild.context({});
const { host } = await ctx.serve({ servedir: publicDir, port });

console.log(`TemplatePlugin UI dev server running at http://${host}:${port}`);
console.log(`Serving ${publicDir} unbundled; lit/three fetched from esm.sh via import map.`);
