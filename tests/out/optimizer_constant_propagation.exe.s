.text
.bss
.globl vnt_global___vnt_symbol_0_base
.comm vnt_global___vnt_symbol_0_base,8,8
.globl vnt_global___vnt_symbol_1_copy
.comm vnt_global___vnt_symbol_1_copy,8,8
.globl vnt_global___vnt_symbol_2_flag
.comm vnt_global___vnt_symbol_2_flag,8,8
.globl vnt_global___vnt_symbol_3_value
.comm vnt_global___vnt_symbol_3_value,8,8
.text
.globl main
main:
    pushq %rbp
    movq %rsp,%rbp
    subq $32,%rsp
    movl $4,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    movq %rax,vnt_global___vnt_symbol_0_base(%rip)
    movl $7,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    movq %rax,vnt_global___vnt_symbol_1_copy(%rip)
    movl $9,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    movq %rax,vnt_global___vnt_symbol_0_base(%rip)
    movl $7,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_print
    addq $32,%rsp
    movl $9,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_print
    addq $32,%rsp
    movl $1,%ecx
    subq $32,%rsp
    call vnt_bool
    addq $32,%rsp
    movq %rax,vnt_global___vnt_symbol_2_flag(%rip)
    lea .Lstr0(%rip),%rcx
    subq $32,%rsp
    call vnt_string
    addq $32,%rsp
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_print
    addq $32,%rsp
    movl $1,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    movq %rax,vnt_global___vnt_symbol_3_value(%rip)
    movl $2,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    movq %rax,vnt_global___vnt_symbol_3_value(%rip)
    movl $2,%ecx
    subq $32,%rsp
    call vnt_int
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
.Lstr0:
    .asciz "CONSTANT_BRANCH"
.Lstr1:
    .asciz "CONSTANT_PROPAGATION_OK"
