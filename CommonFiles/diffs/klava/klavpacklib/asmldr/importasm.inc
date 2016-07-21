align 4
resolve_import_asm proc near
; parameter 1: 20 + esp
; parameter 2: 24 + esp
; parameter 3: 28 + esp
; parameter 4: 32 + esp
$B1$1:                          ; Preds $B1$0
        push      edi                                           ;15.1
        push      esi                                           ;15.1
        push      ebp                                           ;15.1
        push      ebx                                           ;15.1
        mov       eax, DWORD PTR [esp+20]                       ;13.5
        test      eax, eax                                      ;20.3
        mov       edx, DWORD PTR [esp+24]                       ;13.5
        je        $B1$25        ; Prob 12%                      ;20.3
                                ; LOE edx
$B1$2:                          ; Preds $B1$1
        test      edx, edx                                      ;20.3
        je        $B1$25        ; Prob 50%                      ;20.3
                                ; LOE edx
$B1$3:                          ; Preds $B1$2
        mov       eax, DWORD PTR [esp+28]                       ;20.3
        test      eax, eax                                      ;20.3
        je        $B1$25        ; Prob 12%                      ;20.3
                                ; LOE edx
$B1$4:                          ; Preds $B1$3
        mov       eax, DWORD PTR [esp+32]                       ;20.3
        test      eax, eax                                      ;20.3
        je        $B1$25        ; Prob 5%                       ;20.3
                                ; LOE edx
$B1$5:                          ; Preds $B1$4
        mov       eax, DWORD PTR [esp+20]                       ;23.39
        lea       edi, DWORD PTR [edx+eax]                      ;23.39
        mov       esi, DWORD PTR [edi+16]                       ;24.26
        test      esi, esi                                      ;24.17
        je        $B1$25        ; Prob 5%                       ;24.17
                                ; LOE esi edi
$B1$7:                          ; Preds $B1$5 $B1$23
        mov       eax, DWORD PTR [edi+12]                       ;32.45
        add       eax, DWORD PTR [esp+20]                       ;32.33
        push      eax                                           ;32.33
        mov       edx, DWORD PTR [esp+32]                       ;32.21
        call      edx                                           ;32.21
                                ; LOE eax esi edi
$B1$29:                         ; Preds $B1$7
        mov       ebp, eax                                      ;32.21
                                ; LOE ebp esi edi
$B1$8:                          ; Preds $B1$29
        test      ebp, ebp                                      ;32.5
        jne       $B1$10        ; Prob 99%                      ;32.5
                                ; LOE ebp esi edi
$B1$9:                          ; Preds $B1$8
        push      1                                             ;33.13
L1:                                                             ;
        pop       eax                                           ;33.13
L2:                                                             ;
        pop       ebx                                           ;33.13
        pop       ebp                                           ;33.13
        pop       esi                                           ;33.13
        pop       edi                                           ;33.13
        ret                                                     ;33.13
                                ; LOE
$B1$10:                         ; Preds $B1$8
        add       esi, DWORD PTR [esp+20]                       ;42.23
        mov       ebx, DWORD PTR [edi]                          ;36.25
        test      ebx, ebx                                      ;36.5
        je        $B1$12        ; Prob 50%                      ;36.5
                                ; LOE ebx ebp esi edi
$B1$11:                         ; Preds $B1$10
        add       ebx, DWORD PTR [esp+20]                       ;37.35
        jmp       $B1$13        ; Prob 100%                     ;37.35
                                ; LOE ebx ebp esi edi
$B1$12:                         ; Preds $B1$10
        mov       ebx, esi                                      ;39.7
                                ; LOE ebx ebp esi edi
$B1$13:                         ; Preds $B1$11 $B1$12
        mov       ecx, DWORD PTR [ebx]                          ;43.28
        add       ebx, 4                                        ;43.28
        test      ecx, ecx                                      ;43.5
        je        $B1$23        ; Prob 10%                      ;43.5
                                ; LOE ecx ebx ebp esi edi
$B1$15:                         ; Preds $B1$13 $B1$20
        test      ecx, -2147483648                              ;49.21
        je        $B1$17        ; Prob 50%                      ;49.7
                                ; LOE ecx ebx ebp esi edi
$B1$16:                         ; Preds $B1$15
        and       ecx, 2147483647                               ;50.37
        jmp       $B1$18        ; Prob 100%                     ;50.37
                                ; LOE ecx ebx ebp esi edi
