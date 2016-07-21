        title   _TryEnter - enter exception protected code

page

INCLUDE ex_data.inc

		public  _TryEnter

EXTRN   __imp__GetLastError@0:NEAR
EXTRN   __imp__TlsGetValue@4:NEAR
EXTRN   __imp__TlsSetValue@8:NEAR

EXTRN   _longjmp:NEAR
EXTRN   __setjmp3:NEAR

EXTRN   _MemoryAllocFuncPtr:BYTE

EXTRN   _enc:DWORD
EXTRN   _tmpe:DWORD

COMM    _dwTIndex:DWORD
COMM    _seh_ptr:DWORD

        .586
        .model flat
        .code _TEXT

_TEXT	SEGMENT


new_data              equ ebx
old_data              equ edx
except_reg_ptr_before equ dword ptr [esp+8]
except_reg_ptr        equ dword ptr [esp+0ch]

_TryEnter proc
    
    ASSUME fs:nothing

    mov         eax, except_reg_ptr_before                      ; pointer to the exception registration param
    test        eax, eax                                        ; check if er is 0
    jne         short process                                   ; if 0 go to error
    mov         eax, 80000046h                                  ; errPARAMETER_INVALID
    ret                                                       
                                                              
process:     
    mov         _tmpe, 0                                                 
    mov         dword   ptr [eax+8], 0                          ; er->reserved = 0;
                                                              
    push        dword ptr [_dwTIndex]                           ; get head of exception data list
    call        dword ptr [__imp__TlsGetValue@4]              
    mov         old_data, eax                                 
    test        eax,  eax                                     
    je          short memory_alloc_lbl                          ; we have no old data
                                                              
    push        old_data                                      
    call        dword ptr [__imp__GetLastError@0]               ; if ( ERROR_SUCCESS == GetLastError() )
    pop         old_data                                        ; edx is a old_data
    test        eax, eax                                      
    je          short exception_frame_check_lbl_0             
    mov         old_data, 0                                     ; TlsGetValue returns bad value
    je          short memory_alloc_lbl                          ; we have no old data

exception_frame_check_lbl_0:
    mov         eax, old_data
    mov         ecx, except_reg_ptr_before

exception_frame_check_lbl:
    cmp         (ExceptionData PTR [eax]).reg_ptr, ecx
    je          short found_frame_lbl
    inc         _tmpe
    mov         eax, (ExceptionData PTR [eax]).next
    test        eax, eax
    je          short memory_alloc_lbl
    jmp         short exception_frame_check_lbl

found_frame_lbl:
    mov         eax, 80000049h                                  ; errEXCEPTION_FRAME_ALREADY_EXIST
    ret                                                       
                                                              
memory_alloc_lbl:    
    mov         eax, _tmpe
    cmp         _enc, eax
    jnl          short memory_alloc_lbl_2
    mov         _enc, eax

memory_alloc_lbl_2:                                         
    push        old_data                                      
    push        SIZEOF    ExceptionData                         ; sizeof(struct ExceptionData)
    call        dword     ptr [_MemoryAllocFuncPtr]             ; alloc memory
    add         esp,      4                                     
    pop         old_data                                      
    test        eax,      eax                                   ; if memory allocated
    jne         short     process_2                             
    mov         eax,      80000041h                             ; errNOT_ENOUGHT_MEMORY
    ret                                                       
                                                              
process_2:                                                    
    push        new_data
    mov         new_data, eax
    test        old_data, old_data                              ; if ( head )
    je          short     new_list                              ; go to new_list
                                                              
    mov         eax, except_reg_ptr                             ; pointer to the exception registration param
    mov         dword ptr [eax], 0                              ; er->prev = 0;
    mov         dword ptr [eax+4], 0                            ; er->now = 0; 

    mov         (ExceptionData ptr [new_data]).next, old_data   ; work->next = head
    jmp         short finish                                    
                                                                
new_list:                                                       ; register new except handler
    mov         eax, except_reg_ptr                             ; pointer to the exception registration param
                                                                
    mov         ecx, dword ptr fs:[0]                           ; previous reistration data
    mov         dword ptr [eax], ecx                            ; save previous registration
    mov         ecx, _seh_ptr                                   
    mov         dword ptr [eax+4], ecx                          ; reg new handler
    mov         dword ptr fs:[0], eax                           
                                                                
finish:                                                         ; do the rest of the job
                                                                ;     TlsSetValue( _dwTIndex, head )
                                                                ;     setjmp( head->jump )
                                                                ;
    push        new_data                                        ;
    push        new_data                                        ;
    push        dword ptr [_dwTIndex]                           ;
    call        dword ptr [__imp__TlsSetValue@8]                ; set new value of head of exception data list to TLS 

    pop         new_data
    mov         ecx, except_reg_ptr                             ; pointer to the exception registration param
    mov         (ExceptionData PTR [new_data]).reg_ptr, ecx     ; store registration pointer

    mov         eax, dword ptr [esp+4]
    mov         (ExceptionData PTR [new_data]).ret_addr, eax    ; store ret addr to ret_addr field

    lea         eax, (ExceptionData PTR [new_data]).jump
    pop         new_data

    push        0                                               ; call setjmp
    push        eax                                             ; offset of setjmp buffer
    mov         eax, __setjmp3
    call        eax
    add         esp, 8
    test        eax, eax
    je          done_lbl

    ; flow control gets here in case of exception fired
    push        dword ptr [_dwTIndex]                           ; get head of exception data list
    call        dword ptr [__imp__TlsGetValue@4]
    mov         eax,  (ExceptionData PTR [eax]).ret_addr
    mov         dword ptr [esp], eax
    mov         eax, 80000040h                                  ; errUNEXPECTED
    
done_lbl:
    ret

_TryEnter endp

_TEXT	ENDS

        end
