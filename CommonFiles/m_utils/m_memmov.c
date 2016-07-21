#include <AVPPort.h>
#include <m_utils.h>


#ifdef NON_STANDART_NAMES
  #define MEMMOVE my_memmove
#else
  #define MEMMOVE memmove
#endif


// ---
_MY_CRTIMP void* __cdecl MEMMOVE( void * dst, const void * src, size_t count ) {

  void * ret = dst;

  #if defined (_M_MRX000) || defined (_M_ALPHA) || defined (_M_PPC)
  {
    extern void RtlMoveMemory( void *, const void *, size_t count );

    RtlMoveMemory( dst, src, count );
  }
  #else  // defined (_M_MRX000) || defined (_M_ALPHA) || defined (_M_PPC)
    if (dst <= src || (char *)dst >= ((char *)src + count)) {
      while (count--) { // Non-Overlapping Buffers --> copy from lower addresses to higher addresses
        *(char *)dst = *(char *)src;
        (*(char**)&dst)++;
        (*(char**)&src)++;
      }
    }
    else {
      dst = (char *)dst + count - 1; // Overlapping Buffers
      src = (char *)src + count - 1; // copy from higher addresses to lower addresses
      while (count--) {
        *(char*)dst = *(char*)src;
        (*(char**)&dst)--;
        (*(char**)&src)--;
      }
    }
  #endif  /* defined (_M_MRX000) || defined (_M_ALPHA) || defined (_M_PPC) */

  return ret;
}


