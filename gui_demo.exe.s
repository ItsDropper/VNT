.text
.globl main
main:
    pushq %rbp
    movq %rsp,%rbp
    subq $16,%rsp
    movl $900,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $620,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_gui_size
    addq $32, %rsp
    lea .Lstr0(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_gui_css
    addq $32, %rsp
    lea .Lstr3(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_gui_open
    addq $32, %rsp
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L1
    lea .Lstr4(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_gui_text
    addq $32, %rsp
    lea .Lstr5(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_gui_text
    addq $32, %rsp
    lea .Lstr6(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_gui_text
    addq $32, %rsp
    movl $1,%ecx
    subq $32, %rsp
    call vnt_bool
    addq $32, %rsp
    movq %rax,-8(%rbp)
.L7:
    movq -8(%rbp),%rax
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_truth
    addq $32, %rsp
    testl %eax,%eax
    jz .L8
    subq $32, %rsp
    call vnt_gui_poll
    addq $32, %rsp
    movq %rax,-8(%rbp)
    lea .Lstr11(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movl $32,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $120,%ecx
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
    jz .L9
    lea .Lstr12(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_gui_text
    addq $32, %rsp
    jmp .L10
.L9:
.L10:
    lea .Lstr15(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movl $32,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $174,%ecx
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
    jz .L13
    lea .Lstr16(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_gui_text
    addq $32, %rsp
    jmp .L14
.L13:
.L14:
    lea .Lstr19(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movl $32,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    pushq %rax
    movl $228,%ecx
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
    jz .L17
    movl $0,%ecx
    subq $32, %rsp
    call vnt_bool
    addq $32, %rsp
    movq %rax,-8(%rbp)
    jmp .L18
.L17:
.L18:
    movl $16,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_sleep_ms
    addq $32, %rsp
    jmp .L7
.L8:
    subq $32, %rsp
    call vnt_gui_close
    addq $32, %rsp
    jmp .L2
.L1:
    lea .Lstr20(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
.L2:
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
    .asciz "examples/gui_demo.css"
.Lstr3:
    .asciz "VNT GUI Demo"
.Lstr4:
    .asciz "VNT native GUI"
.Lstr5:
    .asciz "This window is styled with a CSS-like stylesheet."
.Lstr6:
    .asciz "Click a button to run its VNT code."
.Lstr11:
    .asciz "Say hello"
.Lstr12:
    .asciz "Hello from a clicked VNT button."
.Lstr15:
    .asciz "Change status"
.Lstr16:
    .asciz "Status changed: button events work."
.Lstr19:
    .asciz "Exit"
.Lstr20:
    .asciz "Could not create the VNT GUI window."
