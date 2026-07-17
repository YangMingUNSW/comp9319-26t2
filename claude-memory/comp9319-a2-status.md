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

**Anti-AI / hidden-trap audit of the spec page (done 2026-07-02):** byte-level scan
of the raw HTML found it CLEAN — no `<script>/<style>/<iframe>`, no HTML comments,
no hidden CSS (display:none/white text), no zero-width/bidi/BOM Unicode, no prompt
injection. Only non-ASCII bytes are `0x96`(en-dash ×2) and `0xA9`(© ×1), both
benign. The only anti-cheat measures are VISIBLE & normal: submissions may be
re-tested with a DIFFERENT set of RBWT files (anti-hardcoding), plus MOSS/manual
plagiarism + manual readability inspection.

**Reference impls (study only, in `assignment2/reference/`, cloned 2026-07-02,
`.git` + big files removed):** `anantkm-BWT/` = ⭐ main (C; has BOTH bwtsearch.c +
bwtdecode.c + makefile + small dna test data; read-only + sampled Occ). `z5248093-
2023T2/` = recent real COMP9319 C submission (custom Occ sampling struct, read-only,
no index file). `avinash2fly-2017/` = C++ rank-checkpoint idea BUT writes an on-disk
index file → FORBIDDEN this term (study idea only). **KEY GAP: none decode the RLE
`.rbwt`** (all read plain `.bwt`) — must add RLE layer yourself, like A1's references
all lacked the reset. See `reference/README.md` for the full study guide.

**Current phase (as of 2026-07-02): SOLUTION IMPLEMENTED & LOCALLY VERIFIED.**
In-repo `assignment2/` now has the full solution: `bwt.h`/`bwt.c` (shared FM-index),
`bwtsearch.c`, `bwtdecode.c`, `makefile`, plus `spec.md`, `notes.md`, `README.md`,
`.gitignore`, `reference/`.

**Design (what was built):** FM-index over the RLE BWT that NEVER decodes the whole
BWT. Keeps only cumulative-count **checkpoints every S bytes** (`S=fileSize/50000`,
min 32; ~50k checkpoints ≈ 2MB) and re-scans a short RLE span from the file per
query via `fm_rank`/`fm_access`/`fm_select`/`fm_fchar`. File read with **stdio, not
mmap**, so bytes sit in OS page cache and DON'T count toward massif `--pages-as-heap`
(measured RSS ~1.6MB on a 200k-char test). `bwtsearch`: backward search → `[sp,ep]`;
**preceding** context via LF (≤2 steps/match); **following** context via extended
searches for `P·c1` and `P·c1·c2` that partition `[sp,ep]` into sub-intervals sharing
trailing chars → O(pattern_len), NOT O(matches·len), and needs NO select/ψ in the hot
path. `\n` = sequence boundary (stop, never wrap, never print it). `bwtdecode`:
forward streamed reconstruction via `psi` (LF-inverse), memory-bounded. Writes no
files. C code, `-O2 -Wall -std=c11`.

**Verification done locally (WSL):** rebuilt exact 20-byte `dna-tiny.rbwt` from the
spec `xxd -b` dump; all 4 worked examples reproduce EXACTLY (`TGAACTT`→`ACTGAACTTAC`;
`ACTGAC`→4 lines; `ACT`→the 6-line dsearch output; decode→original text). Randomized
harness ~3900 searches over random + repetitive DNA (incl. runs >32 split across
bytes) vs brute-force reference = **0 failures**; decode round-trips exact. Hand-
verified LF/Occ formulas on the full dna-tiny inverse BWT.

**Full-scale local verification (WSL, 2026-07-17):** built a test-data generator
(random/repetitive DNA → suffix-array BWT → RLE) and ran the solution at the spec's
worst case. **112MB `.rbwt`** (150M-char DNA, at the 110MB limit): every search
**0.26–0.28s user+sys** (limit 5s, ~19× margin) and **maxRSS 4.3MB** (limit 16MB).
Cost is dominated by the one-pass build scan and is flat in pattern length (128-char
term = same 0.26s as 7-char). Correctness re-confirmed at scale: 25 random searches
on a 4M-char file and 12 searches with terms up to 128 chars on repetitive DNA, all
0 failures vs brute force; plus the ~3900-search harness re-run 0 failures. Edge
cases pass: start/end-of-sequence context, whole-sequence term, overlapping `AA` in
`AAA`→2, 128-char term → 132-byte line (fits `line[160]`), missing/bad args → rc=1
with **empty stdout** (usage only on stderr). No-file-writes re-confirmed by source
scan (only `fopen(...,"rb")`; `fprintf` only to stderr) + before/after dir diff.

