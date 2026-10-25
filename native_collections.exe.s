	.file	"native_collections.exe.s.c"
	.text
	.section .rdata,"dr"
.LC0:
	.ascii "Runtime error: %s\12\0"
	.text
	.p2align 4
	.def	vnt_error;	.scl	3;	.type	32;	.endef
	.seh_proc	vnt_error
vnt_error:
	pushq	%rbx
	.seh_pushreg	%rbx
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, %rbx
	movl	$2, %ecx
	call	*__imp___acrt_iob_func(%rip)
	movq	%rbx, %r8
	leaq	.LC0(%rip), %rdx
	movq	%rax, %rcx
	call	fprintf
	movl	$1, %ecx
	call	exit
	nop
	.seh_endproc
	.section .rdata,"dr"
.LC1:
	.ascii "out of memory.\0"
	.text
	.p2align 4
	.def	vnt_string;	.scl	3;	.type	32;	.endef
	.seh_proc	vnt_string
vnt_string:
	pushq	%rdi
	.seh_pushreg	%rdi
	pushq	%rsi
	.seh_pushreg	%rsi
	pushq	%rbx
	.seh_pushreg	%rbx
	subq	$64, %rsp
	.seh_stackalloc	64
	.seh_endprologue
	pxor	%xmm0, %xmm0
	movq	%rcx, %rbx
	movq	%rdx, %rcx
	movq	%rdx, %rsi
	movups	%xmm0, 36(%rsp)
	movl	$0, 52(%rsp)
	call	strlen
	leaq	1(%rax), %rdi
	movq	%rdi, %rcx
	call	malloc
	movq	%rax, %rcx
	testq	%rax, %rax
	je	.L5
	movq	%rdi, %r8
	movq	%rsi, %rdx
	call	memcpy
	movl	$1, 32(%rsp)
	movq	%rax, 40(%rsp)
	movq	48(%rsp), %rax
	movdqu	32(%rsp), %xmm0
	movq	%rax, 16(%rbx)
	movq	%rbx, %rax
	movups	%xmm0, (%rbx)
	addq	$64, %rsp
	popq	%rbx
	popq	%rsi
	popq	%rdi
	ret
.L5:
	leaq	.LC1(%rip), %rcx
	call	vnt_error
	nop
	.seh_endproc
	.section .rdata,"dr"
.LC2:
	.ascii "\0"
.LC3:
	.ascii "true\0"
.LC4:
	.ascii "false\0"
.LC5:
	.ascii "%s\0"
.LC6:
	.ascii "%d\0"
.LC7:
	.ascii ", \0"
	.text
	.p2align 4
	.def	vnt_print_inner;	.scl	3;	.type	32;	.endef
	.seh_proc	vnt_print_inner
vnt_print_inner:
	pushq	%rdi
	.seh_pushreg	%rdi
	pushq	%rsi
	.seh_pushreg	%rsi
	pushq	%rbx
	.seh_pushreg	%rbx
	subq	$64, %rsp
	.seh_stackalloc	64
	.seh_endprologue
	movl	(%rcx), %eax
	movq	%rcx, %rbx
	cmpl	$3, %eax
	je	.L7
	ja	.L8
	cmpl	$1, %eax
	je	.L9
	cmpl	$2, %eax
	jne	.L6
	movl	8(%rcx), %edx
	leaq	.LC6(%rip), %rcx
	addq	$64, %rsp
	popq	%rbx
	popq	%rsi
	popq	%rdi
	jmp	printf
	.p2align 4,,10
	.p2align 3
.L8:
	cmpl	$4, %eax
	jne	.L6
	movl	16(%rcx), %esi
	movl	$91, %ecx
	movl	%esi, %edi
	call	putchar
	testl	%esi, %esi
	jle	.L15
	movq	8(%rbx), %rbx
	xorl	%esi, %esi
.L16:
	movdqu	(%rbx), %xmm0
	leaq	32(%rsp), %rcx
	addl	$1, %esi
	movups	%xmm0, 32(%rsp)
	movq	16(%rbx), %rax
	movq	%rax, 48(%rsp)
	call	vnt_print_inner
	cmpl	%esi, %edi
	je	.L15
	testl	%esi, %esi
	jne	.L20
	movdqu	24(%rbx), %xmm0
	leaq	32(%rsp), %rcx
	movups	%xmm0, 32(%rsp)
	movq	40(%rbx), %rax
	movq	%rax, 48(%rsp)
	call	vnt_print_inner
	cmpl	$1, %edi
	je	.L15
	addq	$48, %rbx
	movl	$1, %esi
.L17:
	leaq	.LC7(%rip), %rcx
	call	printf
	jmp	.L16
	.p2align 4,,10
	.p2align 3
