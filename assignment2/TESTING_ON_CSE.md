# COMP9319 A2 在 CSE 上测试指南（中文）

在 UNSW CSE 的 **db-perftest** 上验证 `bwtsearch` / `bwtdecode`，再提交作业。
所有步骤以 `spec.md` 为准。

> **注意：A2 有三条「直接 0 分」的红线**，测试时优先确认它们，再谈正确性：
> 编译失败（spec 第 172 行）、程序写任何文件（第 160 行）、内存超 16MB（第 183 行）。

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

## 4）正确性验证：和 `dsearch` 对拍

核心要求：你在 `.rbwt` 上的输出，要等于官方 `dsearch` 在对应 `.txt` 明文上的输出，
**顺序无所谓**（评测两边都会排序，spec 第 137–139 行）。所以对拍时两边都要 `sort`：

```bash
./bwtsearch ~cs9319/a2/dna-small.rbwt ACT | sort > mine.txt
~cs9319/a2/dsearch ~cs9319/a2/dna-small.txt ACT | sort > ref.txt
diff mine.txt ref.txt
```

`diff` 没有任何输出就表示这组通过。要多试几个搜索词，尤其是：

- **重叠匹配**必须全部输出（`spec` 第 3 节；用 `AA` 这类自重叠的词测）。
- 匹配落在**文件开头 / 结尾**时，前后文不足 2 个字符要能正常截断。
- 匹配跨越 `\n`（序列边界）时的前后文行为——这条 spec 没写死，
  以 `dsearch` 的实际输出为准（见 `spec.md` 第 7 节的待确认项）。

> **别输出任何多余内容**：调试信息、行号、提示语都会扣分（spec 第 129 行）。
> 排查问题时用 `stderr`（`fprintf(stderr, ...)`），提交前删掉。

`bwtdecode` 的行为 spec 没明确定义（第 214–217 行），先去 `~cs9319/a2` 看官方
makefile / autotest 怎么调它，确认后再补一条 `bwtdecode <rbwt>` 还原明文、
和 `.txt` 做 `diff` 的验证。

## 5）内存检查（16MB 红线）

这是 A2 相对 A1 最不一样、也最容易翻车的地方。用 spec 第 177 行的原命令：

```bash
valgrind --tool=massif --pages-as-heap=yes --massif-out-file=memory.out \
    ./bwtsearch ~cs9319/a2/dna-small.rbwt ACT
ms_print memory.out > memory.txt
grep -m1 "peak" -A12 memory.txt        # 看峰值那一栏
```

要点：

- ⚠️ **这一步是当前实现最大的风险点，务必优先做。** 现在的设计（见 `README.md`
  「Design notes」）刻意用 **stdio 读文件而不是 mmap**，赌的是「文件内容待在 OS
  page cache 里、不计入 massif 的 `--pages-as-heap`」。本地在 20 万字符的测试上量到
  RSS ≈ 1.6MB，但**这个假设必须在 db-perftest 上用接近 110MB 的 `.rbwt` 亲自验证**——
  假设一旦不成立，整个索引设计就得推翻重来，越早知道越好。
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
/usr/bin/time ./bwtsearch ~cs9319/a2/dna-small.rbwt ACT > /dev/null
```

超时直接判这组 0 分。同样要用大文件测。跑满所有测试且总时间最快的那份有
**3.5 分 bonus**（第 196 行附近），但先保正确和内存，再谈快。

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

提交清单：

- [ ] db-perftest 上裸 `make` 通过，`bwtsearch` + `bwtdecode` 都产出
- [ ] `~cs9319/a2/autotest` 通过
- [ ] 大文件上和 `dsearch` 对拍（两边 `sort`）零 diff，含重叠匹配 / 边界
- [ ] 最大文件上 massif 峰值 < 16MB
- [ ] 最大文件上 `/usr/bin/time` < 5s
- [ ] `strace` 确认没有任何写文件行为
- [ ] 输出无任何多余文本
- [ ] 截止：**2026-07-28（周二）17:00 AEST**
