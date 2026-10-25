.text
.bss
.globl vnt_global___vnt_symbol_0_directory
.comm vnt_global___vnt_symbol_0_directory,8,8
.globl vnt_global___vnt_symbol_1_first_path
.comm vnt_global___vnt_symbol_1_first_path,8,8
.globl vnt_global___vnt_symbol_2_renamed_path
.comm vnt_global___vnt_symbol_2_renamed_path,8,8
.globl vnt_global___vnt_symbol_3_created_dir
.comm vnt_global___vnt_symbol_3_created_dir,8,8
.globl vnt_global___vnt_symbol_4_removed_first
.comm vnt_global___vnt_symbol_4_removed_first,8,8
.globl vnt_global___vnt_symbol_5_removed_renamed
.comm vnt_global___vnt_symbol_5_removed_renamed,8,8
.text
.globl vnt_fn_fs__exists
vnt_fn_fs__exists:
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

.globl vnt_fn_fs__is_file
vnt_fn_fs__is_file:
    pushq %rbp
    movq %rsp,%rbp
    subq $16,%rsp
    movq %rcx,-8(%rbp)
    movq -8(%rbp),%rax
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_fs_is_file
    addq $32,%rsp
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_fs__is_dir
vnt_fn_fs__is_dir:
    pushq %rbp
    movq %rsp,%rbp
    subq $16,%rsp
    movq %rcx,-8(%rbp)
    movq -8(%rbp),%rax
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_fs_is_dir
    addq $32,%rsp
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_fs__read_text
vnt_fn_fs__read_text:
    pushq %rbp
    movq %rsp,%rbp
    subq $16,%rsp
    movq %rcx,-8(%rbp)
    movq -8(%rbp),%rax
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_fs_read
    addq $32,%rsp
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_fs__write_text
vnt_fn_fs__write_text:
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

.globl vnt_fn_fs__append_text
vnt_fn_fs__append_text:
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

.globl vnt_fn_fs__remove_file
vnt_fn_fs__remove_file:
    pushq %rbp
    movq %rsp,%rbp
    subq $16,%rsp
    movq %rcx,-8(%rbp)
    movq -8(%rbp),%rax
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_fs_delete
    addq $32,%rsp
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_fs__rename_path
vnt_fn_fs__rename_path:
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
    call vnt_fs_rename
    addq $32,%rsp
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_fs__create_dir
vnt_fn_fs__create_dir:
    pushq %rbp
    movq %rsp,%rbp
    subq $16,%rsp
    movq %rcx,-8(%rbp)
    movq -8(%rbp),%rax
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_dir_create
    addq $32,%rsp
    leave
    ret
    xorl %eax,%eax
    leave
    ret

.globl vnt_fn_fs__list_dir
vnt_fn_fs__list_dir:
    pushq %rbp
    movq %rsp,%rbp
    subq $16,%rsp
    movq %rcx,-8(%rbp)
    movq -8(%rbp),%rax
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_fs_list
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
    subq $80,%rsp
    lea .Lstr0(%rip),%rcx
    subq $32,%rsp
    call vnt_string
    addq $32,%rsp
    movq %rax,vnt_global___vnt_symbol_0_directory(%rip)
    movq vnt_global___vnt_symbol_0_directory(%rip),%rax
    pushq %rax
    lea .Lstr1(%rip),%rcx
    subq $32,%rsp
    call vnt_string
    addq $32,%rsp
    movq %rax,%rdx
    popq %rcx
    testq %rcx,%rcx
    jz .L2
    testq %rdx,%rdx
    jz .L2
    cmpl $1,0(%rcx)
    jne .L2
    cmpl $1,0(%rdx)
    jne .L2
    movl 8(%rcx),%r10d
    addl 8(%rdx),%r10d
    jo .L2
    movl %r10d,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    jmp .L3
.L2:
    subq $32,%rsp
    call vnt_add
    addq $32,%rsp
.L3:
    movq %rax,vnt_global___vnt_symbol_1_first_path(%rip)
    movq vnt_global___vnt_symbol_0_directory(%rip),%rax
    pushq %rax
    lea .Lstr4(%rip),%rcx
    subq $32,%rsp
    call vnt_string
    addq $32,%rsp
    movq %rax,%rdx
    popq %rcx
    testq %rcx,%rcx
    jz .L5
    testq %rdx,%rdx
    jz .L5
    cmpl $1,0(%rcx)
    jne .L5
    cmpl $1,0(%rdx)
    jne .L5
    movl 8(%rcx),%r10d
    addl 8(%rdx),%r10d
    jo .L5
    movl %r10d,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    jmp .L6
