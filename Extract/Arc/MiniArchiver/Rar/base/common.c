#include <Prague/prague.h>
#include <Prague/iface/i_seqio.h>

#include "const.h"
#include "proto.h"

// ----------------------------------------------------------------------------

tINT Rotate(tRAR* RarData)
{
  if ( errOPERATION_CANCELED == CALL_SYS_SendMsg(RarData->hSeqIn,
    pmc_PROCESSING, pm_PROCESSING_YIELD, RarData->hThis, 0 ,0) )
  {
    RarData->LastError = (errOPERATION_CANCELED);
    return(1);
  }

  return(0);
}

// ----------------------------------------------------------------------------

tUINT PackRead(tRAR* RAR, tBYTE* Buff, tDWORD Cnt)
{
  tERROR Error;
  tUINT  Result;

  Result = 0;
  if ( PR_FAIL(Error = CALL_SeqIO_Read(RAR->hSeqIn,&Result,Buff,Cnt)) &&
      ((Error != errOUT_OF_OBJECT) || (!Result && Cnt)) )
  {
    RAR->LastError = (errOUT_OF_OBJECT);
    return(0);
  }

  return(tUINT)(Result);
}

// ----------------------------------------------------------------------------

tQWORD PackSeek(tRAR* RarData, tQWORD Offset, tDWORD Origin)
{
  tERROR Error;
  tQWORD Result;

  Result = 0;
  Error = CALL_SeqIO_Seek(RarData->hSeqIn,&Result,Offset,Origin);
  if ( PR_FAIL(Error) )
  {
    RarData->LastError = Error;
  }

  return(Result);
}

// ----------------------------------------------------------------------------

#ifdef DEBUG_MEMALLOC
# include <stdio.h>
  static unsigned int mem_cnt = 0;
#endif

tVOID* Malloc(tRAR* RarData, tUINT Cnt)
{
  tVOID* Result = (NULL);

#ifdef DEBUG_MEMALLOC
  printf(" ===> Malloc(), Num: %u, Size: %u [0x%X]\n", mem_cnt++, Cnt, Cnt);
#endif

	if ( errOK != CALL_Heap_Alloc(RarData->hHeap, &Result, Cnt) )
  {
    RarData->LastError = (errNOT_ENOUGH_MEMORY);
    return(NULL);
  }

  return(Result);
}

// ----------------------------------------------------------------------------

tVOID Free(tRAR* RarData, tVOID* Mem)
{
  if ( NULL == Mem )
    return;

#ifdef DEBUG_MEMALLOC
  printf(" ===> Free(), Num: %u\n", --mem_cnt);
#endif

  RarData->LastError = CALL_Heap_Free(RarData->hHeap, Mem);
}

// ----------------------------------------------------------------------------

tUINT  FileWrite(tRAR* RarData, tBYTE* Mem, tDWORD Cnt)
{
  tUINT Result;
  tERROR Error;

  Result = 0;
  if ( PR_FAIL(Error = CALL_SeqIO_Write(RarData->hSeqOut, &Result, Mem, Cnt)) )
  {
    RarData->LastError = Error;
    return(0);
  }

  return(Result);
}

// ----------------------------------------------------------------------------

tUINT Pack20Read(tPACK20* Data, tBYTE* Addr, tUINT Count)
{
  tERROR Error;
  tUINT  Result;
 
  Result = 0;
  if ( Data->MaxPackRead < Count )
    Count = Data->MaxPackRead;
  if ( PR_FAIL(Error = CALL_SeqIO_Read(Data->hSeqInp,&Result,Addr,Count)) &&
     ((Error != errOUT_OF_OBJECT) || (!Result && Count)) )
  {
    return(0);
  }

  Data->MaxPackRead -= Result;
  Data->CurPackRead += Result;;
  Data->PackFileCRC  = CRC32(Data->PackFileCRC, Addr, Result);

  return(Result);
}

// ----------------------------------------------------------------------------

tUINT Pack20Write(tPACK20* Data, tBYTE* Addr, tUINT Count)
{
  tERROR Error;
  tUINT  Result;

  CRC32(Data->PackFileCRC, Addr, Count);
  Data->PackSize += Count;
  if ( PR_FAIL(Error = CALL_SeqIO_Write(Data->hSeqOut,&Result,Addr,Count)) )
    return(0);

  return(Result);
}

// ----------------------------------------------------------------------------

#if defined (_WIN32)

#if !defined _DEBUG // for stack growing
void __declspec(naked) _chkstk(void)
{
  _asm
  {
                push    ecx
                cmp     eax, 1000h
                lea     ecx, [esp+8]
                jb      short stk_page_done
stk_page_do:
                sub     ecx, 1000h
                sub     eax, 1000h
                test    [ecx], eax
                cmp     eax, 1000h
                jnb     short stk_page_do
stk_page_done:
                sub     ecx, eax
                mov     eax, esp
                test    [ecx], eax
                mov     esp, ecx
                mov     ecx, [eax]
                mov     eax, [eax+4]
                push    eax
                retn
  }
}
#endif
#endif
// ----------------------------------------------------------------------------



















