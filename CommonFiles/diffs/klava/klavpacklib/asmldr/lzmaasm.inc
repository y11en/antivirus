;; ----------------------------------------------------------------------------
;;  LzmaDecode.c
;;  LZMA Decoder (optimized for Speed version)
;;  
;;  LZMA SDK 4.40 Copyright (c) 1999-2006 Igor Pavlov (2006-05-01)
;;  http://www.7-zip.org/
;;
;;  LZMA SDK is licensed under two licenses:
;;  1) GNU Lesser General Public License (GNU LGPL)
;;  2) Common Public License (CPL)
;;  It means that you can select one of these two licenses and 
;;  follow rules of that license.
;;
;;  SPECIAL EXCEPTION:
;;  Igor Pavlov, as the author of this Code, expressly permits you to 
;;  statically or dynamically link your Code (or bind by name) to the 
;;  interfaces of this file without subjecting your linked Code to the 
;;  terms of the CPL or GNU LGPL. Any modifications or additions 
;;  to this file, however, are subject to the LGPL or CPL terms.
;; ----------------------------------------------------------------------------
;;
;;  #define LZMAASM_WORKMEM_SIZE      0x4000    /* aligned 0x3e6c */
;;
;;  #define LZMAASM_RESULT_OK         0
;;  #define LZMAASM_RESULT_DATA_ERROR 1
;;
;;  int LzmaDecodeAsm(const unsigned char* inStream, unsigned int inSize,
;;                    unsigned char* outStream, unsigned int outSize,
;;                    void* WorkMem);
;; ----------------------------------------------------------------------------

align 4
lzma_decode_asm	proc near
; parameter 1: 72 + esp
; parameter 2: 76 + esp
; parameter 3: 80 + esp
; parameter 4: 84 + esp
; parameter 5: 88 + esp
$B1$1:                          ; Preds $B1$0
        push      edi                                           ;145.1
        push      esi                                           ;145.1
        push      ebp                                           ;145.1
        push      ebx                                           ;145.1
        sub       esp, 52                                       ;145.1
        mov       ebp, DWORD PTR [esp+72]                       ;142.5
        mov       ebx, DWORD PTR [esp+76]                       ;142.5
        mov       eax, DWORD PTR [esp+88]                       ;142.5
        lea       ecx, DWORD PTR [eax+15980]                    ;171.10
        
                                ; LOE eax ecx ebx ebp
$B1$2:                          ; Preds $B1$2 $B1$1
        mov       edx, 1024                                     ;172.7
        mov       WORD PTR [eax], dx                            ;172.7
        add       eax, 2                                        ;171.31
        cmp       ecx, eax                                      ;171.5
        ja        $B1$2         ; Prob 90%                      ;171.5
                                ; LOE eax ecx ebx ebp
$B1$3:                          ; Preds $B1$2
        lea       eax, DWORD PTR [ebx+ebp]                      ;175.3
        mov       DWORD PTR [esp+24], eax                       ;175.3
        xor       ebx, ebx                                      ;
        xor       edx, edx                                      ;
        
                                ; LOE edx ebx ebp
$B1$4:                          ; Preds $B1$6 $B1$3
        mov       eax, DWORD PTR [esp+24]                       ;175.3
        cmp       ebp, eax                                      ;175.3
        jne       $B1$6         ; Prob 99%                      ;175.3
                                ; LOE edx ebx ebp
$B1$5:                          ; Preds $B1$4
        push      1                                             ;175.3
        pop       eax                                           ;175.3
$L1:                                                            ;
        add       esp, 52                                       ;175.3
        pop       ebx                                           ;175.3
        pop       ebp                                           ;175.3
        pop       esi                                           ;175.3
        pop       edi                                           ;175.3
        ret                                                     ;175.3
                                ; LOE
$B1$6:                          ; Preds $B1$4
        movzx     eax, BYTE PTR [ebp]                           ;175.3
        shl       ebx, 8                                        ;175.3
        or        ebx, eax                                      ;175.3
        inc       ebp                                           ;175.3
        inc       edx                                           ;175.3
        cmp       edx, 5                                        ;175.3
        jl        $B1$4         ; Prob 80%                      ;175.3
                                ; LOE edx ebx ebp
$B1$7:                          ; Preds $B1$6
        mov       eax, DWORD PTR [esp+84]                       ;177.3
        test      eax, eax                                      ;177.3
        jbe       $B1$159       ; Prob 2%                       ;177.3
                                ; LOE ebx ebp
$B1$8:                          ; Preds $B1$7
        xor       esi, esi                                      ;
        mov       DWORD PTR [esp+28], esi                       ;
        xor       eax, eax                                      ;
        xor       edx, edx                                      ;
        push      1                                             ;
        pop       ecx                                           ;
        mov       DWORD PTR [esp+12], eax                       ;
        mov       DWORD PTR [esp+20], edx                       ;
        mov       DWORD PTR [esp+8], ecx                        ;
        push      1                                             ;
        pop       edi                                           ;
        mov       DWORD PTR [esp+44], edi                       ;
        push      1                                             ;
        pop       eax                                           ;
        push      -1                                            ;
        pop       edx                                           ;
        mov       DWORD PTR [esp+40], eax                       ;
        push      1                                             ;
        pop       eax                                           ;
        mov       DWORD PTR [esp+48], eax                       ;
                                ; LOE edx ebx ebp
$B1$9:                          ; Preds $B1$156 $B1$8
        mov       ecx, DWORD PTR [esp+28]                       ;181.35
        mov       eax, DWORD PTR [esp+20]                       ;183.12
        and       ecx, 3                                        ;181.35
        shl       eax, 5                                        ;183.12
        add       eax, DWORD PTR [esp+88]                       ;183.12
        mov       DWORD PTR [esp+16], eax                       ;183.12
        cmp       edx, 16777216                                 ;184.5
        jae       $B1$13        ; Prob 50%                      ;184.5
                                ; LOE edx ecx ebx ebp
$B1$10:                         ; Preds $B1$9
        mov       eax, DWORD PTR [esp+24]                       ;184.5
        cmp       ebp, eax                                      ;184.5
        jne       $B1$12        ; Prob 99%                      ;184.5
                                ; LOE edx ecx ebx ebp
$B1$11:                         ; Preds $B1$10
        jmp       $B1$5         ; Prob 0%                       ;184.5
                                ; LOE
$B1$12:                         ; Preds $B1$10
        shl       edx, 8                                        ;184.5
        mov       eax, ebp                                      ;181.35
        movzx     esi, BYTE PTR [eax]                           ;184.5
        inc       ebp                                           ;184.5
        shl       ebx, 8                                        ;184.5
        or        ebx, esi                                      ;184.5
                                ; LOE edx ecx ebx ebp
$B1$13:                         ; Preds $B1$12 $B1$9
        mov       eax, DWORD PTR [esp+16]                       ;184.5
        movzx     esi, WORD PTR [eax+ecx*2]                     ;184.5
        mov       DWORD PTR [esp+4], esi                        ;184.5
        mov       eax, edx                                      ;184.5
        shr       eax, 11                                       ;184.5
        imul      eax, esi                                      ;184.5
        cmp       ebx, eax                                      ;184.5
        jae       $B1$44        ; Prob 62%                      ;184.5
                                ; LOE eax edx ecx ebx ebp esi
$B1$14:                         ; Preds $B1$13
        mov       edi, esi                                      ;187.7
        mov       edx, eax                                      ;184.5
        mov       eax, DWORD PTR [esp+16]                       ;187.7
        mov       esi, edi                                      ;187.7
        neg       esi                                           ;187.7
        add       esi, 2048                                     ;187.7
        sar       esi, 5                                        ;187.7
        add       edi, esi                                      ;187.7
        mov       WORD PTR [eax+ecx*2], di                      ;187.7
        mov       ecx, DWORD PTR [esp+12]                       ;189.68
        sar       ecx, 5                                        ;189.68
        imul      esi, ecx, 1536                                ;188.14
        mov       ecx, DWORD PTR [esp+88]                       ;188.14
        lea       eax, DWORD PTR [ecx+esi]                      ;188.14
        mov       DWORD PTR [esp+12], eax                       ;188.14
        lea       eax, DWORD PTR [ecx+esi+3692]                 ;188.14
        mov       DWORD PTR [esp+4], eax                        ;188.14
        mov       eax, DWORD PTR [esp+20]                       ;191.7
        cmp       eax, 7                                        ;191.7
        jl        $B1$28        ; Prob 10%                      ;191.7
                                ; LOE edx ebx ebp
$B1$15:                         ; Preds $B1$14
        mov       ecx, DWORD PTR [esp+80]                       ;194.21
        mov       eax, DWORD PTR [esp+28]                       ;194.21
        lea       esi, DWORD PTR [eax+ecx]                      ;194.21
        push      1                                             ;
        pop       eax                                           ;
        sub       esi, DWORD PTR [esp+8]                        ;194.40
        movzx     edi, BYTE PTR [esi]                           ;194.21
        mov       DWORD PTR [esp+16], edi                       ;194.21
        
                                ; LOE eax edx ebx ebp
$B1$16:                         ; Preds $B1$26 $B1$15
        mov       esi, DWORD PTR [esp+16]                       ;199.11
        mov       edi, DWORD PTR [esp+4]                        ;201.21
        add       esi, esi                                      ;199.11
        mov       DWORD PTR [esp+16], esi                       ;199.11
        and       esi, 256                                      ;200.30
        cmp       edx, 16777216                                 ;202.11
        lea       ecx, DWORD PTR [eax+eax]                      ;201.21
        mov       DWORD PTR [esp], ecx                          ;201.21
        lea       ecx, DWORD PTR [edi+eax*2]                    ;201.21
        mov       DWORD PTR [esp+36], ecx                       ;201.21
        jae       $B1$20        ; Prob 50%                      ;202.11
                                ; LOE eax edx ebx ebp esi
$B1$17:                         ; Preds $B1$16
        mov       ecx, DWORD PTR [esp+24]                       ;202.11
        cmp       ebp, ecx                                      ;202.11
        jne       $B1$19        ; Prob 99%                      ;202.11
                                ; LOE eax edx ebx ebp esi
