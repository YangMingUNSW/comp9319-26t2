# COMP9319 2026T2 — Assignment 2 (BWT backward search)

Search a **run-length-encoded BWT** DNA file (`.rbwt`) using **FM-index backward
search**, printing each match with up to 2 characters of context on each side.

> **Status: complete and verified on `db-perftest` (2026-07-17).** Official
> `autotest` **8/8 CORRECT**; **93 `dsearch` cross-checks, 0 failures** across all
> five sample files; massif peak **6.96MB** of the 16MB budget; worst in-spec
> search **0.12s** of the 5s budget; no file writes. Full transcript in
> [`cse-test/`](./cse-test/). Only submission remains — see below.

## Files

### Submitted (`give cs9319 a2 makefile *.c *.cpp *.h`)

| File | Role |
|---|---|
| `makefile` | builds `bwtsearch` with a bare `make` |
| `bwtsearch.c` | backward search → `[sp,ep]`; preceding context via LF, following context via extended searches (`P·c1`, `P·c1·c2`) that partition the interval |
| `bwt.c` / `bwt.h` | FM-index over the RLE BWT: sampled checkpoints (~2MB) + on-demand file reads; `fm_build`, `fm_rank`, `fm_access` |

That is the whole submission — four files. **`bwtdecode` is not part of this
assignment**: the official spec never mentions it (see [`spec.md`](./spec.md) §7).

### Not submitted (test aids; `give` only takes `makefile *.c *.cpp *.h`)

| File | Role |
|---|---|
| `crosscheck.sh` | diffs `bwtsearch` against the official `~cs9319/a2/dsearch` over start/middle/**end** terms at many lengths, plus self-overlapping terms |
| `run_cse_tests.sh` | runs every check in `TESTING_ON_CSE.md` and tees one transcript to `cse-test/<commit>/` |

### Design notes (why it fits the constraints)

- **Never decodes the whole BWT.** The decoded BWT can dwarf the `.rbwt`, so the
  index keeps only cumulative-count checkpoints every `S` bytes (`S =
  fileSize/50000`, min 32 → ~50k checkpoints ≈ 2MB regardless of file size) and
  re-scans a short RLE span from the file per query. Measured: the massif peak is
  effectively **flat** in file size (5.26MB on a 178KB `.rbwt`, 6.96MB on a 58MB
  one).
- **File bytes are read via stdio, not `mmap`**, so they live in the OS page cache
  and are *not* counted by `massif --pages-as-heap=yes`. This was the design's
  main bet; it held on db-perftest.
- **No `select`/ψ anywhere.** Following context is obtained by searching `P·c1`
  and `P·c1·c2`, which partition `[sp,ep]` into sub-intervals sharing the same
  trailing chars — O(pattern_len) work, not O(matches·len).
- **`\n` is the sequence boundary**: context stops there, never wraps, never
  prints it. This was the one behaviour local testing could not settle, and the
  `dsearch` cross-check confirmed it.
- **Writes no files.**

### Known limit (within spec)

`bwtsearch` buffers per-match context (`f1`/`f2`, `ep-sp+1` bytes each), so memory
and time scale with match count. Measured on a 58MB `.rbwt`: 1023 matches → 0.12s;
256k matches → 1.58s. This is safe only because spec assumption (5) guarantees
**no test case exceeds 5000 matches**. Streaming the matches instead of buffering
would remove the dependence, but it is not needed to pass.

## Task at a glance

| | |
|---|---|
| Program | `bwtsearch <rbwt_file> <search_term>` |
| Input | RBWT file (RLE-encoded BWT of a DNA `.txt`; chars `A C G T \n`) |
| Output | per match: `≤2 preceding` + `match` + `≤2 following`, one per line |
| Limits | **< 16 MB** memory (massif, `--pages-as-heap=yes`), **< 5 s** / search |
| Files | DNA files ≤ 110 MB (largest `.rbwt` provided is 58 MB); **no writing any files** |
| Worth | 35 pts + 3.5 bonus (fastest); auto-marked |
| Deadline | **Tue 28 July 2026, 5:00pm AEST** |

## RLE format (one byte = one run)

`[3 bits char][5 bits run-1]` — `A=000 C=001 G=010 T=011 \n=100`; actual run
length = stored value + 1 (so `0..31` → `1..32`); runs > 32 split across bytes.

## Build & test (on db-perftest)

```bash
make                              # builds bwtsearch
./bwtsearch ~cs9319/a2/dna-tiny.rbwt ACTGAC   # -> 4 matches, one per line
~cs9319/a2/autotest               # official sanity: 8 tests on dna-tiny
./crosscheck.sh                   # official dsearch cross-check, all sample files
```

Step-by-step guide (Chinese): [`TESTING_ON_CSE.md`](./TESTING_ON_CSE.md).
Algorithm notes: [`notes.md`](./notes.md). Task: [`spec.md`](./spec.md).

## Submit

Run on a **normal CSE machine** (vx*/vlab) — `give` does not exist on db-perftest:

```bash
give cs9319 a2 makefile *.c *.cpp *.h
9319 classrun -check a2
```
