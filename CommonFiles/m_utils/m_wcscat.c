#include <AVPPort.h>


#ifdef NON_STANDART_NAMES
  #define WCSCAT my_wcscat
#else
  #define WCSCAT wcscat
	#if !defined(_DEBUG) && !defined(WANT_INTRINSIC)
		#define _NO_INTRINSIC
	#endif
#endif

// ---
#if !defined(_NO_INTRINSIC)
	wchar_t* __cdecl WCSCAT( wchar_t* dst, const wchar_t* src ) {
		wchar_t* cp = dst;
		while( *cp ) 
			cp++;
		while( 0 != (*cp++ = *src++) )
			;
		return dst;
	}
#endif


