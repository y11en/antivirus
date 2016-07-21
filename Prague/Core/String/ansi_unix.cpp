#if defined (__unix__) 

#include <string.h>
#include <ctype.h>

#include "ansi.h"

namespace CodecUnix {
  bool PrAnsiToUpper ( const void * src, void * dst, size_t len )  
  {
    if ( src != dst ) {
      memmove ( dst, src, len );
    }
    char* Dst = static_cast <char*> ( dst );
    for ( unsigned i = 0; i < len; ++i )
    {
        Dst[i] = toupper ( Dst[i] );
    }
    return true;
  }
  
  bool PrAnsiToLower ( const void * src, void * dst, size_t len )
  {
    if ( src != dst ) {
      memmove ( dst, src, len );
    }
    char* Dst = static_cast <char*> ( dst );
    for ( unsigned i = 0; i < len; ++i )
    {
        Dst[i] = tolower ( Dst[i] );
    }    
    return true; 
  }
};

#endif
