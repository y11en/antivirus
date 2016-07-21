#include <AVPPort.h>

#ifdef NON_STANDART_NAMES
  #define STRLEN my_strlen
#else
  #define STRLEN strlen
  #pragma function(STRLEN)
#endif


// ---
size_t STRLEN( const char* src ) {

  const char* eos = src;
  while( *eos++ ) ;
  return (size_t)(eos - src - 1);
}


// ---
size_t _mbslen( const unsigned char* src ) {
	
  const char* eos = (const char*)src;
  while( *eos++ ) ;
  return (size_t)(eos - src - 1);
}

