# COMP9319 2026T2 Assignment 2: Searching BWT Transformed DNA Sequence (Run-length Encoded)

> Original page: <https://cgi.cse.unsw.edu.au/~wong/cs9319-2026a2.html>
> Worth **35 points**, auto-marked. Deadline **Tuesday 28 July 2026, 5:00pm AEST**.

---

## 1. Task

Create a C or C++ search program called **`bwtsearch`** that implements **BWT
backward search** to efficiently search a BWT-transformed DNA file that has been
**run-length encoded** (an `.rbwt` file). Correctness *and* space/time
performance are marked.

Sample files live at `~cs9319/a2` on any CSE Linux machine. Original DNA files
(`.txt`) contain the four letters `C, G, A, T` (nucleobases Cytosine, Guanine,
Adenine, Thymine) terminated by a newline `\n`.

```text
cs9319@vx09:~/a2$ cat dna-tiny.txt
ACTGACTGACTGACTGACTGAACTTACGTAGTCCAAGTA

cs9319@vx09:~/a2$ cat dna-tiny.bwt
ATGCTGGGG
AATCTAAAAAAATTTTTACAGTGGCCCCCC
```

(The `.bwt` above is the BWT of the `.txt`; it itself contains a `\n`, so it
prints across two visual lines.) BWT compresses extremely well via RLE/MTF, and
backward search still works directly on the compressed form (whereas `grep`
cannot). **You must search on the run-length encoded BWT (`.rbwt`) file.**

---

## 2. The RLE scheme (assignment-specific)

Each **run** of consecutive repeated characters in the BWT is encoded into one
or more bytes. Each byte:

| Bits | Meaning |
|---|---|
| First 3 bits (MSBs) | the character |
| Last 5 bits (LSBs) | the run length |

Character codes:

| Char | Code |
|---|---|
| `A` | `000` |
| `C` | `001` |
| `G` | `010` |
| `T` | `011` |
| `\n` | `100` |

The 5-bit run length holds `0..31`, but the **actual run length is one more** than
the stored value: stored `0` → length `1`, stored `31` → length `32`.

If a run is longer than 32, it is **split across multiple bytes**. Example: 35
consecutive `G` → two bytes: one run of 32 `G`, then one run of 3 `G`.

### Worked example

BWT sequence `AAACCCGGGG…(35 G total)…GGTT\n` encodes to **6 bytes**:

```text
00000010 00100010 01011111 01000010 01100001 10000000
   A×3      C×3      G×32     G×3      T×2      \n×1
```

The `dna-tiny.rbwt` for the first example:

