import { copyFile, readFile } from "node:fs/promises";
import path from "node:path";
import { fileURLToPath } from "node:url";

const __dirname = path.dirname(fileURLToPath(import.meta.url));
const distHtml = path.resolve(__dirname, "../dist/index.html");
const destHtml = path.resolve(__dirname, "../../web_ui.html");
const token = "{{version}}";

const src = await readFile(distHtml, "utf8");
if (!src.includes(token)) {
  throw new Error(`copy-to-web-ui-html: missing ${token} in ${distHtml}`);
}

await copyFile(distHtml, destHtml);

const dest = await readFile(destHtml, "utf8");
if (!dest.includes(token)) {
  throw new Error(`copy-to-web-ui-html: missing ${token} in copied ${destHtml}`);
}

console.log(`Copied ${distHtml} -> ${destHtml} ({{version}} preserved)`);
