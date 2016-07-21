.code

HookThunk proc public

;	mov edi, edi
	jmp @@skip

	push rax
	push rcx
	push rdx
	push r11
	push r10
	push r9
	push r8
	mov  rdx, [rsp+38h]
	mov  rcx, [Hook]
	sub  rsp, 20h
	call [SetCurrentHook]
	add  rsp, 20h
	pop  r8
	pop  r9
	pop  r10
	pop  r11
	pop  rdx
	pop  rcx
	pop  rax
	push [HookFunc]
	ret

@@skip:
	push [OrigFunc]
	ret	
	jmp [Hook]

Hook            dq 01234567812345678h
SetCurrentHook	dq 01234567812345678h
HookFunc        dq 01234567812345678h
OrigFunc        dq 01234567812345678h

HookThunk endp

HookThunkEnd proc public
	db 0cch
HookThunkEnd endp

END