import assert from "node:assert/strict";
import { execFileSync } from "node:child_process";
import { mkdtemp, mkdir, readFile, rm, writeFile } from "node:fs/promises";
import { tmpdir } from "node:os";
import { join } from "node:path";
import test from "node:test";
import { changedPaths, publishData, snapshotPaths, validateSnapshots } from "../publish-data.mjs";

const sourceCommit = "a".repeat(40);
const newerCommit = "b".repeat(40);
const repository = { owner: "xw7qwq", name: "codeflare", branch: "main" };
function snapshots(generatedAt = "2026-10-05T08:00:00.000Z") {
  const common = { repository, generatedAt, sourceRef: "origin/main" };
  return {
    "data/site-data.json": `${JSON.stringify({ ...common, problems: [], commitCount: 1 })}\n`,
    "data/recent-commits.json": `${JSON.stringify({ ...common, commits: [{ sha: sourceCommit }] })}\n`,
  };
}

async function fixture(t) {
  const checkout = await mkdtemp(join(tmpdir(), "codeflare-publish-test-"));
  t.after(() => rm(checkout, { recursive: true, force: true }));
  const realGit = (...args) => execFileSync("git", args, {
    cwd: checkout, encoding: "utf8", stdio: ["ignore", "pipe", "pipe"],
  }).trimEnd();
  realGit("init", "-b", "gh-pages");
  realGit("config", "user.name", "Test");
  realGit("config", "user.email", "test@example.invalid");
  realGit("remote", "add", "origin", "https://github.com/xw7qwq/codeflare.git");
  await mkdir(join(checkout, "data"));
  await writeFile(join(checkout, "CNAME"), "codeflare.lucius7.dev\n");
  const previous = snapshots("2026-10-04T08:00:00.000Z");
  for (const [name, contents] of Object.entries(previous)) await writeFile(join(checkout, name), contents);
  realGit("add", ".");
  realGit("commit", "-m", "initial website");
  const initial = realGit("rev-parse", "HEAD");
  realGit("update-ref", "refs/remotes/origin/gh-pages", initial);
  const current = snapshots();
  for (const [name, contents] of Object.entries(current)) await writeFile(join(checkout, name), contents);
  const calls = [];
  const pushes = [];
  const settings = { source: { branch: "gh-pages", path: "/" }, cname: "codeflare.lucius7.dev" };
  const options = {
    checkout,
    git: (...args) => {
      if (args[0] === "fetch") return "";
      if (args[0] === "push") { pushes.push(args); return ""; }
      return realGit(...args);
    },
    api: async (endpoint, ...args) => {
      calls.push([endpoint, ...args]);
      if (endpoint.endsWith("/git/ref/heads/main")) return { object: { sha: sourceCommit } };
      if (endpoint.endsWith("/pages")) return settings;
      if (endpoint.endsWith("/pages/builds")) return { status: "queued" };
      if (endpoint.endsWith("/pages/builds/latest")) return { status: "built", commit: realGit("rev-parse", "HEAD") };
      throw new Error(`unexpected API call ${endpoint}`);
    },
    fetch: async (url) => new Response(await readFile(join(checkout, new URL(url).pathname.slice(1)), "utf8")),
    delay: async () => {},
    log: () => {},
    buildAttempts: 2,
    publicAttempts: 2,
  };
  return { checkout, realGit, initial, previous, current, calls, pushes, settings, options };
}

test("Git status parsing includes both sides of a rename and preserves spaces", () => {
  assert.deepEqual(changedPaths(" M data/site-data.json\0R  data/recent-commits.json\0outside file.json\0"), [
    "data/site-data.json", "data/recent-commits.json", "outside file.json",
  ]);
});

test("reject mismatched snapshot generation times, refs, and repositories", () => {
  for (const change of [
    { generatedAt: "2026-10-04T08:00:00.000Z" },
    { sourceRef: "main" },
    { repository: { ...repository, name: "another-repository" } },
  ]) {
    const contents = snapshots();
    contents[snapshotPaths[1]] = JSON.stringify({ ...JSON.parse(contents[snapshotPaths[1]]), ...change });
    assert.throws(() => validateSnapshots(contents));
  }
});

