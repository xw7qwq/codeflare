import assert from 'node:assert/strict';
import { execFileSync } from 'node:child_process';
import { readFile } from 'node:fs/promises';
import path from 'node:path';
import { fileURLToPath } from 'node:url';
import Ajv from 'ajv/dist/2020.js';
import addFormats from 'ajv-formats';
import SwaggerParser from '@apidevtools/swagger-parser';

const siteRoot = path.dirname(fileURLToPath(import.meta.url));
const schema = JSON.parse(await readFile(path.join(siteRoot, 'public/api/site-data.schema.json'), 'utf8'));
const specification = path.join(siteRoot, 'public/api/openapi.json');
const api = await SwaggerParser.validate(specification);
assert.equal(api.info.version, '2.0.0');
assert.deepEqual(api.servers, [{ url: 'https://codeflare.lucius7.dev' }]);
assert.equal(schema.$id, 'https://codeflare.lucius7.dev/docs/api/site-data.schema.json');
const ajv = new Ajv({ strict: true, allErrors: true });
addFormats(ajv);
const validate = ajv.compile(schema);
const source = process.argv[2];
const snapshot = JSON.parse(source
  ? await readFile(path.resolve(source), 'utf8')
  : execFileSync('git', ['show', 'origin/gh-pages:data/site-data.json'], { cwd: siteRoot, encoding: 'utf8', maxBuffer: 16 * 1024 * 1024 }));

function check(value) {
  assert(validate(value), JSON.stringify(validate.errors, null, 2));
  const seen = new Set();
  const languages = { cpp: 'C++', cc: 'C++', cxx: 'C++', c: 'C', py: 'Python', java: 'Java', rs: 'Rust', go: 'Go', kt: 'Kotlin' };
  for (const problem of value.problems) {
    const parts = problem.path.split('/');
    assert(parts.length >= 2 && parts.every((part) => part && part !== '.' && part !== '..'), 'invalid repository path');
    assert(!/[\\\u0000-\u001f\u007f]/.test(problem.path), 'invalid path characters');
    assert(!seen.has(problem.path), `duplicate path: ${problem.path}`);
    seen.add(problem.path);
    assert.equal(problem.platform, parts[0], 'platform mismatch');
    assert.equal(problem.language, languages[parts.at(-1).split('.').at(-1).toLowerCase()], 'language mismatch');
    const encoded = parts.map(encodeURIComponent).join('/');
    const { owner, name, branch } = value.repository;
    assert.equal(problem.githubUrl, `https://github.com/${owner}/${name}/blob/${branch}/${encoded}`, 'GitHub URL mismatch');
    assert.equal(problem.rawUrl, `https://raw.githubusercontent.com/${owner}/${name}/${branch}/${encoded}`, 'raw URL mismatch');
  }
  assert.equal(Object.values(value.contributions).reduce((total, count) => total + count, 0), value.commitCount, 'commit total mismatch');
  for (const rating of Object.values(value.ratings)) {
    if (rating?.rating != null && rating.maxRating != null) assert(rating.maxRating >= rating.rating, 'maximum rating below current rating');
  }
}
check(snapshot);

const invalid = [
  ['missing required field', (s) => { delete s.generatedAt; }],
  ['undeclared field', (s) => { s.schemaVersion = 'invented'; }],
  ['invalid calendar date', (s) => { s.contributions['2026-02-30'] = 1; s.commitCount += 1; }],
  ['invalid count', (s) => { s.commitCount = -1; }],
  ['legacy repository identity', (s) => { s.repository.owner = 'theLucius7'; s.repository.name = 'CompetitiveProgramming'; }],
];
if (snapshot.problems.length) invalid.push(
  ['duplicate path', (s) => { s.problems.push(structuredClone(s.problems[0])); }],
  ['invalid timestamp', (s) => { s.problems[0].submittedAt = 'yesterday'; }],
  ['wrong source URL', (s) => { s.problems[0].rawUrl = 'https://raw.githubusercontent.com/wrong'; }],
);
for (const [name, mutate] of invalid) {
  const broken = structuredClone(snapshot);
  mutate(broken);
  assert.throws(() => check(broken), name);
}
const nullable = structuredClone(snapshot);
nullable.ratings = { atcoder: null, codeforces: null };
if (nullable.problems.length) nullable.problems[0].submittedAt = null;
check(nullable);
console.log(`API contract OK: OpenAPI 3.1, JSON Schema 2020-12, ${snapshot.problems.length} records, ${invalid.length} invalid-data cases rejected.`);

const recentSchema = JSON.parse(await readFile(path.join(siteRoot, 'public/api/recent-commits.schema.json'), 'utf8'));
assert.equal(recentSchema.$id, 'https://codeflare.lucius7.dev/docs/api/recent-commits.schema.json');
const validateRecent = ajv.compile(recentSchema);
const recentSource = process.argv[3];
const recent = JSON.parse(recentSource
  ? await readFile(path.resolve(recentSource), 'utf8')
  : execFileSync('git', ['show', 'origin/gh-pages:data/recent-commits.json'], { cwd: siteRoot, encoding: 'utf8' }));

function checkRecent(value) {
  assert(validateRecent(value), JSON.stringify(validateRecent.errors, null, 2));
  const seen = new Set();
  for (const commit of value.commits) {
    assert(!seen.has(commit.sha), 'duplicate commit SHA');
    seen.add(commit.sha);
    assert.equal(commit.url, `https://github.com/${value.repository.owner}/${value.repository.name}/commit/${commit.sha}`, 'commit URL mismatch');
  }
}
checkRecent(recent);
const badRecent = [
  ['missing field', (s) => { delete s.generatedAt; }],
  ['unknown field', (s) => { s.total = 6; }],
  ['legacy repository identity', (s) => { s.repository.owner = 'theLucius7'; s.repository.name = 'CompetitiveProgramming'; }],
];
if (recent.commits.length) badRecent.push(
  ['more than six commits', (s) => { while (s.commits.length <= 6) s.commits.push(structuredClone(recent.commits[0])); }],
  ['duplicate SHA', (s) => { s.commits = [s.commits[0], structuredClone(s.commits[0])]; }],
  ['short SHA', (s) => { s.commits[0].sha = 'abc1234'; }],
  ['invalid date', (s) => { s.commits[0].committedAt = 'yesterday'; }],
  ['wrong commit URL', (s) => { s.commits[0].url = 'https://github.com/xw7qwq/solutions/commit/' + (s.commits[0].sha[0] === '0' ? '1' : '0').repeat(40); }],
  ['unknown commit field', (s) => { s.commits[0].rating = 1; }],
  ['multiline subject', (s) => { s.commits[0].subject = 'one\ntwo'; }],
);
for (const [name, mutate] of badRecent) {
  const broken = structuredClone(recent);
  mutate(broken);
  assert.throws(() => checkRecent(broken), name);
}
checkRecent({ ...recent, commits: [] });
console.log(`Recent commits contract OK: ${recent.commits.length} commits, ${badRecent.length} invalid-data cases rejected.`);
