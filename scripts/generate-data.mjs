#!/usr/bin/env node

import { execFileSync } from "node:child_process";
import { mkdirSync, readFileSync, writeFileSync } from "node:fs";
import { dirname, resolve } from "node:path";
import { fileURLToPath } from "node:url";

const siteRoot = resolve(dirname(fileURLToPath(import.meta.url)), "..");
const repositoryRoot = resolve(siteRoot);
const sourceRef = process.argv[2] || "main";
const supportedExtensions = new Set(["cpp", "cc", "cxx", "c", "py", "java", "rs", "go", "kt"]);
const ignoredRoots = new Set(["Templates", ".cph", ".vscode", ".github", "assets", "data", "scripts"]);
const repository = {
  owner: "xw7qwq",
  name: "solutions",
  branch: "main",
};

// Retained only for compatibility with the published v1 snapshot contract.
function legacyRatings() {
  try {
    return JSON.parse(readFileSync(resolve(siteRoot, "data/site-data.json"), "utf8")).ratings
      || { atcoder: null, codeforces: null };
  } catch {
    return { atcoder: null, codeforces: null };
  }
}

function git(args) {
  return execFileSync("git", args, {
    cwd: repositoryRoot,
    encoding: "utf8",
    env: { ...process.env, TZ: "Asia/Taipei" },
  });
}

function encodePath(path) {
  return path.split("/").map(encodeURIComponent).join("/");
}

function cleanToken(value) {
  return value.replace(/[_-]+/g, " ").replace(/\s+/g, " ").trim();
}

function uppercaseProblem(value) {
  return cleanToken(value).replace(/^([a-z])(\d?)$/i, (_, letter, number) => `${letter.toUpperCase()}${number}`);
}

function atCoderContestName(contest) {
  const match = contest.match(/^([a-z]+)(\d+)$/i);
  return match ? `${match[1].toUpperCase()} ${Number(match[2])}` : contest.toUpperCase();
}

function languageFromExtension(extension) {
  return ({
    cpp: "C++", cc: "C++", cxx: "C++", c: "C", py: "Python",
    java: "Java", rs: "Rust", go: "Go", kt: "Kotlin",
  })[extension] || extension.toUpperCase();
}

function platformHome(platform) {
  return ({
    AtCoder: "https://atcoder.jp/",
    Codeforces: "https://codeforces.com/",
    CSES: "https://cses.fi/problemset/",
    HDU: "https://acm.hdu.edu.cn/",
    Lanqiao: "https://www.lanqiao.cn/problems/",
    LOJ: "https://loj.ac/",
    Luogu: "https://www.luogu.com.cn/",
    Mati: "https://www.matiji.net/",
    NowCoder: "https://ac.nowcoder.com/acm/contest/vip-index",
    ProjectEuler: "https://projecteuler.net/archives",
    QOJ: "https://qoj.ac/",
    SMQYOJ: "https://www.smqyoj.com/training",
    SPOJ: "https://www.spoj.com/problems/classical/",
    VJudge: "https://vjudge.net/",
  })[platform] || `https://github.com/${repository.owner}/${repository.name}`;
}

