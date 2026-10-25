.text
.bss
.globl vnt_global___vnt_symbol_0_test_global
.comm vnt_global___vnt_symbol_0_test_global,8,8
.text
.globl vnt_fn_factorial
vnt_fn_factorial:
    pushq %rbp
    movq %rsp,%rbp
    subq $16,%rsp
    movq %rcx,-8(%rbp)
    movq -8(%rbp),%rax
    movq %rax,%r11
    testq %r11,%r11
    jz .L2
    cmpl $1,0(%r11)
    jne .L2
    movl 8(%r11),%eax
    cmpl $1,%eax
    jg .L0
    jmp .L3
.L2:
    movq -8(%rbp),%rax
    pushq %rax
    movl $1,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    movq %rax,%rdx
    popq %rcx
    subq $32,%rsp
    call vnt_le
    addq $32,%rsp
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_truth
    addq $32,%rsp
    testl %eax,%eax
    jz .L0
.L3:
    movl $1,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    leave
    ret
    jmp .L1
.L0:
    movq -8(%rbp),%rax
    pushq %rax
    movq -8(%rbp),%rax
    pushq %rax
    movq (%rsp),%r11
    testq %r11,%r11
    jz .L4
    cmpl $1,0(%r11)
    jne .L4
    movl 8(%r11),%r10d
    subl $1,%r10d
    jo .L4
    movl %r10d,%ecx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    jmp .L5
.L4:
    movl $1,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    movq %rax,%rdx
    popq %rcx
    subq $32,%rsp
    call vnt_sub
    addq $32,%rsp
.L5:
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_fn_factorial
    addq $32,%rsp
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
    leave
    ret
.L1:
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_fibonacci
vnt_fn_fibonacci:
    pushq %rbp
    movq %rsp,%rbp
    subq $16,%rsp
    movq %rcx,-8(%rbp)
    movq -8(%rbp),%rax
    movq %rax,%r11
    testq %r11,%r11
    jz .L10
    cmpl $1,0(%r11)
    jne .L10
    movl 8(%r11),%eax
    cmpl $1,%eax
    jg .L8
    jmp .L11
.L10:
    movq -8(%rbp),%rax
    pushq %rax
    movl $1,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    movq %rax,%rdx
    popq %rcx
    subq $32,%rsp
    call vnt_le
    addq $32,%rsp
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_truth
    addq $32,%rsp
    testl %eax,%eax
    jz .L8
.L11:
    movq -8(%rbp),%rax
    leave
    ret
    jmp .L9
.L8:
    movq -8(%rbp),%rax
    pushq %rax
    movq (%rsp),%r11
    testq %r11,%r11
    jz .L12
    cmpl $1,0(%r11)
    jne .L12
    movl 8(%r11),%r10d
    subl $1,%r10d
    jo .L12
    movl %r10d,%ecx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    jmp .L13
.L12:
    movl $1,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    movq %rax,%rdx
    popq %rcx
    subq $32,%rsp
    call vnt_sub
    addq $32,%rsp
.L13:
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_fn_fibonacci
    addq $32,%rsp
    pushq %rax
    movq -8(%rbp),%rax
    pushq %rax
    movq (%rsp),%r11
    testq %r11,%r11
    jz .L14
    cmpl $1,0(%r11)
    jne .L14
    movl 8(%r11),%r10d
    subl $2,%r10d
    jo .L14
    movl %r10d,%ecx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    jmp .L15
.L14:
    movl $2,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    movq %rax,%rdx
    popq %rcx
    subq $32,%rsp
    call vnt_sub
    addq $32,%rsp
.L15:
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_fn_fibonacci
    addq $32,%rsp
    movq %rax,%rdx
    popq %rcx
    testq %rcx,%rcx
    jz .L16
    testq %rdx,%rdx
    jz .L16
    cmpl $1,0(%rcx)
    jne .L16
    cmpl $1,0(%rdx)
    jne .L16
    movl 8(%rcx),%r10d
    addl 8(%rdx),%r10d
    jo .L16
    movl %r10d,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    jmp .L17
.L16:
    subq $32,%rsp
    call vnt_add
    addq $32,%rsp
.L17:
    leave
    ret
