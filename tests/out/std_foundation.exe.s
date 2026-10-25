.text
.globl vnt_fn_math__square
vnt_fn_math__square:
    pushq %rbp
    movq %rsp,%rbp
    subq $16,%rsp
    movq %rcx,-8(%rbp)
    movq -8(%rbp),%rax
    pushq %rax
    movq -8(%rbp),%rax
    movq %rax,%rdx
    popq %rcx
    testq %rcx,%rcx
    jz .L0
    testq %rdx,%rdx
    jz .L0
    cmpl $1,0(%rcx)
    jne .L0
    cmpl $1,0(%rdx)
    jne .L0
    movl 8(%rcx),%r10d
    imull 8(%rdx),%r10d
    jo .L0
    movl %r10d,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    jmp .L1
.L0:
    subq $32,%rsp
    call vnt_mul
    addq $32,%rsp
.L1:
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_math__clamp
vnt_fn_math__clamp:
    pushq %rbp
    movq %rsp,%rbp
    subq $32,%rsp
    movq %rcx,-8(%rbp)
    movq %rdx,-16(%rbp)
    movq %r8,-24(%rbp)
    movq -8(%rbp),%rax
    pushq %rax
    movq -16(%rbp),%rax
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
    jz .L2
    movq -16(%rbp),%rax
    leave
    ret
    jmp .L3
.L2:
    movq -8(%rbp),%rax
    pushq %rax
    movq -24(%rbp),%rax
    movq %rax,%rdx
    popq %rcx
    subq $32,%rsp
    call vnt_gt
    addq $32,%rsp
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_truth
    addq $32,%rsp
    testl %eax,%eax
    jz .L4
    movq -24(%rbp),%rax
    leave
    ret
    jmp .L5
.L4:
    movq -8(%rbp),%rax
    leave
    ret
.L5:
.L3:
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_math__lerp
vnt_fn_math__lerp:
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
    movq -8(%rbp),%rax
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
    subl 8(%rdx),%r10d
    jo .L6
    movl %r10d,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    jmp .L7
.L6:
    subq $32,%rsp
    call vnt_sub
    addq $32,%rsp
.L7:
    pushq %rax
    movq -24(%rbp),%rax
    movq %rax,%rdx
    popq %rcx
    testq %rcx,%rcx
    jz .L8
    testq %rdx,%rdx
    jz .L8
    cmpl $1,0(%rcx)
    jne .L8
    cmpl $1,0(%rdx)
    jne .L8
    movl 8(%rcx),%r10d
    imull 8(%rdx),%r10d
    jo .L8
    movl %r10d,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    jmp .L9
.L8:
    subq $32,%rsp
    call vnt_mul
    addq $32,%rsp
.L9:
    movq %rax,%rdx
    popq %rcx
    testq %rcx,%rcx
    jz .L10
    testq %rdx,%rdx
    jz .L10
    cmpl $1,0(%rcx)
    jne .L10
    cmpl $1,0(%rdx)
    jne .L10
    movl 8(%rcx),%r10d
    addl 8(%rdx),%r10d
    jo .L10
    movl %r10d,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    jmp .L11
.L10:
    subq $32,%rsp
    call vnt_add
    addq $32,%rsp
