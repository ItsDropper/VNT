/*
 * Copyright (c) 2026 ItsDropper
 * Licensed under the MIT License. See LICENSE in the repository root.
 *
 * Multiboot2 32-bit entry transitions to long mode with a 1 GiB identity map.
 * The freestanding kernel then enters a COM1-backed serial command shell.
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

.section .text,"ax"
.code64
.global os_shell
.type os_shell,@function
os_shell:
    /* Entry from kernel_main has RSP%16 == 8; align before nested calls. */
    subq $8, %rsp
    movw $0xe9, %dx
    movb $83, %al
    outb %al, %dx
    call os_serial_init
    leaq .Los_welcome(%rip), %rdi
    call os_puts
    leaq .Los_prompt(%rip), %rdi
    call os_puts
    leaq .Los_ready(%rip), %rdi
    call os_puts

.Los_input:
    /* COM1 line-status register bit 0 means receive data is ready. */
    movw $0x3fd, %dx
    inb %dx, %al
    testb $1, %al
    jz .Los_input
    movw $0x3f8, %dx
    inb %dx, %al
    cmpb $13, %al
    je .Los_enter
    cmpb $10, %al
    je .Los_enter
    cmpb $8, %al
    je .Los_backspace
    cmpb $127, %al
    je .Los_backspace
    testb %al, %al
    jz .Los_input
    movb %al, %r13b
    movl os_command_length(%rip), %eax
    cmpl $127, %eax
    jae .Los_input
    leaq os_command(%rip), %rdx
    movb %r13b, (%rdx,%rax)
    incl %eax
    movl %eax, os_command_length(%rip)
    movzbl %r13b, %edi
    call os_putc
    jmp .Los_input

.Los_backspace:
    movl os_command_length(%rip), %eax
    testl %eax, %eax
    jz .Los_input
    decl %eax
    movl %eax, os_command_length(%rip)
    leaq os_command(%rip), %rdx
    movb $0, (%rdx,%rax)
    movl $8, %edi
    call os_putc
    movl $32, %edi
    call os_putc
    movl $8, %edi
    call os_putc
    jmp .Los_input

.Los_enter:
    movl os_command_length(%rip), %eax
    leaq os_command(%rip), %rdx
    movb $0, (%rdx,%rax)
    movl $10, %edi
    call os_putc

    leaq os_command(%rip), %rdi
    leaq .Los_cmd_help(%rip), %rsi
    call os_match
    testl %eax, %eax
    jnz .Los_help

    leaq os_command(%rip), %rdi
    leaq .Los_cmd_about(%rip), %rsi
    call os_match
    testl %eax, %eax
    jnz .Los_about

    leaq os_command(%rip), %rdi
    leaq .Los_cmd_clear(%rip), %rsi
    call os_match
    testl %eax, %eax
    jnz .Los_clear

    leaq os_command(%rip), %rdi
    leaq .Los_cmd_mem(%rip), %rsi
    call os_match
    testl %eax, %eax
    jnz .Los_mem

    leaq os_command(%rip), %rdi
    leaq .Los_cmd_reboot(%rip), %rsi
    call os_match
    testl %eax, %eax
    jnz .Los_reboot

    leaq os_command(%rip), %rdi
    leaq .Los_cmd_halt(%rip), %rsi
    call os_match
    testl %eax, %eax
    jnz .Los_halt

    /* Match "echo " exactly, then print the remaining argument bytes. */
    leaq os_command(%rip), %rdi
    leaq .Los_echo_prefix(%rip), %rsi
    movl $5, %ecx
.Los_echo_prefix_check:
    movb (%rdi), %al
    cmpb (%rsi), %al
    jne .Los_unknown_command
    incq %rdi
    incq %rsi
    loop .Los_echo_prefix_check
    call os_puts
    jmp .Los_next_prompt

.Los_unknown_command:
    cmpl $0, os_command_length(%rip)
    je .Los_next_prompt
    leaq .Los_unknown(%rip), %rdi
    call os_puts
    jmp .Los_next_prompt

.Los_help:
    leaq .Los_help_text(%rip), %rdi
    call os_puts
    jmp .Los_next_prompt
.Los_about:
    leaq .Los_about_text(%rip), %rdi
    call os_puts
    jmp .Los_next_prompt
.Los_mem:
    leaq .Los_mem_start(%rip), %rdi
    call os_puts
    leaq __kernel_start(%rip), %rdi
    call os_put_hex64
    leaq .Los_mem_end(%rip), %rdi
    call os_puts
    leaq __kernel_end(%rip), %rdi
    call os_put_hex64
    leaq .Los_mem_map(%rip), %rdi
    call os_puts
    jmp .Los_next_prompt
.Los_clear:
    leaq .Los_clear_text(%rip), %rdi
    call os_puts
    jmp .Los_next_prompt
