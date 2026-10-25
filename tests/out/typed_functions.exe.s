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
    testq %rcx,%rcx
    jz .L0
    testq %rdx,%rdx
    jz .L0
    cmpl $1,0(%rcx)
    jne .L0
    cmpl $1,0(%rdx)
    jne .L0
    movl 8(%rcx),%r10d
    addl 8(%rdx),%r10d
    jo .L0
    movl %r10d,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    jmp .L1
.L0:
    subq $32,%rsp
    call vnt_add
    addq $32,%rsp
.L1:
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_label
vnt_fn_label:
    pushq %rbp
    movq %rsp,%rbp
    subq $16,%rsp
    movq %rcx,-8(%rbp)
    movq -8(%rbp),%rax
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_truth
    addq $32,%rsp
    testl %eax,%eax
    jz .L2
    lea .Lstr4(%rip),%rcx
    subq $32,%rsp
    call vnt_string
    addq $32,%rsp
    leave
    ret
    jmp .L3
.L2:
    lea .Lstr5(%rip),%rcx
    subq $32,%rsp
    call vnt_string
    addq $32,%rsp
    leave
    ret
.L3:
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_circle_area
vnt_fn_circle_area:
    pushq %rbp
    movq %rsp,%rbp
    subq $16,%rsp
    movq %rcx,-8(%rbp)
    movq -8(%rbp),%rax
    pushq %rax
    movq -8(%rbp),%rax
    movq %rax,%rdx
    popq %rcx
    testq %rcx,%rcx
    jz .L6
    testq %rdx,%rdx
    jz .L6
    cmpl $1,0(%rcx)
    jne .L6
    cmpl $1,0(%rdx)
    jne .L6
    movl 8(%rcx),%r10d
    imull 8(%rdx),%r10d
    jo .L6
    movl %r10d,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    jmp .L7
.L6:
    subq $32,%rsp
    call vnt_mul
    addq $32,%rsp
.L7:
    pushq %rax
    movsd .Lflt8(%rip),%xmm0
    subq $32,%rsp
    call vnt_float
    addq $32,%rsp
    movq %rax,%rdx
    popq %rcx
    testq %rcx,%rcx
    jz .L9
    testq %rdx,%rdx
    jz .L9
    cmpl $1,0(%rcx)
    jne .L9
    cmpl $1,0(%rdx)
    jne .L9
    movl 8(%rcx),%r10d
    imull 8(%rdx),%r10d
    jo .L9
    movl %r10d,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    jmp .L10
.L9:
    subq $32,%rsp
    call vnt_mul
    addq $32,%rsp
.L10:
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl main
main:
    pushq %rbp
    movq %rsp,%rbp
    movl $20,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    pushq %rax
    movl $22,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    pushq %rax
    movq 8(%rsp),%rcx
    movq 0(%rsp),%rdx
    addq $16,%rsp
    subq $32,%rsp
    call vnt_fn_add
    addq $32,%rsp
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_print
    addq $32,%rsp
    movl $1,%ecx
    subq $32,%rsp
    call vnt_bool
    addq $32,%rsp
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_fn_label
    addq $32,%rsp
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_print
    addq $32,%rsp
    movsd .Lflt11(%rip),%xmm0
    subq $32,%rsp
    call vnt_float
    addq $32,%rsp
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_fn_circle_area
    addq $32,%rsp
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_print
    addq $32,%rsp
    lea .Lstr12(%rip),%rcx
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
.Lflt8:
    .double 3.1400000000000001
.Lflt11:
    .double 2
.Lstr4:
    .asciz "active"
.Lstr5:
    .asciz "inactive"
.Lstr12:
    .asciz "TYPED_FUNCTIONS_OK"
