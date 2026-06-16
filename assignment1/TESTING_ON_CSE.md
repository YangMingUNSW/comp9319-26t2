# COMP9319 A1 在 CSE 上测试指南（中文）

这份文档用于你在 UNSW CSE Linux 上验证 `lencode` / `ldecode`，再提交作业。

## 1）登录 CSE 并进入作业目录

```bash
ssh zID@login.cse.unsw.edu.au
cd /path/to/your/assignment1
```

> 把 `zID` 和路径改成你自己的。

## 2）先编译程序

如果你交 C 版本（`.c`）：

```bash
gcc -O2 -Wall -Wextra -std=c11 -o lencode lencode.c
gcc -O2 -Wall -Wextra -std=c11 -o ldecode ldecode.c
```

如果你交 C++ 版本（`.cpp`）：

```bash
g++ -O2 -Wall -Wextra -std=c++17 -o lencode lencode.cpp
g++ -O2 -Wall -Wextra -std=c++17 -o ldecode ldecode.cpp
```

## 3）跑官方 sanity test（最低要求）

```bash
~cs9319/a1/autotest
```

这一步是最基础的检查，提交前建议至少通过它。

## 4）做 round-trip 验证（最关键）

题目最核心要求是：

`原文件 -> lencode -> ldecode -> 原文件`，且输出必须逐字节完全一致。

示例：

```bash
./lencode ~cs9319/a1/test1.txt test1.enc
./ldecode test1.enc test1.dec
diff ~cs9319/a1/test1.txt test1.dec
```

如果 `diff` 没有任何输出，就表示这组通过。

## 5）快速检查编码格式

```bash
xxd -b test1.enc | head
```

你应该能看到：

- 普通 ASCII 字符对应字节通常是最高位 `MSB=0`
- 字典索引是 2 字节表示，且高字节最高位 `MSB=1`

## 6）性能与大文件测试（避免被卡 5 秒）

评测里单个测试超过 5 秒会被终止，所以要额外测：

- 重复模式数据（看压缩与查找是否稳定）
- 随机 7-bit ASCII 数据（看鲁棒性）
- 接近 1MB 的输入（看性能上限）

这能更早发现「字典查找过慢」或「字典满后 reset 同步错误」。

## 7）提交前最终检查

先检查你当前提交内容：

```bash
9319 classrun -check a1
```

再提交：

```bash
give cs9319 a1 lencode.c ldecode.c
```

或者（C++）：

```bash
give cs9319 a1 lencode.cpp ldecode.cpp
```
