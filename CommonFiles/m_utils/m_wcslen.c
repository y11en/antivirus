#include <AVPPort.h>


#ifdef NON_STANDART_NAMES
  #define WCSLEN my_wcslen
#else
  #define WCSLEN wcslen
	#if !defined(_DEBUG) && !defined(WANT_INTRINSIC)
		#define _NO_INTRINSIC
	#endif
#endif


// ---
#if !defined(_NO_INTRINSIC)
	size_t __cdecl WCSLEN( const wchar_t* wcs ) {
		const wchar_t *eos = wcs;
		while( *eos++ ) ;
		return (size_t)(eos - wcs - 1);
	}
#endif