.Los_halt:
    leaq .Los_halt_text(%rip), %rdi
    call os_puts
.Los_halt_wait:
    cli
    hlt
    jmp .Los_halt_wait
.Los_reboot:
    leaq .Los_reboot_text(%rip), %rdi
    call os_puts
.Los_reboot_wait:
    inb $0x64, %al
    testb $2, %al
    jnz .Los_reboot_wait
    movb $0xfe, %al
    outb %al, $0x64
    jmp .Los_reboot_wait

.Los_next_prompt:
    movl $0, os_command_length(%rip)
    leaq os_command(%rip), %rdx
    movb $0, (%rdx)
    leaq .Los_prompt(%rip), %rdi
    call os_puts
    jmp .Los_input
.size os_shell, .-os_shell

.type os_serial_init,@function
os_serial_init:
    movw $0x3f9, %dx
    xorb %al, %al
    outb %al, %dx
    movw $0x3fb, %dx
    movb $0x80, %al
    outb %al, %dx
    movw $0x3f8, %dx
    movb $1, %al
    outb %al, %dx
    movw $0x3f9, %dx
    xorb %al, %al
    outb %al, %dx
    movw $0x3fb, %dx
    movb $3, %al
    outb %al, %dx
    movw $0x3fa, %dx
    movb $0xc7, %al
    outb %al, %dx
    movw $0x3fc, %dx
    movb $0x0b, %al
    outb %al, %dx
    ret

.type os_puts,@function
os_puts:
    pushq %r12
    movq %rdi, %r12
.Los_puts_loop:
    movzbl (%r12), %edi
    testl %edi, %edi
    jz .Los_puts_done
    call os_putc
    incq %r12
    jmp .Los_puts_loop
.Los_puts_done:
    popq %r12
    ret

.type os_putc,@function
os_putc:
    pushq %rbx
    pushq %r12
    movb %dil, %r12b
    movw $0x3fd, %dx
.Los_serial_wait:
    inb %dx, %al
    testb $0x20, %al
    jz .Los_serial_wait
    movw $0x3f8, %dx
    movb %r12b, %al
    outb %al, %dx
    popq %r12
    popq %rbx
    ret

.type os_put_hex64,@function
os_put_hex64:
    pushq %rbx
    pushq %r12
    movq %rdi, %r12
    movl 
.Los_match_loop:
    movzbl (%rdi), %eax
    movzbl (%rsi), %edx
    cmpl %edx, %eax
    jne .Los_match_false
    testl %eax, %eax
    je .Los_match_true
    incq %rdi
    incq %rsi
    jmp .Los_match_loop
.Los_match_true:
    movl $1, %eax
    ret
.Los_match_false:
    xorl %eax, %eax
    ret
.type os_match,@function
os_match:
.Los_match_loop:
    movzbl (%rdi), %eax
    movzbl (%rsi), %edx
    cmpl %edx, %eax
    jne .Los_match_false
    testl %eax, %eax
    je .Los_match_true
    incq %rdi
    incq %rsi
    jmp .Los_match_loop
.Los_match_true:
    movl $1, %eax
    ret
.Los_match_false:
    xorl %eax, %eax
    ret

.section .rodata
.Los_welcome:
    .asciz "VNTOS_BOOT_OK\r\nVNTOS - freestanding x86-64 serial shell\r\nType help for commands.\r\n"
.Los_prompt: .asciz "VNTOS> "
.Los_ready: .asciz "VNTOS_COMMAND_LINE_READY\r\n"
.Los_cmd_help: .asciz "help"
.Los_cmd_about: .asciz "about"
.Los_cmd_clear: .asciz "clear"
.Los_cmd_reboot: .asciz "reboot"
.Los_cmd_halt: .asciz "halt"
.Los_cmd_mem: .asciz "mem"
.Los_echo_prefix: .asciz "echo "
.Los_help_text: .asciz "Commands: help, about, clear, echo, mem, reboot, halt\r\n"
.Los_about_text: .asciz "VNTOS: VNT-compiled freestanding kernel on x86-64.\r\n"
.Los_mem_start: .asciz "Kernel start: "
.Los_mem_end: .asciz "Kernel end: "
.Los_mem_map: .asciz "Identity map: 0x0000000000000000-0x000000003fffffff (1 GiB)\r\n"
.Los_clear_text: .asciz "\033[2J\033[H"
.Los_unknown: .asciz "Unknown command. Type help.\r\n"
.Los_reboot_text: .asciz "Rebooting...\r\n"
.Los_halt_text: .asciz "CPU halted.\r\n"

.section .bss
.balign 4096
pml4_table: .skip 4096
pdpt_table: .skip 4096
page_directory: .skip 4096
.balign 16
boot_stack_bottom: .skip 16384
boot_stack_top:
os_command_length: .long 0
os_command: .skip 128
