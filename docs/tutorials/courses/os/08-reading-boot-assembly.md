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