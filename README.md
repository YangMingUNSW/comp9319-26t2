<div align="center">

# COMP9319 · Web Data Compression and Search
### Web 数据压缩与搜索 · UNSW 课程作业仓库

![UNSW](https://img.shields.io/badge/UNSW-COMP9319-FFD200?style=flat-square&labelColor=000000)
![Topic](https://img.shields.io/badge/Topic-Compression%20%26%20Search-4c8bf5?style=flat-square)
![C](https://img.shields.io/badge/C-LZW-00599C?style=flat-square&logo=c&logoColor=white)

</div>

> **EN** — Personal cross-platform workspace for everything in **COMP9319 — Web Data Compression and Search** (UNSW): assignments, projects, and any written deliverable.
>
> **中文** — 这是 **COMP9319（Web 数据压缩与搜索）** 的个人作业仓库,跨平台同步课程里的作业、项目及一切书面产出。

> 🧩 This repo follows the **same reusable template** as the COMP9312 / COMP9313 repos —
> the Claude-Code memory handling and the one-folder-per-deliverable layout carry over to any
> new course unchanged. **This course's deliverables are all C code**, built and verified on
> CSE — see the deliverable-type note below.
> 本仓库与 COMP9312 / COMP9313 共用**同一套可复用模板**(记忆管理、每个交付物独立文件夹的布局),换课直接沿用。**本课交付物全部为 C 代码**,在 CSE 上编译并验证,详见下方「交付物类型」说明。

---

## 📖 Overview · 简介

Coursework for COMP9319. Each deliverable lives in its own top-level folder, and
everything for one deliverable — solution code, the task spec, study notes, and any
study-only reference material — lives inside that folder.

本仓库汇总 COMP9319 的课程产出。每个交付物各占一个顶层文件夹,且一个交付物相关的全部
内容(解答代码、题面、学习笔记、仅供学习参考的外部实现)都收在同一个文件夹里。

## 📂 Structure · 目录结构

| Folder · 文件夹 | Contents · 内容 |
| --- | --- |
| [`assignment1/`](assignment1/) | **15-bit LZW encoder / decoder** in C (`lencode.c` / `ldecode.c`).<br>**15-bit LZW 编码 / 解码器**(C 语言)。<br>设计说明见 [`assignment1/README.md`](assignment1/README.md);`spec.md` 为题面,`notes.md` 为 LZW 学习笔记,`reference/` 为仅供学习的外部实现。 |
| [`assignment2/`](assignment2/) | **BWT backward search** (`bwtsearch`) over run-length-encoded BWT DNA files, implemented in C (`bwt.c` / `bwt.h` / `bwtsearch.c`) and verified on CSE.<br>**BWT 反向搜索**:在行程编码的 BWT DNA 文件(`.rbwt`)上做 FM-index 检索,C 语言实现,已在 CSE 上验证通过。<br>题面见 [`assignment2/spec.md`](assignment2/spec.md),学习笔记见 [`assignment2/notes.md`](assignment2/notes.md),CSE 上机测试说明见 [`assignment2/TESTING_ON_CSE.md`](assignment2/TESTING_ON_CSE.md)。 |
| [`claude-memory/`](claude-memory/) | Claude Code 跨机记忆镜像(见下方说明)。 |

### Build & Run · 编译与运行

```bash
gcc -O2 -o lencode lencode.c     # 编码器
gcc -O2 -o ldecode ldecode.c     # 解码器

lencode <input_file> <output_file>   # 压缩
ldecode <input_file> <output_file>   # 解压
```

> 对任意 7-bit ASCII 输入,`ldecode(lencode(x)) == x`(逐字节一致)。

## 🧠 `claude-memory/`

A mirror of Claude Code's persistent memory for this project, committed to the repo
so prior context follows you across machines.

这是 Claude Code 针对本项目的持久记忆镜像,随仓库一起提交,让 AI 的历史上下文能跨机器
跟随你。在新机器上 clone 后,把这些文件复制到 Claude Code 的项目记忆目录
(`~/.claude/projects/<project-key>/memory/`,`project-key` 由该机器上的项目绝对路径推导)。

## 📝 Deliverable Type · 交付物类型

Every COMP9319 deliverable is a **code / auto-marked** task built and graded on **CSE** — so the
deliverable **is** the C source **+ its tests** in each folder (`lencode.c` / `ldecode.c`;
`bwt.c` / `bwt.h` / `bwtsearch.c` + `makefile`), alongside each folder's `spec.md`, `notes.md`,
and `TESTING_ON_CSE.md`. There is **no** `.tex` / `_solution.md` write-up pair — that Markdown +
LaTeX solution convention applies only to human-graded PDF deliverables, and this course has none.

本课每个交付物都是**代码 / 自动评测**任务,在 **CSE** 上编译并评分——因此交付物**就是**各文件夹
内的 **C 源码 + 测试**(`lencode.c` / `ldecode.c`;`bwt.c` / `bwt.h` / `bwtsearch.c` + `makefile`),
连同各文件夹的 `spec.md`、`notes.md`、`TESTING_ON_CSE.md`。**没有** `.tex` / `_solution.md` 这一对
书面解答文件——那套 Markdown + LaTeX 约定只用于人工评分的 PDF 交付物,而本课没有此类交付物。
