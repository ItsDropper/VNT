/*
 * Copyright (c) 2026 ItsDropper
 * Licensed under the MIT License. See LICENSE in the repository root.
 *
 * Multiboot2 enters 32-bit protected mode. This bootstrap identity-maps the
 * first GiB with 2 MiB pages, enables long mode, and transfers to VNT's _start.
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

    movl $pdpt_table, %eax
    orl $3, %eax
    movl %eax, pml4_table
    movl $page_directory, %eax
    orl $3, %eax
    movl %eax, pdpt_table

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
    movw $0xe9, %dx
    movb $83, %al
    outb %al, %dx
    call os_serial_init
    call os_clear
    leaq .Los_welcome(%rip), %rdi
    call os_puts
    leaq .Los_prompt(%rip), %rdi
    call os_puts

.Los_keyboard_poll:
    inb $0x64, %al
    testb $1, %al
    jz .Los_keyboard_poll
    inb $0x60, %al
    testb $0x80, %al
    jnz .Los_keyboard_poll
    cmpb $0x1c, %al
    je .Los_enter
    cmpb $0x0e, %al
    je .Los_backspace
    call os_scancode_ascii
    testl %eax, %eax
    jz .Los_keyboard_poll
    movb %al, %r13b
    movl os_command_length(%rip), %eax
    cmpl $63, %eax
    jae .Los_keyboard_poll
    leaq os_command(%rip), %rdx
    movb %r13b, (%rdx,%rax)
    incl %eax
    movl %eax, os_command_length(%rip)
    movzbl %r13b, %edi
    call os_putc
    jmp .Los_keyboard_poll

.Los_backspace:
    movl os_command_length(%rip), %eax
    testl %eax, %eax
    jz .Los_keyboard_poll
    decl %eax
    movl %eax, os_command_length(%rip)
    leaq os_command(%rip), %rdx
    movb $0, (%rdx,%rax)
    movl os_cursor(%rip), %eax
    testl %eax, %eax
    jz .Los_keyboard_poll
    decl %eax
    movl %eax, os_cursor(%rip)
    leaq 0xb8000(,%rax,2), %rdx
    movb $' ', (%rdx)
    movb $0x0f, 1(%rdx)
    jmp .Los_keyboard_poll

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
    leaq .Los_cmd_reboot(%rip), %rsi
    call os_match
    testl %eax, %eax
    jnz .Los_reboot

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
.Los_clear:
    call os_clear
    jmp .Los_next_prompt
.Los_reboot:
    leaq .Los_reboot_text(%rip), %rdi
    call os_puts
.Los_reboot_wait:
    inb $0x64, %al
    testb $2, %al
    jnz .Los_reboot_wait
    movb $0xfe, %al
    outb %al, $0x64
    jmp .Los_keyboard_poll

.Los_next_prompt:
    movl $0, os_command_length(%rip)
    leaq os_command(%rip), %rdx
    movb $0, (%rdx)
    leaq .Los_prompt(%rip), %rdi
    call os_puts
    jmp .Los_keyboard_poll
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

.type os_clear,@function
os_clear:
    movabsq $0xb8000, %rdi
    movl $0x0f200f20, %eax
    movl $1000, %ecx
    rep stosl
    movl $0, os_cursor(%rip)
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
    cmpb $10, %r12b
    je .Los_putc_newline
    movl os_cursor(%rip), %ebx
    cmpl $2000, %ebx
    jb .Los_putc_write
    xorl %ebx, %ebx
.Los_putc_write:
    leaq 0xb8000(,%rbx,2), %rdx
    movb %r12b, (%rdx)
    movb $0x0f, 1(%rdx)
    incl %ebx
    movl %ebx, os_cursor(%rip)
    jmp .Los_putc_serial
.Los_putc_newline:
    movl os_cursor(%rip), %eax
    xorl %edx, %edx
    movl $80, %ecx
    divl %ecx
    incl %eax
    imull $80, %eax
    movl %eax, os_cursor(%rip)
.Los_putc_serial:
    movb %r12b, %r10b
    movw $0x3fd, %dx
.Los_serial_wait:
    inb %dx, %al
    testb $0x20, %al
    jz .Los_serial_wait
    movw $0x3f8, %dx
    movb %r10b, %al
    outb %al, %dx
    popq %r12
    popq %rbx
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

.type os_scancode_ascii,@function
os_scancode_ascii:
    cmpb $0x02, %al
    je .Los_key_1
    cmpb $0x03, %al
    je .Los_key_2
    cmpb $0x04, %al
    je .Los_key_3
    cmpb $0x05, %al
    je .Los_key_4
    cmpb $0x06, %al
    je .Los_key_5
    cmpb $0x07, %al
    je .Los_key_6
    cmpb $0x08, %al
    je .Los_key_7
    cmpb $0x09, %al
    je .Los_key_8
    cmpb $0x0a, %al
    je .Los_key_9
    cmpb $0x0b, %al
    je .Los_key_0
    cmpb $0x10, %al
    je .Los_key_q
    cmpb $0x11, %al
    je .Los_key_w
    cmpb $0x12, %al
    je .Los_key_e
    cmpb $0x13, %al
    je .Los_key_r
    cmpb $0x14, %al
    je .Los_key_t
    cmpb $0x15, %al
    je .Los_key_y
    cmpb $0x16, %al
    je .Los_key_u
    cmpb $0x17, %al
    je .Los_key_i
    cmpb $0x18, %al
    je .Los_key_o
    cmpb $0x19, %al
    je .Los_key_p
    cmpb $0x1e, %al
    je .Los_key_a
    cmpb $0x1f, %al
    je .Los_key_s
    cmpb $0x20, %al
    je .Los_key_d
    cmpb $0x21, %al
    je .Los_key_f
    cmpb $0x22, %al
    je .Los_key_g
    cmpb $0x23, %al
    je .Los_key_h
    cmpb $0x24, %al
    je .Los_key_j
    cmpb $0x25, %al
    je .Los_key_k
    cmpb $0x26, %al
    je .Los_key_l
    cmpb $0x2c, %al
    je .Los_key_z
    cmpb $0x2d, %al
    je .Los_key_x
    cmpb $0x2e, %al
    je .Los_key_c
    cmpb $0x2f, %al
    je .Los_key_v
    cmpb $0x30, %al
    je .Los_key_b
    cmpb $0x31, %al
    je .Los_key_n
    cmpb $0x32, %al
    je .Los_key_m
    cmpb $0x39, %al
    je .Los_key_space
    xorl %eax, %eax
    ret
.Los_key_1: movl $'1', %eax; ret
.Los_key_2: movl $'2', %eax; ret
.Los_key_3: movl $'3', %eax; ret
.Los_key_4: movl $'4', %eax; ret
.Los_key_5: movl $'5', %eax; ret
.Los_key_6: movl $'6', %eax; ret
.Los_key_7: movl $'7', %eax; ret
.Los_key_8: movl $'8', %eax; ret
.Los_key_9: movl $'9', %eax; ret
.Los_key_0: movl $'0', %eax; ret
.Los_key_q: movl $'q', %eax; ret
.Los_key_w: movl $'w', %eax; ret
.Los_key_e: movl $'e', %eax; ret
.Los_key_r: movl $'r', %eax; ret
.Los_key_t: movl $'t', %eax; ret
.Los_key_y: movl $'y', %eax; ret
.Los_key_u: movl $'u', %eax; ret
.Los_key_i: movl $'i', %eax; ret
.Los_key_o: movl $'o', %eax; ret
.Los_key_p: movl $'p', %eax; ret
.Los_key_a: movl $'a', %eax; ret
.Los_key_s: movl $'s', %eax; ret
.Los_key_d: movl $'d', %eax; ret
.Los_key_f: movl $'f', %eax; ret
.Los_key_g: movl $'g', %eax; ret
.Los_key_h: movl $'h', %eax; ret
.Los_key_j: movl $'j', %eax; ret
.Los_key_k: movl $'k', %eax; ret
.Los_key_l: movl $'l', %eax; ret
.Los_key_z: movl $'z', %eax; ret
.Los_key_x: movl $'x', %eax; ret
.Los_key_c: movl $'c', %eax; ret
.Los_key_v: movl $'v', %eax; ret
.Los_key_b: movl $'b', %eax; ret
.Los_key_n: movl $'n', %eax; ret
.Los_key_m: movl $'m', %eax; ret
.Los_key_space: movl $' ', %eax; ret

.section .rodata
.Los_welcome:
    .asciz "VNTOS_BOOT_OK\nVNTOS - tiny x86-64 command shell\nType help for commands.\n"
.Los_prompt:
    .asciz "vntos> "
.Los_cmd_help: .asciz "help"
.Los_cmd_about: .asciz "about"
.Los_cmd_clear: .asciz "clear"
.Los_cmd_reboot: .asciz "reboot"
.Los_help_text:
    .asciz "Commands: help, about, clear, reboot\n"
.Los_about_text:
    .asciz "VNTOS: freestanding x86-64 kernel compiled from VNT.\n"
.Los_unknown:
    .asciz "Unknown command. Type help.\n"
.Los_reboot_text:
    .asciz "Rebooting...\n"

.section .bss
.balign 4096
pml4_table: .skip 4096
.balign 4096
pdpt_table: .skip 4096
.balign 4096
page_directory: .skip 4096
.balign 16
boot_stack_bottom: .skip 16384
boot_stack_top:
.balign 8
os_cursor: .long 0
os_command_length: .long 0
os_command: .skip 64

.section .note.GNU-stack,"",@progbits