.L6:
	addq	$64, %rsp
	popq	%rbx
	popq	%rsi
	popq	%rdi
	ret
	.p2align 4,,10
	.p2align 3
.L15:
	movl	$93, %ecx
	addq	$64, %rsp
	popq	%rbx
	popq	%rsi
	popq	%rdi
	jmp	putchar
	.p2align 4,,10
	.p2align 3
.L9:
	movq	8(%rcx), %rdx
	leaq	.LC2(%rip), %rax
	testq	%rdx, %rdx
	cmove	%rax, %rdx
.L24:
	leaq	.LC5(%rip), %rcx
	addq	$64, %rsp
	popq	%rbx
	popq	%rsi
	popq	%rdi
	jmp	printf
	.p2align 4,,10
	.p2align 3
.L7:
	movl	8(%rcx), %edx
	leaq	.LC4(%rip), %rax
	testl	%edx, %edx
	leaq	.LC3(%rip), %rdx
	cmove	%rax, %rdx
	jmp	.L24
	.p2align 4,,10
	.p2align 3
.L20:
	addq	$24, %rbx
	jmp	.L17
	.seh_endproc
	.section .rdata,"dr"
.LC8:
	.ascii "Vanta\0"
.LC9:
	.ascii "OS\0"
.LC10:
	.ascii "arithmetic requires integers.\0"
	.section	.text.startup,"x"
	.p2align 4
	.globl	main
	.def	main;	.scl	2;	.type	32;	.endef
	.seh_proc	main
main:
	pushq	%rdi
	.seh_pushreg	%rdi
	pushq	%rsi
	.seh_pushreg	%rsi
	pushq	%rbx
	.seh_pushreg	%rbx
	subq	$368, %rsp
	.seh_stackalloc	368
	movups	%xmm6, 352(%rsp)
	.seh_savexmm	%xmm6, 352
	.seh_endprologue
	pxor	%xmm6, %xmm6
	call	__main
	leaq	.LC8(%rip), %rdx
	leaq	80(%rsp), %rcx
	call	vnt_string
	movdqu	80(%rsp), %xmm0
	movq	96(%rsp), %rax
	leaq	48(%rsp), %rcx
	movups	%xmm0, 48(%rsp)
	movq	%rax, 64(%rsp)
	call	vnt_print_inner
	movl	$10, %ecx
	call	putchar
	leaq	.LC9(%rip), %rdx
	leaq	112(%rsp), %rcx
	call	vnt_string
	movdqu	80(%rsp), %xmm0
	movq	96(%rsp), %rax
	movq	128(%rsp), %rdx
	movq	%rax, 192(%rsp)
	movl	80(%rsp), %eax
	movups	%xmm0, 176(%rsp)
	movdqu	112(%rsp), %xmm0
	movq	%rdx, 224(%rsp)
	movups	%xmm0, 208(%rsp)
	cmpl	$1, %eax
	je	.L40
	cmpl	$2, 112(%rsp)
	jne	.L27
	cmpl	$2, %eax
	jne	.L27
	movl	216(%rsp), %eax
	addl	184(%rsp), %eax
	movups	%xmm6, 276(%rsp)
	movl	$0, 292(%rsp)
	movl	%eax, 280(%rsp)
	movq	288(%rsp), %rax
	movl	$2, 272(%rsp)
	movdqu	272(%rsp), %xmm0
	movq	%rax, 160(%rsp)
	movups	%xmm0, 144(%rsp)
.L31:
	movdqu	144(%rsp), %xmm0
	movq	160(%rsp), %rax
	leaq	48(%rsp), %rcx
	movups	%xmm0, 48(%rsp)
	movq	%rax, 64(%rsp)
	call	vnt_print_inner
	movl	$10, %ecx
	call	putchar
	movups	%xmm6, 244(%rsp)
	movl	$192, %ecx
	movl	$10, 248(%rsp)
	movl	$0, 260(%rsp)
	movq	256(%rsp), %rax
	movl	$2, 240(%rsp)
	movdqu	240(%rsp), %xmm1
	movups	%xmm6, 244(%rsp)
	movq	256(%rsp), %rdx
	movl	$20, 248(%rsp)
	movdqu	240(%rsp), %xmm2
	movq	%rdx, 312(%rsp)
	movups	%xmm6, 244(%rsp)
	movq	256(%rsp), %rdx
	movl	$30, 248(%rsp)
	movdqu	240(%rsp), %xmm0
	movq	%rax, 288(%rsp)
	movq	%rdx, 336(%rsp)
	movq	%rax, 256(%rsp)
	movups	%xmm1, 272(%rsp)
	movups	%xmm2, 296(%rsp)
	movups	%xmm0, 320(%rsp)
	movups	%xmm1, 240(%rsp)
	call	malloc
	movq	%rax, %rbx
	testq	%rax, %rax
	je	.L34
	xorl	%eax, %eax
	xorl	%edx, %edx
	jmp	.L36
	.p2align 4,,10
	.p2align 3
