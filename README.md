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
> the Claude-Code memory handling and the Markdown + LaTeX solution layout carry over
> to any new course unchanged.
> 本仓库与 COMP9312 / COMP9313 共用**同一套可复用模板**(记忆管理、Markdown + LaTeX 解答布局),换课直接沿用。

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
| [`assignment2/`](assignment2/) | **BWT backward search** over run-length-encoded BWT DNA files (`bwtsearch` / `bwtdecode`).<br>**BWT 反向搜索**:在行程编码的 BWT DNA 文件(`.rbwt`)上做 FM-index 检索。<br>题面见 [`assignment2/spec.md`](assignment2/spec.md),学习笔记见 [`assignment2/notes.md`](assignment2/notes.md)(当前为资料整理阶段,解答尚未开始)。 |
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

## 📝 File Naming Convention · 文件命名约定

For every deliverable (assignment **or** project), each written-up solution comes in
two forms —— 每个交付物(作业或项目)的书面解答都提供两种形式:

- **`*_solution.md`** — readable Markdown solution · 可读的 Markdown 解答。
- **`<submission>.tex`** — the same content as **LaTeX source**, named to match the
  submission PDF (e.g. `ass1_z1234567.tex`); upload to Overleaf and compile to get the
  PDF for submission · 同样内容的 **LaTeX 源文件**,文件名与提交 PDF 对应,上传 Overleaf
  编译即得提交用 PDF。
