---
name: comp9319-a1-status
description: "COMP9319 2026T2 Assignment 1 (LZW) — current phase, spec, and reference materials"
metadata: 
  node_type: memory
  type: project
  originSessionId: 5c4e45ed-d836-47c9-9588-2956b1f6f758
---

COMP9319 **2026T2 Assignment 1** = implement `lencode`/`ldecode` in C/C++ doing **LZW** with a **15-bit / 32,768-entry** dictionary. Key rules: ASCII = 1 byte (MSB 0), dict index = 2 bytes (MSB 1, high byte first), an index is only output when its entry holds **≥3 chars**, and the dictionary is **reset when full** (this reset-when-full requirement is the ONLY substantive change vs the near-identical 2023T2 version). Deadline **Tue 30 June 2026 5:00pm AEST**; auto-marked /15; `give cs9319 a1`; 5s & ≤1MB per test; `~cs9319/a1/autotest`.

**Current phase (as of 2026-06-17): SOLUTION DONE & verified.** `assignment1/lencode.c` + `assignment1/ldecode.c` (C) implemented and round-trip-tested byte-exact: spec example, edge cases (empty/1/2 bytes), randomised 7-bit inputs, and large low-entropy inputs that trigger many dictionary resets (11 resets on a 1MB 20-symbol file) and the KwKwK case. ~0.01s + ~5MB RAM for 1MB. Submit with `give cs9319 a1 lencode.c ldecode.c`; deadline still 30 June 2026.

**Key implementation insight (why the references fail to round-trip):** the `≥3-char → index` rule means length-2 phrases are emitted as two RAW bytes, not an index. A naive "one output token = one LZW code" decoder then loses phrase boundaries and its dictionary numbering DESYNCS from the encoder once any index is reused — this is the exact reason VegasFlower (and the other public impls) don't round-trip. Fix: the decoder recovers the original characters and feeds every char back through the **encoder's own automaton** (`feed_char`), reproducing the encoder's dict/indices/reset-timing char-for-char; index tokens then resolve correctly, and KwKwK is `prevPhrase + prevPhrase[0]`. Dict = 15-bit trie via a phrase hash table keyed `(parentNode<<7)|char` with O(1) epoch-counter reset; single ASCII chars never use index space. Reset-when-full = interpretation (a): emit p, clear dict, `p=c`, no add at the boundary (both sides identical → lossless). A1 is auto-marked CODE only (no PDF), so [[solution-file-convention]]'s `.tex`/`_solution.md` does NOT apply here.

**Build/test environment:** no native Windows compiler; build + round-trip test via **WSL** (`gcc 13.3`, Ubuntu 24.04 — CSE-like). User intends to run Claude Code from inside WSL for native debugging (then no `wsl -e bash -lc` wrapper needed; repo path is `/mnt/c/Users/A/Desktop/UNSW/9319`). Note WSL2 9p `/mnt/c` can show stale dir listings right after a Windows-side mkdir.

**⚠️ Reference reliability finding:** the so-called ⭐main reference `VegasFlower-2023T2` **does NOT round-trip** — compiled with gcc, `lencode` turns `^WED^WE^WEE^WEB^WET` into 10 literal bytes (`^WEDEEEBET`), i.e. lossy, no index emitted. Do **not** port it; implement `lencode`/`ldecode` from the algorithm and verify every step by `原文→lencode→ldecode→原文` byte-diff (small spec example + a >32768-entry file to exercise reset).

**Permissions:** shared allow-list committed at `.claude/settings.json` (wsl/gcc/g++/diff/git…); machine-local extras stay in gitignored `.claude/settings.local.json`. Fully-unattended runs still need the user to set Claude Code's permission mode to "bypass permissions" (Shift+Tab) — Claude cannot set that itself.

In-repo artifacts — everything A1 now lives under `assignment1/` (consolidated 2026-06-17):
- `assignment1/lencode.c`, `assignment1/ldecode.c` — the solution.
- `assignment1/README.md` — solution design/build/submit notes.
- `assignment1/spec.md` — full spec writeup + repo comparison (was top-level `COMP9319-2026T2-Assignment1.md`).
- `assignment1/notes.md` — LZW algorithm notes, study path, self-check list (was top-level `LEARNING-NOTES.md`).
- `assignment1/reference/` — cloned ass1 dirs (study only; already in MOSS/Turnitin): `VegasFlower-2023T2/` is the ⭐main reference (15-bit/2-byte/≥3-char format **identical** to this term, but lacks dictionary reset); `SongyuQi-2025T3/` and `dioxyq-25T3/` are 22-bit **variable-length** (format INCOMPATIBLE with this term) kept only for reset-logic ideas, autotest/automark scripts, and reusable `.txt` test inputs. Their `.lzw`/`.bin` expected outputs are NOT valid to diff against a 2026 encoder; verify via round-trip on `.txt` instead.

When the solution starts, follow [[solution-file-convention]] and place it per the per-deliverable folder rule in [[comp9319-repo]].
