.section .rdata,"dr"
.Lvnt_int_fmt:
    .asciz "%d\n"

.text
.globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $16, %rsp
    movl $10, %eax
    movl %eax, -8(%rbp)
    movl $20, %eax
    movl %eax, -16(%rbp)
    movl -8(%rbp), %eax
    pushq %rax
    movl -16(%rbp), %eax
    movl %eax, %ecx
    popq %rax
    addl %ecx, %eax
    movl %eax, %edx
    lea .Lvnt_int_fmt(%rip), %rcx
    subq $32, %rsp
    call printf
    addq $32, %rsp
    movl -8(%rbp), %eax
    pushq %rax
    movl $3, %eax
    movl %eax, %ecx
    popq %rax
    imull %ecx, %eax
    movl %eax, %edx
    lea .Lvnt_int_fmt(%rip), %rcx
    subq $32, %rsp
    call printf
    addq $32, %rsp
    movl -16(%rbp), %eax
    pushq %rax
    movl -8(%rbp), %eax
    movl %eax, %ecx
    popq %rax
    subl %ecx, %eax
    movl %eax, %edx
    lea .Lvnt_int_fmt(%rip), %rcx
    subq $32, %rsp
    call printf
    addq $32, %rsp
    movl -16(%rbp), %eax
    pushq %rax
    movl $2, %eax
    movl %eax, %ecx
    popq %rax
    cltd
    idivl %ecx
    movl %eax, %edx
    lea .Lvnt_int_fmt(%rip), %rcx
    subq $32, %rsp
    call printf
    addq $32, %rsp
    movl -16(%rbp), %eax
    pushq %rax
    movl $3, %eax
    movl %eax, %ecx
    popq %rax
    cltd
    idivl %ecx
    movl %edx, %eax
    movl %eax, %edx
    lea .Lvnt_int_fmt(%rip), %rcx
    subq $32, %rsp
    call printf
    addq $32, %rsp
    movl -8(%rbp), %eax
    pushq %rax
    movl -16(%rbp), %eax
    movl %eax, %ecx
    popq %rax
    cmpl %ecx, %eax
    setg %al
    movzbl %al, %eax
    movl %eax, %edx
    lea .Lvnt_int_fmt(%rip), %rcx
    subq $32, %rsp
    call printf
    addq $32, %rsp
    movl -8(%rbp), %eax
    pushq %rax
    movl -16(%rbp), %eax
    movl %eax, %ecx
    popq %rax
    cmpl %ecx, %eax
    setl %al
    movzbl %al, %eax
    movl %eax, %edx
    lea .Lvnt_int_fmt(%rip), %rcx
    subq $32, %rsp
    call printf
    addq $32, %rsp
    movl -8(%rbp), %eax
    pushq %rax
    movl $10, %eax
    movl %eax, %ecx
    popq %rax
    cmpl %ecx, %eax
    sete %al
    movzbl %al, %eax
    movl %eax, %edx
    lea .Lvnt_int_fmt(%rip), %rcx
    subq $32, %rsp
    call printf
    addq $32, %rsp
    movl $0, %eax
    cmpl $0, %eax
    sete %al
    movzbl %al, %eax
.L0:
    movl %eax, %edx
    lea .Lvnt_int_fmt(%rip), %rcx
    subq $32, %rsp
    call printf
    addq $32, %rsp
    xorl %eax, %eax
    leave
    ret
