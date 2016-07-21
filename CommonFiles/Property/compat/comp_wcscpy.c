/*
 * $Id: comp_wcscpy.c,v 1.1 2005/06/09 11:00:51 santucco Exp $
 */

#include <config-unix.h>

#if defined (HAVE_WCHAR_H)
	#include <sys/types.h>
	#include <wchar.h>
#else
	typedef unsigned short	wchar_t;
#endif

wchar_t * wcscpy (wchar_t * dst, const wchar_t * src)
{
	wchar_t * cp = dst;
	
	while ( (*cp++ = *src++) != 0 )
		;   // copy src over dst
	
	return dst;
}
