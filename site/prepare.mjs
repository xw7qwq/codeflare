import { cp, mkdir, readFile, readdir, rm, writeFile } from 'node:fs/promises';
import path from 'node:path';
import { fileURLToPath } from 'node:url';
import { execFileSync } from 'node:child_process';

const siteRoot = path.dirname(fileURLToPath(import.meta.url));
const repoRoot = path.dirname(siteRoot);
const output = path.join(siteRoot, '.content');
const sourceBranch = 'docs/project-guide';
const github = 'https://github.com/xw7qwq/solutions';
const routes = new Map([
  ['README.md', 'repository.md'],
  ['CONTRIBUTING.md', 'contributing.md'],
]);

async function collect(directory) {
  for (const entry of await readdir(path.join(repoRoot, directory), { withFileTypes: true })) {
    const relative = `${directory}/${entry.name}`;
    if (entry.isDirectory()) await collect(relative);
    else if (entry.name.endsWith('.md')) {
      routes.set(relative, relative === 'docs/README.md' ? 'handbook.md' : relative.slice(5));
    }
  }
}
await collect('docs');
const schema = JSON.parse(await readFile(path.join(siteRoot, 'public/api/site-data.schema.json'), 'utf8'));

function describeType(definition) {
  if (definition.$ref) return definition.$ref.split('/').at(-1);
  if (definition.anyOf) return definition.anyOf.map(describeType).join(' / ');
  if (definition.type === 'array') return `array<${describeType(definition.items)}>`;
  return Array.isArray(definition.type) ? definition.type.join(' / ') : definition.type;
}

const recentSchema = JSON.parse(await readFile(path.join(siteRoot, 'public/api/recent-commits.schema.json'), 'utf8'));
const api = JSON.parse(await readFile(path.join(siteRoot, 'public/api/openapi.json'), 'utf8'));

function modelTables(models = { SiteSnapshot: schema, ...schema.$defs }) {
  return Object.entries(models).map(([name, model]) => {
    const rows = Object.entries(model.properties).map(([field, definition]) => {
      const limits = [definition.format, definition.minimum !== undefined ? `最小值 ${definition.minimum}` : null,
        definition.maxItems !== undefined ? `最多 ${definition.maxItems} 项` : null,
        definition.pattern ? `格式 ${definition.pattern}` : null,
        definition.const !== undefined ? `固定值 ${definition.const}` : null,
        definition.enum ? `枚举：${definition.enum.join('、')}` : null].filter(Boolean).join('；');
      const description = [definition.description || '', limits].filter(Boolean).join(' ');
      return `| \`${field}\` | \`${describeType(definition)}\` | ${model.required.includes(field) ? '是' : '否'} | ${description.replaceAll('|', '\\|')} |`;
    });
    return [`## ${name}`, '', '| 字段 | 类型 | 必填 | 说明 |', '| --- | --- | --- | --- |', ...rows, ''].join('\n');
  }).join('\n');
}

function rewrite(markdown, source, route) {
  let fence = null;
  return markdown.split('\n').map((line) => {
    const match = line.match(/^\s*(`{3,}|~{3,})/);
    if (match) {
      if (!fence) fence = match[1][0];
      else if (fence === match[1][0]) fence = null;
      return line;
    }
    if (fence) return line;
    return line.replace(/(!?\[[^\]\n]*\]\()([^\s)]+)(\))/g, (_, before, href, after) => {
      if (/^(?:[a-z][a-z\d+.-]*:|\/\/|#)/i.test(href)) return `${before}${href}${after}`;
      const split = href.search(/[?#]/);
      const pathname = split < 0 ? href : href.slice(0, split);
      const suffix = split < 0 ? '' : href.slice(split);
      const target = path.posix.normalize(path.posix.join(path.posix.dirname(source), decodeURIComponent(pathname)));
      if (routes.has(target)) {
        const relative = path.posix.relative(path.posix.dirname(route), routes.get(target)) || path.posix.basename(route);
        return `${before}${relative}${suffix}${after}`;
      }
      if (target.startsWith('site/public/') && !pathname.endsWith('/')) {
        const relative = path.posix.relative(path.posix.dirname(route), target.slice('site/public/'.length));
        return `${before}${relative}${suffix}${after}`;
      }
      const kind = pathname.endsWith('/') ? 'tree' : 'blob';
      return `${before}${github}/${kind}/${sourceBranch}/${target.split('/').map(encodeURIComponent).join('/')}${suffix}${after}`;
    });
  }).join('\n');
}

// Only this generated source directory is replaced; canonical Markdown stays in docs/.
await rm(output, { recursive: true, force: true });
await mkdir(output, { recursive: true });
for (const [source, route] of routes) {
  const content = (await readFile(path.join(repoRoot, source), 'utf8')).replace('<!-- API_MODELS -->', modelTables())
    .replace('<!-- UPDATES_MODELS -->', modelTables({ RecentCommitsSnapshot: recentSchema, Commit: recentSchema.$defs.Commit }));
  const target = path.join(output, route);
  await mkdir(path.dirname(target), { recursive: true });
  await writeFile(target, `---\neditLink: true\n---\n\n${rewrite(content, source, route)}\n`);
}
await cp(path.join(siteRoot, 'public'), path.join(output, 'public'), { recursive: true });
await writeFile(path.join(output, 'public/build-info.json'), JSON.stringify({
  managedBy: 'codeflare-docs',
  sourceBranch,
  sourceCommit: execFileSync('git', ['rev-parse', 'HEAD'], { cwd: repoRoot, encoding: 'utf8' }).trim(),
  contractVersion: api.info.version,
}, null, 2) + '\n');
await writeFile(path.join(output, 'routes.json'), JSON.stringify(Object.fromEntries([...routes].map(([source, route]) => [route, source]))));
console.log(`Prepared ${routes.size} documentation pages.`);
