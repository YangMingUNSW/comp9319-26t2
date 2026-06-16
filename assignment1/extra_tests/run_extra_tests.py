#!/usr/bin/env python3
"""Additional tests beyond COMP9319 A1 spec examples.

Usage:
  python3 extra_tests/run_extra_tests.py
"""

from __future__ import annotations

import random
import subprocess
import tempfile
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
ENCODER = ROOT / "lencode"
DECODER = ROOT / "ldecode"


def run_roundtrip(name: str, data: bytes) -> tuple[bool, str]:
    """Encode then decode, and verify byte-perfect restoration."""
    with tempfile.TemporaryDirectory() as td:
        tdir = Path(td)
        in_file = tdir / "input.bin"
        enc_file = tdir / "encoded.lzw"
        out_file = tdir / "decoded.bin"

        in_file.write_bytes(data)

        e = subprocess.run([str(ENCODER), str(in_file), str(enc_file)], capture_output=True)
        if e.returncode != 0:
            return False, f"encode failed ({e.returncode})"

        d = subprocess.run([str(DECODER), str(enc_file), str(out_file)], capture_output=True)
        if d.returncode != 0:
            return False, f"decode failed ({d.returncode})"

        restored = out_file.read_bytes()
        if restored != data:
            return False, f"roundtrip mismatch (input={len(data)} output={len(restored)})"

        return True, f"ok (len={len(data)}, enc={enc_file.stat().st_size})"


def main() -> int:
    if not ENCODER.exists() or not DECODER.exists():
        print("missing ./lencode or ./ldecode; build first:")
        print("  gcc -O2 -o lencode lencode.c && gcc -O2 -o ldecode ldecode.c")
        return 1

    rng = random.Random(9319)
    tests: list[tuple[str, bytes]] = [
        ("empty-file", b""),
        ("single-byte", b"A"),
        ("two-bytes", b"AB"),
        ("alternating-pattern", (b"AB" * 250_000)[:1_000_000]),
        ("ascending-7bit", bytes((i % 128 for i in range(900_000)))),
        ("all-7bit-once", bytes(range(128))),
        ("nul-and-controls", bytes([0, 1, 2, 3, 9, 10, 13, 31, 32, 65, 127]) * 7000),
        ("random-7bit-700k", bytes(rng.randint(0, 127) for _ in range(700_000))),
    ]

    failed = 0
    for name, payload in tests:
        ok, detail = run_roundtrip(name, payload)
        status = "PASS" if ok else "FAIL"
        print(f"{status:4} {name:20} {detail}")
        if not ok:
            failed += 1

    print(f"\nSummary: {len(tests) - failed}/{len(tests)} passed")
    return 0 if failed == 0 else 2


if __name__ == "__main__":
    raise SystemExit(main())
