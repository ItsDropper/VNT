/*
 * Copyright (c) 2026 ItsDropper
 * Licensed under the MIT License. See LICENSE in the repository root.
 *
 * x86-64 Multiboot2 bootstrap. This is the architecture-specific entry path:
 * it establishes the initial identity map and enters long mode before calling
 * the compiler-generated kernel entry point.
 */
.section .multiboot,"a"
.balign 8
.global multiboot2_header
multiboot2_header:
    .long 0xe85250d6
    .long 0
    .long multiboot2_header_end - multiboot2_header
    .long -(0xe85250d6 + (multiboot2_header_end - multiboot2_header))
    .short 0
    .short 0
    .long 8
multiboot2_header_end:

.section .boot32,"ax"
.code32
.global boot32_entry
.type boot32_entry,@function
boot32_entry:
    /* Preserve the Multiboot2 register contract before using EAX/EBX. */
    movl %eax, multiboot2_boot_magic
    movl %ebx, multiboot2_info_ptr
    movw $0xe9, %dx
    movb $66, %al
    outb %al, %dx
    cli
    cld
    movl $boot_stack_top, %esp

    /* PML4[0] -> PDPT[0] -> page directory, all present+writable. */
    movl $pdpt_table, %eax
    orl $3, %eax
    movl %eax, pml4_table
    movl $page_directory, %eax
    orl $3, %eax
    movl %eax, pdpt_table

    /* Identity-map the first GiB using 512 2-MiB pages. */
    movl $page_directory, %edi
    xorl %ecx, %ecx
.Los_map_2m:
    movl %ecx, %eax
    shll $21, %eax
    orl $0x83, %eax
    movl %eax, (%edi,%ecx,8)
    movl $0, 4(%edi,%ecx,8)
    incl %ecx
    cmpl $512, %ecx
    jne .Los_map_2m
    movw $0xe9, %dx
    movb $49, %al
    outb %al, %dx

    movl %cr4, %eax
    orl $0x20, %eax
    movl %eax, %cr4
    movl $pml4_table, %eax
    movl %eax, %cr3
    movw $0xe9, %dx
    movb $50, %al
    outb %al, %dx

    movl $0xc0000080, %ecx
    rdmsr
    orl $0x100, %eax
    wrmsr
    movw $0xe9, %dx
    movb $51, %al
    outb %al, %dx

    lgdt gdt64_pointer
    movl %cr0, %eax
    orl $0x80000000, %eax
    movl %eax, %cr0
    movw $0xe9, %dx
    movb $52, %al
    outb %al, %dx
    ljmp $0x08, $long_mode_entry
.size boot32_entry, .-boot32_entry

.code64
long_mode_entry:
    movw $0xe9, %dx
    movb $76, %al
    outb %al, %dx
    movw $0x10, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %ss
    xorw %ax, %ax
    movw %ax, %fs
    movw %ax, %gs
    leaq boot_stack_top(%rip), %rsp
    andq $-16, %rsp
    call _start
.Los_boot_halt:
    cli
    hlt
    jmp .Los_boot_halt

.balign 8
gdt64:
    .quad 0
    .quad 0x00af9a000000ffff
    .quad 0x00cf92000000ffff
gdt64_end:
gdt64_pointer:
    .word gdt64_end - gdt64 - 1
    .long gdt64


.section .bss
.balign 4096
pml4_table: .skip 4096
pdpt_table: .skip 4096
.global page_directory
page_directory: .skip 4096
.balign 16
boot_stack_bottom: .skip 16384
boot_stack_top:


/* Multiboot2 values captured at the 32-bit entry point. */
.section .data
.balign 4
.global multiboot2_boot_magic
multiboot2_boot_magic: .long 0
.global multiboot2_info_ptr
multiboot2_info_ptr: .long 0

/* Physical-frame bitmap storage; bitmap algorithms live in os/mm/phys.vnt. */
.balign 4096
.global vntos_phys_usable_bitmap
vntos_phys_usable_bitmap:
    .skip 32768
.balign 4096
.global vntos_phys_allocated_bitmap
vntos_phys_allocated_bitmap:
    .skip 32768
.balign 4096
.global vntos_phys_reserved_bitmap
vntos_phys_reserved_bitmap:
    .skip 32768

/* Shared scalar state for the VNT-compiled VGA/PS2 drivers. */
.balign 8
.global vntos_state
vntos_state:
    .quad 0       /* VGA text cursor */
    .quad 15      /* VGA color attribute */
    .quad 40      /* Mouse X cell */
    .quad 12      /* Mouse Y cell */
    .quad 0       /* Mouse button bits */
    .quad 0       /* Mouse packet index */
    .quad 0       /* Mouse packet byte 0 */
    .quad 0       /* Mouse packet byte 1 */
    .quad 0       /* Mouse packet byte 2 */
    .quad 0       /* Keyboard shift state */
    .quad 0       /* Keyboard caps-lock state */
    .quad 0       /* PIT timer interrupt count */
    .quad 0       /* IRQ0 count */
    .quad 0       /* IRQ1 count */
    .quad 0       /* IRQ2 count */
    .quad 0       /* IRQ3 count */
    .quad 0       /* IRQ4 count */
    .quad 0       /* IRQ5 count */
    .quad 0       /* IRQ6 count */
    .quad 0       /* IRQ7 count */
    .quad 0       /* IRQ8 count */
    .quad 0       /* IRQ9 count */
    .quad 0       /* IRQ10 count */
    .quad 0       /* IRQ11 count */
    .quad 0       /* IRQ12 count */
    .quad 0       /* IRQ13 count */
    .quad 0       /* IRQ14 count */
    .quad 0       /* IRQ15 count */
    .quad 0       /* Usable physical page count */
    .quad 0       /* Free physical page count */
