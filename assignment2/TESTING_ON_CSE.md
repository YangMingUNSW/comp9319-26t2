# COMP9319 A2 在 CSE 上测试指南（完整版）

在 UNSW CSE 的 **db-perftest** 上验证 `bwtsearch` / `bwtdecode`，再提交作业。
一切以 `spec.md`（官方题目整理）和 **`~cs9319/a2` 里官方给的测试代码**为准；
本文档里凡是和官方产物冲突的，**以官方为准**。

---

## 0. 先读这一段：现在到底该测什么

### 三条「直接 0 分」的红线（spec）

| 红线 | 后果 | spec 出处 | 本文档步骤 |
|---|---|---|---|
| `make` 编译失败 | **整个作业 0 分** | 第 172 行 | §4 |
| 程序写任何文件（含临时文件） | **整个作业 0 分** | 第 160 行 | §9 |
| 运行内存 ≥ 16MB | 那些测试 0 分 | 第 174/183 行 | §7 |

外加一条软红线：单次搜索 **user+system > 5 秒** → 那组测试 0 分（第 184–185 行）。

### 本机（WSL）已经验证到什么程度

2026-07-17 造了一个 **112MB 的 `.rbwt`**（1.5 亿字符 DNA，已到 spec 的 110MB 上限）实测：

| 项目 | 限制 | 本机实测 | 结论 |
|---|---|---|---|
| 单次搜索 user+sys | < 5s | **0.26–0.28s** | 约 19 倍余量 |
| 内存 maxRSS | < 16MB | **4.3MB** | 通过 |
| 正确性 | 与参考一致 | ~3900 次随机搜索 + 4M/重复 DNA 对拍，**0 失败** | 通过 |
| 不写文件 | 零分红线 | 源码只有 `fopen(...,"rb")`，`fprintf` 只写 stderr | 通过 |
| 编译 | 无错误 | `gcc 13.3 -O2 -Wall -std=c11` 零警告 | 通过 |

**所以这次上机的重点不是「探雷」，而是三件本机做不到的事：**

1. **§6 的 `dsearch` 对拍** ← **最重要**。`\n` 边界语义是唯一真正没验证的东西：
   本机的暴力参考和 `bwtsearch.c` 编码了**同一个**假设（`\n` = 序列边界，遇到就停、
   不输出、不环绕），所以它在逻辑上**不可能证伪**这个假设 —— 那 3900 次 0 失败在
   这一件事上**没有说服力**。spec 的 `ACT`→`ACTGA` 例子只证实了**开头**边界，
   **结尾**边界只有官方 `dsearch` 能判。
2. **§3 摸清官方测试资料**：官方 sample makefile 的目标、`autotest` 的调用方式、
   尤其 **`bwtdecode` 的真实用法**（spec 正文没定义，现在是猜的）。
3. **§7 的 massif**：本机没有 valgrind 也没有 sudo，4.3MB 的 maxRSS 只是**代理指标**。
   massif `--pages-as-heap` 会把库和程序段的映射页也算进去、RSS 不算，
   所以预计 massif 报 **6–8MB**（仍在 16MB 内）。**这个差值就是唯一要确认的东西。**

### 一键跑法（推荐）

不想一步步敲就直接跑这个，它按本文档顺序跑完全部检查，产出**一份可粘贴的 transcript**：

```bash
chmod +x run_cse_tests.sh crosscheck.sh
./run_cse_tests.sh
```

结果同时打印并存到 `cse-test/<commit-hash>/transcript.txt`。
**把整份 transcript 贴回来即可**（判读标准见 §12）。

> 一键脚本里每一步都是独立、失败不中断的，所以一份 transcript 就能看到全貌。
> 但**如果 §3 发现官方用法和我们的假设不一致，以官方为准**，脚本要相应改。

---

## 1. 登录 CSE 并跳到 db-perftest

评测在 **db-perftest** 上进行，它不能从校外直连，要先登录一台普通 CSE 机器再跳
（spec 第 167 行）：

```bash
ssh zID@login.cse.unsw.edu.au
ssh db-perftest                    # 或 ssh zID@db-perftest.cse.unsw.edu.au
```

> **一定要在 db-perftest 上编译和计时** —— 它和 vlab 的 CPU / 内存表现不一样，
> 在 vlab 上测出来的 5 秒不算数。一键脚本 §0 会检查 hostname 并在不对时警告。

## 2. 把代码弄到 db-perftest 上

