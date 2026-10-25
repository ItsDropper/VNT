/*
 * Copyright (c) 2026 ItsDropper
 * Licensed under the MIT License. See LICENSE in the repository root.
 *
 * Temporary x86-64 shell implementation. The low-level mode transition and
 * page-table bootstrap live in boot.S; shell behavior is being migrated to VNT.
 */
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
    /* Keep RSP 16-byte aligned before calls under the SysV ABI. */
    subq $8, %rsp
    pushq %rbx
    pushq %r12
    movq %rdi, %r12
    movl $'0', %edi
    call os_putc
    movl $'x', %edi
    call os_putc
    movl $60, %ebx
.Los_hex_loop:
    movq %r12, %rax
    movl %ebx, %ecx
    shrq %cl, %rax
    andl $15, %eax
    cmpl $9, %eax
    ja .Los_hex_alpha
    addl $48, %eax
    jmp .Los_hex_emit
.Los_hex_alpha:
    addl $87, %eax
.Los_hex_emit:
    movl %eax, %edi
    call os_putc
    subl $4, %ebx
    jns .Los_hex_loop
    movl $13, %edi
    call os_putc
    movl $10, %edi
    call os_putc
    popq %r12
    popq %rbx
    addq $8, %rsp
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
.balign 16
os_command_length: .long 0
os_command: .skip 128
