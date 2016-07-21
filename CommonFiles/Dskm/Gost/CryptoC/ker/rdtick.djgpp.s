	.file	"tmp.c"
	.section .text
	.p2align 1
.globl _rdtick
_rdtick:
	pushl	%edx
	rdtsc
	popl	%edx
	ret
	.ident	"GCC: (GNU) 3.1"
