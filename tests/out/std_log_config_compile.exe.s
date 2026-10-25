.text
.bss
.globl vnt_global___vnt_symbol_0_saved
.comm vnt_global___vnt_symbol_0_saved,8,8
.globl vnt_global___vnt_symbol_1_loaded
.comm vnt_global___vnt_symbol_1_loaded,8,8
.globl vnt_global___vnt_symbol_2_exists
.comm vnt_global___vnt_symbol_2_exists,8,8
.globl vnt_global___vnt_symbol_3_logged
.comm vnt_global___vnt_symbol_3_logged,8,8
.globl vnt_global___vnt_symbol_4_warned
.comm vnt_global___vnt_symbol_4_warned,8,8
.globl vnt_global___vnt_symbol_5_failed
.comm vnt_global___vnt_symbol_5_failed,8,8
.text
.globl vnt_fn_log__info
vnt_fn_log__info:
    pushq %rbp
    movq %rsp,%rbp
    subq $16,%rsp
    movq %rcx,-16(%rbp)
    lea .Lstr0(%rip),%rcx
    subq $32,%rsp
    call vnt_string
    addq $32,%rsp
    pushq %rax
    movq -16(%rbp),%rax
    movq %rax,%rdx
    popq %rcx
    testq %rcx,%rcx
    jz .L1
    testq %rdx,%rdx
    jz .L1
    cmpl $1,0(%rcx)
    jne .L1
    cmpl $1,0(%rdx)
    jne .L1
    movl 8(%rcx),%r10d
    addl 8(%rdx),%r10d
    jo .L1
    movl %r10d,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    jmp .L2
.L1:
    subq $32,%rsp
    call vnt_add
    addq $32,%rsp
.L2:
    pushq %rax
    lea .Lstr3(%rip),%rcx
    subq $32,%rsp
    call vnt_string
    addq $32,%rsp
    movq %rax,%rdx
    popq %rcx
    testq %rcx,%rcx
    jz .L4
    testq %rdx,%rdx
    jz .L4
    cmpl $1,0(%rcx)
    jne .L4
    cmpl $1,0(%rdx)
    jne .L4
    movl 8(%rcx),%r10d
    addl 8(%rdx),%r10d
    jo .L4
    movl %r10d,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    jmp .L5
.L4:
    subq $32,%rsp
    call vnt_add
    addq $32,%rsp
.L5:
    movq %rax,-8(%rbp)
    lea .Lstr6(%rip),%rcx
    subq $32,%rsp
    call vnt_string
    addq $32,%rsp
    pushq %rax
    movq -8(%rbp),%rax
    pushq %rax
    movq 8(%rsp),%rcx
    movq 0(%rsp),%rdx
    addq $16,%rsp
    subq $32,%rsp
    call vnt_fs_append
    addq $32,%rsp
    movq -8(%rbp),%rax
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_io_write
    addq $32,%rsp
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_log__warn
vnt_fn_log__warn:
    pushq %rbp
    movq %rsp,%rbp
    subq $16,%rsp
    movq %rcx,-16(%rbp)
    lea .Lstr7(%rip),%rcx
    subq $32,%rsp
    call vnt_string
    addq $32,%rsp
    pushq %rax
    movq -16(%rbp),%rax
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
    addl 8(%rdx),%r10d
    jo .L8
    movl %r10d,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    jmp .L9
.L8:
    subq $32,%rsp
    call vnt_add
    addq $32,%rsp
.L9:
    pushq %rax
    lea .Lstr3(%rip),%rcx
    subq $32,%rsp
    call vnt_string
    addq $32,%rsp
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
    movq %rax,-8(%rbp)
    lea .Lstr6(%rip),%rcx
    subq $32,%rsp
    call vnt_string
    addq $32,%rsp
    pushq %rax
    movq -8(%rbp),%rax
    pushq %rax
    movq 8(%rsp),%rcx
    movq 0(%rsp),%rdx
    addq $16,%rsp
    subq $32,%rsp
    call vnt_fs_append
    addq $32,%rsp
    movq -8(%rbp),%rax
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_io_write_error
    addq $32,%rsp
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_log__error
vnt_fn_log__error:
    pushq %rbp
    movq %rsp,%rbp
    subq $16,%rsp
    movq %rcx,-16(%rbp)
    lea .Lstr12(%rip),%rcx
    subq $32,%rsp
    call vnt_string
    addq $32,%rsp
    pushq %rax
    movq -16(%rbp),%rax
    movq %rax,%rdx
    popq %rcx
    testq %rcx,%rcx
    jz .L13
    testq %rdx,%rdx
    jz .L13
    cmpl $1,0(%rcx)
    jne .L13
    cmpl $1,0(%rdx)
    jne .L13
    movl 8(%rcx),%r10d
    addl 8(%rdx),%r10d
    jo .L13
    movl %r10d,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    jmp .L14
.L13:
    subq $32,%rsp
    call vnt_add
    addq $32,%rsp
