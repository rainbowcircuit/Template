import esbuild from 'esbuild';
import { readFileSync, writeFileSync, mkdirSync } from 'node:fs';
import { fileURLToPath } from 'node:url';
import path from 'node:path';

const rootDir = path.dirname(fileURLToPath(import.meta.url));
const publicDir = path.join(rootDir, 'public');
const distDir = path.join(rootDir, 'dist');

mkdirSync(distDir, { recursive: true });

await esbuild.build({
    entryPoints: [path.join(publicDir, 'ui', 'app.js')],
    outfile: path.join(distDir, 'app.bundle.js'),
    bundle: true,
    format: 'esm',
    target: 'es2020',
    minify: true,
    legalComments: 'none',
});

const html = readFileSync(path.join(publicDir, 'index.html'), 'utf8')
    .replace('<script type="module" src="./ui/app.js"></script>',
              '<script type="module" src="./app.bundle.js"></script>');

writeFileSync(path.join(distDir, 'index.html'), html);

console.log('TemplatePlugin UI bundled into', distDir);
