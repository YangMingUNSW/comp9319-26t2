# COMP9319 A2 在 CSE 上测试指南（中文）

在 UNSW CSE 的 **db-perftest** 上验证 `bwtsearch` / `bwtdecode`，再提交作业。
所有步骤以 `spec.md` 为准。

> **注意：A2 有三条「直接 0 分」的红线**：编译失败（spec 第 172 行）、程序写任何
> 文件（第 160 行）、内存超 16MB（第 183 行）。

> **先看这里 —— 当前最该花时间的一步是 §4 的 `dsearch` 对拍。**
> 2026-07-17 在本机(WSL)造了一个 **112MB 的 `.rbwt`**(1.5 亿字符 DNA，已到 110MB
> 上限)实测：每次搜索 **0.26s / RSS 4.3MB**(限制 5s / 16MB)。所以内存和时间这两条
> 已经基本落地，§5 §6 现在是**确认性**步骤，不再是风险点。
>
> 真正没验证的只剩一条:**`\n` 边界语义**。本机的暴力参考和代码本身编码了*同一个*
> 假设(`\n` = 序列边界，遇到就停)，所以它在逻辑上不可能证伪这个假设 —— 3900 次
> 0 失败在这一点上没有说服力。spec 的 `ACT`→`ACTGA` 例子只证实了**开头**边界，
> **结尾**边界只有 `dsearch` 能判。§4 的 `crosscheck.sh` 就是专门打这个缺口的。

## 1）登录 CSE 并跳到 db-perftest

评测在 **db-perftest** 上进行，它不能从校外直连，要先登录一台普通 CSE 机器再跳
（spec 第 167 行）：

```bash
ssh zID@login.cse.unsw.edu.au
ssh db-perftest                    # 或 ssh zID@db-perftest.cse.unsw.edu.au
cd /path/to/your/assignment2
```

> 把 `zID` 和路径改成你自己的；这个目录里要有 `makefile`、`bwtsearch.c`、`bwtdecode.c`。
> **一定要在 db-perftest 上编译和计时**——它和 vlab 的 CPU / 内存表现不一样，
> 在 vlab 上测出来的 5 秒不算数。

## 2）用【评测同款命令】编译

评测机只跑一条裸 `make`（spec 第 172 行），而且 makefile 必须同时产出
**`bwtsearch` 和 `bwtdecode` 两个**可执行文件（第 169–170 行）。
**编译失败 = 整个作业 0 分**，所以两个目标都要能过。

```bash
make
ls -l bwtsearch bwtdecode          # 两个都必须存在
```

`~cs9319/a2` 里有官方的 makefile 样例，建议直接以它为基准改，别自己发明。

## 3）跑官方 sanity test

```bash
~cs9319/a2/autotest
```

它只在 `dna-tiny.rbwt` 上检查参数处理和输出格式，**不测大文件、不测性能**
（spec 第 135 行）。它通过只代表「格式没写错」，离拿分还远。

## 4）正确性验证：和 `dsearch` 对拍 ← **本次上机的重点**

核心要求：你在 `.rbwt` 上的输出，要等于官方 `dsearch` 在对应 `.txt` 明文上的输出，
**顺序无所谓**（评测两边都会排序，spec 第 137–139 行）。所以对拍时两边都要 `sort`：

```bash
./bwtsearch ~cs9319/a2/dna-small.rbwt ACT | sort > mine.txt
~cs9319/a2/dsearch ~cs9319/a2/dna-small.txt ACT | sort > ref.txt
diff mine.txt ref.txt
```

`diff` 没有任何输出就表示这组通过。但**别只手工试几个词** —— 直接跑仓库里的
[`crosscheck.sh`](./crosscheck.sh)，它会自动把每个 `.rbwt`/`.txt` 配对跑一遍：

```bash
chmod +x crosscheck.sh
./crosscheck.sh                              # ~cs9319/a2 里所有有 .txt 配对的 .rbwt
./crosscheck.sh ~cs9319/a2/dna-small.rbwt    # 或只跑指定文件
```

它从每条序列的**开头 / 中间 / 结尾**各取多种长度（1…128）的词，外加 `AA` `ACAC`
这类自重叠词，两边都 `sort` 后 `cmp`，最后打印 `pass=N fail=M`。要点：

