# COMP9319 2026T2 — Assignment 2 (BWT backward search)

Search a **run-length-encoded BWT** DNA file (`.rbwt`) using **FM-index backward
search**, printing each match with up to 2 characters of context on each side.

> **Status: solution implemented & locally verified.** `bwtsearch.c`,
> `bwtdecode.c`, `bwt.c`/`bwt.h` and the `makefile` are done and pass all spec
> examples plus randomized brute-force stress tests (see below). Still to do on
> CSE: build on **db-perftest**, `dsearch`/`autotest` diff, and massif memory
> check. See [`notes.md`](./notes.md) for the algorithm and [`spec.md`](./spec.md)
> for the task.

## Files

| File | Role |
|---|---|
| `bwt.h` / `bwt.c` | shared FM-index over the RLE BWT: sampled checkpoints (~2MB) + on-demand file reads; `fm_rank`, `fm_access`, `fm_select`, `fm_fchar` |
| `bwtsearch.c` | backward search → `[sp,ep]`; preceding context via LF, following context via extended searches (`P·c1`, `P·c1·c2`) that partition the interval |
| `bwtdecode.c` | forward reconstruction via `psi` (streamed, memory-bounded) → prints original DNA |
| `makefile` | builds both `bwtsearch` and `bwtdecode` |

### Design notes (why it fits the constraints)

- **Never decodes the whole BWT.** The decoded BWT can dwarf the ≤110MB `.rbwt`,
  so the index keeps only cumulative-count checkpoints every `S` bytes (~50k
  checkpoints ≈ 2MB) and re-scans a short RLE span from the file per query.
- **File bytes are read via stdio, not `mmap`**, so they live in the OS page
  cache and are *not* counted by `massif --pages-as-heap=yes`. Measured RSS on a
  200k-char test was ~1.6MB.
- **No `select`/ψ in `bwtsearch`'s hot path.** Following context is obtained by
  searching `P·c1` and `P·c1·c2`, which partition `[sp,ep]` into sub-intervals
  sharing the same trailing chars — O(pattern_len) work, not O(matches·len).
- **Writes no files.**

### Local verification (in this repo's build)

`dna-tiny.rbwt` was reconstructed from the spec's `xxd -b` dump (20 bytes) and
all worked examples reproduce exactly (`TGAACTT`→`ACTGAACTTAC`; `ACTGAC`→4 lines;
`ACT`→ the 6-line `dsearch` output; `bwtdecode`→ the original text). A randomized
harness (~3900 searches over random + repetitive DNA, incl. runs >32) matched a
brute-force reference with **0 failures**, and decode round-trips were exact.

## Task at a glance

| | |
|---|---|
| Program | `bwtsearch <rbwt_file> <search_term>` (+ `bwtdecode`, see spec §7) |
| Input | RBWT file (RLE-encoded BWT of a DNA `.txt`; chars `A C G T \n`) |
| Output | per match: `≤2 preceding` + `match` + `≤2 following`, one per line |
| Limits | **< 16 MB** memory (massif, `--pages-as-heap=yes`), **< 5 s** / search |
| Files | ≤ 110 MB; **no writing any files**, even temporary |
| Worth | 35 pts + 3.5 bonus (fastest); auto-marked |
| Deadline | **Tue 28 July 2026, 5:00pm AEST** |

## RLE format (one byte = one run)

`[3 bits char][5 bits run-1]` — `A=000 C=001 G=010 T=011 \n=100`; actual run
length = stored value + 1 (so `0..31` → `1..32`); runs > 32 split across bytes.

## Build & run (on db-perftest)

```bash
make                              # builds bwtsearch and bwtdecode
./bwtsearch dna-tiny.rbwt ACTGAC  # -> 4 matches, one per line
```

## Testing

Step-by-step guide (Chinese): [`TESTING_ON_CSE.md`](./TESTING_ON_CSE.md) — covers the
three zero-mark red lines (16 MB / 5 s / no file writes). Results are archived per
commit hash under [`cse-test/`](./cse-test/).

- `~cs9319/a2/autotest` — sanity check on `dna-tiny.rbwt` (formatting only).
- `~cs9319/a2/dsearch <dna.txt> <term>` — reference answers on the **plaintext**;
  your `.rbwt` output must match (ignoring order — both get sorted).
- Memory (< 16 MB) via `valgrind --tool=massif --pages-as-heap=yes` + `ms_print`;
  time (< 5 s) via `/usr/bin/time` — **both must be measured on `db-perftest`,
  with the largest test file.**

## Submit

```bash
give cs9319 a2 makefile *.c *.cpp *.h
9319 classrun -check a2
```
