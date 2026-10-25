.text
.bss
.globl vnt_global_current_note
.comm vnt_global_current_note,8,8
.globl vnt_global_title_path
.comm vnt_global_title_path,8,8
.globl vnt_global_body_path
.comm vnt_global_body_path,8,8
.globl vnt_global_saved_title
.comm vnt_global_saved_title,8,8
.globl vnt_global_saved_body
.comm vnt_global_saved_body,8,8
.globl vnt_global_original_title
.comm vnt_global_original_title,8,8
.globl vnt_global_original_body
.comm vnt_global_original_body,8,8
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

.globl vnt_fn_vanta_button_sized
vnt_fn_vanta_button_sized:
    pushq %rbp
    movq %rsp,%rbp
    subq $48,%rsp
    movq %rcx,-8(%rbp)
    movq %rdx,-16(%rbp)
    movq %r8,-24(%rbp)
    movq %r9,-32(%rbp)
    movq 48(%rbp),%r10
    movq %r10,-40(%rbp)
    movq -8(%rbp),%rax
    pushq %rax
    movq -16(%rbp),%rax
    pushq %rax
    movq -24(%rbp),%rax
    pushq %rax
    movq -32(%rbp),%rax
    pushq %rax
    movq -40(%rbp),%rax
    pushq %rax
    movq 32(%rsp),%rcx
    movq 24(%rsp),%rdx
    movq 16(%rsp),%r8
    movq 8(%rsp),%r9
    movq 0(%rsp),%r10
    addq $40,%rsp
    subq $48,%rsp
    movq %r10,32(%rsp)
    call vnt_gui_button_sized
    addq $48,%rsp
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_vanta_textarea
vnt_fn_vanta_textarea:
    pushq %rbp
    movq %rsp,%rbp
    subq $48,%rsp
    movq %rcx,-8(%rbp)
    movq %rdx,-16(%rbp)
    movq %r8,-24(%rbp)
    movq %r9,-32(%rbp)
    movq 48(%rbp),%r10
    movq %r10,-40(%rbp)
    movq -8(%rbp),%rax
    pushq %rax
    movq -16(%rbp),%rax
    pushq %rax
    movq -24(%rbp),%rax
    pushq %rax
    movq -32(%rbp),%rax
    pushq %rax
    movq -40(%rbp),%rax
    pushq %rax
    movq 32(%rsp),%rcx
    movq 24(%rsp),%rdx
    movq 16(%rsp),%r8
    movq 8(%rsp),%r9
    movq 0(%rsp),%r10
    addq $40,%rsp
    subq $48,%rsp
    movq %r10,32(%rsp)
    call vnt_gui_textarea
    addq $48,%rsp
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_vanta_input_set
vnt_fn_vanta_input_set:
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
    call vnt_gui_input_set
    addq $32, %rsp
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_vanta_surface
vnt_fn_vanta_surface:
    pushq %rbp
    movq %rsp,%rbp
    subq $48,%rsp
    movq %rcx,-8(%rbp)
    movq %rdx,-16(%rbp)
    movq %r8,-24(%rbp)
    movq %r9,-32(%rbp)
    movq 48(%rbp),%r10
    movq %r10,-40(%rbp)
    movq -8(%rbp),%rax
    pushq %rax
    movq -16(%rbp),%rax
    pushq %rax
    movq -24(%rbp),%rax
    pushq %rax
    movq -32(%rbp),%rax
    pushq %rax
    movq -40(%rbp),%rax
    pushq %rax
    movq 32(%rsp),%rcx
    movq 24(%rsp),%rdx
    movq 16(%rsp),%r8
    movq 8(%rsp),%r9
    movq 0(%rsp),%r10
    addq $40,%rsp
    subq $48,%rsp
    movq %r10,32(%rsp)
    call vnt_gui_panel_color
    addq $48,%rsp
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_vanta_text_style
vnt_fn_vanta_text_style:
    pushq %rbp
    movq %rsp,%rbp
    subq $48,%rsp
    movq %rcx,-8(%rbp)
    movq %rdx,-16(%rbp)
    movq %r8,-24(%rbp)
    movq %r9,-32(%rbp)
    movq 48(%rbp),%r10
    movq %r10,-40(%rbp)
    movq -8(%rbp),%rax
    pushq %rax
    movq -16(%rbp),%rax
    pushq %rax
    movq -24(%rbp),%rax
    pushq %rax
    movq -32(%rbp),%rax
    pushq %rax
    movq -40(%rbp),%rax
    pushq %rax
    movq 32(%rsp),%rcx
    movq 24(%rsp),%rdx
    movq 16(%rsp),%r8
    movq 8(%rsp),%r9
    movq 0(%rsp),%r10
    addq $40,%rsp
    subq $48,%rsp
    movq %r10,32(%rsp)
    call vnt_gui_text_style
    addq $48,%rsp
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_note_title_path
vnt_fn_note_title_path:
    pushq %rbp
    movq %rsp,%rbp
    subq $16,%rsp
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
    call vnt_eq
    addq $32, %rsp
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L0
    lea .Lstr2(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    leave
    ret
    jmp .L1
.L0:
.L1:
    movq -8(%rbp),%rax
    pushq %rax
    movl $2,%ecx
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
    jz .L3
    lea .Lstr5(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    leave
    ret
    jmp .L4
.L3:
.L4:
    movq -8(%rbp),%rax
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
    jz .L6
    lea .Lstr8(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    leave
    ret
    jmp .L7
.L6:
.L7:
    lea .Lstr9(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_note_body_path
vnt_fn_note_body_path:
    pushq %rbp
    movq %rsp,%rbp
    subq $16,%rsp
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
    call vnt_eq
    addq $32, %rsp
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L10
    lea .Lstr12(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    leave
    ret
    jmp .L11
.L10:
.L11:
    movq -8(%rbp),%rax
    pushq %rax
    movl $2,%ecx
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
    jz .L13
    lea .Lstr15(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    leave
    ret
    jmp .L14
.L13:
.L14:
    movq -8(%rbp),%rax
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
    jz .L16
    lea .Lstr18(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    leave
    ret
    jmp .L17
.L16:
.L17:
    lea .Lstr19(%rip),%rcx
    subq $32, %rsp
    call vnt_string
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
    subq $112,%rsp
    movl $1,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,vnt_global_current_note(%rip)
    movq vnt_global_current_note(%rip),%rax
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_fn_note_title_path
    addq $32,%rsp
    movq %rax,vnt_global_title_path(%rip)
    movq vnt_global_current_note(%rip),%rax
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_fn_note_body_path
    addq $32,%rsp
    movq %rax,vnt_global_body_path(%rip)
    lea .Lstr20(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,vnt_global_saved_title(%rip)
    lea .Lstr21(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,vnt_global_saved_body(%rip)
    movq vnt_global_title_path(%rip),%rax
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_fs_exists
    addq $32, %rsp
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L22
    movq vnt_global_title_path(%rip),%rax
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_fs_read
    addq $32, %rsp
    movq %rax,vnt_global_saved_title(%rip)
    jmp .L23
.L22:
.L23:
    movq vnt_global_body_path(%rip),%rax
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_fs_exists
    addq $32, %rsp
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L24
    movq vnt_global_body_path(%rip),%rax
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_fs_read
    addq $32, %rsp
    movq %rax,vnt_global_saved_body(%rip)
    jmp .L25
.L24:
.L25:
    movq vnt_global_saved_title(%rip),%rax
    movq %rax,vnt_global_original_title(%rip)
    movq vnt_global_saved_body(%rip),%rax
    movq %rax,vnt_global_original_body(%rip)
    lea .Lstr28(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movl $1120,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $760,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    lea .Lstr29(%rip),%rcx
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
    jz .L26
    movq vnt_global_saved_title(%rip),%rax
    pushq %rax
    movl $290,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $186,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 16(%rsp), %rcx
    movq 8(%rsp), %rdx
    movq 0(%rsp), %r8
    subq $40,%rsp
    call vnt_fn_vanta_input_set
    addq $40,%rsp
    addq $24,%rsp
    movq vnt_global_saved_body(%rip),%rax
    pushq %rax
    movl $290,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $254,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 16(%rsp), %rcx
    movq 8(%rsp), %rdx
    movq 0(%rsp), %r8
    subq $40,%rsp
    call vnt_fn_vanta_input_set
    addq $40,%rsp
    addq $24,%rsp
    lea .Lstr30(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,-64(%rbp)
    movl $0,%ecx
    subq $32, %rsp
    call vnt_bool
    addq $32, %rsp
    movq %rax,-72(%rbp)
    movl $1,%ecx
    subq $32, %rsp
    call vnt_bool
    addq $32, %rsp
    movq %rax,-80(%rbp)
.L31:
    movq -80(%rbp),%rax
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L32
    subq $32,%rsp
    call vnt_fn_vanta_gui_poll
    addq $32,%rsp
    movq %rax,-80(%rbp)
    movl $0,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $0,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $248,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $760,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $1054762,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 32(%rsp), %rcx
    movq 24(%rsp), %rdx
    movq 16(%rsp), %r8
    movq 8(%rsp), %r9
    subq $40,%rsp
    movq 40(%rsp),%r10
    movq %r10,32(%rsp)
    call vnt_fn_vanta_surface
    addq $40,%rsp
    addq $40,%rsp
    movl $18,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $18,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $212,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $76,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $1516346,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 32(%rsp), %rcx
    movq 24(%rsp), %rdx
    movq 16(%rsp), %r8
    movq 8(%rsp), %r9
    subq $40,%rsp
    movq 40(%rsp),%r10
    movq %r10,32(%rsp)
    call vnt_fn_vanta_surface
    addq $40,%rsp
    addq $40,%rsp
    movl $270,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $18,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $830,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $104,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $1384244,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 32(%rsp), %rcx
    movq 24(%rsp), %rdx
    movq 16(%rsp), %r8
    movq 8(%rsp), %r9
    subq $40,%rsp
    movq 40(%rsp),%r10
    movq %r10,32(%rsp)
    call vnt_fn_vanta_surface
    addq $40,%rsp
    addq $40,%rsp
    movl $270,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $140,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $830,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $560,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $1384244,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 32(%rsp), %rcx
    movq 24(%rsp), %rdx
    movq 16(%rsp), %r8
    movq 8(%rsp), %r9
    subq $40,%rsp
    movq 40(%rsp),%r10
    movq %r10,32(%rsp)
    call vnt_fn_vanta_surface
    addq $40,%rsp
    addq $40,%rsp
    movl $286,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $154,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $798,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $2,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $3235048,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 32(%rsp), %rcx
    movq 24(%rsp), %rdx
    movq 16(%rsp), %r8
    movq 8(%rsp), %r9
    subq $40,%rsp
    movq 40(%rsp),%r10
    movq %r10,32(%rsp)
    call vnt_fn_vanta_surface
    addq $40,%rsp
    addq $40,%rsp
    lea .Lstr33(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movl $36,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $35,%ecx
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
    lea .Lstr34(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movl $37,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $70,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $8361668,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $11,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 32(%rsp), %rcx
    movq 24(%rsp), %rdx
    movq 16(%rsp), %r8
    movq 8(%rsp), %r9
    subq $40,%rsp
    movq 40(%rsp),%r10
    movq %r10,32(%rsp)
    call vnt_fn_vanta_text_style
    addq $40,%rsp
    addq $40,%rsp
    lea .Lstr35(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movl $30,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $128,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $8361668,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $11,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 32(%rsp), %rcx
    movq 24(%rsp), %rdx
    movq 16(%rsp), %r8
    movq 8(%rsp), %r9
    subq $40,%rsp
    movq 40(%rsp),%r10
    movq %r10,32(%rsp)
    call vnt_fn_vanta_text_style
    addq $40,%rsp
    addq $40,%rsp
    lea .Lstr36(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movl $30,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $157,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $14213880,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $13,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 32(%rsp), %rcx
    movq 24(%rsp), %rdx
    movq 16(%rsp), %r8
    movq 8(%rsp), %r9
    subq $40,%rsp
    movq 40(%rsp),%r10
    movq %r10,32(%rsp)
    call vnt_fn_vanta_text_style
    addq $40,%rsp
    addq $40,%rsp
    movl $24,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $195,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $200,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 16(%rsp), %rcx
    movq 8(%rsp), %rdx
    movq 0(%rsp), %r8
    subq $40,%rsp
    call vnt_fn_vanta_separator
    addq $40,%rsp
    addq $24,%rsp
    movq vnt_global_current_note(%rip),%rax
    pushq %rax
    movl $1,%ecx
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
    jz .L37
    movl $24,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $211,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $200,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $42,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $2505312,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 32(%rsp), %rcx
    movq 24(%rsp), %rdx
    movq 16(%rsp), %r8
    movq 8(%rsp), %r9
    subq $40,%rsp
    movq 40(%rsp),%r10
    movq %r10,32(%rsp)
    call vnt_fn_vanta_surface
    addq $40,%rsp
    addq $40,%rsp
    jmp .L38
.L37:
.L38:
    lea .Lstr41(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movl $30,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $213,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $188,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $38,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 32(%rsp), %rcx
    movq 24(%rsp), %rdx
    movq 16(%rsp), %r8
    movq 8(%rsp), %r9
    subq $40,%rsp
    movq 40(%rsp),%r10
    movq %r10,32(%rsp)
    call vnt_fn_vanta_button_sized
    addq $40,%rsp
    addq $40,%rsp
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L39
    movq -72(%rbp),%rax
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
    movq %rax,-64(%rbp)
    jmp .L43
.L42:
    movl $1,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,vnt_global_current_note(%rip)
    movq vnt_global_current_note(%rip),%rax
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_fn_note_title_path
    addq $32,%rsp
    movq %rax,vnt_global_title_path(%rip)
    movq vnt_global_current_note(%rip),%rax
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_fn_note_body_path
    addq $32,%rsp
    movq %rax,vnt_global_body_path(%rip)
    lea .Lstr20(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,vnt_global_saved_title(%rip)
    lea .Lstr21(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,vnt_global_saved_body(%rip)
    movq vnt_global_title_path(%rip),%rax
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_fs_exists
    addq $32, %rsp
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L45
    movq vnt_global_title_path(%rip),%rax
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_fs_read
    addq $32, %rsp
    movq %rax,vnt_global_saved_title(%rip)
    jmp .L46
.L45:
.L46:
    movq vnt_global_body_path(%rip),%rax
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_fs_exists
    addq $32, %rsp
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L47
    movq vnt_global_body_path(%rip),%rax
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_fs_read
    addq $32, %rsp
    movq %rax,vnt_global_saved_body(%rip)
    jmp .L48
.L47:
.L48:
    movq vnt_global_saved_title(%rip),%rax
    movq %rax,vnt_global_original_title(%rip)
    movq vnt_global_saved_body(%rip),%rax
    movq %rax,vnt_global_original_body(%rip)
    movq vnt_global_saved_title(%rip),%rax
    pushq %rax
    movl $290,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $186,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 16(%rsp), %rcx
    movq 8(%rsp), %rdx
    movq 0(%rsp), %r8
    subq $40,%rsp
    call vnt_fn_vanta_input_set
    addq $40,%rsp
    addq $24,%rsp
    movq vnt_global_saved_body(%rip),%rax
    pushq %rax
    movl $290,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $254,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 16(%rsp), %rcx
    movq 8(%rsp), %rdx
    movq 0(%rsp), %r8
    subq $40,%rsp
    call vnt_fn_vanta_input_set
    addq $40,%rsp
    addq $24,%rsp
    lea .Lstr49(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,-64(%rbp)
.L43:
    jmp .L40
.L39:
.L40:
    movq vnt_global_current_note(%rip),%rax
    pushq %rax
    movl $2,%ecx
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
    jz .L50
    movl $24,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $257,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $200,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $42,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $2505312,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 32(%rsp), %rcx
    movq 24(%rsp), %rdx
    movq 16(%rsp), %r8
    movq 8(%rsp), %r9
    subq $40,%rsp
    movq 40(%rsp),%r10
    movq %r10,32(%rsp)
    call vnt_fn_vanta_surface
    addq $40,%rsp
    addq $40,%rsp
    jmp .L51
.L50:
.L51:
    lea .Lstr54(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movl $30,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $259,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $188,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $38,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 32(%rsp), %rcx
    movq 24(%rsp), %rdx
    movq 16(%rsp), %r8
    movq 8(%rsp), %r9
    subq $40,%rsp
    movq 40(%rsp),%r10
    movq %r10,32(%rsp)
    call vnt_fn_vanta_button_sized
    addq $40,%rsp
    addq $40,%rsp
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L52
    movq -72(%rbp),%rax
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L55
    lea .Lstr44(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,-64(%rbp)
    jmp .L56
.L55:
    movl $2,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,vnt_global_current_note(%rip)
    movq vnt_global_current_note(%rip),%rax
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_fn_note_title_path
    addq $32,%rsp
    movq %rax,vnt_global_title_path(%rip)
    movq vnt_global_current_note(%rip),%rax
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_fn_note_body_path
    addq $32,%rsp
    movq %rax,vnt_global_body_path(%rip)
    lea .Lstr20(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,vnt_global_saved_title(%rip)
    lea .Lstr21(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,vnt_global_saved_body(%rip)
    movq vnt_global_title_path(%rip),%rax
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_fs_exists
    addq $32, %rsp
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L57
    movq vnt_global_title_path(%rip),%rax
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_fs_read
    addq $32, %rsp
    movq %rax,vnt_global_saved_title(%rip)
    jmp .L58
.L57:
.L58:
    movq vnt_global_body_path(%rip),%rax
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_fs_exists
    addq $32, %rsp
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L59
    movq vnt_global_body_path(%rip),%rax
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_fs_read
    addq $32, %rsp
    movq %rax,vnt_global_saved_body(%rip)
    jmp .L60
.L59:
.L60:
    movq vnt_global_saved_title(%rip),%rax
    movq %rax,vnt_global_original_title(%rip)
    movq vnt_global_saved_body(%rip),%rax
    movq %rax,vnt_global_original_body(%rip)
    movq vnt_global_saved_title(%rip),%rax
    pushq %rax
    movl $290,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $186,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 16(%rsp), %rcx
    movq 8(%rsp), %rdx
    movq 0(%rsp), %r8
    subq $40,%rsp
    call vnt_fn_vanta_input_set
    addq $40,%rsp
    addq $24,%rsp
    movq vnt_global_saved_body(%rip),%rax
    pushq %rax
    movl $290,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $254,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 16(%rsp), %rcx
    movq 8(%rsp), %rdx
    movq 0(%rsp), %r8
    subq $40,%rsp
    call vnt_fn_vanta_input_set
    addq $40,%rsp
    addq $24,%rsp
    lea .Lstr61(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,-64(%rbp)
.L56:
    jmp .L53
.L52:
.L53:
    movq vnt_global_current_note(%rip),%rax
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
    jz .L62
    movl $24,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $303,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $200,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $42,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $2505312,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 32(%rsp), %rcx
    movq 24(%rsp), %rdx
    movq 16(%rsp), %r8
    movq 8(%rsp), %r9
    subq $40,%rsp
    movq 40(%rsp),%r10
    movq %r10,32(%rsp)
    call vnt_fn_vanta_surface
    addq $40,%rsp
    addq $40,%rsp
    jmp .L63
.L62:
.L63:
    lea .Lstr66(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movl $30,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $305,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $188,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $38,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 32(%rsp), %rcx
    movq 24(%rsp), %rdx
    movq 16(%rsp), %r8
    movq 8(%rsp), %r9
    subq $40,%rsp
    movq 40(%rsp),%r10
    movq %r10,32(%rsp)
    call vnt_fn_vanta_button_sized
    addq $40,%rsp
    addq $40,%rsp
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L64
    movq -72(%rbp),%rax
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L67
    lea .Lstr44(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,-64(%rbp)
    jmp .L68
.L67:
    movl $3,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,vnt_global_current_note(%rip)
    movq vnt_global_current_note(%rip),%rax
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_fn_note_title_path
    addq $32,%rsp
    movq %rax,vnt_global_title_path(%rip)
    movq vnt_global_current_note(%rip),%rax
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_fn_note_body_path
    addq $32,%rsp
    movq %rax,vnt_global_body_path(%rip)
    lea .Lstr20(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,vnt_global_saved_title(%rip)
    lea .Lstr21(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,vnt_global_saved_body(%rip)
    movq vnt_global_title_path(%rip),%rax
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_fs_exists
    addq $32, %rsp
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L69
    movq vnt_global_title_path(%rip),%rax
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_fs_read
    addq $32, %rsp
    movq %rax,vnt_global_saved_title(%rip)
    jmp .L70
.L69:
.L70:
    movq vnt_global_body_path(%rip),%rax
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_fs_exists
    addq $32, %rsp
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L71
    movq vnt_global_body_path(%rip),%rax
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_fs_read
    addq $32, %rsp
    movq %rax,vnt_global_saved_body(%rip)
    jmp .L72
.L71:
.L72:
    movq vnt_global_saved_title(%rip),%rax
    movq %rax,vnt_global_original_title(%rip)
    movq vnt_global_saved_body(%rip),%rax
    movq %rax,vnt_global_original_body(%rip)
    movq vnt_global_saved_title(%rip),%rax
    pushq %rax
    movl $290,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $186,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 16(%rsp), %rcx
    movq 8(%rsp), %rdx
    movq 0(%rsp), %r8
    subq $40,%rsp
    call vnt_fn_vanta_input_set
    addq $40,%rsp
    addq $24,%rsp
    movq vnt_global_saved_body(%rip),%rax
    pushq %rax
    movl $290,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $254,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 16(%rsp), %rcx
    movq 8(%rsp), %rdx
    movq 0(%rsp), %r8
    subq $40,%rsp
    call vnt_fn_vanta_input_set
    addq $40,%rsp
    addq $24,%rsp
    lea .Lstr73(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,-64(%rbp)
.L68:
    jmp .L65
.L64:
.L65:
    movq vnt_global_current_note(%rip),%rax
    pushq %rax
    movl $4,%ecx
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
    jz .L74
    movl $24,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $349,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $200,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $42,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $2505312,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 32(%rsp), %rcx
    movq 24(%rsp), %rdx
    movq 16(%rsp), %r8
    movq 8(%rsp), %r9
    subq $40,%rsp
    movq 40(%rsp),%r10
    movq %r10,32(%rsp)
    call vnt_fn_vanta_surface
    addq $40,%rsp
    addq $40,%rsp
    jmp .L75
.L74:
.L75:
    lea .Lstr78(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movl $30,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $351,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $188,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $38,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 32(%rsp), %rcx
    movq 24(%rsp), %rdx
    movq 16(%rsp), %r8
    movq 8(%rsp), %r9
    subq $40,%rsp
    movq 40(%rsp),%r10
    movq %r10,32(%rsp)
    call vnt_fn_vanta_button_sized
    addq $40,%rsp
    addq $40,%rsp
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L76
    movq -72(%rbp),%rax
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L79
    lea .Lstr44(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,-64(%rbp)
    jmp .L80
.L79:
    movl $4,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,vnt_global_current_note(%rip)
    movq vnt_global_current_note(%rip),%rax
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_fn_note_title_path
    addq $32,%rsp
    movq %rax,vnt_global_title_path(%rip)
    movq vnt_global_current_note(%rip),%rax
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_fn_note_body_path
    addq $32,%rsp
    movq %rax,vnt_global_body_path(%rip)
    lea .Lstr20(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,vnt_global_saved_title(%rip)
    lea .Lstr21(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,vnt_global_saved_body(%rip)
    movq vnt_global_title_path(%rip),%rax
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_fs_exists
    addq $32, %rsp
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L81
    movq vnt_global_title_path(%rip),%rax
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_fs_read
    addq $32, %rsp
    movq %rax,vnt_global_saved_title(%rip)
    jmp .L82
.L81:
.L82:
    movq vnt_global_body_path(%rip),%rax
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_fs_exists
    addq $32, %rsp
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L83
    movq vnt_global_body_path(%rip),%rax
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_fs_read
    addq $32, %rsp
    movq %rax,vnt_global_saved_body(%rip)
    jmp .L84
.L83:
.L84:
    movq vnt_global_saved_title(%rip),%rax
    movq %rax,vnt_global_original_title(%rip)
    movq vnt_global_saved_body(%rip),%rax
    movq %rax,vnt_global_original_body(%rip)
    movq vnt_global_saved_title(%rip),%rax
    pushq %rax
    movl $290,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $186,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 16(%rsp), %rcx
    movq 8(%rsp), %rdx
    movq 0(%rsp), %r8
    subq $40,%rsp
    call vnt_fn_vanta_input_set
    addq $40,%rsp
    addq $24,%rsp
    movq vnt_global_saved_body(%rip),%rax
    pushq %rax
    movl $290,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $254,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 16(%rsp), %rcx
    movq 8(%rsp), %rdx
    movq 0(%rsp), %r8
    subq $40,%rsp
    call vnt_fn_vanta_input_set
    addq $40,%rsp
    addq $24,%rsp
    lea .Lstr85(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,-64(%rbp)
.L80:
    jmp .L77
.L76:
.L77:
    movl $24,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $414,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $200,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 16(%rsp), %rcx
    movq 8(%rsp), %rdx
    movq 0(%rsp), %r8
    subq $40,%rsp
    call vnt_fn_vanta_separator
    addq $40,%rsp
    addq $24,%rsp
    lea .Lstr86(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movl $30,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $438,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $8361668,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $11,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 32(%rsp), %rcx
    movq 24(%rsp), %rdx
    movq 16(%rsp), %r8
    movq 8(%rsp), %r9
    subq $40,%rsp
    movq 40(%rsp),%r10
    movq %r10,32(%rsp)
    call vnt_fn_vanta_text_style
    addq $40,%rsp
    addq $40,%rsp
    lea .Lstr87(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movl $30,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $466,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $14213880,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $12,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 32(%rsp), %rcx
    movq 24(%rsp), %rdx
    movq 16(%rsp), %r8
    movq 8(%rsp), %r9
    subq $40,%rsp
    movq 40(%rsp),%r10
    movq %r10,32(%rsp)
    call vnt_fn_vanta_text_style
    addq $40,%rsp
    addq $40,%rsp
    lea .Lstr88(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movl $30,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $489,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $11187923,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $11,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 32(%rsp), %rcx
    movq 24(%rsp), %rdx
    movq 16(%rsp), %r8
    movq 8(%rsp), %r9
    subq $40,%rsp
    movq 40(%rsp),%r10
    movq %r10,32(%rsp)
    call vnt_fn_vanta_text_style
    addq $40,%rsp
    addq $40,%rsp
    lea .Lstr89(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movl $30,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $514,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $11187923,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $11,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 32(%rsp), %rcx
    movq 24(%rsp), %rdx
    movq 16(%rsp), %r8
    movq 8(%rsp), %r9
    subq $40,%rsp
    movq 40(%rsp),%r10
    movq %r10,32(%rsp)
    call vnt_fn_vanta_text_style
    addq $40,%rsp
    addq $40,%rsp
    lea .Lstr90(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movl $30,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $532,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $11187923,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $11,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 32(%rsp), %rcx
    movq 24(%rsp), %rdx
    movq 16(%rsp), %r8
    movq 8(%rsp), %r9
    subq $40,%rsp
    movq 40(%rsp),%r10
    movq %r10,32(%rsp)
    call vnt_fn_vanta_text_style
    addq $40,%rsp
    addq $40,%rsp
    movl $24,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $632,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $200,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $78,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $1714500,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 32(%rsp), %rcx
    movq 24(%rsp), %rdx
    movq 16(%rsp), %r8
    movq 8(%rsp), %r9
    subq $40,%rsp
    movq 40(%rsp),%r10
    movq %r10,32(%rsp)
    call vnt_fn_vanta_surface
    addq $40,%rsp
    addq $40,%rsp
    lea .Lstr91(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movl $38,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $648,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $6477488,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $11,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 32(%rsp), %rcx
    movq 24(%rsp), %rdx
    movq 16(%rsp), %r8
    movq 8(%rsp), %r9
    subq $40,%rsp
    movq 40(%rsp),%r10
    movq %r10,32(%rsp)
    call vnt_fn_vanta_text_style
    addq $40,%rsp
    addq $40,%rsp
    lea .Lstr92(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movl $38,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $674,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $11187923,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $10,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 32(%rsp), %rcx
    movq 24(%rsp), %rdx
    movq 16(%rsp), %r8
    movq 8(%rsp), %r9
    subq $40,%rsp
    movq 40(%rsp),%r10
    movq %r10,32(%rsp)
    call vnt_fn_vanta_text_style
    addq $40,%rsp
    addq $40,%rsp
    lea .Lstr93(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movl $300,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $42,%ecx
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
    lea .Lstr94(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movl $302,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $84,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $11187923,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $13,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 32(%rsp), %rcx
    movq 24(%rsp), %rdx
    movq 16(%rsp), %r8
    movq 8(%rsp), %r9
    subq $40,%rsp
    movq 40(%rsp),%r10
    movq %r10,32(%rsp)
    call vnt_fn_vanta_text_style
    addq $40,%rsp
    addq $40,%rsp
    lea .Lstr95(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movl $302,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $172,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $8361668,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $11,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 32(%rsp), %rcx
    movq 24(%rsp), %rdx
    movq 16(%rsp), %r8
    movq 8(%rsp), %r9
    subq $40,%rsp
    movq 40(%rsp),%r10
    movq %r10,32(%rsp)
    call vnt_fn_vanta_text_style
    addq $40,%rsp
    addq $40,%rsp
    lea .Lstr96(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movl $290,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $186,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $790,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 24(%rsp), %rcx
    movq 16(%rsp), %rdx
    movq 8(%rsp), %r8
    movq 0(%rsp), %r9
    subq $32,%rsp
    call vnt_fn_vanta_text_input
    addq $32,%rsp
    addq $32,%rsp
    movq %rax,-88(%rbp)
    lea .Lstr97(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movl $302,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $229,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $8361668,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $11,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 32(%rsp), %rcx
    movq 24(%rsp), %rdx
    movq 16(%rsp), %r8
    movq 8(%rsp), %r9
    subq $40,%rsp
    movq 40(%rsp),%r10
    movq %r10,32(%rsp)
    call vnt_fn_vanta_text_style
    addq $40,%rsp
    addq $40,%rsp
    lea .Lstr98(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movl $290,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $254,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $790,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $330,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 32(%rsp), %rcx
    movq 24(%rsp), %rdx
    movq 16(%rsp), %r8
    movq 8(%rsp), %r9
    subq $40,%rsp
    movq 40(%rsp),%r10
    movq %r10,32(%rsp)
    call vnt_fn_vanta_textarea
    addq $40,%rsp
    addq $40,%rsp
    movq %rax,-96(%rbp)
    movq -88(%rbp),%rax
    pushq %rax
    movq vnt_global_original_title(%rip),%rax
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
    jnz .L99
    movq -96(%rbp),%rax
    pushq %rax
    movq vnt_global_original_body(%rip),%rax
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_ne
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
    jmp .L100
.L99:
    movl $1,%eax
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_bool
    addq $32, %rsp
.L100:
    movq %rax,-72(%rbp)
    movl $302,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $604,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $770,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 16(%rsp), %rcx
    movq 8(%rsp), %rdx
    movq 0(%rsp), %r8
    subq $40,%rsp
    call vnt_fn_vanta_separator
    addq $40,%rsp
    addq $24,%rsp
    movq -64(%rbp),%rax
    pushq %rax
    movl $302,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $620,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $11187923,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $12,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 32(%rsp), %rcx
    movq 24(%rsp), %rdx
    movq 16(%rsp), %r8
    movq 8(%rsp), %r9
    subq $40,%rsp
    movq 40(%rsp),%r10
    movq %r10,32(%rsp)
    call vnt_fn_vanta_text_style
    addq $40,%rsp
    addq $40,%rsp
    lea .Lstr101(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movq vnt_global_current_note(%rip),%rax
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_add
    addq $32, %rsp
    pushq %rax
    lea .Lstr102(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_add
    addq $32, %rsp
    pushq %rax
    movl $302,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $651,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $8361668,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $11,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 32(%rsp), %rcx
    movq 24(%rsp), %rdx
    movq 16(%rsp), %r8
    movq 8(%rsp), %r9
    subq $40,%rsp
    movq 40(%rsp),%r10
    movq %r10,32(%rsp)
    call vnt_fn_vanta_text_style
    addq $40,%rsp
    addq $40,%rsp
    lea .Lstr105(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movl $302,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $670,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $132,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $38,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 32(%rsp), %rcx
    movq 24(%rsp), %rdx
    movq 16(%rsp), %r8
    movq 8(%rsp), %r9
    subq $40,%rsp
    movq 40(%rsp),%r10
    movq %r10,32(%rsp)
    call vnt_fn_vanta_button_sized
    addq $40,%rsp
    addq $40,%rsp
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L103
    movq vnt_global_title_path(%rip),%rax
    pushq %rax
    movq -88(%rbp),%rax
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_fs_write
    addq $32, %rsp
    movq %rax,-104(%rbp)
    movq vnt_global_body_path(%rip),%rax
    pushq %rax
    movq -96(%rbp),%rax
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_fs_write
    addq $32, %rsp
    movq %rax,-112(%rbp)
    movq -104(%rbp),%rax
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L108
    movq -112(%rbp),%rax
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
    jmp .L109
.L108:
    movl $0,%eax
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_bool
    addq $32, %rsp
.L109:
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L106
    movq -88(%rbp),%rax
    movq %rax,vnt_global_original_title(%rip)
    movq -96(%rbp),%rax
    movq %rax,vnt_global_original_body(%rip)
    lea .Lstr110(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movq vnt_global_current_note(%rip),%rax
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_add
    addq $32, %rsp
    pushq %rax
    lea .Lstr111(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_add
    addq $32, %rsp
    movq %rax,-64(%rbp)
    jmp .L107
.L106:
    lea .Lstr112(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,-64(%rbp)
.L107:
    jmp .L104
.L103:
.L104:
    lea .Lstr115(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movl $446,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $670,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $132,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $38,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 32(%rsp), %rcx
    movq 24(%rsp), %rdx
    movq 16(%rsp), %r8
    movq 8(%rsp), %r9
    subq $40,%rsp
    movq 40(%rsp),%r10
    movq %r10,32(%rsp)
    call vnt_fn_vanta_button_sized
    addq $40,%rsp
    addq $40,%rsp
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L113
    lea .Lstr20(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,-88(%rbp)
    lea .Lstr21(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,-96(%rbp)
    movq -88(%rbp),%rax
    pushq %rax
    movl $290,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $186,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 16(%rsp), %rcx
    movq 8(%rsp), %rdx
    movq 0(%rsp), %r8
    subq $40,%rsp
    call vnt_fn_vanta_input_set
    addq $40,%rsp
    addq $24,%rsp
    movq -96(%rbp),%rax
    pushq %rax
    movl $290,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $254,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 16(%rsp), %rcx
    movq 8(%rsp), %rdx
    movq 0(%rsp), %r8
    subq $40,%rsp
    call vnt_fn_vanta_input_set
    addq $40,%rsp
    addq $24,%rsp
    lea .Lstr116(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,-64(%rbp)
    jmp .L114
.L113:
.L114:
    lea .Lstr119(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movl $590,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $670,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $132,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $38,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 32(%rsp), %rcx
    movq 24(%rsp), %rdx
    movq 16(%rsp), %r8
    movq 8(%rsp), %r9
    subq $40,%rsp
    movq 40(%rsp),%r10
    movq %r10,32(%rsp)
    call vnt_fn_vanta_button_sized
    addq $40,%rsp
    addq $40,%rsp
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L117
    lea .Lstr20(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,vnt_global_saved_title(%rip)
    lea .Lstr21(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,vnt_global_saved_body(%rip)
    movq vnt_global_title_path(%rip),%rax
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_fs_exists
    addq $32, %rsp
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L120
    movq vnt_global_title_path(%rip),%rax
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_fs_read
    addq $32, %rsp
    movq %rax,vnt_global_saved_title(%rip)
    jmp .L121
.L120:
.L121:
    movq vnt_global_body_path(%rip),%rax
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_fs_exists
    addq $32, %rsp
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L122
    movq vnt_global_body_path(%rip),%rax
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_fs_read
    addq $32, %rsp
    movq %rax,vnt_global_saved_body(%rip)
    jmp .L123
.L122:
.L123:
    movq vnt_global_saved_title(%rip),%rax
    movq %rax,vnt_global_original_title(%rip)
    movq vnt_global_saved_body(%rip),%rax
    movq %rax,vnt_global_original_body(%rip)
    movq vnt_global_saved_title(%rip),%rax
    pushq %rax
    movl $290,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $186,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 16(%rsp), %rcx
    movq 8(%rsp), %rdx
    movq 0(%rsp), %r8
    subq $40,%rsp
    call vnt_fn_vanta_input_set
    addq $40,%rsp
    addq $24,%rsp
    movq vnt_global_saved_body(%rip),%rax
    pushq %rax
    movl $290,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $254,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movq 16(%rsp), %rcx
    movq 8(%rsp), %rdx
    movq 0(%rsp), %r8
    subq $40,%rsp
    call vnt_fn_vanta_input_set
    addq $40,%rsp
    addq $24,%rsp
    movq vnt_global_saved_title(%rip),%rax
    movq %rax,vnt_global_original_title(%rip)
    movq vnt_global_saved_body(%rip),%rax
    movq %rax,vnt_global_original_body(%rip)
    lea .Lstr124(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movq vnt_global_current_note(%rip),%rax
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_add
    addq $32, %rsp
    pushq %rax
    lea .Lstr125(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_add
    addq $32, %rsp
    movq %rax,-64(%rbp)
    jmp .L118
.L117:
.L118:
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
    jmp .L31
.L32:
    subq $32,%rsp
    call vnt_fn_vanta_gui_close
    addq $32,%rsp
    jmp .L27
.L26:
    lea .Lstr126(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
.L27:
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
    .asciz "vnt_notes_1_title.txt"
.Lstr5:
    .asciz "vnt_notes_2_title.txt"
.Lstr8:
    .asciz "vnt_notes_3_title.txt"
.Lstr9:
    .asciz "vnt_notes_4_title.txt"
.Lstr12:
    .asciz "vnt_notes_1_body.txt"
.Lstr15:
    .asciz "vnt_notes_2_body.txt"
.Lstr18:
    .asciz "vnt_notes_3_body.txt"
.Lstr19:
    .asciz "vnt_notes_4_body.txt"
.Lstr20:
    .asciz "Untitled note"
.Lstr21:
    .asciz ""
.Lstr28:
    .asciz "VNT Notes  |  Private by default"
.Lstr29:
    .asciz "examples/vnt_notes.css"
.Lstr30:
    .asciz "Ready. Your notes stay on this device."
.Lstr33:
    .asciz "VNT NOTES"
.Lstr34:
    .asciz "LOCAL WRITING SPACE"
.Lstr35:
    .asciz "YOUR NOTEBOOK"
.Lstr36:
    .asciz "Pick a note to open it."
.Lstr41:
    .asciz "01   Quick note"
.Lstr44:
    .asciz "Unsaved changes. Save this note before switching slots."
.Lstr49:
    .asciz "Opened note 1."
.Lstr54:
    .asciz "02   Project ideas"
.Lstr61:
    .asciz "Opened note 2."
.Lstr66:
    .asciz "03   To remember"
.Lstr73:
    .asciz "Opened note 3."
.Lstr78:
    .asciz "04   Scratch space"
.Lstr85:
    .asciz "Opened note 4."
.Lstr86:
    .asciz "STORAGE"
.Lstr87:
    .asciz "Plain text files"
.Lstr88:
    .asciz "Portable and readable"
.Lstr89:
    .asciz "Saved in the current"
.Lstr90:
    .asciz "working directory."
.Lstr91:
    .asciz "OFFLINE BY DESIGN"
.Lstr92:
    .asciz "No accounts or telemetry."
.Lstr93:
    .asciz "A quieter place to think."
.Lstr94:
    .asciz "Four local notes. Nothing leaves your computer."
.Lstr95:
    .asciz "NOTE TITLE"
.Lstr96:
    .asciz "Give this note a title"
.Lstr97:
    .asciz "NOTE CONTENT"
.Lstr98:
    .asciz "Start writing here... Enter creates a new line."
.Lstr101:
    .asciz "Slot "
.Lstr102:
    .asciz "  /  Plain text  /  Local-first"
.Lstr105:
    .asciz "Save note"
.Lstr110:
    .asciz "Saved note "
.Lstr111:
    .asciz ". Both files are up to date."
.Lstr112:
    .asciz "Save failed. Check write permissions for this folder."
.Lstr115:
    .asciz "Clear draft"
.Lstr116:
    .asciz "Draft cleared. Existing saved files are unchanged until Save."
.Lstr119:
    .asciz "Reload saved"
.Lstr124:
    .asciz "Reloaded note "
.Lstr125:
    .asciz " from disk."
.Lstr126:
    .asciz "Could not create the VNT Notes window."
