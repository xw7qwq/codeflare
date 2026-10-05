#!/usr/bin/env node

import assert from "node:assert/strict";
import { execFileSync } from "node:child_process";
import { existsSync, realpathSync } from "node:fs";
import { readFile } from "node:fs/promises";
import { dirname, resolve } from "node:path";
import { setTimeout as delay } from "node:timers/promises";
import { fileURLToPath } from "node:url";

const repository = "xw7qwq/codeflare";
const domain = "codeflare.lucius7.dev";
export const snapshotPaths = ["data/site-data.json", "data/recent-commits.json"];

// With -z, Git returns renamed/copied paths as a second, separate entry.
export function changedPaths(status) {
  const entries = status.split("\0").filter(Boolean);
  const paths = [];
  for (let index = 0; index < entries.length; index += 1) {
    paths.push(entries[index].slice(3));
    if (/[RC]/.test(entries[index].slice(0, 2))) {
      assert(entries[index + 1], "incomplete Git rename status");
      paths.push(entries[++index]);
    }
  }
  return paths;
}

export function validateSnapshots(contents) {
  const [site, recent] = snapshotPaths.map((name) => JSON.parse(contents[name]));
  for (const snapshot of [site, recent]) {
    assert.deepEqual(snapshot.repository, { owner: "xw7qwq", name: "codeflare", branch: "main" },
      "unexpected snapshot repository");
  }
  assert.equal(site.generatedAt, recent.generatedAt, "snapshots have different generation times");
  assert.equal(typeof site.generatedAt, "string", "snapshot has no generation time");
  assert(Number.isFinite(Date.parse(site.generatedAt)), "invalid snapshot generation time");
  assert.equal(site.sourceRef, recent.sourceRef, "snapshots have different source refs");
  assert(["main", "origin/main"].includes(site.sourceRef), "unexpected snapshot source ref");
  assert(Array.isArray(site.problems), "snapshot has no problem list");
  assert(Array.isArray(recent.commits) && recent.commits.length, "snapshot has no recent commits");
  const sourceCommit = recent.commits[0].sha;
  assert(typeof sourceCommit === "string" && /^[a-f0-9]{40}$/.test(sourceCommit),
    "invalid snapshot source commit");
  return { sourceCommit, generatedAt: site.generatedAt, sourceRef: site.sourceRef };
}

function snapshotBlobs(git, commit) {
  return snapshotPaths.map((name) => git("rev-parse", `${commit}:${name}`));
}

export function buildMatchesSnapshots(git, commit, expected) {
  try {
    return snapshotBlobs(git, commit).every((blob, index) => blob === expected[index]);
  } catch {
    // A previous build's commit might not belong to the fetched Pages history.
    return false;
  }
}

