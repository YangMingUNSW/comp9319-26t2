# cse-test

存放每次在 **CSE 服务器**上运行 `lencode` / `ldecode` 得到的测试**输出结果**。

## 约定

- 这里**只放 CSE 的测试结果**，不放源码。
  源码（`lencode.c` / `ldecode.c`）只在仓库根目录 `assignment1/` 保留一份，开发改动都在那里进行。
- 每次从 CSE 复制回来的一批结果，单独放进一个**以代码 commit hash 命名的子文件夹**，
  例如 `9a918e8`。这个 hash 就是“生成这批结果的那份代码”所在的提交。
- 好处：结果和代码版本一一对应，要复现/debug 时直接 `git checkout <hash>`
  就能拿回当时的确切代码，比时间戳精确。

## 工作流

1. 在 `assignment1/` 根目录改好代码 → commit + push，记下这次的 commit hash。
2. 把**这个 commit** 的代码拿到 CSE 服务器编译运行，产出测试结果。
3. 把这一批结果复制进 `cse-test/<commit-hash>/`。
