#include <AVPPort.h>


#ifdef NON_STANDART_NAMES
  #define STRCPY my_strcpy
#else
  #define STRCPY strcpy
  #pragma function(STRCPY)
#endif


// ---
char* __cdecl STRCPY( char* dst, const char* src ) {
  char* ret = dst;
  for( ; *src; src++,dst++ )
    *dst = *src;
	*dst = *src;
  return ret;
}
