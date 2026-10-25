.text
.bss
.globl vnt_global___vnt_symbol_0_answer
.comm vnt_global___vnt_symbol_0_answer,8,8
.text
.globl vnt_fn_math__multiply
vnt_fn_math__multiply:
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
    testq %rcx,%rcx
    jz .L0
    testq %rdx,%rdx
    jz .L0
    cmpl $1,0(%rcx)
    jne .L0
    cmpl $1,0(%rdx)
    jne .L0
    movl 8(%rcx),%r10d
    imull 8(%rdx),%r10d
    jo .L0
    movl %r10d,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    jmp .L1
.L0:
    subq $32,%rsp
    call vnt_mul
    addq $32,%rsp
.L1:
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
    movl $6,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    pushq %rax
    movl $7,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    pushq %rax
    movq 8(%rsp),%rcx
    movq 0(%rsp),%rdx
    addq $16,%rsp
    subq $32,%rsp
    call vnt_fn_math__multiply
    addq $32,%rsp
    movq %rax,vnt_global___vnt_symbol_0_answer(%rip)
    movq vnt_global___vnt_symbol_0_answer(%rip),%rax
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
    .asciz "NAMESPACED_MODULE_OK"