test("reject unrelated dirty files before publishing", async (t) => {
  const f = await fixture(t);
  await writeFile(join(f.checkout, "README.md"), "unrelated edit\n");
  await assert.rejects(publishData(f.options), /changes outside the two data snapshots/);
  assert.equal(f.realGit("rev-parse", "HEAD"), f.initial);
  assert.equal(f.pushes.length, 0);
});

test("reject unrelated staged files before publishing", async (t) => {
  const f = await fixture(t);
  await writeFile(join(f.checkout, "README.md"), "unrelated edit\n");
  f.realGit("add", "README.md");
  await assert.rejects(publishData(f.options), /changes outside the two data snapshots/);
  assert.equal(f.pushes.length, 0);
});

test("reject an unexpected Pages domain", async (t) => {
  const f = await fixture(t);
  f.settings.cname = "another.example.invalid";
  await assert.rejects(publishData(f.options), /unexpected Pages domain/);
  assert.equal(f.pushes.length, 0);
});

test("superseded snapshots successfully skip without committing, pushing, or building", async (t) => {
  const f = await fixture(t);
  const api = f.options.api;
  f.options.api = (endpoint, ...args) => endpoint.endsWith("/git/ref/heads/main")
    ? { object: { sha: newerCommit } } : api(endpoint, ...args);
  assert.deepEqual(await publishData(f.options), { status: "skipped" });
  assert.equal(f.realGit("rev-parse", "HEAD"), f.initial);
  assert.equal(f.pushes.length, 0);
  assert(!f.calls.some(([endpoint]) => endpoint.includes("/pages/builds")));
});

test("source advancement immediately before push successfully skips", async (t) => {
  const f = await fixture(t);
  const api = f.options.api;
  let checks = 0;
  f.options.api = (endpoint, ...args) => endpoint.endsWith("/git/ref/heads/main")
    ? { object: { sha: ++checks === 1 ? sourceCommit : newerCommit } } : api(endpoint, ...args);
  assert.deepEqual(await publishData(f.options), { status: "skipped" });
  assert.equal(f.pushes.length, 0);
  assert(!f.calls.some(([endpoint]) => endpoint.includes("/pages/builds")));
});

test("push failures propagate after bounded retries and never request a build", async (t) => {
  const f = await fixture(t);
  const git = f.options.git;
  let pushes = 0;
  f.options.git = (...args) => {
    if (args[0] === "push") { pushes += 1; throw new Error("push denied"); }
    return git(...args);
  };
  await assert.rejects(publishData(f.options), /push denied/);
  assert.equal(pushes, 3);
  assert(!f.calls.some(([endpoint]) => endpoint.includes("/pages/builds")));
});

test("a competing docs push is preserved by the retry rebase", async (t) => {
  const f = await fixture(t);
  const remoteCheckout = await mkdtemp(join(tmpdir(), "codeflare-docs-test-"));
  t.after(() => rm(remoteCheckout, { recursive: true, force: true }));
  f.realGit("worktree", "add", "--detach", remoteCheckout, f.initial);
  const remoteGit = (...args) => execFileSync("git", args, {
    cwd: remoteCheckout, encoding: "utf8", stdio: ["ignore", "pipe", "pipe"],
  }).trimEnd();
  await mkdir(join(remoteCheckout, "docs"));
  await writeFile(join(remoteCheckout, "docs/index.html"), "concurrent documentation\n");
  remoteGit("add", "docs");
  remoteGit("commit", "-m", "docs: publish concurrently");
  f.realGit("update-ref", "refs/remotes/origin/gh-pages", remoteGit("rev-parse", "HEAD"));
  const git = f.options.git;
  let pushes = 0;
  f.options.git = (...args) => {
    if (args[0] === "push" && ++pushes === 1) throw new Error("non-fast-forward");
    return git(...args);
  };
  const result = await publishData(f.options);
  assert.equal(result.status, "published");
  assert.equal(pushes, 2);
  assert.equal(await readFile(join(f.checkout, "docs/index.html"), "utf8"), "concurrent documentation\n");
  assert.deepEqual(f.realGit("diff-tree", "--no-commit-id", "--name-only", "-r", result.deployed).split("\n"), snapshotPaths.slice().sort());
});