export async function publishData(options = {}) {
  const checkout = options.checkout || resolve(dirname(fileURLToPath(import.meta.url)), "..");
  const git = options.git || ((...args) => execFileSync("git", args, {
    cwd: checkout, encoding: "utf8",
  }).trimEnd());
  const api = options.api || ((endpoint, ...args) => JSON.parse(execFileSync("gh", [
    "api", endpoint, ...args,
  ], { encoding: "utf8" })));
  const get = options.fetch || globalThis.fetch;
  const pause = options.delay || delay;
  const log = options.log || console.log;
  const buildAttempts = options.buildAttempts ?? 48;
  const publicAttempts = options.publicAttempts ?? 18;

  assert.equal(git("branch", "--show-current"), "gh-pages", "unexpected checkout branch");
  assert(/(?:github\.com[:/])xw7qwq\/codeflare(?:\.git)?$/.test(git("remote", "get-url", "origin")),
    "unexpected remote");
  const dirty = changedPaths(git("status", "--porcelain=v1", "-z"));
  assert(dirty.every((name) => snapshotPaths.includes(name)),
    "publication checkout has changes outside the two data snapshots");
  assert.equal((await readFile(resolve(checkout, "CNAME"), "utf8")).trim(), domain,
    "unexpected checkout domain");
  const settings = await api(`repos/${repository}/pages`);
  assert.equal(settings.source?.branch, "gh-pages", "unexpected Pages source branch");
  assert.equal(settings.source?.path, "/", "unexpected Pages source path");
  assert.equal(settings.cname, domain, "unexpected Pages domain");
  const contents = Object.fromEntries(await Promise.all(snapshotPaths.map(async (name) => [
    name, await readFile(resolve(checkout, name), "utf8"),
  ])));
  const metadata = validateSnapshots(contents);
  async function sourceIsCurrent() {
    const latest = (await api(`repos/${repository}/git/ref/heads/main`)).object.sha;
    if (latest === metadata.sourceCommit) return true;
    log(`Skipping superseded data snapshot ${metadata.sourceCommit}; main is now ${latest}.`);
    return false;
  }
  if (!await sourceIsCurrent()) return { status: "skipped" };

  git("add", "--", ...snapshotPaths);
  const changed = git("diff", "--cached", "--name-only", "-z").split("\0").filter(Boolean);
  assert(changed.every((name) => snapshotPaths.includes(name)),
    "publication changed files outside the two data snapshots");
  if (changed.length) {
    git("config", "user.name", "github-actions[bot]");
    git("config", "user.email", "41898282+github-actions[bot]@users.noreply.github.com");
    git("commit", "-m", `data: snapshot ${metadata.sourceCommit.slice(0, 7)}`);
    for (let attempt = 0; ; attempt += 1) {
      if (!await sourceIsCurrent()) return { status: "skipped" };
      try { git("push", "origin", "HEAD:gh-pages"); break; }
      catch (error) {
        if (attempt >= 2) throw error;
        git("fetch", "origin", "gh-pages");
        git("rebase", "origin/gh-pages");
      }
    }
  }
  const deployed = git("rev-parse", "HEAD");
  const blobs = snapshotBlobs(git, "HEAD");
  log(`Published data from ${metadata.sourceCommit} in Pages commit ${deployed}.`);
  // GITHUB_TOKEN pushes do not trigger Pages, so explicitly request the build.
  await api(`repos/${repository}/pages/builds`, "--method", "POST");
  let built = false;
  for (let attempt = 0; attempt < buildAttempts; attempt += 1) {
    await pause(5000);
    const build = await api(`repos/${repository}/pages/builds/latest`);
    log(`Pages: ${build.status} ${build.commit}`);
    if (!["built", "errored"].includes(build.status)) continue;
    git("fetch", "--quiet", "origin", "gh-pages");
    // A subsequent docs publication can change HEAD while retaining both snapshots.
    if (!buildMatchesSnapshots(git, build.commit, blobs)) continue;
    if (build.status === "errored") throw new Error(`Pages build failed: ${JSON.stringify(build.error)}`);
    built = true;
    break;
  }
  assert(built, "Pages did not complete a build of both published data snapshots");

  let fresh = false;
  for (let attempt = 0; attempt < publicAttempts; attempt += 1) {
    const responses = await Promise.all(snapshotPaths.map(async (name) => {
      try {
        const response = await get(`https://${domain}/${name}?ref=${deployed}&attempt=${attempt}`, {
          signal: AbortSignal.timeout(15000), headers: { "Cache-Control": "no-cache" },
        });
        return response.ok && await response.text() === contents[name];
      } catch (error) {
        log(`Waiting for public snapshot ${name}: ${error.message}`);
        return false;
      }
    }));
    if (responses.every(Boolean)) { fresh = true; break; }
    if (attempt + 1 < publicAttempts) await pause(5000);
  }
  assert(fresh, "both published data snapshots have not reached the public URL");
  log(`Data Pages verified: https://${domain}/`);
  return { status: "published", deployed, ...metadata };
}

if (process.argv[1] && existsSync(process.argv[1])
    && realpathSync(process.argv[1]) === fileURLToPath(import.meta.url)) {
  try { await publishData(); }
  catch (error) { console.error(error); process.exitCode = 1; }
}
