.text
.bss
.globl vnt_global_global_path
.comm vnt_global_global_path,8,8
.globl vnt_global_global_count
.comm vnt_global_global_count,8,8
.text
.globl vnt_fn_read_globals
vnt_fn_read_globals:
    pushq %rbp
    movq %rsp,%rbp
    subq $16,%rsp
    lea .Lstr0(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movq vnt_global_global_path(%rip),%rax
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_add
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movq vnt_global_global_count(%rip),%rax
    pushq %rax
    movl $1,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_add
    addq $32, %rsp
    movq %rax,vnt_global_global_count(%rip)
    movq vnt_global_global_count(%rip),%rax
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
    lea .Lstr1(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,vnt_global_global_path(%rip)
    movl $1,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,vnt_global_global_count(%rip)
    subq $32,%rsp
    call vnt_fn_read_globals
    addq $32,%rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movq vnt_global_global_count(%rip),%rax
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
    .asciz "Journal: "
.Lstr1:
    .asciz "vnt_desk.log"
