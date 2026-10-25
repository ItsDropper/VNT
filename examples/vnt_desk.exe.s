.text
.globl vnt_fn_show_header
vnt_fn_show_header:
    pushq %rbp
    movq %rsp,%rbp
    subq $16,%rsp
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
    lea .Lstr4(%rip),%rcx
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
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr5(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    subq $32, %rsp
    call vnt_cwd
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
    lea .Lstr6(%rip),%rcx
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
    lea .Lstr0(%rip),%rcx
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
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_add_entry
vnt_fn_add_entry:
    pushq %rbp
    movq %rsp,%rbp
    subq $32,%rsp
    lea .Lstr0(%rip),%rcx
    subq $32, %rsp
    call vnt_string
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
    call vnt_input
    addq $32, %rsp
    movq %rax,-8(%rbp)
    movq -8(%rbp),%rax
    pushq %rax
    lea .Lstr0(%rip),%rcx
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
    jz .L8
    lea .Lstr10(%rip),%rcx
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
    leave
    ret
    jmp .L9
.L8:
.L9:
    movq -24(%rbp),%rax
    pushq %rax
    lea .Lstr11(%rip),%rcx
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
    lea .Lstr12(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_add
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_fs_append
    addq $32, %rsp
    movq %rax,-16(%rbp)
    movq -16(%rbp),%rax
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L13
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
    lea .Lstr15(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    jmp .L14
.L13:
    lea .Lstr16(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
.L14:
    movl $0,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_show_journal
vnt_fn_show_journal:
    pushq %rbp
    movq %rsp,%rbp
    subq $16,%rsp
    lea .Lstr0(%rip),%rcx
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
    movq -8(%rbp),%rax
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_fs_exists
    addq $32, %rsp
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_not
    addq $32, %rsp
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L18
    lea .Lstr20(%rip),%rcx
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
    leave
    ret
    jmp .L19
.L18:
.L19:
    movq -8(%rbp),%rax
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_fs_read
    addq $32, %rsp
    movq %rax,-16(%rbp)
    movq -16(%rbp),%rax
    pushq %rax
    lea .Lstr0(%rip),%rcx
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
    lea .Lstr23(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    jmp .L22
.L21:
    movq -16(%rbp),%rax
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
.L22:
    lea .Lstr1(%rip),%rcx
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
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_show_diagnostics
vnt_fn_show_diagnostics:
    pushq %rbp
    movq %rsp,%rbp
    subq $32,%rsp
    lea .Lstr0(%rip),%rcx
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
    lea .Lstr5(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    subq $32, %rsp
    call vnt_cwd
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
    call vnt_env_get
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr27(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr28(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_env_get
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr29(%rip),%rcx
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
    call vnt_fs_exists
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr30(%rip),%rcx
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
    lea .Lstr31(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    subq $32, %rsp
    call vnt_time_ms
    addq $32, %rsp
    pushq %rax
    movq -24(%rbp),%rax
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_sub
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
    movl $0,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_clear_journal
vnt_fn_clear_journal:
    pushq %rbp
    movq %rsp,%rbp
    subq $16,%rsp
    lea .Lstr0(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    lea .Lstr32(%rip),%rcx
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
    lea .Lstr33(%rip),%rcx
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
    lea .Lstr34(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_input
    addq $32, %rsp
    movq %rax,-16(%rbp)
    movq -16(%rbp),%rax
    pushq %rax
    lea .Lstr37(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_ne
    addq $32, %rsp
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L35
    lea .Lstr38(%rip),%rcx
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
    leave
    ret
    jmp .L36
.L35:
.L36:
    movq -8(%rbp),%rax
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_fs_exists
    addq $32, %rsp
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_not
    addq $32, %rsp
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L39
    lea .Lstr41(%rip),%rcx
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
    leave
    ret
    jmp .L40
.L39:
.L40:
    movq -8(%rbp),%rax
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_fs_delete
    addq $32, %rsp
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L42
    lea .Lstr44(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    jmp .L43
.L42:
    lea .Lstr45(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
.L43:
    movl $0,%ecx
    subq $32, %rsp
    call vnt_int
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
    subq $48,%rsp
    lea .Lstr46(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,-8(%rbp)
    subq $32, %rsp
    call vnt_time_ms
    addq $32, %rsp
    movq %rax,-16(%rbp)
    movl $0,%eax
    movl %eax,-24(%rbp)
    movl $1,%ecx
    subq $32, %rsp
    call vnt_bool
    addq $32, %rsp
    movq %rax,-32(%rbp)
.L47:
    movq -32(%rbp),%rax
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L48
    subq $32,%rsp
    call vnt_fn_show_header
    addq $32,%rsp
    lea .Lstr49(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
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
    lea .Lstr53(%rip),%rcx
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
    lea .Lstr54(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_input
    addq $32, %rsp
    movq %rax,-40(%rbp)
    movq -40(%rbp),%rax
    pushq %rax
    lea .Lstr57(%rip),%rcx
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
    jz .L55
    subq $32,%rsp
    call vnt_fn_add_entry
    addq $32,%rsp
    jmp .L56
.L55:
    movq -40(%rbp),%rax
    pushq %rax
    lea .Lstr60(%rip),%rcx
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
    jz .L58
    subq $32,%rsp
    call vnt_fn_show_journal
    addq $32,%rsp
    jmp .L59
.L58:
    movq -40(%rbp),%rax
    pushq %rax
    lea .Lstr63(%rip),%rcx
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
    jz .L61
    subq $32,%rsp
    call vnt_fn_show_diagnostics
    addq $32,%rsp
    jmp .L62
.L61:
    movq -40(%rbp),%rax
    pushq %rax
    lea .Lstr66(%rip),%rcx
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
    jz .L64
    subq $32,%rsp
    call vnt_fn_clear_journal
    addq $32,%rsp
    jmp .L65
.L64:
    movq -40(%rbp),%rax
    pushq %rax
    lea .Lstr69(%rip),%rcx
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
    jz .L67
    movl $0,%ecx
    subq $32, %rsp
    call vnt_bool
    addq $32, %rsp
    movq %rax,-32(%rbp)
    lea .Lstr70(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    jmp .L68
.L67:
    lea .Lstr71(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
.L68:
.L65:
.L62:
.L59:
.L56:
    jmp .L47
.L48:
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
    subq $32, %rsp
    call vnt_int_div_zero
    addq $32, %rsp

.section .rdata
.Lstr0:
    .asciz ""
.Lstr1:
    .asciz "================================================"
.Lstr2:
    .asciz "                  VNT DESK"
.Lstr3:
    .asciz "          LOCAL DEVELOPER JOURNAL"
.Lstr4:
    .asciz "Journal: "
.Lstr5:
    .asciz "Working directory: "
.Lstr6:
    .asciz "Session entries added:"
.Lstr7:
    .asciz "Write an entry (one line): "
.Lstr10:
    .asciz "Empty entry cancelled."
.Lstr11:
    .asciz "- "
.Lstr12:
    .asciz "\\n"
.Lstr15:
    .asciz "Saved locally."
.Lstr16:
    .asciz "Could not save the entry. Check directory permissions."
.Lstr17:
    .asciz "================ SAVED JOURNAL ================"
.Lstr20:
    .asciz "No journal yet. Add your first entry."
.Lstr23:
    .asciz "The journal is empty."
.Lstr24:
    .asciz "================ SYSTEM REPORT ================="
.Lstr25:
    .asciz "OS environment variable:"
.Lstr26:
    .asciz "OS"
.Lstr27:
    .asciz "Home directory variable:"
.Lstr28:
    .asciz "HOME"
.Lstr29:
    .asciz "Journal exists:"
.Lstr30:
    .asciz "Entries added this session:"
.Lstr31:
    .asciz "Session elapsed (ms):"
.Lstr32:
    .asciz "This permanently deletes "
.Lstr33:
    .asciz "."
.Lstr34:
    .asciz "Type DELETE to confirm: "
.Lstr37:
    .asciz "DELETE"
.Lstr38:
    .asciz "Cancelled. Journal unchanged."
.Lstr41:
    .asciz "There is no journal to delete."
.Lstr44:
    .asciz "Journal deleted."
.Lstr45:
    .asciz "Could not delete the journal."
.Lstr46:
    .asciz "vnt_desk.log"
.Lstr49:
    .asciz "1. Write a journal entry"
.Lstr50:
    .asciz "2. Read saved journal"
.Lstr51:
    .asciz "3. System diagnostics"
.Lstr52:
    .asciz "4. Delete journal"
.Lstr53:
    .asciz "5. Exit"
.Lstr54:
    .asciz "vnt-desk> "
.Lstr57:
    .asciz "1"
.Lstr60:
    .asciz "2"
.Lstr63:
    .asciz "3"
.Lstr66:
    .asciz "4"
.Lstr69:
    .asciz "5"
.Lstr70:
    .asciz "VNT Desk closed. Your journal stays on disk."
.Lstr71:
    .asciz "Unknown command. Choose 1-5."
