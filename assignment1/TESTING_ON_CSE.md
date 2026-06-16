# COMP9319 A1 Testing Guide on CSE

This guide is for testing your `lencode`/`ldecode` on UNSW CSE Linux before final submission.

## 1) Login and go to your folder

```bash
ssh zID@login.cse.unsw.edu.au
cd /path/to/your/assignment1
```

## 2) Build binaries

For C:

```bash
gcc -O2 -Wall -Wextra -std=c11 -o lencode lencode.c
gcc -O2 -Wall -Wextra -std=c11 -o ldecode ldecode.c
```

For C++ (if you submit `.cpp`):

```bash
g++ -O2 -Wall -Wextra -std=c++17 -o lencode lencode.cpp
g++ -O2 -Wall -Wextra -std=c++17 -o ldecode ldecode.cpp
```

## 3) Run official sanity test

```bash
~cs9319/a1/autotest
```

This is the minimum check you should pass before submitting.

## 4) Run round-trip checks (must-pass)

The assignment's core correctness requirement is:

`source -> lencode -> ldecode -> source` (byte-identical)

Example:

```bash
./lencode ~cs9319/a1/test1.txt test1.enc
./ldecode test1.enc test1.dec
diff ~cs9319/a1/test1.txt test1.dec
```

No `diff` output means pass.

## 5) Validate encoding format quickly

```bash
xxd -b test1.enc | head
```

You should see:

- regular ASCII bytes with MSB 0
- dictionary index tokens as 2 bytes with MSB 1 on the high byte

## 6) Performance and larger inputs

Each single test has a 5-second limit in marking.

So test at least:

- repetitive pattern files
- random 7-bit ASCII files
- larger files close to 1MB

This helps catch slow dictionary lookup and reset bugs.

## 7) Use the provided one-shot script

This repo includes `extra_tests/cse_verify.sh`, which automates:

- compile
- official sanity test (`~cs9319/a1/autotest`, if available)
- round-trip checks on provided and generated files
- per-case runtime checks against a 5s threshold

Run:

```bash
chmod +x extra_tests/cse_verify.sh
./extra_tests/cse_verify.sh
```

## 8) Final submission check

```bash
9319 classrun -check a1
```

Then submit:

```bash
give cs9319 a1 lencode.c ldecode.c
```

or:

```bash
give cs9319 a1 lencode.cpp ldecode.cpp
```
