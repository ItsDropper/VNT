/*
 * Copyright (c) 2026 ItsDropper
 * Licensed under the MIT License. See LICENSE in the repository root.
 *
 * Early x86-64 page-table operations for the bootstrap's low 1-GiB identity
 * window. This deliberately does not implement arbitrary canonical addresses,
 * huge-page allocation, SMP synchronization, or permission policy.
 */
.section .text
.code64
.extern page_directory

/* int vntos_vm_map_page(va, pa, flags, new_pt_pa)
 * Returns 0 on rejection, 1 when a 2-MiB PDE was split and new_pt_pa is now
 * owned by the page tables, or 2 when an existing 4-KiB PT was reused.
 * va/pa/new_pt_pa must be 4-KiB aligned and below 1 GiB. Caller owns a newly
 * allocated page-table frame until return 1.
 */
.global vntos_vm_map_page
.type vntos_vm_map_page,@function
vntos_vm_map_page:
    movq %rdi, %r8
    movq %rsi, %r9
    movl %edx, %r10d
    movq %rcx, %r11
    testq $0xfff, %r8
    jnz .Lvm_map_bad
    testq $0xfff, %r9
    jnz .Lvm_map_bad
    testq $0xfff, %r11
    jnz .Lvm_map_bad
    cmpq $0x40000000, %r8
    jae .Lvm_map_bad
    cmpq $0x40000000, %r9
    jae .Lvm_map_bad
    cmpq $0x40000000, %r11
    jae .Lvm_map_bad
    testl $1, %r10d
    jz .Lvm_map_bad
    andl $0xfff, %r10d

    movq %r8, %rax
    shrq $21, %rax
    andl $511, %eax
    leaq page_directory(%rip), %rsi
    leaq (%rsi,%rax,8), %rsi
    movq (%rsi), %rdx
    testq $1, %rdx
    jz .Lvm_map_bad
    testq $0x80, %rdx
    jz .Lvm_map_existing_pt

    /* Split the present 2-MiB identity mapping into 512 4-KiB PTEs. */
    movq %rdx, %rax
    andq $0xffe00000, %rax
    xorl %ecx, %ecx
.Lvm_split_loop:
    movq %rcx, %rdx
    shlq $12, %rdx
    addq %rax, %rdx
    orq $3, %rdx
    movq %rdx, (%r11,%rcx,8)
    incl %ecx
    cmpl $512, %ecx
    jne .Lvm_split_loop
    movq %r11, %rdx
    orq $3, %rdx
    movq %rdx, (%rsi)
    movl $1, %eax
    jmp .Lvm_write_pte

.Lvm_map_existing_pt:
    /* The caller's fresh page is unused; return 2 so VNT can free it. */
    andq $0xfffff000, %rdx
    movq %rdx, %rsi
    movl $2, %eax
    jmp .Lvm_write_pte

.Lvm_write_pte:
    /* Recompute the PT base from the PDE after a possible split. */
    movq %r8, %rcx
    shrq $21, %rcx
    andl $511, %ecx
    leaq page_directory(%rip), %rdx
    movq (%rdx,%rcx,8), %rdx
    andq $0xfffff000, %rdx
    movq %r8, %rcx
    shrq $12, %rcx
    andl $511, %ecx
    movq %r9, %rdi
    orq %r10, %rdi
    movq %rdi, (%rdx,%rcx,8)
    invlpg (%r8)
    ret

.Lvm_map_bad:
    xorl %eax, %eax
    ret
.size vntos_vm_map_page, .-vntos_vm_map_page

/* int64_t vntos_vm_translate(va): physical address or -1 if unmapped/unsupported. */
.global vntos_vm_translate
.type vntos_vm_translate,@function
vntos_vm_translate:
    cmpq $0x40000000, %rdi
    jae .Lvm_translate_bad
    movq %rdi, %r8
    movq %rdi, %rax
    shrq $21, %rax
    andl $511, %eax
    leaq page_directory(%rip), %rdx
    movq (%rdx,%rax,8), %rdx
    testq $1, %rdx
    jz .Lvm_translate_bad
    testq $0x80, %rdx
    jz .Lvm_translate_4k
    andq $0xffe00000, %rdx
    movq %r8, %rax
    andq $0x1fffff, %rax
    addq %rdx, %rax
    ret
.Lvm_translate_4k:
    andq $0xfffff000, %rdx
    movq %r8, %rcx
    shrq $12, %rcx
    andl $511, %ecx
    movq (%rdx,%rcx,8), %rdx
    testq $1, %rdx
    jz .Lvm_translate_bad
    andq $0xfffff000, %rdx
    movq %r8, %rax
    andq $0xfff, %rax
    addq %rdx, %rax
    ret
.Lvm_translate_bad:
    movq $-1, %rax
    ret
.size vntos_vm_translate, .-vntos_vm_translate

/* int vntos_vm_probe(va): write/read a sentinel at the mapped address. */
.global vntos_vm_probe
.type vntos_vm_probe,@function
vntos_vm_probe:
    cmpq $0x40000000, %rdi
    jae .Lvm_probe_bad
    testq $7, %rdi
    jnz .Lvm_probe_bad
    movabsq $0x564e54564d544553, %rax
    movq %rax, (%rdi)
    cmpq (%rdi), %rax
    jne .Lvm_probe_bad
    movl $1, %eax
    ret
.Lvm_probe_bad:
    xorl %eax, %eax
    ret
.size vntos_vm_probe, .-vntos_vm_probe

.section .note.GNU-stack,"",@progbits
