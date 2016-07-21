#include <AVPPort.h>


#ifdef NON_STANDART_NAMES
  #define WCSCMP my_wcscmp
#else
  #define WCSCMP wcscmp
	#if !defined(_DEBUG) && !defined(WANT_INTRINSIC)
		#define _NO_INTRINSIC
	#endif
#endif

// ---
#if !defined(_NO_INTRINSIC)
	int __cdecl WCSCMP( const wchar_t* s1, const wchar_t* s2 ) {
		int ret;
		while( !(ret = *s1 - *s2) && *s1 )
			++s1, ++s2;
		return ret;
	}
#endif