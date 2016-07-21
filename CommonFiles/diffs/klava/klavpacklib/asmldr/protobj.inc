
align 4
protect_sections_asm proc near

var_4           = byte ptr -4
arg_0           = dword ptr  8
arg_4           = dword ptr  0Ch
arg_8           = dword ptr  10h

                push    ebp
                mov     ebp, esp
                push    ecx
                mov     eax, [ebp+arg_4]
                movzx   ecx, word ptr [eax+14h]
                and     [ebp+arg_4], 0
                push    ebx
                movzx   ebx, word ptr [eax+6]
                test    ebx, ebx
                push    esi
                push    edi
                lea     ecx, [ecx+eax+18h]
                jbe     short @@loc_72
                lea     esi, [ecx+8]
                mov     edi, 0A0000000h
@@loc_26:
                mov     eax, [esi+1Ch]
                and     eax, edi
                cmp     eax, 20000000h
                jz      short @@loc_4F
                cmp     eax, 80000000h
                jz      short @@loc_4B
                xor     ecx, ecx
                cmp     eax, edi
                setz    cl
                dec     ecx
                and     ecx, 0FFFFFFC2h
                add     ecx, 40h
                mov     eax, ecx
                jmp     short @@loc_52
@@loc_4B:
                push    4
                jmp     short @@loc_51
@@loc_4F:
                push    20h
@@loc_51:
                pop     eax
@@loc_52:
                lea     ecx, [ebp+var_4]
                push    ecx
                push    eax
                mov     eax, [esi+4]
                push    dword ptr [esi]
                add     eax, [ebp+arg_0]
                push    eax
                call    [ebp+arg_8]
                test    eax, eax
                jz      short @@loc_79
                add     esi, 28h
                inc     [ebp+arg_4]
                cmp     [ebp+arg_4], ebx
                jb      short @@loc_26
@@loc_72:
                xor     eax, eax
@@loc_74:
                pop     edi
                pop     esi
                pop     ebx
                leave
                retn
@@loc_79:
                xor     eax, eax
                inc     eax
                jmp     short @@loc_74
protect_sections_asm endp

;; ----------------------------------------------------------------------------

comment $ ;; original source
typedef int (__stdcall *fVirtualProtect)(void* Addr, unsigned long Size, unsigned long NewProt, unsigned long* OldProt);

int ProtectSectionsAsm(unsigned char* hInstance, IMAGE_NT_HEADERS* PE, fVirtualProtect VP)
{
  IMAGE_SECTION_HEADER* Obj;
  unsigned int ObjNum, I;

  Obj = (IMAGE_SECTION_HEADER*)((unsigned char*)PE + sizeof(PE->Signature) +
    sizeof(PE->FileHeader) + PE->FileHeader.SizeOfOptionalHeader);
  ObjNum = PE->FileHeader.NumberOfSections;

  for ( I = 0; I < ObjNum; Obj++, I++ )
  {
    unsigned long ObjProt;
    unsigned long MemProt;
    unsigned long OldProt;

    ObjProt = Obj->Characteristics;
    switch( ObjProt & (IMAGE_SCN_MEM_WRITE | IMAGE_SCN_MEM_EXECUTE) )
    {
    case (IMAGE_SCN_MEM_WRITE | IMAGE_SCN_MEM_EXECUTE):
      MemProt = PAGE_EXECUTE_READWRITE;
      break;
    case IMAGE_SCN_MEM_EXECUTE:
      MemProt = PAGE_EXECUTE_READ;
      break;
    case IMAGE_SCN_MEM_WRITE:
      MemProt = PAGE_READWRITE;
      break;
    default:
      MemProt = PAGE_READONLY;
    }

    if ( !VP(hInstance + Obj->VirtualAddress, Obj->Misc.VirtualSize, MemProt, &OldProt) )
      return(1); /* fail */
  }

  /* done */
  return(0);
}
$

;; ----------------------------------------------------------------------------
