#!/bin/sh
set -eu

VNX="${1:?usage: smoke.sh /path/to/vnx-interpreter}"
TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT HUP INT TERM

run_game() {
    name="$1"
    input="$2"
    printf '%b' "$input" | "$VNX" > "$TMP/$name.out"
}

# The interpreter reads the source path from stdin; it does not accept it as argv.
printf 'vnx/tests/smoke.vnx\n' | "$VNX" > "$TMP/smoke.out"
grep -F 'VNX_SMOKE_OK' "$TMP/smoke.out"
grep -F 'VNX_STATE_OK' "$TMP/smoke.out"
if grep -F 'VNX_SMOKE_FAILED' "$TMP/smoke.out"; then
    echo "language smoke program reached its failure branch" >&2
    exit 1
fi

# Every intentional ending.
run_game homeward 'vnx/game/last_signal.vnx\nAda\n1\n1\n3\n2\n1\n3\n5\n'
grep -F 'ENDING: HOMEWARD.' "$TMP/homeward.out"

run_game containment 'vnx/game/last_signal.vnx\nAda\n2\n2\n'
grep -F 'ENDING: CONTAINMENT.' "$TMP/containment.out"

run_game signal 'vnx/game/last_signal.vnx\nAda\n1\n1\n3\n2\n1\n3\n3\n1\n2\nyes\n3\n4\n'
grep -F 'ENDING: THE SIGNAL.' "$TMP/signal.out"

# Reach generator, terminal, and airlock before their prerequisites: these must
# show an in-game response rather than crash on an uninitialized variable.
run_game generator_without_key 'vnx/game/last_signal.vnx\nAda\n2\n1\n3\n6\n'
grep -F 'ACCESS DENIED' "$TMP/generator_without_key.out"
grep -F 'You abandon the station.' "$TMP/generator_without_key.out"

run_game terminal_without_power 'vnx/game/last_signal.vnx\nAda\n4\n6\n'
grep -F 'LOG: Emergency grid disabled from inside.' "$TMP/terminal_without_power.out"
grep -F 'You abandon the station.' "$TMP/terminal_without_power.out"

run_game airlock_without_power 'vnx/game/last_signal.vnx\nAda\n5\n6\n'
grep -F 'The airlock is dead.' "$TMP/airlock_without_power.out"
grep -F 'You abandon the station.' "$TMP/airlock_without_power.out"

# Explore the monitor and decline the AI, then return safely to the hub.
run_game monitor 'vnx/game/last_signal.vnx\nAda\n1\n2\n3\n6\n'
grep -F 'The image smiles.' "$TMP/monitor.out"
grep -F 'You abandon the station.' "$TMP/monitor.out"

run_game decline_ai 'vnx/game/last_signal.vnx\nAda\n3\n2\nno\n3\n6\n'
grep -F 'It whispers without power.' "$TMP/decline_ai.out"
grep -F 'You abandon the station.' "$TMP/decline_ai.out"

# With power and a sample but without trusting the AI, the terminal must not
# take the signal ending.
run_game sample_without_trust 'vnx/game/last_signal.vnx\nAda\n1\n1\n3\n2\n1\n3\n3\n1\n3\n4\n6\n'
grep -F 'The AI asks for the sample.' "$TMP/sample_without_trust.out"
if grep -F 'ENDING: THE SIGNAL.' "$TMP/sample_without_trust.out"; then
    echo "signal ending should require trusting the AI" >&2
    exit 1
fi

# Parser errors, unknown variables, and unknown labels must return non-zero.
if printf 'vnx/tests/invalid.vnx\n' | "$VNX" > "$TMP/invalid.out" 2>&1; then
    echo "malformed VNX source unexpectedly succeeded" >&2
    exit 1
fi
grep -F 'VNX parser error' "$TMP/invalid.out"

cat > "$TMP/unknown-variable.vnx" <<'VNX'
say missing
end
VNX
if printf '%s\n' "$TMP/unknown-variable.vnx" | "$VNX" > "$TMP/unknown-variable.out" 2>&1; then
    echo "unknown variable unexpectedly succeeded" >&2
    exit 1
fi
grep -F 'unknown variable missing' "$TMP/unknown-variable.out"

cat > "$TMP/unknown-label.vnx" <<'VNX'
jump nowhere
end
VNX
if printf '%s\n' "$TMP/unknown-label.vnx" | "$VNX" > "$TMP/unknown-label.out" 2>&1; then
    echo "unknown label unexpectedly succeeded" >&2
    exit 1
fi
grep -F 'unknown label nowhere' "$TMP/unknown-label.out"

if printf 'does-not-exist.vnx\n' | "$VNX" > "$TMP/missing-file.out" 2>&1; then
    echo "missing source file unexpectedly succeeded" >&2
    exit 1
fi
grep -F 'could not read source file' "$TMP/missing-file.out"

echo "VNX smoke tests and adventure route checks passed."