```text
cs9319@vx09:~/a2$ xxd -b dna-tiny.rbwt
00000000: 00000000 01100000 01000000 00100000 01100000 01000011  .`@ `C
00000006: 10000000 00000001 01100000 00100000 01100000 00000110  ..` `.
0000000c: 01100100 00000000 00100000 00000000 01000000 01100000  d. .@`
00000012: 01000001 00100101                                      A%
```

> Decoded by hand this yields exactly `ATGCTGGGG\nAATCTAAAAAAATTTTTACAGTGGCCCCCC`
> (40 chars), matching `dna-tiny.bwt`. Confirms the byte layout above.

**During marking, only the `.rbwt` files are provided.** All other files
(`.txt`, `.bwt`, …) exist only for your own reference/testing. Submissions may be
re-tested against a *different* set of RBWT files to detect hardcoding.

---

## 3. Program behaviour

`bwtsearch` accepts **two command-line arguments**:

1. path to the input file (an RBWT-encoded file);
2. a search term of length **1 to 128** characters.

It performs a BWT backward search for **all occurrences** of the search term and
prints each match on its own line to **stdout**. For every match, output:

- up to **2 characters immediately preceding** the match,
- the **matched substring** itself,
- up to **2 characters immediately following** the match.

If fewer than two characters are available before the start of the sequence or
after the end, output only the available characters. The search must consider
**all substring positions, including overlapping matches** — e.g. searching `AA`
in `AAA` gives two matches (positions 1–2 and 2–3).

```text
cs9319@vx09: ./bwtsearch ~cs9319/a2/dna-tiny.rbwt TGAACTT
ACTGAACTTAC
```

(matched `TGAACTT`, preceded by `AC`, followed by `AC`.)

```text
cs9319@vx09: ./bwtsearch ~cs9319/a2/dna-tiny.rbwt ACTGAC
ACTGACTG
TGACTGACTG
TGACTGACTG
TGACTGACTG
```

### Output requirements

- One match per line; **each line ends with `\n`**.
- Number of matches == number of output lines (`wc -l`).
- **Order does not matter** — both your output and the reference are *sorted*
  before comparison.
- **Marks are deducted for any extra text** (debug messages, line numbers, …).

---

## 4. Correctness testing

- `~cs9319/a2/autotest` — basic sanity script on `dna-tiny.rbwt` (checks argument
  handling and output formatting only; does not exercise large files).
- `~cs9319/a2/dsearch <dna_file> <search_term>` — reference tool that searches the
  **plain-text** DNA file and produces the correct answer. For a `.txt` and its
  matching `.rbwt`, your program's output on the `.rbwt` must equal `dsearch`'s
  output on the `.txt` (ignoring order). Use it to generate expected answers.

```text
cs9319@vx09: ~cs9319/a2/dsearch ~cs9319/a2/dna-tiny.txt ACT | sort
ACTGA
GAACTTA
TGACTGA
TGACTGA
TGACTGA
TGACTGA
```

### Assumptions (given)

1. Search is **case sensitive** (no lowercase to worry about).
2. Search term is **never empty**.
3. Search term contains **only `A, C, G, T`**.
4. Search term is **≤ 128** characters.
5. No test case produces **more than 5000 matches**.

**Your solution must not write any files (even temporary) — doing so scores zero
for the whole assignment.**

---

## 5. Compiling & performance

- Compiled/run/tested on **`db-perftest`** (`ssh zID@db-perftest.cse.unsw.edu.au`
  via a CSE Linux machine such as vlab).
- Provide a **`makefile`**; a sample one is in `~cs9319/a2`. The makefile must
  build the executables **`bwtsearch`** and **`bwtdecode`** on db-perftest. You
  may use any C/C++ library available there.
- Compiled with `make`. **Any compilation error → zero for the whole assignment.**
- **Memory:** total runtime memory of *each* program (including footprint) is
  assumed **always < 16 MB**, measured by:

  ```bash
  valgrind --tool=massif --pages-as-heap=yes --massif-out-file=memory.out \
      ./bwtsearch ~/a2/dna-small.rbwt < mytest.in
  ms_print memory.out > memory.txt
  ```

  `--pages-as-heap=yes` measures **all** memory used. Deliberately allocating to
  evade measurement is checked manually. Violating 16 MB → zero for those tests.
- **Time:** measured with `/usr/bin/time` (user + system time). **Limit: 5 s per
  search.** Over-limit runs are killed and score zero for that test.
- File sizes: **never larger than 110 MB**; at least half the test cases use
  `.rbwt` files **smaller than 5 MB**.

---

## 6. Marking, bonus, submission

- **35 points**, auto-marked; code manually checked for readability & plagiarism.
  Marks are not scaled up if the code is hard to read.
- **Bonus:** 10% of the assignment (3.5 pts) for the solution that passes *all*
  tests (correct, within memory & time) and has the **fastest overall** total
  runtime. Final subject mark is still capped at 100.
- **Submit:**

  ```bash
  give cs9319 a2 makefile *.c *.cpp *.h
  9319 classrun -check a2      # verify (run on a normal CSE machine, not db-perftest)
  ```

- **Deadline:** Tuesday 28 July 2026 5:00pm AEST. Late penalty 5%/day, max 5 days
  (7 with a granted extension).
- **Plagiarism:** work must be your own; penalties include negative marks and
  course failure.

---

## 7. Open questions / things to confirm on CSE

- **`bwtdecode`**: §5 says the makefile must also build `bwtdecode`, but the body
  only specifies `bwtsearch`'s behaviour. Almost certainly `bwtdecode <rbwt>`
  reconstructs/prints the original DNA (RBWT → BWT → invert BWT → text). Confirm
  the exact usage from the sample makefile / autotest in `~cs9319/a2`.
- **`< mytest.in`** in the massif command suggests stdin might feed input, yet the
  body says the search term is a command-line argument. Likely a template
  leftover (stdin ignored); verify with `autotest`.
- Character collation order for the FM-index `C[]` table: by ASCII value the order
  is `\n`(10) < `A`(65) < `C`(67) < `G`(71) < `T`(84). Confirm the reference BWT
  was built with this ordering (it matches the decoded `dna-tiny` example).
