.text
.bss
.globl vnt_global___vnt_symbol_0_value
.comm vnt_global___vnt_symbol_0_value,8,8
.text
.globl main
main:
    pushq %rbp
    movq %rsp,%rbp
    subq $16,%rsp
    movl $1,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    movq %rax,vnt_global___vnt_symbol_0_value(%rip)
    lea .Lstr0(%rip),%rcx
    subq $32,%rsp
    call vnt_string
    addq $32,%rsp
    movq %rax,vnt_global___vnt_symbol_0_value(%rip)
    movq vnt_global___vnt_symbol_0_value(%rip),%rax
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
.Lstr0:
    .asciz "text"
.Lstr1:
    .asciz "BRANCH_MERGE_OK"
