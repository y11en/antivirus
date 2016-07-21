#include <AVPPort.h>

#ifdef NON_STANDART_NAMES
  #define STRCMP my_strcmp
#else
  #define STRCMP strcmp
  #pragma function(STRCMP)
#endif


// ---
int __cdecl STRCMP( const char* s1, const char* s2 ) {
  int ret;
  while( !(ret = *s1 - *s2) && *s1 )
    ++s1, ++s2;
  return ret;
}