$B1$18:                         ; Preds $B1$17
        jmp       $B1$5         ; Prob 0%                       ;202.11
                                ; LOE
$B1$19:                         ; Preds $B1$17
        shl       edx, 8                                        ;202.11
        mov       ecx, ebp                                      ;199.11
        movzx     edi, BYTE PTR [ecx]                           ;202.11
        inc       ebp                                           ;202.11
        shl       ebx, 8                                        ;202.11
        or        ebx, edi                                      ;202.11
                                ; LOE eax edx ebx ebp esi
$B1$20:                         ; Preds $B1$19 $B1$16
        mov       ecx, DWORD PTR [esp+36]                       ;202.11
        movzx     edi, WORD PTR [ecx+esi*2+512]                 ;202.11
        mov       DWORD PTR [esp+32], edi                       ;202.11
        mov       ecx, edx                                      ;202.11
        shr       ecx, 11                                       ;202.11
        imul      ecx, edi                                      ;202.11
        cmp       ebx, ecx                                      ;202.11
        jae       $B1$23        ; Prob 62%                      ;202.11
                                ; LOE eax edx ecx ebx ebp esi edi
$B1$21:                         ; Preds $B1$20
        mov       edi, DWORD PTR [esp+36]                       ;202.11
        mov       edx, ecx                                      ;202.11
        mov       ecx, DWORD PTR [esp+32]                       ;202.11
        mov       eax, ecx                                      ;202.11
        neg       eax                                           ;202.11
        add       eax, 2048                                     ;202.11
        sar       eax, 5                                        ;202.11
        add       ecx, eax                                      ;202.11
        mov       eax, DWORD PTR [esp]                          ;201.21
        mov       WORD PTR [edi+esi*2+512], cx                  ;202.11
        test      esi, esi                                      ;202.11
        jne       $B1$25        ; Prob 10%                      ;202.11
        jmp       $B1$26        ; Prob 100%                     ;202.11
        ALIGN     4
                                ; LOE eax edx ebx ebp al ah
$B1$23:                         ; Preds $B1$20
        sub       edx, ecx                                      ;202.11
        sub       ebx, ecx                                      ;202.11
        mov       ecx, edi                                      ;202.11
        mov       edi, ecx                                      ;202.11
        sar       edi, 5                                        ;202.11
        sub       ecx, edi                                      ;202.11
        mov       edi, DWORD PTR [esp+36]                       ;202.11
        mov       WORD PTR [edi+esi*2+512], cx                  ;202.11
        lea       eax, DWORD PTR [eax+eax+1]                    ;202.11
        test      esi, esi                                      ;202.11
        jne       $B1$26        ; Prob 90%                      ;202.11
                                ; LOE eax edx ebx ebp
$B1$25:                         ; Preds $B1$23 $B1$21
        cmp       eax, 256                                      ;206.7
        jl        $B1$30        ; Prob 98%                      ;206.7
        jmp       $B1$39        ; Prob 100%                     ;206.7
        ALIGN     4
                                ; LOE eax edx ebx ebp
$B1$26:                         ; Preds $B1$21 $B1$23
        cmp       eax, 256                                      ;204.25
        jl        $B1$16        ; Prob 90%                      ;204.25
        jmp       $B1$39        ; Prob 100%                     ;204.25
        ALIGN     4
                                ; LOE eax edx ebx ebp
$B1$28:                         ; Preds $B1$14
        push      1                                             ;
        pop       eax                                           ;
        
                                ; LOE eax edx ebx ebp
$B1$30:                         ; Preds $B1$25 $B1$28 $B1$37
        cmp       edx, 16777216                                 ;209.9
        lea       esi, DWORD PTR [eax+eax]                      ;208.26
        jae       $B1$34        ; Prob 50%                      ;209.9
                                ; LOE eax edx ebx ebp esi
$B1$31:                         ; Preds $B1$30
        mov       ecx, DWORD PTR [esp+24]                       ;209.9
        cmp       ebp, ecx                                      ;209.9
        jne       $B1$33        ; Prob 99%                      ;209.9
                                ; LOE eax edx ebx ebp esi
$B1$32:                         ; Preds $B1$31
        jmp       $B1$5         ; Prob 0%                       ;209.9
                                ; LOE
$B1$33:                         ; Preds $B1$31
        shl       edx, 8                                        ;209.9
        mov       ecx, ebp                                      ;208.26
        movzx     edi, BYTE PTR [ecx]                           ;209.9
        inc       ebp                                           ;209.9
        shl       ebx, 8                                        ;209.9
        or        ebx, edi                                      ;209.9
                                ; LOE eax edx ebx ebp esi
$B1$34:                         ; Preds $B1$33 $B1$30
        mov       ecx, DWORD PTR [esp+12]                       ;209.9
        movzx     ecx, WORD PTR [ecx+esi+3692]                  ;209.9
        mov       edi, edx                                      ;209.9
        shr       edi, 11                                       ;209.9
        imul      edi, ecx                                      ;209.9
        cmp       ebx, edi                                      ;209.9
        jae       $B1$36        ; Prob 62%                      ;209.9
                                ; LOE eax edx ecx ebx ebp esi edi
$B1$35:                         ; Preds $B1$34
        mov       edx, edi                                      ;209.9
        mov       edi, DWORD PTR [esp+12]                       ;209.9
        mov       eax, ecx                                      ;209.9
        neg       eax                                           ;209.9
        add       eax, 2048                                     ;209.9
        sar       eax, 5                                        ;209.9
        add       ecx, eax                                      ;209.9
        mov       WORD PTR [edi+esi+3692], cx                   ;209.9
        mov       eax, esi                                      ;208.26
        jmp       $B1$37        ; Prob 100%                     ;208.26
        ALIGN     4
                                ; LOE eax edx ebx ebp
$B1$36:                         ; Preds $B1$34
        sub       edx, edi                                      ;209.9
        sub       ebx, edi                                      ;209.9
        mov       edi, ecx                                      ;209.9
        sar       edi, 5                                        ;209.9
        sub       ecx, edi                                      ;209.9
        lea       eax, DWORD PTR [eax+eax+1]                    ;209.9
        mov       edi, DWORD PTR [esp+12]                       ;209.9
        mov       WORD PTR [edi+esi+3692], cx                   ;209.9
                                ; LOE eax edx ebx ebp
$B1$37:                         ; Preds $B1$35 $B1$36
        cmp       eax, 256                                      ;206.7
        jl        $B1$30        ; Prob 90%                      ;206.7
                                ; LOE eax edx ebx ebp
$B1$39:                         ; Preds $B1$37 $B1$26 $B1$25
        mov       ecx, DWORD PTR [esp+80]                       ;213.7
        mov       esi, DWORD PTR [esp+28]                       ;213.7
        mov       edi, DWORD PTR [esp+20]                       ;214.7
        movzx     eax, al                                       ;211.28
        mov       DWORD PTR [esp+12], eax                       ;211.28
        mov       BYTE PTR [ecx+esi], al                        ;213.7
        inc       esi                                           ;213.17
        mov       DWORD PTR [esp+28], esi                       ;213.17
        cmp       edi, 4                                        ;214.7
        jge       $B1$41        ; Prob 38%                      ;214.7
                                ; LOE edx ebx ebp edi
$B1$40:                         ; Preds $B1$39
        xor       eax, eax                                      ;214.22
        mov       DWORD PTR [esp+20], eax                       ;214.22
        jmp       $B1$156       ; Prob 100%                     ;214.22
        ALIGN     4
                                ; LOE edx ebx ebp
$B1$41:                         ; Preds $B1$39
        mov       eax, edi                                      ;215.12
        cmp       eax, 10                                       ;215.12
        jge       $B1$43        ; Prob 50%                      ;215.12
                                ; LOE edx ebx ebp
$B1$42:                         ; Preds $B1$41
        add       DWORD PTR [esp+20], -3                        ;215.28
        jmp       $B1$156       ; Prob 100%                     ;215.28
        ALIGN     4
                                ; LOE edx ebx ebp
$B1$43:                         ; Preds $B1$41
        add       DWORD PTR [esp+20], -6                        ;216.12
        jmp       $B1$156       ; Prob 100%                     ;216.12
        ALIGN     4
                                ; LOE edx ebx ebp
$B1$44:                         ; Preds $B1$13
        mov       edi, DWORD PTR [esp+16]                       ;220.7
        sub       edx, eax                                      ;220.7
        sub       ebx, eax                                      ;220.7
        mov       eax, esi                                      ;220.7
        sar       eax, 5                                        ;220.7
        sub       esi, eax                                      ;220.7
        mov       eax, DWORD PTR [esp+20]                       ;221.14
        cmp       edx, 16777216                                 ;222.7
        mov       WORD PTR [edi+ecx*2], si                      ;220.7
        lea       eax, DWORD PTR [eax+eax]                      ;221.14
        mov       DWORD PTR [esp], eax                          ;221.14
        jae       $B1$48        ; Prob 50%                      ;222.7
                                ; LOE edx ecx ebx ebp
$B1$45:                         ; Preds $B1$44
        mov       eax, DWORD PTR [esp+24]                       ;222.7
        cmp       ebp, eax                                      ;222.7
        jne       $B1$47        ; Prob 99%                      ;222.7
                                ; LOE edx ecx ebx ebp
$B1$46:                         ; Preds $B1$45
        jmp       $B1$5         ; Prob 0%                       ;222.7
                                ; LOE
$B1$47:                         ; Preds $B1$45
        shl       edx, 8                                        ;222.7
        mov       eax, ebp                                      ;184.5
        movzx     esi, BYTE PTR [eax]                           ;222.7
        inc       ebp                                           ;222.7
        shl       ebx, 8                                        ;222.7
        or        ebx, esi                                      ;222.7
                                ; LOE edx ecx ebx ebp