.L5:
    subq $32,%rsp
    call vnt_add
    addq $32,%rsp
.L6:
    movq %rax,vnt_global___vnt_symbol_2_renamed_path(%rip)
    movq vnt_global___vnt_symbol_0_directory(%rip),%rax
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_fn_fs__create_dir
    addq $32,%rsp
    movq %rax,vnt_global___vnt_symbol_3_created_dir(%rip)
    movq vnt_global___vnt_symbol_1_first_path(%rip),%rax
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_fn_fs__remove_file
    addq $32,%rsp
    movq %rax,vnt_global___vnt_symbol_4_removed_first(%rip)
    movq vnt_global___vnt_symbol_2_renamed_path(%rip),%rax
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_fn_fs__remove_file
    addq $32,%rsp
    movq %rax,vnt_global___vnt_symbol_5_removed_renamed(%rip)
    movq vnt_global___vnt_symbol_0_directory(%rip),%rax
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_fn_fs__is_dir
    addq $32,%rsp
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_not
    addq $32,%rsp
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_truth
    addq $32,%rsp
    testl %eax,%eax
    jz .L7
    lea .Lstr9(%rip),%rcx
    subq $32,%rsp
    call vnt_string
    addq $32,%rsp
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_print
    addq $32,%rsp
    jmp .L8
.L7:
    movq vnt_global___vnt_symbol_1_first_path(%rip),%rax
    pushq %rax
    lea .Lstr12(%rip),%rcx
    subq $32,%rsp
    call vnt_string
    addq $32,%rsp
    pushq %rax
    movq 8(%rsp),%rcx
    movq 0(%rsp),%rdx
    addq $16,%rsp
    subq $32,%rsp
    call vnt_fn_fs__write_text
    addq $32,%rsp
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_not
    addq $32,%rsp
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_truth
    addq $32,%rsp
    testl %eax,%eax
    jz .L10
    lea .Lstr13(%rip),%rcx
    subq $32,%rsp
    call vnt_string
    addq $32,%rsp
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_print
    addq $32,%rsp
    jmp .L11
.L10:
    movq vnt_global___vnt_symbol_1_first_path(%rip),%rax
    pushq %rax
    lea .Lstr14(%rip),%rcx
    subq $32,%rsp
    call vnt_string
    addq $32,%rsp
    pushq %rax
    movq 8(%rsp),%rcx
    movq 0(%rsp),%rdx
    addq $16,%rsp
    subq $32,%rsp
    call vnt_fn_fs__append_text
    addq $32,%rsp
    movq %rax,-56(%rbp)
    movq vnt_global___vnt_symbol_1_first_path(%rip),%rax
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_fn_fs__read_text
    addq $32,%rsp
    pushq %rax
    lea .Lstr17(%rip),%rcx
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
    call vnt_truth
    addq $32,%rsp
    testl %eax,%eax
    jz .L15
    lea .Lstr18(%rip),%rcx
    subq $32,%rsp
    call vnt_string
    addq $32,%rsp
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_print
    addq $32,%rsp
    jmp .L16
.L15:
    movq vnt_global___vnt_symbol_0_directory(%rip),%rax
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_fn_fs__list_dir
    addq $32,%rsp
    movq %rax,-64(%rbp)
    movl $0,%ecx
    subq $32,%rsp
    call vnt_bool
    addq $32,%rsp
    movq %rax,-72(%rbp)
    movl $0,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    movq %rax,-80(%rbp)
.L19:
    movq -80(%rbp),%rax
    pushq %rax
    movq -64(%rbp),%rax
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_len
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
    jz .L20
    movq -64(%rbp),%rax
    pushq %rax
    movq -80(%rbp),%rax
    movq %rax,%rdx
    popq %rcx
    subq $32,%rsp
    call vnt_array_get
    addq $32,%rsp
    pushq %rax
    lea .Lstr23(%rip),%rcx
    subq $32,%rsp
    call vnt_string
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
    jz .L21
    movl $1,%ecx
    subq $32,%rsp
    call vnt_bool
    addq $32,%rsp
    movq %rax,-72(%rbp)
    jmp .L22