.L9:
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_sum_to
vnt_fn_sum_to:
    pushq %rbp
    movq %rsp,%rbp
    subq $32,%rsp
    movq %rcx,-24(%rbp)
    movl $0,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    movq %rax,-8(%rbp)
    movl $1,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    movq %rax,-16(%rbp)
.L18:
    movq -16(%rbp),%rax
    pushq %rax
    movq -24(%rbp),%rax
    movq %rax,%rdx
    popq %rcx
    subq $32,%rsp
    call vnt_le
    addq $32,%rsp
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_truth
    addq $32,%rsp
    testl %eax,%eax
    jz .L19
    movq -8(%rbp),%rax
    pushq %rax
    movq -16(%rbp),%rax
    movq %rax,%rdx
    popq %rcx
    testq %rcx,%rcx
    jz .L20
    testq %rdx,%rdx
    jz .L20
    cmpl $1,0(%rcx)
    jne .L20
    cmpl $1,0(%rdx)
    jne .L20
    movl 8(%rcx),%r10d
    addl 8(%rdx),%r10d
    jo .L20
    movl %r10d,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    jmp .L21
.L20:
    subq $32,%rsp
    call vnt_add
    addq $32,%rsp
.L21:
    movq %rax,-8(%rbp)
    movq -16(%rbp),%r9
    testq %r9,%r9
    jz .L22
    cmpl $1,0(%r9)
    jne .L22
    movl 8(%r9),%r10d
    addl $1,%r10d
    jo .L22
    movl %r10d,8(%r9)
    movq %r9,%rax
    jmp .L23
.L22:
    movq -16(%rbp),%rax
    pushq %rax
    movq (%rsp),%r11
    testq %r11,%r11
    jz .L24
    cmpl $1,0(%r11)
    jne .L24
    movl 8(%r11),%r10d
    addl $1,%r10d
    jo .L24
    movl %r10d,%ecx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    jmp .L25
.L24:
    movl $1,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    movq %rax,%rdx
    popq %rcx
    subq $32,%rsp
    call vnt_add
    addq $32,%rsp
.L25:
.L23:
    movq %rax,-16(%rbp)
    jmp .L18
.L19:
    movq -8(%rbp),%rax
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_classify
vnt_fn_classify:
    pushq %rbp
    movq %rsp,%rbp
    subq $16,%rsp
    movq %rcx,-8(%rbp)
    movq -8(%rbp),%rax
    movq %rax,%r11
    testq %r11,%r11
    jz .L28
    cmpl $1,0(%r11)
    jne .L28
    movl 8(%r11),%eax
    cmpl $0,%eax
    jge .L26
    jmp .L29
.L28:
    movq -8(%rbp),%rax
    pushq %rax
    movl $0,%ecx
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
    jz .L26
.L29:
    lea .Lstr30(%rip),%rcx
    subq $32,%rsp
    call vnt_string
    addq $32,%rsp
    leave
    ret
    jmp .L27
.L26:
    movq -8(%rbp),%rax
    movq %rax,%r11
    testq %r11,%r11
    jz .L33
    cmpl $1,0(%r11)
    jne .L33
    movl 8(%r11),%eax
    cmpl $0,%eax
    jne .L31
    jmp .L34
.L33:
    movq -8(%rbp),%rax
    pushq %rax
    movl $0,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    movq %rax,%rdx
    popq %rcx
    subq $32,%rsp
    call vnt_eq
    addq $32,%rsp
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_truth
    addq $32,%rsp
    testl %eax,%eax
    jz .L31
.L34:
    lea .Lstr35(%rip),%rcx
    subq $32,%rsp
    call vnt_string
    addq $32,%rsp
    leave
    ret
    jmp .L32
.L31:
    lea .Lstr36(%rip),%rcx
    subq $32,%rsp
    call vnt_string
    addq $32,%rsp
    leave
    ret
.L32:
.L27:
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_global_increment
vnt_fn_global_increment:
    pushq %rbp
    movq %rsp,%rbp
    subq $16,%rsp
    movq vnt_global___vnt_symbol_0_test_global(%rip),%rax
    pushq %rax
    movq (%rsp),%r11
    testq %r11,%r11
    jz .L37
    cmpl $1,0(%r11)
    jne .L37
    movl 8(%r11),%r10d
    addl $1,%r10d
    jo .L37
    movl %r10d,%ecx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    jmp .L38
