#!/bin/sh
set -eu
VNX="$1"
TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT
cat > "$TMP/basics.vnx" <<'VNX'
let n = 1 + 2 * 3;
print("math=" + n);
let name = "V" + "NX";
print(name);
let i = 0;
while (i < 3) { print(i); i = i + 1; }
if (n == 7 && name == "VNX") { print("control-flow-ok"); }
else { print("control-flow-failed"); }
VNX
"$VNX" "$TMP/basics.vnx" > "$TMP/out"
grep -F 'math=7' "$TMP/out"
grep -F 'VNX' "$TMP/out"
grep -F 'control-flow-ok' "$TMP/out"
grep -F '2' "$TMP/out"

printf 'Ada\n1\n1\n3\n2\n1\n3\n5\n' | "$VNX" game/last_signal.vnx > "$TMP/homeward"
grep -F 'ENDING: HOMEWARD.' "$TMP/homeward"

printf 'Ada\n2\n2\n' | "$VNX" game/last_signal.vnx > "$TMP/containment"
grep -F 'ENDING: CONTAINMENT.' "$TMP/containment"

printf 'Ada\n1\n1\n3\n2\n1\n3\n3\n1\n3\n3\n2\nyes\nyes\n' | "$VNX" game/last_signal.vnx > "$TMP/signal"
grep -F 'ENDING: THE SIGNAL.' "$TMP/signal"

cat > "$TMP/unknown.vnx" <<'VNX'
print(missing);
VNX
if "$VNX" "$TMP/unknown.vnx" > /dev/null 2>&1; then
  echo "unknown variable should fail" >&2
  exit 1
fi
cat > "$TMP/divzero.vnx" <<'VNX'
print(10 / 0);
VNX
if "$VNX" "$TMP/divzero.vnx" > /dev/null 2>&1; then
  echo "division by zero should fail" >&2
  exit 1
fi
echo "VNX smoke tests passed."