.L11:
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_math__sqrt
vnt_fn_math__sqrt:
    pushq %rbp
    movq %rsp,%rbp
    subq $16,%rsp
    movq %rcx,-8(%rbp)
    movq -8(%rbp),%rax
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_sqrt
    addq $32,%rsp
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_math__sin
vnt_fn_math__sin:
    pushq %rbp
    movq %rsp,%rbp
    subq $16,%rsp
    movq %rcx,-8(%rbp)
    movq -8(%rbp),%rax
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_sin
    addq $32,%rsp
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_math__cos
vnt_fn_math__cos:
    pushq %rbp
    movq %rsp,%rbp
    subq $16,%rsp
    movq %rcx,-8(%rbp)
    movq -8(%rbp),%rax
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_cos
    addq $32,%rsp
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_math__tan
vnt_fn_math__tan:
    pushq %rbp
    movq %rsp,%rbp
    subq $16,%rsp
    movq %rcx,-8(%rbp)
    movq -8(%rbp),%rax
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_tan
    addq $32,%rsp
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_math__abs
vnt_fn_math__abs:
    pushq %rbp
    movq %rsp,%rbp
    subq $16,%rsp
    movq %rcx,-8(%rbp)
    movq -8(%rbp),%rax
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_abs
    addq $32,%rsp
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_math__floor
vnt_fn_math__floor:
    pushq %rbp
    movq %rsp,%rbp
    subq $16,%rsp
    movq %rcx,-8(%rbp)
    movq -8(%rbp),%rax
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_floor
    addq $32,%rsp
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_math__ceil
vnt_fn_math__ceil:
    pushq %rbp
    movq %rsp,%rbp
    subq $16,%rsp
    movq %rcx,-8(%rbp)
    movq -8(%rbp),%rax
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_ceil
    addq $32,%rsp
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_math__min
vnt_fn_math__min:
    pushq %rbp
    movq %rsp,%rbp
    subq $16,%rsp
    movq %rcx,-8(%rbp)
    movq %rdx,-16(%rbp)
    movq -8(%rbp),%rax
    pushq %rax
    movq -16(%rbp),%rax
    pushq %rax
    movq 8(%rsp),%rcx
    movq 0(%rsp),%rdx
    addq $16,%rsp
    subq $32,%rsp
    call vnt_min
    addq $32,%rsp
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_math__max
vnt_fn_math__max:
    pushq %rbp
    movq %rsp,%rbp
    subq $16,%rsp
    movq %rcx,-8(%rbp)
    movq %rdx,-16(%rbp)
    movq -8(%rbp),%rax
    pushq %rax
    movq -16(%rbp),%rax
    pushq %rax
    movq 8(%rsp),%rcx
    movq 0(%rsp),%rdx
    addq $16,%rsp
    subq $32,%rsp
    call vnt_max
    addq $32,%rsp
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_math__mod
vnt_fn_math__mod:
    pushq %rbp
    movq %rsp,%rbp
    subq $16,%rsp
    movq %rcx,-8(%rbp)
    movq %rdx,-16(%rbp)
    movq -8(%rbp),%rax
    pushq %rax
    movq -16(%rbp),%rax
    pushq %rax
    movq 8(%rsp),%rcx
    movq 0(%rsp),%rdx
    addq $16,%rsp
    subq $32,%rsp
    call vnt_mod
    addq $32,%rsp
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_time__now_ms
vnt_fn_time__now_ms:
    pushq %rbp
    movq %rsp,%rbp
    subq $32,%rsp
    call vnt_time_ms
    addq $32,%rsp
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_time__sleep_ms
vnt_fn_time__sleep_ms:
    pushq %rbp
    movq %rsp,%rbp
    subq $16,%rsp
    movq %rcx,-8(%rbp)
    movq -8(%rbp),%rax
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_sleep_ms
    addq $32,%rsp
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_time__elapsed_ms
vnt_fn_time__elapsed_ms:
    pushq %rbp
    movq %rsp,%rbp
    subq $16,%rsp
    movq %rcx,-8(%rbp)
    subq $32,%rsp
    call vnt_time_ms
    addq $32,%rsp
    pushq %rax
    movq -8(%rbp),%rax
    movq %rax,%rdx
    popq %rcx
    testq %rcx,%rcx
    jz .L12
    testq %rdx,%rdx
    jz .L12
    cmpl $1,0(%rcx)
    jne .L12
    cmpl $1,0(%rdx)
    jne .L12
    movl 8(%rcx),%r10d
    subl 8(%rdx),%r10d
    jo .L12
    movl %r10d,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    jmp .L13
.L12:
    subq $32,%rsp
    call vnt_sub
    addq $32,%rsp
