#include "StdAfx.h"

#if defined (__unix__)

#include <string.h>
#include <DupMem/DupMem.h>

DWORD CustomDWORDs [ 10 ];

BOOL 	DupInit (BOOL multithread)	
{	
  memset ( CustomDWORDs, 0, sizeof ( CustomDWORDs ) ); return 1;	
}

BOOL	DupEnter(BOOL* exclusive)	
{	
  return 1;	
}

void    DupDestroy()                    
{}

BOOL	DupExit(BOOL* exclusive)	
{
  return 1;
}

void*   DupAlloc(unsigned size, BOOL static_, BOOL* allocated_by_new) 
{	
  if ( allocated_by_new ) 
    *allocated_by_new = 1; 
  return new char[size];	
}

void	DupFree (void* ptr)						
{	
  char* l_ptr = reinterpret_cast<char*>(ptr);
  delete [] l_ptr;				
}

void    DupRegisterThread(void* pdata)                 
{}

BOOL    DupGetDebugData( DWORD* MtDebug, DWORD* count) 
{
  return 0; 
}

DWORD   DupSetCustomDWord(unsigned index, DWORD data)  
{ 
  CustomDWORDs [ index ] = data; 
  return data; 
}

DWORD   DupGetCustomDWord(unsigned index)              
{ 
  return  CustomDWORDs [ index ]; 
}

#endif // __unix__
