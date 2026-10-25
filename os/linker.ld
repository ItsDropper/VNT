/* Copyright (c) 2026 ItsDropper
 * Licensed under the MIT License. See LICENSE in the repository root.
 */
OUTPUT_FORMAT(elf64-x86-64)
OUTPUT_ARCH(i386:x86-64)
ENTRY(boot32_entry)

PHDRS
{
    text PT_LOAD FLAGS(5);
    data PT_LOAD FLAGS(6);
}

SECTIONS
{
    . = 0x00100000;
    __kernel_start = .;
    .multiboot ALIGN(8) : { KEEP(*(.multiboot)) } :text
    .boot32 ALIGN(16) : { KEEP(*(.boot32)) } :text
    .text ALIGN(0x1000) : { *(.text .text.*) } :text
    .rodata ALIGN(0x1000) : { *(.rodata .rodata.*) } :text
    .data ALIGN(0x1000) : { *(.data .data.*) } :data
    .bss ALIGN(0x1000) (NOLOAD) : {
        *(COMMON)
        *(.bss .bss.*)
    } :data
    __kernel_end = .;
    /DISCARD/ : { *(.comment) *(.eh_frame .eh_frame.*) *(.note .note.*) }
}