$B1$48:                         ; Preds $B1$47 $B1$44
        mov       eax, DWORD PTR [esp+88]                       ;222.7
        mov       esi, DWORD PTR [esp]                          ;222.7
        movzx     edi, WORD PTR [eax+esi+384]                   ;222.7
        mov       DWORD PTR [esp+4], edi                        ;222.7
        mov       eax, edx                                      ;222.7
        shr       eax, 11                                       ;222.7
        imul      eax, edi                                      ;222.7
        cmp       ebx, eax                                      ;222.7
        jae       $B1$53        ; Prob 62%                      ;222.7
                                ; LOE eax edx ecx ebx ebp edi
$B1$49:                         ; Preds $B1$48
        mov       edx, edi                                      ;224.9
        mov       edi, DWORD PTR [esp+88]                       ;224.9
        mov       esi, edx                                      ;224.9
        neg       esi                                           ;224.9
        add       esi, 2048                                     ;224.9
        sar       esi, 5                                        ;224.9
        add       edx, esi                                      ;224.9
        mov       esi, DWORD PTR [esp]                          ;224.9
        mov       WORD PTR [edi+esi+384], dx                    ;224.9
        mov       edx, DWORD PTR [esp+40]                       ;181.35
        mov       DWORD PTR [esp+48], edx                       ;181.35
        mov       edx, DWORD PTR [esp+44]                       ;181.35
        mov       DWORD PTR [esp+40], edx                       ;181.35
        mov       edx, DWORD PTR [esp+8]                        ;181.35
        mov       DWORD PTR [esp+44], edx                       ;181.35
        mov       edx, DWORD PTR [esp+20]                       ;228.9
        cmp       edx, 7                                        ;228.9
        jge       $B1$51        ; Prob 50%                      ;228.9
                                ; LOE eax ecx ebx ebp
$B1$50:                         ; Preds $B1$49
        xor       edx, edx                                      ;228.9
        mov       DWORD PTR [esp+20], edx                       ;228.9
        jmp       $B1$52        ; Prob 100%                     ;228.9
        ALIGN     4
                                ; LOE eax ecx ebx ebp
$B1$51:                         ; Preds $B1$49
        push      3                                             ;228.9
        pop       edx                                           ;228.9
        mov       DWORD PTR [esp+20], edx                       ;228.9
                                ; LOE eax ecx ebx ebp
$B1$52:                         ; Preds $B1$50 $B1$51
        mov       edx, DWORD PTR [esp+88]                       ;229.16
        lea       esi, DWORD PTR [edx+1636]                     ;229.16
        mov       DWORD PTR [esp], esi                          ;229.16
        jmp       $B1$89        ; Prob 100%                     ;229.16
        ALIGN     4
                                ; LOE eax ecx ebx ebp
$B1$53:                         ; Preds $B1$48
        mov       edi, DWORD PTR [esp+88]                       ;233.9
        sub       edx, eax                                      ;233.9
        sub       ebx, eax                                      ;233.9
        mov       eax, DWORD PTR [esp+4]                        ;233.9
        mov       esi, eax                                      ;233.9
        sar       esi, 5                                        ;233.9
        sub       eax, esi                                      ;233.9
        mov       esi, DWORD PTR [esp]                          ;233.9
        mov       WORD PTR [edi+esi+384], ax                    ;233.9
        cmp       edx, 16777216                                 ;235.9
        jae       $B1$57        ; Prob 50%                      ;235.9
                                ; LOE edx ecx ebx ebp
$B1$54:                         ; Preds $B1$53
        mov       eax, DWORD PTR [esp+24]                       ;235.9
        cmp       ebp, eax                                      ;235.9
        jne       $B1$56        ; Prob 99%                      ;235.9
                                ; LOE edx ecx ebx ebp
$B1$55:                         ; Preds $B1$54
        jmp       $B1$5         ; Prob 0%                       ;235.9
                                ; LOE
$B1$56:                         ; Preds $B1$54
        shl       edx, 8                                        ;235.9
        mov       eax, ebp                                      ;222.7
        movzx     esi, BYTE PTR [eax]                           ;235.9
        inc       ebp                                           ;235.9
        shl       ebx, 8                                        ;235.9
        or        ebx, esi                                      ;235.9
                                ; LOE edx ecx ebx ebp
$B1$57:                         ; Preds $B1$56 $B1$53
        mov       eax, DWORD PTR [esp+88]                       ;235.9
        mov       esi, DWORD PTR [esp]                          ;235.9
        movzx     edi, WORD PTR [eax+esi+408]                   ;235.9
        mov       DWORD PTR [esp+4], edi                        ;235.9
        mov       eax, edx                                      ;235.9
        shr       eax, 11                                       ;235.9
        imul      eax, edi                                      ;235.9
        cmp       ebx, eax                                      ;235.9
        jae       $B1$70        ; Prob 62%                      ;235.9
                                ; LOE eax edx ecx ebx ebp edi
$B1$58:                         ; Preds $B1$57
        mov       edx, edi                                      ;237.11
        mov       edi, DWORD PTR [esp+88]                       ;237.11
        mov       esi, edx                                      ;237.11
        neg       esi                                           ;237.11
        add       esi, 2048                                     ;237.11
        sar       esi, 5                                        ;237.11
        add       edx, esi                                      ;237.11
        mov       esi, DWORD PTR [esp]                          ;237.11
        mov       WORD PTR [edi+esi+408], dx                    ;237.11
        cmp       eax, 16777216                                 ;239.11
        jae       $B1$62        ; Prob 50%                      ;239.11
                                ; LOE eax ecx ebx ebp
$B1$59:                         ; Preds $B1$58
        mov       edx, DWORD PTR [esp+24]                       ;239.11
        cmp       ebp, edx                                      ;239.11
        jne       $B1$61        ; Prob 99%                      ;239.11
                                ; LOE eax ecx ebx ebp
$B1$60:                         ; Preds $B1$59
        jmp       $B1$5         ; Prob 0%                       ;239.11
                                ; LOE
$B1$61:                         ; Preds $B1$59
        shl       eax, 8                                        ;239.11
        mov       edx, ebp                                      ;235.9
        movzx     esi, BYTE PTR [edx]                           ;239.11
        inc       ebp                                           ;239.11
        shl       ebx, 8                                        ;239.11
        or        ebx, esi                                      ;239.11
                                ; LOE eax ecx ebx ebp
$B1$62:                         ; Preds $B1$61 $B1$58
        mov       edx, DWORD PTR [esp+16]                       ;239.11
        movzx     esi, WORD PTR [edx+ecx*2+480]                 ;239.11
        mov       DWORD PTR [esp+32], esi                       ;239.11
        mov       edx, eax                                      ;239.11
        shr       edx, 11                                       ;239.11
        imul      edx, esi                                      ;239.11
        cmp       ebx, edx                                      ;239.11
        jae       $B1$69        ; Prob 62%                      ;239.11
                                ; LOE eax edx ecx ebx ebp esi
$B1$63:                         ; Preds $B1$62
        mov       edi, DWORD PTR [esp+16]                       ;241.13
        mov       eax, esi                                      ;241.13
        neg       eax                                           ;241.13
        add       eax, 2048                                     ;241.13
        sar       eax, 5                                        ;241.13
        add       esi, eax                                      ;241.13
        mov       WORD PTR [edi+ecx*2+480], si                  ;241.13
        mov       ecx, DWORD PTR [esp+28]                       ;243.13
        test      ecx, ecx                                      ;243.13
        jne       $B1$65        ; Prob 96%                      ;243.13
                                ; LOE edx ebx ebp
$B1$64:                         ; Preds $B1$63
        jmp       $B1$5         ; Prob 0%                       ;244.22
                                ; LOE
$B1$65:                         ; Preds $B1$63
        mov       eax, DWORD PTR [esp+20]                       ;246.13
        cmp       eax, 7                                        ;246.13
        jge       $B1$67        ; Prob 50%                      ;246.13
                                ; LOE edx ebx ebp
$B1$66:                         ; Preds $B1$65
        push      9                                             ;246.13
        pop       eax                                           ;246.13
        mov       DWORD PTR [esp+20], eax                       ;246.13
        jmp       $B1$68        ; Prob 100%                     ;246.13
        ALIGN     4
                                ; LOE edx ebx ebp
$B1$67:                         ; Preds $B1$65
        push      11                                            ;246.13
        pop       eax                                           ;246.13
        mov       DWORD PTR [esp+20], eax                       ;246.13
                                ; LOE edx ebx ebp
$B1$68:                         ; Preds $B1$66 $B1$67
        mov       ecx, DWORD PTR [esp+80]                       ;247.28
        mov       esi, DWORD PTR [esp+28]                       ;247.28
        lea       edi, DWORD PTR [esi+ecx]                      ;247.28
        sub       edi, DWORD PTR [esp+8]                        ;247.47
        movzx     eax, BYTE PTR [edi]                           ;247.28
        mov       DWORD PTR [esp+12], eax                       ;247.28
        mov       edi, esi                                      ;181.35
        inc       esi                                           ;248.23
        mov       DWORD PTR [esp+28], esi                       ;248.23
        mov       BYTE PTR [edi+ecx], al                        ;248.13
        jmp       $B1$156       ; Prob 100%                     ;248.13
        ALIGN     4
                                ; LOE edx ebx ebp
$B1$69:                         ; Preds $B1$62
        mov       edi, DWORD PTR [esp+16]                       ;253.13
        sub       eax, edx                                      ;253.13
        sub       ebx, edx                                      ;253.13
        mov       edx, esi                                      ;253.13
        sar       edx, 5                                        ;253.13
        sub       esi, edx                                      ;253.13
        mov       WORD PTR [edi+ecx*2+480], si                  ;253.13
        jmp       $B1$85        ; Prob 100%                     ;253.13
        ALIGN     4
                                ; LOE eax ecx ebx ebp
