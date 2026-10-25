.text
.globl vnt_fn_accept_float
vnt_fn_accept_float:
    pushq %rbp
    movq %rsp,%rbp
    subq $16,%rsp
    movq %rcx,-8(%rbp)
    movq -8(%rbp),%rax
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_integer_result
vnt_fn_integer_result:
    pushq %rbp
    movq %rsp,%rbp
    movsd .Lflt0(%rip),%xmm0
    subq $32,%rsp
    call vnt_float
    addq $32,%rsp
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl main
main:
    pushq %rbp
    movq %rsp,%rbp
    movl $2,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_fn_accept_float
    addq $32,%rsp
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_print
    addq $32,%rsp
    subq $32,%rsp
    call vnt_fn_integer_result
    addq $32,%rsp
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_print
    addq $32,%rsp
    lea .Lstr1(%rip),%rcx
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
.Lflt0:
    .double 3
.Lstr1:
    .asciz "NUMERIC_COMPATIBILITY_OK"
