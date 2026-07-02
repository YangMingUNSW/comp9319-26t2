# COMP9319 2026T2 — Assignment 2 (BWT backward search)

Search a **run-length-encoded BWT** DNA file (`.rbwt`) using **FM-index backward
search**, printing each match with up to 2 characters of context on each side.

> **Status: study / preparation phase.** Spec and learning notes are ready;
> `bwtsearch.c` / `bwtdecode.c` are **not started yet**. See [`notes.md`](./notes.md)
> for the algorithm study path and [`spec.md`](./spec.md) for the task.

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

- `~cs9319/a2/autotest` — sanity check on `dna-tiny.rbwt` (formatting only).
- `~cs9319/a2/dsearch <dna.txt> <term>` — reference answers on the **plaintext**;
  your `.rbwt` output must match (ignoring order — both get sorted).

## Submit

```bash
give cs9319 a2 makefile *.c *.cpp *.h
9319 classrun -check a2
```
