.text
.globl vnt_fn_sum8
vnt_fn_sum8:
    pushq %rbp
    movq %rsp,%rbp
    subq $64,%rsp
    movq %rcx,-8(%rbp)
    movq %rdx,-16(%rbp)
    movq %r8,-24(%rbp)
    movq %r9,-32(%rbp)
    movq 48(%rbp),%r10
    movq %r10,-40(%rbp)
    movq 56(%rbp),%r10
    movq %r10,-48(%rbp)
    movq 64(%rbp),%r10
    movq %r10,-56(%rbp)
    movq 72(%rbp),%r10
    movq %r10,-64(%rbp)
    movq -8(%rbp),%rax
    pushq %rax
    movq -16(%rbp),%rax
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_add
    addq $32, %rsp
    pushq %rax
    movq -24(%rbp),%rax
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
    call vnt_add
    addq $32, %rsp
    pushq %rax
    movq -40(%rbp),%rax
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_add
    addq $32, %rsp
    pushq %rax
    movq -48(%rbp),%rax
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_add
    addq $32, %rsp
    pushq %rax
    movq -56(%rbp),%rax
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_add
    addq $32, %rsp
    pushq %rax
    movq -64(%rbp),%rax
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

.globl main
main:
    pushq %rbp
    movq %rsp,%rbp
    subq $32,%rsp
    movl $10,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,-8(%rbp)
    leaq -8(%rbp),%rcx
    subq $32, %rsp
    call vnt_ref
    addq $32, %rsp
    movq %rax,-16(%rbp)
    lea .Lstr0(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movq -16(%rbp),%rax
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_deref
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movq -16(%rbp),%rax
    pushq %rax
    movl $42,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_ref_set
    addq $32, %rsp
    movq -8(%rbp),%rax
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr1(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr2(%rip),%rcx
    subq $32, %rsp
    call vnt_struct_new
    addq $32, %rsp
    movq %rax,-24(%rbp)
    movq -24(%rbp),%rax
    pushq %rax
    movl $12,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%r8
    lea .Lstr3(%rip),%rdx
    popq %rcx
    subq $32,%rsp
    call vnt_object_set
    addq $32,%rsp
    movq -24(%rbp),%rax
    pushq %rax
    movl $30,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%r8
    lea .Lstr4(%rip),%rdx
    popq %rcx
    subq $32,%rsp
    call vnt_object_set
    addq $32,%rsp
    movq -24(%rbp),%rax
    pushq %rax
    lea .Lstr3(%rip),%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_object_get
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movq -24(%rbp),%rax
    pushq %rax
    lea .Lstr4(%rip),%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_object_get
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr5(%rip),%rcx
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
    pushq %rax
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
    movl $6,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $7,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $8,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 56(%rsp), %rcx
    movq 48(%rsp), %rdx
    movq 40(%rsp), %r8
    movq 32(%rsp), %r9
    subq $64,%rsp
    movq 88(%rsp),%r10
    movq %r10,32(%rsp)
    movq 80(%rsp),%r10
    movq %r10,40(%rsp)
    movq 72(%rsp),%r10
    movq %r10,48(%rsp)
    movq 64(%rsp),%r10
    movq %r10,56(%rsp)
    call vnt_fn_sum8
    addq $64,%rsp
    popq %r10
    popq %r10
    popq %r10
    popq %r10
    popq %r10
    popq %r10
    popq %r10
    popq %r10
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr6(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movl $100,%eax
    pushq %rax
    movl $20,%eax
    pushq %rax
    movl $3,%eax
    movl %eax,%r10d
    popq %rax
    imull %r10d,%eax
    movl %eax,%r10d
    popq %rax
    addl %r10d,%eax
    pushq %rax
    movl $5,%eax
    movl %eax,%r10d
    popq %rax
    subl %r10d,%eax
    movl %eax,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movl $80,%eax
    pushq %rax
    movl $4,%eax
    movl %eax,%r10d
    popq %rax
    testl %r10d,%r10d
    jz .Lvnt_int_div_zero
    cltd
    idivl %r10d
    pushq %rax
    movl $17,%eax
    pushq %rax
    movl $5,%eax
    movl %eax,%r10d
    popq %rax
    testl %r10d,%r10d
    jz .Lvnt_int_div_zero
    cltd
    idivl %r10d
    movl %edx,%eax
    movl %eax,%r10d
    popq %rax
    addl %r10d,%eax
    movl %eax,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr7(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr8(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    lea .Lstr9(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    pushq $0
    movq 16(%rsp),%rcx
    movq 8(%rsp),%rdx
    xorl %r8d,%r8d
    xorl %r9d,%r9d
    subq $72,%rsp
    movq $0,32(%rsp)
    movq $0,40(%rsp)
    movq $0,48(%rsp)
    movq $0,56(%rsp)
    movq 72(%rsp),%r10
    movq %r10,64(%rsp)
    call vnt_ffi_int
    addq $72,%rsp
    popq %r10
    popq %r10
    popq %r10
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr10(%rip),%rcx
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
.Lvnt_int_div_zero:
    subq $32, %rsp
    call vnt_int_div_zero
    addq $32, %rsp

.section .rdata
.Lstr0:
    .asciz "Reference:"
.Lstr1:
    .asciz "Struct:"
.Lstr2:
    .asciz "Vec2"
.Lstr3:
    .asciz "x"
.Lstr4:
    .asciz "y"
.Lstr5:
    .asciz "8 args:"
.Lstr6:
    .asciz "Integer fast path:"
.Lstr7:
    .asciz "FFI:"
.Lstr8:
    .asciz "kernel32.dll"
.Lstr9:
    .asciz "GetCurrentProcessId"
.Lstr10:
    .asciz "=== Systems features done ==="
