#ifndef __avp_data_h
#define __avp_data_h


  #define DATA_PROC
  #if (defined (__LINUX__))
    //#include <wchar.h>
    //typedef wchar_t     TCHAR;
    #ifndef _TCHAR_DEFINED
     //#if     !__STDC__
      typedef char            TCHAR;
     //#endif
     #define _TCHAR_DEFINED
    #endif
   #define  DATA_PARAM 
  #else
   #define  DATA_PARAM  __cdecl
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
  typedef unsigned long     AVP_dword;
  typedef unsigned long     AVP_bool;

  typedef int               AVP_int;
  typedef unsigned int      AVP_uint;
  typedef unsigned char     AVP_date[8];
  typedef unsigned char     AVP_time[8];
  typedef unsigned char     AVP_datetime[16];

  #if (defined (__LINUX__) || defined (__unix__))
		typedef unsigned long long int AVP_qword;
		typedef long long			     AVP_longlong;
	#else
		typedef unsigned __int64	AVP_qword;
		typedef signed __int64	  AVP_longlong;
	#endif
#endif
