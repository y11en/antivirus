        .386p

rminfo struc
    EDI         dd ?     ;long ;0   -50
    ESI         dd ?           ;4   -46
    EBP         dd ?           ;8   -42
    ResBySystem dd ?           ;12  -38
    EBX         dd ?           ;16  -34
    EDX         dd ?           ;20  -30
    ECX         dd ?           ;24  -26
    EAX         dd ?           ;28  -22
    flags      dw ?    ;short  ;32  -18
    ES         dw ?            ;34  -16
    DS         dw ?            ;36  -14
    FS         dw ?            ;38  -12
    GS         dw ?            ;40  -10
    IP         dw ?            ;42  -8     26
    CS         dw ?            ;44  -6     24
    SP         dw ?            ;46  -4
    SS         dw ?            ;48  -2
rminfo ends                    ;50
;i_rph           db size reqPacket dup (?)

;CODE16  segment
;        assume cs:CODE16, ds:DATA16, ES:FLAT
;
;ASSUME CS:CODE16, DS:FLAT
;        mov ax,ax
;
;thunk16$CallRNIret:
;        pushf
;        pop   ax
;        or    ax,0100h
;        push  ax
;        popf
;        int   031h
;        pushf
;        pop   ax
;        and   ax,0feffh
;        push  ax
;        popf
;        jmp   thunk32$CallRNIret
;
;CODE16 ends

CODE32 segment
       assume CS:FLAT, DS:FLAT, ES:FLAT

;public Int01asm
;public Int01EndAsm
;public CallRNIretProc
public SimulateRMInt
public pmAllocMem
;public pmResizeMem
public StrPos
public SetCharAttr
public DPMIAllocMem
public DPMIFreeMem

;Int01asm proc far
;        mov   ax,cs
;        push  ds
;        mov   ds,ax

;        pop   ds
;        iret
;Int01asm endp

;Int01EndAsm proc near
;        ret
;Int01EndAsm endp

;BOOL CallRNIretProc( __148fbr_INTERRUPT ):
;CallRNIretProc proc near
;        enter 50,0                    ;50
;        push  ebx
;        push  ecx
;        push  edx
;        push  edi
;        push  es

;        mov   dword ptr [ebp-22],0    ;rmi.EAX = 0;
;        mov   dword ptr [ebp-30],080h ;rmi.EDX = 0x80; //dl=80
;        mov   eax,[ebp+12]            ;rmi.CS  = IntBody.segment;
;        mov   [ebp-6],ax
;        mov   eax,[ebp+8]             ;rmi.IP  = IntBody.offset;
;        mov   [ebp-8],ax
;
;        mov   ax,ss                   ;regs.w.es =FP_SEG(&rmi);
;        mov   es,ax
;        lea   eax,[ebp-50]            ;regs.x.edi=FP_OFF(&rmi);
;        mov   edi,eax
;        mov   eax,0302H               ;regs.w.ax =0x0302;
;        mov   bh,00H                  ;regs.h.bh =0;
;        mov   cx,0000H                ;regs.w.cx =0;

;        push  ds                      ;Push_DS();
;        jmp   thunk16$CallRNIret
 ;        pushf
 ;        pop   ax
 ;        or    ax,0100H
 ;        push  ax
 ;        popf
 ;        int   031h                    ;intr(0x31,&regs);
;thunk32$CallRNIret:
;        pop   ds                      ;Pop_DS();
;        jc    CIret_Err               ;if(((regs.w.flags&0x1)==0)&&((rmi.flags&0x1)==0))
;        test  word ptr [ebp-38],0001H
;        jne   CIret_Err
;        mov   eax,1                   ;return TRUE;
;        jmp   CIret_done
;CIret_Err:
;        mov   eax,0                   ;return FALSE;
;CIret_done:
;        mov   eax,-10H[ebp]
;        pop   es
;        pop   edi
;        pop   edx
;        pop   ecx
;        pop   ebx
;        leave
;        ret
;CallRNIretProc endp

;BOOL SimulateRMInt(UCHAR Int,RMI far* rmi)
SimulateRMInt proc near
        enter 0,0              ;esp+4
        push  ebx
        push  ecx
        push  edx
        push  edi
        push  es
        push  gs
        mov   ax,0300h   ;regs.w.ax =0x0300;        //DPMI Simulate Real Mode Int
        mov   bl,[ebp+8] ;regs.h.bl =Int;           //Number of the interrupt to simulate
        mov   bh,00h     ;regs.h.bh =0;             //Bit 0 = 1; all other bits must be 0
        mov   cx,0000h   ;regs.w.cx =0;             //No words to copy
        mov   edx,[ebp+10H]
        mov   es,dx      ;regs.w.es =FP_SEG(rmi);
        mov   edi,[ebp+0cH] ;regs.x.edi=FP_OFF(rmi);
        int   031h
        jc    SI_Err     ;if(((regs.w.flags&0x1)==0)&&((rmi->flags&0x1)==0))
        lgs   eax,pword ptr [ebp+0cH]
        test  byte ptr gs:+20H[eax],01H
        jne   SI_Err
        mov   eax,1
        jmp   short SI_Done
SI_Err: mov   eax,0
SI_Done:
        pop   gs
        pop   es
        pop   edi
        pop   edx
        pop   ecx
        pop   ebx
        leave
        ret
