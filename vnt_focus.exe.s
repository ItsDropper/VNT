.text
.globl vnt_fn_vanta_gui_start
vnt_fn_vanta_gui_start:
    pushq %rbp
    movq %rsp,%rbp
    subq $32,%rsp
    movq %rcx,-32(%rbp)
    movq %rdx,-8(%rbp)
    movq %r8,-16(%rbp)
    movq %r9,-24(%rbp)
    movq -8(%rbp),%rax
    pushq %rax
    movq -16(%rbp),%rax
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_gui_size
    addq $32, %rsp
    movq -24(%rbp),%rax
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_gui_css
    addq $32, %rsp
    movq -32(%rbp),%rax
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_gui_open
    addq $32, %rsp
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_vanta_gui_poll
vnt_fn_vanta_gui_poll:
    pushq %rbp
    movq %rsp,%rbp
    subq $32, %rsp
    call vnt_gui_poll
    addq $32, %rsp
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_vanta_gui_present
vnt_fn_vanta_gui_present:
    pushq %rbp
    movq %rsp,%rbp
    subq $32, %rsp
    call vnt_gui_present
    addq $32, %rsp
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_vanta_gui_close
vnt_fn_vanta_gui_close:
    pushq %rbp
    movq %rsp,%rbp
    subq $32, %rsp
    call vnt_gui_close
    addq $32, %rsp
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_vanta_label
vnt_fn_vanta_label:
    pushq %rbp
    movq %rsp,%rbp
    subq $32,%rsp
    movq %rcx,-8(%rbp)
    movq %rdx,-16(%rbp)
    movq %r8,-24(%rbp)
    movq -8(%rbp),%rax
    pushq %rax
    movq -16(%rbp),%rax
    pushq %rax
    movq -24(%rbp),%rax
    pushq %rax
    movq 16(%rsp),%rcx
    movq 8(%rsp),%rdx
    movq 0(%rsp),%r8
    addq $24,%rsp
    subq $32, %rsp
    call vnt_gui_text_at
    addq $32, %rsp
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_vanta_heading
vnt_fn_vanta_heading:
    pushq %rbp
    movq %rsp,%rbp
    subq $32,%rsp
    movq %rcx,-8(%rbp)
    movq %rdx,-16(%rbp)
    movq %r8,-24(%rbp)
    movq -8(%rbp),%rax
    pushq %rax
    movq -16(%rbp),%rax
    pushq %rax
    movq -24(%rbp),%rax
    pushq %rax
    movq 16(%rsp),%rcx
    movq 8(%rsp),%rdx
    movq 0(%rsp),%r8
    addq $24,%rsp
    subq $32, %rsp
    call vnt_gui_title
    addq $32, %rsp
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_vanta_card
vnt_fn_vanta_card:
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
    pushq %rax
    movq -32(%rbp),%rax
    pushq %rax
    movq 24(%rsp),%rcx
    movq 16(%rsp),%rdx
    movq 8(%rsp),%r8
    movq 0(%rsp),%r9
    addq $32,%rsp
    subq $32, %rsp
    call vnt_gui_panel
    addq $32, %rsp
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_vanta_button
vnt_fn_vanta_button:
    pushq %rbp
    movq %rsp,%rbp
    subq $32,%rsp
    movq %rcx,-8(%rbp)
    movq %rdx,-16(%rbp)
    movq %r8,-24(%rbp)
    movq -8(%rbp),%rax
    pushq %rax
    movq -16(%rbp),%rax
    pushq %rax
    movq -24(%rbp),%rax
    pushq %rax
    movq 16(%rsp),%rcx
    movq 8(%rsp),%rdx
    movq 0(%rsp),%r8
    addq $24,%rsp
    subq $32, %rsp
    call vnt_gui_button
    addq $32, %rsp
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_vanta_text_input
vnt_fn_vanta_text_input:
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
    pushq %rax
    movq -32(%rbp),%rax
    pushq %rax
    movq 24(%rsp),%rcx
    movq 16(%rsp),%rdx
    movq 8(%rsp),%r8
    movq 0(%rsp),%r9
    addq $32,%rsp
    subq $32, %rsp
    call vnt_gui_input
    addq $32, %rsp
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_vanta_checkbox
vnt_fn_vanta_checkbox:
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
    pushq %rax
    movq -32(%rbp),%rax
    pushq %rax
    movq 24(%rsp),%rcx
    movq 16(%rsp),%rdx
    movq 8(%rsp),%r8
    movq 0(%rsp),%r9
    addq $32,%rsp
    subq $32, %rsp
    call vnt_gui_checkbox
    addq $32, %rsp
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_vanta_progress
vnt_fn_vanta_progress:
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
    pushq %rax
    movq -32(%rbp),%rax
    pushq %rax
    movq 24(%rsp),%rcx
    movq 16(%rsp),%rdx
    movq 8(%rsp),%r8
    movq 0(%rsp),%r9
    addq $32,%rsp
    subq $32, %rsp
    call vnt_gui_progress
    addq $32, %rsp
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_vanta_separator
vnt_fn_vanta_separator:
    pushq %rbp
    movq %rsp,%rbp
    subq $32,%rsp
    movq %rcx,-8(%rbp)
    movq %rdx,-16(%rbp)
    movq %r8,-24(%rbp)
    movq -8(%rbp),%rax
    pushq %rax
    movq -16(%rbp),%rax
    pushq %rax
    movq -24(%rbp),%rax
    pushq %rax
    movq 16(%rsp),%rcx
    movq 8(%rsp),%rdx
    movq 0(%rsp),%r8
    addq $24,%rsp
    subq $32, %rsp
    call vnt_gui_separator
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
    subq $128,%rsp
    lea .Lstr2(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movl $1000,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $720,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    lea .Lstr3(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movq 24(%rsp), %rcx
    movq 16(%rsp), %rdx
    movq 8(%rsp), %r8
    movq 0(%rsp), %r9
    subq $32,%rsp
    call vnt_fn_vanta_gui_start
    addq $32,%rsp
    addq $32,%rsp
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L0
    movl $1,%ecx
    subq $32, %rsp
    call vnt_bool
    addq $32, %rsp
    movq %rax,-8(%rbp)
    movl $0,%ecx
    subq $32, %rsp
    call vnt_bool
    addq $32, %rsp
    movq %rax,-16(%rbp)
    movl $1500,%eax
    movl %eax,-24(%rbp)
    subq $32, %rsp
    call vnt_time_ms
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
    movl $0,%ecx
    subq $32, %rsp
    call vnt_bool
    addq $32, %rsp
    movq %rax,-56(%rbp)
    movl $0,%eax
    movl %eax,-64(%rbp)
    lea .Lstr4(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,-72(%rbp)
    lea .Lstr5(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,-80(%rbp)
.L6:
    movq -8(%rbp),%rax
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L7
    subq $32,%rsp
    call vnt_fn_vanta_gui_poll
    addq $32,%rsp
    movq %rax,-8(%rbp)
    movq -16(%rbp),%rax
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L8
    subq $32, %rsp
    call vnt_time_ms
    addq $32, %rsp
    pushq %rax
    movq -32(%rbp),%rax
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_sub
    addq $32, %rsp
    pushq %rax
    movl $1000,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_ge
    addq $32, %rsp
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L10
    movl -24(%rbp),%eax
    pushq %rax
    movl $1,%eax
    movl %eax,%r10d
    popq %rax
    subl %r10d,%eax
    jo .Lvnt_int_sub_overflow
    movl %eax,-24(%rbp)
    subq $32, %rsp
    call vnt_time_ms
    addq $32, %rsp
    movq %rax,-32(%rbp)
    movl -24(%rbp),%eax
    pushq %rax
    movl $0,%eax
    movl %eax,%r10d
    popq %rax
    cmpl %r10d,%eax
    jg .L12
    movl $0,%ecx
    subq $32, %rsp
    call vnt_bool
    addq $32, %rsp
    movq %rax,-16(%rbp)
    lea .Lstr14(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,-72(%rbp)
    movq -80(%rbp),%rax
    pushq %rax
    lea .Lstr15(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_fs_append
    addq $32, %rsp
    movl -64(%rbp),%eax
    pushq %rax
    movl $1,%eax
    movl %eax,%r10d
    popq %rax
    addl %r10d,%eax
    jo .Lvnt_int_add_overflow
    movl %eax,-64(%rbp)
    jmp .L13
.L12:
.L13:
    jmp .L11
.L10:
.L11:
    jmp .L9
.L8:
.L9:
    movl -24(%rbp),%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $60,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_div
    addq $32, %rsp
    movq %rax,-88(%rbp)
    movl -24(%rbp),%eax
    pushq %rax
    movl $60,%eax
    movl %eax,%r10d
    popq %rax
    testl %r10d,%r10d
    jz .Lvnt_int_div_zero
    cmpl $-1,%r10d
    jne .L16
    cmpl $-2147483648,%eax
    jne .L16
    xorl %eax,%eax
    jmp .L17
.L16:
    cltd
    idivl %r10d
    movl %edx,%eax
.L17:
    movl %eax,-96(%rbp)
    movq -88(%rbp),%rax
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
    jz .L18
    lea .Lstr20(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movq -88(%rbp),%rax
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_add
    addq $32, %rsp
    movq %rax,-104(%rbp)
    jmp .L19
.L18:
    lea .Lstr21(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movq -88(%rbp),%rax
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_add
    addq $32, %rsp
    movq %rax,-104(%rbp)
.L19:
    movl -96(%rbp),%eax
    pushq %rax
    movl $10,%eax
    movl %eax,%r10d
    popq %rax
    cmpl %r10d,%eax
    jge .L22
    lea .Lstr20(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movl -96(%rbp),%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_add
    addq $32, %rsp
    movq %rax,-112(%rbp)
    jmp .L23
.L22:
    lea .Lstr21(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movl -96(%rbp),%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_add
    addq $32, %rsp
    movq %rax,-112(%rbp)
.L23:
    movl $28,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $120,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $610,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $330,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 24(%rsp), %rcx
    movq 16(%rsp), %rdx
    movq 8(%rsp), %r8
    movq 0(%rsp), %r9
    subq $32,%rsp
    call vnt_fn_vanta_card
    addq $32,%rsp
    addq $32,%rsp
    movl $658,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $120,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $312,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $330,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 24(%rsp),%rcx
    movq 16(%rsp),%rdx
    movq 8(%rsp),%r8
    movq 0(%rsp),%r9
    addq $32,%rsp
    subq $32, %rsp
    call vnt_gui_panel
    addq $32, %rsp
    movl $28,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $470,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $942,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $190,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 24(%rsp),%rcx
    movq 16(%rsp),%rdx
    movq 8(%rsp),%r8
    movq 0(%rsp),%r9
    addq $32,%rsp
    subq $32, %rsp
    call vnt_gui_panel
    addq $32, %rsp
    lea .Lstr24(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movl $34,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $26,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 16(%rsp), %rcx
    movq 8(%rsp), %rdx
    movq 0(%rsp), %r8
    subq $40,%rsp
    call vnt_fn_vanta_heading
    addq $40,%rsp
    addq $24,%rsp
    lea .Lstr25(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movl $36,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $72,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 16(%rsp), %rcx
    movq 8(%rsp), %rdx
    movq 0(%rsp), %r8
    subq $40,%rsp
    call vnt_fn_vanta_label
    addq $40,%rsp
    addq $24,%rsp
    lea .Lstr26(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movl $36,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $94,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 16(%rsp),%rcx
    movq 8(%rsp),%rdx
    movq 0(%rsp),%r8
    addq $24,%rsp
    subq $32, %rsp
    call vnt_gui_text_at
    addq $32, %rsp
    lea .Lstr27(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movl $52,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $145,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 16(%rsp),%rcx
    movq 8(%rsp),%rdx
    movq 0(%rsp),%r8
    addq $24,%rsp
    subq $32, %rsp
    call vnt_gui_text_at
    addq $32, %rsp
    movq -104(%rbp),%rax
    pushq %rax
    lea .Lstr28(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_add
    addq $32, %rsp
    pushq %rax
    movq -112(%rbp),%rax
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_add
    addq $32, %rsp
    pushq %rax
    movl $52,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $184,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 16(%rsp),%rcx
    movq 8(%rsp),%rdx
    movq 0(%rsp),%r8
    addq $24,%rsp
    subq $32, %rsp
    call vnt_gui_title
    addq $32, %rsp
    movq -16(%rbp),%rax
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L29
    lea .Lstr31(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movl $54,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $244,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 16(%rsp),%rcx
    movq 8(%rsp),%rdx
    movq 0(%rsp),%r8
    addq $24,%rsp
    subq $32, %rsp
    call vnt_gui_text_at
    addq $32, %rsp
    jmp .L30
.L29:
    lea .Lstr32(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movl $54,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $244,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 16(%rsp),%rcx
    movq 8(%rsp),%rdx
    movq 0(%rsp),%r8
    addq $24,%rsp
    subq $32, %rsp
    call vnt_gui_text_at
    addq $32, %rsp
.L30:
    lea .Lstr33(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movl $54,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $280,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 16(%rsp),%rcx
    movq 8(%rsp),%rdx
    movq 0(%rsp),%r8
    addq $24,%rsp
    subq $32, %rsp
    call vnt_gui_text_at
    addq $32, %rsp
    lea .Lstr34(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movl $682,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $145,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 16(%rsp),%rcx
    movq 8(%rsp),%rdx
    movq 0(%rsp),%r8
    addq $24,%rsp
    subq $32, %rsp
    call vnt_gui_text_at
    addq $32, %rsp
    movq -40(%rbp),%rax
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L35
    lea .Lstr37(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movl $682,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $190,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 16(%rsp),%rcx
    movq 8(%rsp),%rdx
    movq 0(%rsp),%r8
    addq $24,%rsp
    subq $32, %rsp
    call vnt_gui_text_at
    addq $32, %rsp
    jmp .L36
.L35:
    lea .Lstr38(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movl $682,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $190,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 16(%rsp),%rcx
    movq 8(%rsp),%rdx
    movq 0(%rsp),%r8
    addq $24,%rsp
    subq $32, %rsp
    call vnt_gui_text_at
    addq $32, %rsp
.L36:
    movq -48(%rbp),%rax
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
    pushq %rax
    movl $682,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $226,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 16(%rsp),%rcx
    movq 8(%rsp),%rdx
    movq 0(%rsp),%r8
    addq $24,%rsp
    subq $32, %rsp
    call vnt_gui_text_at
    addq $32, %rsp
    jmp .L40
.L39:
    lea .Lstr42(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movl $682,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $226,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 16(%rsp),%rcx
    movq 8(%rsp),%rdx
    movq 0(%rsp),%r8
    addq $24,%rsp
    subq $32, %rsp
    call vnt_gui_text_at
    addq $32, %rsp
.L40:
    movq -56(%rbp),%rax
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L43
    lea .Lstr45(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movl $682,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $262,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 16(%rsp),%rcx
    movq 8(%rsp),%rdx
    movq 0(%rsp),%r8
    addq $24,%rsp
    subq $32, %rsp
    call vnt_gui_text_at
    addq $32, %rsp
    jmp .L44
.L43:
    lea .Lstr46(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movl $682,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $262,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 16(%rsp),%rcx
    movq 8(%rsp),%rdx
    movq 0(%rsp),%r8
    addq $24,%rsp
    subq $32, %rsp
    call vnt_gui_text_at
    addq $32, %rsp
.L44:
    lea .Lstr47(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movl $682,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $305,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 16(%rsp),%rcx
    movq 8(%rsp),%rdx
    movq 0(%rsp),%r8
    addq $24,%rsp
    subq $32, %rsp
    call vnt_gui_text_at
    addq $32, %rsp
    lea .Lstr48(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movl $52,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $495,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 16(%rsp),%rcx
    movq 8(%rsp),%rdx
    movq 0(%rsp),%r8
    addq $24,%rsp
    subq $32, %rsp
    call vnt_gui_text_at
    addq $32, %rsp
    movq -72(%rbp),%rax
    pushq %rax
    movl $52,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $526,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 16(%rsp),%rcx
    movq 8(%rsp),%rdx
    movq 0(%rsp),%r8
    addq $24,%rsp
    subq $32, %rsp
    call vnt_gui_text_at
    addq $32, %rsp
    lea .Lstr49(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movl -64(%rbp),%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_add
    addq $32, %rsp
    pushq %rax
    movl $52,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $554,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 16(%rsp),%rcx
    movq 8(%rsp),%rdx
    movq 0(%rsp),%r8
    addq $24,%rsp
    subq $32, %rsp
    call vnt_gui_text_at
    addq $32, %rsp
    lea .Lstr52(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movl $52,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $350,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 16(%rsp), %rcx
    movq 8(%rsp), %rdx
    movq 0(%rsp), %r8
    subq $40,%rsp
    call vnt_fn_vanta_button
    addq $40,%rsp
    addq $24,%rsp
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L50
    movl -24(%rbp),%eax
    pushq %rax
    movl $0,%eax
    movl %eax,%r10d
    popq %rax
    cmpl %r10d,%eax
    jg .L53
    movl $1500,%eax
    movl %eax,-24(%rbp)
    jmp .L54
.L53:
.L54:
    movl $1,%ecx
    subq $32, %rsp
    call vnt_bool
    addq $32, %rsp
    movq %rax,-16(%rbp)
    subq $32, %rsp
    call vnt_time_ms
    addq $32, %rsp
    movq %rax,-32(%rbp)
    lea .Lstr55(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,-72(%rbp)
    jmp .L51
.L50:
.L51:
    lea .Lstr58(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movl $220,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $350,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 16(%rsp),%rcx
    movq 8(%rsp),%rdx
    movq 0(%rsp),%r8
    addq $24,%rsp
    subq $32, %rsp
    call vnt_gui_button
    addq $32, %rsp
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L56
    movl $0,%ecx
    subq $32, %rsp
    call vnt_bool
    addq $32, %rsp
    movq %rax,-16(%rbp)
    lea .Lstr59(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,-72(%rbp)
    jmp .L57
.L56:
.L57:
    lea .Lstr62(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movl $326,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $350,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 16(%rsp),%rcx
    movq 8(%rsp),%rdx
    movq 0(%rsp),%r8
    addq $24,%rsp
    subq $32, %rsp
    call vnt_gui_button
    addq $32, %rsp
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L60
    movl $0,%ecx
    subq $32, %rsp
    call vnt_bool
    addq $32, %rsp
    movq %rax,-16(%rbp)
    movl $1500,%eax
    movl %eax,-24(%rbp)
    lea .Lstr63(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,-72(%rbp)
    jmp .L61
.L60:
.L61:
    lea .Lstr66(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movl $682,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $344,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 16(%rsp),%rcx
    movq 8(%rsp),%rdx
    movq 0(%rsp),%r8
    addq $24,%rsp
    subq $32, %rsp
    call vnt_gui_button
    addq $32, %rsp
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L64
    movq -40(%rbp),%rax
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_not
    addq $32, %rsp
    movq %rax,-40(%rbp)
    movq -40(%rbp),%rax
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L67
    lea .Lstr69(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,-72(%rbp)
    jmp .L68
.L67:
    lea .Lstr70(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,-72(%rbp)
.L68:
    jmp .L65
.L64:
.L65:
    lea .Lstr73(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movl $682,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $382,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 16(%rsp),%rcx
    movq 8(%rsp),%rdx
    movq 0(%rsp),%r8
    addq $24,%rsp
    subq $32, %rsp
    call vnt_gui_button
    addq $32, %rsp
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L71
    movq -48(%rbp),%rax
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_not
    addq $32, %rsp
    movq %rax,-48(%rbp)
    movq -48(%rbp),%rax
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L74
    lea .Lstr76(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,-72(%rbp)
    jmp .L75
.L74:
    lea .Lstr77(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,-72(%rbp)
.L75:
    jmp .L72
.L71:
.L72:
    lea .Lstr80(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movl $682,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $420,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 16(%rsp),%rcx
    movq 8(%rsp),%rdx
    movq 0(%rsp),%r8
    addq $24,%rsp
    subq $32, %rsp
    call vnt_gui_button
    addq $32, %rsp
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L78
    movq -56(%rbp),%rax
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_not
    addq $32, %rsp
    movq %rax,-56(%rbp)
    movq -56(%rbp),%rax
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L81
    lea .Lstr83(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,-72(%rbp)
    jmp .L82
.L81:
    lea .Lstr84(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,-72(%rbp)
.L82:
    jmp .L79
.L78:
.L79:
    lea .Lstr87(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movl $52,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $590,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 16(%rsp),%rcx
    movq 8(%rsp),%rdx
    movq 0(%rsp),%r8
    addq $24,%rsp
    subq $32, %rsp
    call vnt_gui_button
    addq $32, %rsp
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L85
    movq -80(%rbp),%rax
    pushq %rax
    lea .Lstr88(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movq -104(%rbp),%rax
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_add
    addq $32, %rsp
    pushq %rax
    lea .Lstr28(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_add
    addq $32, %rsp
    pushq %rax
    movq -112(%rbp),%rax
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_add
    addq $32, %rsp
    pushq %rax
    lea .Lstr89(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_add
    addq $32, %rsp
    pushq %rax
    movq -72(%rbp),%rax
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_add
    addq $32, %rsp
    pushq %rax
    lea .Lstr90(%rip),%rcx
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
    movq %rax,-120(%rbp)
    movq -120(%rbp),%rax
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L91
    movl -64(%rbp),%eax
    pushq %rax
    movl $1,%eax
    movl %eax,%r10d
    popq %rax
    addl %r10d,%eax
    jo .Lvnt_int_add_overflow
    movl %eax,-64(%rbp)
    lea .Lstr93(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,-72(%rbp)
    jmp .L92
.L91:
    lea .Lstr94(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,-72(%rbp)
.L92:
    jmp .L86
.L85:
.L86:
    lea .Lstr97(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movl $202,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $590,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 16(%rsp),%rcx
    movq 8(%rsp),%rdx
    movq 0(%rsp),%r8
    addq $24,%rsp
    subq $32, %rsp
    call vnt_gui_button
    addq $32, %rsp
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L95
    movl $0,%ecx
    subq $32, %rsp
    call vnt_bool
    addq $32, %rsp
    movq %rax,-8(%rbp)
    jmp .L96
.L95:
.L96:
    subq $32,%rsp
    call vnt_fn_vanta_gui_present
    addq $32,%rsp
    movl $16,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_sleep_ms
    addq $32, %rsp
    jmp .L6
.L7:
    subq $32,%rsp
    call vnt_fn_vanta_gui_close
    addq $32,%rsp
    jmp .L1
.L0:
    lea .Lstr98(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
.L1:
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
.Lstr2:
    .asciz "VNT Focus  |  Make room for deep work"
.Lstr3:
    .asciz "examples/vnt_focus.css"
.Lstr4:
    .asciz "Ready when you are. Start a focus session."
.Lstr5:
    .asciz "vnt_focus.log"
.Lstr14:
    .asciz "Focus block complete. Take a short break."
.Lstr15:
    .asciz "Completed 25-minute focus block.\\n"
.Lstr20:
    .asciz "0"
.Lstr21:
    .asciz ""
.Lstr24:
    .asciz "VNT FOCUS"
.Lstr25:
    .asciz "YOUR QUIET SPACE FOR GETTING THINGS DONE"
.Lstr26:
    .asciz "LOCAL FIRST  /  NO ACCOUNTS  /  NO TELEMETRY"
.Lstr27:
    .asciz "FOCUS SESSION"
.Lstr28:
    .asciz ":"
.Lstr31:
    .asciz "IN FOCUS  -  STAY WITH ONE THING"
.Lstr32:
    .asciz "A 25-MINUTE BLOCK. ONE THING AT A TIME."
.Lstr33:
    .asciz "Choose a control to manage your session."
.Lstr34:
    .asciz "TODAY'S TASKS"
.Lstr37:
    .asciz "[x] Review current VNT build"
.Lstr38:
    .asciz "[ ] Review current VNT build"
.Lstr41:
    .asciz "[x] Ship one useful feature"
.Lstr42:
    .asciz "[ ] Ship one useful feature"
.Lstr45:
    .asciz "[x] Write a short dev log"
.Lstr46:
    .asciz "[ ] Write a short dev log"
.Lstr47:
    .asciz "Click a task below to toggle it."
.Lstr48:
    .asciz "SESSION CONTROLS"
.Lstr49:
    .asciz "Saved sessions this run: "
.Lstr52:
    .asciz "Start / Resume"
.Lstr55:
    .asciz "Focus timer running. Notifications stay out of your way."
.Lstr58:
    .asciz "Pause"
.Lstr59:
    .asciz "Paused. Resume whenever you're ready."
.Lstr62:
    .asciz "Reset"
.Lstr63:
    .asciz "Timer reset to 25:00."
.Lstr66:
    .asciz "Toggle task 1"
.Lstr69:
    .asciz "Task completed: Review current VNT build."
.Lstr70:
    .asciz "Task reopened: Review current VNT build."
.Lstr73:
    .asciz "Toggle task 2"
.Lstr76:
    .asciz "Task completed: Ship one useful feature."
.Lstr77:
    .asciz "Task reopened: Ship one useful feature."
.Lstr80:
    .asciz "Toggle task 3"
.Lstr83:
    .asciz "Task completed: Write a short dev log."
.Lstr84:
    .asciz "Task reopened: Write a short dev log."
.Lstr87:
    .asciz "Save session"
.Lstr88:
    .asciz "Manual log: remaining "
.Lstr89:
    .asciz " | "
.Lstr90:
    .asciz "\\n"
.Lstr93:
    .asciz "Session note saved to vnt_focus.log."
.Lstr94:
    .asciz "Could not write vnt_focus.log in this folder."
.Lstr97:
    .asciz "Exit"
.Lstr98:
    .asciz "Could not create the VNT Focus window."