test("Pages build request failures propagate", async (t) => {
  const f = await fixture(t);
  const api = f.options.api;
  f.options.api = (endpoint, ...args) => {
    if (endpoint.endsWith("/pages/builds")) throw new Error("build request denied");
    return api(endpoint, ...args);
  };
  await assert.rejects(publishData(f.options), /build request denied/);
  assert.equal(f.pushes.length, 1);
});

test("a failed build containing the published snapshots fails publication", async (t) => {
  const f = await fixture(t);
  const api = f.options.api;
  f.options.api = (endpoint, ...args) => endpoint.endsWith("/pages/builds/latest")
    ? { status: "errored", commit: f.realGit("rev-parse", "HEAD"), error: { message: "build failed" } }
    : api(endpoint, ...args);
  await assert.rejects(publishData(f.options), /Pages build failed/);
});

test("a concurrent docs commit counts as deployed when both snapshot blobs match", async (t) => {
  const f = await fixture(t);
  const api = f.options.api;
  f.options.api = async (endpoint, ...args) => {
    if (endpoint.endsWith("/pages/builds")) {
      await mkdir(join(f.checkout, "docs"));
      await writeFile(join(f.checkout, "docs/index.html"), "new documentation\n");
      f.realGit("add", "docs");
      f.realGit("commit", "-m", "docs: concurrent publication");
    }
    return api(endpoint, ...args);
  };
  const result = await publishData(f.options);
  assert.equal(result.status, "published");
  assert.notEqual(result.deployed, f.realGit("rev-parse", "HEAD"));
  assert.deepEqual(f.realGit("diff-tree", "--no-commit-id", "--name-only", "-r", result.deployed).split("\n"), snapshotPaths.slice().sort());
  assert(f.calls.some(([, ...args]) => args.join(" ") === "--method POST"));
});

test("a built commit matching only one snapshot does not pass", async (t) => {
  const f = await fixture(t);
  const api = f.options.api;
  f.options.api = async (endpoint, ...args) => {
    if (endpoint.endsWith("/pages/builds")) {
      await writeFile(join(f.checkout, snapshotPaths[0]), f.previous[snapshotPaths[0]]);
      f.realGit("add", snapshotPaths[0]);
      f.realGit("commit", "-m", "different site snapshot");
    }
    return api(endpoint, ...args);
  };
  await assert.rejects(publishData(f.options), /did not complete a build of both/);
});

test("public verification requires both exact snapshot contents", async (t) => {
  const f = await fixture(t);
  f.options.fetch = async (url) => {
    const name = new URL(url).pathname.slice(1);
    return new Response(name === snapshotPaths[0] ? f.previous[name] : f.current[name]);
  };
  await assert.rejects(publishData(f.options), /both published data snapshots have not reached/);
});

test("public network failures fail after bounded retries", async (t) => {
  const f = await fixture(t);
  f.options.fetch = async () => { throw new Error("network unavailable"); };
  await assert.rejects(publishData(f.options), /both published data snapshots have not reached/);
});

test("CLI validation failures exit nonzero", async (t) => {
  const f = await fixture(t);
  const script = new URL("../publish-data.mjs", import.meta.url);
  // Running the actual CLI from a temp copy selects the fixture's checkout.
  await mkdir(join(f.checkout, "scripts"));
  await writeFile(join(f.checkout, "scripts/publish-data.mjs"), await readFile(script, "utf8"));
  f.realGit("switch", "-c", "wrong-branch");
  assert.throws(() => execFileSync(process.execPath, [join(f.checkout, "scripts/publish-data.mjs")], {
    stdio: ["ignore", "pipe", "pipe"],
  }), (error) => error.status === 1 && /unexpected checkout branch/.test(String(error.stderr)));
});
