.text
.bss
.globl vnt_global_round
.comm vnt_global_round,8,8
.text
.globl main
main:
    pushq %rbp
    movq %rsp,%rbp
    subq $32,%rsp
    movl $0,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    movq %rax,vnt_global_round(%rip)
.L0:
    movq vnt_global_round(%rip),%rax
    movq %rax,%r11
    testq %r11,%r11
    jz .L2
    cmpl $1,0(%r11)
    jne .L2
    movl 8(%r11),%eax
    cmpl $20,%eax
    jge .L1
    jmp .L3
.L2:
    movq vnt_global_round(%rip),%rax
    pushq %rax
    movl $20,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    movq %rax,%rdx
    popq %rcx
    subq $32,%rsp
    call vnt_lt
    addq $32,%rsp
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_truth
    addq $32,%rsp
    testl %eax,%eax
    jz .L1
.L3:
    movl $0,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    movq %rax,-16(%rbp)
    movl $0,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    movq %rax,-24(%rbp)
.L4:
    movq -16(%rbp),%rax
    movq %rax,%r11
    testq %r11,%r11
    jz .L6
    cmpl $1,0(%r11)
    jne .L6
    movl 8(%r11),%eax
    cmpl $40000,%eax
    jge .L5
    jmp .L7
.L6:
    movq -16(%rbp),%rax
    pushq %rax
    movl $40000,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    movq %rax,%rdx
    popq %rcx
    subq $32,%rsp
    call vnt_lt
    addq $32,%rsp
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_truth
    addq $32,%rsp
    testl %eax,%eax
    jz .L5
.L7:
    movq -24(%rbp),%r9
    testq %r9,%r9
    jz .L8
    cmpl $1,0(%r9)
    jne .L8
    movl 8(%r9),%r10d
    movq -16(%rbp),%r11
    testq %r11,%r11
    jz .L8
    cmpl $1,0(%r11)
    jne .L8
    movl 8(%r11),%eax
    addl %eax,%r10d
    jo .L8
    movl %r10d,8(%r9)
    movq %r9,%rax
    jmp .L9
.L8:
    movq -24(%rbp),%rax
    pushq %rax
    movq -16(%rbp),%rax
    movq %rax,%rdx
    popq %rcx
    testq %rcx,%rcx
    jz .L10
    testq %rdx,%rdx
    jz .L10
    cmpl $1,0(%rcx)
    jne .L10
    cmpl $1,0(%rdx)
    jne .L10
    movl 8(%rcx),%r10d
    addl 8(%rdx),%r10d
    jo .L10
    movl %r10d,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    jmp .L11
.L10:
    subq $32,%rsp
    call vnt_add
    addq $32,%rsp
.L11:
.L9:
    movq %rax,-24(%rbp)
    movq -16(%rbp),%r9
    testq %r9,%r9
    jz .L12
    cmpl $1,0(%r9)
    jne .L12
    movl 8(%r9),%r10d
    addl $1,%r10d
    jo .L12
    movl %r10d,8(%r9)
    movq %r9,%rax
    jmp .L13
.L12:
    movq -16(%rbp),%rax
    pushq %rax
    movq (%rsp),%r11
    testq %r11,%r11
    jz .L14
    cmpl $1,0(%r11)
    jne .L14
    movl 8(%r11),%r10d
    addl $1,%r10d
    jo .L14
    movl %r10d,%ecx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    jmp .L15
.L14:
    movl $1,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    movq %rax,%rdx
    popq %rcx
    subq $32,%rsp
    call vnt_add
    addq $32,%rsp
.L15:
.L13:
    movq %rax,-16(%rbp)
    jmp .L4
.L5:
    movq vnt_global_round(%rip),%rax
    pushq %rax
    movq (%rsp),%r11
    testq %r11,%r11
    jz .L16
    cmpl $1,0(%r11)
    jne .L16
    movl 8(%r11),%r10d
    addl $1,%r10d
    jo .L16
    movl %r10d,%ecx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    jmp .L17
.L16:
    movl $1,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    movq %rax,%rdx
    popq %rcx
    subq $32,%rsp
    call vnt_add
    addq $32,%rsp
.L17:
    movq %rax,vnt_global_round(%rip)
    jmp .L0
.L1:
    movq -24(%rbp),%rax
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
