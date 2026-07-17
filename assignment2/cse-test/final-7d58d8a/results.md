# CSE db-perftest 最终验证(全量)

- 日期:2026-07-17 · 机器 `db-perftest`(经 login.cse 跳板)· gcc 12.2.0 (Debian)
- 代码:删除 `bwtdecode.c` + `fm_select`/`fm_fchar`/`nlPos` 之后的最终版
- 提交文件集:`makefile` `bwtsearch.c` `bwt.c` `bwt.h`(4 个)
- **结论:官方要求的四样测试工具全部跑过,全部通过**

官方测试工具(已核实这四样都写在官方页面上):
`autotest`(1 次提及)、`dsearch`(6 次)、`valgrind massif`(3 次)+ `ms_print`(2 次)、
`/usr/bin/time`(1 次)。

## 1. 编译

裸 `make` exit=0,零警告。**并在只含 4 个提交文件的干净目录里复现**(模拟评测机环境)。

## 2. 官方 autotest:8/8 CORRECT

`ACT` / `AC` / `TA` / `TCCT`(无匹配) / `GAC` / `A` / `T` / 38 字符全序列 —— 全部 CORRECT。

## 3. 官方 dsearch 对拍:93 项 0 失败

`./crosscheck.sh` 覆盖**全部五个**样例文件,每条序列取开头/中间/**结尾**多种词长 + 自重叠词。
**`end` 组全过 → `\n` 序列边界语义与官方 `dsearch` 一致**(本机唯一无法验证的行为)。

## 4. massif 内存(全量,spec 原文命令,限 16,777,216 B)

`valgrind --tool=massif --pages-as-heap=yes --massif-out-file=memory.out ./bwtsearch <f>.rbwt ACTG`

| 文件 | .rbwt | 峰值 | | |
|---|---|---|---|---|
| dna-tiny | 20 B | 5,296,128 B | 5.05 MB | PASS |
| dna-small | 178,467 B | 5,521,408 B | 5.26 MB | PASS |
| dna-medium | 4,698,818 B | 7,319,552 B | 6.98 MB | PASS |
| dna-large | 15,399,615 B | 7,303,168 B | 6.96 MB | PASS |
| dna-huge | 60,898,347 B | 7,299,072 B | 6.96 MB | PASS |

`ms_print` 报告(评测方实际看的视图)顶部标 **6.961 MB**,与上表一致。
峰值**基本不随文件增大**(checkpoint 表固定 ~2MB)。

## 5. 计时(全量,`/usr/bin/time` user+sys,限 5s)

| 文件 | 词长 4 | 词长 8 | 词长 20 |
|---|---|---|---|
| dna-tiny | 0.00s (5 匹配) | 0.00s | 0.00s |
| dna-small | 0.00s (731) | 0.00s (1) | 0.00s |
| dna-medium | 0.06s (16,101) | 0.00s (11) | 0.01s |
| dna-large | 0.27s (60,971) | 0.03s (229) | 0.03s |
| dna-huge | 1.56s (256,579 ← **超 spec 的 5000 保证**) | **0.13s** (1,023) | **0.12s** (3) |

规格内(≤5000 匹配)最坏约 **0.13s**,余量约 38 倍。

## 6. 不写文件

`strace -f -e trace=openat,creat,unlink` 无任何 `O_WRONLY`/`O_CREAT`/`unlink` → PASS。

## 未验证 / 无法验证

1. **评测会用另一套 RBWT 文件**(spec 明示,防硬编码)。本实现是通用 FM-index,
   无任何针对样例的硬编码,但那批文件拿不到,测不了。
2. **人工可读性检查** —— 主观判断。
3. **`give` 提交本身** —— 未执行,留给用户(不可逆的对外动作)。
4. **`≤5000 匹配` 假设是承重的**:`f1`/`f2` 按匹配数分配,单字符词(3700 万匹配)
   会同时爆内存和时间。靠 spec 假设 (5) 才安全。
