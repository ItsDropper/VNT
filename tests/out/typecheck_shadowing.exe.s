.text
.bss
.globl vnt_global___vnt_symbol_0_value
.comm vnt_global___vnt_symbol_0_value,8,8
.text
.globl vnt_fn_shadow_parameter
vnt_fn_shadow_parameter:
    pushq %rbp
    movq %rsp,%rbp
    subq $16,%rsp
    movq %rcx,-16(%rbp)
    movq -16(%rbp),%rax
    pushq %rax
    movq (%rsp),%r11
    testq %r11,%r11
    jz .L0
    cmpl $1,0(%r11)
    jne .L0
    movl 8(%r11),%r10d
    addl $1,%r10d
    jo .L0
    movl %r10d,%ecx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    jmp .L1
.L0:
    movl $1,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    movq %rax,%rdx
    popq %rcx
    subq $32,%rsp
    call vnt_add
    addq $32,%rsp
.L1:
    movq %rax,-8(%rbp)
    movq -8(%rbp),%rax
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_print
    addq $32,%rsp
    movq -16(%rbp),%rax
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl main
main:
    pushq %rbp
    movq %rsp,%rbp
    subq $32,%rsp
    movl $10,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    movq %rax,vnt_global___vnt_symbol_0_value(%rip)
    movl $15,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    movq %rax,-16(%rbp)
    movl $15,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_print
    addq $32,%rsp
    movl $10,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_print
    addq $32,%rsp
    movl $99,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    movq %rax,-24(%rbp)
    movl $99,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_print
    addq $32,%rsp
    movl $10,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_print
    addq $32,%rsp
    movl $10,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_fn_shadow_parameter
    addq $32,%rsp
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_print
    addq $32,%rsp
    lea .Lstr2(%rip),%rcx
    subq $32,%rsp
    call vnt_string
    addq $32,%rsp
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_print
    addq $32,%rsp
    xorl %eax,%eax
    leave
    ret
.Lvnt_int_add_overflow:
    subq $32,%rsp
    call vnt_int_add_overflow
    addq $32,%rsp
.Lvnt_int_sub_overflow:
    subq $32,%rsp
    call vnt_int_sub_overflow
    addq $32,%rsp
.Lvnt_int_mul_overflow:
    subq $32,%rsp
    call vnt_int_mul_overflow
    addq $32,%rsp
.Lvnt_int_neg_overflow:
    subq $32,%rsp
    call vnt_int_neg_overflow
    addq $32,%rsp
.Lvnt_int_div_zero:
    subq $32,%rsp
    call vnt_int_div_zero
    addq $32,%rsp

.section .rdata
.Lstr2:
    .asciz "SYMBOL_SHADOW_OK"
