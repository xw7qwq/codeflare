# 源码与外部接口

当前在线接口属于 GitHub，不属于本站 API。访问限制和返回约定以对应服务为准。

## 源码文本

快照不包含源码正文。可以打开 `githubUrl` 查看 GitHub 文件，或读取 `rawUrl` 获取文本。程序化获取时也可使用 GitHub Contents API：

```sh
curl --fail --location \
  -H 'Accept: application/vnd.github.raw+json' \
  -H 'X-GitHub-Api-Version: 2022-11-28' \
  'https://api.github.com/repos/xw7qwq/solutions/contents/AtCoder/abc231/a.cpp?ref=main'
```

此请求返回原始文本，不能按默认 JSON / Base64 响应解析。文件路径应逐段 URL 编码，保留 `/` 分隔；源码仅用于阅读，不应在接入时自动执行。

题库条目在新标签页打开独立源码阅读页。阅读页先请求 Contents API（5 秒超时），请求失败或超时后并行尝试 jsDelivr 和 `rawUrl`，两路各 7 秒超时。源码与快照提交时间并行读取，元数据失败不阻塞源码。当前阅读页保留原始文本供复制，没有按路径缓存、持久化缓存、TTL 或 SHA 完整性验证。完整实现边界见[网站维护](../pages.md)。

## 数据依赖

| 来源 | 请求 | 用途 |
| --- | --- | --- |
| GitHub 文件树 | `GET https://api.github.com/repos/xw7qwq/solutions/git/trees/main?recursive=1` | 在线题目路径；大仓库需检查 `truncated` |
| GitHub 最近提交 | `GET https://api.github.com/repos/xw7qwq/solutions/commits?sha=main&per_page=6` | main 最近六次提交；读取 `sha`、`commit.message` 首行、`commit.committer.date` |

[GitHub 官方提交接口](https://docs.github.com/en/rest/commits/commits#list-commits)。两个列表独立加载，快照和在线 JSON 请求均设置 6 秒超时并重新验证缓存；题库拒绝 `truncated: true` 的文件树，防止部分数据覆盖完整快照。

## 历史兼容字段

`site-data.json` 仍包含 `commitCount`、`contributions` 和 `ratings`，避免破坏旧契约。首页不再渲染或请求提交日历、AtCoder、Codeforces Rating。生成器仅保留已有 Rating，未知时为 `null`，不再调用外部 Rating 服务；`generatedAt` 不代表这些历史 Rating 的更新时间。

## 请求限制

GitHub REST API 有匿名与认证请求限制，遇到限额时读取实际的限流响应头，遵循服务返回的重试要求。不要在公开静态页面内嵌个人访问令牌，也不要用并发重试绕过限制。[GitHub 官方限流说明](https://docs.github.com/en/rest/using-the-rest-api/rate-limits-for-the-rest-api)

外部调用失败时，接入者应保留已验证的数据并显示更新时间；不要把“请求失败”改写成“当天没有提交”或“Rating 为零”。
