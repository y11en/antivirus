	.version	"01.01"
.text
	.align 16
.globl rdtick
	.type		rdtick,@function
rdtick:
	push		%rdx
	rdtsc
	pop		%rdx
	ret
.Lfe1:
	.size	 rdtick,.Lfe1-rdick
