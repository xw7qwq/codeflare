# CodeFlare · Project documentation

CodeFlare is a competitive programming archive of C++ solutions, templates, and practice, organized by online judge and contest. This branch, `docs/project-guide`, maintains project guides, the static problem browser's API contracts, and the VitePress documentation site.

[Website](https://codeflare.lucius7.dev) · [Online documentation](https://codeflare.lucius7.dev/docs/) · [Source repository](https://github.com/xw7qwq/solutions) · [Source catalog](docs/catalog.md) · [Contributing](CONTRIBUTING.md)

## Documentation

| Goal | Guide |
| --- | --- |
| Integrate problem data and look up fields or error conventions | [API reference](docs/api/index.md) |
| Find platforms, templates, and tools | [Project structure](docs/structure.md), [generated source catalog](docs/catalog.md) |
| Compile a problem locally | [Quick start](docs/quick-start.md) |
| Add a solution and record validation | [Code and archive conventions](docs/conventions.md), [Contributing](CONTRIBUTING.md) |
| Configure an editor and problem imports | [Development tools](docs/tooling.md) |
| Update documentation or maintain the website | [Maintenance](docs/maintenance.md), [GitHub Pages maintenance](docs/pages.md) |
| Browse all documentation topics | [Documentation index](docs/README.md) |

The API documentation describes static JSON snapshots and external interfaces used by the website, not an online judging service. Download OpenAPI and JSON Schema contracts from the [API reference](docs/api/index.md).

## Branch responsibilities

| Branch | Contents |
| --- | --- |
| [`main`](https://github.com/xw7qwq/solutions/tree/main) | Algorithm source code, templates, and existing development tools |
| [`docs/project-guide`](https://github.com/xw7qwq/solutions/tree/docs/project-guide) | This branch: documentation source, API contracts, project conventions, and build checks |
| [`gh-pages`](https://github.com/xw7qwq/solutions/tree/gh-pages) | Problem browser, data snapshots, and generated documentation in `docs/` |

Documentation does not need to be merged into `main`. After a documentation PR merges into this branch, the workflow builds the site and updates only `gh-pages/docs/`. The source catalog records the default-branch commit it describes; see [Maintenance](docs/maintenance.md) for refresh instructions.

## Quick start

### Read and check Markdown

Read the documentation directly on GitHub or in a Markdown reader. The checker requires Git and Python 3.10+, with no third-party Python packages.

```sh
git clone --branch docs/project-guide https://github.com/xw7qwq/solutions.git codeflare-docs
cd codeflare-docs
python3 scripts/docs.py check
```

Keep complete Git history and remote branch references. Catalog and API checks read a recorded commit and `origin/gh-pages`. Compiling an individual solution requires its language toolchain; see [Quick start](docs/quick-start.md).

### Preview the documentation site

Run from the repository root with Node.js 22+ and npm:

```sh
npm --prefix site ci
npm --prefix site run dev
```

Open <http://127.0.0.1:4174/docs/>. Stop the server with `Ctrl+C`.

The development server converts Markdown into `site/.content/` at startup. After editing source documents, run `npm --prefix site run prepare:docs` in another terminal to regenerate page content.

## Repository layout

| Path | Purpose |
| --- | --- |
| [README.md](README.md), [CONTRIBUTING.md](CONTRIBUTING.md) | Project entry point and contribution guidelines |
| [docs/](docs/) | Guides, API documentation, and source catalog |
| [scripts/docs.py](scripts/docs.py) | Markdown, relative link, heading anchor, and source catalog checks |
| [scripts/README.md](scripts/README.md), [archive/submissions.json](archive/submissions.json) | Archive tools, offline tests, and verified submission receipts |
| [site/package.json](site/package.json) | Site dependencies and development and build commands |
| [site/.vitepress/](site/.vitepress/) | Site configuration and theme |
| [site/public/api/](site/public/api/) | OpenAPI and JSON Schema contracts |
| [site/prepare.mjs](site/prepare.mjs), [site/check-build.mjs](site/check-build.mjs) | Content conversion and build output checks |
| [site/publish.mjs](site/publish.mjs) | Documentation publishing and live verification |
| [.github/workflows/docs.yml](.github/workflows/docs.yml) | Documentation validation, build, and publishing workflow |
| [.github/workflows/archive.yml](.github/workflows/archive.yml) | Offline archive tests and source integrity audit |

`site/.content/`, `site/.vitepress/dist/`, and `site/node_modules/` are generated directories. Edit Markdown sources and configuration, and keep generated directories out of commits. [Project structure](docs/structure.md) and the [source catalog](docs/catalog.md) define source categories and counts.

## Validation and contributions

After installing site dependencies, run from the repository root:

```sh
python3 scripts/docs.py check
npm --prefix site run check:api
npm --prefix site run build
git diff --check
```

These commands validate documentation and the catalog, API contracts and snapshots, the static site build, and whitespace. By default, API checks read two snapshots from local `origin/gh-pages`. They do not judge algorithms or establish that the remote site was deployed successfully.

Create a working branch from `docs/project-guide`, stage changes by path, and describe the reason, changes, and validation in a pull request. Track new Markdown files and local link targets in Git first. See [Contributing](CONTRIBUTING.md) for the full process.

## Domain and deployment

CodeFlare uses **[codeflare.lucius7.dev](https://codeflare.lucius7.dev)**. Documentation is published under [/docs/](https://codeflare.lucius7.dev/docs/), and the static API lives under `/data/`.

GitHub Pages should publish from the root of `gh-pages`, with `codeflare.lucius7.dev` as its custom domain. The publishing script validates the repository, source branch, and domain, then updates only the managed `docs/` directory, preserving the problem browser and domain configuration.

API collection `2.0.0` uses the new domain and the `xw7qwq/solutions` repository identity while retaining response field structures. See [Errors and compatibility](docs/api/standards.md) for migration details. The source catalog retains its original commit SHA, timestamp, and counting rules.

The workflow checks every PR targeting `docs/project-guide` and every push to that branch. Only pushes publish. Verify releases through [Actions](https://github.com/xw7qwq/solutions/actions), public pages, and `docs/build-info.json`; see [Maintenance](docs/maintenance.md) for details.

## Content and licensing

Each algorithm source file is used independently; there is no single executable for the entire repository. The archive includes unfinished drafts and template fragments without `main`. A file's presence in Git or on a website does not mean it passed an online judge. Consult the original judge for the problem statement.

The repository has no single declared open-source license. Retain attribution and follow the applicable licenses when using problem statements, third-party code, or dependencies. This documentation grants no additional permissions.

## Related projects

[OJFlare](https://github.com/xw7qwq/ojflare) uses [ojflare.lucius7.dev](https://ojflare.lucius7.dev) and is maintained and deployed separately.
