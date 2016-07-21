#if !defined(__AVPPort_h__) 
#define __AVPPort_h__


#if defined( _WIN32_WCE	) // Pocket PC
  #include "port_wce.h"

#elif defined( _WIN32 )
// Windows section    

#elif defined ( __MC68K__	) // Palm OS
  #include "port_palm.h"

#elif defined (__unix__)
  #include "port_unix.h"	

#elif defined (__DOS__)
  // DOS section

#elif defined (__OS2__)
  // OS2 section

#elif defined (__MWERKS__)
  #include "port_novell.h"

#else
  #error UNKNOWN PLATFORM

#endif


#if defined( M_UTILS )
  #include <m_utils.h>
#else
  #include <string.h>
#endif
    

#if defined(_WIN32) || defined(_MAC)

	#include <crtdbg.h>
	#include <minmax.h>
   
#else // for all other platforms

  #if !defined( _ASSERT )
	  #define _RPT0(a, b)
	  #define _RPT1(a, b, c)
	  #define _RPT2(a, b, c, d)
	  #define _RPT3(a, b, c, d, e)
	  #define _RPT4(a, b, c, d, e, f)

    #define _ASSERT(a)
    #define _ASSERTE(a)
  #endif


  #if !defined( max )
	  #define max(a,b) (((a) > (b)) ? (a) : (b))
  #endif

  #if !defined( min )
    #define min(a,b) (((a) < (b)) ? (a) : (b))
  #endif

  #if !defined NULL
    #if defined(__cplusplus) || defined( N_RC_INVOKED ) || \
          defined(M_I86S) || defined(M_I86SM) || defined(M_I86C) || defined(M_I86CM) || \
          defined(__TINY__) || defined(__SMALL__) || defined(__MEDIUM__) || \
          defined(N_PLAT_NLM)
      #define NULL   0
	  #elif defined( N_MSC )
			#define NULL ((void *) 0)
    #else
      #define NULL   0L
    #endif
	#endif /* NULL */
#endif // for all other platforms


#endif /* __AVPPort_h__ */



