/*
 * Copyright (c) 2026 ItsDropper
 * Licensed under the MIT License. See LICENSE in the repository root.
 *
 * x86-64 bridge for Multiboot2 memory-map records and physical-frame bitmaps.
 * Parsing policy and allocation policy live in VNT; this file only exposes
 * bounded primitives because the current freestanding VNT subset has no
 * general pointer/dereference ABI yet.
 */
.section .text
.code64

/* int vntos_mmap_count(void): -1 if the boot information/map is invalid. */
.global vntos_mmap_count
.type vntos_mmap_count,@function
vntos_mmap_count:
    call .Lmmap_validate
    testq %rax, %rax
    jz .Lmmap_count_invalid
    movq %rax, %r10                 /* info base */
    movl (%r10), %r11d              /* total info bytes */
    leaq 8(%r10), %r8               /* current tag */
    addq %r11, %r10                 /* end */
    xorl %eax, %eax                 /* entry count */
.Lmmap_count_tag:
    cmpq %r10, %r8
    jae .Lmmap_count_done
    movl (%r8), %ecx                /* tag type */
    movl 4(%r8), %edx               /* tag size */
    cmpl $8, %edx
    jb .Lmmap_count_invalid
    leaq (%r8,%rdx), %r9
    cmpq %r10, %r9
    ja .Lmmap_count_invalid
    testl %ecx, %ecx
    jz .Lmmap_count_done
    cmpl $6, %ecx
    jne .Lmmap_count_next
    cmpl $16, %edx
    jb .Lmmap_count_invalid
    movl 8(%r8), %ecx               /* entry_size */
    cmpl $24, %ecx
    jb .Lmmap_count_invalid
    movl %edx, %eax
    subl $16, %eax
    xorl %edx, %edx
    divl %ecx
    ret
.Lmmap_count_next:
    addq $7, %rdx
    andq $-8, %rdx
    addq %rdx, %r8
    jmp .Lmmap_count_tag
.Lmmap_count_done:
    xorl %eax, %eax
    ret
.Lmmap_count_invalid:
    movl $-1, %eax
    ret
.size vntos_mmap_count, .-vntos_mmap_count

/* int64_t vntos_mmap_field(int index, int field); field 0=base, 1=length, 2=type. */
.global vntos_mmap_field
.type vntos_mmap_field,@function
vntos_mmap_field:
    testl %edi, %edi
    js .Lmmap_field_zero
    movl %edi, %r8d
    movl %esi, %r9d
    cmpl $2, %r9d
    ja .Lmmap_field_zero
    call .Lmmap_validate
    testq %rax, %rax
    jz .Lmmap_field_zero
    movq %rax, %r10
    movl (%r10), %r11d
    leaq 8(%r10), %rsi
    addq %r11, %r10
.Lmmap_field_tag:
    cmpq %r10, %rsi
    jae .Lmmap_field_zero
    movl (%rsi), %ecx
    movl 4(%rsi), %edx
    cmpl $8, %edx
    jb .Lmmap_field_zero
    leaq (%rsi,%rdx), %rax
    cmpq %r10, %rax
    ja .Lmmap_field_zero
    testl %ecx, %ecx
    jz .Lmmap_field_zero
    cmpl $6, %ecx
    jne .Lmmap_field_next
    cmpl $16, %edx
    jb .Lmmap_field_zero
    movl 8(%rsi), %ecx
    cmpl $24, %ecx
    jb .Lmmap_field_zero
    leaq 16(%rsi), %rdi
    movl %edx, %eax
    subl $16, %eax
    xorl %edx, %edx
    divl %ecx
    movl %eax, %r11d               /* entry count */
    cmpl %r11d, %r8d
    jae .Lmmap_field_zero
    imulq %r8, %rcx
    addq %rcx, %rdi
    cmpl $0, %r9d
    je .Lmmap_field_base
    cmpl $1, %r9d
    je .Lmmap_field_length
    movl 16(%rdi), %eax
    ret
.Lmmap_field_base:
    movq 0(%rdi), %rax
    ret
.Lmmap_field_length:
    movq 8(%rdi), %rax
    ret
.Lmmap_field_next:
    movq %rdx, %rax
    addq $7, %rax
    andq $-8, %rax
    addq %rax, %rsi
    jmp .Lmmap_field_tag
.Lmmap_field_zero:
    xorl %eax, %eax
    ret
.size vntos_mmap_field, .-vntos_mmap_field

/* Return a valid Multiboot2 information pointer or zero.
 * The current bootstrap only identity-maps physical addresses below 1 GiB.
 */
.Lmmap_validate:
    cmpl $0x36d76289, multiboot2_boot_magic(%rip)
    jne .Lmmap_invalid
    movl multiboot2_info_ptr(%rip), %eax
    testl %eax, %eax
    jz .Lmmap_invalid
    cmpl $0x40000000, %eax
    jae .Lmmap_invalid
    movl (%rax), %ecx
    cmpl $16, %ecx
    jb .Lmmap_invalid
    movl %eax, %edx
    addl %ecx, %edx
    jc .Lmmap_invalid
    cmpl $0x40000000, %edx
    ja .Lmmap_invalid
    ret
.Lmmap_invalid:
    xorl %eax, %eax
    ret

/* Bitmap primitives use one bit per 4-KiB frame in the first 1 GiB.
 * frame >= 262144 is rejected. VNT owns policy and calls these primitives.
 */
.macro BITMAP_SET name, symbol
.global \name
.type \name,@function
\name:
    cmpl $262144, %edi
    jae .Lbitmap_bad_\@
    movl %edi, %ecx
    movl %edi, %eax
    shrl $3, %ecx
    andl $7, %eax
    movl $1, %edx
    movb %al, %cl
    shll %cl, %edx
    leaq \symbol(%rip), %rax
    testl %esi, %esi
    jz .Lbitmap_clear_\@
    orb %dl, (%rax,%rcx)
    movl $1, %eax
    ret
.Lbitmap_clear_\@:
    notb %dl
    andb %dl, (%r11,%r8)
    movl $1, %eax
    ret
.Lbitmap_bad_\@:
    xorl %eax, %eax
    ret
.size \name, .-\name
.endm

.macro BITMAP_GET name, symbol
.global \name
.type \name,@function
\name:
    cmpl $262144, %edi
    jae .Lbitmap_get_bad_\@
    movl %edi, %ecx
    movl %edi, %edx
    shrl $3, %ecx
    andl $7, %edx
    movl $1, %eax
    movb %dl, %cl
    shll %cl, %eax
    leaq \symbol(%rip), %rdx
    andb (%rdx,%rcx), %al
    setne %al
    movzbl %al, %eax
    ret
.Lbitmap_get_bad_\@:
    xorl %eax, %eax
    ret
.size \name, .-\name
.endm

BITMAP_SET vntos_phys_usable_set, vntos_phys_usable_bitmap
BITMAP_GET vntos_phys_usable_get, vntos_phys_usable_bitmap
BITMAP_SET vntos_phys_allocated_set, vntos_phys_allocated_bitmap
BITMAP_GET vntos_phys_allocated_get, vntos_phys_allocated_bitmap

.section .bss
.balign 4096
.global vntos_phys_usable_bitmap
vntos_phys_usable_bitmap:
    .skip 32768
.balign 4096
.global vntos_phys_allocated_bitmap
vntos_phys_allocated_bitmap:
    .skip 32768

.section .note.GNU-stack,"",@progbits
