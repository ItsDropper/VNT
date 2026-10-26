# VNT Programming Courses

These tutorials teach VNT through small, practical projects. VNT is experimental; use the compiler and examples in your current checkout as the final syntax reference.

## Beginner course

1. [First program: values, variables, and output](01-first-program.md)
2. [Conditions, loops, and functions](02-control-flow-and-functions.md)
3. [Modules and text-file storage](03-modules-and-files.md)
4. [Build a small terminal journal](04-terminal-journal.md)
5. [Windows GUI basics](05-windows-gui.md)

## Intermediate course

6. [Arrays, structs, and references](06-arrays-structs-references.md)
7. [Launching and managing child processes](07-native-processes.md)
8. [HTTP APIs and external tools](08-http-apis.md)
9. [Build a complete local task tracker](09-build-a-project.md)
10. [Low-level systems programming](10-low-level-systems-programming.md)


## Advanced low-level project course

[Build a Memory Arena and Inspector in VNT — 20 lessons](courses/memory-lab/README.md). Starts from pointer validity and unsafe contracts, then teaches alignment, overflow-safe bounds checks, allocation lifetime, testing, stress testing, and benchmarking. Includes a multi-file example project under `examples/courses/memory-lab/`.


## Advanced systems project courses

- [Build a Memory Arena and Inspector in VNT — 20 lessons](courses/memory-lab/README.md). Teaches pointer validity, alignment, overflow-safe bounds checks, allocation lifetime, testing, and benchmarking, with a multi-file project in `examples/courses/memory-lab/`.
- [Build a Network Stack Simulator in VNT — 20 lessons](courses/net-stack/README.md). Teaches packet parsing, Ethernet, ARP, IPv4, checksums, routing, ICMP, UDP, TCP state concepts, fragmentation, and parser fuzzing. All exercises are offline and include a multi-file project in `examples/courses/net-stack/`.

## Programming-language implementation course

[Build a programming language in VNT — 15 beginner lessons](courses/programming-language/README.md). Covers language design, lexing, tokens, ASTs, parsing, runtime values, control flow, a text adventure, testing, and extensions.

## Full operating-system development course

[Build VNTOS from the ground up — 19 beginner-friendly lessons](courses/os/README.md). Starts with computer and OS fundamentals, then covers the build pipeline, GRUB/Multiboot2, x86-64 startup, paging, ELF/linking, freestanding VNT, serial I/O, debugging, testing, and a first kernel modification. Designed to explain the current `os/` implementation without claiming unimplemented subsystems exist.

## Systems programming course

11. [Native memory fundamentals](04-native-memory.md) — pointers, allocation lifetime, and unsafe blocks.
12. [Build the VNTOS learning kernel](05-vntos-kernel-walkthrough.md) — how `os/` boots, how the VNT kernel is built, and how to test it in QEMU.
13. [Arrays, functions, and modules](03-arrays-and-modules.md) — a supplemental practice lesson.
14. [Low-level systems programming](10-low-level-systems-programming.md) — memory, unsafe code, assembly, and the kernel roadmap.

The [API reference](../api-reference.md) summarizes runtime APIs. The [raw-memory guide](../raw-memory.md) and [compiler architecture guide](../compiler-architecture.md) cover lower-level details.

## Compile and run a tutorial

For a source file called `lesson.vnt` in the repository root:

```powershell
.\\vnt.exe --compile lesson.vnt -o lesson.exe
.\\lesson.exe
```

The compiler also keeps generated assembly beside the output executable. GUI examples require Windows. If an example fails, confirm the compiler was built from the current repository revision and run `.\\tests\\run_language_checks.ps1`.

For kernel experiments, use QEMU and follow the [complete VNTOS course](courses/os/README.md) or the [short kernel walkthrough](05-vntos-kernel-walkthrough.md). Do not write an experimental kernel image to a disk containing important data.
