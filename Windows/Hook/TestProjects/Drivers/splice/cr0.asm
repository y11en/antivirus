_text				SEGMENT

PUBLIC	Cr0off
PUBLIC	Cr0on						


Cr0off	PROC
		push   rax
		mov    rax, cr0        ; грузим управляющий регистр cr0 в регистр rax
		and    rax, 0FFFEFFFFh ; сбрасываем бит WP, запрещающий запись
		mov    cr0, rax        ; обновляем управляющий регистр cr0
		pop    rax
		ret
Cr0off	ENDP

Cr0on	PROC
		mov    rax, cr0      ; грузим управляющий регистр cr0 в регистр rax
		or     rax, 10000h   ; сбрасываем бит WP, запрещающий запись
		mov    cr0, rax
		ret
Cr0on	ENDP

_text				ENDS

					END