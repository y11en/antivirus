#ifndef _SIGNATURE_DETACH_H__
#define _SIGNATURE_DETACH_H__

bool
__stdcall
CheckForSignOld( char* BasePtr, unsigned long TotalSize, unsigned long* pSignlessSize );

bool
__stdcall
CheckForSignNew( char* BasePtr, unsigned long TotalSize, unsigned long* pSignlessSize );

long
__stdcall
GetCryptoCPtrs( 
               char*            BasePtr, 
               unsigned long    TotalSize, 
               
               char**         pKey1,
               unsigned long* pKey1Size,

               char**         pKey2,
               unsigned long* pKey2Size,

               char**         pReg,
               unsigned long* pRegSize,
               
               unsigned long* pFileSize );

#endif