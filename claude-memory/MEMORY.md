# Memory Index

- [COMP9319 repo](comp9319-repo.md) — private GitHub repo YangMingUNSW/comp9319, clean template store for all course deliverables (assignments/projects).
- [Solution file convention](solution-file-convention.md) — 9319 deliverables are code / auto-marked (CSE): C source + tests in each folder, no `.tex`/`_solution.md` pair.
- [Sync memory to repo](sync-memory-to-repo.md) — mirror CC memory files into the repo's `claude-memory/` for cross-platform memory continuity.
- [Auto-push after changes](auto-push-after-changes.md) — commit + push to GitHub after every code/file change so the user can trace/rollback/debug.
- [A1 (LZW) status](comp9319-a1-status.md) — 2026T2 Assignment 1 spec (15-bit LZW, reset-when-full), study phase only, solution NOT started; reference impls under reference/.
- [A2 (BWT search) status](comp9319-a2-status.md) — 2026T2 A2 (bwtsearch: FM-index backward search over RLE BWT); **db-perftest 全部通过 2026-07-17**: autotest 8/8, 93 dsearch 对拍 0 fail, massif 6.96/16MB, 0.12/5s. 提交 4 files: makefile+bwtsearch.c+bwt.c+bwt.h. 坑: `reference/` 是往年题面(有 bwtdecode), 别当本期 spec.
