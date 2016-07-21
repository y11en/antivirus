#if !defined(_avp_size_t)
#define _avp_size_t

#if !defined(_SIZE_T_DEFINED)
	#if defined (__unix__)
		#include <sys/types.h>
	#elif defined( _WIN32 )
		#if defined(_M_X64) || defined(_M_IA64) || defined(_WIN64)
			typedef unsigned __int64    size_t;
		#elif defined( _W64 )
			typedef _W64 unsigned int   size_t;
		#else
			typedef unsigned int        size_t;
		#endif
		#define _SIZE_T_DEFINED
	#endif
#endif

#endif // _avp_size_t
