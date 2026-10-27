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

    /* Reserve PDPT[3] for a narrow identity map of APIC MMIO windows. */
    movl $mmio_page_directory, %eax
    orl $3, %eax
    movl %eax, pdpt_table+24
    movl $0, pdpt_table+28
    /* IOAPIC at 0xFEC00000 and Local APIC at 0xFEE00000. */
    movl $0xfec0009b, mmio_page_directory+4016
    movl $0, mmio_page_directory+4020
    movl $0xfee0009b, mmio_page_directory+4024
    movl $0, mmio_page_directory+4028

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
.global pml4_table
pml4_table: .skip 4096
pdpt_table: .skip 4096
.global page_directory
page_directory: .skip 4096
.balign 4096
mmio_page_directory: .skip 4096
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
.section .bss
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

.section .data
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
    .quad 0       /* ACPI/APIC/SMP state 30 */
    .quad 0       /* ACPI/APIC/SMP state 31 */
    .quad 0       /* ACPI/APIC/SMP state 32 */
    .quad 0       /* ACPI/APIC/SMP state 33 */
    .quad 0       /* ACPI/APIC/SMP state 34 */
    .quad 0       /* ACPI/APIC/SMP state 35 */
    .quad 0       /* ACPI/APIC/SMP state 36 */
    .quad 0       /* ACPI/APIC/SMP state 37 */
    .quad 0       /* ACPI/APIC/SMP state 38 */
    .quad 0       /* ACPI/APIC/SMP state 39 */
    .quad 0       /* ACPI/APIC/SMP state 40 */
    .quad 0       /* ACPI/APIC/SMP state 41 */
    .quad 0       /* ACPI/APIC/SMP state 42 */
    .quad 0       /* ACPI/APIC/SMP state 43 */
    .quad 0       /* ACPI/APIC/SMP state 44 */
    .quad 0       /* ACPI/APIC/SMP state 45 */
    .quad 0       /* ACPI/APIC/SMP state 46 */
    .quad 0       /* ACPI/APIC/SMP state 47 */
    .quad 0       /* ACPI/APIC/SMP state 48 */
    .quad 0       /* ACPI/APIC/SMP state 49 */
    .quad 0       /* ACPI/APIC/SMP state 50 */
    .quad 0       /* ACPI/APIC/SMP state 51 */
    .quad 0       /* ACPI/APIC/SMP state 52 */
    .quad 0       /* ACPI/APIC/SMP state 53 */
    .quad 0       /* ACPI/APIC/SMP state 54 */
    .quad 0       /* ACPI/APIC/SMP state 55 */
    .quad 0       /* ACPI/APIC/SMP state 56 */
    .quad 0       /* ACPI/APIC/SMP state 57 */
    .quad 0       /* ACPI/APIC/SMP state 58 */
    .quad 0       /* ACPI/APIC/SMP state 59 */
    .quad 0       /* ACPI/APIC/SMP state 60 */
    .quad 0       /* ACPI/APIC/SMP state 61 */
    .quad 0       /* ACPI/APIC/SMP state 62 */
    .quad 0       /* ACPI/APIC/SMP state 63 */
    .quad 0       /* ACPI interrupt-source override storage 64 */
    .quad 0       /* ACPI interrupt-source override storage 65 */
    .quad 0       /* ACPI interrupt-source override storage 66 */
    .quad 0       /* ACPI interrupt-source override storage 67 */
    .quad 0       /* ACPI interrupt-source override storage 68 */
    .quad 0       /* ACPI interrupt-source override storage 69 */
    .quad 0       /* ACPI interrupt-source override storage 70 */
    .quad 0       /* ACPI interrupt-source override storage 71 */
    .quad 0       /* ACPI interrupt-source override storage 72 */
    .quad 0       /* ACPI interrupt-source override storage 73 */
    .quad 0       /* ACPI interrupt-source override storage 74 */
    .quad 0       /* ACPI interrupt-source override storage 75 */
    .quad 0       /* ACPI interrupt-source override storage 76 */
    .quad 0       /* ACPI interrupt-source override storage 77 */
    .quad 0       /* ACPI interrupt-source override storage 78 */
    .quad 0       /* ACPI interrupt-source override storage 79 */
    .quad 0       /* ACPI interrupt-source override storage 80 */
    .quad 0       /* ACPI interrupt-source override storage 81 */
    .quad 0       /* ACPI interrupt-source override storage 82 */
    .quad 0       /* ACPI interrupt-source override storage 83 */
    .quad 0       /* ACPI interrupt-source override storage 84 */
    .quad 0       /* ACPI interrupt-source override storage 85 */
    .quad 0       /* ACPI interrupt-source override storage 86 */
    .quad 0       /* ACPI interrupt-source override storage 87 */
    .quad 0       /* ACPI interrupt-source override storage 88 */
    .quad 0       /* ACPI interrupt-source override storage 89 */
    .quad 0       /* ACPI interrupt-source override storage 90 */
    .quad 0       /* ACPI interrupt-source override storage 91 */
    .quad 0       /* ACPI interrupt-source override storage 92 */
    .quad 0       /* ACPI interrupt-source override storage 93 */
    .quad 0       /* ACPI interrupt-source override storage 94 */
    .quad 0       /* ACPI interrupt-source override storage 95 */
    .quad 0       /* ACPI interrupt-source override storage 96 */
    .quad 0       /* ACPI interrupt-source override storage 97 */
    .quad 0       /* ACPI interrupt-source override storage 98 */
    .quad 0       /* ACPI interrupt-source override storage 99 */
    .quad 0       /* ACPI interrupt-source override storage 100 */
    .quad 0       /* ACPI interrupt-source override storage 101 */
    .quad 0       /* ACPI interrupt-source override storage 102 */
    .quad 0       /* ACPI interrupt-source override storage 103 */
    .quad 0       /* ACPI interrupt-source override storage 104 */
    .quad 0       /* ACPI interrupt-source override storage 105 */
    .quad 0       /* ACPI interrupt-source override storage 106 */
    .quad 0       /* ACPI interrupt-source override storage 107 */
    .quad 0       /* ACPI interrupt-source override storage 108 */
    .quad 0       /* ACPI interrupt-source override storage 109 */
    .quad 0       /* ACPI interrupt-source override storage 110 */
    .quad 0       /* ACPI interrupt-source override storage 111 */
    .quad 0       /* SMP CPU state 112 */
    .quad 0       /* SMP CPU state 113 */
    .quad 0       /* SMP CPU state 114 */
    .quad 0       /* SMP CPU state 115 */
    .quad 0       /* SMP CPU state 116 */
    .quad 0       /* SMP CPU state 117 */
    .quad 0       /* SMP CPU state 118 */
    .quad 0       /* SMP CPU state 119 */
    .quad 0       /* SMP CPU state 120 */
    .quad 0       /* SMP CPU state 121 */
    .quad 0       /* SMP CPU state 122 */
    .quad 0       /* SMP CPU state 123 */
    .quad 0       /* SMP CPU state 124 */
    .quad 0       /* SMP CPU state 125 */
    .quad 0       /* SMP CPU state 126 */
    .quad 0       /* SMP CPU state 127 */
    .quad 0       /* SMP CPU state 128 */
    .quad 0       /* SMP CPU state 129 */
    .quad 0       /* SMP CPU state 130 */
    .quad 0       /* SMP CPU state 131 */
    .quad 0       /* SMP CPU state 132 */
    .quad 0       /* SMP CPU state 133 */
    .quad 0       /* SMP CPU state 134 */
    .quad 0       /* SMP CPU state 135 */
    .quad 0       /* SMP CPU state 136 */
    .quad 0       /* SMP CPU state 137 */
    .quad 0       /* SMP CPU state 138 */
    .quad 0       /* SMP CPU state 139 */
    .quad 0       /* SMP CPU state 140 */
    .quad 0       /* SMP CPU state 141 */
    .quad 0       /* SMP CPU state 142 */
    .quad 0       /* SMP CPU state 143 */
    .quad 0       /* SMP IPI/CPU state 144 */
    .quad 0       /* SMP IPI/CPU state 145 */
    .quad 0       /* SMP IPI/CPU state 146 */
    .quad 0       /* SMP IPI/CPU state 147 */
    .quad 0       /* SMP IPI/CPU state 148 */
    .quad 0       /* SMP IPI/CPU state 149 */
    .quad 0       /* SMP IPI/CPU state 150 */
    .quad 0       /* SMP IPI/CPU state 151 */
    .quad 0       /* SMP IPI/CPU state 152 */
    .quad 0       /* SMP IPI/CPU state 153 */
    .quad 0       /* SMP IPI/CPU state 154 */
    .quad 0       /* SMP IPI/CPU state 155 */
    .quad 0       /* SMP IPI/CPU state 156 */
    .quad 0       /* SMP IPI/CPU state 157 */
    .quad 0       /* SMP IPI/CPU state 158 */
    .quad 0       /* SMP IPI/CPU state 159 */
    .quad 0       /* SMP IPI/CPU state 160 */
    .quad 0       /* SMP IPI/CPU state 161 */
    .quad 0       /* SMP IPI/CPU state 162 */
    .quad 0       /* SMP IPI/CPU state 163 */
    .quad 0       /* SMP IPI/CPU state 164 */
    .quad 0       /* SMP IPI/CPU state 165 */
    .quad 0       /* SMP IPI/CPU state 166 */
    .quad 0       /* SMP IPI/CPU state 167 */
    .quad 0       /* SMP IPI/CPU state 168 */
    .quad 0       /* SMP IPI/CPU state 169 */
    .quad 0       /* SMP IPI/CPU state 170 */
    .quad 0       /* SMP IPI/CPU state 171 */
    .quad 0       /* SMP IPI/CPU state 172 */
    .quad 0       /* SMP IPI/CPU state 173 */
    .quad 0       /* SMP IPI/CPU state 174 */
    .quad 0       /* SMP IPI/CPU state 175 */
    .quad 256     /* Physical allocator next-fit hint: frame index 176 */
    .quad 0       /* PS/2 extended-key prefix pending: state 177 */
    .quad 0       /* Last shell command length: state 178 */
    .quad 0       /* Reserved shell state: state 179 */
    .rept 128
    .quad 0       /* Last shell command bytes: states 180..307 */
    .endr
    .quad 0       /* Atomic SMP self-test counter: state 308 */
    .quad 0       /* Atomic SMP self-test armed flag: state 309 */
    .quad 0       /* Atomic SMP self-test AP completion count: state 310 */
    .quad 0       /* Atomic SMP self-test AP-ready count: state 311 */
    .quad 0       /* Atomic SMP self-test start barrier: state 312 */
.balign 8
.global vntos_sync_test_lock
vntos_sync_test_lock:
    .quad 0

.section .note.GNU-stack,"",@progbits