function problemFromPath(path) {
  const segments = path.split("/");
  if (segments.length < 2 || ignoredRoots.has(segments[0])) return null;
  const fileName = segments.at(-1);
  const extension = fileName.includes(".") ? fileName.split(".").pop().toLowerCase() : "";
  if (!supportedExtensions.has(extension)) return null;

  const platform = segments[0];
  const stem = fileName.slice(0, -(extension.length + 1));
  const item = {
    path,
    platform,
    language: languageFromExtension(extension),
    title: cleanToken(stem),
    problemUrl: platformHome(platform),
    githubUrl: `https://github.com/${repository.owner}/${repository.name}/blob/${repository.branch}/${encodePath(path)}`,
    rawUrl: `https://raw.githubusercontent.com/${repository.owner}/${repository.name}/${repository.branch}/${encodePath(path)}`,
  };

  if (platform === "AtCoder" && segments.length >= 3) {
    const contest = segments[1];
    const problem = uppercaseProblem(stem);
    item.title = `${atCoderContestName(contest)} · Problem ${problem}`;
    item.problemUrl = `https://atcoder.jp/contests/${encodeURIComponent(contest)}/tasks/${encodeURIComponent(contest)}_${encodeURIComponent(stem.toLowerCase())}`;
  } else if (platform === "Codeforces" && segments.length >= 3) {
    const contest = segments[1];
    const problem = uppercaseProblem(stem);
    const area = Number(contest) >= 100000 ? "gym" : "contest";
    item.title = `${area === "gym" ? "Gym" : "Codeforces"} ${contest} · ${problem}`;
    item.problemUrl = `https://codeforces.com/${area}/${encodeURIComponent(contest)}/problem/${encodeURIComponent(problem)}`;
  } else if (platform === "CSES") {
    const match = stem.match(/^(\d+)[_-]?(.*)$/);
    if (match) {
      item.title = cleanToken(match[2]) || `Task ${match[1]}`;
      item.problemUrl = `https://cses.fi/problemset/task/${match[1]}`;
    }
  } else if (platform === "Luogu") {
    item.title = stem.toUpperCase();
    item.problemUrl = `https://www.luogu.com.cn/problem/${encodeURIComponent(stem)}`;
  } else if (platform === "LOJ") {
    item.title = `LibreOJ #${stem}`;
    item.problemUrl = `https://loj.ac/p/${encodeURIComponent(stem)}`;
  } else if (platform === "Lanqiao") {
    item.title = `蓝桥云课 #${stem}`;
    item.problemUrl = `https://www.lanqiao.cn/problems/${encodeURIComponent(stem)}/learning/`;
  } else if (platform === "SPOJ") {
    item.title = stem.toUpperCase();
    item.problemUrl = `https://www.spoj.com/problems/${encodeURIComponent(stem.toUpperCase())}/`;
  } else if (platform === "ProjectEuler") {
    const problemNumber = (segments[1] || "").replace(/^P/i, "");
    item.title = `Problem ${problemNumber} · Solution ${cleanToken(stem)}`;
    item.problemUrl = `https://projecteuler.net/problem=${encodeURIComponent(problemNumber)}`;
  } else if (platform === "NowCoder" && segments.length >= 3) {
    const contest = segments[1];
    const problem = uppercaseProblem(stem);
    item.title = `Contest ${contest} · ${problem}`;
    item.problemUrl = `https://ac.nowcoder.com/acm/contest/${encodeURIComponent(contest)}/${encodeURIComponent(problem)}`;
  } else if (platform === "VJudge" && segments.length >= 3) {
    const contest = segments[1];
    const problem = uppercaseProblem(stem);
    item.title = `Contest ${contest} · ${problem}`;
    item.problemUrl = `https://vjudge.net/contest/${encodeURIComponent(contest)}#problem/${encodeURIComponent(problem)}`;
  } else if (platform === "QOJ" && segments.length >= 3) {
    item.title = `Contest ${segments[1]} · ${uppercaseProblem(stem)}`;
    item.problemUrl = `https://qoj.ac/contest/${encodeURIComponent(segments[1])}`;
  } else if (platform === "HDU") {
    if (segments.length === 2 && /^\d+$/.test(stem)) {
      item.title = `HDU ${stem}`;
      item.problemUrl = `https://acm.hdu.edu.cn/showproblem.php?pid=${encodeURIComponent(stem)}`;
    } else if (segments.length >= 3) {
      item.title = `HDU ${segments[1]} · ${uppercaseProblem(stem)}`;
      item.problemUrl = "https://acm.hdu.edu.cn/";
    }
  } else if (platform === "Mati" && segments.length >= 3) {
    item.title = `Contest ${segments[1]} · ${cleanToken(stem)}`;
    item.problemUrl = "https://www.matiji.net/";
  } else if (platform === "L7OJ" && segments.length >= 3) {
    item.title = `${cleanToken(segments[1]).toUpperCase()} · ${uppercaseProblem(stem)}`;
  } else if (platform === "SMQYOJ") {
    item.title = cleanToken(stem).toUpperCase();
    item.problemUrl = "https://www.smqyoj.com/training";
  }
  return item;
}

const paths = git(["ls-tree", "-r", "-z", "--name-only", sourceRef]).split("\0").filter(Boolean);
const latestCommitByPath = new Map();
let currentCommitTime = null;
for (const line of git([
  "-c", "core.quotepath=false", "log", sourceRef,
  "--name-only", "--format=@@COMMIT@@%cI",
]).split("\n")) {
  if (line.startsWith("@@COMMIT@@")) {
    currentCommitTime = line.slice("@@COMMIT@@".length);
  } else if (line && currentCommitTime && !latestCommitByPath.has(line)) {
    latestCommitByPath.set(line, currentCommitTime);
  }
}
const problems = paths
  .map((path) => {
    const problem = problemFromPath(path);
    if (problem) problem.submittedAt = latestCommitByPath.get(path) || null;
    return problem;
  })
  .filter(Boolean);
const commitDates = git(["log", sourceRef, "--date=format-local:%Y-%m-%d", "--format=%ad"])
  .split("\n")
  .filter(Boolean);
const contributions = commitDates.reduce((days, date) => {
  days[date] = (days[date] || 0) + 1;
  return days;
}, {});

const payload = {
  repository,
  generatedAt: new Date().toISOString(),
  sourceRef,
  commitCount: Number(git(["rev-list", "--count", sourceRef])),
  contributions,
  problems,
  ratings: legacyRatings(),
};

mkdirSync(resolve(siteRoot, "data"), { recursive: true });
writeFileSync(resolve(siteRoot, "data/site-data.json"), `${JSON.stringify(payload)}\n`);
console.log(`Generated ${problems.length} problems and ${Object.keys(contributions).length} active days from ${sourceRef}.`);

const fields = git(["log", sourceRef, "-6", "-z", "--format=%H%x00%B%x00%cI"]).split("\0");
if (fields.at(-1) === "") fields.pop();
if (fields.length % 3) throw new Error("Unexpected Git commit output");
const commits = [];
for (let index = 0; index < fields.length; index += 3) {
  const [sha, message, committedAt] = fields.slice(index, index + 3);
  commits.push({
    sha,
    subject: message.split("\n")[0],
    committedAt: new Date(committedAt).toISOString(),
    url: `https://github.com/${repository.owner}/${repository.name}/commit/${sha}`,
  });
}
writeFileSync(resolve(siteRoot, "data/recent-commits.json"), `${JSON.stringify({
  repository,
  generatedAt: payload.generatedAt,
  sourceRef,
  commits,
})}\n`);
console.log(`Generated ${commits.length} recent main commits.`);
