---
name: solution-file-convention
description: Two-file output convention for every COMP9319 deliverable (assignment or project)
metadata:
  node_type: memory
  type: feedback
---

For every COMP9319 written deliverable — assignment OR project — produce the solution in TWO files (do not delete the readable one):

- `<name>_solution.md` — readable Markdown solution.
- `<name>_solution_pdf.md` — the SAME content but as raw LaTeX source (a full `\documentclass … \end{document}` document), so the user can copy the whole file into Overleaf and compile to PDF for submission. Note: it has a `.md` extension but its contents are LaTeX, not Markdown.

**Why:** the user submits PDFs built in Overleaf but keeps the repo Markdown-only; the `_pdf` suffix tells them which file to paste into Overleaf. This is the standing cross-platform writing workflow, reused for every new course.

**How to apply:** target Overleaf's default pdfLaTeX — use ASCII art inside `verbatim` (no Unicode box-drawing or `∞`), math mode for all complexities, and `\resizebox{\textwidth}{!}{…}` for wide tables. See [[comp9319-repo]].