.L21:
.L22:
    movq -80(%rbp),%rax
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
    movq %rax,-80(%rbp)
    jmp .L19
.L20:
    movq -72(%rbp),%rax
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_not
    addq $32,%rsp
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_truth
    addq $32,%rsp
    testl %eax,%eax
    jz .L26
    lea .Lstr28(%rip),%rcx
    subq $32,%rsp
    call vnt_string
    addq $32,%rsp
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_print
    addq $32,%rsp
    jmp .L27
.L26:
    movq vnt_global___vnt_symbol_1_first_path(%rip),%rax
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_fn_fs__is_file
    addq $32,%rsp
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_not
    addq $32,%rsp
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_truth
    addq $32,%rsp
    testl %eax,%eax
    jz .L29
    lea .Lstr31(%rip),%rcx
    subq $32,%rsp
    call vnt_string
    addq $32,%rsp
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_print
    addq $32,%rsp
    jmp .L30
.L29:
    movq vnt_global___vnt_symbol_1_first_path(%rip),%rax
    pushq %rax
    movq vnt_global___vnt_symbol_2_renamed_path(%rip),%rax
    pushq %rax
    movq 8(%rsp),%rcx
    movq 0(%rsp),%rdx
    addq $16,%rsp
    subq $32,%rsp
    call vnt_fn_fs__rename_path
    addq $32,%rsp
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_not
    addq $32,%rsp
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_truth
    addq $32,%rsp
    testl %eax,%eax
    jz .L32
    lea .Lstr34(%rip),%rcx
    subq $32,%rsp
    call vnt_string
    addq $32,%rsp
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_print
    addq $32,%rsp
    jmp .L33
.L32:
    movq vnt_global___vnt_symbol_2_renamed_path(%rip),%rax
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_fn_fs__exists
    addq $32,%rsp
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_not
    addq $32,%rsp
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_truth
    addq $32,%rsp
    testl %eax,%eax
    jnz .L37
    movq vnt_global___vnt_symbol_2_renamed_path(%rip),%rax
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_fn_fs__is_file
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
    jmp .L38
.L37:
    movl $1,%ecx
    subq $32,%rsp
    call vnt_bool
    addq $32,%rsp
.L38:
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_truth
    addq $32,%rsp
    testl %eax,%eax
    jz .L35
    lea .Lstr39(%rip),%rcx
    subq $32,%rsp
    call vnt_string
    addq $32,%rsp
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_print
    addq $32,%rsp
    jmp .L36
.L35:
    movq vnt_global___vnt_symbol_2_renamed_path(%rip),%rax
    pushq %rax
    movq 0(%rsp),%rcx
    addq $8,%rsp
    subq $32,%rsp
    call vnt_fn_fs__remove_file
    addq $32,%rsp
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_not
    addq $32,%rsp
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_truth
    addq $32,%rsp
    testl %eax,%eax
    jz .L40
    lea .Lstr42(%rip),%rcx
    subq $32,%rsp
    call vnt_string
    addq $32,%rsp
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_print
    addq $32,%rsp
    jmp .L41
.L40:
    lea .Lstr43(%rip),%rcx
    subq $32,%rsp
    call vnt_string
    addq $32,%rsp
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_print
    addq $32,%rsp
.L41:
.L36:
.L33:
.L30:
.L27:
.L16:
.L11:
.L8:
    lea .Lstr44(%rip),%rcx
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
    .asciz "tests/out/std_fs_tmp"
.Lstr1:
    .asciz "/sample.txt"
.Lstr4:
    .asciz "/renamed.txt"
.Lstr9:
    .asciz "FS_FAIL_DIR"
.Lstr12:
    .asciz "hello"
.Lstr13:
    .asciz "FS_FAIL_WRITE"
.Lstr14:
    .asciz " world"
.Lstr17:
    .asciz "hello world"
.Lstr18:
    .asciz "FS_FAIL_READ_APPEND"
.Lstr23:
    .asciz "sample.txt"
.Lstr28:
    .asciz "FS_FAIL_LIST"
.Lstr31:
    .asciz "FS_FAIL_IS_FILE"
.Lstr34:
    .asciz "FS_FAIL_RENAME"
.Lstr39:
    .asciz "FS_FAIL_RENAMED_FILE"
.Lstr42:
    .asciz "FS_FAIL_DELETE"
.Lstr43:
    .asciz "FS_OK"
.Lstr44:
    .asciz "STANDARD_FS_OK"
