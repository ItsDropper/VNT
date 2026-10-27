# Build VNTOS: A Beginner's Operating-System Course

**Goal:** understand, build, boot, inspect, and safely extend the real VNTOS kernel in `os/`. The course follows the current implementation rather than treating planned features as completed.

VNTOS is an experimental x86-64 learning kernel. It has a Multiboot2 bootstrap and initial 1-GiB identity map, VNT serial/VGA console, polled PS/2 input, IDT and exception handling, PIT/PIC timer interrupts, a bounded physical page-frame allocator, early virtual-memory mapping/probing, ACPI/MADT discovery, Local APIC setup, AP startup, and a fixed-vector APIC IPI test. It does **not** yet have a scheduler, process isolation, ring-3 execution, a filesystem, a general heap, or a generally SMP-safe allocator.

## Who this is for

No prior OS-development experience is required. Basic terminal use helps. Read lessons in order: later lessons depend on concepts introduced earlier. Implementation lessons point to the real files, explain design decisions and failure cases, and provide commands and observable checks.

## Safety and working rules

- Use QEMU for experiments; do not install this kernel on your physical PC.
- Never write an image to a USB drive or disk containing important files.
- Run build commands from the repository root unless a lesson says otherwise.
- Inspect the first build error. Later linker errors can be consequences.
- A successful compile is not proof of a successful boot or interrupt.
- The freestanding VNT compiler subset is experimental. If course text and source disagree, source and current CI behavior win.
- Make one change at a time and retain a recoverable Git branch.

## How to study a lesson

1. Read the goal and vocabulary.
2. Open the exact source files named by the lesson and trace the call path.
3. Run the stated build/test commands and compare the observed output with the expectation.
4. Answer the checkpoint in your own words.
5. Change code only after you can explain its failure modes.
6. Rebuild, boot in QEMU, and inspect the relevant GitHub Actions run.

## Course map

### Phase 1 — Foundations and setup
1. [Start here: your first kernel lab](00-start-here.md)
2. [What an operating system does](01-what-is-an-os.md)
3. [The machine underneath the OS](02-computer-basics.md)
4. [Set up the safe development environment](03-setup.md)
5. [Learn just enough VNT for kernel code](04-vnt-for-kernels.md)

### Phase 2 — Boot, CPU state, and the ELF
6. [Follow the build pipeline](05-build-pipeline.md)
7. [Understand the boot sequence](06-the-boot-sequence.md)
8. [Understand GRUB and Multiboot2](07-grub-and-multiboot2.md)
9. [Read the assembly bootstrap](08-reading-boot-assembly.md)
10. [Enter x86-64 long mode](09-long-mode.md)
11. [Understand the initial page tables](10-initial-paging.md)
12. [Understand ELF and the linker script](11-elf-and-linker.md)

### Phase 3 — Freestanding VNT and the original shell
13. [Hosted programs versus freestanding kernel code](12-freestanding-vnt.md)
14. [Serial output and input](13-serial-io.md)
15. [Read the command shell](14-the-serial-shell.md)
16. [Debugging boot failures](15-debugging.md)
17. [Test changes without guessing](16-testing.md)
18. [Make your first kernel change](17-first-kernel-change.md)
19. [What comes next?](18-next-steps.md)

### Phase 4 — A modular kernel and interrupts
20. [Split the kernel into VNT modules](19-split-the-kernel-into-vnt-modules.md)
21. [Interrupts, exceptions, and the IDT](20-interrupts-and-the-idt.md)
22. [The small assembly boundary](21-minimal-assembly-entry-stubs.md)
23. [Keep interrupt policy in VNT](22-vnt-interrupt-dispatch.md)
24. [The PIT, PIC, and timer ticks](23-pit-pic-and-timer-ticks.md)
25. [Build and verify the interrupt kernel](24-build-and-test-the-interrupt-kernel.md)

### Phase 5 — Memory management
26. [Physical memory and page frames](26-physical-memory-and-page-frames.md)
27. [Graduation project: improve a subsystem](25-graduation-project.md)

### Phase 6 — Firmware, SMP, and synchronization
28. [ACPI, APIC, and the road to SMP](27-acpi-apic-and-smp.md)
29. [Fixed-vector APIC IPIs](28-apic-ipi-service.md)
30. [SMP safety: synchronization before shared memory](29-smp-synchronization-contract.md)
31. [Improve physical-frame allocation with next-fit](30-physical-allocator-next-fit.md)

## Source map

| File | Responsibility |
| --- | --- |
| `os/arch/x86_64/boot.S` | Multiboot2 header, early stack, initial paging, long-mode transition, AP startup support |
| `os/arch/x86_64/interrupts.S` | IDT gate construction, minimal interrupt entry/exit, PIC/PIT setup |
| `os/arch/x86_64/ap_trampoline.S` | Low-memory secondary-CPU startup trampoline |
| `os/mm/phys.vnt` | Multiboot2 memory-map parsing, frame bitmaps, and next-fit allocation policy |
| `os/mm/virt.vnt` | Early page-table inspection and mapping policy in VNT |
| `os/firmware/acpi.vnt` | ACPI tables and MADT CPU/controller discovery |
| `os/arch/x86_64/apic.vnt` | Local APIC and I/O APIC policy in VNT |
| `os/kernel/smp.vnt` | AP startup, online bookkeeping, fixed-vector IPI test |
| `os/kernel.vnt` | Ordered initialization and top-level kernel flow |
| `os/kernel/core.vnt` | Shared-state access, timer ticks, CPU halt/reboot primitives |
| `os/kernel/shell.vnt` | Shell command parsing and input loop |
| `os/interrupts.vnt` | VNT-side exception and IRQ dispatch policy |
| `os/mm/phys.vnt` | Bounded physical page-frame allocator |
| `os/mm/virt.vnt` | Early virtual translation/mapping policy and self-test |
| `.github/workflows/vntos-boot.yml` | Build, compiler-codegen checks, and QEMU shell/boot assertions |

## Build tooling note: VBuild

The authoritative VNTOS build remains:

```sh
bash os/build.sh
```

The generic VBuild `elf` target does not replace this mixed VNT/assembly kernel pipeline with its custom linker script and GRUB ISO packaging. See [VBuild documentation](../../../VBUILD.md) for supported targets and limitations.

Start with [Lesson 00](00-start-here.md).
