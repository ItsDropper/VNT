.text
.globl vnt_fn_is_prime
vnt_fn_is_prime:
    pushq %rbp
    movq %rsp,%rbp
    subq $16,%rsp
    movq %rcx,-8(%rbp)
    movq -8(%rbp),%rax
    pushq %rax
    movl $2,%ecx
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
    jz .L0
    movl $0,%ecx
    subq $32, %rsp
    call vnt_bool
    addq $32, %rsp
    leave
    ret
    jmp .L1
.L0:
.L1:
    movl $2,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,-16(%rbp)
.L2:
    movq -16(%rbp),%rax
    pushq %rax
    movq -16(%rbp),%rax
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_mul
    addq $32, %rsp
    pushq %rax
    movq -8(%rbp),%rax
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_le
    addq $32, %rsp
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L3
    movq -8(%rbp),%rax
    pushq %rax
    movq -16(%rbp),%rax
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_mod
    addq $32, %rsp
    pushq %rax
    movl $0,%ecx
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
    jz .L4
    movl $0,%ecx
    subq $32, %rsp
    call vnt_bool
    addq $32, %rsp
    leave
    ret
    jmp .L5
.L4:
.L5:
    movq -16(%rbp),%rax
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
    movq %rax,-16(%rbp)
    jmp .L2
.L3:
    movl $1,%ecx
    subq $32, %rsp
    call vnt_bool
    addq $32, %rsp
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_fibonacci
vnt_fn_fibonacci:
    pushq %rbp
    movq %rsp,%rbp
    subq $48,%rsp
    movq %rcx,-8(%rbp)
    movq -8(%rbp),%rax
    pushq %rax
    movl $1,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_le
    addq $32, %rsp
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L6
    movq -8(%rbp),%rax
    leave
    ret
    jmp .L7
.L6:
.L7:
    movl $0,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,-16(%rbp)
    movl $1,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,-24(%rbp)
    movl $2,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,-32(%rbp)
.L8:
    movq -32(%rbp),%rax
    pushq %rax
    movq -8(%rbp),%rax
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_le
    addq $32, %rsp
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L9
    movq -16(%rbp),%rax
    pushq %rax
    movq -24(%rbp),%rax
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_add
    addq $32, %rsp
    movq %rax,-40(%rbp)
    movq -24(%rbp),%rax
    movq %rax,-16(%rbp)
    movq -40(%rbp),%rax
    movq %rax,-24(%rbp)
    movq -32(%rbp),%rax
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
    movq %rax,-32(%rbp)
    jmp .L8
.L9:
    movq -24(%rbp),%rax
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
.L10:
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
    jz .L11
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
    jmp .L10
