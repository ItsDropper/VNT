.text
.globl main
main:
    pushq %rbp
    movq %rsp,%rbp
    subq $48,%rsp
    subq $32, %rsp
    call vnt_array_new
    addq $32, %rsp
    pushq %rax
    lea .Lstr0(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_array_push
    addq $32, %rsp
    pushq %rax
    lea .Lstr1(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_array_push
    addq $32, %rsp
    movq %rax,-8(%rbp)
    lea .Lstr2(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movq -8(%rbp),%rax
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_process_start
    addq $32, %rsp
    movq %rax,-16(%rbp)
    movq -16(%rbp),%rax
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_process_pid
    addq $32, %rsp
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
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movq -16(%rbp),%rax
    pushq %rax
    movl $5000,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_process_wait
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movq -16(%rbp),%rax
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_process_stdout
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movq -16(%rbp),%rax
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_process_stderr
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movq -16(%rbp),%rax
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_process_exit_code
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movq -16(%rbp),%rax
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_process_poll
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    subq $32, %rsp
    call vnt_array_new
    addq $32, %rsp
    pushq %rax
    lea .Lstr0(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_array_push
    addq $32, %rsp
    pushq %rax
    lea .Lstr3(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_array_push
    addq $32, %rsp
    movq %rax,-24(%rbp)
    lea .Lstr2(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movq -24(%rbp),%rax
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_process_start
    addq $32, %rsp
    movq %rax,-32(%rbp)
    movq -32(%rbp),%rax
    pushq %rax
    movl $5000,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_process_wait
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movq -32(%rbp),%rax
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_process_stdout
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movq -32(%rbp),%rax
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_process_stderr
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movq -32(%rbp),%rax
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_process_exit_code
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    subq $32, %rsp
    call vnt_array_new
    addq $32, %rsp
    pushq %rax
    lea .Lstr0(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_array_push
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
    movq %rax,-40(%rbp)
    lea .Lstr2(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    pushq %rax
    movq -40(%rbp),%rax
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_process_start
    addq $32, %rsp
    movq %rax,-48(%rbp)
    movq -48(%rbp),%rax
    pushq %rax
    movl $10,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_process_wait
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movq -48(%rbp),%rax
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_process_terminate
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movq -48(%rbp),%rax
    pushq %rax
    movl $5000,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_process_wait
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
    .asciz "/c"
.Lstr1:
    .asciz "echo VNT_PROCESS_STDOUT_OK"
.Lstr2:
    .asciz "cmd.exe"
.Lstr3:
    .asciz "echo VNT_PROCESS_STDERR_OK 1>&2 & exit /b 7"
.Lstr4:
    .asciz "timeout /t 10 /nobreak > nul"
.Lstr5:
    .asciz "NATIVE_PROCESS_API_OK"
