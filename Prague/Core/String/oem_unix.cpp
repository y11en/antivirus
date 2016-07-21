#if defined (__unix__) 
#include <string.h>

#include "oem.h"
#include "ansi.h"

namespace CodecUnix {
  bool PrOemToAnsi ( const void * src, void * dst, size_t len )
  {
    if ( src != dst ) {
        memmove ( dst, src, len );
    }
    return true;
  }

  bool PrAnsiToOem ( const void * src, void * dst, size_t len )
  {
    if ( src != dst ) {
      memmove ( dst, src, len );
    }
    return true;
  }

  bool PrOemToUpper ( const void * src, void * dst, size_t len )
  {
    if ( src != dst ) {
        memmove ( dst, src, len );
    }

    return PrOemToAnsi ( src, dst, len )
      &&  PrAnsiToUpper ( dst, dst, len )
      &&  PrAnsiToOem ( dst, dst, len );
  }

  bool PrOemToLower ( const void * src, void * dst, size_t len )
  {
    if ( src != dst ) {
        memmove ( dst, src, len );
      }

    return PrOemToAnsi ( src, dst, len )
      &&  PrAnsiToLower ( dst, dst, len )
      &&  PrAnsiToOem ( dst, dst, len );
  }
};

#endif