.L11:
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
    subq $96,%rsp
    subq $32, %rsp
    call vnt_array_new
    addq $32, %rsp
    pushq %rax
    movl $2,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_array_push
    addq $32, %rsp
    pushq %rax
    movl $3,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_array_push
    addq $32, %rsp
    pushq %rax
    movl $4,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_array_push
    addq $32, %rsp
    pushq %rax
    movl $5,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_array_push
    addq $32, %rsp
    pushq %rax
    movl $6,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_array_push
    addq $32, %rsp
    pushq %rax
    movl $7,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_array_push
    addq $32, %rsp
    pushq %rax
    movl $8,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_array_push
    addq $32, %rsp
    pushq %rax
    movl $9,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_array_push
    addq $32, %rsp
    pushq %rax
    movl $10,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_array_push
    addq $32, %rsp
    pushq %rax
    movl $11,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_array_push
    addq $32, %rsp
    pushq %rax
    movl $12,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_array_push
    addq $32, %rsp
    pushq %rax
    movl $13,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_array_push
    addq $32, %rsp
    pushq %rax
    movl $14,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_array_push
    addq $32, %rsp
    pushq %rax
    movl $15,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_array_push
    addq $32, %rsp
    pushq %rax
    movl $16,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_array_push
    addq $32, %rsp
    pushq %rax
    movl $17,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_array_push
    addq $32, %rsp
    pushq %rax
    movl $18,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_array_push
    addq $32, %rsp
    pushq %rax
    movl $19,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_array_push
    addq $32, %rsp
    pushq %rax
    movl $20,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_array_push
    addq $32, %rsp
    pushq %rax
    movl $21,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_array_push
    addq $32, %rsp
    pushq %rax
    movl $22,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_array_push
    addq $32, %rsp
    pushq %rax
    movl $23,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_array_push
    addq $32, %rsp
    pushq %rax
    movl $24,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_array_push
    addq $32, %rsp
    pushq %rax
    movl $25,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_array_push
    addq $32, %rsp
    pushq %rax
    movl $26,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_array_push
    addq $32, %rsp
    pushq %rax
    movl $27,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_array_push
    addq $32, %rsp
    pushq %rax
    movl $28,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_array_push
    addq $32, %rsp
    pushq %rax
    movl $29,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_array_push
    addq $32, %rsp
    pushq %rax
    movl $30,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_array_push
    addq $32, %rsp
    movq %rax,-8(%rbp)
    lea .Lstr12(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr13(%rip),%rcx
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
    movl $0,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,-16(%rbp)
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
    movl $0,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,-40(%rbp)
.L14:
    movq -40(%rbp),%rax
    pushq %rax
    movq -8(%rbp),%rax
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_len
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
    movq -8(%rbp),%rax
    pushq %rax
    movq -40(%rbp),%rax
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_array_get
    addq $32, %rsp
    movq %rax,-48(%rbp)
    movq -48(%rbp),%rax
    pushq %rax
    movq 0(%rsp), %rcx
    subq $8,%rsp
    subq $32,%rsp
    call vnt_fn_is_prime
    addq $32,%rsp
    addq $8,%rsp
    popq %r10
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L16
    movq -16(%rbp),%rax
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
    movq %rax,-16(%rbp)
    movq -24(%rbp),%rax
    pushq %rax
    movq -48(%rbp),%rax
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_add
    addq $32, %rsp
    movq %rax,-24(%rbp)
    movq -48(%rbp),%rax
    movq %rax,-32(%rbp)
    jmp .L17
.L16:
.L17:
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
    jmp .L14
.L15:
    lea .Lstr18(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movq -16(%rbp),%rax
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr19(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movq -24(%rbp),%rax
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr20(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movq -32(%rbp),%rax
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr21(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movl $0,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,-56(%rbp)
.L22:
    movq -56(%rbp),%rax
    pushq %rax
    movl $10,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_le
    addq $32, %rsp
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L23
    movq -56(%rbp),%rax
    pushq %rax
    movq 0(%rsp), %rcx
    subq $8,%rsp
    subq $32,%rsp
    call vnt_fn_fibonacci
    addq $32,%rsp
    addq $8,%rsp
    popq %r10
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
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
    jmp .L22
.L23:
    lea .Lstr24(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movl $1,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,-64(%rbp)
.L25:
    movq -64(%rbp),%rax
    pushq %rax
    movl $7,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_le
    addq $32, %rsp
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L26
    movq -64(%rbp),%rax
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
    movq -64(%rbp),%rax
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
    movq %rax,-64(%rbp)
    jmp .L25
.L26:
    lea .Lstr27(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movl $1,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,-72(%rbp)
    movl $0,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,-80(%rbp)
.L28:
    movq -72(%rbp),%rax
    pushq %rax
    movl $5,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_le
    addq $32, %rsp
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L29
    movl $1,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,-88(%rbp)
.L30:
    movq -88(%rbp),%rax
    pushq %rax
    movl $5,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_le
    addq $32, %rsp
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L31
    movq -72(%rbp),%rax
    pushq %rax
    movq -88(%rbp),%rax
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_mul
    addq $32, %rsp
    movq %rax,-96(%rbp)
    movq -96(%rbp),%rax
    pushq %rax
    movl $2,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_mod
    addq $32, %rsp
    pushq %rax
    movl $0,%ecx
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
    jz .L32
    movq -80(%rbp),%rax
    pushq %rax
    movq -96(%rbp),%rax
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_add
    addq $32, %rsp
    movq %rax,-80(%rbp)
    jmp .L33
.L32:
.L33:
    movq -88(%rbp),%rax
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
    movq %rax,-88(%rbp)
    jmp .L30
.L31:
    movq -72(%rbp),%rax
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
    movq %rax,-72(%rbp)
    jmp .L28
.L29:
    lea .Lstr34(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movq -80(%rbp),%rax
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr35(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
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
    testl %eax,%eax
    jz .L36
    movq -32(%rbp),%rax
    pushq %rax
    movl $20,%ecx
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
    jmp .L37
.L36:
    movl $0,%eax
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_bool
    addq $32, %rsp
.L37:
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movq -16(%rbp),%rax
    pushq %rax
    movl $0,%ecx
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
    jnz .L38
    movq -24(%rbp),%rax
    pushq %rax
    movl $100,%ecx
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
    jmp .L39
.L38:
    movl $1,%eax
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_bool
    addq $32, %rsp
.L39:
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr40(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    xorl %eax,%eax
    leave
    ret

.section .rdata
.Lstr12:
    .asciz "=== VNT Native Showcase ==="
.Lstr13:
    .asciz "Numbers:"
.Lstr18:
    .asciz "Prime count:"
.Lstr19:
    .asciz "Prime sum:"
.Lstr20:
    .asciz "Largest prime:"
.Lstr21:
    .asciz "Fibonacci sequence:"
.Lstr24:
    .asciz "Factorials:"
.Lstr27:
    .asciz "Nested-loop multiplication check:"
.Lstr34:
    .asciz "Even-product checksum:"
.Lstr35:
    .asciz "Short-circuit logic:"
.Lstr40:
    .asciz "=== Done ==="
