# COMP9319 A2 在 CSE 上测试指南

在 UNSW CSE 的 **db-perftest** 上验证 `bwtsearch`,再提交作业。
一切以 **`~cs9319/a2` 里官方给的测试代码**和官方题目页为准。

---

## 0. 现状:测试已全部完成并通过(2026-07-17)

在 db-perftest 上实测,**四条红线一条没碰**:

| 检查项 | 限制 | 实测 | |
|---|---|---|---|
| `make` 编译 | 任何错误 = **整个作业 0 分** | exit=0,零警告(gcc 12.2.0) | ✅ |
| **官方 `autotest`** | 8 个测试 | **8/8 CORRECT** | ✅ |
| **官方 `dsearch` 对拍** | 输出必须一致 | **93 项 0 失败**(全部五个样例文件) | ✅ |
| massif 内存 | < 16,777,216 B | **5.26–6.96 MB** | ✅ |
| 时间 user+sys | < 5s | **0.12s**(规格内最坏情况) | ✅ |
| 不写文件 | 违反 = **整个作业 0 分** | strace 无任何写调用 | ✅ |

完整记录见 [`cse-test/`](./cse-test/)。**剩下的只有提交(§7)。**

> 内存峰值**基本不随文件增大**(178KB 的 `dna-small` → 5.26MB;58MB 的 `dna-huge`
> → 6.96MB),因为 checkpoint 表固定 ~2MB。时间余量约 40 倍。

本文档保留下来是为了:**改了代码之后照它复跑一遍**。

## 1. 一键复跑

```bash
ssh z5565446@login.cse.unsw.edu.au
ssh db-perftest                    # 必须在这台上,vlab 的时间不算数
cd ~/Desktop/9319/Assignment2
chmod +x run_cse_tests.sh crosscheck.sh
./run_cse_tests.sh                 # 跑完下面全部检查,结果存 cse-test/<commit>/transcript.txt
```

从 WSL 直接一条命令跳过去也行(公钥已装好):

```bash
ssh -J z5565446@login.cse.unsw.edu.au z5565446@db-perftest.cse.unsw.edu.au
```

## 2. 编译(红线 1)

评测机只跑一条**裸 `make`**,必须产出 **`bwtsearch`**。**编译失败 = 整个作业 0 分。**

```bash
make clean && make
ls -l bwtsearch
```

> **只有 `bwtsearch`。** 官方页面写的是「generate the executable program
> (i.e., **bwtsearch**)」,官方 sample makefile 也只有 `all: bwtsearch`。
> 早先版本的 `spec.md` 曾声称还要产出 `bwtdecode` —— **那是捏造的,该程序不存在**,
> 详见 [`spec.md`](./spec.md) §7。

## 3. 官方 autotest

```bash
~cs9319/a2/autotest
```

它在 `dna-tiny.rbwt` 上跑 8 个测试,和 `~cs9319/a2/output/output{1..8}.txt` 对比。
**不测大文件、不测性能** —— 通过只代表格式没写错,真正的正确性看 §4。

它会先 `rm -f *.out`、再跑一次 `make`,然后把结果写成 `test{1..8}.out`。
这些是 **autotest 自己写的**,不是你的程序写的,不违反 §6 的红线。

## 4. 官方 dsearch 对拍(核心正确性)

你在 `.rbwt` 上的输出,必须等于官方 `dsearch` 在对应 `.txt` 明文上的输出,
**顺序无所谓**(两边都会被排序后再比)。所以对拍时两边都要 `sort`:

```bash
./bwtsearch ~cs9319/a2/dna-small.rbwt ACT | sort > mine.txt
~cs9319/a2/dsearch ~cs9319/a2/dna-small.txt ACT | sort > ref.txt
diff mine.txt ref.txt              # 无输出 = 通过
```

别只手工试几个词 —— 直接跑 [`crosscheck.sh`](./crosscheck.sh):

```bash
./crosscheck.sh                              # 所有有 .txt 配对的 .rbwt
./crosscheck.sh ~cs9319/a2/dna-small.rbwt    # 或只跑指定文件
```

它对每条序列,从**开头 / 中间 / 结尾**各取 1…128 多种长度的词,加上 `AA` `ACAC`
这类自重叠词,两边 `sort` 后 `cmp`,最后打印 `pass=N fail=M`。要点:

- **`end` 那组是 `\n` 边界测试** —— 取的词正好结束在序列最后一个字符上。
  这曾是本机**唯一无法验证**的行为(本机暴力参考和实现编码了同一假设,不可能证伪
  自己),现已被官方 `dsearch` 判定**通过**。改动边界逻辑后**务必**重看这组。
