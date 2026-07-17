# cse-test

存放每次在 **CSE db-perftest** 上运行 `bwtsearch` 得到的测试**输出结果**。

## 约定

- 这里**只放 CSE 的测试结果**，不放源码。
  源码（`bwtsearch.c` / `bwt.c` / `bwt.h` / `makefile`）只在 `assignment2/` 根目录保留一份，
  开发改动都在那里进行。
- 每次从 CSE 复制回来的一批结果，单独放进一个**以代码 commit hash 命名的子文件夹**，
  例如 `9a918e8`。这个 hash 就是「生成这批结果的那份代码」所在的提交。
- 好处：结果和代码版本一一对应，要复现/debug 时直接 `git checkout <hash>`
  就能拿回当时的确切代码，比时间戳精确。

## 工作流

1. 在 `assignment2/` 根目录改好代码 → commit + push，记下这次的 commit hash。
2. 把**这个 commit** 的代码拿到 db-perftest 编译运行，产出测试结果。
3. 把这一批结果复制进 `cse-test/<commit-hash>/`。

## A2 建议归档的内容

除了搜索输出本身，A2 的两条红线证据也一起存，方便回头比对哪一版代码把内存跑超了：

| 文件 | 内容 |
|---|---|
| `<name>.out` | `./bwtsearch <file> <term>` 的输出 |
| `<name>.ref` | 对应 `~cs9319/a2/dsearch <txt> <term>` 的参考输出（对拍用，两边已 `sort`） |
| `memory.txt` | `ms_print memory.out` 的 massif 峰值报告（16MB 红线证据） |
| `time.txt` | `/usr/bin/time` 的输出（5s 红线证据） |

**不要**把 `.rbwt` / `.txt` 测试数据本身复制进来——它们最大 110MB，
`~cs9319/a2/` 上本来就有。

测试步骤见 [`../TESTING_ON_CSE.md`](../TESTING_ON_CSE.md)。