.L37:
    movl $1,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    movq %rax,%rdx
    popq %rcx
    subq $32,%rsp
    call vnt_add
    addq $32,%rsp
.L38:
    movq %rax,vnt_global___vnt_symbol_0_test_global(%rip)
    movq vnt_global___vnt_symbol_0_test_global(%rip),%rax
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
    movl $40,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    movq %rax,vnt_global___vnt_symbol_0_test_global(%rip)
    lea .Lstr39(%rip),%rcx
    subq $32,%rsp
    call vnt_string
    addq $32,%rsp
    pushq %rax
    movl $10,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_fn_factorial
    addq $32,%rsp
    movq %rax,%rdx
    popq %rcx
    testq %rcx,%rcx
    jz .L40
    testq %rdx,%rdx
    jz .L40
    cmpl $1,0(%rcx)
    jne .L40
    cmpl $1,0(%rdx)
    jne .L40
    movl 8(%rcx),%r10d
    addl 8(%rdx),%r10d
    jo .L40
    movl %r10d,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    jmp .L41
.L40:
    subq $32,%rsp
    call vnt_add
    addq $32,%rsp
.L41:
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_print
    addq $32,%rsp
    lea .Lstr42(%rip),%rcx
    subq $32,%rsp
    call vnt_string
    addq $32,%rsp
    pushq %rax
    movl $12,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_fn_fibonacci
    addq $32,%rsp
    movq %rax,%rdx
    popq %rcx
    testq %rcx,%rcx
    jz .L43
    testq %rdx,%rdx
    jz .L43
    cmpl $1,0(%rcx)
    jne .L43
    cmpl $1,0(%rdx)
    jne .L43
    movl 8(%rcx),%r10d
    addl 8(%rdx),%r10d
    jo .L43
    movl %r10d,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    jmp .L44
.L43:
    subq $32,%rsp
    call vnt_add
    addq $32,%rsp
.L44:
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_print
    addq $32,%rsp
    lea .Lstr45(%rip),%rcx
    subq $32,%rsp
    call vnt_string
    addq $32,%rsp
    pushq %rax
    movl $100,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_fn_sum_to
    addq $32,%rsp
    movq %rax,%rdx
    popq %rcx
    testq %rcx,%rcx
    jz .L46
    testq %rdx,%rdx
    jz .L46
    cmpl $1,0(%rcx)
    jne .L46
    cmpl $1,0(%rdx)
    jne .L46
    movl 8(%rcx),%r10d
    addl 8(%rdx),%r10d
    jo .L46
    movl %r10d,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    jmp .L47
.L46:
    subq $32,%rsp
    call vnt_add
    addq $32,%rsp
.L47:
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_print
    addq $32,%rsp
    lea .Lstr50(%rip),%rcx
    subq $32,%rsp
    call vnt_string
    addq $32,%rsp
    pushq %rax
    movq (%rsp),%r11
    testq %r11,%r11
    jz .L48
    cmpl $1,0(%r11)
    jne .L48
    movl 8(%r11),%r10d
    addl $14,%r10d
    jo .L48
    movl %r10d,%ecx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    jmp .L49
.L48:
    movl $14,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    movq %rax,%rdx
    popq %rcx
    subq $32,%rsp
    call vnt_add
    addq $32,%rsp
.L49:
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_print
    addq $32,%rsp
    lea .Lstr53(%rip),%rcx
    subq $32,%rsp
    call vnt_string
    addq $32,%rsp
    pushq %rax
    movq (%rsp),%r11
    testq %r11,%r11
    jz .L51
    cmpl $1,0(%r11)
    jne .L51
    movl 8(%r11),%r10d
    addl $20,%r10d
    jo .L51
    movl %r10d,%ecx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    jmp .L52
.L51:
    movl $20,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    movq %rax,%rdx
    popq %rcx
    subq $32,%rsp
    call vnt_add
    addq $32,%rsp
.L52:
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_print
    addq $32,%rsp
    lea .Lstr54(%rip),%rcx
    subq $32,%rsp
    call vnt_string
    addq $32,%rsp
    pushq %rax
    movl $7,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_neg
    addq $32,%rsp
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_fn_classify
    addq $32,%rsp
    movq %rax,%rdx
    popq %rcx
    testq %rcx,%rcx
    jz .L55
    testq %rdx,%rdx
    jz .L55
    cmpl $1,0(%rcx)
    jne .L55
    cmpl $1,0(%rdx)
    jne .L55
    movl 8(%rcx),%r10d
    addl 8(%rdx),%r10d
    jo .L55
    movl %r10d,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    jmp .L56
