.text
.bss
.globl vnt_global___vnt_symbol_0_payload
.comm vnt_global___vnt_symbol_0_payload,8,8
.text
.globl vnt_fn_json__value_at
vnt_fn_json__value_at:
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
    subq $32,%rsp
    call vnt_json_value_at
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
    subq $16,%rsp
    lea .Lstr0(%rip),%rcx
    subq $32,%rsp
    call vnt_string
    addq $32,%rsp
    movq %rax,vnt_global___vnt_symbol_0_payload(%rip)
    movq vnt_global___vnt_symbol_0_payload(%rip),%rax
    pushq %rax
    lea .Lstr1(%rip),%rcx
    subq $32,%rsp
    call vnt_string
    addq $32,%rsp
    pushq %rax
    movl $0,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    pushq %rax
    movq 16(%rsp),%rcx
    movq 8(%rsp),%rdx
    movq 0(%rsp),%r8
    addq $24,%rsp
    subq $32,%rsp
    call vnt_fn_json__value_at
    addq $32,%rsp
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_print
    addq $32,%rsp
    movq vnt_global___vnt_symbol_0_payload(%rip),%rax
    pushq %rax
    lea .Lstr2(%rip),%rcx
    subq $32,%rsp
    call vnt_string
    addq $32,%rsp
    pushq %rax
    movl $1,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    pushq %rax
    movq 16(%rsp),%rcx
    movq 8(%rsp),%rdx
    movq 0(%rsp),%r8
    addq $24,%rsp
    subq $32,%rsp
    call vnt_fn_json__value_at
    addq $32,%rsp
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_print
    addq $32,%rsp
    movq vnt_global___vnt_symbol_0_payload(%rip),%rax
    pushq %rax
    lea .Lstr3(%rip),%rcx
    subq $32,%rsp
    call vnt_string
    addq $32,%rsp
    pushq %rax
    movl $0,%ecx
    subq $32,%rsp
    call vnt_int
    addq $32,%rsp
    pushq %rax
    movq 16(%rsp),%rcx
    movq 8(%rsp),%rdx
    movq 0(%rsp),%r8
    addq $24,%rsp
    subq $32,%rsp
    call vnt_fn_json__value_at
    addq $32,%rsp
    movq %rax,%rcx
    subq $32,%rsp
    call vnt_print
    addq $32,%rsp
    lea .Lstr4(%rip),%rcx
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
    .asciz "{\"hits\":[{\"title\":\"Sodium\",\"slug\":\"sodium\",\"downloads\":100},{\"title\":\"Lithium\",\"slug\":\"lithium\",\"downloads\":200}]}"
.Lstr1:
    .asciz "title"
.Lstr2:
    .asciz "slug"
.Lstr3:
    .asciz "downloads"
.Lstr4:
    .asciz "JSON_VALUE_AT_OK"
