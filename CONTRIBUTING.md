# Contributing

Read the [organization contribution guide](https://github.com/xw7qwq/.github/blob/main/CONTRIBUTING.md). Choose the target branch before editing: `main` for algorithms, `docs/project-guide` for documentation, or `gh-pages` for the website. See [Maintenance](docs/maintenance.md) for branch responsibilities and synchronization.

## Make a change

1. Check `git status --short --branch` and preserve existing uncommitted work.
2. Create a clearly named branch from the relevant baseline, such as `fix/cf-1000-e` or `docs/update-tooling`.
3. Follow the [code and archive conventions](docs/conventions.md), keeping each commit focused on one purpose.
4. Track new files by path, complete the applicable checks below, and review the staged diff.
5. Explain the reason and validation scope, push the working branch, and open a PR against its target. Merge after checks pass.
6. Confirm the target contains the changes before deleting the temporary branch. Keep the three permanent branches: `main`, `docs/project-guide`, and `gh-pages`.

| Change | Evidence before submission |
| --- | --- |
| Individual algorithm | Compile independently and record sample, boundary, and counterexample results; an AC claim requires an online judge record |
| Template | Provide a minimal caller and boundary checks; explain interface changes |
| Rename or move | Preserve traceable source mappings and check CPH path rules and Pages inclusion rules |
| Archive tools or imported source | Run offline archive tests and the read-only provenance audit below |
| Documentation | Run documentation checks, API contract checks, and a build; see [Maintenance](docs/maintenance.md) |
| Website | Validate assets, snapshots, and affected interactions using [website maintenance](docs/pages.md) |

If the source branch does not yet include documentation checks, run the relevant checks available on that branch and update documentation separately on its branch. Run archive tests and the integrity audit from the repository root:

```sh
python3 -m unittest discover -s scripts/tests -p 'test_archive_*.py'
python3 scripts/archive_verify.py
```

The `archive` check verifies tool behavior and source integrity where provenance receipts exist. It does not execute algorithms or establish an AC verdict. Archive tools read and write local files without committing or pushing Git. New sources enter `main` through a temporary branch such as `sync/` and a PR, preserving existing source files. See the [archive tool guide](scripts/README.md) for operations and receipt formats.

All manual changes require PRs. The documentation publishing workflow writes directly to the generated `gh-pages/docs/` directory. The `Site snapshots` workflow on `main` writes only the generated `gh-pages/data/site-data.json` and `gh-pages/data/recent-commits.json` after main pushes, daily, or on manual runs; PR runs only validate. Both publishers share a lock and preserve pending jobs with `queue: max`. Do not edit generated output manually. Keep the full `gh-pages` branch separate from other branches.

## Commit messages

Use English and the format `type(scope): specific action`, for example:

```text
fix(codeforces): handle disconnected graph in 1000E
feat(atcoder): add abc231 A solution
docs(tooling): explain local relay configuration
refactor(layout): move verified source paths
```

Common types are `feat`, `fix`, `refactor`, `docs`, and `chore`. Include necessary motivation, provenance, and test results in the body. Successful compilation alone does not mean all tests passed.

Stage and review files individually. For a README-only change:

```sh
git add README.md
git diff --cached --check
git diff --cached
git commit -m "docs(readme): clarify repository navigation"
```

## Review notes

Provide the following information, keeping simple changes brief:

```text
Reason: The previous behavior or missing information.
Change: The files changed and resulting behavior.
Validation: Commands or examples actually run, and their results.
Limitations: Any scope not yet verified; omit when unnecessary.
```

For algorithm discussions, provide the problem link, a minimal counterexample, and expected output. Retain attribution and applicable licenses for third-party code. The repository has no single declared license; do not invent permissions for existing code.
