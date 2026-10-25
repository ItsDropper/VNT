.text
.bss
.globl vnt_global___vnt_symbol_0_line
.comm vnt_global___vnt_symbol_0_line,8,8
.globl vnt_global___vnt_symbol_1_write_result
.comm vnt_global___vnt_symbol_1_write_result,8,8
.globl vnt_global___vnt_symbol_2_line_result
.comm vnt_global___vnt_symbol_2_line_result,8,8
.text
.globl vnt_fn_io__write
vnt_fn_io__write:
    pushq %rbp
    movq %rsp,%rbp
    subq $16,%rsp
    movq %rcx,-8(%rbp)
    movq -8(%rbp),%rax
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_io_write
    addq $32,%rsp
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_io__write_line
vnt_fn_io__write_line:
    pushq %rbp
    movq %rsp,%rbp
    subq $16,%rsp
    movq %rcx,-8(%rbp)
    movq -8(%rbp),%rax
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_io_write
    addq $32,%rsp
    lea .Lstr0(%rip),%rcx
    subq $32,%rsp
    call vnt_string
    addq $32,%rsp
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_io_write
    addq $32,%rsp
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_io__write_error
vnt_fn_io__write_error:
    pushq %rbp
    movq %rsp,%rbp
    subq $16,%rsp
    movq %rcx,-8(%rbp)
    movq -8(%rbp),%rax
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_io_write_error
    addq $32,%rsp
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_io__read_line
vnt_fn_io__read_line:
    pushq %rbp
    movq %rsp,%rbp
    subq $16,%rsp
    movq %rcx,-8(%rbp)
    movq -8(%rbp),%rax
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_input
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
    subq $32,%rsp
    lea .Lstr1(%rip),%rcx
    subq $32,%rsp
    call vnt_string
    addq $32,%rsp
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_fn_io__read_line
    addq $32,%rsp
    movq %rax,vnt_global___vnt_symbol_0_line(%rip)
    lea .Lstr2(%rip),%rcx
    subq $32,%rsp
    call vnt_string
    addq $32,%rsp
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_fn_io__write_line
    addq $32,%rsp
    movq %rax,vnt_global___vnt_symbol_1_write_result(%rip)
    movq vnt_global___vnt_symbol_0_line(%rip),%rax
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_fn_io__write_line
    addq $32,%rsp
    movq %rax,vnt_global___vnt_symbol_2_line_result(%rip)
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
    .asciz "\n"
.Lstr1:
    .asciz ""
.Lstr2:
    .asciz "IO_OK"
