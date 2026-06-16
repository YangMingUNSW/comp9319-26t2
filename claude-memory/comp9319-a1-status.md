---
name: comp9319-a1-status
description: COMP9319 2026T2 Assignment 1 (LZW) — current phase, spec, and reference materials
metadata:
  node_type: memory
  type: project
---

COMP9319 **2026T2 Assignment 1** = implement `lencode`/`ldecode` in C/C++ doing **LZW** with a **15-bit / 32,768-entry** dictionary. Key rules: ASCII = 1 byte (MSB 0), dict index = 2 bytes (MSB 1, high byte first), an index is only output when its entry holds **≥3 chars**, and the dictionary is **reset when full** (this reset-when-full requirement is the ONLY substantive change vs the near-identical 2023T2 version). Deadline **Tue 30 June 2026 5:00pm AEST**; auto-marked /15; `give cs9319 a1`; 5s & ≤1MB per test; `~cs9319/a1/autotest`.

**Current phase (as of 2026-06-16): study/reference-gathering only — the solution is NOT started.** Do not write `lencode`/`ldecode` until the user explicitly says to begin.

In-repo artifacts:
- `COMP9319-2026T2-Assignment1.md` — full spec writeup + repo comparison.
- `LEARNING-NOTES.md` — LZW algorithm notes, study path, self-check list.
- `reference/` — cloned ass1 dirs (study only; already in MOSS/Turnitin): `VegasFlower-2023T2/` is the ⭐main reference (15-bit/2-byte/≥3-char format **identical** to this term, but lacks dictionary reset); `SongyuQi-2025T3/` and `dioxyq-25T3/` are 22-bit **variable-length** (format INCOMPATIBLE with this term) kept only for reset-logic ideas, autotest/automark scripts, and reusable `.txt` test inputs. Their `.lzw`/`.bin` expected outputs are NOT valid to diff against a 2026 encoder; verify via round-trip on `.txt` instead.

When the solution starts, follow [[solution-file-convention]] and place it per the per-deliverable folder rule in [[comp9319-repo]].
