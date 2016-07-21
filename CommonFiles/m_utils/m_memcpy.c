#include <AVPPort.h>


#ifdef NON_STANDART_NAMES
  #define MEMCPY my_memcpy
#else
  #define MEMCPY memcpy
  #pragma function(memcpy)
#endif



// ---
void* __cdecl MEMCPY( void* dst, const void* src, size_t count ) {

  unsigned int i;
  void*  ret = dst;
  size_t u_size = count / sizeof(unsigned int);
  for( i=0; i<u_size; i++ )
    *((*(unsigned int**)&dst)++) = *((*(unsigned int**)&src)++);
  u_size = count % sizeof(unsigned int);
  for( i=0; i<u_size; i++ )
    *((*(unsigned char**)&dst)++) = *((*(unsigned char**)&src)++);
  return ret;
}