- **`end` 那组就是 `\n` 边界测试** —— 取的词正好结束在序列最后一个字符上。
  **如果只看一处输出，就看它。** `fail=0` 才说明代码对 `\n` 的假设是对的。
- 脚本会按序列长度自动跳过过短的词（spec 保证单个测试 ≤5000 匹配，而 4M 序列上
  1 个字符的词有上百万匹配，既不代表真实测试又很慢）。
- 它只读样例文件、只在 `/tmp` 下写临时文件，不会污染你的目录；也不会被提交
  （提交只收 `makefile *.c *.cpp *.h`）。

万一 `end` 组 `fail`，说明 `dsearch` 对序列结尾的处理和现在的实现不一致，改
`bwtsearch.c` 里 `p1 != 4` / `f1[...] != 5` 那两处边界判断（见文件头注释）。

> **别输出任何多余内容**：调试信息、行号、提示语都会扣分（spec 第 129 行）。
> 排查问题时用 `stderr`（`fprintf(stderr, ...)`），提交前删掉。

`bwtdecode` 的行为 spec 没明确定义（第 214–217 行），先去 `~cs9319/a2` 看官方
makefile / autotest 怎么调它，确认后再补一条 `bwtdecode <rbwt>` 还原明文、
和 `.txt` 做 `diff` 的验证。

## 5）内存检查（16MB 红线）

这是 A2 相对 A1 最不一样、也最容易翻车的地方。用 spec 第 177 行的原命令：

先找出**最大**的那个 `.rbwt`（峰值只有用最大文件测才有意义）：

```bash
ls -lLS ~cs9319/a2/*.rbwt | head -3          # -L 跟随符号链接，别量到链接本身
BIG=$(ls -LS ~cs9319/a2/*.rbwt | head -1)

valgrind --tool=massif --pages-as-heap=yes --massif-out-file=memory.out \
    ./bwtsearch "$BIG" ACGTACGT
ms_print memory.out > memory.txt
grep -m1 "peak" -A12 memory.txt        # 看峰值那一栏
```

要点：

- **预期能过，这一步是确认而非探雷。** 现在的设计（见 `README.md`「Design notes」）
  刻意用 **stdio 读文件而不是 mmap**，赌的是「文件内容待在 OS page cache 里、不计入
  massif 的 `--pages-as-heap`」。2026-07-17 本机在 **112MB `.rbwt`** 上量到
  **maxRSS 4.3MB**，这个赌注基本成立了。
- 但 **maxRSS 只是代理指标**：massif `--pages-as-heap` 会把库和程序段的映射页也算进去，
  RSS 不会。所以预计 massif 报出来是 **6–8MB** 左右，比 4.3MB 高但仍在 16MB 内。
  **这个差值就是本步骤唯一还要确认的东西。**
- `--pages-as-heap=yes` 统计的是**全部**内存，包括 mmap 和程序段，不只是 malloc。
  所以「用 mmap 把整个文件映射进来」**不能**绕过这条限制。
- 峰值必须 **< 16MB = 16,777,216 bytes**（spec 第 174 行）。
  别忘了 checkpoint 表本身就占 ~2MB，留够余量。
- 文件最大 **110MB**（第 186 行），所以你**不可能**把解码后的完整 BWT 放进内存——
  必须保持 RLE 压缩态 + 采样 Occ。**一定要用最大的文件测峰值**，
  用 `dna-tiny` 测出来的 2MB 毫无意义。
- 故意用奇怪的分配方式规避测量**会被人工检查**（第 183 行），别试。
- 注：这里写出的 `memory.out` / `memory.txt` 是 **valgrind 和 ms_print 写的**，
  不是你的程序写的，不违反第 7 节那条红线。

## 6）时间检查（5 秒红线）

评测用 `/usr/bin/time` 看 user + system 时间，**每次搜索限 5 秒**（spec 第 184–185 行）：

```bash
BIG=$(ls -LS ~cs9319/a2/*.rbwt | head -1)
/usr/bin/time ./bwtsearch "$BIG" ACGTACGT > /dev/null      # 看 user + system
```