.L13:
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_time__seconds_to_ms
vnt_fn_time__seconds_to_ms:
    pushq %rbp
    movq %rsp,%rbp
    subq $16,%rsp
    movq %rcx,-8(%rbp)
    movq -8(%rbp),%rax
    pushq %rax
    movq (%rsp),%r11
    testq %r11,%r11
    jz .L14
    cmpl $1,0(%r11)
    jne .L14
    movl 8(%r11),%r10d
    imull $1000,%r10d,%r10d
    jo .L14
    movl %r10d,%ecx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    jmp .L15
.L14:
    movl $1000,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    movq %rax,%rdx
    popq %rcx
    subq $32,%rsp
    call vnt_mul
    addq $32,%rsp
.L15:
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_time__ms_to_seconds
vnt_fn_time__ms_to_seconds:
    pushq %rbp
    movq %rsp,%rbp
    subq $16,%rsp
    movq %rcx,-8(%rbp)
    movq -8(%rbp),%rax
    pushq %rax
    movl $1000,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    movq %rax,%rdx
    popq %rcx
    subq $32,%rsp
    call vnt_div
    addq $32,%rsp
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_os__current_dir
vnt_fn_os__current_dir:
    pushq %rbp
    movq %rsp,%rbp
    subq $32,%rsp
    call vnt_cwd
    addq $32,%rsp
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_os__env_get
vnt_fn_os__env_get:
    pushq %rbp
    movq %rsp,%rbp
    subq $16,%rsp
    movq %rcx,-8(%rbp)
    movq -8(%rbp),%rax
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_env_get
    addq $32,%rsp
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_os__env_set
vnt_fn_os__env_set:
    pushq %rbp
    movq %rsp,%rbp
    subq $16,%rsp
    movq %rcx,-8(%rbp)
    movq %rdx,-16(%rbp)
    movq -8(%rbp),%rax
    pushq %rax
    movq -16(%rbp),%rax
    pushq %rax
    movq 8(%rsp),%rcx
    movq 0(%rsp),%rdx
    addq $16,%rsp
    subq $32,%rsp
    call vnt_env_set
    addq $32,%rsp
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_os__env_has
vnt_fn_os__env_has:
    pushq %rbp
    movq %rsp,%rbp
    subq $16,%rsp
    movq %rcx,-8(%rbp)
    movq -8(%rbp),%rax
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_env_has
    addq $32,%rsp
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_process__start
vnt_fn_process__start:
    pushq %rbp
    movq %rsp,%rbp
    subq $16,%rsp
    movq %rcx,-8(%rbp)
    movq %rdx,-16(%rbp)
    movq -8(%rbp),%rax
    pushq %rax
    movq -16(%rbp),%rax
    pushq %rax
    movq 8(%rsp),%rcx
    movq 0(%rsp),%rdx
    addq $16,%rsp
    subq $32,%rsp
    call vnt_process_start
    addq $32,%rsp
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_process__poll
vnt_fn_process__poll:
    pushq %rbp
    movq %rsp,%rbp
    subq $16,%rsp
    movq %rcx,-8(%rbp)
    movq -8(%rbp),%rax
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_process_poll
    addq $32,%rsp
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_process__wait
vnt_fn_process__wait:
    pushq %rbp
    movq %rsp,%rbp
    subq $16,%rsp
    movq %rcx,-8(%rbp)
    movq %rdx,-16(%rbp)
    movq -8(%rbp),%rax
    pushq %rax
    movq -16(%rbp),%rax
    pushq %rax
    movq 8(%rsp),%rcx
    movq 0(%rsp),%rdx
    addq $16,%rsp
    subq $32,%rsp
    call vnt_process_wait
    addq $32,%rsp
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_process__pid
vnt_fn_process__pid:
    pushq %rbp
    movq %rsp,%rbp
    subq $16,%rsp
    movq %rcx,-8(%rbp)
    movq -8(%rbp),%rax
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_process_pid
    addq $32,%rsp
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_process__terminate
vnt_fn_process__terminate:
    pushq %rbp
    movq %rsp,%rbp
    subq $16,%rsp
    movq %rcx,-8(%rbp)
    movq -8(%rbp),%rax
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_process_terminate
    addq $32,%rsp
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_process__stdout
vnt_fn_process__stdout:
    pushq %rbp
    movq %rsp,%rbp
    subq $16,%rsp
    movq %rcx,-8(%rbp)
    movq -8(%rbp),%rax
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_process_stdout
    addq $32,%rsp
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_process__stderr
vnt_fn_process__stderr:
    pushq %rbp
    movq %rsp,%rbp
    subq $16,%rsp
    movq %rcx,-8(%rbp)
    movq -8(%rbp),%rax
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_process_stderr
    addq $32,%rsp
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_process__exit_code
vnt_fn_process__exit_code:
    pushq %rbp
    movq %rsp,%rbp
    subq $16,%rsp
    movq %rcx,-8(%rbp)
    movq -8(%rbp),%rax
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_process_exit_code
    addq $32,%rsp
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl main
main:
    pushq %rbp
    movq %rsp,%rbp
    movl $7,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_fn_math__square
    addq $32,%rsp
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_print
    addq $32,%rsp
    movl $12,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    pushq %rax
    movl $0,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    pushq %rax
    movl $10,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    pushq %rax
    movq 16(%rsp),%rcx
    movq 8(%rsp),%rdx
    movq 0(%rsp),%r8
    addq $24,%rsp
    subq $32,%rsp
    call vnt_fn_math__clamp
    addq $32,%rsp
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_print
    addq $32,%rsp
    movl $10,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    pushq %rax
    movl $20,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    pushq %rax
    movsd .Lflt16(%rip),%xmm0
    subq $32,%rsp
    call vnt_float
    addq $32,%rsp
    pushq %rax
    movq 16(%rsp),%rcx
    movq 8(%rsp),%rdx
    movq 0(%rsp),%r8
    addq $24,%rsp
    subq $32,%rsp
    call vnt_fn_math__lerp
    addq $32,%rsp
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_print
    addq $32,%rsp
    movl $17,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    pushq %rax
    movl $5,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    pushq %rax
    movq 8(%rsp),%rcx
    movq 0(%rsp),%rdx
    addq $16,%rsp
    subq $32,%rsp
    call vnt_fn_math__mod
    addq $32,%rsp
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_print
    addq $32,%rsp
    movl $2,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_fn_time__seconds_to_ms
    addq $32,%rsp
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_fn_time__ms_to_seconds
    addq $32,%rsp
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_print
    addq $32,%rsp
    lea .Lstr17(%rip),%rcx
    subq $32,%rsp
    call vnt_string
    addq $32,%rsp
    pushq %rax
    lea .Lstr18(%rip),%rcx
    subq $32,%rsp
    call vnt_string
    addq $32,%rsp
    pushq %rax
    movq 8(%rsp),%rcx
    movq 0(%rsp),%rdx
    addq $16,%rsp
    subq $32,%rsp
    call vnt_fn_os__env_set
    addq $32,%rsp
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_print
    addq $32,%rsp
    lea .Lstr17(%rip),%rcx
    subq $32,%rsp
    call vnt_string
    addq $32,%rsp
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_fn_os__env_get
    addq $32,%rsp
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_print
    addq $32,%rsp
    lea .Lstr17(%rip),%rcx
    subq $32,%rsp
    call vnt_string
    addq $32,%rsp
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_fn_os__env_has
    addq $32,%rsp
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_print
    addq $32,%rsp
    subq $32,%rsp
    call vnt_fn_os__current_dir
    addq $32,%rsp
    pushq %rax
    lea .Lstr19(%rip),%rcx
    subq $32,%rsp
    call vnt_string
    addq $32,%rsp
    movq %rax,%rdx
    popq %rcx
    subq $32,%rsp
    call vnt_ne
    addq $32,%rsp
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_print
    addq $32,%rsp
    lea .Lstr20(%rip),%rcx
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
.Lflt16:
    .double 0.5
.Lstr17:
    .asciz "VNT_STD_TEST"
.Lstr18:
    .asciz "ready"
.Lstr19:
    .asciz ""
.Lstr20:
    .asciz "STD_FOUNDATION_OK"