$B1$70:                         ; Preds $B1$57
        mov       edi, DWORD PTR [esp+88]                       ;259.11
        sub       edx, eax                                      ;259.11
        sub       ebx, eax                                      ;259.11
        mov       eax, DWORD PTR [esp+4]                        ;259.11
        mov       esi, eax                                      ;259.11
        sar       esi, 5                                        ;259.11
        sub       eax, esi                                      ;259.11
        mov       esi, DWORD PTR [esp]                          ;259.11
        mov       WORD PTR [edi+esi+408], ax                    ;259.11
        cmp       edx, 16777216                                 ;261.11
        jae       $B1$74        ; Prob 50%                      ;261.11
                                ; LOE edx ecx ebx ebp
$B1$71:                         ; Preds $B1$70
        mov       eax, DWORD PTR [esp+24]                       ;261.11
        cmp       ebp, eax                                      ;261.11
        jne       $B1$73        ; Prob 99%                      ;261.11
                                ; LOE edx ecx ebx ebp
$B1$72:                         ; Preds $B1$71
        jmp       $B1$5         ; Prob 0%                       ;261.11
                                ; LOE
$B1$73:                         ; Preds $B1$71
        shl       edx, 8                                        ;261.11
        mov       eax, ebp                                      ;235.9
        movzx     esi, BYTE PTR [eax]                           ;261.11
        inc       ebp                                           ;261.11
        shl       ebx, 8                                        ;261.11
        or        ebx, esi                                      ;261.11
                                ; LOE edx ecx ebx ebp
$B1$74:                         ; Preds $B1$73 $B1$70
        mov       eax, DWORD PTR [esp+88]                       ;261.11
        mov       esi, DWORD PTR [esp]                          ;261.11
        movzx     edi, WORD PTR [eax+esi+432]                   ;261.11
        mov       DWORD PTR [esp+12], edi                       ;261.11
        mov       eax, edx                                      ;261.11
        shr       eax, 11                                       ;261.11
        imul      eax, edi                                      ;261.11
        cmp       ebx, eax                                      ;261.11
        jae       $B1$76        ; Prob 62%                      ;261.11
                                ; LOE eax edx ecx ebx ebp edi
$B1$75:                         ; Preds $B1$74
        mov       edx, edi                                      ;263.13
        mov       edi, DWORD PTR [esp+88]                       ;263.13
        mov       esi, edx                                      ;263.13
        neg       esi                                           ;263.13
        add       esi, 2048                                     ;263.13
        sar       esi, 5                                        ;263.13
        add       edx, esi                                      ;263.13
        mov       esi, DWORD PTR [esp]                          ;263.13
        mov       WORD PTR [edi+esi+432], dx                    ;263.13
        mov       edx, DWORD PTR [esp+44]                       ;181.35
        jmp       $B1$84        ; Prob 100%                     ;181.35
        ALIGN     4
                                ; LOE eax edx ecx ebx ebp
$B1$76:                         ; Preds $B1$74
        mov       edi, DWORD PTR [esp+88]                       ;268.13
        sub       edx, eax                                      ;268.13
        sub       ebx, eax                                      ;268.13
        mov       eax, DWORD PTR [esp+12]                       ;268.13
        mov       esi, eax                                      ;268.13
        sar       esi, 5                                        ;268.13
        sub       eax, esi                                      ;268.13
        mov       esi, DWORD PTR [esp]                          ;268.13
        mov       WORD PTR [edi+esi+432], ax                    ;268.13
        cmp       edx, 16777216                                 ;270.13
        jae       $B1$80        ; Prob 50%                      ;270.13
                                ; LOE edx ecx ebx ebp
$B1$77:                         ; Preds $B1$76
        mov       eax, DWORD PTR [esp+24]                       ;270.13
        cmp       ebp, eax                                      ;270.13
        jne       $B1$79        ; Prob 99%                      ;270.13
                                ; LOE edx ecx ebx ebp
$B1$78:                         ; Preds $B1$77
        jmp       $B1$5         ; Prob 0%                       ;270.13
                                ; LOE
$B1$79:                         ; Preds $B1$77
        shl       edx, 8                                        ;270.13
        mov       eax, ebp                                      ;261.11
        movzx     esi, BYTE PTR [eax]                           ;270.13
        inc       ebp                                           ;270.13
        shl       ebx, 8                                        ;270.13
        or        ebx, esi                                      ;270.13
                                ; LOE edx ecx ebx ebp
$B1$80:                         ; Preds $B1$79 $B1$76
        mov       eax, DWORD PTR [esp+88]                       ;270.13
        mov       esi, DWORD PTR [esp]                          ;270.13
        movzx     edi, WORD PTR [eax+esi+456]                   ;270.13
        mov       DWORD PTR [esp+32], edi                       ;270.13
        mov       esi, edx                                      ;270.13
        shr       esi, 11                                       ;270.13
        imul      esi, edi                                      ;270.13
        cmp       ebx, esi                                      ;270.13
        jae       $B1$82        ; Prob 62%                      ;270.13
                                ; LOE edx ecx ebx ebp esi edi
$B1$81:                         ; Preds $B1$80
        mov       edx, edi                                      ;272.15
        mov       edi, DWORD PTR [esp+88]                       ;272.15
        mov       eax, esi                                      ;270.13
        mov       esi, edx                                      ;272.15
        neg       esi                                           ;272.15
        add       esi, 2048                                     ;272.15
        sar       esi, 5                                        ;272.15
        add       edx, esi                                      ;272.15
        mov       esi, DWORD PTR [esp]                          ;272.15
        mov       WORD PTR [edi+esi+456], dx                    ;272.15
        mov       edx, DWORD PTR [esp+40]                       ;181.35
        jmp       $B1$83        ; Prob 100%                     ;181.35
        ALIGN     4
                                ; LOE eax edx ecx ebx ebp
$B1$82:                         ; Preds $B1$80
        mov       edi, DWORD PTR [esp+88]                       ;277.15
        mov       eax, edx                                      ;277.15
        mov       edx, DWORD PTR [esp+32]                       ;277.15
        sub       eax, esi                                      ;277.15
        sub       ebx, esi                                      ;277.15
        mov       esi, edx                                      ;277.15
        sar       esi, 5                                        ;277.15
        sub       edx, esi                                      ;277.15
        mov       esi, DWORD PTR [esp]                          ;277.15
        mov       WORD PTR [edi+esi+456], dx                    ;277.15
        mov       edx, DWORD PTR [esp+48]                       ;181.35
        mov       esi, DWORD PTR [esp+40]                       ;181.35
        mov       DWORD PTR [esp+48], esi                       ;181.35
                                ; LOE eax edx ecx ebx ebp
$B1$83:                         ; Preds $B1$81 $B1$82
        mov       esi, DWORD PTR [esp+44]                       ;181.35
        mov       DWORD PTR [esp+40], esi                       ;181.35
                                ; LOE eax edx ecx ebx ebp
$B1$84:                         ; Preds $B1$75 $B1$83
        mov       esi, DWORD PTR [esp+8]                        ;181.35
        mov       DWORD PTR [esp+44], esi                       ;181.35
        mov       DWORD PTR [esp+8], edx                        ;283.11
                                ; LOE eax ecx ebx ebp
$B1$85:                         ; Preds $B1$69 $B1$84
        mov       edx, DWORD PTR [esp+20]                       ;286.9
        cmp       edx, 7                                        ;286.9
        jge       $B1$87        ; Prob 50%                      ;286.9
                                ; LOE eax ecx ebx ebp
$B1$86:                         ; Preds $B1$85
        push      8                                             ;286.9
        pop       edx                                           ;286.9
        mov       DWORD PTR [esp+20], edx                       ;286.9
        jmp       $B1$88        ; Prob 100%                     ;286.9
        ALIGN     4
                                ; LOE eax ecx ebx ebp
$B1$87:                         ; Preds $B1$85
        push      11                                            ;286.9
        pop       edx                                           ;286.9
        mov       DWORD PTR [esp+20], edx                       ;286.9
                                ; LOE eax ecx ebx ebp
$B1$88:                         ; Preds $B1$86 $B1$87
        mov       edx, DWORD PTR [esp+88]                       ;287.16
        lea       esi, DWORD PTR [edx+2664]                     ;287.16
        mov       DWORD PTR [esp], esi                          ;287.16
                                ; LOE eax ecx ebx ebp
$B1$89:                         ; Preds $B1$52 $B1$88
        cmp       eax, 16777216                                 ;292.9
        jae       $B1$93        ; Prob 50%                      ;292.9
                                ; LOE eax ecx ebx ebp
$B1$90:                         ; Preds $B1$89
        mov       edx, DWORD PTR [esp+24]                       ;292.9
        cmp       ebp, edx                                      ;292.9
        jne       $B1$92        ; Prob 99%                      ;292.9
                                ; LOE eax ecx ebx ebp
$B1$91:                         ; Preds $B1$90
        jmp       $B1$5         ; Prob 0%                       ;292.9
                                ; LOE
$B1$92:                         ; Preds $B1$90
        shl       eax, 8                                        ;292.9
        mov       edx, ebp                                      ;291.26
        movzx     esi, BYTE PTR [edx]                           ;292.9
        inc       ebp                                           ;292.9
        shl       ebx, 8                                        ;292.9
        or        ebx, esi                                      ;292.9
                                ; LOE eax ecx ebx ebp
$B1$93:                         ; Preds $B1$92 $B1$89
        mov       edx, DWORD PTR [esp]                          ;292.9
        movzx     esi, WORD PTR [edx]                           ;292.9
        mov       edx, eax                                      ;292.9
        shr       edx, 11                                       ;292.9
        imul      edx, esi                                      ;292.9
        cmp       ebx, edx                                      ;292.9
        jae       $B1$95        ; Prob 62%                      ;292.9
                                ; LOE eax edx ecx ebx ebp esi
