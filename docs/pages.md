# GitHub Pages 维护

[根题库](https://codeflare.lucius7.dev/)展示 main 最近六次提交、题目检索与源码阅读。首页不再展示 Rating 或贡献日历，也不再请求其在线接口。根题库是纯静态网站，不在浏览器中执行题解。[文档站](https://codeflare.lucius7.dev/docs/)使用 VitePress，独立流程见[文档部署](maintenance.md)。

## 分支与发布边界

| 分支 | 职责 | 更新后影响 |
| --- | --- | --- |
| `main` | OJ source archive and snapshot workflow | Pushes regenerate and publish both JSON snapshots; the browser still checks the online file tree, recent commits, and source independently |
| `gh-pages` | 根题库、静态快照、生成的 `docs/` | GitHub Pages 发布来源，目录为 `/(root)` |
| `docs/project-guide` | 文档源、API Schema 与发布校验 | 推送后独立发布至 `gh-pages/docs/`；无需向 main 推送 |

不要把 main 或文档分支整体合入 gh-pages。网站源码修改基于 gh-pages；文档修改基于 docs/project-guide。文档工作流只更新生成的 docs 目录，保留根题库。

The `Site snapshots` workflow on main generates `data/site-data.json` and `data/recent-commits.json` from the current `origin/main` after main pushes, daily schedules, and manual **Run workflow** runs. PRs targeting main validate the current main snapshots and website without publishing. The generation job uploads both verified JSON files; the publishing job shares the `codeflare-docs-pages` lock with documentation publishing, commits only those two files, explicitly requests a Pages build, and verifies the public JSON. Both publishers use `queue: max` to preserve pending jobs; superseded main snapshots skip publication.

## 网站文件

路径相对于 gh-pages 根目录：

```text
index.html                    题库页面与最近提交列表
theme.css                     共享主题、字体、间距与基础组件
styles.css                    题库布局
app.js                        检索、排序和独立在线检查
archive.js                    共享路径校验、题目解析与请求工具
code.html                     独立全屏源码阅读页
reader.js                     源码、提交时间、复制与高亮
reader.css                    全视口阅读布局与高亮适配
data/site-data.json            题库快照，保留旧统计字段
data/recent-commits.json       最近六次提交快照
scripts/generate-data.mjs      从 Git 文件树和历史生成两份快照
scripts/publish-data.mjs       Publish only the two snapshots and verify Pages and public JSON
scripts/check-reader.mjs       非浏览器阅读页与题库回归测试
scripts/check-theme.mjs        样式共享、资源引用与深浅色对比度校验
vendor/                       本地 Highlight.js、主题及许可证
favicon.svg                   网站图标
.nojekyll                     直接提供静态资源
docs/                         自动发布的文档网站，勿手工修改
```

## 生成与预览

需要 Git、Node.js（建议受支持的 LTS）和 Python 3。保留完整 Git 历史，示例工作目录应尚不存在：

```bash
git fetch origin main gh-pages
git worktree add --detach ../codeflare-pages origin/gh-pages
cd ../codeflare-pages
git switch -c pages/update-snapshot
node scripts/generate-data.mjs origin/main
python3 -m http.server 4173 --bind 127.0.0.1
```

浏览[本地预览](http://127.0.0.1:4173/)，结束时按 Ctrl+C。省略生成器参数时读取本地 main；建议使用刚获取的 origin/main。参数不会改变源码链接中的 main 分支。浅克隆先补齐历史，否则提交数量和时间不完整。

生成器写入两份 JSON。题库的 `commitCount`、`contributions` 继续生成以兼容旧契约；`ratings` 仅保留已有历史值，未知时为 null，不再请求 Rating 服务。生成时刻不等于 Rating 更新时间。

## 页面更新机制

题目与最近提交是两个独立流程：各自先读部署快照，再检查 GitHub；单个 JSON 请求最多 6 秒，使用缓存重新验证。失败的在线请求保留该列表的快照并注明，不会阻塞另一列表。快照与在线请求都失败时停止加载提示，显示错误入口。

| 内容 | 来源与规则 |
| --- | --- |
| 最近提交 | `commits?sha=main&per_page=6`；显示消息首行、SHA 与提交者时间，点击进入提交详情 |
| 提交顺序 | Git/API 返回顺序，最多六次，含合并提交；不按日期二次排序 |
| 题目列表 | `git/trees/main?recursive=1`；拒绝被截断的树；按路径识别 OJ、题号和语言，每页 60 条 |
| 文件提交时间 | 生成器遍历 Git 历史，记录每个路径首次遇到的 `%cI`（提交者时间），不是 OJ 评测时间 |
| 默认题目排序 | `submittedAt` 从新到旧，相同时间按路径，未知时间置后 |

显示日期使用浏览器本地时区。最近提交的时间是 `commit.committer.date`，不是快照生成时间，也不是作者日期。提交说明作为纯文本插入页面。

The online file tree does not contain per-file commit times, so existing files keep their snapshot dates. Before automatic snapshot publication finishes, newly added files with no snapshot record show an unknown time, and changed files retain their previous snapshot dates. The status therefore reports that the code list was checked, without claiming that every field is current. Source content is fetched separately over the network.

## 路径解析与完整性

`archive.js` 与生成器各有 `problemFromPath`；题库和阅读页共用前者，新增 OJ 或改路径时检查两份规则：

- 子目录内支持 cpp、cc、cxx、c、py、java、rs、go、kt；第一段作为平台。
- 排除 Templates、.cph、.vscode、.github、assets、data、scripts；根目录独立源码不收录。
- 部分平台只能推导比赛页或平台首页，不保证每条均为精确原题地址。
- 未排除的新目录也可能被当作 OJ，需核对实际语义。

旧生成器没有用 NUL 分隔 Git 文件树，导致中文等非 ASCII 路径转义后漏收。现改用 `git ls-tree -r -z --name-only`。对 main 的 `7cca2a6` 核对：新快照和真实可识别路径均为 **1,291** 条，全部有提交日期；旧快照为 1,257 条。补齐的 34 条分布为 HDU 4、L7OJ 27、NowCoder 3。该数量是此提交的核对记录，不是契约固定值。

## 源码与高亮

题目行使用原生链接，在新标签页打开 `code.html?path=...`；原题库的筛选和滚动位置不变。阅读页占据完整视口，源码区域独立横纵滚动，无半屏抽屉、遮罩或关闭按钮。可直接复制阅读页地址访问；左上 QQ 头像链接返回题库，不依赖上一页历史或原标签页对象。

阅读页校验路径后立即请求 GitHub Contents API（5 秒超时），失败时并行尝试 jsDelivr 和 raw.githubusercontent.com（各 7 秒），取首个成功结果。均失败则结束加载状态，提供重试按钮及 GitHub 源码入口。提交时间从快照独立读取，失败或缺失时显示未知，不阻塞代码。

仅接受合法的仓库相对源码路径；URL 参数解码一次，不允许空段、路径穿越、反斜杠或控制字符。仓库与 main 分支固定，不接受自定义源码服务器地址。

本地 Highlight.js 11.12.0 与 GitHub 浅色／深色主题跟随系统外观，失败回退纯文本。渲染时统一换行，行号不将文件末尾的换行符计为额外一行；复制保留原始完整内容（包括 CRLF）。当前阅读页保留源码原文供复制，不再使用题库按路径索引的内存缓存。保留 `[hidden] { display: none !important; }`，避免成功后仍显示加载提示。

## 视觉规范

题库与阅读页必须先加载同一版本的 `theme.css`，再加载页面布局样式。配色、字体、字号级别、间距、圆角、按钮、焦点和系统外观仅在共享主题定义；`styles.css` 与 `reader.css` 不得重新定义主题或写入独立色值。CSS 变量按用途命名，不按某个页面命名。

- 使用 `theLucius7` 的 QQ 头像（QQ 号 `3012967200`）、绿色强调色和低对比度边框；头像通过 HTTPS 直接加载，不使用第三方代理。预留 36px 尺寸并圆形显示，不使用 L7 文字标识。正文及辅助文字与所在背景的对比度至少 4.5:1。
- 正文和代码为 16px，控件标签 14px、辅助信息 12px（均使用 rem）；代码与行号必须共用字体、行高和上下内边距。
- 面板圆角 16px，控件 10px，标签 6px；按钮最小高度 44px。间距使用共享 4px 基准变量。
- 选中、悬停、禁用、键盘焦点和隐藏状态必须保留；不能仅用颜色表达筛选状态，使用 `aria-pressed`。
- 根站通过 `prefers-color-scheme` 跟随系统，不依赖 JS；尊重减少动态效果设置。移动端将路径与日期分行，代码区域保持独立滚动。
- 文档分支的 `site/.vitepress/theme/style.css` 将同一配色与字体映射到 VitePress 变量；保留文档布局及其系统／手动主题能力，不加载根站 CSS reset。修改根站主题时须同步核对此适配。

跨分支核对时，可向 `node scripts/check-theme.mjs` 传入文档源中 `site/.vitepress/theme/style.css` 的绝对路径；脚本会校验两套主题的颜色与字体映射一致。

## 校验与发布

```bash
node --check app.js
node --check archive.js
node --check reader.js
node --check scripts/generate-data.mjs
node scripts/check-reader.mjs
node scripts/check-theme.mjs
git diff --check
curl --fail --head http://127.0.0.1:4173/
curl --fail --head http://127.0.0.1:4173/data/recent-commits.json
```

Both JSON files must pass [API validation](api/standards.md). Publish website source changes through PRs targeting gh-pages, preserving the docs directory. Snapshots normally need no manual commit: `Site snapshots` generates them after main pushes, and can be rerun manually from main when needed. Publish documentation and contract changes through PRs targeting docs/project-guide. When adding a data endpoint, publish its data files before running documentation checks that depend on the remote snapshots.

```bash
gh api repos/xw7qwq/codeflare/pages --jq '{status,source,https_enforced,html_url}'
gh api repos/xw7qwq/codeflare/pages/builds/latest --jq '{status,commit,error}'
curl --fail --head https://codeflare.lucius7.dev/
```

构建成功后核对公共资源与预期版本，不能把本地语法检查当作已上线。

## 常见故障

| 现象 | 处理 |
| --- | --- |
| 新题目未出现 | 检查 main 路径、排除规则、树接口是否失败或截断 |
| File dates are unknown or stale | Inspect main's `Site snapshots` generation, publication, Pages, and public JSON checks; fix a failed run and rerun it manually from main |
| 最近提交保持旧数据 | 在线接口可能超时或限流，页面会注明使用部署快照；通过“全部提交”核对 GitHub |
| 代码已显示但仍有加载文字 | 核对 code.html、reader.js、reader.css、theme.css 同次发布，以及 hidden 样式 |
| 代码加载失败 | 检查 Contents、jsDelivr、raw 来源；打开 GitHub 源码链接 |
| 部署后仍为旧页面 | 核对 Pages 构建提交、公共资源与缓存，不能只靠 URL 查询参数判断部署 |
