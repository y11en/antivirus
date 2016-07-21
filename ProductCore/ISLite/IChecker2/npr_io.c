#include <sys/stat.h>
#include <sys/types.h>
#if defined (__unix__)
#include <unistd.h>
#endif
#include "npr.h"
#include "npr_io.h"

tERROR CALL_IO_SeekRead(  hIO io, tDWORD* pdwResult, tQWORD qwOffset, void* pBuffer, tDWORD dwSize )
{
  tERROR error = errUNEXPECTED;
  int result = 0;
  if ( lseek ( io->handle, (long)qwOffset, SEEK_SET) != -1 ) {
    result = read (io->handle,pBuffer,dwSize);
    if (pdwResult) 
      *(pdwResult) = result;
    if ( result > 0 )
      error = errOK;
    if ( result == 0 )
      error = errEOF;
    if ( result == -1 )
      error = errUNEXPECTED;
  }
  return error;
}
