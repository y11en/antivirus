	.version	"01.01"
.text
	.align 4
.globl rdtick
	.type		rdtick,@function
rdtick:
	pushl		%edx
	rdtsc
	popl		%edx
	ret
.Lfe1:
	.size	 rdtick,.Lfe1-rdick