$B1$17:                         ; Preds $B1$15
        mov       edx, DWORD PTR [esp+20]                       ;52.28
        lea       ecx, DWORD PTR [edx+ecx+2]                    ;52.28
                                ; LOE ecx ebx ebp esi edi
$B1$18:                         ; Preds $B1$16 $B1$17
        push      ecx                                           ;55.25
        push      ebp                                           ;55.25
        mov       edx, DWORD PTR [esp+40]                       ;55.25
        call      edx                                           ;55.25
                                ; LOE eax ebx ebp esi edi
$B1$19:                         ; Preds $B1$18
        test      eax, eax                                      ;55.7
        je        $B1$22        ; Prob 1%                       ;55.7
                                ; LOE eax ebx ebp esi edi
$B1$20:                         ; Preds $B1$19
        mov       DWORD PTR [esi], eax                          ;59.8
        mov       edx, ebx                                      ;49.7
        mov       ecx, DWORD PTR [edx]                          ;43.28
        add       ebx, 4                                        ;43.28
        add       esi, 4                                        ;59.8
        test      ecx, ecx                                      ;43.5
        jne       $B1$15        ; Prob 82%                      ;43.5
        jmp       $B1$23        ; Prob 100%                     ;43.5
                                ; LOE ecx ebx ebp esi edi
$B1$22:                         ; Preds $B1$19
        push      2                                             ;56.15
        jmp       L1            ; Prob 100%                     ;56.15
                                ; LOE
$B1$23:                         ; Preds $B1$20 $B1$13
        mov       esi, DWORD PTR [edi+36]                       ;24.26
        add       edi, 20                                       ;24.47
        test      esi, esi                                      ;24.17
        jne       $B1$7         ; Prob 82%                      ;24.17
                                ; LOE esi edi
$B1$25:                         ; Preds $B1$23 $B1$4 $B1$3 $B1$2 $B1$1
                                ;       $B1$5
        xor       eax, eax                                      ;64.9
        jmp       L2            ; Prob 100%                     ;64.9
                                ; LOE
; mark_end;
resolve_import_asm	endp

;; ----------------------------------------------------------------------------

comment $ ;; original source

typedef struct _IMAGE_IMPORT_DESCRIPTOR {
    unsigned long  OriginalFirstThunk;
    unsigned long  TimeDateStamp;
    unsigned long  ForwarderChain;
    unsigned long  Name;
    unsigned long  FirstThunk;
} IMAGE_IMPORT_DESCRIPTOR;

typedef void* (__stdcall *fGetModuleHandleA)(const char* ModuleName);
typedef void* (__stdcall *fGetProcAddress)(const void* hModule, const char* ProcName);

int ResolveImportAsm(unsigned char* hInstance, unsigned int ImportRVA,
                     fGetModuleHandleA GMH, fGetProcAddress GPA)
{
  IMAGE_IMPORT_DESCRIPTOR* ImpDir;
  unsigned long IatRVA;

  /* test params */
  if ( !hInstance || !ImportRVA || !GMH || !GPA )
    return(0); /* done */

  ImpDir = (IMAGE_IMPORT_DESCRIPTOR*)(hInstance + ImportRVA);
  for ( ; 0 != (IatRVA = ImpDir->FirstThunk); ImpDir++ )
  {
    unsigned long* ThunkPtr;
    unsigned long  IatVal;
    void** IatPtr;
    void*  Module;

    /* get module handle */
    if ( !(Module = GMH((char*)(hInstance + ImpDir->Name))) )
      return(1); /* error */

    /* find name thunks */
    if ( 0 != (IatVal = ImpDir->OriginalFirstThunk) )
      ThunkPtr = (unsigned long*)(hInstance + IatVal);
    else
      ThunkPtr = (unsigned long*)(hInstance + IatRVA);

    /* process thunks */
    IatPtr = (void**)(hInstance + IatRVA);
    while( 0 != (IatVal = *ThunkPtr++) )
    {
      char* FuncName;
      void* Function;

      /* getn name/ordinal */
      if ( IatVal & 0x80000000 )
        FuncName = (char*)(IatVal & 0x7fffffff);
      else
        FuncName = (char*)(hInstance + 2 + IatVal);

      /* get proc address */
      if ( !(Function = GPA(Module, FuncName)) )
        return(2); /* error */

      /* save result */
      *IatPtr++ = Function;
    }
  }

  /* done */
  return(0);
}

$

;; ----------------------------------------------------------------------------
