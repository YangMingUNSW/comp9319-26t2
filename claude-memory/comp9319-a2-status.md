---
name: comp9319-a2-status
description: "COMP9319 2026T2 Assignment 2 (BWT backward search) — spec, constraints, current phase"
metadata: 
  node_type: memory
  type: project
  originSessionId: 5e055e56-a7ab-40ac-a8db-e4590faf0cbf
---

COMP9319 **2026T2 Assignment 2** = implement `bwtsearch` (C/C++) doing **BWT
backward search / FM-index** over a **run-length-encoded BWT** DNA file (`.rbwt`).
Spec: <https://cgi.cse.unsw.edu.au/~wong/cs9319-2026a2.html>. Worth **35 pts** +
**3.5 bonus** (fastest overall). Deadline **Tue 28 July 2026 5:00pm AEST**; auto-marked.
`give cs9319 a2 makefile *.c *.cpp *.h`; check `9319 classrun -check a2`.

**Task:** `bwtsearch <rbwt_file> <search_term>` (term 1–128 chars, only A/C/G/T).
Output each occurrence (incl. **overlapping**) as `≤2 preceding chars + match +
≤2 following chars`, one per line, `\n`-terminated, to stdout. Order doesn't
matter (marker sorts both sides); **any extra text loses marks**; **≤5000 matches
per test** guaranteed. Verify against `~cs9319/a2/dsearch <dna.txt> <term>`
(reference on plaintext) and `~cs9319/a2/autotest` (formatting sanity on dna-tiny).

**RLE format** (assignment-specific): one byte = `[3-bit char][5-bit run-1]`;
`A=000 C=001 G=010 T=011 \n=100`; **actual run length = stored value + 1** (0→1,
31→32); runs >32 split across bytes. Decode: `ch=byte>>5; len=(byte&0x1F)+1`.
Char collation by ASCII: `\n`(10) < A(65) < C(67) < G(71) < T(84) → that's the
`C[]` order. Hand-decoded `dna-tiny.rbwt` = `ATGCTGGGG\nAATCTAAAAAAATTTTTACAGTGGCCCCCC`
(matches dna-tiny.bwt), confirming the byte layout.

**Hard constraints:** total runtime memory **< 16MB** (`valgrind --tool=massif
--pages-as-heap=yes` — counts ALL memory incl. mmap/segments; evasion checked
manually) → **cannot** hold decoded full BWT for up-to-**110MB** files; need RLE
kept compressed + sampled `Occ`/rank. Time **< 5s per search**. **No writing any
file (even temp) → zero for whole assignment.** ≥ half of tests use `.rbwt` < 5MB.
Compiled/run/tested on **db-perftest** (`ssh zID@db-perftest.cse.unsw.edu.au` via a
CSE machine). `make` must build BOTH **`bwtsearch`** AND **`bwtdecode`**; any
compile error → zero.

**Open items to confirm on CSE before coding:** (1) `bwtdecode` behaviour is
unspecified in the body — almost certainly `bwtdecode <rbwt>` → print original DNA
(invert BWT); check sample makefile/autotest in `~cs9319/a2`. (2) massif example
uses `< mytest.in` (stdin) though term is a CLI arg — likely template leftover.
(3) whether context extraction crosses/prints the `\n` sequence boundary — settle
by `dsearch` diff.

**Current phase (as of 2026-07-02): STUDY / PREP only — solution NOT started.**
In-repo `assignment2/` holds `spec.md` (full spec writeup), `notes.md` (Chinese
FM-index study notes + self-check), `README.md`, `.gitignore`. Next step (needs
user OK): implement `bwtsearch.c`/`bwtdecode.c` + makefile, then round-trip vs
`dsearch`. Follow [[solution-file-convention]] and the per-deliverable folder rule
in [[comp9319-repo]]. Mirrors [[comp9319-a1-status]]'s prep structure.
