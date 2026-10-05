# Contributing

Read the [organization contribution guide](https://github.com/xw7qwq/.github/blob/main/CONTRIBUTING.md), then follow this repository's branch responsibilities and validation requirements. Detailed [code and archive conventions](https://github.com/xw7qwq/codeflare/blob/docs/project-guide/docs/conventions.md) and the [maintenance workflow](https://github.com/xw7qwq/codeflare/blob/docs/project-guide/docs/maintenance.md) live on the documentation source branch.

## Branches and pull requests

| Target branch | Contents |
| --- | --- |
| `main` | Algorithm source code, templates, provenance receipts, and archive tools |
| `docs/project-guide` | Project documentation, API contracts, and documentation site source |
| `gh-pages` | Problem browser source, data snapshots, and the generated `docs/` website |

Create a temporary branch from the relevant target, such as `fix/archive-import`, `docs/update-tooling`, or `sync/contest-submissions`. After validation, open a PR against that target and describe the reason for the change and actual check results. Merge after checks pass, then confirm the target contains the changes before deleting the working branch. Keep all three permanent branches; do not merge the entire website branch into the algorithm branch.

Archive scripts operate only on local files and do not commit or push Git changes. New provenance records and source files also enter `main` through a working branch and PR. The documentation publishing workflow updates `gh-pages/docs/` directly. That directory is generated output and must not be edited manually. Manual documentation and website changes also require PRs.

## Validation before submission

Run these commands from the repository root:

```sh
python3 -m unittest discover -s scripts/tests -p 'test_archive_*.py'
python3 scripts/archive_verify.py
git diff --check
```

The `Archive` workflow runs offline tests and a read-only integrity audit on every PR and push to `main` or `docs/project-guide`. Its stable check name is `archive`. The audit checks provenance receipts, hashes, byte counts, and encodings of imported source files. It does not run algorithms or establish an AC verdict.

The `Site snapshots` workflow validates the website and current `main` snapshots on PRs targeting `main`. Pushes to `main`, daily scheduled runs, and manual runs also publish the two generated `gh-pages/data/` snapshots and verify the Pages build and public data. Like the documentation publisher, this automation updates generated output directly; manual website changes still use PRs targeting `gh-pages`.

Compile individual problem changes separately and provide the original problem link, reproduction input, and expected output. Include a minimal invocation and boundary checks for template changes. Each branch's README describes additional documentation and website checks. Preserve source paths and verified provenance; successful compilation or file existence is not evidence of an accepted submission.

## Commit conventions

Use `type(scope): specific action`, such as `fix(archive): validate submission identity` or `docs(tooling): explain local setup`. Stage files by path and review the diff. Exclude build output, local samples, temporary caches, and credentials, and avoid unrelated bulk renaming or formatting. Retain attribution and licenses when citing third-party material. This repository has no single license, so do not invent permissions for historical content.