$B1$94:                         ; Preds $B1$93
        mov       edi, DWORD PTR [esp]                          ;294.11
        mov       eax, esi                                      ;294.11
        neg       eax                                           ;294.11
        add       eax, 2048                                     ;294.11
        sar       eax, 5                                        ;294.11
        add       esi, eax                                      ;294.11
        mov       WORD PTR [edi], si                            ;294.11
        shl       ecx, 4                                        ;295.21
        lea       ecx, DWORD PTR [ecx+edi+4]                    ;295.21
        mov       DWORD PTR [esp], ecx                          ;295.21
        xor       esi, esi                                      ;
        push      3                                             ;
        pop       eax                                           ;
        mov       DWORD PTR [esp+4], esi                        ;
        mov       DWORD PTR [esp+12], eax                       ;
        jmp       $B1$102       ; Prob 100%                     ;
        ALIGN     4
                                ; LOE edx ebx ebp
$B1$95:                         ; Preds $B1$93
        mov       edi, DWORD PTR [esp]                          ;301.11
        sub       eax, edx                                      ;301.11
        sub       ebx, edx                                      ;301.11
        mov       edx, esi                                      ;301.11
        sar       edx, 5                                        ;301.11
        sub       esi, edx                                      ;301.11
        cmp       eax, 16777216                                 ;303.11
        mov       WORD PTR [edi], si                            ;301.11
        lea       edi, DWORD PTR [edi+2]                        ;302.21
        jae       $B1$99        ; Prob 50%                      ;303.11
                                ; LOE eax ecx ebx ebp edi
$B1$96:                         ; Preds $B1$95
        mov       edx, DWORD PTR [esp+24]                       ;303.11
        cmp       ebp, edx                                      ;303.11
        jne       $B1$98        ; Prob 99%                      ;303.11
                                ; LOE eax ecx ebx ebp edi
$B1$97:                         ; Preds $B1$96
        jmp       $B1$5         ; Prob 0%                       ;303.11
                                ; LOE
$B1$98:                         ; Preds $B1$96
        shl       eax, 8                                        ;303.11
        mov       edx, ebp                                      ;292.9
        movzx     esi, BYTE PTR [edx]                           ;303.11
        inc       ebp                                           ;303.11
        shl       ebx, 8                                        ;303.11
        or        ebx, esi                                      ;303.11
                                ; LOE eax ecx ebx ebp edi
$B1$99:                         ; Preds $B1$98 $B1$95
        movzx     edx, WORD PTR [edi]                           ;303.11
        mov       DWORD PTR [esp+4], edx                        ;303.11
        mov       esi, eax                                      ;303.11
        shr       esi, 11                                       ;303.11
        imul      esi, edx                                      ;303.11
        cmp       ebx, esi                                      ;303.11
        jae       $B1$101       ; Prob 62%                      ;303.11
                                ; LOE eax edx ecx ebx ebp esi edi dl dh
$B1$100:                        ; Preds $B1$99
        mov       edx, esi                                      ;303.11
        mov       esi, DWORD PTR [esp+4]                        ;305.13
        mov       eax, esi                                      ;305.13
        neg       eax                                           ;305.13
        add       eax, 2048                                     ;305.13
        sar       eax, 5                                        ;305.13
        add       esi, eax                                      ;305.13
        mov       WORD PTR [edi], si                            ;305.13
        mov       edi, DWORD PTR [esp]                          ;306.23
        shl       ecx, 4                                        ;306.23
        lea       ecx, DWORD PTR [ecx+edi+260]                  ;306.23
        push      8                                             ;
        pop       eax                                           ;
        mov       DWORD PTR [esp], ecx                          ;306.23
        mov       DWORD PTR [esp+4], eax                        ;
        push      3                                             ;
        pop       eax                                           ;
        mov       DWORD PTR [esp+12], eax                       ;
        jmp       $B1$102       ; Prob 100%                     ;
        ALIGN     4
                                ; LOE edx ebx ebp
$B1$101:                        ; Preds $B1$99
        mov       ecx, edx                                      ;312.13
        add       DWORD PTR [esp], 516                          ;313.23
        mov       edx, eax                                      ;312.13
        sub       edx, esi                                      ;312.13
        sub       ebx, esi                                      ;312.13
        mov       eax, ecx                                      ;312.13
        sar       eax, 5                                        ;312.13
        sub       ecx, eax                                      ;312.13
        mov       WORD PTR [edi], cx                            ;312.13
        push      16                                            ;
        pop       esi                                           ;
        mov       DWORD PTR [esp+4], esi                        ;
        push      8                                             ;
        pop       edi                                           ;
        mov       DWORD PTR [esp+12], edi                       ;
                                ; LOE edx ebx ebp
$B1$102:                        ; Preds $B1$94 $B1$100 $B1$101
        mov       ecx, DWORD PTR [esp+12]                       ;318.9
        push      1                                             ;318.9
        pop       esi                                           ;318.9
        
                                ; LOE edx ecx ebx ebp esi
$B1$103:                        ; Preds $B1$110 $B1$102
        mov       edi, DWORD PTR [esp]                          ;318.9
        lea       eax, DWORD PTR [esi+esi]                      ;318.9
        mov       DWORD PTR [esp+16], eax                       ;318.9
        lea       eax, DWORD PTR [edi+esi*2]                    ;318.9
        dec       ecx                                           ;318.9
        mov       DWORD PTR [esp+36], eax                       ;318.9
        cmp       edx, 16777216                                 ;318.9
        jae       $B1$107       ; Prob 50%                      ;318.9
                                ; LOE edx ecx ebx ebp esi
$B1$104:                        ; Preds $B1$103
        mov       eax, DWORD PTR [esp+24]                       ;318.9
        cmp       ebp, eax                                      ;318.9
        jne       $B1$106       ; Prob 99%                      ;318.9
                                ; LOE edx ecx ebx ebp esi
$B1$105:                        ; Preds $B1$104
        jmp       $B1$5         ; Prob 0%                       ;318.9
                                ; LOE
$B1$106:                        ; Preds $B1$104
        shl       edx, 8                                        ;318.9
        mov       eax, ebp                                      ;318.9
        movzx     edi, BYTE PTR [eax]                           ;318.9
        inc       ebp                                           ;318.9
        shl       ebx, 8                                        ;318.9
        or        ebx, edi                                      ;318.9
                                ; LOE edx ecx ebx ebp esi
$B1$107:                        ; Preds $B1$106 $B1$103
        mov       eax, DWORD PTR [esp+36]                       ;318.9
        movzx     edi, WORD PTR [eax]                           ;318.9
        mov       DWORD PTR [esp+32], edi                       ;318.9
        mov       eax, edx                                      ;318.9
        shr       eax, 11                                       ;318.9
        imul      eax, edi                                      ;318.9
        cmp       ebx, eax                                      ;318.9
        jae       $B1$109       ; Prob 62%                      ;318.9
                                ; LOE eax edx ecx ebx ebp esi edi
$B1$108:                        ; Preds $B1$107
        mov       esi, edi                                      ;318.9
        mov       edi, DWORD PTR [esp+36]                       ;318.9
        mov       edx, eax                                      ;318.9
        mov       eax, esi                                      ;318.9
        neg       eax                                           ;318.9
        add       eax, 2048                                     ;318.9
        sar       eax, 5                                        ;318.9
        add       esi, eax                                      ;318.9
        mov       WORD PTR [edi], si                            ;318.9
        mov       esi, DWORD PTR [esp+16]                       ;318.9
        jmp       $B1$110       ; Prob 100%                     ;318.9
        ALIGN     4
                                ; LOE edx ecx ebx ebp esi
$B1$109:                        ; Preds $B1$107
        sub       edx, eax                                      ;318.9
        sub       ebx, eax                                      ;318.9
        mov       eax, edi                                      ;318.9
        mov       edi, eax                                      ;318.9
        sar       edi, 5                                        ;318.9
        sub       eax, edi                                      ;318.9
        mov       edi, DWORD PTR [esp+36]                       ;318.9
        mov       WORD PTR [edi], ax                            ;318.9
        lea       esi, DWORD PTR [esi+esi+1]                    ;318.9
                                ; LOE edx ecx ebx ebp esi
$B1$110:                        ; Preds $B1$108 $B1$109
        test      ecx, ecx                                      ;318.9
        jne       $B1$103       ; Prob 90%                      ;318.9
                                ; LOE edx ecx ebx ebp esi
$B1$111:                        ; Preds $B1$110
        mov       ecx, DWORD PTR [esp+12]                       ;318.9
        mov       edi, DWORD PTR [esp+20]                       ;322.7
        push      1                                             ;318.9
        pop       eax                                           ;318.9
        shl       eax, cl                                       ;318.9
        sub       esi, eax                                      ;318.9
        add       esi, DWORD PTR [esp+4]                        ;319.9
        cmp       edi, 4                                        ;322.7
        jge       $B1$149       ; Prob 38%                      ;322.7
                                ; LOE edx ebx ebp esi
$B1$112:                        ; Preds $B1$111
        add       DWORD PTR [esp+20], 7                         ;325.9
        cmp       esi, 4                                        ;327.21
        jge       $B1$114       ; Prob 50%                      ;327.21
                                ; LOE edx ebx ebp esi
$B1$113:                        ; Preds $B1$112
        mov       DWORD PTR [esp+4], esi                        ;319.9
        jmp       $B1$115       ; Prob 100%                     ;319.9
        ALIGN     4
                                ; LOE edx ebx ebp esi
$B1$114:                        ; Preds $B1$112
        push      3                                             ;327.21
        pop       eax                                           ;327.21
        mov       DWORD PTR [esp+4], eax                        ;327.21
                                ; LOE edx ebx ebp esi
$B1$115:                        ; Preds $B1$113 $B1$114
        mov       eax, DWORD PTR [esp+4]                        ;326.16
        shl       eax, 7                                        ;326.16
        add       eax, DWORD PTR [esp+88]                       ;326.16
        push      6                                             ;
        pop       ecx                                           ;
        mov       DWORD PTR [esp+4], eax                        ;326.16
        mov       DWORD PTR [esp+16], ecx                       ;
        push      1                                             ;
        pop       edi                                           ;
        mov       DWORD PTR [esp], edi                          ;
        
                                ; LOE edx ebx ebp esi
