# 快速开始

## 获取代码和文档

```sh
git clone --branch docs/project-guide https://github.com/xw7qwq/solutions.git
cd solutions
python3 scripts/docs.py check
```

文档分支包含建立或最近同步时的主分支代码。若只需要最新算法代码，克隆默认 `main`；文档和校验脚本目前在 `docs/project-guide`。先执行 `git status --short --branch` 确认所在位置。

## 编译一道 C++ 题

需要 Git、shell 和 GNU C++ 编译器。大量代码使用 GNU 的 `bits/stdc++.h`；macOS 的 `/usr/bin/g++` 通常实际调用 Apple Clang，不能据名称认定它支持 GNU 头文件。已有编辑器任务使用 GNU++23；示例也采用这一标准，实际提交时以 OJ 支持的标准为准。

在仓库根目录执行，先选好本机编译器：

```sh
# Linux 或已把 GNU GCC 加入 PATH 的环境。
CP_CXX=g++

# macOS 若 GNU GCC 命令为 g++-16，则改用下面一行。
# CP_CXX=g++-16

"$CP_CXX" --version
cp_build_dir=$(mktemp -d)
"$CP_CXX" -std=gnu++23 -O2 -Wall -Wextra \
  AtCoder/abc231/a.cpp -o "$cp_build_dir/abc231-a"
printf '1000\n' | "$cp_build_dir/abc231-a"
```

预期输出：

```text
10.000000
```

这是该题的一个最小运行检查，不代表测试了所有边界或整个仓库。产物位于本机临时目录，不会混入源码。

## 验证自己的修改

先编译对应文件，再运行官方样例、最小/最大边界和针对算法的反例。可以在上述同一个 shell 中保存样例并比较：

```sh
printf '1000\n' > "$cp_build_dir/input.txt"
printf '10.000000\n' > "$cp_build_dir/expected.txt"
"$cp_build_dir/abc231-a" < "$cp_build_dir/input.txt" > "$cp_build_dir/actual.txt"
diff -u "$cp_build_dir/expected.txt" "$cp_build_dir/actual.txt"
```

无差异时 `diff` 返回 0。浮点题应按题目容差判断，交互题需要对应交互器，不能套用普通文件比较。涉及越界或未定义行为时，可另外使用支持 AddressSanitizer / UBSan 的编译器构建调试版本。

仓库中的 `.c` 使用适合题目的 C 编译器单独编译；模板片段需要调用程序。不要把全仓所有源码链接成一个程序。

## 常见问题

| 现象 | 处理 |
| --- | --- |
| `bits/stdc++.h` 找不到 | 检查编译器实际版本；选择已安装的 GNU GCC |
| VS Code 的构建命令找不到 | 当前任务依赖作者本机 wrapper；先按本页命令编译，再按[工具文档](tooling.md)改本地任务 |
| 模板提示缺少 `main` | 模板不是独立题解，为它编写最小调用程序 |
| 程序不结束或没有输出 | 检查输入格式、测试组数和是否为交互题；部分文件本身未完成 |
| 文档检查提示缺少源提交 | 使用完整 Git 克隆；按[维护流程](maintenance.md)补齐历史 |

[返回文档导航](README.md)
