/* Copyright (c) 2026 ItsDropper
 * Licensed under the MIT License. See LICENSE in the repository root.
 *
 * The only new assembly dedicated to interrupts is the CPU entry/exit boundary.
 * IDT policy and exception reporting live in VNT. The bootstrap still owns
 * the initial CPU-mode transition in os/boot.S.
 */
.section .text
.code64

.extern vnt_fn_interrupt_dispatch
.global vntos_interrupts_init
.type vntos_interrupts_init,@function

/* No-error-code exception/IRQ stubs normalize the stack to:
 * [vector, error-code, RIP, CS, RFLAGS, ...].
 */
.macro ISR_NOERR number
.global isr\number
isr\number:
    pushq $0
    pushq $\number
    jmp interrupt_common
.endm

/* The CPU already pushed an error code for these exception vectors. */
.macro ISR_ERROR number
.global isr\number
isr\number:
    pushq $\number
    jmp interrupt_common
.endm

ISR_NOERR 0
ISR_NOERR 1
ISR_NOERR 2
ISR_NOERR 3
ISR_NOERR 4
ISR_NOERR 5
ISR_NOERR 6
ISR_NOERR 7
ISR_ERROR 8
ISR_NOERR 9
ISR_ERROR 10
ISR_ERROR 11
ISR_ERROR 12
ISR_ERROR 13
ISR_ERROR 14
ISR_NOERR 15
ISR_NOERR 16
ISR_ERROR 17
ISR_NOERR 18
ISR_NOERR 19
ISR_NOERR 20
ISR_ERROR 21
ISR_NOERR 22
ISR_NOERR 23
ISR_NOERR 24
ISR_NOERR 25
ISR_NOERR 26
ISR_NOERR 27
ISR_NOERR 28
ISR_ERROR 29
ISR_ERROR 30
ISR_NOERR 31

/* The legacy master/slave PICs are remapped to IDT vectors 32..47. */
ISR_NOERR 32
ISR_NOERR 33
ISR_NOERR 34
ISR_NOERR 35
ISR_NOERR 36
ISR_NOERR 37
ISR_NOERR 38
ISR_NOERR 39
ISR_NOERR 40
ISR_NOERR 41
ISR_NOERR 42
ISR_NOERR 43
ISR_NOERR 44
ISR_NOERR 45
ISR_NOERR 46
ISR_NOERR 47

default_interrupt_stub:
    pushq $0
    pushq $255
    jmp interrupt_common

/* Save every general-purpose register before calling VNT. The interrupt gate
 * clears IF; this avoids a nested timer IRQ while the VNT dispatcher runs.
 * The saved RSP is held in R12, which the x86-64 ABI requires callees to keep.
 */
.type interrupt_common,@function
interrupt_common:
    cld
    pushq %rax
    pushq %rbx
    pushq %rcx
    pushq %rdx
    pushq %rsi
    pushq %rdi
    pushq %rbp
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15

    /* After 15 pushes, vector and error code are at offsets 120 and 128. */
    movq 120(%rsp), %rdi
    movq 128(%rsp), %rsi
    movq %rsp, %r12
    andq $-16, %rsp
    call vnt_fn_interrupt_dispatch
    movq %r12, %rsp

    /* Acknowledge the PIC only for hardware IRQ vectors. */
    movq 120(%rsp), %rax
    cmpq $32, %rax
    jb .Lno_pic_eoi
    cmpq $48, %rax
    jae .Lno_pic_eoi
    cmpq $40, %rax
    jb .Lmaster_eoi
    movb $0x20, %al
    outb %al, $0xa0
.Lmaster_eoi:
    movb $0x20, %al
    outb %al, $0x20
.Lno_pic_eoi:
    popq %r15
    popq %r14
    popq %r13
    popq %r12
    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rbp
    popq %rdi
    popq %rsi
    popq %rdx
    popq %rcx
    popq %rbx
    popq %rax
    addq $16, %rsp
    iretq

/* Build 256 present IDT gates. Unknown vectors point at a diagnostic default
 * stub; the 48 exception/PIC entries below override their gates.
 */
vntos_interrupts_init:
    leaq idt_table(%rip), %rdi
    leaq default_interrupt_stub(%rip), %rdx
    movl $256, %ecx
.Lfill_idt:
    movw %dx, 0(%rdi)
    movw $0x08, 2(%rdi)
    movb $0, 4(%rdi)
    movb $0x8e, 5(%rdi)
    movq %rdx, %rax
    shrq $16, %rax
    movw %ax, 6(%rdi)
    movq %rdx, %rax
    shrq $32, %rax
    movl %eax, 8(%rdi)
    movl $0, 12(%rdi)
    addq $16, %rdi
    loop .Lfill_idt

    /* Replace IDT vectors 0..47 with their dedicated entry stubs. */
    leaq isr_stub_table(%rip), %rsi
    xorl %ecx, %ecx
.Linstall_stubs:
    leaq idt_table(%rip), %rdi
    movq %rcx, %r8
    shlq $4, %r8
    addq %r8, %rdi
    movq (%rsi,%rcx,8), %rdx
    movw %dx, 0(%rdi)
    movw $0x08, 2(%rdi)
    movb $0, 4(%rdi)
    movb $0x8e, 5(%rdi)
    movq %rdx, %rax
    shrq $16, %rax
    movw %ax, 6(%rdi)
    movq %rdx, %rax
    shrq $32, %rax
    movl %eax, 8(%rdi)
    movl $0, 12(%rdi)
    incl %ecx
    cmpl $48, %ecx
    jne .Linstall_stubs

    lidt idt_pointer(%rip)

    /* Remap the 8259 PIC away from CPU exception vectors 0..31. */
    movb $0x11, %al
    outb %al, $0x20
    outb %al, $0xa0
    movb $0x20, %al
    outb %al, $0x21
    movb $0x28, %al
    outb %al, $0xa1
    movb $0x04, %al
    outb %al, $0x21
    movb $0x02, %al
    outb %al, $0xa1
    movb $0x01, %al
    outb %al, $0x21
    outb %al, $0xa1

    /* Program PIT channel 0 to approximately 100 ticks per second. */
    movb $0x36, %al
    outb %al, $0x43
    movw $11931, %ax
    outb %al, $0x40
    movb %ah, %al
    outb %al, $0x40

    /* Enable only IRQ0 (PIT); leave keyboard and other IRQs masked. */
    movb $0xfe, %al
    outb %al, $0x21
    movb $0xff, %al
    outb %al, $0xa1
    sti
    ret
.size vntos_interrupts_init, .-vntos_interrupts_init

.section .rodata
.balign 8
isr_stub_table:
    .quad isr0, isr1, isr2, isr3, isr4, isr5, isr6, isr7
    .quad isr8, isr9, isr10, isr11, isr12, isr13, isr14, isr15
    .quad isr16, isr17, isr18, isr19, isr20, isr21, isr22, isr23
    .quad isr24, isr25, isr26, isr27, isr28, isr29, isr30, isr31
    .quad isr32, isr33, isr34, isr35, isr36, isr37, isr38, isr39
    .quad isr40, isr41, isr42, isr43, isr44, isr45, isr46, isr47

.section .data
.balign 16
idt_pointer:
    .word 4095
    .quad idt_table

.section .bss
.balign 4096
idt_table:
    .skip 4096

.section .note.GNU-stack,"",@progbits
