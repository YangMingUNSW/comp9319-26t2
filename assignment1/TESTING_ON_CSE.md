# COMP9319 A1 在 CSE 上测试指南（中文）

在 UNSW CSE Linux 上验证 `lencode` / `ldecode`，再提交作业。所有步骤以 `spec.md` 为准。

## 1）登录 CSE 并进入源码目录

```bash
ssh zID@login.cse.unsw.edu.au
cd /path/to/your/assignment1
```

> 把 `zID` 和路径改成你自己的；这个目录里要有 `lencode.c` 和 `ldecode.c`。

## 2）用【评测同款命令】编译

评测机用的就是下面这两条裸命令（spec 第 90 行）。必须用它们编译通过，
**编译失败 = 整个作业 0 分**（spec 第 101 行）。不要加 `-O2`/`-Wall`/`-std=` 等任何 flag，
否则你测的命令和评测的命令不一致。

```bash
gcc -o lencode lencode.c
gcc -o ldecode ldecode.c
```

## 3）跑官方 sanity test

进入放源码的目录，直接运行（spec 第 77 行）：

```bash
~cs9319/a1/autotest
```

提交前至少要通过它。

## 4）做 round-trip 验证

核心要求：`原文件 -> lencode -> ldecode -> 原文件`，逐字节完全一致（spec 第 11–18 行）。

```bash
./lencode ~cs9319/a1/test7.txt test7.encoded
./ldecode test7.encoded test7.decoded
diff ~cs9319/a1/test7.txt test7.decoded
```

`diff` 没有任何输出就表示这组通过。若你当前主要在排查 `test7`，建议优先固定测它。

也可以顺便验证你的编码结果和官方 `.lzw` 一致：

```bash
./lencode ~cs9319/a1/test7.txt test7.encoded
diff test7.encoded ~cs9319/a1/test7.lzw
```

## 5）检查编码格式

```bash
xxd -b test7.encoded | head
```

应能看到（spec 第 20 行）：

- ASCII 字符是单字节，最高位 `MSB=0`
- 字典索引是 2 字节，高字节最高位 `MSB=1`，高字节先写

## 6）性能与大文件测试

单个测试超过 5 秒会被终止；目标文件最大约 1 MB（spec 第 101、105 行）。
本期相比旧版唯一的新要求是「字典满 32,768 条时 reset」（spec 第 7 行），
所以要用足够大、重复模式足够多的输入把字典撑满来触发 reset，再做 round-trip 验证。

## 7）提交前最终检查

先检查当前提交内容：

```bash
9319 classrun -check a1
```

再提交：

```bash
give cs9319 a1 lencode.c ldecode.c
```

