# Memory Index

- [COMP9319 repo](comp9319-repo.md) — private GitHub repo YangMingUNSW/comp9319, clean template store for all course deliverables (assignments/projects).
- [Solution file convention](solution-file-convention.md) — every deliverable (assignment or project) ships a readable `_solution.md` plus a LaTeX `_solution_pdf.md` for Overleaf.
- [Sync memory to repo](sync-memory-to-repo.md) — mirror CC memory files into the repo's `claude-memory/` for cross-platform memory continuity.
- [Auto-push after changes](auto-push-after-changes.md) — commit + push to GitHub after every code/file change so the user can trace/rollback/debug.
- [A1 (LZW) status](comp9319-a1-status.md) — 2026T2 Assignment 1 spec (15-bit LZW, reset-when-full), study phase only, solution NOT started; reference impls under reference/.
- [A2 (BWT search) status](comp9319-a2-status.md) — 2026T2 Assignment 2 (bwtsearch: FM-index backward search over RLE-encoded BWT DNA); **VERIFIED ON db-perftest 2026-07-17: autotest 8/8, 93 dsearch cross-checks 0 fail, massif 6.98MB/16MB, 0.12s/5s — all pass**. Note: repo spec.md had fabricated a `bwtdecode` requirement (does not exist in the official spec); corrected.
