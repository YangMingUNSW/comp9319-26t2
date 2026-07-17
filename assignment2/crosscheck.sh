#!/bin/bash
#
# crosscheck.sh - diff ./bwtsearch (on .rbwt) against ~cs9319/a2/dsearch (on .txt).
#
# Run on db-perftest from the directory holding a built ./bwtsearch:
#     ./crosscheck.sh                       # every .rbwt in ~cs9319/a2 with a .txt pair
#     ./crosscheck.sh ~cs9319/a2/dna-small.rbwt
#
# Terms are drawn from the START, MIDDLE and END of each sequence at a range of
# lengths, plus self-overlapping ones.  The END terms are the point of this
# script: whether context extraction at the '\n' sequence boundary matches the
# reference is the one behaviour local testing cannot settle (spec.md section 7).
#
# This script only reads the sample files and writes nothing outside /tmp; it is
# a test aid and is never submitted (submission is: makefile *.c *.cpp *.h).

set -u

A2="${A2:-$(eval echo ~cs9319/a2)}"
DSEARCH="$A2/dsearch"
BWTSEARCH="${BWTSEARCH:-./bwtsearch}"
TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT

[ -x "$BWTSEARCH" ] || { echo "no executable $BWTSEARCH - run make first"; exit 1; }
[ -x "$DSEARCH" ]   || { echo "no executable $DSEARCH"; exit 1; }

# Byte range [off, off+len) of a file, without a bs=1 dd crawl.
substr() { tail -c "+$(( $2 + 1 ))" "$1" | head -c "$3"; }

pass=0
fail=0

check() {   # check <rbwt> <txt> <term> <label>
    local rbwt="$1" txt="$2" term="$3" label="$4"
    "$BWTSEARCH" "$rbwt" "$term" 2>"$TMP/err" | sort > "$TMP/mine"
    "$DSEARCH"   "$txt"  "$term"              | sort > "$TMP/ref"
    if [ -s "$TMP/err" ]; then
        echo "FAIL [$label] len=${#term} - wrote to stderr: $(head -c 100 "$TMP/err")"
        fail=$(( fail + 1 ))
        return
    fi
    if cmp -s "$TMP/mine" "$TMP/ref"; then
        pass=$(( pass + 1 ))
    else
        fail=$(( fail + 1 ))
        echo "FAIL [$label] len=${#term} term=$(echo "$term" | head -c 40)"
        echo "     mine=$(wc -l < "$TMP/mine") lines, ref=$(wc -l < "$TMP/ref") lines"
        diff "$TMP/ref" "$TMP/mine" | head -6 | sed 's/^/     /'
    fi
}

rbwts=( "$@" )
if [ ${#rbwts[@]} -eq 0 ]; then
    rbwts=( "$A2"/*.rbwt )
fi

for rbwt in "${rbwts[@]}"; do
    txt="${rbwt%.rbwt}.txt"
    [ -f "$txt" ] || { echo "skip $(basename "$rbwt") - no matching .txt"; continue; }

    # Body length = file size minus the trailing '\n'.
    # -L follows symlinks: the sample files may well be links, and sizing the
    # link instead of its target silently defeats the length guard below.
    n=$(( $(stat -Lc%s "$txt") - 1 ))

    # The spec guarantees no test case yields more than 5000 matches.  A 1- or
    # 2-char term on a multi-megabyte sequence yields millions: unrepresentative,
    # slow, and it makes bwtsearch's per-match buffers balloon.  So skip lengths
    # whose rough expected match count (n / 4^L) is far past that.
    minlen=1; expected=$n
    while [ "$expected" -gt 200000 ] && [ "$minlen" -lt 12 ]; do
        expected=$(( expected / 4 )); minlen=$(( minlen + 1 ))
    done
    echo "=== $(basename "$rbwt")  (body ${n} chars, term length >= ${minlen}) ==="

    for L in 1 2 3 5 8 12 30 128; do
        [ "$L" -le "$n" ] && [ "$L" -ge "$minlen" ] || continue
        check "$rbwt" "$txt" "$(substr "$txt" 0 "$L")"            start
        check "$rbwt" "$txt" "$(substr "$txt" $(( n / 2 )) "$L")"  middle
        # Term ending on the LAST body char - exercises the '\n' boundary.
        check "$rbwt" "$txt" "$(substr "$txt" $(( n - L )) "$L")"  end
    done

    # Self-overlapping terms: spec requires overlapping matches all be reported.
    for term in AA AAA ACAC GTGT TTTT; do
        [ "${#term}" -ge "$minlen" ] || continue
        grep -q "$term" "$txt" && check "$rbwt" "$txt" "$term" overlap
    done
done

echo
echo "pass=$pass fail=$fail"
[ "$fail" -eq 0 ]
