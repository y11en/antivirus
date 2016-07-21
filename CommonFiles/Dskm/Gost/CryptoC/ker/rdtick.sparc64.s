	.file	"rd.c"
	.section	".text"
	.align 4
	.global rdtick
	.type	rdtick,#function
	.proc	016
rdtick:
	!#PROLOGUE# 0
	save	%sp, -104, %sp
	!#PROLOGUE# 1
	rd	%tick, %i0
	return	%i7+8
	nop
.LLfe1:
	.size	rdtick,.LLfe1-rdtick
	.ident	"GCC: (GNU) 3.0.4"
