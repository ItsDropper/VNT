# Build a Network Stack Simulator in VNT — 20 Lessons

A beginner-friendly course that builds an offline, multi-file network stack simulator. It covers byte parsing, Ethernet, ARP, IPv4, checksums, routing, ICMP, UDP, TCP state concepts, fragmentation, fuzzing, and an end-to-end packet inspector. It does not send packets to real networks.

## Safety and scope

This is an offline protocol laboratory: all frames are synthetic byte arrays and fixtures. It uses no raw sockets, packet injection, or network sniffing. The goal is to learn protocol design and parser correctness, not to create a production TCP/IP stack. Verify VNT array and module syntax against the current compiler and standard library.

## Lessons

- [00 — Network lab and safety](00-network-lab-and-safety.md)
- [01 — Bits, bytes, and wire formats](01-bits-bytes-and-wire-formats.md)
- [02 — Build a packet cursor](02-build-a-packet-cursor.md)
- [03 — Checksums and one's complement](03-checksums-and-one-s-complement.md)
- [04 — Ethernet frames](04-ethernet-frames.md)
- [05 — ARP and address resolution](05-arp-and-address-resolution.md)
- [06 — IPv4 header parsing](06-ipv4-header-parsing.md)
- [07 — IPv4 packet construction](07-ipv4-packet-construction.md)
- [08 — Routing and TTL](08-routing-and-ttl.md)
- [09 — ICMP messages](09-icmp-messages.md)
- [10 — UDP datagrams](10-udp-datagrams.md)
- [11 — Socket-like endpoints](11-socket-like-endpoints.md)
- [12 — TCP concepts and state](12-tcp-concepts-and-state.md)
- [13 — Sequence numbers and acknowledgements](13-sequence-numbers-and-acknowledgements.md)
- [14 — Fragmentation and reassembly](14-fragmentation-and-reassembly.md)
- [15 — Protocol errors and observability](15-protocol-errors-and-observability.md)
- [16 — Fuzzing parsers](16-fuzzing-parsers.md)
- [17 — End-to-end simulated packet path](17-end-to-end-simulated-packet-path.md)
- [18 — Testing and performance](18-testing-and-performance.md)
- [19 — Graduation: packet inspector](19-graduation-packet-inspector.md)

## Example project

The companion project is in `examples/courses/net-stack/`. Its packet parser, checksum code, protocol modules, simulator, fixtures, and tests are separate files. Implement each module as you reach its lesson; the starter is a scaffold, not a finished stack.

## Use VBuild while following the course

Set up the companion project using its [VBuild instructions](../../../../examples/courses/net-stack/README.md#build-and-run-with-vbuild). This gives the simulator consistent `build`, `run`, `test`, and `verify` tasks, so you can focus on packet formats and parser correctness rather than repeating compiler commands.

1. Build VBuild from the repository root: `gcc -std=c11 -O2 tools/vbuild.c -o vbuild.exe` on Windows, or `gcc -std=c11 -O2 tools/vbuild.c -o vbuild` on Linux/WSL.
2. Add the `vbuild.properties` and `build.vbuild` templates from the companion README inside `examples/courses/net-stack/`. Do not run `init` on top of the existing source tree.
3. From that example directory, run `..\..\..\vbuild.exe build`, `run`, and `test`; on Linux/WSL use `../../../vbuild`.
4. For each new parser or protocol feature, run the test task and add valid, truncated, malformed, and unsupported fixtures. Make failing assertions produce a non-zero exit status; VBuild cannot infer failure from printed text alone.

Use direct `vnt --compile` commands when isolating compiler errors from build-driver errors. Read the [VBuild reference](../../../VBUILD.md) for target selection, task dependencies, incremental rebuilds, and the current limitations of test discovery.

## Working method

For every protocol, define the wire format, validate lengths before reading, test known-good fixtures, test malformed fixtures, and document unsupported behavior. A parser that merely avoids crashing is not necessarily correct.