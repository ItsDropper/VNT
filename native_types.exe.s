.text
.globl main
main:
    pushq %rbp
    movq %rsp,%rbp
    subq $48,%rsp
    movsd .Lflt0(%rip),%xmm0
    subq $32, %rsp
    call vnt_float
    addq $32, %rsp
    movq %rax,-8(%rbp)
    movsd .Lflt1(%rip),%xmm0
    subq $32, %rsp
    call vnt_float
    addq $32, %rsp
    movq %rax,-16(%rbp)
    movq -8(%rbp),%rax
    pushq %rax
    movq -16(%rbp),%rax
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_mul
    addq $32, %rsp
    pushq %rax
    movq -16(%rbp),%rax
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_mul
    addq $32, %rsp
    movq %rax,-24(%rbp)
    lea .Lstr2(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movq -24(%rbp),%rax
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
    movsd .Lflt4(%rip),%xmm0
    subq $32, %rsp
    call vnt_float
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_sqrt
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movsd .Lflt5(%rip),%xmm0
    subq $32, %rsp
    call vnt_float
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_sin
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movsd .Lflt5(%rip),%xmm0
    subq $32, %rsp
    call vnt_float
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_cos
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movsd .Lflt6(%rip),%xmm0
    subq $32, %rsp
    call vnt_float
    addq $32, %rsp
    movq %rax, %rcx
    subq $32, %rsp
    call vnt_neg
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_abs
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movsd .Lflt7(%rip),%xmm0
    subq $32, %rsp
    call vnt_float
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_floor
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movsd .Lflt8(%rip),%xmm0
    subq $32, %rsp
    call vnt_float
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_ceil
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movsd .Lflt9(%rip),%xmm0
    subq $32, %rsp
    call vnt_float
    addq $32, %rsp
    pushq %rax
    movsd .Lflt10(%rip),%xmm0
    subq $32, %rsp
    call vnt_float
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_min
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movsd .Lflt9(%rip),%xmm0
    subq $32, %rsp
    call vnt_float
    addq $32, %rsp
    pushq %rax
    movsd .Lflt10(%rip),%xmm0
    subq $32, %rsp
    call vnt_float
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_max
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
    lea .Lstr12(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,-32(%rbp)
    movq -32(%rbp),%rax
    pushq %rax
    lea .Lstr13(%rip),%rcx
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
    movq -32(%rbp),%rax
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_len
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movq -32(%rbp),%rax
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
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movq -32(%rbp),%rax
    pushq %rax
    movl $4,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_array_get
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movq -32(%rbp),%rax
    pushq %rax
    lea .Lstr12(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_eq
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movq -32(%rbp),%rax
    pushq %rax
    lea .Lstr14(%rip),%rcx
    subq $32, %rsp
    call vnt_string
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_ne
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
    subq $32, %rsp
    call vnt_object_new
    addq $32, %rsp
    movq %rax,-40(%rbp)
    movq -40(%rbp),%rax
    pushq %rax
    movsd .Lflt16(%rip),%xmm0
    subq $32, %rsp
    call vnt_float
    addq $32, %rsp
    movq %rax,%r8
    lea .Lstr17(%rip),%rdx
    popq %rcx
    subq $32,%rsp
    call vnt_object_set
    addq $32,%rsp
    movq -40(%rbp),%rax
    pushq %rax
    movsd .Lflt18(%rip),%xmm0
    subq $32, %rsp
    call vnt_float
    addq $32, %rsp
    movq %rax,%r8
    lea .Lstr19(%rip),%rdx
    popq %rcx
    subq $32,%rsp
    call vnt_object_set
    addq $32,%rsp
    movq -40(%rbp),%rax
    pushq %rax
    movl $100,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%r8
    lea .Lstr20(%rip),%rdx
    popq %rcx
    subq $32,%rsp
    call vnt_object_set
    addq $32,%rsp
    movq -40(%rbp),%rax
    pushq %rax
    lea .Lstr17(%rip),%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_object_get
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movq -40(%rbp),%rax
    pushq %rax
    lea .Lstr19(%rip),%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_object_get
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movq -40(%rbp),%rax
    pushq %rax
    lea .Lstr20(%rip),%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_object_get
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movq -40(%rbp),%rax
    pushq %rax
    movq -40(%rbp),%rax
    pushq %rax
    lea .Lstr17(%rip),%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_object_get
    addq $32, %rsp
    pushq %rax
    movsd .Lflt21(%rip),%xmm0
    subq $32, %rsp
    call vnt_float
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_add
    addq $32, %rsp
    movq %rax,%r8
    lea .Lstr17(%rip),%rdx
    popq %rcx
    subq $32,%rsp
    call vnt_object_set
    addq $32,%rsp
    movq -40(%rbp),%rax
    pushq %rax
    movq -40(%rbp),%rax
    pushq %rax
    lea .Lstr20(%rip),%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_object_get
    addq $32, %rsp
    pushq %rax
    movl $10,%ecx
    subq $32, %rsp
    call vnt_int
    addq $32, %rsp
    movq %rax,%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_sub
    addq $32, %rsp
    movq %rax,%r8
    lea .Lstr20(%rip),%rdx
    popq %rcx
    subq $32,%rsp
    call vnt_object_set
    addq $32,%rsp
    movq -40(%rbp),%rax
    pushq %rax
    lea .Lstr17(%rip),%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_object_get
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movq -40(%rbp),%rax
    pushq %rax
    lea .Lstr20(%rip),%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_object_get
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movq -40(%rbp),%rax
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_len
    addq $32, %rsp
    movq %rax,%rcx
    subq $32, %rsp
    call vnt_print
    addq $32, %rsp
    movq -40(%rbp),%rax
    pushq %rax
    subq $32, %rsp
    call vnt_object_new
    addq $32, %rsp
    movq %rax,%r8
    lea .Lstr22(%rip),%rdx
    popq %rcx
    subq $32,%rsp
    call vnt_object_set
    addq $32,%rsp
    movq -40(%rbp),%rax
    pushq %rax
    lea .Lstr22(%rip),%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_object_get
    addq $32, %rsp
    pushq %rax
    movsd .Lflt10(%rip),%xmm0
    subq $32, %rsp
    call vnt_float
    addq $32, %rsp
    movq %rax,%r8
    lea .Lstr23(%rip),%rdx
    popq %rcx
    subq $32,%rsp
    call vnt_object_set
    addq $32,%rsp
    movq -40(%rbp),%rax
    pushq %rax
    lea .Lstr22(%rip),%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_object_get
    addq $32, %rsp
    pushq %rax
    lea .Lstr23(%rip),%rdx
    popq %rcx
    subq $32, %rsp
    call vnt_object_get
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
    xorl %eax,%eax
    leave
    ret

.section .rdata
.Lflt0:
    .double 3.1415926535897931
.Lflt1:
    .double 2.5
.Lflt4:
    .double 9
.Lflt5:
    .double 0
.Lflt6:
    .double 12.5
.Lflt7:
    .double 3.8999999999999999
.Lflt8:
    .double 3.1000000000000001
.Lflt9:
    .double 4.5
.Lflt10:
    .double 2
.Lflt16:
    .double 120.5
.Lflt18:
    .double 64
.Lflt21:
    .double 7.5
.Lstr2:
    .asciz "Area:"
.Lstr3:
    .asciz "Math:"
.Lstr11:
    .asciz "Strings:"
.Lstr12:
    .asciz "Vanta"
.Lstr13:
    .asciz "OS"
.Lstr14:
    .asciz "VNT"
.Lstr15:
    .asciz "Objects:"
.Lstr17:
    .asciz "x"
.Lstr19:
    .asciz "y"
.Lstr20:
    .asciz "health"
.Lstr22:
    .asciz "transform"
.Lstr23:
    .asciz "scale"
.Lstr24:
    .asciz "=== Types done ==="
