.text
.globl main
main:
    pushq %rbp
    movq %rsp,%rbp
    subq $48,%rsp
    movl $0,%eax
    movl %eax,-8(%rbp)
    movl $0,%eax
    movl %eax,-16(%rbp)
    movl $2,%eax
    movl %eax,-24(%rbp)
    movl -24(%rbp),%eax
    pushq %rax
    movl $3,%eax
    movl %eax,%r10d
    popq %rax
    addl %r10d,%eax
    jo .Lvnt_int_add_overflow
    movl %eax,-32(%rbp)
    movl -32(%rbp),%eax
    pushq %rax
    movl $4,%eax
    movl %eax,%r10d
    popq %rax
    imull %r10d,%eax
    jo .Lvnt_int_mul_overflow
    movl %eax,-40(%rbp)
.L0:
    movl -8(%rbp),%eax
    pushq %rax
    movl $100,%eax
    movl %eax,%r10d
    popq %rax
    cmpl %r10d,%eax
    jge .L1
    movl -16(%rbp),%eax
    pushq %rax
    movl -8(%rbp),%eax
    movl %eax,%r10d
    popq %rax
    addl %r10d,%eax
    jo .Lvnt_int_add_overflow
    movl %eax,-16(%rbp)
    movl -8(%rbp),%eax
    pushq %rax
    movl $1,%eax
    movl %eax,%r10d
    popq %rax
    addl %r10d,%eax
    jo .Lvnt_int_add_overflow
    movl %eax,-8(%rbp)
    jmp .L0
.L1:
    movl -16(%rbp),%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movl -40(%rbp),%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr2(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
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
    subq $32, %rsp
    call vnt_int_div_zero
    addq $32, %rsp

.section .rdata
.Lstr2:
    .asciz "NATIVE_INT_INFERENCE_OK"
