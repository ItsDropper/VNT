.text
.globl main
main:
    pushq %rbp
    movq %rsp,%rbp
    subq $96,%rsp
    lea .Lstr0(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
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
    lea .Lstr0(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr2(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr3(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_input
    addq $32, %rsp
    movq %rax,-8(%rbp)
    movl $3,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,-16(%rbp)
    movl $0,%ecx
    subq $32, %rsp
    call vnt_bool
    addq $32, %rsp
    movq %rax,-24(%rbp)
    movl $0,%ecx
    subq $32, %rsp
    call vnt_bool
    addq $32, %rsp
    movq %rax,-32(%rbp)
    movl $0,%ecx
    subq $32, %rsp
    call vnt_bool
    addq $32, %rsp
    movq %rax,-40(%rbp)
    movl $0,%ecx
    subq $32, %rsp
    call vnt_bool
    addq $32, %rsp
    movq %rax,-48(%rbp)
    subq $32, %rsp
    call vnt_array_new
    addq $32, %rsp
    pushq %rax
    lea .Lstr4(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_array_push
    addq $32, %rsp
    pushq %rax
    lea .Lstr5(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_array_push
    addq $32, %rsp
    pushq %rax
    lea .Lstr5(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_array_push
    addq $32, %rsp
    movq %rax,-56(%rbp)
    lea .Lstr2(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr6(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movq -8(%rbp),%rax
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_add
    addq $32, %rsp
    pushq %rax
    lea .Lstr7(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_add
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr2(%rip),%rcx
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
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr9(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
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
    lea .Lstr11(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr2(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
.L12:
    movq -48(%rbp),%rax
    pushq %rax
    movl $0,%ecx
    subq $32, %rsp
    call vnt_bool
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
    jz .L13
    lea .Lstr2(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr14(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr15(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr16(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr17(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr18(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
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
    lea .Lstr2(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
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
    call vnt_input
    addq $32, %rsp
    movq %rax,-64(%rbp)
    movq -64(%rbp),%rax
    pushq %rax
    lea .Lstr23(%rip),%rcx
    subq $32, %rsp
    call vnt_string
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
    jz .L21
    lea .Lstr2(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr24(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr25(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr26(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr2(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movq -24(%rbp),%rax
    pushq %rax
    movl $0,%ecx
    subq $32, %rsp
    call vnt_bool
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
    jz .L27
    lea .Lstr29(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_input
    addq $32, %rsp
    movq %rax,-72(%rbp)
    movq -72(%rbp),%rax
    pushq %rax
    lea .Lstr32(%rip),%rcx
    subq $32, %rsp
    call vnt_string
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
    jz .L30
    movl $1,%ecx
    subq $32, %rsp
    call vnt_bool
    addq $32, %rsp
    movq %rax,-24(%rbp)
    movq -56(%rbp),%rax
    pushq %rax
    movl $1,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    lea .Lstr33(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%r8
    popq %rdx
    popq %rcx
    movq %r8,%r8
    subq $32,%rsp
    call vnt_array_set
    addq $32,%rsp
    lea .Lstr34(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    jmp .L31
.L30:
    lea .Lstr35(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
.L31:
    jmp .L28
.L27:
    lea .Lstr36(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
.L28:
    jmp .L22
.L21:
    movq -64(%rbp),%rax
    pushq %rax
    lea .Lstr39(%rip),%rcx
    subq $32, %rsp
    call vnt_string
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
    jz .L37
    lea .Lstr2(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
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
    lea .Lstr41(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr42(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr2(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movq -24(%rbp),%rax
    pushq %rax
    movl $1,%ecx
    subq $32, %rsp
    call vnt_bool
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
    jz .L43
    movq -32(%rbp),%rax
    pushq %rax
    movl $0,%ecx
    subq $32, %rsp
    call vnt_bool
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
    jz .L45
    lea .Lstr47(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_input
    addq $32, %rsp
    movq %rax,-80(%rbp)
    movq -80(%rbp),%rax
    pushq %rax
    lea .Lstr32(%rip),%rcx
    subq $32, %rsp
    call vnt_string
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
    jz .L48
    movl $1,%ecx
    subq $32, %rsp
    call vnt_bool
    addq $32, %rsp
    movq %rax,-32(%rbp)
    lea .Lstr50(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr51(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr52(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    jmp .L49
.L48:
    lea .Lstr53(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
.L49:
    jmp .L46
.L45:
    lea .Lstr54(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
.L46:
    jmp .L44
.L43:
    lea .Lstr55(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
.L44:
    jmp .L38
.L37:
    movq -64(%rbp),%rax
    pushq %rax
    lea .Lstr58(%rip),%rcx
    subq $32, %rsp
    call vnt_string
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
    jz .L56
    lea .Lstr2(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr59(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movq -32(%rbp),%rax
    pushq %rax
    movl $0,%ecx
    subq $32, %rsp
    call vnt_bool
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
    jz .L60
    lea .Lstr62(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr63(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    jmp .L61
.L60:
    lea .Lstr64(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr2(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr65(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr66(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr2(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr67(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_input
    addq $32, %rsp
    movq %rax,-88(%rbp)
    movq -88(%rbp),%rax
    pushq %rax
    lea .Lstr70(%rip),%rcx
    subq $32, %rsp
    call vnt_string
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
    jz .L68
    movl $1,%ecx
    subq $32, %rsp
    call vnt_bool
    addq $32, %rsp
    movq %rax,-40(%rbp)
    lea .Lstr2(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr71(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr2(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movq -40(%rbp),%rax
    pushq %rax
    movl $1,%ecx
    subq $32, %rsp
    call vnt_bool
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
    jz .L72
    lea .Lstr74(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr75(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr2(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr76(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr77(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr78(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr2(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr79(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr2(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr80(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_input
    addq $32, %rsp
    movq %rax,-96(%rbp)
    movq -96(%rbp),%rax
    pushq %rax
    lea .Lstr83(%rip),%rcx
    subq $32, %rsp
    call vnt_string
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
    jz .L81
    movq -56(%rbp),%rax
    pushq %rax
    movl $2,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    lea .Lstr84(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%r8
    popq %rdx
    popq %rcx
    movq %r8,%r8
    subq $32,%rsp
    call vnt_array_set
    addq $32,%rsp
    lea .Lstr2(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr85(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr86(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr2(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr87(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr88(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr89(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr2(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movl $1,%ecx
    subq $32, %rsp
    call vnt_bool
    addq $32, %rsp
    movq %rax,-48(%rbp)
    jmp .L82
.L81:
    lea .Lstr2(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr90(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
.L82:
    jmp .L73
.L72:
.L73:
    jmp .L69
.L68:
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
    lea .Lstr2(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr91(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr92(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr93(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
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
    movq -16(%rbp),%rax
    pushq %rax
    movl $0,%ecx
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
    jz .L94
    lea .Lstr2(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr96(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr97(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr2(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr98(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    jmp .L13
    jmp .L95
.L94:
.L95:
.L69:
.L61:
    jmp .L57
.L56:
    movq -64(%rbp),%rax
    pushq %rax
    lea .Lstr101(%rip),%rcx
    subq $32, %rsp
    call vnt_string
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
    jz .L99
    lea .Lstr2(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr102(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr103(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movq -56(%rbp),%rax
    pushq %rax
    movl $0,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_array_get
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_add
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr104(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movq -56(%rbp),%rax
    pushq %rax
    movl $1,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_array_get
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_add
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr105(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movq -56(%rbp),%rax
    pushq %rax
    movl $2,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_array_get
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_add
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    jmp .L100
.L99:
    movq -64(%rbp),%rax
    pushq %rax
    lea .Lstr108(%rip),%rcx
    subq $32, %rsp
    call vnt_string
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
    jz .L106
    lea .Lstr2(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr109(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr110(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr2(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr98(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    jmp .L13
    jmp .L107
.L106:
    lea .Lstr2(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr111(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
.L107:
.L100:
.L57:
.L38:
.L22:
    movq -48(%rbp),%rax
    pushq %rax
    movl $1,%ecx
    subq $32, %rsp
    call vnt_bool
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
    jz .L112
    lea .Lstr2(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr114(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr2(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr0(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr115(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr0(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    jmp .L113
.L112:
.L113:
    jmp .L12
.L13:
    lea .Lstr2(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr116(%rip),%rcx
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
    .asciz "================================"
.Lstr1:
    .asciz "         THE LAST SIGNAL"
.Lstr2:
    .asciz ""
.Lstr3:
    .asciz "What is your name? "
.Lstr4:
    .asciz "flashlight"
.Lstr5:
    .asciz "empty"
.Lstr6:
    .asciz "Welcome, "
.Lstr7:
    .asciz "."
.Lstr8:
    .asciz "You wake up inside an abandoned research station."
.Lstr9:
    .asciz "The emergency lights are flashing."
.Lstr10:
    .asciz "Your radio suddenly turns on."
.Lstr11:
    .asciz "Do not open the vault."
.Lstr14:
    .asciz "Where do you want to go?"
.Lstr15:
    .asciz "1 - laboratory"
.Lstr16:
    .asciz "2 - armory"
.Lstr17:
    .asciz "3 - lower level"
.Lstr18:
    .asciz "4 - inventory"
.Lstr19:
    .asciz "5 - quit"
.Lstr20:
    .asciz "> "
.Lstr23:
    .asciz "1"
.Lstr24:
    .asciz "--- LABORATORY ---"
.Lstr25:
    .asciz "Broken computers cover the walls."
.Lstr26:
    .asciz "A blue keycard is sitting on a desk."
.Lstr29:
    .asciz "Take the keycard? "
.Lstr32:
    .asciz "yes"
.Lstr33:
    .asciz "blue keycard"
.Lstr34:
    .asciz "You take the blue keycard."
.Lstr35:
    .asciz "You leave it where it is."
.Lstr36:
    .asciz "There is nothing useful left here."
.Lstr39:
    .asciz "2"
.Lstr40:
    .asciz "--- ARMORY ---"
.Lstr41:
    .asciz "The room has been completely stripped."
.Lstr42:
    .asciz "A large emergency generator is behind a locked panel."
.Lstr47:
    .asciz "Use the blue keycard on the panel? "
.Lstr50:
    .asciz "The panel opens."
.Lstr51:
    .asciz "You activate the emergency generator."
.Lstr52:
    .asciz "The entire station comes alive."
.Lstr53:
    .asciz "You decide not to activate it."
.Lstr54:
    .asciz "The generator is already running."
.Lstr55:
    .asciz "You need a blue keycard."
.Lstr58:
    .asciz "3"
.Lstr59:
    .asciz "--- LOWER LEVEL ---"
.Lstr62:
    .asciz "The elevator has no power."
.Lstr63:
    .asciz "You cannot reach the lower level yet."
.Lstr64:
    .asciz "The elevator takes you underground."
.Lstr65:
    .asciz "A massive vault door stands in front of you."
.Lstr66:
    .asciz "A keypad asks for a four-digit code."
.Lstr67:
    .asciz "Enter the code: "
.Lstr70:
    .asciz "4172"
.Lstr71:
    .asciz "ACCESS GRANTED."
.Lstr74:
    .asciz "The vault opens."
.Lstr75:
    .asciz "Inside is a strange black device."
.Lstr76:
    .asciz "The device displays:"
.Lstr77:
    .asciz "SIGNAL SOURCE: THIS STATION"
.Lstr78:
    .asciz "EXTERNAL CONNECTION: NONE"
.Lstr79:
    .asciz "Something is very wrong."
.Lstr80:
    .asciz "Take the device or leave it? "
.Lstr83:
    .asciz "take"
.Lstr84:
    .asciz "black device"
.Lstr85:
    .asciz "You take the device."
.Lstr86:
    .asciz "Every alarm in the station activates."
.Lstr87:
    .asciz "The radio speaks:"
.Lstr88:
    .asciz "YOU FOUND IT."
.Lstr89:
    .asciz "NOW RUN."
.Lstr90:
    .asciz "You leave the device behind."
.Lstr91:
    .asciz "WRONG CODE."
.Lstr92:
    .asciz "An alarm sounds."
.Lstr93:
    .asciz "Health: "
.Lstr96:
    .asciz "The vault locks permanently."
.Lstr97:
    .asciz "You are trapped underground."
.Lstr98:
    .asciz "========== GAME OVER =========="
.Lstr101:
    .asciz "4"
.Lstr102:
    .asciz "--- INVENTORY ---"
.Lstr103:
    .asciz "Slot 1: "
.Lstr104:
    .asciz "Slot 2: "
.Lstr105:
    .asciz "Slot 3: "
.Lstr108:
    .asciz "5"
.Lstr109:
    .asciz "You abandon the station."
.Lstr110:
    .asciz "The radio keeps transmitting behind you."
.Lstr111:
    .asciz "Invalid choice."
.Lstr114:
    .asciz "You escape into the night."
.Lstr115:
    .asciz "           YOU WIN"
.Lstr116:
    .asciz "Game finished."
