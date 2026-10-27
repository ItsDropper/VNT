/*
 * Copyright (c) 2026 ItsDropper
 * Licensed under the MIT License. See LICENSE in the repository root.
 *
 * Low-memory AP startup trampoline. VNT copies this position-independent
 * blob to physical 0x8000, fills its stack/entry slots, then sends INIT/SIPI.
 */
.section .rodata.ap_trampoline,"a",@progbits
.balign 16
.code16
.global ap_trampoline_start
ap_trampoline_start:
    cli
    cld
    xorw %ax, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %ss

    /* The copied GDT descriptor must contain its low-memory base address. */
    lgdt 0x8000 + (ap_trampoline_gdt_pointer - ap_trampoline_start)

    movl %cr4, %eax
    orl $0x20, %eax
    movl %eax, %cr4
    movl $pml4_table, %eax
    movl %eax, %cr3

    movl $0xc0000080, %ecx
    rdmsr
    orl $0x100, %eax
    wrmsr

    movl %cr0, %eax
    orl $0x80000001, %eax
    movl %eax, %cr0
    ljmpl $0x08, $(0x8000 + (ap_trampoline_long_mode - ap_trampoline_start))

.code64
ap_trampoline_long_mode:
    movw $0x10, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %ss
    xorw %ax, %ax
    movw %ax, %fs
    movw %ax, %gs

    /* AP-local IA32_APIC_BASE: conventional xAPIC MMIO, software enabled
       later only if/when per-CPU interrupt policy is ready. */
    movl $0x1b, %ecx
    movl $0xfee00800, %eax
    xorl %edx, %edx
    wrmsr

    movabsq $0xfee00020, %rax
    movl (%rax), %edi
    shrl $24, %edi

    movabsq $(0x8000 + (ap_trampoline_stack_slot - ap_trampoline_start)), %rax
    movq (%rax), %rsp
    testq %rsp, %rsp
    jz ap_trampoline_halt
    andq $-16, %rsp

    movabsq $(0x8000 + (ap_trampoline_entry_slot - ap_trampoline_start)), %rax
    movq (%rax), %rax
    testq %rax, %rax
    jz ap_trampoline_halt
    call *%rax

ap_trampoline_halt:
    cli
1:
    hlt
    jmp 1b

.balign 8
ap_trampoline_gdt:
    .quad 0
    .quad 0x00af9a000000ffff
    .quad 0x00cf92000000ffff
ap_trampoline_gdt_end:
ap_trampoline_gdt_pointer:
    .word ap_trampoline_gdt_end - ap_trampoline_gdt - 1
    .long 0x8000 + (ap_trampoline_gdt - ap_trampoline_start)

.balign 8
.global ap_trampoline_stack_slot
ap_trampoline_stack_slot:
    .quad 0
.global ap_trampoline_entry_slot
ap_trampoline_entry_slot:
    .quad 0
.global ap_trampoline_end
ap_trampoline_end:

.section .note.GNU-stack,"",@progbits