.L55:
    subq $32,%rsp
    call vnt_add
    addq $32,%rsp
.L56:
    pushq %rax
    lea .Lstr57(%rip),%rcx
    subq $32,%rsp
    call vnt_string
    addq $32,%rsp
    movq %rax,%rdx
    popq %rcx
    testq %rcx,%rcx
    jz .L58
    testq %rdx,%rdx
    jz .L58
    cmpl $1,0(%rcx)
    jne .L58
    cmpl $1,0(%rdx)
    jne .L58
    movl 8(%rcx),%r10d
    addl 8(%rdx),%r10d
    jo .L58
    movl %r10d,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    jmp .L59
.L58:
    subq $32,%rsp
    call vnt_add
    addq $32,%rsp
.L59:
    pushq %rax
    movl $0,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_fn_classify
    addq $32,%rsp
    movq %rax,%rdx
    popq %rcx
    testq %rcx,%rcx
    jz .L60
    testq %rdx,%rdx
    jz .L60
    cmpl $1,0(%rcx)
    jne .L60
    cmpl $1,0(%rdx)
    jne .L60
    movl 8(%rcx),%r10d
    addl 8(%rdx),%r10d
    jo .L60
    movl %r10d,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    jmp .L61
.L60:
    subq $32,%rsp
    call vnt_add
    addq $32,%rsp
.L61:
    pushq %rax
    lea .Lstr57(%rip),%rcx
    subq $32,%rsp
    call vnt_string
    addq $32,%rsp
    movq %rax,%rdx
    popq %rcx
    testq %rcx,%rcx
    jz .L62
    testq %rdx,%rdx
    jz .L62
    cmpl $1,0(%rcx)
    jne .L62
    cmpl $1,0(%rdx)
    jne .L62
    movl 8(%rcx),%r10d
    addl 8(%rdx),%r10d
    jo .L62
    movl %r10d,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    jmp .L63
.L62:
    subq $32,%rsp
    call vnt_add
    addq $32,%rsp
.L63:
    pushq %rax
    movl $9,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_fn_classify
    addq $32,%rsp
    movq %rax,%rdx
    popq %rcx
    testq %rcx,%rcx
    jz .L64
    testq %rdx,%rdx
    jz .L64
    cmpl $1,0(%rcx)
    jne .L64
    cmpl $1,0(%rdx)
    jne .L64
    movl 8(%rcx),%r10d
    addl 8(%rdx),%r10d
    jo .L64
    movl %r10d,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    jmp .L65
.L64:
    subq $32,%rsp
    call vnt_add
    addq $32,%rsp
.L65:
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_print
    addq $32,%rsp
    lea .Lstr66(%rip),%rcx
    subq $32,%rsp
    call vnt_string
    addq $32,%rsp
    pushq %rax
    movl $1,%ecx
    subq $32,%rsp
    call vnt_bool
    addq $32,%rsp
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_truth
    addq $32,%rsp
    testl %eax,%eax
    jz .L67
    movl $0,%ecx
    subq $32,%rsp
    call vnt_bool
    addq $32,%rsp
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_not
    addq $32,%rsp
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_truth
    addq $32,%rsp
    movl %eax,%ecx
    subq $32,%rsp
    call vnt_bool
    addq $32,%rsp
    jmp .L68
.L67:
    movl $0,%ecx
    subq $32,%rsp
    call vnt_bool
    addq $32,%rsp
.L68:
    movq %rax,%rdx
    popq %rcx
    testq %rcx,%rcx
    jz .L69
    testq %rdx,%rdx
    jz .L69
    cmpl $1,0(%rcx)
    jne .L69
    cmpl $1,0(%rdx)
    jne .L69
    movl 8(%rcx),%r10d
    addl 8(%rdx),%r10d
    jo .L69
    movl %r10d,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    jmp .L70
.L69:
    subq $32,%rsp
    call vnt_add
    addq $32,%rsp