$B1$116:                        ; Preds $B1$123 $B1$115
        mov       eax, DWORD PTR [esp]                          ;329.9
        dec       DWORD PTR [esp+16]                            ;329.9
        lea       ecx, DWORD PTR [eax+eax]                      ;329.9
        mov       DWORD PTR [esp+8], ecx                        ;329.9
        cmp       edx, 16777216                                 ;329.9
        jae       $B1$120       ; Prob 50%                      ;329.9
                                ; LOE edx ebx ebp esi
$B1$117:                        ; Preds $B1$116
        mov       eax, DWORD PTR [esp+24]                       ;329.9
        cmp       ebp, eax                                      ;329.9
        jne       $B1$119       ; Prob 99%                      ;329.9
                                ; LOE edx ebx ebp esi
$B1$118:                        ; Preds $B1$117
        jmp       $B1$5         ; Prob 0%                       ;329.9
                                ; LOE
$B1$119:                        ; Preds $B1$117
        shl       edx, 8                                        ;329.9
        mov       eax, ebp                                      ;329.9
        movzx     ecx, BYTE PTR [eax]                           ;329.9
        inc       ebp                                           ;329.9
        shl       ebx, 8                                        ;329.9
        or        ebx, ecx                                      ;329.9
                                ; LOE edx ebx ebp esi
$B1$120:                        ; Preds $B1$119 $B1$116
        mov       eax, DWORD PTR [esp+4]                        ;329.9
        mov       ecx, DWORD PTR [esp+8]                        ;329.9
        movzx     edi, WORD PTR [eax+ecx+864]                   ;329.9
        mov       DWORD PTR [esp+12], edi                       ;329.9
        mov       eax, edx                                      ;329.9
        shr       eax, 11                                       ;329.9
        imul      eax, edi                                      ;329.9
        cmp       ebx, eax                                      ;329.9
        jae       $B1$122       ; Prob 62%                      ;329.9
                                ; LOE eax edx ebx ebp esi
$B1$121:                        ; Preds $B1$120
        mov       edi, DWORD PTR [esp+4]                        ;329.9
        mov       edx, eax                                      ;329.9
        mov       eax, DWORD PTR [esp+12]                       ;329.9
        mov       ecx, eax                                      ;329.9
        neg       ecx                                           ;329.9
        add       ecx, 2048                                     ;329.9
        sar       ecx, 5                                        ;329.9
        add       eax, ecx                                      ;329.9
        mov       ecx, DWORD PTR [esp+8]                        ;329.9
        mov       WORD PTR [edi+ecx+864], ax                    ;329.9
        mov       DWORD PTR [esp], ecx                          ;329.9
        jmp       $B1$123       ; Prob 100%                     ;329.9
        ALIGN     4
                                ; LOE edx ebx ebp esi
$B1$122:                        ; Preds $B1$120
        mov       edi, DWORD PTR [esp+4]                        ;329.9
        sub       edx, eax                                      ;329.9
        sub       ebx, eax                                      ;329.9
        mov       eax, DWORD PTR [esp+12]                       ;329.9
        mov       ecx, eax                                      ;329.9
        sar       ecx, 5                                        ;329.9
        sub       eax, ecx                                      ;329.9
        mov       ecx, DWORD PTR [esp+8]                        ;329.9
        mov       WORD PTR [edi+ecx+864], ax                    ;329.9
        mov       eax, DWORD PTR [esp]                          ;329.9
        lea       eax, DWORD PTR [eax+eax+1]                    ;329.9
        mov       DWORD PTR [esp], eax                          ;329.9
                                ; LOE edx ebx ebp esi
$B1$123:                        ; Preds $B1$121 $B1$122
        mov       eax, DWORD PTR [esp+16]                       ;329.9
        test      eax, eax                                      ;329.9
        jne       $B1$116       ; Prob 90%                      ;329.9
                                ; LOE edx ebx ebp esi
$B1$124:                        ; Preds $B1$123
        mov       eax, DWORD PTR [esp]                          ;329.9
        add       eax, -64                                      ;329.9
        mov       DWORD PTR [esp], eax                          ;329.9
        cmp       eax, 4                                        ;330.9
        jge       $B1$126       ; Prob 50%                      ;330.9
                                ; LOE eax edx ebx ebp esi al ah
$B1$125:                        ; Preds $B1$124
        mov       DWORD PTR [esp+8], eax                        ;329.9
        jmp       $B1$147       ; Prob 100%                     ;329.9
        ALIGN     4
                                ; LOE edx ebx ebp esi
$B1$126:                        ; Preds $B1$124
        mov       ecx, eax                                      ;332.44
        sar       ecx, 1                                        ;332.44
        lea       edi, DWORD PTR [ecx-1]                        ;332.49
        mov       DWORD PTR [esp+16], edi                       ;332.49
        mov       edi, eax                                      ;333.42
        and       edi, 1                                        ;333.42
        or        edi, 2                                        ;333.42
        mov       DWORD PTR [esp+8], edi                        ;333.42
        cmp       eax, 14                                       ;334.11
        jge       $B1$128       ; Prob 90%                      ;334.11
                                ; LOE edx ecx ebx ebp esi edi
$B1$127:                        ; Preds $B1$126
        mov       ecx, DWORD PTR [esp+16]                       ;336.13
        mov       eax, edi                                      ;336.13
        mov       edi, DWORD PTR [esp+88]                       ;337.20
        shl       eax, cl                                       ;336.13
        mov       ecx, DWORD PTR [esp]                          ;337.41
        mov       DWORD PTR [esp+8], eax                        ;336.13
        lea       edi, DWORD PTR [edi-2]                        ;337.20
        add       ecx, ecx                                      ;337.41
        sub       edi, ecx                                      ;337.20
        lea       eax, DWORD PTR [edi+eax*2+1376]               ;337.51
        mov       DWORD PTR [esp+36], eax                       ;337.51
        jmp       $B1$137       ; Prob 100%                     ;337.51
        ALIGN     4
                                ; LOE edx ebx ebp esi
$B1$128:                        ; Preds $B1$126
        add       ecx, -5                                       ;341.13
        
                                ; LOE edx ecx ebx ebp esi
$B1$129:                        ; Preds $B1$135 $B1$128
        dec       ecx                                           ;344.15
        cmp       edx, 16777216                                 ;344.15
        jae       $B1$133       ; Prob 62%                      ;344.15
                                ; LOE edx ecx ebx ebp esi
$B1$130:                        ; Preds $B1$129
        mov       eax, DWORD PTR [esp+24]                       ;344.15
        cmp       ebp, eax                                      ;344.15
        jne       $B1$132       ; Prob 99%                      ;344.15
                                ; LOE edx ecx ebx ebp esi
$B1$131:                        ; Preds $B1$130
        jmp       $B1$5         ; Prob 0%                       ;344.15
                                ; LOE
$B1$132:                        ; Preds $B1$130
        shl       edx, 8                                        ;344.15
        mov       eax, ebp                                      ;344.15
        movzx     edi, BYTE PTR [eax]                           ;344.15
        inc       ebp                                           ;344.15
        shl       ebx, 8                                        ;344.15
        or        ebx, edi                                      ;344.15
                                ; LOE edx ecx ebx ebp esi
$B1$133:                        ; Preds $B1$132 $B1$129
        mov       eax, DWORD PTR [esp+8]                        ;346.15
        shr       edx, 1                                        ;345.15
        add       eax, eax                                      ;346.15
        mov       DWORD PTR [esp+8], eax                        ;346.15
        cmp       ebx, edx                                      ;347.15
        jb        $B1$135       ; Prob 38%                      ;347.15
                                ; LOE edx ecx ebx ebp esi
$B1$134:                        ; Preds $B1$133
        or        DWORD PTR [esp+8], 1                          ;350.17
        sub       ebx, edx                                      ;349.17
                                ; LOE edx ecx ebx ebp esi
$B1$135:                        ; Preds $B1$134 $B1$133
        test      ecx, ecx                                      ;353.39
        jne       $B1$129       ; Prob 90%                      ;353.39
                                ; LOE edx ecx ebx ebp esi
$B1$136:                        ; Preds $B1$135
        mov       eax, DWORD PTR [esp+88]                       ;354.20
        mov       edi, DWORD PTR [esp+8]                        ;355.13
        lea       ecx, DWORD PTR [eax+1604]                     ;354.20
        mov       DWORD PTR [esp+36], ecx                       ;354.20
        shl       edi, 4                                        ;355.13
        mov       DWORD PTR [esp+8], edi                        ;355.13
        push      4                                             ;356.13
        pop       eax                                           ;356.13
        mov       DWORD PTR [esp+16], eax                       ;356.13
                                ; LOE edx ebx ebp esi
$B1$137:                        ; Preds $B1$127 $B1$136
        push      1                                             ;
        pop       eax                                           ;
        mov       DWORD PTR [esp], eax                          ;
        push      1                                             ;
        pop       ecx                                           ;
        mov       DWORD PTR [esp+4], ecx                        ;
        
                                ; LOE edx ebx ebp esi
$B1$138:                        ; Preds $B1$145 $B1$137
        mov       edi, DWORD PTR [esp+4]                        ;363.30
        mov       ecx, DWORD PTR [esp+36]                       ;363.30
        cmp       edx, 16777216                                 ;364.15
        lea       eax, DWORD PTR [edi+edi]                      ;363.30
        mov       DWORD PTR [esp+32], eax                       ;363.30
        lea       eax, DWORD PTR [ecx+edi*2]                    ;363.30
        jae       $B1$142       ; Prob 50%                      ;364.15
                                ; LOE eax edx ebx ebp esi
$B1$139:                        ; Preds $B1$138
        mov       ecx, DWORD PTR [esp+24]                       ;364.15
        cmp       ebp, ecx                                      ;364.15
        jne       $B1$141       ; Prob 99%                      ;364.15
                                ; LOE eax edx ebx ebp esi
$B1$140:                        ; Preds $B1$139
        jmp       $B1$5         ; Prob 0%                       ;364.15
                                ; LOE
