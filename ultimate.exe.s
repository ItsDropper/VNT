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

.globl vnt_fn_mix
vnt_fn_mix:
    pushq %rbp
    movq %rsp,%rbp
    subq $32,%rsp
    movq %rcx,-8(%rbp)
    movq %rdx,-16(%rbp)
    movq %r8,-24(%rbp)
    movq %r9,-32(%rbp)
    movq -8(%rbp),%rax
    pushq %rax
    movq -16(%rbp),%rax
    pushq %rax
    movq -24(%rbp),%rax
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_mul
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_add
    addq $32, %rsp
    pushq %rax
    movq -32(%rbp),%rax
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_sub
    addq $32, %rsp
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_factorial
vnt_fn_factorial:
    pushq %rbp
    movq %rsp,%rbp
    subq $16,%rsp
    movq %rcx,-16(%rbp)
    movl $1,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,-8(%rbp)
.L0:
    movq -16(%rbp),%rax
    pushq %rax
    movl $1,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_gt
    addq $32, %rsp
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L1
    movq -8(%rbp),%rax
    pushq %rax
    movq -16(%rbp),%rax
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_mul
    addq $32, %rsp
    movq %rax,-8(%rbp)
    movq -16(%rbp),%rax
    pushq %rax
    movl $1,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_sub
    addq $32, %rsp
    movq %rax,-16(%rbp)
    jmp .L0
.L1:
    movq -8(%rbp),%rax
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl main
main:
    pushq %rbp
    movq %rsp,%rbp
    subq $64,%rsp
    movl $10,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,-8(%rbp)
    movl $3,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,-16(%rbp)
    movq -8(%rbp),%rax
    pushq %rax
    movq -16(%rbp),%rax
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_add
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movq -8(%rbp),%rax
    pushq %rax
    movq -16(%rbp),%rax
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_sub
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movq -8(%rbp),%rax
    pushq %rax
    movq -16(%rbp),%rax
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_mul
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movq -8(%rbp),%rax
    pushq %rax
    movq -16(%rbp),%rax
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_div
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movq -8(%rbp),%rax
    pushq %rax
    movq -16(%rbp),%rax
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_mod
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movq -8(%rbp),%rax
    pushq %rax
    movl $5,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_add
    addq $32, %rsp
    movq %rax,-8(%rbp)
    movq -16(%rbp),%rax
    pushq %rax
    movl $2,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_mul
    addq $32, %rsp
    movq %rax,-16(%rbp)
    movq -8(%rbp),%rax
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movq -16(%rbp),%rax
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movq -8(%rbp),%rax
    pushq %rax
    movq -16(%rbp),%rax
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_gt
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movq -8(%rbp),%rax
    pushq %rax
    movq -16(%rbp),%rax
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_lt
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movq -8(%rbp),%rax
    pushq %rax
    movl $15,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_ge
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movq -16(%rbp),%rax
    pushq %rax
    movl $6,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_le
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movq -8(%rbp),%rax
    pushq %rax
    movl $15,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_eq
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movq -8(%rbp),%rax
    pushq %rax
    movq -16(%rbp),%rax
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_ne
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movq -8(%rbp),%rax
    pushq %rax
    movl $0,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_gt
    addq $32, %rsp
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L2
    movq -16(%rbp),%rax
    pushq %rax
    movl $0,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_gt
    addq $32, %rsp
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    movl %eax,%ecx
    movq %rcx,%rax
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_bool
    addq $32, %rsp
    jmp .L3
.L2:
    movl $0,%eax
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_bool
    addq $32, %rsp
.L3:
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movq -8(%rbp),%rax
    pushq %rax
    movl $0,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_lt
    addq $32, %rsp
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jnz .L4
    movq -16(%rbp),%rax
    pushq %rax
    movl $0,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_gt
    addq $32, %rsp
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    movl %eax,%ecx
    movq %rcx,%rax
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_bool
    addq $32, %rsp
    jmp .L5
.L4:
    movl $1,%eax
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_bool
    addq $32, %rsp
.L5:
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movl $0,%ecx
    subq $32, %rsp
    call vnt_bool
    addq $32, %rsp
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_not
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movq -8(%rbp),%rax
    pushq %rax
    movl $15,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_eq
    addq $32, %rsp
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L6
    movl $100,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    jmp .L7
.L6:
    movl $200,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
.L7:
    movl $20,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $22,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 0(%rsp), %rdx
    movq 8(%rsp), %rcx
    subq $32,%rsp
    call vnt_fn_add
    addq $32,%rsp
    popq %r10
    popq %r10
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movl $2,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $3,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $4,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $5,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 0(%rsp), %r9
    movq 8(%rsp), %r8
    movq 16(%rsp), %rdx
    movq 24(%rsp), %rcx
    subq $32,%rsp
    call vnt_fn_mix
    addq $32,%rsp
    popq %r10
    popq %r10
    popq %r10
    popq %r10
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movl $5,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 0(%rsp), %rcx
    subq $8,%rsp
    subq $32,%rsp
    call vnt_fn_factorial
    addq $32,%rsp
    addq $8,%rsp
    popq %r10
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movl $0,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,-24(%rbp)
    movl $0,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,-32(%rbp)
.L8:
    movq -24(%rbp),%rax
    pushq %rax
    movl $10,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_lt
    addq $32, %rsp
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L9
    movq -24(%rbp),%rax
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
    movq %rax,-24(%rbp)
    movq -24(%rbp),%rax
    pushq %rax
    movl $3,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_eq
    addq $32, %rsp
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L10
    jmp .L8
    jmp .L11
.L10:
.L11:
    movq -32(%rbp),%rax
    pushq %rax
    movq -24(%rbp),%rax
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_add
    addq $32, %rsp
    movq %rax,-32(%rbp)
    movq -24(%rbp),%rax
    pushq %rax
    movl $5,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_eq
    addq $32, %rsp
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L12
    jmp .L9
    jmp .L13
.L12:
.L13:
    jmp .L8
.L9:
    movq -32(%rbp),%rax
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movl $0,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,-40(%rbp)
    movl $0,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,-48(%rbp)
.L14:
    movq -40(%rbp),%rax
    pushq %rax
    movl $3,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_lt
    addq $32, %rsp
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L15
    movq -40(%rbp),%rax
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
    movq %rax,-40(%rbp)
    movl $0,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,-56(%rbp)
.L16:
    movq -56(%rbp),%rax
    pushq %rax
    movq -40(%rbp),%rax
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_lt
    addq $32, %rsp
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L17
    movq -56(%rbp),%rax
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
    movq %rax,-56(%rbp)
    movq -48(%rbp),%rax
    pushq %rax
    movq -56(%rbp),%rax
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_add
    addq $32, %rsp
    movq %rax,-48(%rbp)
    jmp .L16
.L17:
    jmp .L14
.L15:
    movq -48(%rbp),%rax
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    xorl %eax,%eax
    leave
    ret
