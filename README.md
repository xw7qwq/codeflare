# CodeFlare

Lucius7's competitive programming source code, templates, and practice archive. Most programs use C++ and are organized by Online Judge (OJ) and contest. A companion static website provides source search and reading.

[Website](https://codeflare.lucius7.dev) · [Documentation](https://codeflare.lucius7.dev/docs/) · [API reference](https://codeflare.lucius7.dev/docs/api/) · [Algorithm templates](Templates/templates) · [Archive tools](scripts/README.md) · [Report an issue](https://github.com/xw7qwq/codeflare/issues) · [OJFlare](https://ojflare.lucius7.dev)

## Contents

- **Problem source code**: standalone programs organized by platform, contest, and problem ID, covering AtCoder, Codeforces, QOJ, Luogu, CSES, and other judges.
- **Algorithm templates**: common data structures, graph algorithms, string algorithms, and number theory implementations in [`Templates/templates/`](Templates/templates).
- **Online reader**: filter by platform, contest, and keyword, with syntax highlighting, line numbers, copying, and raw file links.
- **Traceable imports**: archive tools verify submission identity and source integrity, and retain provenance, verdicts, file sizes, and SHA-256 hashes.

This is a personal learning and contest record. It includes unfinished drafts, submissions without an accepted (AC) verdict, and template fragments without `main`. A file's presence does not imply that it passed judging; validate each program against its original problem.

## Quick start

Browsing source files requires no dependencies. Running C++ programs requires GNU G++; archive tools require macOS or Linux, Python 3.10+, and Git. Choose the compilation standard required by each source file.

```sh
git clone https://github.com/xw7qwq/codeflare.git
cd codeflare
```

For example, compile and run a standalone C++ program:

```sh
g++ -std=gnu++17 -O2 -Wall -Wextra AtCoder/abc052/b.cpp -o /tmp/codeflare-abc052-b
/tmp/codeflare-abc052-b <<'EOF'
5
IIDID
EOF
```

The expected output is `2`. This example uses the GNU standard library header `bits/stdc++.h`. The `g++` bundled with macOS is usually Apple Clang; replace it with the GNU G++ executable installed on your machine.

There is no single application entry point or repository-wide build command. To run another problem, replace the source path and check the input and output against the relevant judge.

## Repository layout

| Path | Contents |
| --- | --- |
| [`AtCoder/`](AtCoder), [`Codeforces/`](Codeforces), [`QOJ/`](QOJ) | Source code organized by contest, such as `AtCoder/abc052/b.cpp` |
| [`Luogu/`](Luogu), [`CSES/`](CSES), [`NowCoder/`](NowCoder), and others | Problems and practice from other judges, preserving the existing directory layout |
| [`Templates/`](Templates) | Algorithm templates and fragments |
| [`archive/submissions.json`](archive/submissions.json) | Submission provenance and source integrity records verified by the importer |
| [`archive/contest-mappings.json`](archive/contest-mappings.json) | Supplemental contest mappings checked against official problem lists |
| [`scripts/`](scripts) | Import planning, source import, integrity auditing, and offline tests |
| [`.cph/scripts/`](.cph/scripts) | Competitive Companion / CPH helper scripts |
| [`.vscode/`](.vscode) | The author's VS Code and CPH configuration |

The VS Code settings include absolute paths and compiler settings from the author's machine. Adjust them for your local paths, compiler, and platform before use. Command-line compilation does not require these editor settings.

## Source imports and verification

The importer handles AtCoder, Codeforces, and QOJ submissions for `Lucius7`. For each problem, it prefers the latest AC submission, or the latest attempt if none was accepted. It does not overwrite existing source files. See the [archive tool guide](scripts/README.md) for candidate planning, normal browser imports, historical QOJ recovery, and resuming interrupted work.

Run the read-only integrity audit from the repository root:

```sh
python3 scripts/archive_verify.py
```

This command checks paths, hashes, byte counts, and encodings against provenance receipts. It does not access the network or execute problem source code. It verifies imported file integrity, not algorithm correctness, and does not invent provenance for historical files.

After changing archive tools, run the existing offline tests:

```sh
python3 -m unittest discover -s scripts/tests -p 'test_archive_*.py'
```

## Website and documentation maintenance

Choose the branch that owns the content before making changes:

| Branch | Contents and entry points |
| --- | --- |
| [`main`](https://github.com/xw7qwq/codeflare/tree/main) | This README, algorithm source code, templates, and archive tools |
| [`gh-pages`](https://github.com/xw7qwq/codeflare/tree/gh-pages) | Problem browser, source reader, data snapshots, and generated documentation; [website maintenance guide](https://github.com/xw7qwq/codeflare/blob/gh-pages/README.md) |
| [`docs/project-guide`](https://github.com/xw7qwq/codeflare/tree/docs/project-guide) | Documentation source, API contracts, and project conventions; [documentation maintenance guide](https://github.com/xw7qwq/codeflare/blob/docs/project-guide/README.md) |

The website is **[codeflare.lucius7.dev](https://codeflare.lucius7.dev)**. GitHub Pages publishes from the root of `gh-pages`. Its `docs/` directory is generated output; edit documentation on `docs/project-guide`. Each branch's README describes local previews, builds, and validation.

The `Site snapshots` workflow refreshes `gh-pages/data/site-data.json` and `gh-pages/data/recent-commits.json` after each push to `main`, once daily, or through **Run workflow**. It validates the generated snapshots and website, publishes only those two files, explicitly requests a Pages build, and verifies the public JSON. PR runs validate without publishing. The snapshot and documentation publishers share a lock so their outputs are preserved.

## Contributing and feedback

Use [issues](https://github.com/xw7qwq/codeflare/issues) or pull requests to report source problems, directory mapping errors, or documentation improvements.

1. Identify the affected files, original problem link, and observed problem. For algorithm fixes, include reproduction input, expected output, and validation results.
2. Preserve the existing platform, contest, and problem ID conventions. Keep templates separate from complete programs, and avoid unrelated bulk renaming or formatting.
3. Provenance records are generated by the tools. Do not label unverified results as AC. Follow the [archive tool guide](scripts/README.md) when importing submissions.
4. Review the diff before committing. Exclude build output, local samples, temporary caches, and credentials. Run the offline tests above when changing tools.

Read this branch's [contribution guide](CONTRIBUTING.md) before submitting. Detailed [code and archive conventions](https://github.com/xw7qwq/codeflare/blob/docs/project-guide/docs/conventions.md) live on the documentation branch. All manual changes go through a pull request targeting the relevant branch. The `Archive` workflow runs offline archive tests and the integrity audit.

## Related projects

| Project | Purpose | Website |
| --- | --- | --- |
| [CodeFlare](https://github.com/xw7qwq/codeflare) | Algorithm source and template archive with online reading | [codeflare.lucius7.dev](https://codeflare.lucius7.dev) |
| [OJFlare](https://github.com/xw7qwq/ojflare) | Cross-platform submissions, contest catalogs, and personal solving statistics | [ojflare.lucius7.dev](https://ojflare.lucius7.dev) |

## License

This repository does not currently declare a single open-source license. Original problems and third-party materials remain subject to their respective owners' rights. Confirm the applicable permissions before redistribution or use in another project.
