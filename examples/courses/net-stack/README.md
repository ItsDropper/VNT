# VNT Network Stack Simulator

Multi-file companion to [the 20-lesson course](../../../docs/tutorials/courses/net-stack/README.md). This is an offline educational scaffold; complete the TODOs lesson by lesson.

## Layout

- `src/bytes.vnt` — bounds-checked byte cursor helpers.
- `src/checksum.vnt` — Internet checksum implementation and test vectors.
- `src/ipv4.vnt` — IPv4 header parser scaffold.
- `src/udp.vnt` — UDP parser scaffold.
- `src/simulator.vnt` — protocol dispatch and synthetic echo flow.
- `src/main.vnt` — entry point.
- `fixtures/README.md` — deterministic packet-fixture contract.
- `tests/smoke.vnt` — test scaffold.

## Build

Build the compiler using the root README, then compile `src/main.vnt` with the normal VNT compile command. Module import paths resolve relative to the importing file in the current loader; verify syntax against compiler tests if your checkout differs.

## Safety and limits

The project must operate on synthetic byte arrays and fixture files only. It does not use raw sockets, packet injection, or packet capture. Do not describe it as a complete TCP/IP stack. Implement only the protocol behavior you can test.
