        title   __chkesp - check esp value for debug purposes

page

        public  __chkesp

EXTRN _dbg_report:NEAR
EXTRN	_dbg_string:DWORD

        .386
        .model flat
        .code _TEXT

_TEXT	SEGMENT


__chkesp proc
    
    jne         exception_lbl
    ret

exception_lbl:
    push        ebp
    mov         ebp,esp
    sub         esp,0
    push        eax
    push        edx
    push        ebx
    push        esi
    push        edi
    push        offset _dbg_string
    call        dword ptr [_dbg_report]
    add         esp,4h
    cmp         eax,1
    jne         ignore_lbl
    int         3
ignore_lbl:
    pop         edi
    pop         esi
    pop         ebx
    pop         edx
    pop         eax
    mov         esp,ebp
    pop         ebp
    ret


__chkesp endp

_TEXT	ENDS

        end
