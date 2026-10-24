import assert from 'node:assert/strict';
import { execFileSync } from 'node:child_process';
import { access, cp, readFile, rm } from 'node:fs/promises';
import path from 'node:path';
import { setTimeout as delay } from 'node:timers/promises';

const [checkoutArg, artifactArg] = process.argv.slice(2);
assert(checkoutArg && artifactArg, 'usage: node publish.mjs <gh-pages-checkout> <artifact>');
const checkout = path.resolve(checkoutArg);
const artifact = path.resolve(artifactArg);
const destination = path.join(checkout, 'docs');
const repository = 'xw7qwq/solutions';
const git = (...args) => execFileSync('git', args, { cwd: checkout, encoding: 'utf8' }).trim();
const api = (endpoint, ...args) => JSON.parse(execFileSync('gh', ['api', endpoint, ...args], { encoding: 'utf8' }));
const metadata = JSON.parse(await readFile(path.join(artifact, 'build-info.json'), 'utf8'));
assert.equal(metadata.managedBy, 'codeflare-docs');
assert.equal(metadata.sourceBranch, 'docs/project-guide');
assert.equal(metadata.sourceCommit, process.env.GITHUB_SHA, 'artifact does not match workflow commit');
function requireCurrentSource() {
  const latest = api(`repos/${repository}/git/ref/heads/docs/project-guide`).object.sha;
  if (latest !== metadata.sourceCommit) {
    console.log(`Skipping superseded docs build ${metadata.sourceCommit}; source is now ${latest}.`);
    process.exit(0);
  }
}
requireCurrentSource();
assert.equal(git('branch', '--show-current'), 'gh-pages');
assert.equal(git('status', '--porcelain'), '', 'Pages checkout must be clean');
const remote = git('remote', 'get-url', 'origin');
assert(/(?:github\.com[:/])xw7qwq\/solutions(?:\.git)?$/.test(remote), 'unexpected remote');
const settings = api(`repos/${repository}/pages`);
assert.equal(settings.source.branch, 'gh-pages');
assert.equal(settings.source.path, '/');
assert.equal(settings.cname, 'codeflare.lucius7.dev', 'unexpected Pages domain');
await access(path.join(artifact, 'index.html'));
await access(path.join(artifact, 'api/openapi.json'));

try {
  await access(destination);
  const previous = JSON.parse(await readFile(path.join(destination, 'build-info.json'), 'utf8'));
  // Accept the previous marker for the first publication after the repository migration.
  assert([metadata.managedBy, 'competitive-programming-docs'].includes(previous.managedBy),
    'refusing to replace an unmanaged docs directory');
} catch (error) {
  // A missing destination is expected on first publication; any existing unmanaged folder fails.
  if (error.code !== 'ENOENT') throw error;
  try {
    await access(destination);
    throw new Error('existing docs folder has no publication marker');
  } catch (missing) {
    if (missing.code !== 'ENOENT') throw missing;
  }
}
// The only replaceable path is the generated docs/ under the verified Pages checkout.
await rm(destination, { recursive: true, force: true });
await cp(artifact, destination, { recursive: true });
git('add', '--', 'docs');
const changed = git('diff', '--cached', '--name-only', '-z').split('\0').filter(Boolean);
assert(changed.every((name) => name.startsWith('docs/')), 'publication changed files outside docs/');
if (changed.length) {
  git('config', 'user.name', 'github-actions[bot]');
  git('config', 'user.email', '41898282+github-actions[bot]@users.noreply.github.com');
  git('commit', '-m', `docs: publish ${metadata.sourceCommit.slice(0, 7)}`);
  for (let attempt = 0; ; attempt += 1) {
    requireCurrentSource();
    try { git('push', 'origin', 'HEAD:gh-pages'); break; }
    catch (error) {
      if (attempt >= 2) throw error;
      git('fetch', 'origin', 'gh-pages');
      git('rebase', 'origin/gh-pages');
    }
  }
}
const deployed = git('rev-parse', 'HEAD');
const docsTree = git('rev-parse', 'HEAD:docs');
console.log(`Published docs/ from ${metadata.sourceCommit} in Pages commit ${deployed}.`);
// GITHUB_TOKEN pushes do not trigger Pages automatically, so request the build explicitly.
api(`repos/${repository}/pages/builds`, '--method', 'POST');
let built = false;
for (let attempt = 0; attempt < 48; attempt += 1) {
  await delay(5000);
  const build = api(`repos/${repository}/pages/builds/latest`);
  console.log(`Pages: ${build.status} ${build.commit}`);
  if (build.commit === deployed && build.status === 'errored') throw new Error(JSON.stringify(build.error));
  if (build.status === 'built') {
    git('fetch', '--quiet', 'origin', 'gh-pages');
    let publishedTree;
    try { publishedTree = git('rev-parse', `${build.commit}:docs`); } catch { continue; }
    if (publishedTree === docsTree) { built = true; break; }
  }
}
assert(built, 'Pages did not complete a build of the published docs tree');

const base = 'https://codeflare.lucius7.dev/docs/';
let fresh = false;
for (let attempt = 0; attempt < 18; attempt += 1) {
  const response = await fetch(`${base}build-info.json?ref=${deployed}`, { signal: AbortSignal.timeout(15000) });
  if (response.ok) {
    const current = await response.json();
    if (current.sourceCommit === metadata.sourceCommit) { fresh = true; break; }
  }
  await delay(5000);
}
assert(fresh, 'published documentation has not reached the public URL');
for (const route of ['', 'api/', 'api/models.html', 'api/updates.html', 'api/openapi.json', 'api/site-data.schema.json', 'api/recent-commits.schema.json']) {
  const response = await fetch(`${base}${route}?ref=${deployed}`, { signal: AbortSignal.timeout(15000) });
  assert(response.ok, `deployed route failed: ${route} HTTP ${response.status}`);
}
console.log(`Docs Pages verified: ${base}`);