- 脚本按序列长度自动跳过过短的词:spec 保证单个测试 ≤5000 匹配,而 4M 序列上
  1 个字符的词有上百万匹配,既不代表真实测试又很慢。
- 它只读样例文件、临时文件只落 `/tmp`,不写你的目录;也不会被提交。

覆盖的要点:重叠匹配全输出(`AA` 在 `AAA` 里算 2 次);匹配落在序列开头/结尾时
前后文不足 2 字符只输出可用的;**输出无任何多余文本**(调试信息、行号都扣分,
排查用 `stderr`,提交前删掉)。

## 5. 内存 / massif(红线 2)

```bash
BIG=$(ls -LS ~cs9319/a2/*.rbwt | head -1)      # -L 跟随符号链接
valgrind --tool=massif --pages-as-heap=yes --massif-out-file=memory.out \
    ./bwtsearch "$BIG" ACTG
grep '^mem_heap_B=' memory.out | cut -d= -f2 | sort -n | tail -1    # 峰值字节数
ms_print memory.out > memory.txt                                     # 详细报告
```

- 峰值必须 **< 16MB = 16,777,216 bytes**,实测 6.96MB。
- `--pages-as-heap=yes` 统计**全部**内存(含 mmap 和程序段),所以「用 mmap 把整个
  文件映射进来」**不能**绕过。本实现刻意用 **stdio 读文件而非 mmap**,让文件内容
  留在 OS page cache 里 —— 这是设计的主要赌注,在 db-perftest 上验证成立。
- 故意用奇怪的分配方式规避测量**会被人工检查**,别试。
- `memory.out`/`memory.txt` 是 valgrind 和 ms_print 写的,不违反 §6。

## 6. 时间 + 不写文件(红线 3)

```bash
/usr/bin/time ./bwtsearch "$BIG" ACGTACGT > /dev/null    # 看 user + system,限 5s
strace -f -e trace=openat,creat,unlink ./bwtsearch ~cs9319/a2/dna-small.rbwt ACTG 2>&1 \
    >/dev/null | grep -iE "O_WRONLY|O_CREAT|unlink" | grep -ivE "\.so|/etc/|/usr/lib"
```

`strace` 那条应该**没有任何输出** —— 除了只读打开 `.rbwt`,不该有任何写调用。
**程序写任何文件(哪怕临时文件)= 整个作业 0 分。**

> 已知的**规格内**限制:`bwtsearch` 给每个匹配缓存前后文(`f1`/`f2` 各
> `ep-sp+1` 字节),内存/时间随匹配数线性增长。实测 58MB 文件:1023 匹配 → 0.12s;
> 25.6 万匹配 → 1.58s(**已超出 spec 保证的 5000 上限**)。靠 spec 假设 (5)
> 「no test case will produce more than 5000 matches」才安全 —— 不改也能拿满分。
> 所以 1 字符词跑得慢**不算问题**,判断看 8/20 字符那几行。

## 7. 提交

`give` 和 `classrun` 要在**普通 CSE 机器**(vx*/vlab)上跑,**db-perftest 上没有**:

```bash
cd ~/Desktop/9319/Assignment2
give cs9319 a2 makefile *.c *.cpp *.h
9319 classrun -check a2
```

提交的就是 4 个文件:`makefile` `bwtsearch.c` `bwt.c` `bwt.h`。
`crosscheck.sh` / `run_cse_tests.sh` / `*.md` 不在通配符范围内,放心留着。

## 8. 归档

每次在 CSE 上的结果按**代码 commit hash** 存到 `cse-test/<commit-hash>/`,
详见 [`cse-test/README.md`](./cse-test/README.md);`run_cse_tests.sh` 会自动放对位置。

---

## 9. 最终清单

- [x] db-perftest 上裸 `make` 通过,产出 `bwtsearch`
- [x] `~cs9319/a2/autotest` 8/8 CORRECT
- [x] `./crosscheck.sh` 全绿(93 项 0 失败),含 `end`(`\n` 边界)组
- [x] 最大文件 massif 峰值 6.96MB < 16MB
- [x] 最大文件 `/usr/bin/time` user+sys 0.12s < 5s
- [x] `strace` 确认无任何写文件行为
- [x] 输出无多余文本(缺参数/坏文件 → rc=1 且 stdout 为空,用法只走 stderr)
- [ ] **`give` 提交 + `classrun -check` 核对**(在普通 CSE 机器上)
- [ ] 截止:**2026-07-28(周二)17:00 AEST**(迟交 5%/天,最多 5 天)