超时直接判这组 0 分。同样要用**最大**文件测。

- **预期余量很大。** 本机 112MB 文件上是 **0.26–0.28s**（限制 5s，约 19 倍余量），
  而且开销主要在建索引那一遍全文件扫描上、**基本与搜索词长度无关**（128 字符词和
  7 字符词都是 0.26s）。db-perftest 更慢也不至于差 19 倍。
- 跑满所有测试且总时间最快的那份有 **3.5 分 bonus**（第 196 行附近）。既然现在离
  5 秒还很远，bonus 是有希望的 —— 但先确认 §4 的正确性，再谈快。

## 7）「不写文件」自检

**程序写任何文件（哪怕临时文件）= 整个作业 0 分**（spec 第 160 行）。
这条不是靠自觉，跑一遍确认：

```bash
strace -f -e trace=openat,open,creat ./bwtsearch ~cs9319/a2/dna-small.rbwt ACT 2>&1 \
    | grep -v "O_RDONLY" | grep -iv "\.so\|/etc/\|/proc/\|/usr/lib"
```

除了以只读方式打开 `.rbwt` 之外，不应该出现任何带 `O_WRONLY` / `O_CREAT` 的调用。
特别提醒：`avinash2fly-2017/` 那份参考实现会**落盘写索引文件**，这在本学期是禁止的，
只能学它的 rank checkpoint 思路，不要照抄它的索引落盘做法。

## 8）保存测试结果

和 A1 一样，把每次在 CSE 上跑出来的结果按**代码 commit hash** 归档到
`cse-test/<commit-hash>/`，详见 [`cse-test/README.md`](./cse-test/README.md)。
`memory.txt`（massif 峰值报告）建议一起存——它是判断某版代码内存是否达标的直接证据。

## 9）提交前最终检查

`give` 和 `classrun` 要在**普通 CSE 机器**上跑，不是 db-perftest（spec 第 202 行）：

```bash
give cs9319 a2 makefile *.c *.cpp *.h
9319 classrun -check a2
```

提交清单（★ = 本机已验证过、上机只是确认；其余是本机做不到、必须亲自跑的）：

- [ ] db-perftest 上裸 `make` 通过，`bwtsearch` + `bwtdecode` 都产出
      （★ 本机 `gcc 13.3 -O2 -Wall -std=c11` 零警告；纯 C11 无 GNU 扩展，风险低）
- [ ] `~cs9319/a2/autotest` 通过
- [ ] **`./crosscheck.sh` 全绿(`fail=0`)，尤其 `end` 那组** ← **最重要的一条**，
      `\n` 边界语义只有 `dsearch` 能判，本机无法验证
- [ ] 最大文件上 massif 峰值 < 16MB（★ 本机 112MB 文件 RSS 4.3MB；massif 预计 6–8MB）
- [ ] 最大文件上 `/usr/bin/time` < 5s（★ 本机 112MB 文件 0.26s）
- [ ] `strace` 确认没有任何写文件行为
      （★ 本机源码扫描 + 目录 diff 已确认：只有 `fopen(...,"rb")`，`fprintf` 只写 stderr）
- [ ] 输出无任何多余文本（★ 本机确认：缺参数/坏文件 → rc=1 且 stdout 为空，用法只走 stderr）
- [ ] 确认 `bwtdecode` 的 CLI 用法和官方 sample makefile 一致（现实现为 `bwtdecode <rbwt>` → stdout）
- [ ] 截止：**2026-07-28（周二）17:00 AEST**

> 已知的一处**规格内**限制：`bwtsearch` 给每个匹配缓存了前后文（`f1`/`f2` 各
> `ep-sp+1` 字节），所以内存/时间随匹配数线性增长。本机 112MB 文件实测：14.6 万
> 匹配 → 0.5s/4.2MB(没事)，但单字符词(3700 万匹配) → **91s/76MB**，两条限制同时爆。
> **靠 spec 假设 5「单个测试 ≤5000 匹配」才安全** —— 不改也能拿满分，但如果想彻底
> 摆脱对这条保证的依赖，把匹配改成流式输出(不缓存 `f1`/`f2`)即可。