.L14:
    pushq %rax
    lea .Lstr3(%rip),%rcx
    subq $32,%rsp
    call vnt_string
    addq $32,%rsp
    movq %rax,%rdx
    popq %rcx
    testq %rcx,%rcx
    jz .L15
    testq %rdx,%rdx
    jz .L15
    cmpl $1,0(%rcx)
    jne .L15
    cmpl $1,0(%rdx)
    jne .L15
    movl 8(%rcx),%r10d
    addl 8(%rdx),%r10d
    jo .L15
    movl %r10d,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    jmp .L16
.L15:
    subq $32,%rsp
    call vnt_add
    addq $32,%rsp
.L16:
    movq %rax,-8(%rbp)
    lea .Lstr6(%rip),%rcx
    subq $32,%rsp
    call vnt_string
    addq $32,%rsp
    pushq %rax
    movq -8(%rbp),%rax
    pushq %rax
    movq 8(%rsp),%rcx
    movq 0(%rsp),%rdx
    addq $16,%rsp
    subq $32,%rsp
    call vnt_fs_append
    addq $32,%rsp
    movq -8(%rbp),%rax
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_io_write_error
    addq $32,%rsp
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_config__load_or_create
vnt_fn_config__load_or_create:
    pushq %rbp
    movq %rsp,%rbp
    subq $16,%rsp
    movq %rcx,-8(%rbp)
    movq %rdx,-16(%rbp)
    movq -8(%rbp),%rax
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_fs_exists
    addq $32,%rsp
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_truth
    addq $32,%rsp
    testl %eax,%eax
    jz .L17
    movq -8(%rbp),%rax
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_fs_read
    addq $32,%rsp
    leave
    ret
    jmp .L18
.L17:
    movq -8(%rbp),%rax
    pushq %rax
    movq -16(%rbp),%rax
    pushq %rax
    movq 8(%rsp),%rcx
    movq 0(%rsp),%rdx
    addq $16,%rsp
    subq $32,%rsp
    call vnt_fs_write
    addq $32,%rsp
    movq -16(%rbp),%rax
    leave
    ret
.L18:
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_config__save
vnt_fn_config__save:
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
    call vnt_fs_write
    addq $32,%rsp
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_config__append
vnt_fn_config__append:
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
    call vnt_fs_append
    addq $32,%rsp
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_config__exists
vnt_fn_config__exists:
    pushq %rbp
    movq %rsp,%rbp
    subq $16,%rsp
    movq %rcx,-8(%rbp)
    movq -8(%rbp),%rax
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_fs_exists
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
    subq $48,%rsp
    lea .Lstr19(%rip),%rcx
    subq $32,%rsp
    call vnt_string
    addq $32,%rsp
    pushq %rax
    lea .Lstr20(%rip),%rcx
    subq $32,%rsp
    call vnt_string
    addq $32,%rsp
    pushq %rax
    movq 8(%rsp),%rcx
    movq 0(%rsp),%rdx
    addq $16,%rsp
    subq $32,%rsp
    call vnt_fn_config__save
    addq $32,%rsp
    movq %rax,vnt_global___vnt_symbol_0_saved(%rip)
    lea .Lstr19(%rip),%rcx
    subq $32,%rsp
    call vnt_string
    addq $32,%rsp
    pushq %rax
    lea .Lstr21(%rip),%rcx
    subq $32,%rsp
    call vnt_string
    addq $32,%rsp
    pushq %rax
    movq 8(%rsp),%rcx
    movq 0(%rsp),%rdx
    addq $16,%rsp
    subq $32,%rsp
    call vnt_fn_config__load_or_create
    addq $32,%rsp
    movq %rax,vnt_global___vnt_symbol_1_loaded(%rip)
    lea .Lstr19(%rip),%rcx
    subq $32,%rsp
    call vnt_string
    addq $32,%rsp
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_fn_config__exists
    addq $32,%rsp
    movq %rax,vnt_global___vnt_symbol_2_exists(%rip)
    lea .Lstr22(%rip),%rcx
    subq $32,%rsp
    call vnt_string
    addq $32,%rsp
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_fn_log__info
    addq $32,%rsp
    movq %rax,vnt_global___vnt_symbol_3_logged(%rip)
    lea .Lstr22(%rip),%rcx
    subq $32,%rsp
    call vnt_string
    addq $32,%rsp
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_fn_log__warn
    addq $32,%rsp
    movq %rax,vnt_global___vnt_symbol_4_warned(%rip)
    lea .Lstr22(%rip),%rcx
    subq $32,%rsp
    call vnt_string
    addq $32,%rsp
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_fn_log__error
    addq $32,%rsp
    movq %rax,vnt_global___vnt_symbol_5_failed(%rip)
    lea .Lstr23(%rip),%rcx
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
.Lstr0:
    .asciz "[INFO] "
.Lstr3:
    .asciz "\n"
.Lstr6:
    .asciz "vnt.log"
.Lstr7:
    .asciz "[WARN] "
.Lstr12:
    .asciz "[ERROR] "
.Lstr19:
    .asciz "settings.ini"
.Lstr20:
    .asciz "theme=dark\n"
.Lstr21:
    .asciz "theme=light\n"
.Lstr22:
    .asciz "compile-check"
.Lstr23:
    .asciz "STD_LOG_CONFIG_COMPILE_OK"