```bash
git clone git@github.com:YangMingUNSW/comp9319.git      # 或 git pull
cd comp9319/assignment2
ls                                 # 至少要有 makefile bwtsearch.c bwtdecode.c bwt.c bwt.h
```

## 3. 【关键】先摸清官方给了什么测试代码

**这一步先做**，因为它决定后面几步怎么跑，也直接回答 `spec.md` 第 7 节的待确认项：

```bash
ls -lL ~cs9319/a2                        # 官方目录全貌
cat ~cs9319/a2/makefile                  # 官方 sample makefile（spec 第 169 行说以它为基准）
head -60 ~cs9319/a2/autotest             # 看它到底怎么调 bwtsearch / bwtdecode
ls -lLS ~cs9319/a2/*.rbwt                # 按大小排；最大的那个才是内存/时间的考题
```

重点看三件事，**任何一条和我们的实现不一致，都以官方为准、回来改代码**：

- **`bwtdecode` 的用法。** spec 第 5 节要求 makefile 必须产出它，但正文只定义了
  `bwtsearch` 的行为（第 214–217 行）。我们现在实现成 `bwtdecode <rbwt>` → 打印原始
  DNA 到 stdout，**这是推测**。sample makefile / autotest 里怎么调它，才是准的。
- **官方 makefile 的目标名和编译选项**，确认我们的 makefile 能被裸 `make` 正确驱动。
- **`autotest` 的判定方式**（它比对什么、对参数错误期待什么返回值）。

> spec 第 177–180 行的 massif 示例里有个 `< mytest.in`，但正文说搜索词是命令行参数
> （第 91 行）。大概率是模板残留、stdin 不用；`autotest` 的源码能确认这点。

## 4. 编译（红线 1）

评测机只跑一条**裸 `make`**（spec 第 172 行），而且必须同时产出
**`bwtsearch` 和 `bwtdecode` 两个**可执行文件（第 169–170 行）。

```bash
make clean
make
ls -l bwtsearch bwtdecode          # 两个都必须存在
```

**编译失败 = 整个作业 0 分。** 本机 `gcc 13.3` 下零警告，代码是纯 C11、没用 GNU 扩展，
所以 db-perftest 上出问题的概率很低；真出了错，多半是 gcc 版本对 `-std=c11` 的差异。

## 5. 官方 autotest（格式 / 参数 sanity）

```bash
~cs9319/a2/autotest
```

它只在 `dna-tiny.rbwt` 上检查参数处理和输出格式，**不测大文件、不测性能**
（spec 第 135 行）。**通过它只代表「格式没写错」，离拿分还远** —— 真正的正确性看 §6。

## 6. 【最重要】官方 dsearch 对拍

核心要求：你在 `.rbwt` 上的输出，要等于官方 `dsearch` 在对应 `.txt` 明文上的输出
（spec 第 137–140 行），**顺序无所谓**（两边都会被排序后再比，第 127–128 行）。
所以对拍时两边都要 `sort`：

```bash
./bwtsearch ~cs9319/a2/dna-small.rbwt ACT | sort > mine.txt
~cs9319/a2/dsearch ~cs9319/a2/dna-small.txt ACT | sort > ref.txt
diff mine.txt ref.txt              # 无输出 = 这组通过
```

但**别只手工试几个词**，直接跑 [`crosscheck.sh`](./crosscheck.sh)：

```bash
./crosscheck.sh                              # ~cs9319/a2 里所有有 .txt 配对的 .rbwt
./crosscheck.sh ~cs9319/a2/dna-small.rbwt    # 或只跑指定文件
```

它对每条序列，从**开头 / 中间 / 结尾**各取 1…128 多种长度的词，加上 `AA` `ACAC`
这类自重叠词，两边 `sort` 后 `cmp`，最后打印 `pass=N fail=M`。要点：

- **`end` 那组就是 `\n` 边界测试** —— 取的词正好结束在序列最后一个字符上。
  **如果只看一处输出，就看它。** `fail=0` 才说明代码对 `\n` 的假设是对的。
- 脚本按序列长度自动跳过过短的词：spec 保证单个测试 ≤5000 匹配（第 158 行），而 4M
  序列上 1 个字符的词有上百万匹配，既不代表真实测试又很慢。
- 它只读样例文件、临时文件只落 `/tmp`，不写你的目录；也不会被提交。

其它必须覆盖的点（`crosscheck.sh` 都已包含）：

