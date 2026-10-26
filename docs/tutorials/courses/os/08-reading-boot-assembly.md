# Lesson 08 — Read the Assembly Bootstrap

**Goal:** understand the layout and purpose of `os/boot.S` without needing to memorize x86 assembly.

## 1. Assembly is close to the CPU

VNT code is compiled into machine instructions. Assembly names those instructions and registers directly. It is useful during the earliest boot stage because the CPU must be put into the right state before normal 64-bit compiled code can execute.

Read one block at a time. The comments in `boot.S` describe the broad intent; this lesson explains how to interpret the structure.

## 2. Sections and labels

The file starts with a Multiboot2 header section, then a `.boot32` section containing 32-bit instructions. A label such as `boot32_entry:` marks an address. Branches and calls can refer to labels rather than hard-coded addresses.

The `.code32` and `.code64` directives tell the assembler which instruction mode to encode for the following code. They do not themselves switch the running CPU into that mode.

## 3. The initial stack

At `boot32_entry`, the code disables interrupts, clears the direction flag, and sets the stack pointer to `boot_stack_top`. The stack storage is reserved later in the `.bss` section.

The bootstrap needs a known stack before it calls compiled functions. A stack pointer aimed at an invalid or uninitialized location can cause immediate failure.

## 4. Building page-table structures

The bootstrap points the top-level page table at lower-level structures and fills 512 entries in a page directory. Each entry maps a 2 MiB page, covering 1 GiB in total. Lesson 10 explains what that means.

## 5. Control-register operations

The bootstrap reads and writes CPU control registers:
- `CR4` is updated to enable Physical Address Extension (PAE), required for this long-mode paging setup.
- `CR3` receives the physical address of the top-level page table.
- The `EFER` model-specific register is updated to enable long mode.
- `CR0` is updated to enable paging.

These operations are architecture-specific. A normal user application cannot execute all of them because the CPU restricts privileged operations.

## 6. The far jump

After the bootstrap configures the required state and loads the GDT, it performs a far jump to `long_mode_entry`. This changes the code-segment context and begins execution at the 64-bit entry label. The 64-bit block sets segment registers, aligns the stack, and calls `_start`.

## 7. Boot assembly and interrupt assembly have different jobs

`os/boot.S` is responsible for getting from the Multiboot2 entry state into 64-bit mode with a known stack and page tables. `os/interrupts.S` is a separate, later boundary: it builds and loads the IDT, saves interrupted registers, normalizes exception frames, and returns through `iretq`. The VNT code does not replace either CPU transition with a normal function call.

The interrupt file is intentionally separate so a reader can audit the two architecture-sensitive paths independently. It does not contain the shell, command parsing, VGA drawing policy, or keyboard translation. Those remain in VNT modules.

## 8. Why the bootstrap should stay small

The assembly is difficult to maintain if it grows into the whole operating system. VNTOS's intended architecture keeps the initial CPU bootstrap in assembly and keeps shell behavior and ordinary kernel logic in VNT. Some hardware operations still use inline assembly in `kernel.vnt`; these are a current limitation to improve as compiler support develops.

## Checkpoint

Point to the instructions that:
1. set the early stack;
2. fill the page-directory entries;
3. load the page-table root;
4. enable long mode;
5. transfer to 64-bit code.

## Exercise

Explain in plain language what would go wrong if `boot32_entry` called the 64-bit kernel before setting up the required CPU state. Do not edit assembly until you can explain the existing sequence.

## 9. Read the bootstrap as a state transition

For every block of assembly, write down the processor state before and after it. At the beginning, execution is in the boot protocol's initial mode. The bootstrap then establishes a stack, prepares page-table structures, enables the CPU features required for long mode, and performs a far transfer into 64-bit code. The exact order matters because an instruction is only valid after its prerequisites are satisfied.

Do not read the file as a list of mysterious instructions. Group instructions by the invariant they establish: a valid stack, a valid page-table root, paging enabled, or execution in the intended code segment. If you cannot name the invariant, investigate the instruction before editing it.

## 10. Why the stack and page tables are early requirements

A compiled function commonly stores local variables and saved registers on the stack. Calling compiled VNT before RSP points to usable writable memory can cause the first prologue or function call to fault. Likewise, long mode with paging enabled requires valid translation structures. The initial page tables are not optional decoration; they make instruction fetches and data accesses possible under the selected CPU mode.

The current bootstrap maps the first 1 GiB using 2 MiB pages. That is a simple early mapping, not a complete virtual-memory manager. It does not track physical page ownership or allow independent address spaces.

## 11. Keep architecture code minimal and reviewable

The course aims to keep device policy, command parsing, and ordinary algorithms in VNT. Assembly remains for the bootstrap and the interrupt entry/exit contract because those operations depend on exact processor state and stack layout. This is not a claim that assembly is inherently better; it is a boundary around operations the current compiler cannot express safely.

Before adding assembly, ask whether a stable VNT function can own the policy. For example, the interrupt stub saves registers and executes `iretq`, while `interrupts.vnt` decides that a timer event should increment a counter. Adding command parsing to the assembly file would mix unrelated responsibilities.

## Lab: annotate without changing behavior

Add comments in your notes—not the source—beside the instructions that establish the stack, build page-table entries, enable long mode, and transfer control. For each group, record its prerequisite and what would fail if it were moved earlier. Then compare your explanation with the comments already present in `os/boot.S`.

## 12. Inspect the actual machine code when comments are insufficient

Assembly source is precise, but the assembler and linker determine the final bytes and addresses. Use `objdump -d build/kernel.elf` to inspect disassembly and `readelf -h -l build/kernel.elf` to inspect the ELF entry point and loadable segments. Use the disassembly to answer a specific question, such as whether the far jump target is present or whether a call reaches the expected symbol.

Do not treat disassembly as a replacement for understanding the source. It is an independent view of what the toolchain produced. If source and output disagree, check which revision was built and whether the file under inspection was regenerated.

## 13. A disciplined edit to boot assembly

Boot assembly is high-risk because errors can occur before the VNT console exists. If you need to change it, first write down the invariant the existing code establishes. Make the smallest edit, rebuild, validate the ELF, and boot in QEMU. If the kernel stops earlier than before, revert the experiment and narrow the change.

Do not combine changes to page tables, GDT entries, stack setup, and interrupt initialization in one patch. These components depend on one another, and a combined change makes the first broken invariant difficult to identify.

## Lab: produce an assembly annotation

Create a table for the bootstrap's main stages: initial entry, stack setup, page-table setup, control-register configuration, long-mode jump, and kernel handoff. For each stage record input state, instructions that establish the next state, output state, and one failure symptom. Verify each claim against `os/boot.S` and the built ELF.

## 14. Explain the reason for each assembly instruction

For each non-obvious instruction, annotate three facts: which CPU state it reads, which state it changes, and which later instruction depends on the change. For example, a control-register write changes a processor feature; a far jump changes the active code segment and instruction pointer; a stack adjustment changes where later pushes and calls write data.

If a line has no clear purpose in your explanation, research it before changing it. This habit is more reliable than copying a bootstrap from a different kernel and hoping the assumptions match.
