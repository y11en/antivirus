_text				SEGMENT

PUBLIC				LoadLONGLONGByGS

; ULONGLONG LoadLONGLONGByGS(IN ULONG_PTR Offset);
LoadLONGLONGByGS	PROC
					mov	rax, gs:[rcx]
					ret
LoadLONGLONGByGS	ENDP

_text				ENDS

					END