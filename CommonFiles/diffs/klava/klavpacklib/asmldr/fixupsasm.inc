align 4
resolve_fixups_asm proc near
; parameter 1: 32 + esp
; parameter 2: 36 + esp
; parameter 3: 40 + esp
; parameter 4: 44 + esp
$B1$1:                          ; Preds $B1$0
        push      edi                                           ;4.1
        push      esi                                           ;4.1
        push      ebp                                           ;4.1
        push      ebx                                           ;4.1
        sub       esp, 12                                       ;4.1
        mov       edx, DWORD PTR [esp+40]                       ;2.5
        test      edx, edx                                      ;12.3
        je        $B1$4         ; Prob 50%                      ;12.3
                                ; LOE edx
$B1$2:                          ; Preds $B1$1
        mov       eax, DWORD PTR [esp+44]                       ;12.3
        test      eax, eax                                      ;12.3
        je        $B1$4         ; Prob 28%                      ;12.3
                                ; LOE edx
$B1$3:                          ; Preds $B1$2
        mov       eax, DWORD PTR [esp+32]                       ;14.48
        sub       eax, DWORD PTR [esp+36]                       ;14.48
        mov       DWORD PTR [esp+8], eax                        ;14.48
        jne       $B1$5         ; Prob 96%                      ;14.3
                                ; LOE edx
$B1$4:                          ; Preds $B1$6 $B1$2 $B1$1 $B1$3
        xor       eax, eax                                      ;15.11
L1:                                                             ;
        add       esp, 12                                       ;15.11
        pop       ebx                                           ;15.11
        pop       ebp                                           ;15.11
        pop       esi                                           ;15.11
        pop       edi                                           ;15.11
        ret                                                     ;15.11
                                ; LOE
$B1$5:                          ; Preds $B1$3
        mov       eax, DWORD PTR [esp+32]                       ;17.15
        lea       edx, DWORD PTR [edx+eax]                      ;17.15
        mov       DWORD PTR [esp], edx                          ;17.15
        xor       ebp, ebp                                      ;18.9
                                ; LOE ebp
$B1$6:                          ; Preds $B1$15 $B1$5
        mov       eax, DWORD PTR [esp+44]                       ;18.3
        cmp       ebp, eax                                      ;18.3
        jae       $B1$4         ; Prob 1%                       ;18.3
                                ; LOE ebp
$B1$7:                          ; Preds $B1$6
        mov       eax, DWORD PTR [esp]                          ;26.33
        mov       ecx, DWORD PTR [ebp+eax]                      ;26.33
        add       ecx, DWORD PTR [esp+32]                       ;29.32
        mov       ebx, DWORD PTR [ebp+eax+4]                    ;27.33
        lea       edx, DWORD PTR [eax+ebp+8]                    ;28.33
        lea       eax, DWORD PTR [ebx-8]                        ;30.26
        shr       eax, 1                                        ;30.33
        test      eax, eax                                      ;32.5
        je        $B1$15        ; Prob 82%                      ;32.5
                                ; LOE eax edx ecx ebx ebp
$B1$9:                          ; Preds $B1$7 $B1$12
        movzx     esi, WORD PTR [edx]                           ;34.17
        mov       DWORD PTR [esp+4], esi                        ;34.17
        shr       esi, 12                                       ;35.25
        add       edx, 2                                        ;34.17
        test      esi, esi                                      ;35.7
        je        $B1$12        ; Prob 33%                      ;35.7
                                ; LOE eax edx ecx ebx ebp esi
$B1$10:                         ; Preds $B1$9
        cmp       esi, 3                                        ;35.7
        jne       $B1$14        ; Prob 50%                      ;35.7
                                ; LOE eax edx ecx ebx ebp
$B1$11:                         ; Preds $B1$10
        mov       edi, DWORD PTR [esp+4]                        ;40.46
        mov       esi, DWORD PTR [esp+8]                        ;40.27
        and       edi, 4095                                     ;40.46
        add       DWORD PTR [edi+ecx], esi                      ;40.27
                                ; LOE eax edx ecx ebx ebp
$B1$12:                         ; Preds $B1$9 $B1$11
        dec       eax                                           ;32.12
        jne       $B1$9         ; Prob 82%                      ;32.5
        jmp       $B1$15        ; Prob 100%                     ;32.5
                                ; LOE eax edx ecx ebx ebp
$B1$14:                         ; Preds $B1$10
        push      1                                             ;43.15
        pop       eax                                           ;43.15
        jmp       L1            ; Prob 100%                     ;43.15
                                ; LOE
$B1$15:                         ; Preds $B1$12 $B1$7
        add       ebp, ebx                                      ;18.47
        jmp       $B1$6         ; Prob 100%                     ;18.47
        ALIGN     2
resolve_fixups_asm endp

;; ----------------------------------------------------------------------------

comment $ ;; original source
int ResolveFixUpsAsm(unsigned char* hInstance, unsigned char* ImageBase,
                     unsigned long  FixUpsRVA, unsigned long  FixUpsLen)
{
  signed long    Delta;
  unsigned char* FixTable;
  unsigned long  FixTblOfs;
  unsigned long  FixBase;
  unsigned long  FixSize;

  /* check params */
  if ( !FixUpsRVA || !FixUpsLen )
    return(0); /* done */
  if ( 0 == (Delta = (signed long)(hInstance - ImageBase)) )
    return(0); /* done */

  FixTable = (hInstance + FixUpsRVA);
  for ( FixTblOfs = 0; FixTblOfs < FixUpsLen; FixTblOfs += FixSize )
  {
    unsigned short* Thunks;
    unsigned char*  CFPage;
    unsigned int    FixCnt;
    unsigned int    FixVal;
    unsigned int    I;

    FixBase = *(unsigned long*)(FixTable + FixTblOfs);
    FixSize = *(unsigned long*)(FixTable + FixTblOfs + 4);
    Thunks  = (unsigned short*)(FixTable + FixTblOfs + 8);
    CFPage  = (unsigned char*)(hInstance + FixBase);
    FixCnt  = (FixSize - 2*4) / 2;

    while( FixCnt-- )
    {
      FixVal = *Thunks++;
      switch( FixVal >> 12 )
      {
      case 0:  /* IMAGE_REL_BASED_ABSOLUTE */
        continue;
      case 3:  /* IMAGE_REL_BASED_HIGHLOW */
        *(unsigned long*)(CFPage + (FixVal & 0xfff)) += Delta;
        continue;
      default: /* unknown fixup type */
        return(1);
      }
    }
  }

  /* done */
  return(0);
}

$

;; ----------------------------------------------------------------------------

