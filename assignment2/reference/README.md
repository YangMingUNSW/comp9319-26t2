# reference/ — 仅供学习的外部参考(BWT backward search)

> ## ⚠️ 这些是**往年**的题面,不是本期的
>
> `anantkm-BWT/Specification.md` 是**旧学期**的 A2:它要求**两个程序**
> (`bwtsearch` **和 `bwtdecode`**)、搜的是**明文 `.bwt`**、`bwtdecode` 还要**写输出文件**。
> **本期(2026T2)只有 `bwtsearch` 一个程序、搜 RLE 编码的 `.rbwt`、且禁止写任何文件。**
>
> 这个区别曾经真的把人坑了:早先的 `spec.md` 把旧题面混进了本期题面,凭空捏造出
> 「makefile 必须产出 `bwtdecode`」的要求,导致整个 `bwtdecode.c` 白写(2026-07-17 已更正,
> 见 [`../spec.md`](../spec.md) §7)。**只学这里的算法思路,任何「要求」一律以官方页面为准。**

> ⚠️ **学术诚信**:本目录代码全部来自 GitHub 公开仓库,已在 UNSW MOSS/Turnitin 查重库中。
> **仅用于理解 FM-index / backward search 算法、复用测试数据、学习测试方法**。直接复制 = 学术不端。
> 📌 这里**不是**我的 assignment 解答 —— 本期作业尚未开始编写。

---

## 已克隆的仓库(已删 `.git` 与大文件,仅留源码 + 小测试数据)

| 子目录 | 来源 / 期次 | 内容 | 与本期关系 |
|---|---|---|---|
| `anantkm-BWT/` | [anantkm](https://github.com/anantkm/BWT) · C(年份不详,旧版明文 BWT 题面) | `bwtsearch.c` + **`bwtdecode.c`** + makefile + Specification.md + `dna-tiny/5KB/100KB/small`(.txt/.bwt) | ⭐ **主参考**:唯一同时含 **两个程序**(bwtsearch+bwtdecode),结构最贴近本期 |
| `z5248093-2023T2/` | [z5248093](https://github.com/z5248093/bwtsearch) · 2023(C) | `bwtsearch.c`(530 行,自定义 `Occ` 采样结构) | 近期真实 COMP9319 提交;**只读输入、无索引文件**,采样思路可学 |
| `avinash2fly-2017/` | [avinash2fly](https://github.com/avinash2fly/BWTSearch) · 2017(C++) | `bwtsearch.cpp` + `BwtSearch2.cpp` + makefile | 大文件 **rank checkpoint / 采样**思路;⚠️ **它把索引写进磁盘文件**——本期禁止,只借算法 |

> 已删除的大文件:`dna-15MB/medium/1MB.*`、`simple7.bwt`、编译产物 `bwtsearch`/`gmon.out`。
> 需要大压力测试数据时,直接用 CSE 上的 `~cs9319/a2/*.rbwt`(权威),或自己从 `.txt` 生成。

---

## ⚠️ 三条最关键的「与本期不一致」提醒

1. **都不解 RLE**:三个参考读的都是**明文 BWT**(`.bwt`),没有一个实现本期的
   **行程编码 `.rbwt`**(3 位字符 + 5 位行程、长度 = 值+1、超 32 拆字节)。
   → RLE 解码这一层**必须自己加**(和 A1 里「写满重置」是各参考都缺的新增点一样)。
2. **禁止写文件**:本期「不能写任何文件(含临时)」。`avinash2fly` 用 `createIndexFile`
   把 FM-index 落盘 → **本期违规,零分**。`anantkm` / `z5248093` 都是**只读输入 + 内存采样**,
   这才是本期该走的路子。
3. **16MB 内存 / 110MB 文件**:`anantkm` 用「按文件大小动态调采样间隔」(见 `bwtsearch.c`
   注释:filesize > 620000 才建间隔),思路对;但仍要核算大文件下是否 < 16MB。
   本期约束比这些参考更紧,采样密度要更省。

---

## 📖 该看什么(逐仓库导读)

### `anantkm-BWT/`(⭐ 主参考)
- `bwtsearch.c`:
  - `occuranceOfChar()` —— `Occ`/rank 的**采样 + 局部扫**实现(内存友好的关键)。
  - `c_table[]` —— `C[]` 表构建。
  - 主循环 `first = c_table[c] + occ(...); last = c_table[c] + occ(...) - 1;` —— **backward search 骨架**,和 `notes.md` §2 对得上。
- `bwtdecode.c`:逆 BWT 还原原文(LF-mapping 连续跳)。⚠️ **本期没有 `bwtdecode` 这个程序** —— 它属于
  往年题面。见下方警告。
- `Specification.md`:是**旧版**题面(明文 BWT,无 RLE),对比本期看差异即可。

### `z5248093-2023T2/`
- `struct Occ` / `add_occ` / `initializeOcc` —— 一种**采样表**数据结构写法;只读输入、内存内建索引,合规路线的范例。

### `avinash2fly-2017/`
- `getRank` / `getOccurences` / `checkPoint` —— **rank 检查点**思想(每隔 N 存一次累计计数,查询时就近 checkpoint 再扫)。**只学思想**,别学它落盘的做法。

---

## ✅ 怎么用它们测试(本期正确做法)

- **权威对照工具在 CSE**:`~cs9319/a2/dsearch <dna.txt> <term>` 给出明文上的正确答案。
  你的 `bwtsearch <对应.rbwt> <term>` 输出(排序后)必须与之一致。
- 本地这些参考的 `.bwt` / 期望输出**不能**直接当本期判据(格式是明文 BWT,非 RLE)。
- 小数据先跑通:`anantkm-BWT/dna-tiny.txt` 与 CSE 的 `dna-tiny.rbwt` 是**同一条序列**,
  可以用来手推、对拍(注意本地是 `.bwt`、CSE 是 `.rbwt`)。

详细算法导读与学习路径见上一级 [`notes.md`](../notes.md);题面见 [`spec.md`](../spec.md)。
