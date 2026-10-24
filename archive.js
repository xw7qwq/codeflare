// Shared, dependency-free repository metadata and safe path handling.
const Archive = (() => {
const REPOSITORY = {
  owner: "xw7qwq",
  name: "solutions",
  branch: "main",
};

const SUPPORTED_EXTENSIONS = new Set(["cpp", "cc", "cxx", "c", "py", "java", "rs", "go", "kt"]);
const IGNORED_ROOTS = new Set(["Templates", ".cph", ".vscode", ".github", "assets", "data", "scripts"]);
const HIGHLIGHT_LANGUAGES = {
  "C++": "cpp",
  C: "c",
  Python: "python",
  Java: "java",
  Rust: "rust",
  Go: "go",
  Kotlin: "kotlin",
};


function encodePath(path) {
  return path.split("/").map(encodeURIComponent).join("/");
}

function cleanToken(value) {
  return value
    .replace(/[_-]+/g, " ")
    .replace(/\s+/g, " ")
    .trim();
}

function uppercaseProblem(value) {
  return cleanToken(value).replace(/^([a-z])(\d?)$/i, (_, letter, number) => `${letter.toUpperCase()}${number}`);
}

function atCoderContestName(contest) {
  const match = contest.match(/^([a-z]+)(\d+)$/i);
  return match ? `${match[1].toUpperCase()} ${Number(match[2])}` : contest.toUpperCase();
}

function problemFromPath(path, submittedAt = null) {
  if (typeof path !== "string" || /[\\\\\u0000-\u001f\u007f]/.test(path)) return null;
  const segments = path.split("/");
  if (segments.length < 2 || segments.some((part) => !part || part === "." || part === "..") || IGNORED_ROOTS.has(segments[0])) return null;

  const fileName = segments.at(-1);
  const extension = fileName.includes(".") ? fileName.split(".").pop().toLowerCase() : "";
  if (!SUPPORTED_EXTENSIONS.has(extension)) return null;

  const platform = segments[0];
  const stem = fileName.slice(0, -(extension.length + 1));
  const item = {
    path,
    platform,
    language: languageFromExtension(extension),
    title: cleanToken(stem),
    problemUrl: platformHome(platform),
    githubUrl: `https://github.com/${REPOSITORY.owner}/${REPOSITORY.name}/blob/${REPOSITORY.branch}/${encodePath(path)}`,
    rawUrl: `https://raw.githubusercontent.com/${REPOSITORY.owner}/${REPOSITORY.name}/${REPOSITORY.branch}/${encodePath(path)}`,
    submittedAt,
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
    const solutionNumber = cleanToken(stem);
    item.title = `Problem ${problemNumber}${solutionNumber ? ` · Solution ${solutionNumber}` : ""}`;
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
    const contest = segments[1];
    item.title = `Contest ${contest} · ${uppercaseProblem(stem)}`;
    item.problemUrl = `https://qoj.ac/contest/${encodeURIComponent(contest)}`;
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

  item.searchText = `${item.title} ${item.platform} ${item.path}`.toLocaleLowerCase();
  return item;
}

function languageFromExtension(extension) {
  return ({
    cpp: "C++",
    cc: "C++",
    cxx: "C++",
    c: "C",
    py: "Python",
    java: "Java",
    rs: "Rust",
    go: "Go",
    kt: "Kotlin",
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
  })[platform] || `https://github.com/${REPOSITORY.owner}/${REPOSITORY.name}`;
}

function formatNumber(value) {
  return new Intl.NumberFormat("zh-CN").format(value || 0);
}

function formatSubmissionTime(value, long = false) {
  if (!value) return "时间未知";
  const date = new Date(value);
  if (Number.isNaN(date.getTime())) return "时间未知";
  return new Intl.DateTimeFormat("zh-CN", {
    year: "numeric",
    month: "2-digit",
    day: "2-digit",
    hour: "2-digit",
    minute: "2-digit",
    hour12: false,
    ...(long ? { timeZoneName: "short" } : {}),
  }).format(date);
}


async function fetchTextWithTimeout(url, options = {}, timeoutMs = 6000) {
  const controller = new AbortController();
  const timeoutId = window.setTimeout(() => controller.abort(), timeoutMs);
  try {
    const response = await fetch(url, { ...options, signal: controller.signal });
    if (!response.ok) throw new Error(`HTTP ${response.status}`);
    return await response.text();
  } finally {
    window.clearTimeout(timeoutId);
  }
}


function readerUrl(path) {
  return `./code.html?path=${encodePath(path)}`;
}
return Object.freeze({ REPOSITORY, HIGHLIGHT_LANGUAGES, problemFromPath, encodePath, formatNumber, formatSubmissionTime, fetchTextWithTimeout, readerUrl });
})();