$B1$141:                        ; Preds $B1$139
        shl       edx, 8                                        ;364.15
        mov       ecx, ebp                                      ;363.30
        movzx     edi, BYTE PTR [ecx]                           ;364.15
        inc       ebp                                           ;364.15
        shl       ebx, 8                                        ;364.15
        or        ebx, edi                                      ;364.15
                                ; LOE eax edx ebx ebp esi
$B1$142:                        ; Preds $B1$141 $B1$138
        movzx     ecx, WORD PTR [eax]                           ;364.15
        mov       DWORD PTR [esp+12], ecx                       ;364.15
        mov       edi, edx                                      ;364.15
        shr       edi, 11                                       ;364.15
        imul      edi, ecx                                      ;364.15
        cmp       ebx, edi                                      ;364.15
        jae       $B1$144       ; Prob 62%                      ;364.15
                                ; LOE eax edx ecx ebx ebp esi edi cl ch
$B1$143:                        ; Preds $B1$142
        mov       edx, edi                                      ;364.15
        mov       edi, ecx                                      ;364.15
        mov       ecx, edi                                      ;364.15
        neg       ecx                                           ;364.15
        add       ecx, 2048                                     ;364.15
        sar       ecx, 5                                        ;364.15
        add       edi, ecx                                      ;364.15
        mov       WORD PTR [eax], di                            ;364.15
        mov       eax, DWORD PTR [esp+32]                       ;363.30
        mov       DWORD PTR [esp+4], eax                        ;363.30
        jmp       $B1$145       ; Prob 100%                     ;363.30
        ALIGN     4
                                ; LOE edx ebx ebp esi
$B1$144:                        ; Preds $B1$142
        sub       edx, edi                                      ;364.15
        sub       ebx, edi                                      ;364.15
        mov       edi, ecx                                      ;364.15
        mov       ecx, edi                                      ;364.15
        sar       ecx, 5                                        ;364.15
        sub       edi, ecx                                      ;364.15
        mov       WORD PTR [eax], di                            ;364.15
        mov       eax, DWORD PTR [esp+4]                        ;364.15
        lea       eax, DWORD PTR [eax+eax+1]                    ;364.15
        mov       DWORD PTR [esp+4], eax                        ;364.15
        mov       eax, DWORD PTR [esp]                          ;364.15
        or        DWORD PTR [esp+8], eax                        ;364.15
                                ; LOE edx ebx ebp esi
$B1$145:                        ; Preds $B1$143 $B1$144
        mov       eax, DWORD PTR [esp]                          ;365.15
        mov       ecx, DWORD PTR [esp+16]                       ;367.21
        add       eax, eax                                      ;365.15
        mov       DWORD PTR [esp], eax                          ;365.15
        dec       ecx                                           ;367.21
        mov       DWORD PTR [esp+16], ecx                       ;367.21
        jne       $B1$138       ; Prob 90%                      ;367.38
                                ; LOE edx ebx ebp esi
$B1$147:                        ; Preds $B1$145 $B1$125
        mov       eax, DWORD PTR [esp+8]                        ;372.15
        inc       eax                                           ;372.15
        mov       DWORD PTR [esp+8], eax                        ;372.15
        je        $B1$160       ; Prob 10%                      ;372.9
                                ; LOE edx ebx ebp esi
$B1$149:                        ; Preds $B1$147 $B1$111
        mov       eax, DWORD PTR [esp+8]                        ;381.7
        mov       ecx, DWORD PTR [esp+28]                       ;381.7
        cmp       eax, ecx                                      ;381.7
        jbe       $B1$151       ; Prob 96%                      ;381.7
                                ; LOE edx ecx ebx ebp esi cl ch
$B1$150:                        ; Preds $B1$149
        jmp       $B1$5         ; Prob 0%                       ;382.16
                                ; LOE
$B1$151:                        ; Preds $B1$149
        mov       eax, DWORD PTR [esp+80]                       ;386.24
        mov       edi, ecx                                      ;386.24
        lea       ecx, DWORD PTR [edi+eax]                      ;386.24
        mov       eax, ecx                                      ;386.43
        inc       edi                                           ;387.19
        sub       eax, DWORD PTR [esp+8]                        ;386.43
        movzx     eax, BYTE PTR [eax]                           ;386.24
        mov       DWORD PTR [esp+12], eax                       ;386.24
        mov       BYTE PTR [ecx], al                            ;387.9
        mov       DWORD PTR [esp+28], edi                       ;387.19
        inc       esi                                           ;388.9
        je        $B1$156       ; Prob 2%                       ;390.20
                                ; LOE edx ebx ebp esi
$B1$152:                        ; Preds $B1$151
        mov       eax, DWORD PTR [esp+80]                       ;386.24
        sub       eax, DWORD PTR [esp+8]                        ;386.24
        mov       DWORD PTR [esp+4], eax                        ;386.24
        
                                ; LOE edx ebx ebp esi
$B1$153:                        ; Preds $B1$154 $B1$152
        mov       ecx, DWORD PTR [esp+84]                       ;390.20
        mov       eax, DWORD PTR [esp+28]                       ;390.20
        cmp       eax, ecx                                      ;390.20
        jae       $B1$160       ; Prob 7%                       ;390.20
                                ; LOE eax edx ebx ebp esi al ah
$B1$154:                        ; Preds $B1$153
        mov       ecx, eax                                      ;386.24
        mov       edi, DWORD PTR [esp+4]                        ;386.24
        movzx     eax, BYTE PTR [edi+ecx]                       ;386.24
        mov       edi, DWORD PTR [esp+80]                       ;387.9
        mov       DWORD PTR [esp+12], eax                       ;386.24
        mov       BYTE PTR [edi+ecx], al                        ;387.9
        inc       ecx                                           ;387.19
        mov       DWORD PTR [esp+28], ecx                       ;387.19
        dec       esi                                           ;388.9
        jne       $B1$153       ; Prob 90%                      ;390.20
                                ; LOE edx ebx ebp esi
$B1$156:                        ; Preds $B1$154 $B1$151 $B1$40 $B1$42 $B1$43
                                ;       $B1$68
        mov       ecx, DWORD PTR [esp+84]                       ;177.3
        mov       eax, DWORD PTR [esp+28]                       ;177.3
        cmp       eax, ecx                                      ;177.3
        jb        $B1$9         ; Prob 90%                      ;177.3
        jmp       $B1$160       ; Prob 100%                     ;177.3
        ALIGN     4
                                ; LOE edx ebx ebp
$B1$159:                        ; Preds $B1$7
        push      -1                                            ;
        pop       edx                                           ;
                                ; LOE edx ebp
$B1$160:                        ; Preds $B1$153 $B1$156 $B1$147 $B1$159
        cmp       edx, 16777216                                 ;393.3
        jae       $B1$163       ; Prob 28%                      ;393.3
                                ; LOE ebp
$B1$161:                        ; Preds $B1$160
        mov       eax, DWORD PTR [esp+24]                       ;393.3
        cmp       ebp, eax                                      ;393.3
        jne       $B1$163       ; Prob 97%                      ;393.3
                                ; LOE
$B1$162:                        ; Preds $B1$161
        jmp       $B1$5         ; Prob 0%                       ;393.3
                                ; LOE
$B1$163:                        ; Preds $B1$161 $B1$160
        xor       eax, eax                                      ;395.10
        jmp       $L1           ; Prob 100%                     ;395.10
                                ; LOE
lzma_decode_asm endp

;; ----------------------------------------------------------------------------

comment $ ;; original source

#define LZMAASM_WORKMEM_SIZE      0x4000    /* aligned 0x3e6c */

#define LZMAASM_RESULT_OK         0
#define LZMAASM_RESULT_DATA_ERROR 1

int LzmaDecodeAsm(const unsigned char* inStream, unsigned int inSize,
                  unsigned char* outStream, unsigned int outSize,
                  void* WorkMem);


/* ------------------------------------------------------------------------- */

#define LzmaGetNumProbs(Properties) (LZMA_BASE_SIZE + (LZMA_LIT_SIZE << ((Properties)->lc + (Properties)->lp)))

typedef unsigned char  Byte;
typedef unsigned short UInt16;
typedef unsigned int   UInt32;
typedef unsigned int   SizeT;

#define LZMA_BASE_SIZE  1846
#define LZMA_LIT_SIZE   768

#define LZMA_PROPERTIES_SIZE 5

#define CProb UInt16

#define kNumTopBits 24
#define kTopValue ((UInt32)1 << kNumTopBits)

#define kNumBitModelTotalBits 11
#define kBitModelTotal (1 << kNumBitModelTotalBits)
#define kNumMoveBits 5

#define RC_READ_BYTE (*Buffer++)

#define RC_INIT2 Code = 0; Range = 0xFFFFFFFF; \
  { int i; for(i = 0; i < 5; i++) { RC_TEST; Code = (Code << 8) | RC_READ_BYTE; }}

#ifdef _LZMA_IN_CB

#define RC_TEST { if (Buffer == BufferLim) \
  { SizeT size; int result = InCallback->Read(InCallback, &Buffer, &size); if (result != LZMA_RESULT_OK) return result; \
  BufferLim = Buffer + size; if (size == 0) return LZMAASM_RESULT_DATA_ERROR; }}

#define RC_INIT Buffer = BufferLim = 0; RC_INIT2

#else

#define RC_TEST { if (Buffer == BufferLim) return LZMAASM_RESULT_DATA_ERROR; }

#define RC_INIT(buffer, bufferSize) Buffer = buffer; BufferLim = buffer + bufferSize; RC_INIT2
 
#endif

#define RC_NORMALIZE if (Range < kTopValue) { RC_TEST; Range <<= 8; Code = (Code << 8) | RC_READ_BYTE; }

#define IfBit0(p) RC_NORMALIZE; bound = (Range >> kNumBitModelTotalBits) * *(p); if (Code < bound)
#define UpdateBit0(p) Range = bound; *(p) += (kBitModelTotal - *(p)) >> kNumMoveBits;
#define UpdateBit1(p) Range -= bound; Code -= bound; *(p) -= (*(p)) >> kNumMoveBits;

