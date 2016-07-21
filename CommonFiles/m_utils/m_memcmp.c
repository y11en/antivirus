#include <AVPPort.h>


#ifdef NON_STANDART_NAMES
  #define MEMCMP my_memcmp
#else
  #define MEMCMP memcmp
  #pragma function(memcmp)
#endif


// ---
int __cdecl MEMCMP( const void* m1, const void* m2, size_t count ) {

  unsigned int i;
  int  ret = 0;
  size_t u_size = count / sizeof(unsigned int);
  for( i=0; i<u_size; i++ ) {
    ret = *((*(unsigned int**)&m1)++) - *((*(unsigned int**)&m2)++);
    if ( ret )
      return ret;
  }
  u_size = count % sizeof(unsigned int);
  for( i=0; i<u_size; i++ ) {
    ret = *((*(unsigned char**)&m1)++) - *((*(unsigned char**)&m2)++);
    if ( ret )
      return ret;
  }
  return 0;
}



