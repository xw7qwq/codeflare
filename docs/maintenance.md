# Maintenance

## Branch responsibilities

`main` stores the algorithm archive, `docs/project-guide` stores documentation source and its source-code baseline, and `gh-pages` stores the problem browser and generated documentation under `docs/`. Publishing documentation does not require merging it into `main`. Create temporary branches from the relevant target, validate changes, open a PR, and merge after checks pass. Confirm that the target contains the changes before deleting the temporary branch. Retain all three permanent branches. See the [organization guide](https://github.com/xw7qwq/.github/blob/main/CONTRIBUTING.md) and [GitHub flow](https://docs.github.com/en/get-started/using-github/github-flow).

The documentation branch can merge a newer `main` through a PR to refresh its source baseline. This is optional catalog maintenance, not a deployment prerequisite. After a documentation PR merges, the workflow directly updates `gh-pages/docs/`. The `Site snapshots` workflow on `main` automatically updates the two generated `gh-pages/data/` snapshots after main pushes, daily, or on manual runs. Archive tools do not commit or push Git automatically; names such as `sync/contest-submissions` identify temporary import branches, not permanent automation branches. Follow [Pages maintenance](pages.md) for the root problem browser.

## Preview and deploy documentation

Live site: [CodeFlare Docs](https://codeflare.lucius7.dev/docs/). VitePress provides API and project documentation with full-text search, navigation, syntax highlighting, and dark mode.

Run from the repository root with Node.js 22+:

```sh
npm --prefix site ci
npm --prefix site run dev
```

The development server runs at `http://127.0.0.1:4174/docs/`. The maintained Markdown sources are `docs/` and the root README and CONTRIBUTING files. Run `npm --prefix site run prepare:docs` to regenerate content while the server is running, then view the updated pages. Stop the server with `Ctrl+C`.

Before publishing, run:

```sh
python3 scripts/docs.py check
npm --prefix site run check:api
npm --prefix site run build
git diff --check
```

Push the checked documents, configuration, and lockfile to a working branch and open a PR targeting `docs/project-guide`. Merging starts publication; no additional push to `main` is needed. Keep generated `site/.content/`, `site/.vitepress/dist/`, and `node_modules` directories out of commits. Dependencies use a patched Vite version pinned in the lockfile. Retain `package-lock.json` and rerun checks and builds when updating dependencies.

The publishing workflow validates Markdown, OpenAPI, JSON Schema, and actual snapshots before building the site. Model field tables come directly from the schemas. Source links are converted to GitHub links so deployed pages do not point to nonexistent local directories.

Only pushes to `docs/project-guide` publish documentation. The publishing job downloads verified artifacts, confirms that they still correspond to the documentation branch's latest commit, and verifies the `xw7qwq/codeflare` repository, the root of `gh-pages` as the Pages source, and the `codeflare.lucius7.dev` domain. It replaces only the managed `gh-pages/docs/` directory. It shares the `codeflare-docs-pages` publication lock with `Site snapshots`, which writes only `data/site-data.json` and `data/recent-commits.json`. Both publishers set `queue: max` so new jobs do not replace pending publications. Superseded runs safely skip publication. After checking staged paths, it performs a normal push, explicitly requests a Pages build, and checks public `docs/build-info.json` and page responses. PRs only validate and build. Inspect [Actions](https://github.com/xw7qwq/codeflare/actions) if any step fails; a successful push alone does not establish a successful deployment.

Build output uses the `codeflare-docs` management marker. The first migration also accepts the previous `competitive-programming-docs` marker to update an existing documentation directory. This compatibility applies only after repository, branch, and domain validation; unknown directories are still protected from replacement. When migrating the API repository identity, publish new `gh-pages` snapshots before documentation that depends on them.

For a rollback, create a working branch from the documentation source branch, explicitly run `git revert <commit-sha>` for the problematic commit, validate, and merge a PR back into the source branch. Keep generated `gh-pages/docs/` content under the publishing workflow's control; the next build replaces manual edits.

## Update documentation and the source catalog

Run from a complete clone's root, handling your uncommitted work first:

```sh
git status --short --branch
git fetch origin
git switch -c docs/refresh-catalog origin/docs/project-guide
git merge origin/main
python3 scripts/docs.py generate --ref origin/main
python3 scripts/docs.py check
git diff --check
git diff --stat
```

If the working branch name already exists, choose another clear name or inspect and continue the existing work. Do not overwrite it. Review conflicts individually, preserving the documentation entry points and build configuration alongside the source branch's archive rules. After merging the source branch, also check directory conventions, tool behavior, and website compatibility. Generated statistics cannot replace these checks.

`generate` writes only `docs/catalog.md`. It parses NUL-delimited Git paths and preserves filenames containing Chinese characters or spaces. It records the complete source commit SHA and commit time, rather than the machine's current date. Counts describe source and other tracked files in the selected commit, excluding uncommitted working files.

Commit explanation updates with the generated catalog:

```sh
git add docs/catalog.md
# Add any manually edited documentation paths individually.
git diff --cached --check
git diff --cached
git commit -m "docs(catalog): refresh source inventory"
git push -u origin HEAD
```

Open a PR targeting `docs/project-guide` and complete documentation builds, API checks, and archive checks before merging. No empty commit is needed if neither the catalog nor documentation changed. Source changes on `main` trigger `Site snapshots` automatically; verify that workflow's Pages and public JSON checks before relying on the new snapshots. Website source changes still use a separate PR targeting `gh-pages`.

## Local checks and CI

```sh
python3 scripts/docs.py check
git diff --check
```

The checker uses Git and the Python 3.10+ standard library. It verifies required documents, one top-level heading, basic whitespace, code fences, relative repository links, and Markdown heading anchors. It recalculates the catalog from its recorded commit and checks the checkout's source paths against the baseline. Track new documentation and local link targets with `git add <path>` first; a file that exists only in a local working directory is not a publishable link target.

Checks cover the root `README.md`, `CONTRIBUTING.md`, and all Markdown under `docs/`, without requiring historical notes to be rewritten. The script does not access the network, run algorithms, check every external link or judge result, or establish that all commands work on every system. It supports the inline links and ATX headings used here, rather than implementing a general Markdown renderer.

The [documentation workflow](../.github/workflows/docs.yml) validates and builds every PR targeting `docs/project-guide` and every push to that branch, without path-based skips. Its stable check name is `documentation`. Whitespace checks cover maintained documentation, configuration, and tools, preserving imported contest sources. The validation job is read-only; a separate publishing job uses `contents: write` and `pages: write` to update documentation output and request a Pages build.

The [archive workflow](../.github/workflows/archive.yml) runs `archive` on every PR and push to `main` and `docs/project-guide`, using offline tests and a read-only provenance audit. `main` requires `archive`; the documentation branch requires both `archive` and `documentation`. The [snapshot workflow](https://github.com/xw7qwq/codeflare/blob/main/.github/workflows/site.yml) validates the website and current main snapshots on PRs targeting `main`. Main pushes, daily schedules, and manual runs also publish the two data files and verify the Pages build and public JSON. Documentation and snapshot publishing write directly to generated paths on `gh-pages`, so the output branch must allow the publishing bot's writes. See the [GitHub Actions documentation](https://docs.github.com/en/actions/get-started/quickstart) for workflow authoring.

When a check fails, inspect the reported path and reason:

| Failure | Fix |
| --- | --- |
| Missing relative link target | Check case, encoding, branch, and whether a new file is tracked |
| Invalid heading anchor | Update headings and references together |
| Catalog differs from its source commit | Regenerate using the commands above; avoid editing generated sections manually |
| Current source paths differ from the baseline | Synchronize with `main`, review changes, then generate from `origin/main` |
| Missing catalog source commit | Use a complete clone: `git fetch --unshallow origin` for a shallow clone, or `git fetch origin` otherwise |
| Git ignores a configuration file | Run `git check-ignore -v <path>` and update the precise allowlist as needed |

## Documentation quality

Add guidance to the document closest to the task and link it from the [documentation index](README.md). Support factual claims with code, configuration, command output, or official sources. Distinguish dated audits from live status. Short examples should identify the working directory, dependencies, and expected result; distinguish pseudocode from commands that have actually been tested.

Maintain statistics only in the generated catalog. Use a fixed commit when documenting provenance and make it clear when a link follows changing content. Update conventions and validation records when branch strategy, problem naming, data definitions, or tool behavior changes.
