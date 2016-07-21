#include <AVPPort.h>

#ifdef NON_STANDART_NAMES
  #define MEMSET my_memset
#else
  #define MEMSET memset
  #pragma function(MEMSET)
#endif


// ---
void* __cdecl MEMSET( void* dst, int fill, size_t count ) {

  unsigned int i;
  void*  ret = dst;
  size_t u_size = count / sizeof(unsigned int);
  if ( u_size ) {
    unsigned int u_fill;
    if ( sizeof(short) < sizeof(unsigned int) )
      u_fill = ((unsigned char)fill) | (((unsigned char)fill)<<8) | (((unsigned char)fill)<<16) | (((unsigned char)fill)<<24);
    else
      u_fill = ((unsigned char)fill) | (((unsigned char)fill)<<8);
    for( i=0; i<u_size; i++ )
      *(((unsigned int*)dst)++) = u_fill;
  }
  u_size = count % sizeof(unsigned int);
  for( i=0; i<u_size; i++ )
    *(((unsigned char*)dst)++) = (unsigned char)fill;
  return ret;
}



