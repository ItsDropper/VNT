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

## Build and run

From the repository root, build the compiler according to the root README. Compile the integrated simulator:

```powershell
.\\vnt.exe --compile examples\\courses\\net-stack\\src\\main.vnt -o net-stack.exe
.\\net-stack.exe
```

Compile the independent tests in the same way:

```powershell
.\\vnt.exe --compile examples\\courses\\net-stack\\tests\\smoke.vnt -o net-stack-smoke.exe
.\\net-stack-smoke.exe
.\\vnt.exe --compile examples\\courses\\net-stack\\tests\\routing.vnt -o net-stack-routing.exe
.\\net-stack-routing.exe
.\\vnt.exe --compile examples\\courses\\net-stack\\tests\\state-machine.vnt -o net-stack-state.exe
.\\net-stack-state.exe
```

Tests emit stable PASS/FAIL text; any FAIL marker is a test failure. These commands describe the intended workflow. Verify compiler output on the current checkout before treating the project as passing.

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
