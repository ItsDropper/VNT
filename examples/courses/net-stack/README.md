# VNT Offline Network Stack Laboratory

Companion to the [20-lesson network systems course](../../../docs/tutorials/courses/net-stack/README.md). This is a multi-module protocol lab, not a one-file packet parser. It builds synthetic Ethernet frames, parses and validates IPv4 and UDP, generates a response through the simulated stack, and includes separate exercises for ARP, ICMP, routing, and TCP state transitions.

## Project map

- `src/bytes.vnt` — checked byte reads/writes, big-endian fields, slices, and byte concatenation.
- `src/checksum.vnt` — Internet checksum and checksum verification.
- `src/ethernet.vnt` — Ethernet II parser and builder.
- `src/arp.vnt` — Ethernet/IPv4 ARP parser and builder.
- `src/ipv4.vnt` — IPv4 header validation, checksum checks, and packet construction.
- `src/icmp.vnt` — ICMPv4 echo parsing and reply construction.
- `src/udp.vnt` — UDP datagram parser, payload extraction, and builder.
- `src/routing.vnt` — longest-prefix route lookup over a deterministic in-memory table.
- `src/tcp_state.vnt` — deliberately limited TCP state-transition model.
- `src/simulator.vnt` — in-process Ethernet → IPv4 → UDP → echo path.
- `src/main.vnt` — constructs a synthetic request and validates the response.
- `fixtures/README.md` — fixture format and malformed-input conventions.
- `tests/smoke.vnt` — parser boundary tests.
- `tests/routing.vnt` — longest-prefix matching tests.
- `tests/state-machine.vnt` — valid and invalid TCP state transition tests.

## Build and run with VBuild

VBuild provides repeatable build, run, and test tasks for the simulator. It is a native C program and does not need Python. Build it once from the repository root:

```powershell
gcc -std=c11 -O2 tools\vbuild.c -o vbuild.exe
```

On Linux or WSL, use `gcc -std=c11 -O2 tools/vbuild.c -o vbuild`.

This example already contains its source files, so do **not** run `vbuild init` inside it: init deliberately refuses to overwrite existing project files. Create the following files in `examples/courses/net-stack/`.

`vbuild.properties`:

```properties
project.name = net-stack
project.version = 0.1.0
source = src/main.vnt
output = net-stack
compiler = vnt
toolchain = ../../..
```

Here, `toolchain = ../../..` points back to the VNT repository root from this example directory. If you copy the project elsewhere, change `toolchain` to your actual VNT checkout path or set `VNT_HOME`.

`build.vbuild`:

```text
target exe
task build = build
task run = run depends build
task check = check
task test = test
task clean = clean
task verify = none depends check, build, test
```

Run from `examples/courses/net-stack/`:

```powershell
..\..\..\vbuild.exe tasks
..\..\..\vbuild.exe build
..\..\..\vbuild.exe run
..\..\..\vbuild.exe test
..\..\..\vbuild.exe verify
```

On Linux or WSL, replace `..\..\..\vbuild.exe` with `../../../vbuild`. The executable is placed under `build/`. VBuild's test discovery is non-recursive and finds the existing `tests/*.vnt` programs. For VBuild to detect a failing test, the test must exit non-zero; a printed `FAIL` message alone is not sufficient. Keep explicit PASS/FAIL output for humans, and make assertions affect the process exit status where supported.

VBuild fingerprints the root source, imported modules, project properties, task file, and relevant compiler/runtime inputs. Use `..\..\..\vbuild.exe build --force` after changes to system libraries or environment variables that are not included in its fingerprint.

## Direct compiler fallback

Direct compilation is useful when you need to isolate a compiler error from a build-driver error. From the repository root:

```powershell
.\vnt.exe --compile examples\courses\net-stack\src\main.vnt -o net-stack.exe
.\net-stack.exe
```

## Data flow

1. The Ethernet parser validates the 14-byte header and exposes EtherType and MAC addresses.
2. The IPv4 parser validates version, IHL, total length, and header checksum before exposing fields.
3. The UDP parser validates its minimum header and declared length.
4. The simulator dispatches only IPv4 protocol 17 and destination UDP port 7.
5. It constructs a UDP echo reply, then an IPv4 packet, then an Ethernet frame with addresses reversed.
6. The entry point reparses the generated response and compares its payload against the original message.

Every layer receives a byte array, checks its own format, and returns an empty array on invalid or unsupported input. This simple error convention is intentionally visible for teaching; a production parser should use a richer tagged error result so empty valid payloads and parse failure cannot be confused.

## Important limits

- Everything is synthetic and offline. No raw sockets, packet capture, or packet injection are used.
- The UDP builder uses the IPv4-permitted zero UDP-checksum option; it does not calculate the UDP pseudoheader checksum.
- The IPv4 builder creates a simple unfragmented 20-byte header and does not implement options.
- The TCP module teaches legal state transitions only. It does not implement sequence-window validation, retransmission, congestion control, or real TCP connections.
- Routing is a lookup exercise, not a forwarding plane.
- ARP and ICMP modules are independently testable components and are not yet wired into the main UDP echo path.
- IPv6, Ethernet VLAN tags, production reassembly, timers, and live networking are not implemented.

The project is intentionally broad enough to teach real protocol boundaries while being honest about which parts are implemented. Extend one module at a time and add valid, truncated, malformed, and unsupported fixtures for each feature.