#define RC_GET_BIT2(p, mi, A0, A1) IfBit0(p) \
  { UpdateBit0(p); mi <<= 1; A0; } else \
  { UpdateBit1(p); mi = (mi + mi) + 1; A1; } 
  
#define RC_GET_BIT(p, mi) RC_GET_BIT2(p, mi, ; , ;)               

#define RangeDecoderBitTreeDecode(probs, numLevels, res) \
  { int i = numLevels; res = 1; \
  do { CProb *p = probs + res; RC_GET_BIT(p, res) } while(--i != 0); \
  res -= (1 << numLevels); }


#define kNumPosBitsMax 4
#define kNumPosStatesMax (1 << kNumPosBitsMax)

#define kLenNumLowBits 3
#define kLenNumLowSymbols (1 << kLenNumLowBits)
#define kLenNumMidBits 3
#define kLenNumMidSymbols (1 << kLenNumMidBits)
#define kLenNumHighBits 8
#define kLenNumHighSymbols (1 << kLenNumHighBits)

#define LenChoice 0
#define LenChoice2 (LenChoice + 1)
#define LenLow (LenChoice2 + 1)
#define LenMid (LenLow + (kNumPosStatesMax << kLenNumLowBits))
#define LenHigh (LenMid + (kNumPosStatesMax << kLenNumMidBits))
#define kNumLenProbs (LenHigh + kLenNumHighSymbols) 


#define kNumStates 12
#define kNumLitStates 7

#define kStartPosModelIndex 4
#define kEndPosModelIndex 14
#define kNumFullDistances (1 << (kEndPosModelIndex >> 1))

#define kNumPosSlotBits 6
#define kNumLenToPosStates 4

#define kNumAlignBits 4
#define kAlignTableSize (1 << kNumAlignBits)

#define kMatchMinLen 2

#define IsMatch 0
#define IsRep (IsMatch + (kNumStates << kNumPosBitsMax))
#define IsRepG0 (IsRep + kNumStates)
#define IsRepG1 (IsRepG0 + kNumStates)
#define IsRepG2 (IsRepG1 + kNumStates)
#define IsRep0Long (IsRepG2 + kNumStates)
#define PosSlot (IsRep0Long + (kNumStates << kNumPosBitsMax))
#define SpecPos (PosSlot + (kNumLenToPosStates << kNumPosSlotBits))
#define Align (SpecPos + kNumFullDistances - kEndPosModelIndex)
#define LenCoder (Align + kAlignTableSize)
#define RepLenCoder (LenCoder + kNumLenProbs)
#define Literal (RepLenCoder + kNumLenProbs)

#if Literal != LZMA_BASE_SIZE
StopCompilingDueBUG
#endif

#define kLzmaStreamWasFinishedId (-1)

#define LZMA_CONF_STATEBITS       (2)       /* 0 .. 4 */
#define LZMA_CONF_CONTEXTBITS     (3)       /* 0 .. 8 */
#define LZMA_CONF_POSITIONBITS    (0)       /* 0 .. 4 */

/* ------------------------------------------------------------------------- */

int LzmaDecodeAsm(const unsigned char* inStream, unsigned int inSize,
                  unsigned char* outStream, unsigned int outSize,
                  void* WorkMem)
{
  const int lc = LZMA_CONF_CONTEXTBITS;
  const int lp = LZMA_CONF_POSITIONBITS;
  const int pb = LZMA_CONF_STATEBITS;

#ifdef _DEBUG
  const int WorkMemSize = sizeof(CProb) * (LZMA_BASE_SIZE + (LZMA_LIT_SIZE << (lc + lp)));
#endif

  SizeT  nowPos = 0;
  Byte   previousByte = 0;
  UInt32 posStateMask   = (1 << pb) - 1;
  UInt32 literalPosMask = (1 << lp) - 1;
  CProb* p = (CProb*)(WorkMem);

  int state = 0;
  UInt32 rep0 = 1, rep1 = 1, rep2 = 1, rep3 = 1;
  int len = 0;
  const Byte *Buffer;
  const Byte *BufferLim;
  UInt32 Range;
  UInt32 Code;

  {
    UInt32 i;
    UInt32 numProbs = Literal + ((UInt32)LZMA_LIT_SIZE << (lc + lp));
    for (i = 0; i < numProbs; i++)
      p[i] = kBitModelTotal >> 1;
  }
  
  RC_INIT(inStream, inSize);

  while(nowPos < outSize)
  {
    CProb *prob;
    UInt32 bound;
    int posState = (int)(nowPos & posStateMask);

    prob = p + IsMatch + (state << kNumPosBitsMax) + posState;
    IfBit0(prob)
    {
      int symbol = 1;
      UpdateBit0(prob)
      prob = p + Literal + (LZMA_LIT_SIZE * 
        (((nowPos & literalPosMask) << lc) + (previousByte >> (8 - lc))));

      if (state >= kNumLitStates)
      {
        int matchByte;
        matchByte = outStream[nowPos - rep0];
        do
        {
          int bit;
          CProb *probLit;
          matchByte <<= 1;
          bit = (matchByte & 0x100);
          probLit = prob + 0x100 + bit + symbol;
          RC_GET_BIT2(probLit, symbol, if (bit != 0) break, if (bit == 0) break)
        }
        while (symbol < 0x100);
      }
      while (symbol < 0x100)
      {
        CProb *probLit = prob + symbol;
        RC_GET_BIT(probLit, symbol)
      }
      previousByte = (Byte)symbol;

      outStream[nowPos++] = previousByte;
      if (state < 4) state = 0;
      else if (state < 10) state -= 3;
      else state -= 6;
    }
    else             
    {
      UpdateBit1(prob);
      prob = p + IsRep + state;
      IfBit0(prob)
      {
        UpdateBit0(prob);
        rep3 = rep2;
        rep2 = rep1;
        rep1 = rep0;
        state = state < kNumLitStates ? 0 : 3;
        prob = p + LenCoder;
      }
      else
      {
        UpdateBit1(prob);
        prob = p + IsRepG0 + state;
        IfBit0(prob)
        {
          UpdateBit0(prob);
          prob = p + IsRep0Long + (state << kNumPosBitsMax) + posState;
          IfBit0(prob)
          {
            UpdateBit0(prob);
            
            if (nowPos == 0)
              return LZMAASM_RESULT_DATA_ERROR;
            
            state = state < kNumLitStates ? 9 : 11;
            previousByte = outStream[nowPos - rep0];
            outStream[nowPos++] = previousByte;
            continue;
          }
          else
          {
            UpdateBit1(prob);
          }
        }
        else
        {
          UInt32 distance;
          UpdateBit1(prob);
          prob = p + IsRepG1 + state;
          IfBit0(prob)
          {
            UpdateBit0(prob);
            distance = rep1;
          }
          else 
          {
            UpdateBit1(prob);
            prob = p + IsRepG2 + state;
            IfBit0(prob)
            {
              UpdateBit0(prob);
              distance = rep2;
            }
            else
            {
              UpdateBit1(prob);
              distance = rep3;
              rep3 = rep2;
            }
            rep2 = rep1;
          }
          rep1 = rep0;
          rep0 = distance;
        }
        state = state < kNumLitStates ? 8 : 11;
        prob = p + RepLenCoder;
      }
      {
        int numBits, offset;
        CProb *probLen = prob + LenChoice;
        IfBit0(probLen)
        {
          UpdateBit0(probLen);
          probLen = prob + LenLow + (posState << kLenNumLowBits);
          offset = 0;
          numBits = kLenNumLowBits;
        }
        else
        {
          UpdateBit1(probLen);
          probLen = prob + LenChoice2;
          IfBit0(probLen)
          {
            UpdateBit0(probLen);
            probLen = prob + LenMid + (posState << kLenNumMidBits);
            offset = kLenNumLowSymbols;
            numBits = kLenNumMidBits;
          }
          else
          {
            UpdateBit1(probLen);
            probLen = prob + LenHigh;
            offset = kLenNumLowSymbols + kLenNumMidSymbols;
            numBits = kLenNumHighBits;
          }
        }
        RangeDecoderBitTreeDecode(probLen, numBits, len);
        len += offset;
      }

      if (state < 4)
      {
        int posSlot;
        state += kNumLitStates;
        prob = p + PosSlot +
            ((len < kNumLenToPosStates ? len : kNumLenToPosStates - 1) << 
            kNumPosSlotBits);
        RangeDecoderBitTreeDecode(prob, kNumPosSlotBits, posSlot);
        if (posSlot >= kStartPosModelIndex)
        {
          int numDirectBits = ((posSlot >> 1) - 1);
          rep0 = (2 | ((UInt32)posSlot & 1));
          if (posSlot < kEndPosModelIndex)
          {
            rep0 <<= numDirectBits;
            prob = p + SpecPos + rep0 - posSlot - 1;
          }
          else
          {
            numDirectBits -= kNumAlignBits;
            do
            {
              RC_NORMALIZE
              Range >>= 1;
              rep0 <<= 1;
              if (Code >= Range)
              {
                Code -= Range;
                rep0 |= 1;
              }
            }
            while (--numDirectBits != 0);
            prob = p + Align;
            rep0 <<= kNumAlignBits;
            numDirectBits = kNumAlignBits;
          }
          {
            int i = 1;
            int mi = 1;
            do
            {
              CProb *prob3 = prob + mi;
              RC_GET_BIT2(prob3, mi, ; , rep0 |= i);
              i <<= 1;
            }
            while(--numDirectBits != 0);
          }
        }
        else
          rep0 = posSlot;
        if (++rep0 == (UInt32)(0))
        {
          /* it's for stream version */
          len = kLzmaStreamWasFinishedId;
          break;
        }
      }

      len += kMatchMinLen;
      if (rep0 > nowPos)
        return LZMAASM_RESULT_DATA_ERROR;

      do
      {
        previousByte = outStream[nowPos - rep0];
        outStream[nowPos++] = previousByte;
        len--;
      }
      while(len != 0 && nowPos < outSize);
    }
  }
  RC_NORMALIZE;

  return LZMAASM_RESULT_OK;
}

$

;; ----------------------------------------------------------------------------

