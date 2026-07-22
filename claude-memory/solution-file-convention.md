---
name: solution-file-convention
description: COMP9319 deliverables are code / auto-marked (CSE) — source + tests, no .tex/_solution.md pair
metadata:
  node_type: memory
  type: feedback
---

**COMP9319 deliverables are code / auto-marked, not written-up PDFs.** Every deliverable so far
(assignment1 = 15-bit LZW `lencode`/`ldecode` in C; assignment2 = BWT backward search
`bwtsearch` in C) is a programming task built and graded on **CSE**. The deliverable IS the
**C source + any tests** in the deliverable folder — e.g. `lencode.c` / `ldecode.c`,
`bwt.c` / `bwt.h` / `bwtsearch.c` + `makefile`, plus each folder's `spec.md`, `notes.md`, and
`TESTING_ON_CSE.md`. There is **NO `<name>_solution.md` / `.tex` pair** — the Markdown + LaTeX
"written solution" convention does **not** apply to this course.

**Why:** the two-file (`_solution.md` + `.tex`) workflow only exists for human-graded PDF
deliverables. COMP9319 has none of those; writing solution PDFs here would be dead files no
grader reads. (An older version of this note framed it as "every deliverable" and the repo's
`MEMORY.md` index even referenced a retired `_solution_pdf.md`; both were wrong for this course.)

**How to apply:** focus on the C code's correctness and on verifying it on CSE — build with the
provided `makefile`, run the provided autotest, and cross-check (e.g. round-trip byte-exact for
LZW, `dsearch` cross-check for BWT) before submitting. Treat `reference/` as **study-only**
past-term implementations, never port blindly. Keep each deliverable's source, `spec.md`,
`notes.md`, and CSE test notes together in its folder. See [[comp9319-repo]],
[[comp9319-a1-status]], [[comp9319-a2-status]].
