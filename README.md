# CodeFlare · 网站分支

CodeFlare 是以 C++ 为主的算法竞赛源码与模板归档。本分支 `gh-pages` 保存用于浏览归档的静态网站、数据快照，以及由文档分支生成的 `docs/` 网站。

[项目网站](https://codeflare.lucius7.dev) · [源码仓库](https://github.com/xw7qwq/codeflare) · [文档源码](https://github.com/xw7qwq/codeflare/tree/docs/project-guide) · [反馈问题](https://github.com/xw7qwq/codeflare/issues)

## 网站功能

- 按平台筛选，并通过比赛、题号或路径关键词检索源码；展示最近六次主分支提交。
- 题目列表和最近提交分别读取部署快照，再通过 GitHub 公共 API 检查更新；请求有超时限制，在线请求失败时保留可用快照。
- 题解通过 `code.html?path=AtCoder/abc231/a.cpp` 这样的链接在新标签页打开，保留原题库的筛选和滚动位置。路径按目录逐段编码，保留可读的 `/`，旧 `%2F` 链接仍可访问。阅读页支持语法高亮、行号、源码链接和复制原始文本。
- 题库与阅读页共用 `theme.css`，跟随系统明暗主题；文档站通过 VitePress 主题变量适配相同配色与字体。

网站只展示源码，不提供在线编译或评测。文件提交时间来自 Git 历史，不代表 OJ 提交或通过时间；源码存在也不代表已经通过评测。首页不再请求或展示 Rating、贡献日历，快照中的旧统计字段保留用于 API 兼容。

## 分支职责

| 分支 | 内容与修改入口 |
| --- | --- |
| [`main`](https://github.com/xw7qwq/codeflare/tree/main) | 算法源码、模板与开发工具 |
| [`gh-pages`](https://github.com/xw7qwq/codeflare/tree/gh-pages) | 本分支：题库前端、源码阅读页和数据快照 |
| [`docs/project-guide`](https://github.com/xw7qwq/codeflare/tree/docs/project-guide) | 文档 Markdown、API 契约、VitePress 配置和文档发布流程 |

`docs/` 是构建产物，修改文档应从 `docs/project-guide` 开始。文档发布流程仅更新 `gh-pages/docs/`，无需将文档分支合入 `main`。不要将其他分支整体合入本分支。

## 本地预览

需要 Git、Python 3；运行快照生成器与检查脚本还需要 Node.js，建议使用 Node.js 22 或更高版本。网站本身无需安装 npm 依赖或执行构建。

```sh
git clone --branch gh-pages https://github.com/xw7qwq/codeflare.git codeflare-pages
cd codeflare-pages
python3 -m http.server 4173 --bind 127.0.0.1
```

打开 <http://127.0.0.1:4173/>，按 `Ctrl+C` 停止服务。本地页面仍会访问配置的远程源码接口；部署快照用于列表回退，不包含全部源码正文。

## 目录结构

| 路径 | 用途 |
| --- | --- |
| [index.html](index.html)、[app.js](app.js)、[styles.css](styles.css) | 题库页面、检索和列表布局 |
| [code.html](code.html)、[reader.js](reader.js)、[reader.css](reader.css) | 独立源码阅读页 |
| [archive.js](archive.js) | 共享仓库配置、路径解析和请求工具 |
| [theme.css](theme.css) | 共享主题、排版、按钮和键盘焦点 |
| [data/site-data.json](data/site-data.json) | 题目与文件提交时间快照，兼容旧统计字段 |
| [data/recent-commits.json](data/recent-commits.json) | 最近六次提交的说明、时间和链接 |
| [scripts/](scripts/) | 快照生成、阅读页回归与主题检查 |
| [vendor/](vendor/) | 本地 Highlight.js、主题及其许可证 |
| [docs/](docs/) | 文档站构建产物 |
| [CNAME](CNAME)、[.nojekyll](.nojekyll) | GitHub Pages 自定义域名与静态文件发布配置 |

## 维护与校验

The **Site snapshots** workflow on `main` refreshes both JSON snapshots after source changes and once per day, and can also be run manually. It fetches the complete `main` history, generates and checks the snapshots, then publishes only `data/site-data.json` and `data/recent-commits.json` to `gh-pages`. Builds superseded by a newer `main` commit are skipped. Data and documentation publication share the `codeflare-docs-pages` concurrency group with `queue: max` to retain pending publications.

The publisher verifies the repository, checkout branch, Pages source, and custom domain. After pushing with `GITHUB_TOKEN`, it explicitly requests a Pages build and waits for both snapshot blobs and their public JSON contents to match. A later documentation commit with the same snapshots is accepted; an unrelated build or stale public snapshot does not count as a successful deployment. Any push, build, or deployment verification failure fails the workflow. Regression checks run with `node --test scripts/tests/*.test.mjs`.

刷新快照前获取完整的主分支历史。以下命令在本分支根目录运行，会覆盖两份 `data/` JSON 文件：

```sh
git fetch origin main
node scripts/generate-data.mjs origin/main
```

省略参数时生成器读取本地 `main`。浅克隆需先补齐历史，以免提交数量和时间不完整。生成器参数只选择 Git 数据来源，不会修改脚本内固定的仓库或源码分支配置。

网站修改后执行：

```sh
node --check app.js
node --check archive.js
node --check reader.js
node --check scripts/generate-data.mjs
node --check scripts/publish-data.mjs
node --test scripts/tests/*.test.mjs
node scripts/check-reader.mjs
node scripts/check-theme.mjs
git diff --check
```

阅读页检查覆盖路径解析、加载失败回退、行号和原文复制等非浏览器场景；主题检查验证共享样式、静态资源与深浅色文字对比度。检查通过后仍需在浏览器核对筛选、源码阅读和移动端布局。

`check-theme.mjs` 可接收文档分支 `site/.vitepress/theme/style.css` 的路径，以额外核对两个站点的主题映射。完整流程见[网站维护文档](https://github.com/xw7qwq/codeflare/blob/docs/project-guide/docs/pages.md)。

## 部署与域名

CodeFlare 使用独立域名 **[codeflare.lucius7.dev](https://codeflare.lucius7.dev)**，文档位于 **[/docs/](https://codeflare.lucius7.dev/docs/)**。GitHub Pages 发布来源为 `gh-pages` 分支根目录，根目录 [CNAME](CNAME) 保存自定义域名。

题库、阅读页、GitHub API 请求与数据快照统一使用 `xw7qwq/codeflare` 的 `main` 分支。在线接口与快照生成器分别在 [archive.js](archive.js) 和 [scripts/generate-data.mjs](scripts/generate-data.mjs) 声明仓库配置；更换仓库时应同步修改，并重新生成快照。个人资料链接和真实 Git 提交信息独立于项目地址维护。

域名的 DNS CNAME 应指向 `xw7qwq.github.io`；GitHub 仓库的 **Settings → Pages → Custom domain** 应设为 `codeflare.lucius7.dev`，证书签发后开启 **Enforce HTTPS**。这些托管配置独立于仓库文件，维护发布时应核对 DNS、Pages 部署结果及 HTTPS 是否正常。文档站在独立域名下使用 `/docs/` 作为构建基路径，发布时应保留根目录 `CNAME` 和其他网站文件。

## 贡献与许可

网站改动请以 `gh-pages` 为基线提交 Pull Request，说明修改原因、影响范围和实际验证结果；算法源码与文档分别提交到对应分支。详细规范见[贡献指南](https://github.com/xw7qwq/codeflare/blob/docs/project-guide/CONTRIBUTING.md)。

仓库目前未声明统一的开源许可证。引用第三方源码、题面或前端依赖时，应保留来源并遵守其各自许可；Highlight.js 的许可证见 [vendor/HIGHLIGHTJS-LICENSE.txt](vendor/HIGHLIGHTJS-LICENSE.txt)。

## 相关项目

[OJFlare](https://github.com/xw7qwq/ojflare) 使用独立域名 [ojflare.lucius7.dev](https://ojflare.lucius7.dev)，与 CodeFlare 分别维护和部署。