SimulateRMInt  endp

;BOOL pmAllocMem(USHORT,USHORT *,USHORT * ):
;Size      EQU [BP+8]
;Selector  EQU [BP+12]
;Segment   EQU [BP+16]
pmAllocMem  proc near
        enter 0,0              ;esp+4
        push  ebx
        push  ecx
        push  edx
        mov   eax,0100h
        mov   ebx,0
        mov   bx,word ptr [ebp+8] ;regs.w.bx=(WORD)((Size>>4)+1);   //количество 16 битовых параграфов
        sar   ebx,4
        inc   ebx
        cmp   ebx,01000h
        jae   AM_1
        mov   ebx,01000h
AM_1:   int   031h
        jc    AM_Err         ;if((regs.w.flags&0x1)==0)
        mov   ecx,[ebp+010h] ;*segment=regs.w.ax;
        mov   [ecx],ax
        mov   ecx,[ebp+0cH]  ;*selector=regs.w.dx;
        mov   [ecx],dx
        mov   eax,1          ;return TRUE;  ;mov     dword ptr -38H[ebp],00000000H
        jmp   short AM_Done
AM_Err:
        mov   eax,0          ;return FALSE; ;mov     eax,-38H[ebp]
AM_Done:
        pop   edx
        pop   ecx
        pop   ebx
        leave
        ret
pmAllocMem endp

;BOOL pmResizeMem(USHORT NewSize,USHORT selector):
;NewSize   EQU [BP+8]
;Selector  EQU [BP+12]
;pmResizeMem  proc near
;        enter 0,0
;        push  ebx
;        push  edx
;        mov   eax,0102h
;        mov   ebx,0
;        mov   bx,word ptr [ebp+8]    ;количество 16 битовых параграфов
;        sar   ebx,4
;        inc   ebx
;        cmp   ebx,01000h
;        jae   RM_1
;        mov   ebx,01000h
;RM_1:   mov   dx,word ptr [ebp+12]   ;Selector
;        int   031h
;        jc    RM_Err
;        mov   eax,1
;        jmp   short RM_Done
;RM_Err:
;        mov   eax,0
;RM_Done:
;        pop   edx
;        pop   ebx
;        leave
;        ret
;pmResizeMem endp

StrPos  proc near
        enter 0,0
        push  ebx
        push  edx
        mov   ah,03
        mov   bh,0
        int   010h
        mov   eax,0
        mov   al,dl
        pop   edx
        pop   ebx
        leave
        ret
StrPos  endp

SetCharAttr proc near
        enter 0,0
        push  ebx
        push  ecx
        push  edx
        mov   ah,03
        mov   bh,0
        int   010h
        cmp   dl,78
        jle   nxt_chr1
        mov   al,0ah
        int   029h
        mov   al,0dh
        int   029h
        mov   dl,0
nxt_chr1:
        mov   al,byte ptr [ebp+12]
        mov   ah,09h
        mov   bl,byte ptr [ebp+8]
        mov   bh,00
        mov   cx,1
        int   010h
;        mov   ah,03
;        mov   bh,0
;        int   010h
        inc   dl
        mov   ah,02
        int   010h

;        cmp   dl,78
;        jle   nxt_chr1
;        mov   al,0ah
;        int   029h
;        mov   al,0dh
;        int   029h
;        mov   al,020h
;        int   029h
;nxt_chr1:
        pop   edx
        pop   ecx
        pop   ebx
        leave
        ret
SetCharAttr endp

;ULONG DPMIAllocMem(ULONG size,void* pMemory)
;Size   EQU [BP+8]
DPMIAllocMem proc near
        enter 0,0
        push  ebx
        push  ecx
        push  edi
        push  si
        mov   ax,0501h
        mov   bx,word ptr [ebp+8][2]         ;Size[2]
        mov   cx,word ptr [ebp+8][0]         ;Size[0]
        int   031h
        mov   eax,edi
        mov   edi,[ebp+12]
        jc    DPMIAM_Err
        mov   word ptr [edi],cx              ;es:[di],bx
        mov   word ptr [edi][2],bx           ;es:[di+2],ax
        shl   ebx,16
        mov   bx, cx
        mov   [ebx],ax
        mov   [ebx][2],si
        mov   eax,0
        jmp   short DPMIAM_Done
DPMIAM_Err:
        mov   word ptr [edi],0               ;es:[di],0
        mov   word ptr [edi][2],0            ;es:[di+2],0
        mov   eax,1
DPMIAM_Done:
        pop   si
        pop   edi
        pop   ecx
        pop   ebx
        leave
        ret
DPMIAllocMem endp

;BOOL DPMIFree_(void* pMemory)
DPMIFreeMem proc near
        enter 0,0
        push  edi
        push  si
        mov   eax,dword ptr [ebp+8]
        mov   di,[eax]
        mov   si,[eax].2
        mov   ax,0502h
        int   031h
        jc    DPMIFM_Err
        mov   eax,0
        jmp   short DPMIFM_Done
DPMIFM_Err:
        mov   eax,1
DPMIFM_Done:
        pop   si
        pop   edi
        leave
        ret
DPMIFreeMem endp

CODE32 ends

DATA32 segment
DATA32 ends

end

