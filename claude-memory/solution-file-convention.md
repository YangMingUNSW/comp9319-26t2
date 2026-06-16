---
name: solution-file-convention
description: Two-file output convention for every COMP9319 deliverable (assignment or project)
metadata:
  node_type: memory
  type: feedback
---

For every COMP9319 written deliverable — assignment OR project — produce the solution in TWO files (do not delete the readable one):

- `<name>_solution.md` — readable Markdown solution.
- `<submission>.tex` — the SAME content as a full LaTeX document (`\documentclass … \end{document}`), as a real `.tex` file named to match the submission PDF (e.g. `ass1_z1234567.tex`), so the user can upload it to Overleaf and compile to PDF for submission.

**Why:** the user submits PDFs built in Overleaf; using a true `.tex` file (not a `.md` that secretly holds LaTeX) removes the extension confusion and uploads cleanly. The `.tex` is the single source for the PDF — edit it to fix the PDF, then regenerate in Overleaf. This is the standing cross-platform writing workflow, reused for every new course. (Older repos used a `<name>_solution_pdf.md`; that pattern is retired.)

**How to apply:** target Overleaf's default pdfLaTeX — use ASCII art inside `verbatim` (no Unicode box-drawing or `∞`), math mode for all complexities, and `\resizebox{\textwidth}{!}{…}` for wide tables. See [[comp9319-repo]].
