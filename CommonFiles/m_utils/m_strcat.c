#include <AVPPort.h>

#ifdef NON_STANDART_NAMES
  #define STRCAT my_strcat
#else
  #define STRCAT strcat
  #pragma function(STRCAT)
#endif


// ---
char* __cdecl STRCAT( char* dst, const char* src ) {
  char* cp = dst;
  while ( *cp ) cp++;
  while ( *cp++ = *src++ ) ;
  return dst;
}
