#!/usr/bin/env bash
set -euo pipefail

# One-shot verifier for COMP9319 A1 on CSE Linux.
# It compiles, runs official autotest (if present), then performs
# round-trip + basic runtime checks on multiple generated inputs.

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
TMP_DIR="$(mktemp -d)"
TIME_LIMIT_SECS=5

cleanup() {
  rm -rf "$TMP_DIR"
}
trap cleanup EXIT

info() {
  printf '[INFO] %s\n' "$*"
}

pass() {
  printf '[PASS] %s\n' "$*"
}

fail() {
  printf '[FAIL] %s\n' "$*" >&2
  exit 1
}

build_binaries() {
  info "Building lencode/ldecode from C sources"
  gcc -O2 -Wall -Wextra -std=c11 -o "$ROOT_DIR/lencode" "$ROOT_DIR/lencode.c"
  gcc -O2 -Wall -Wextra -std=c11 -o "$ROOT_DIR/ldecode" "$ROOT_DIR/ldecode.c"
  pass "Build succeeded"
}

run_official_autotest_if_available() {
  if [[ -x /home/cs9319/a1/autotest ]]; then
    info "Running official autotest: ~cs9319/a1/autotest"
    (
      cd "$ROOT_DIR"
      /home/cs9319/a1/autotest
    )
    pass "Official autotest passed"
  else
    info "Official autotest not found on this machine, skipping"
  fi
}

measure_command_runtime() {
  local start end elapsed_ns
  start="$(date +%s%N)"
  "$@"
  end="$(date +%s%N)"
  elapsed_ns=$((end - start))
  awk -v ns="$elapsed_ns" 'BEGIN { printf "%.3f", ns / 1000000000 }'
}

assert_under_time_limit() {
  local seconds="$1"
  local label="$2"
  awk -v t="$seconds" -v limit="$TIME_LIMIT_SECS" 'BEGIN { exit !(t <= limit) }' \
    || fail "$label exceeded ${TIME_LIMIT_SECS}s (got ${seconds}s)"
}

run_roundtrip_case() {
  local case_name="$1"
  local input_file="$2"

  local enc_file="${TMP_DIR}/${case_name}.enc"
  local dec_file="${TMP_DIR}/${case_name}.dec"

  info "Case: ${case_name}"

  local enc_time dec_time
  enc_time="$(measure_command_runtime "$ROOT_DIR/lencode" "$input_file" "$enc_file")"
  dec_time="$(measure_command_runtime "$ROOT_DIR/ldecode" "$enc_file" "$dec_file")"

  cmp -s "$input_file" "$dec_file" || fail "Round-trip mismatch for ${case_name}"
  assert_under_time_limit "$enc_time" "Encode ${case_name}"
  assert_under_time_limit "$dec_time" "Decode ${case_name}"

  local in_size enc_size
  in_size="$(wc -c < "$input_file" | tr -d ' ')"
  enc_size="$(wc -c < "$enc_file" | tr -d ' ')"
  pass "${case_name}: round-trip ok, encode=${enc_time}s decode=${dec_time}s, in=${in_size}B enc=${enc_size}B"
}

generate_cases() {
  mkdir -p "$TMP_DIR/cases"

  # Empty file
  : > "$TMP_DIR/cases/empty.bin"

  # Small fixed examples
  printf 'A' > "$TMP_DIR/cases/single.bin"
  printf '^WED^WE^WEE^WEB^WET' > "$TMP_DIR/cases/pattern.bin"

  # Near-1MB repetitive data
  python3 - <<'PY' > "$TMP_DIR/cases/repeat_1m.bin"
pat = (b"ABCD1234" * 131072)[:1000000]
import sys
sys.stdout.buffer.write(pat)
PY

  # Random 7-bit ASCII
  python3 - <<'PY' > "$TMP_DIR/cases/random_700k.bin"
import random, sys
rng = random.Random(9319)
data = bytes(rng.randint(0, 127) for _ in range(700000))
sys.stdout.buffer.write(data)
PY
}

run_reference_case_if_available() {
  if [[ -f /home/cs9319/a1/test1.txt ]]; then
    run_roundtrip_case "official_test1" "/home/cs9319/a1/test1.txt"
  else
    info "Reference file ~cs9319/a1/test1.txt not found, skipping that case"
  fi
}

main() {
  info "Workspace: $ROOT_DIR"
  build_binaries
  run_official_autotest_if_available

  generate_cases
  run_reference_case_if_available
  run_roundtrip_case "empty" "$TMP_DIR/cases/empty.bin"
  run_roundtrip_case "single" "$TMP_DIR/cases/single.bin"
  run_roundtrip_case "pattern" "$TMP_DIR/cases/pattern.bin"
  run_roundtrip_case "repeat_1m" "$TMP_DIR/cases/repeat_1m.bin"
  run_roundtrip_case "random_700k" "$TMP_DIR/cases/random_700k.bin"

  pass "All checks passed"
}

main "$@"
