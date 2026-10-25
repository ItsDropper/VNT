.text
.globl vnt_fn_add
vnt_fn_add:
    pushq %rbp
    movq %rsp,%rbp
    subq $16,%rsp
    movq %rcx,-8(%rbp)
    movq %rdx,-16(%rbp)
    movq -8(%rbp),%rax
    pushq %rax
    movq -16(%rbp),%rax
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_add
    addq $32, %rsp
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl main
main:
    pushq %rbp
    movq %rsp,%rbp
    subq $16,%rsp
    movl $20,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $22,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32,%rsp
    call vnt_fn_add
    addq $32,%rsp
    movq %rax,-8(%rbp)
    lea .Lstr0(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movq -8(%rbp),%rax
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr1(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movl $65,%ecx
    subq $32, %rsp
    call vnt_int
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
.Lstr0:
    .asciz "Modules:"
.Lstr1:
    .asciz "Optimized:"