.L41:
	addq	$24, %rax
	movq	288(%rsp,%rax), %rcx
	movdqu	272(%rsp,%rax), %xmm0
	movq	%rcx, 256(%rsp)
	movups	%xmm0, 240(%rsp)
.L36:
	movdqu	240(%rsp), %xmm0
	movq	256(%rsp), %rcx
	addl	$1, %edx
	movq	%rcx, 16(%rbx,%rax)
	movups	%xmm0, (%rbx,%rax)
	cmpl	$3, %edx
	jne	.L41
	movdqu	(%rbx), %xmm0
	leaq	48(%rsp), %rcx
	movups	%xmm0, 48(%rsp)
	movq	16(%rbx), %rax
	movq	%rax, 64(%rsp)
	call	vnt_print_inner
	movl	$10, %ecx
	call	putchar
	movups	%xmm6, 244(%rsp)
	leaq	48(%rsp), %rcx
	movl	$0, 260(%rsp)
	movq	256(%rsp), %rax
	movl	$2, 240(%rsp)
	movq	%rax, 40(%rbx)
	movl	$99, 248(%rsp)
	movdqu	240(%rsp), %xmm0
	movq	%rax, 192(%rsp)
	movups	%xmm0, 24(%rbx)
	movups	%xmm0, 176(%rsp)
	movups	%xmm0, 48(%rsp)
	movq	%rax, 64(%rsp)
	call	vnt_print_inner
	movl	$10, %ecx
	call	putchar
	movups	%xmm6, 244(%rsp)
	leaq	48(%rsp), %rcx
	movl	$0, 260(%rsp)
	movq	256(%rsp), %rax
	movl	$2, 240(%rsp)
	movl	$3, 248(%rsp)
	movdqu	240(%rsp), %xmm0
	movq	%rax, 224(%rsp)
	movq	%rax, 64(%rsp)
	movups	%xmm0, 208(%rsp)
	movups	%xmm0, 48(%rsp)
	call	vnt_print_inner
	movl	$10, %ecx
	call	putchar
	nop
	movups	352(%rsp), %xmm6
	xorl	%eax, %eax
	addq	$368, %rsp
	popq	%rbx
	popq	%rsi
	popq	%rdi
	ret
.L40:
	cmpl	$1, 112(%rsp)
	je	.L42
.L27:
	leaq	.LC10(%rip), %rcx
	call	vnt_error
.L42:
	movq	184(%rsp), %rdx
	leaq	.LC2(%rip), %rbx
	testq	%rdx, %rdx
	cmove	%rbx, %rdx
	movq	%rdx, %rcx
	movq	%rdx, 40(%rsp)
	call	strlen
	movq	216(%rsp), %r9
	movq	%rax, %rsi
	testq	%r9, %r9
	cmove	%rbx, %r9
	movq	%r9, %rcx
	movq	%r9, 32(%rsp)
	call	strlen
	leaq	1(%rsi,%rax), %rcx
	movq	%rax, %rdi
	call	malloc
	movq	40(%rsp), %rdx
	testq	%rax, %rax
	movq	%rax, %rbx
	je	.L34
	movq	%rsi, %r8
	movq	%rax, %rcx
	call	memcpy
	movq	32(%rsp), %rdx
	leaq	1(%rdi), %r8
	leaq	(%rbx,%rsi), %rcx
	call	memcpy
	leaq	272(%rsp), %rcx
	movq	%rbx, %rdx
	call	vnt_string
	movq	%rbx, %rcx
	call	free
	movq	288(%rsp), %rax
	movdqu	272(%rsp), %xmm0
	movq	%rax, 160(%rsp)
	movups	%xmm0, 144(%rsp)
	jmp	.L31
.L34:
	leaq	.LC1(%rip), %rcx
	call	vnt_error
	nop
	.seh_endproc
	.def	__main;	.scl	2;	.type	32;	.endef
	.ident	"GCC: (Rev5, Built by MSYS2 project) 16.1.0"
	.def	fprintf;	.scl	2;	.type	32;	.endef
	.def	exit;	.scl	2;	.type	32;	.endef
	.def	strlen;	.scl	2;	.type	32;	.endef
	.def	malloc;	.scl	2;	.type	32;	.endef
	.def	memcpy;	.scl	2;	.type	32;	.endef
	.def	printf;	.scl	2;	.type	32;	.endef
	.def	putchar;	.scl	2;	.type	32;	.endef
	.def	malloc;	.scl	2;	.type	32;	.endef
	.def	free;	.scl	2;	.type	32;	.endef
