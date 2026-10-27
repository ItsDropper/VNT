# Deterministic packet fixtures

The fixture set is synthetic. Do not add captures of private traffic or use the data for packet injection.

## Fixture naming

Use one fixture per behavior and include the expected result in its name or adjacent test:
- `ethernet-short` — fewer than 14 bytes, reject.
- `ipv4-valid-minimal` — valid 20-byte header, valid checksum, no payload.
- `ipv4-total-length-shorter-than-header` — reject before slicing.
- `ipv4-total-length-beyond-buffer` — reject before slicing.
- `ipv4-bad-checksum` — reject.
- `udp-short-header` — fewer than 8 bytes, reject.
- `udp-length-under-header` — reject.
- `udp-length-beyond-ip-payload` — reject.
- `udp-echo-request` — synthetic valid request to port 7.
- `udp-echo-reply` — expected payload and swapped endpoints after simulation.
- `unsupported-ethertype` — reject or report unsupported.
- `unsupported-ip-protocol` — reject or report unsupported.

## Required fixture metadata

For each fixture, record:
1. the layer being tested;
2. total byte length;
3. the expected parser result;
4. the exact rejection reason for invalid data;
5. which boundary check the fixture exercises.

Do not only test malformed fixtures. Include known-good packets with independently calculated expected field values and checksums.

## Current status

The current smoke test creates several small malformed arrays directly in VNT. The integrated main program constructs a synthetic Ethernet/IPv4/UDP request in memory and checks the echo response. Expanding this directory into reusable named fixture files is a graduation exercise; the current project does not yet load binary fixture files from disk.
