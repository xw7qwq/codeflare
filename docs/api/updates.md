# GET 最近提交

```http
GET /data/recent-commits.json
Host: codeflare.lucius7.dev
```

## 请求约定

| 项目 | 约定 |
| --- | --- |
| 方法与认证 | 公开 `GET`，只读，无密钥 |
| 查询参数、请求体 | 均无；不支持服务端分页或筛选 |
| 成功响应 | `200`，`application/json` |
| 数据范围 | 生成时所选 main 历史中的最近六次提交，包含合并提交 |

[读取线上快照](https://codeflare.lucius7.dev/data/recent-commits.json) · [下载 JSON Schema](../../site/public/api/recent-commits.schema.json)

## 响应示例

```json
{
  "repository": {
    "owner": "xw7qwq",
    "name": "solutions",
    "branch": "main"
  },
  "generatedAt": "2026-09-08T01:00:00Z",
  "sourceRef": "origin/main",
  "commits": [
    {
      "sha": "7cca2a682247670b87af2c8bb9755acda9e434e5",
      "subject": "example",
      "committedAt": "2026-09-04T02:54:39Z",
      "url": "https://github.com/xw7qwq/solutions/commit/7cca2a682247670b87af2c8bb9755acda9e434e5"
    }
  ]
}
```

示例仅说明结构，`subject` 和生成时间不是线上实况。字段定义见[数据模型](models.md)。

## 时间与顺序

- `committedAt` 来自 Git 提交者时间 `%cI`，在线接口对应 `commit.committer.date`；不是作者日期，也不是 OJ 提交时间。
- `subject` 是完整提交消息的第一行，必须作为纯文本展示，不解释为 HTML。
- 保持 `git log -6` / GitHub 返回顺序，不按时间戳二次排序，不过滤合并提交；异常 Git 时钟可能导致时间不严格递减。
- `generatedAt` 是快照生成时间。若展示最近提交时间，应读取首条记录的 `committedAt`。
- 空数组合法；提交 SHA 必须唯一、完整且小写，链接必须指向该 SHA。

## 缓存与失败

此端点是部署快照，不会随 main 的每次 push 自动刷新。根题库先读快照，再独立请求 GitHub 最近提交接口；每个请求最多 6 秒，失败时保留快照并注明。调用方遵循实际缓存响应头，HTTP 错误、无效 JSON 和 Schema 失败不得覆盖有效数据。

错误响应沿用[错误规范](standards.md)：可能是 GitHub Pages 的 HTML，不承诺 JSON 错误体。实时接口属于[外部依赖](dependencies.md)，不属于本站端点。
