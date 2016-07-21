#include <AVPPort.h>
#include <m_utils.h>


#ifdef NON_STANDART_NAMES
  #define STRNCPY my_strncpy
#else
  #define STRNCPY strncpy
#endif

#include <m_utils.h>

// ---
_MY_CRTIMP char* __cdecl STRNCPY( char* dst, const char* src, size_t max_len ) {
	size_t i;
  char*  p;

	if ( !max_len )
		return dst;
	
	p = dst;
  for( i=0; *src && (i<max_len); ++i,++src,++p )
    *p = *src;
	if ( i < max_len )
		memset( p, 0, max_len-i );
  return dst;
}
