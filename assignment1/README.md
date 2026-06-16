# COMP9319 2026T2 — Assignment 1 (LZW)

Implementation of the 15-bit LZW encoder/decoder.

## Build

```bash
gcc -O2 -o lencode lencode.c
gcc -O2 -o ldecode ldecode.c
```

## Run

```bash
lencode <input_file> <output_file>
ldecode <input_file> <output_file>
```

`ldecode(lencode(x)) == x` byte-for-byte for any 7-bit ASCII input.

## Submit

```bash
give cs9319 a1 lencode.c ldecode.c
```

## Design notes

- **Dictionary**: a 15-bit / 32,768-entry trie. Single ASCII characters are
  *not* stored as dictionary entries — they never consume index space — so the
  32,768 indices are used only for multi-character phrases. Lookups use a
  phrase hash table keyed by `(parent_node << 7) | char`, with O(1) reset via an
  epoch counter (no array clearing).
- **Output format**: an ASCII char is one byte (MSB = 0); a dictionary index is
  two bytes, high byte first, with the high byte's MSB set as the flag —
  `byte0 = 0x80 | (idx >> 8)`, `byte1 = idx & 0xFF`. A matched phrase is written
  as a 2-byte index only when it holds **≥ 3 characters**; phrases of length 1
  or 2 are written as their literal bytes (a 2-byte index would not be smaller).
- **Reset when full**: when all 32,768 indices are used, the dictionary is reset
  *after* the current phrase has been emitted and rebuilt from scratch, exactly
  as at the start of the input.
- **Decoder synchronisation (the subtle part)**: because length-2 phrases are
  emitted as two literal bytes rather than an index, the naive "one token = one
  LZW code" decoder loses phrase boundaries and its dictionary numbering drifts
  out of step with the encoder (this is why several public reference
  implementations fail to round-trip once an index is reused). Instead, the
  decoder recovers the original characters from the token stream and feeds every
  character back through the **encoder's own automaton** (`feed_char`). This
  reproduces the encoder's dictionary — same indices, same reset timing —
  character for character, so index tokens always resolve correctly. The classic
  LZW *KwKwK* case (an index referring to the entry the encoder is about to
  create) is handled as `(current phrase) + (its first character)`.

Verified by round-trip testing on the spec example, edge cases (empty / 1 / 2
bytes), randomised 7-bit inputs, and large low-entropy inputs that trigger many
dictionary resets and the KwKwK case. Encodes/decodes a 1 MB file in ~0.01 s.