- **重叠匹配**必须全部输出（spec 第 106 行：`AA` 在 `AAA` 里算 2 次）。
- 匹配落在序列**开头 / 结尾**时，前后文不足 2 个字符要**只输出可用的**（第 103–104 行）。
- **别输出任何多余内容**：调试信息、行号、提示语都扣分（第 129 行）。
  排查问题用 `stderr`，提交前删掉。

## 7. 内存 / massif（红线 3）

先找出**最大**的那个 `.rbwt` —— 峰值只有用最大文件测才有意义：

```bash
BIG=$(ls -LS ~cs9319/a2/*.rbwt | head -1)      # -L 跟随符号链接，别量到链接本身
valgrind --tool=massif --pages-as-heap=yes --massif-out-file=memory.out \
    ./bwtsearch "$BIG" ACGTACGT < /dev/null
ms_print memory.out > memory.txt
grep '^mem_heap_B=' memory.out | cut -d= -f2 | sort -n | tail -1    # 峰值字节数
```

要点：

- 峰值必须 **< 16MB = 16,777,216 bytes**（spec 第 174 行）。
- `--pages-as-heap=yes` 统计**全部**内存，包括 mmap 和程序段，不只是 malloc
  （第 181 行）。所以「用 mmap 把整个文件映射进来」**不能**绕过这条限制。
- 现在的设计（见 `README.md`「Design notes」）刻意用 **stdio 读文件而不是 mmap**，
  赌的是「文件内容待在 OS page cache 里、不计入 `--pages-as-heap`」。
  本机 112MB 上 RSS 4.3MB，这个赌注基本成立；**massif 预计报 6–8MB。**
- checkpoint 表本身固定占 ~2MB（`S=fileSize/50000`，约 5 万个 × 40 字节），
  与文件大小无关，所以余量足够。
- spec 第 174 行说限制是「**每个**程序」，所以 `bwtdecode` 也要量（一键脚本已包含）。
- 故意用奇怪的分配方式规避测量**会被人工检查**（第 183 行），别试。
- 注：`memory.out` / `memory.txt` 是 **valgrind 和 ms_print 写的**，不是你的程序写的，
  **不违反** §9 那条红线。

## 8. 时间（5 秒线）

评测用 `/usr/bin/time` 看 **user + system** 时间，每次搜索限 5 秒（spec 第 184–185 行）：

```bash
BIG=$(ls -LS ~cs9319/a2/*.rbwt | head -1)
/usr/bin/time ./bwtsearch "$BIG" ACGTACGT > /dev/null
```

- 同样要用**最大**文件测。
- 本机 112MB 上是 **0.26–0.28s**，而且开销主要在建索引那一遍全文件扫描上、
  **基本与搜索词长度无关**（128 字符词和 7 字符词都是 0.26s）。db-perftest 更慢也
  不至于差 19 倍。
- 跑满所有测试且总时间最快的那份有 **3.5 分 bonus**（第 195–197 行）。
  既然离 5 秒还很远，bonus 有希望 —— 但先确认 §6 的正确性，再谈快。

## 9. 「不写文件」自检（红线 2）

**程序写任何文件（哪怕临时文件）= 整个作业 0 分**（spec 第 160 行）。跑一遍确认：

```bash
strace -f -e trace=openat,open,creat ./bwtsearch ~cs9319/a2/dna-small.rbwt ACGTACGT 2>&1 \
    | grep -v "O_RDONLY" | grep -iv "\.so\|/etc/\|/proc/\|/usr/lib"
```

除了以只读方式打开 `.rbwt`，不应出现任何带 `O_WRONLY` / `O_CREAT` 的调用。

> 特别提醒：`reference/avinash2fly-2017/` 那份参考实现会**落盘写索引文件**，
> 本学期禁止 —— 只能学它的 rank checkpoint 思路，别照抄索引落盘。

## 10. 归档结果

把每次在 CSE 上跑出来的结果按**代码 commit hash** 归档到 `cse-test/<commit-hash>/`，
详见 [`cse-test/README.md`](./cse-test/README.md)。一键脚本会自动放对位置。

## 11. 提交

`give` 和 `classrun` 要在**普通 CSE 机器**上跑，不是 db-perftest（spec 第 202 行）：

```bash
give cs9319 a2 makefile *.c *.cpp *.h
9319 classrun -check a2
```

> 注意提交只收 `makefile *.c *.cpp *.h` —— `crosscheck.sh` / `run_cse_tests.sh` /
> `*.md` 都不会被提交，放心留在仓库里。

