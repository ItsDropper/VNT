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
printf 'Ada\n1\n1\n3\n2\n1\n3\n5\n' | "$VNX" game/last_signal.vnx > "$TMP/game"
grep -F 'THE LAST SIGNAL' "$TMP/game"
grep -F 'ENDING: HOMEWARD.' "$TMP/game"
grep -F 'Thank you for playing THE LAST SIGNAL, Ada.' "$TMP/game"
echo "VNX smoke tests passed."