.L70:
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_print
    addq $32,%rsp
    lea .Lstr71(%rip),%rcx
    subq $32,%rsp
    call vnt_string
    addq $32,%rsp
    pushq %rax
    movl $1,%ecx
    subq $32,%rsp
    call vnt_bool
    addq $32,%rsp
    movq %rax,%rdx
    popq %rcx
    testq %rcx,%rcx
    jz .L72
    testq %rdx,%rdx
    jz .L72
    cmpl $1,0(%rcx)
    jne .L72
    cmpl $1,0(%rdx)
    jne .L72
    movl 8(%rcx),%r10d
    addl 8(%rdx),%r10d
    jo .L72
    movl %r10d,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    jmp .L73
.L72:
    subq $32,%rsp
    call vnt_add
    addq $32,%rsp
.L73:
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_print
    addq $32,%rsp
    lea .Lstr74(%rip),%rcx
    subq $32,%rsp
    call vnt_string
    addq $32,%rsp
    pushq %rax
    movsd .Lflt75(%rip),%xmm0
    subq $32,%rsp
    call vnt_float
    addq $32,%rsp
    movq %rax,%rdx
    popq %rcx
    testq %rcx,%rcx
    jz .L76
    testq %rdx,%rdx
    jz .L76
    cmpl $1,0(%rcx)
    jne .L76
    cmpl $1,0(%rdx)
    jne .L76
    movl 8(%rcx),%r10d
    addl 8(%rdx),%r10d
    jo .L76
    movl %r10d,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    jmp .L77
.L76:
    subq $32,%rsp
    call vnt_add
    addq $32,%rsp
.L77:
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_print
    addq $32,%rsp
    lea .Lstr78(%rip),%rcx
    subq $32,%rsp
    call vnt_string
    addq $32,%rsp
    pushq %rax
    subq $32,%rsp
    call vnt_fn_global_increment
    addq $32,%rsp
    movq %rax,%rdx
    popq %rcx
    testq %rcx,%rcx
    jz .L79
    testq %rdx,%rdx
    jz .L79
    cmpl $1,0(%rcx)
    jne .L79
    cmpl $1,0(%rdx)
    jne .L79
    movl 8(%rcx),%r10d
    addl 8(%rdx),%r10d
    jo .L79
    movl %r10d,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    jmp .L80
.L79:
    subq $32,%rsp
    call vnt_add
    addq $32,%rsp
.L80:
    pushq %rax
    lea .Lstr57(%rip),%rcx
    subq $32,%rsp
    call vnt_string
    addq $32,%rsp
    movq %rax,%rdx
    popq %rcx
    testq %rcx,%rcx
    jz .L81
    testq %rdx,%rdx
    jz .L81
    cmpl $1,0(%rcx)
    jne .L81
    cmpl $1,0(%rdx)
    jne .L81
    movl 8(%rcx),%r10d
    addl 8(%rdx),%r10d
    jo .L81
    movl %r10d,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    jmp .L82
.L81:
    subq $32,%rsp
    call vnt_add
    addq $32,%rsp
.L82:
    pushq %rax
    subq $32,%rsp
    call vnt_fn_global_increment
    addq $32,%rsp
    movq %rax,%rdx
    popq %rcx
    testq %rcx,%rcx
    jz .L83
    testq %rdx,%rdx
    jz .L83
    cmpl $1,0(%rcx)
    jne .L83
    cmpl $1,0(%rdx)
    jne .L83
    movl 8(%rcx),%r10d
    addl 8(%rdx),%r10d
    jo .L83
    movl %r10d,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    jmp .L84
.L83:
    subq $32,%rsp
    call vnt_add
    addq $32,%rsp
.L84:
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_print
    addq $32,%rsp
    lea .Lstr85(%rip),%rcx
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
.Lflt75:
    .double 6.25
.Lstr30:
    .asciz "negative"
.Lstr35:
    .asciz "zero"
.Lstr36:
    .asciz "positive"
.Lstr39:
    .asciz "factorial="
.Lstr42:
    .asciz "fibonacci="
.Lstr45:
    .asciz "sum="
.Lstr50:
    .asciz "precedence="
.Lstr53:
    .asciz "parentheses="
.Lstr54:
    .asciz "classify="
.Lstr57:
    .asciz ","
.Lstr66:
    .asciz "logic="
.Lstr71:
    .asciz "comparison="
.Lstr74:
    .asciz "float="
.Lstr78:
    .asciz "global="
.Lstr85:
    .asciz "ULTIMATE_STRESS_OK"
