#ifndef __avp_data_h
#define __avp_data_h

#include "AVPSize_t.h"

#define DATA_PROC

#if defined (__unix__) || defined (__MC68K__)	// Unix or Palm OS Motarolla 68k Compiller
	#define DATA_PARAM
#else
	#define DATA_PARAM __cdecl
#endif

	typedef          char     AVP_char;
	typedef          char*    AVP_str;
	typedef unsigned short*   AVP_wstr;
	typedef signed   short    AVP_short;
	typedef signed   long     AVP_long;

	typedef unsigned char     AVP_byte;
	typedef unsigned char     AVP_group;
	typedef unsigned short    AVP_word;
	typedef unsigned short    AVP_wchar;

/* When compiling for F-Secure, our type definitions should match those
 * in the F-Secure SDK. In particular, DWORD should be "unsigned long"
 */
#if (defined (__unix__)  || defined (__MWERKS__)) && !defined(USE_FM)
	typedef unsigned int	    AVP_dword;
	typedef unsigned int	    AVP_bool;
#else
	typedef unsigned long     AVP_dword;
	typedef unsigned long     AVP_bool;
#endif /* __unix__ */

	typedef int               AVP_int;
	typedef unsigned int      AVP_uint;
	typedef unsigned char     AVP_date[8];
	typedef unsigned char     AVP_time[8];
	typedef unsigned char     AVP_datetime[16];
	typedef size_t            AVP_size_t;

#if defined (__unix__)
	typedef unsigned long long int AVP_qword;
	typedef long long int          AVP_longlong;
#elif (defined (__MC68K__))
	//Palm OS Motarolla 68k Compiller
	typedef unsigned long          AVP_qword;
	typedef long                   AVP_longlong;
#else
	typedef unsigned __int64       AVP_qword;
	typedef signed __int64         AVP_longlong;
#endif

#endif /* __avp_data_h */