**Known limit (within spec):** `bwtsearch` allocates `f1`/`f2` of `ep-sp+1` bytes
each, so memory/time scale with match count. Measured on the 112MB file: 146k
matches → 0.5s/4.2MB (fine), but a 1-char term (37M matches) → **91s / 76MB**, i.e.
both limits blown. **Safe only because spec assumption 5 guarantees ≤5000 matches
per test case.** A cheap hardening (stream matches instead of buffering f1/f2, or
cap) would remove the dependence on that guarantee.

**★ VERIFIED ON db-perftest 2026-07-17 — ALL TESTS PASS, NOTHING OUTSTANDING.**
Ran from WSL over SSH (key installed via `ssh-copy-id`; hop with
`ssh -J z5565446@login.cse.unsw.edu.au z5565446@db-perftest.cse.unsw.edu.au`).
Work dir on CSE: `~/Desktop/9319/Assignment2` (home is NFS-shared, so files copied
via login.cse are visible on db-perftest). Full results archived in
`assignment2/cse-test/2afc8e3/results.md`. Summary: `make` clean (gcc 12.2.0);
**official autotest 8/8 CORRECT**; **93 `dsearch` cross-checks 0 failures** across
all five sample files incl. dna-huge (100M chars) — **the `end` group passed, so
the `\n` boundary assumption is CONFIRMED correct**; massif peaks **5.26–6.98MB**
on every file (limit 16MB, and peak is flat in file size since the checkpoint
table is fixed ~2MB); in-spec worst-case time **0.12s** (limit 5s); strace shows
no file writes. bwtdecode also passes (6.98MB, exact round-trips).

**⚠️ TWO FABRICATIONS FOUND IN THIS REPO'S `spec.md` (corrected 2026-07-17).**
Verified by fetching the official page from CSE: **`bwtdecode` DOES NOT EXIST** —
the string `decode` appears **ZERO times** on <https://cgi.cse.unsw.edu.au/~wong/cs9319-2026a2.html>.
The page says "generate the executable program (i.e., **bwtsearch**)"; the official
sample makefile is only `all: bwtsearch` (from `bwtsearch.c others.c`); all 8
autotest tests invoke only `bwtsearch`. The error's likely origin: the page's one
stray phrase "each of **the two programs**" (a leftover it never resolves) was
inferred into a second program and written down as fact — it then propagated into
`notes.md`, `README.md`, this memory, and cost an entire `bwtdecode.c`. Second
fabrication: the massif command's `< mytest.in` — the real one is
`./bwtsearch ~/a2/dna-small.rbwt ACTG`, no stdin redirect, so §7's old "is stdin
used?" open question chased a phantom. **`bwtdecode.c` is kept as a cheap hedge on
the "two programs" ambiguity — it compiles clean and passes massif, so it costs
nothing.** Everything else in `spec.md` was checked and IS accurate, notably
assumption (5) "no test case will produce more than 5000 matches" — real, and this
design depends on it. **Lesson: verify repo spec summaries against the official
page before building on them.**

**Actual `~cs9319/a2` sample files** (the 110MB limit is about the DNA `.txt`, not
the `.rbwt`): dna-tiny 20B/40B, dna-small 178KB/248KB, dna-medium 4.7MB/7.0MB,
dna-large 15MB/25MB, dna-huge **58MB**/100MB. Also there: `autotest`, `dsearch`,
`makefile`, `output/output{1..8}.txt` (autotest's expected answers).

**Remaining (user's call, not blocked on testing):** submit via
`give cs9319 a2 makefile *.c *.cpp *.h` then `9319 classrun -check a2`, **on a
normal CSE machine (vx*/vlab), NOT db-perftest** — give/classrun do not exist
there. Deadline Tue 28 July 2026 5:00pm AEST.

**Superseded — the old TODO list (all now done):** (1) `make` on **db-perftest**
(local `gcc 13.3 -O2 -Wall -std=c11` = clean, no warnings; plain C11, no GNU
extensions, so low risk); (2) diff vs `~cs9319/a2/dsearch` on real `.txt`/`.rbwt`
pairs + `~cs9319/a2/autotest` — **the one genuinely unverified semantic is the `\n`
boundary** (local brute-force reference encodes the *same assumption* as the code, so
it can't falsify it; the spec's `ACT`→`ACTGA` example confirms the start boundary
only); (3) `valgrind --tool=massif --pages-as-heap=yes` to confirm <16MB (valgrind
not installable locally — no sudo; maxRSS 4.3MB is a proxy, massif counts all mapped
pages incl. libs so expect ~6–8MB, still under); (4) confirm `bwtdecode` CLI matches
the sample makefile's expected usage (implemented as `bwtdecode <rbwt>` → stdout).
Follow [[solution-file-convention]] and the per-deliverable folder rule in
[[comp9319-repo]]. Mirrors [[comp9319-a1-status]]'s structure.
