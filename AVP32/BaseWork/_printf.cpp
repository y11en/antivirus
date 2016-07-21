////////////////////////////////////////////////////////////////////
//
//  _PRINTF.CPP
//  Printf implementation for different plathforms
//  General purpose
//  Alexey de Mont de Rique [graf@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs.
//
////////////////////////////////////////////////////////////////////
#include "StdAfx.h"

#if ((defined(__WATCOMC__) && (defined(__DOS__)||defined(__OS2__)))||(defined(__LINUX__)))
 #ifndef __LINUX__
  #define NOAFX
  #include <os2.h>
  #include <string.h>
  #include "../typedef.h"
  #include "../retcode3.h"
  #include "../../common.fil/scanapi/avp_msg.h"
  #include <tchar.h>
  #define AFXAPI
  #include "../../common.fil/stuff/_carray.h"


  #include <conio.h>
 #else
  #define NEAR
  #include <sys/stat.h>
  #include <string.h>
  #include "../avplinux/typedef.h"
  #include "../typedef.h"
  #include "../avp_msg.h"
  #include "../retcode3.h"
  #include "../_carray.h"
  #include <libio.h>
//  #include <kpathsea/c-vararg.h>
 #endif
 #include <stdarg.h>
#endif

#include <_AVPIO.h>

#include <stdio.h>
#ifdef __MWERKS__
	#include <stdarg.h>
	#include "avp_msg.h"
#endif	


#include "_printf.h"

int Printf (const char *msg, ...)
{
	static char s[512];
	int ret=0;
	va_list ap;
	va_start(ap,msg);
	ret=vsprintf_s(s, sizeof(s), msg, ap);
	va_end(ap);
	AvpCallback(AVP_CALLBACK_PUT_STRING,(DWORD)s);
	return ret;
}
