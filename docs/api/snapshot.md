# GET 数据快照

```http
GET /data/site-data.json
Host: codeflare.lucius7.dev
```

完整请求地址是 `https://codeflare.lucius7.dev/data/site-data.json`；OpenAPI 中使用相对于基地址的 `/data/site-data.json`。

## 请求约定

| 项目 | 约定 |
| --- | --- |
| 方法 | `GET`，只读 |
| 认证 | 无 |
| 查询参数 | 无；不支持 `page`、`limit`、`oj`、`sort` 服务端参数 |
| 请求体 | 无 |
| 成功响应 | `200`，`application/json` |
| 数据范围 | 一份完整的部署快照，不是实时 Git 仓库镜像 |

## 响应结构

```json
{
  "repository": {
    "owner": "xw7qwq",
    "name": "solutions",
    "branch": "main"
  },
  "generatedAt": "2026-09-04T05:48:48.809Z",
  "sourceRef": "main",
  "commitCount": 1,
  "contributions": { "2026-09-02": 1 },
  "problems": [
    {
      "path": "AtCoder/abc231/a.cpp",
      "platform": "AtCoder",
      "language": "C++",
      "title": "ABC 231 · Problem A",
      "problemUrl": "https://atcoder.jp/contests/abc231/tasks/abc231_a",
      "githubUrl": "https://github.com/xw7qwq/solutions/blob/main/AtCoder/abc231/a.cpp",
      "rawUrl": "https://raw.githubusercontent.com/xw7qwq/solutions/main/AtCoder/abc231/a.cpp",
      "submittedAt": "2026-09-02T15:48:58+08:00"
    }
  ],
  "ratings": { "atcoder": null, "codeforces": null }
}
```

此示例用于演示结构，数量和 Rating 不代表线上实况。完整字段定义见[数据模型](models.md)。

## 筛选和排序

筛选由调用方完成，不向接口发送不存在的参数。数组原始顺序不保证按提交日期排列；下面示例把未知时间排在末尾，同一时间按路径排序。

```js
const rankTime = (value) => value === null ? -Infinity : Date.parse(value);
const atcoder = snapshot.problems
  .filter((problem) => problem.platform === 'AtCoder')
  .sort((a, b) => {
    const left = rankTime(a.submittedAt);
    const right = rankTime(b.submittedAt);
    return left === right ? a.path.localeCompare(b.path) : right - left;
  });
```

调用前先做 schema 校验；不要让无效日期静默进入排序。`platform` 是开放字符串，不要把当前平台数量固定为程序常量。

## 缓存与更新

遵循响应实际提供的 `Cache-Control`、`ETag` 等托管平台头部，不假设固定刷新间隔。带条件请求收到 `304` 时复用已有缓存；没有缓存时重新取完整响应。

`generatedAt` 描述快照构建时间，`submittedAt` 描述该条目的 Git 时间。即使源码已经在 `main` 更新，快照仍需重新生成和发布；字段通过校验也不代表题库收录完整。

`commitCount`、`contributions`、`ratings` 作为兼容字段保留，当前首页不再展示。Rating 不再主动刷新，其更新时间不能从 `generatedAt` 推断。仓库最近提交应读取[独立接口](updates.md)。
