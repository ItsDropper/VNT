# Memory Lab: Arena Allocator and Inspector

Multi-file companion to [the 20-lesson course](../../../docs/tutorials/courses/memory-lab/README.md). This is an educational scaffold; complete the TODOs in lesson order.

## Layout

- `src/main.vnt` — entry point.
- `src/arena.vnt` — allocation policy and invariants.
- `src/report.vnt` — reporting.
- `tests/smoke.vnt` — smoke-test scaffold.
- `tests/bounds.vnt` — oversized-reservation regression scaffold.

Verify API signatures against `lib/std/mem.vnt` and `docs/raw-memory.md`. The TODOs are deliberate exercises, not a claim that the starter already implements an allocator.

## Build

From the repository root, build VNT using the top-level README instructions, then compile the entry file:

```powershell
.\\build.ps1
.\\vnt.exe --compile examples\\courses\\memory-lab\\src\\main.vnt -o memory-lab.exe
.\\memory-lab.exe
```

On Linux use `./vnt` and the matching executable name. Imports resolve relative to the importing file in the current loader; inspect module tests if your checkout differs.

## Safety

Only access allocations created by the documented API. Never dereference arbitrary addresses. This hosted example does not demonstrate kernel heap management, page tables, or hardware access.