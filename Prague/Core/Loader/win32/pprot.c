/*-----------------02.05.99 09:31-------------------
 * Project Prague                                 *
 * Subproject loader Win32 try/except service     *
 * Author Petrovitch                              *
 * Copyright (c) Kaspersky Lab                    *
 * Purpose Prague API                             *
 *                                                *
--------------------------------------------------*/

#if defined (_WIN32)

#define WIN32_LEAN_AND_MEAN
#include <wincompat.h>
#if defined (_WIN32)
#include <excpt.h>
#endif
#include <stdio.h>
#include <setjmp.h>

#include <Prague/prague.h>

#include "ex_data.inc"

// ---
typedef struct tag_ExceptionReg_i {
  struct tag_ExceptionReg_i* prev;
  EXCEPTION_DISPOSITION (__cdecl* now)( EXCEPTION_RECORD*, void*, CONTEXT*, void* );
  tDWORD reserved;
} ExceptionReg;

tERROR pr_call TryEnter( tPTR reserved, ExceptionReg* );           // enter protected code section
tERROR pr_call TryLeave( tPTR reserved, ExceptionReg* );           // leave protected code section
tERROR pr_call TryGetExceptionInfo( tPTR reserved, ExceptionReg*, tDWORD* excpt_code, tPTR* info );  // get exception information
tERROR pr_call TryThrow( tPTR reserved, tDWORD excpt_code );          // throw an exception
tERROR pr_call TryInitialize( tVOID );                             // initialize protection
tERROR pr_call TryDeinitialize( tVOID );                           // deinitialize protection


//extern iMEM NfioMemoryTable;
tDWORD dwTIndex;
tDWORD seh_ptr = (tDWORD)_except_handler;
tPTR (*MemoryAllocFuncPtr)( tUINT size );

tPTR MemAllocFunc( tUINT size ) {
	return HeapAlloc( GetProcessHeap(), HEAP_ZERO_MEMORY, size );
}


// ---
iPROT Win32ExceptTable = {
  (tERROR (pr_call*)(tPTR,tExceptionReg*))                TryEnter,
  (tERROR (pr_call*)(tPTR,tExceptionReg*))                TryLeave,
  (tERROR (pr_call*)(tPTR,tExceptionReg*,tDWORD*,tPTR*))  TryGetExceptionInfo,
  (tERROR (pr_call*)(tPTR,tDWORD))                        TryThrow,
  (tERROR (pr_call*)( tVOID ))                            TryInitialize,
  (tERROR (pr_call*)( tVOID ))                            TryDeinitialize,
};

DWORD enc;
DWORD tmpe;

// ---
EXCEPTION_DISPOSITION _except_handler( EXCEPTION_RECORD* er, void* ef, CONTEXT* ctx, void* dc ) {

  struct ExceptionData* head = (struct ExceptionData*)TlsGetValue( dwTIndex );

  if ( head ) {
    head->happened = 1;
    head->ep.ExceptionRecord = &head->er;
    head->ep.ContextRecord = &head->ctx;
    memcpy( &head->er, er, sizeof(EXCEPTION_RECORD) );
    memcpy( &head->ctx, ctx, sizeof(CONTEXT) );
    longjmp( head->jump, head->er.ExceptionCode );
  }
  return ExceptionContinueSearch;
}



// ---
tERROR pr_call TryInitialize( tVOID ) { // initialize protection
	MemoryAllocFuncPtr = MemAllocFunc;
  return errOK;
}



// ---
tERROR pr_call TryDeinitialize( tVOID ) { // deinitialize protection
  return errOK;
}



// ---
tERROR pr_call TryThrow( tPTR reserved, tDWORD excpt_code ) { // deinitialize protection
  RaiseException( excpt_code, 0, 0, 0 );
  return errOK;
}


#pragma warning(disable:4733)

// ---
tERROR pr_call TryLeave( tPTR reserved, ExceptionReg* er ) {
  struct ExceptionData* head = (struct ExceptionData*)TlsGetValue( dwTIndex );
  if ( head ) {
    struct ExceptionData* next;
    struct ExceptionData* work = head;

    while( work && (work->reg_ptr != er) )
      work = work->next;

    if ( !work )
      return errUNEXPECTED;

    next = work->next;

    while( head && (head != next) ) {
      work = head->next;
      HeapFree( GetProcessHeap(), 0, head );
      head = work;
    }

    if ( next == 0 ) {
      __asm mov eax, FS:[0]
      __asm mov eax, [eax]
      __asm mov FS:[0], eax
    }
    TlsSetValue( dwTIndex, next );

    return errOK;
  }
  else
    return errUNEXPECTED;
}

#pragma warning(default:4733)


// ---
tERROR pr_call TryGetExceptionInfo( tPTR reserved, ExceptionReg* er, tDWORD* excpt_code, tPTR* info ) {
  struct ExceptionData* head = (struct ExceptionData*)TlsGetValue( dwTIndex );
  if ( head && head->happened ) {

    if ( excpt_code )
      *excpt_code = head->er.ExceptionCode;

    if ( info ) 
      *info = &head->ep;

    return errOK;
  }
  else {
    if ( info )
      *info = 0;
    return errEXCEPTION_NOT_FIRED;
  }
}



// ---
/*
__declspec( naked )
tERROR pr_call TryEnter( tPTR reserved, ExceptionReg* er ) {
	return errOK;
}
*/

#endif //_WIN32
