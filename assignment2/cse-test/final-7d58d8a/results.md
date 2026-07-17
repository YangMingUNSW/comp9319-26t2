# CSE db-perftest 最终验证(删除 bwtdecode 后)

- 日期:2026-07-17
- 机器:`db-perftest`(经 `login.cse.unsw.edu.au` 跳板),gcc 12.2.0 (Debian)
- 目录:`~/Desktop/9319/Assignment2`
- 提交文件集:`makefile` `bwtsearch.c` `bwt.c` `bwt.h`(4 个)
- 结论:**全部通过**

| 检查项 | 限制 | 实测 | |
|---|---|---|---|
| 裸 `make` | 任何错误 = 整个作业 0 分 | exit=0,零警告,产出 `bwtsearch` | PASS |
| 官方 `autotest` | 8 个测试 | **8/8 CORRECT** | PASS |
| 官方 `dsearch` 对拍 | 输出一致 | **93 项 0 失败**(全部五个样例文件) | PASS |
| massif `dna-small` | < 16,777,216 B | 5,521,408 B (5.26 MB) | PASS |
| massif `dna-huge` (58MB) | < 16,777,216 B | 7,299,072 B (6.96 MB) | PASS |
| 时间 `dna-huge` 词长 8 | < 5s | user+sys = **0.12s** | PASS |
| 时间 `dna-huge` 词长 20 | < 5s | user+sys = **0.12s** | PASS |
| 不写文件 | 违反 = 整个作业 0 分 | strace 无任何写调用 | PASS |

删除 `bwtdecode.c` + `fm_select`/`fm_fchar`/`nlPos` 后,以上全部与删除前一致
(删除前记录见 `../2afc8e3/results.md`)。本机另跑 1584 次随机对拍,0 失败。
