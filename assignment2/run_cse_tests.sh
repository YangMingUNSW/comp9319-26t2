#!/bin/bash
#
# run_cse_tests.sh - one-shot A2 test run on CSE db-perftest.
#
# Produces ONE labelled transcript covering every check in TESTING_ON_CSE.md, so
# the whole picture can be reviewed from a single paste.
#
#   ./run_cse_tests.sh
#       -> prints the transcript and saves it to cse-test/<commit>/transcript.txt
#
# Every step is independent and non-fatal: a failing step is recorded with its
# exit code and the run continues.  Nothing here is submitted (submission is
# `give cs9319 a2 makefile *.c *.cpp *.h`).
#
# Env overrides:  A2=<dir>  (default ~cs9319/a2)

set -u

A2="${A2:-$(eval echo ~cs9319/a2)}"
LIMIT_BYTES=16777216          # spec: total runtime memory must stay under 16MB
STAMP="$(git rev-parse --short HEAD 2>/dev/null || echo nogit)"
OUT_DIR="cse-test/$STAMP"
mkdir -p "$OUT_DIR"
TRANSCRIPT="$OUT_DIR/transcript.txt"

section() { echo; echo "==================== $* ===================="; }
run() { echo "\$ $*"; "$@" 2>&1; echo "[exit=$?]"; }

main() {

section "0. ENVIRONMENT"
echo "date      : $(date)"
echo "host      : $(hostname)"
echo "commit    : $STAMP"
echo "A2 dir    : $A2"
run uname -a
run gcc --version
echo "--- is this db-perftest? (marking happens there; vlab timings do not count)"
case "$(hostname)" in
    *perftest*) echo "OK: on db-perftest" ;;
    *) echo "WARNING: hostname does not look like db-perftest" ;;
esac

section "1. OFFICIAL TEST MATERIAL IN $A2"
# This is what settles the spec's open questions: the sample makefile's expected
# targets and how autotest invokes the program.
run ls -lL "$A2"
echo "--- sample makefile (spec says to base ours on it):"
for f in "$A2"/makefile "$A2"/Makefile; do
    [ -f "$f" ] && { echo "--- $f"; cat "$f"; }
done
echo "--- autotest (how the marker actually invokes the programs):"
[ -f "$A2/autotest" ] && head -60 "$A2/autotest"
echo "--- .rbwt files by size (largest is the one that matters for mem/time):"
run ls -lLS "$A2"/*.rbwt

BIG="$(ls -LS "$A2"/*.rbwt 2>/dev/null | head -1)"
SMALL="$A2/dna-small.rbwt"
[ -f "$SMALL" ] || SMALL="$BIG"
echo "largest .rbwt = $BIG"

section "2. COMPILE (red line: any compile error = 0 for the whole assignment)"
run make clean
run make
echo "--- the executable must exist:"
run ls -l bwtsearch

section "3. OFFICIAL autotest (formatting/argument sanity on dna-tiny)"
if [ -x "$A2/autotest" ]; then
    run "$A2/autotest"
else
    echo "SKIP: $A2/autotest not found or not executable"
fi

section "4. OFFICIAL dsearch CROSS-CHECK (the decisive correctness test)"
# The 'end' group is the point: it is the only check that can settle whether our
# '\n' sequence-boundary handling matches the reference.
if [ -x "./crosscheck.sh" ]; then
    run ./crosscheck.sh
else
    echo "SKIP: ./crosscheck.sh missing"
fi

section "5. MEMORY / massif (red line: >=16MB = 0 for those tests)"
# The spec's own command form: ./bwtsearch ~/a2/dna-small.rbwt ACTG
for target in "$SMALL" "$BIG"; do
    [ -f "$target" ] || continue
    [ "$target" = "$BIG" ] && [ "$SMALL" = "$BIG" ] && continue   # same file, ran it already
    echo "--- massif on $(basename "$target") ($(stat -Lc%s "$target") bytes)"
    valgrind --tool=massif --pages-as-heap=yes --massif-out-file="$OUT_DIR/memory.out" \
        ./bwtsearch "$target" ACGTACGT > /dev/null 2>"$OUT_DIR/massif.err"
    echo "[valgrind exit=$?]"; head -3 "$OUT_DIR/massif.err"
    peak=$(grep '^mem_heap_B=' "$OUT_DIR/memory.out" 2>/dev/null | cut -d= -f2 | sort -n | tail -1)
    if [ -n "${peak:-}" ]; then
        echo "PEAK = $peak bytes ($(( peak / 1024 ))KB) | limit $LIMIT_BYTES | \
$( [ "$peak" -lt "$LIMIT_BYTES" ] && echo PASS || echo FAIL )"
    else
        echo "could not read peak from $OUT_DIR/memory.out"
    fi
    ms_print "$OUT_DIR/memory.out" > "$OUT_DIR/memory-$(basename "$target").txt" 2>&1
done

section "6. TIME (red line: >5s user+system per search = 0 for that test)"
if [ -f "$BIG" ]; then
    echo "--- largest file, several term lengths (user+system is what is marked):"
    for term in A ACGTACGT ACGTACGTACGTACGT; do
        echo "term length ${#term}:"
        /usr/bin/time -f "  user=%U sys=%S elapsed=%e maxRSS=%MkB" \
            ./bwtsearch "$BIG" "$term" > /dev/null
    done
    echo "NOTE: a 1-char term far exceeds the spec's <=5000-match guarantee, so a"
    echo "      slow/heavy result there is out of scope - judge by the longer terms."
fi

section "7. NO FILE WRITES (red line: writing any file = 0 for the whole assignment)"
if command -v strace > /dev/null; then
    echo "--- any open for write / create? (expect none)"
    strace -f -e trace=openat,open,creat,unlink,rename ./bwtsearch "$SMALL" ACGTACGT 2>&1 > /dev/null \
        | grep -iE "O_WRONLY|O_RDWR|O_CREAT|creat\(|unlink|rename" \
        | grep -ivE "\.so|/etc/|/proc/|/usr/lib|/sys/" | head -10
    echo "[end of write-syscall list - empty above means PASS]"
else
    echo "strace unavailable; falling back to a directory diff:"
    before=$(ls -a); ./bwtsearch "$SMALL" ACGTACGT > /dev/null; after=$(ls -a)
    [ "$before" = "$after" ] && echo "PASS: no files created in cwd" || { echo "FAIL: cwd changed:"; diff <(echo "$before") <(echo "$after"); }
fi

section "8. SUMMARY"
echo "Transcript saved to: $TRANSCRIPT"
echo "Paste the WHOLE transcript back for review."
echo "Reminder: run 'give'/'classrun' on a NORMAL CSE machine, not db-perftest."

}

main 2>&1 | tee "$TRANSCRIPT"
