# CSE db-perftest 测试结果

- 日期:2026-07-17
- 机器:`db-perftest`(经 `login.cse.unsw.edu.au` 跳板),gcc 12.2.0 (Debian)
- 目录:`~/Desktop/9319/Assignment2`
- 结论:**全部通过,四条红线无一触碰**

## 编译

`make` exit=0,零警告,产出 `bwtsearch` + `bwtdecode`。

## 官方 autotest:8/8 CORRECT

| # | 搜索词 | 结果 |
|---|---|---|
| 1 | `ACT` | CORRECT |
| 2 | `AC` | CORRECT |
| 3 | `TA` | CORRECT |
| 4 | `TCCT`(无匹配,期望空输出) | CORRECT |
| 5 | `GAC` | CORRECT |
| 6 | `A` | CORRECT |
| 7 | `T` | CORRECT |
| 8 | 38 字符全序列 | CORRECT |

## 官方 dsearch 对拍:93 项 0 失败

`./crosscheck.sh` 覆盖全部五个样例文件,每条序列取开头/中间/结尾多种词长 + 自重叠词。

| 文件 | body 字符数 | 结果 |
|---|---|---|
| dna-tiny + dna-small + dna-medium | 39 / 247,909 / 7,013,108 | pass=66 fail=0 |
| dna-large + dna-huge | 25,000,000 / 104,857,599 | pass=27 fail=0 |

**`end` 组全过 —— `\n` 序列边界语义确认与官方 `dsearch` 一致。** 这是本机唯一
无法验证的行为(本机暴力参考与实现编码了同一假设,不可能证伪)。

## massif 内存(`--pages-as-heap=yes`,限 16,777,216 B)

| 文件 | .rbwt 大小 | 峰值 | |
|---|---|---|---|
| dna-small | 178,467 B | 5,521,408 B (5.26 MB) | PASS |
| dna-medium | 4,698,818 B | 7,319,552 B (6.98 MB) | PASS |
| dna-large | 15,399,615 B | 7,303,168 B (6.96 MB) | PASS |
| dna-huge | 60,898,347 B | 7,299,072 B (6.96 MB) | PASS |
| bwtdecode (dna-medium) | — | 7,319,552 B (6.98 MB) | PASS |

峰值与文件大小基本无关(checkpoint 表固定 ~2MB),余量 >2 倍。

## 计时 `/usr/bin/time`(user+sys,限 5s)

| 文件 | 词长 | 匹配数 | user+sys |
|---|---|---|---|
| dna-medium | 4 / 8 / 20 | 16,101 / 11 / 0 | 0.05s / 0.00s / 0.00s |
| dna-large | 4 / 8 / 20 | 60,971 / 229 / 0 | 0.26s / 0.02s / 0.02s |
| dna-huge | 4 | 256,579 ← **超出 spec 的 5000 保证** | 1.58s |
| dna-huge | 8 | 1,023 | **0.12s** |
| dna-huge | 20 | 3 | **0.11s** |

规格内最坏情况约 **0.12s**,余量约 40 倍。

## 其余

- **不写文件**:strace 无任何 `O_WRONLY`/`O_CREAT`/`unlink` → PASS
- **bwtdecode 往返**:tiny/small/medium 解码结果与 `.txt` 逐字节一致