---

## 12. 判读：怎么算「满分」，怎么算「要 debug」

把 transcript 贴回来时，我会按这张表看。你自己也可以先对一遍：

| transcript 里的位置 | 满分长这样 | 不对劲长这样 → 要 debug |
|---|---|---|
| §2 COMPILE | `make` `[exit=0]`，`bwtsearch` + `bwtdecode` 都在 | 任何编译错误 = **整个作业 0 分**，最高优先级 |
| §3 autotest | 官方 autotest 报通过 | 报 fail → 多半是输出格式/参数处理 |
| §4 CROSS-CHECK | **`fail=0`** | 任何 `fail>0`，尤其 `[end]` 组 → 见下方 |
| §5 bwtdecode | 每行 `PASS decode` | `FAIL decode`，或 §1 显示官方用法不是 `bwtdecode <rbwt>` |
| §6 massif | `PEAK = ... | PASS`，预计 6–8MB | 接近或超过 16,777,216 → 要缩 checkpoint 表 |
| §7 TIME | 8/16 字符词 user+sys 远小于 5s | 超过 1s 就值得看；超 5s = 那组 0 分 |
| §8 NO FILE WRITES | 写调用列表为空 / `PASS` | 出现任何 `O_CREAT`/`O_WRONLY` = **整个作业 0 分** |

### 最可能出问题的一处：`[end]` 组 fail

如果 `crosscheck.sh` 只有 `end` 组 fail、start/middle 全过，那就**证实了**我们对 `\n`
边界的假设和官方 `dsearch` 不一致 —— 这正是本机测不出来的那件事。改这两处
（都在 `bwtsearch.c`，文件头注释有说明）：

- **前置上下文**：`if (p1 != 4)`（第 105 行）/ `if (p2 != 4)`（第 108 行）——
  `4` 是 `\n` 的字符码，现在遇到就停、不输出。
- **后置上下文**：`if (a != 5)`（第 117 行）/ `if (b != 5)`（第 120 行）——
  `5` 是「序列到头了」的哨兵。

贴 transcript 时**把 `dsearch` 和我们输出的实际差异一起贴上**（`crosscheck.sh` 已经
自动打印前 6 行 diff），我据此判断是「该输出 `\n`」还是「该环绕到序列另一头」。

### 一处**规格内**的已知限制（不影响满分）

`bwtsearch` 给每个匹配缓存了前后文（`f1`/`f2` 各 `ep-sp+1` 字节），所以内存/时间随
匹配数线性增长。本机 112MB 实测：14.6 万匹配 → 0.5s/4.2MB（没事），但单字符词
（3700 万匹配）→ **91s/76MB**，两条限制同时爆。

**靠 spec 假设 5「单个测试 ≤5000 匹配」（第 158 行）才安全** —— 不改也能拿满分。
所以 §7 里 1 字符词跑得慢**不算问题**，判断看 8/16 字符那两行。
如果想彻底摆脱对这条保证的依赖，把匹配改成流式输出（不缓存 `f1`/`f2`）即可。

---

## 13. 提交前最终清单

★ = 本机已验证、上机只是确认；其余是本机做不到、必须亲自跑的。

- [ ] db-perftest 上裸 `make` 通过，`bwtsearch` + `bwtdecode` 都产出
      （★ 本机 `gcc 13.3 -O2 -Wall -std=c11` 零警告，纯 C11 无 GNU 扩展）
- [ ] **官方用法确认**：`bwtdecode` 的 CLI 与 sample makefile / autotest 一致
- [ ] `~cs9319/a2/autotest` 通过
- [ ] **`./crosscheck.sh` 全绿（`fail=0`），尤其 `end` 组** ← **最重要**，
      `\n` 边界语义只有官方 `dsearch` 能判
- [ ] 最大文件上 massif 峰值 < 16MB（★ 本机 112MB RSS 4.3MB；massif 预计 6–8MB）
- [ ] 最大文件上 `/usr/bin/time` user+sys < 5s（★ 本机 112MB 0.26s）
- [ ] `strace` 确认没有任何写文件行为
      （★ 本机源码扫描 + 目录 diff 已确认）
- [ ] 输出无任何多余文本（★ 本机确认：缺参数/坏文件 → rc=1 且 stdout 为空）
- [ ] 结果已归档到 `cse-test/<commit-hash>/`
- [ ] 截止：**2026-07-28（周二）17:00 AEST**（迟交 5%/天，最多 5 天）
