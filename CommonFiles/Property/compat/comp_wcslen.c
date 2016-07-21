/*
 * $Id: comp_wcslen.c,v 1.1 2005/06/09 11:00:51 santucco Exp $
 */

#include <config-unix.h>
#include <sys/types.h>
	
#if defined (HAVE_WCHAR_H)
	#include <wchar.h>
#else
	typedef unsigned short	wchar_t;
#endif

size_t wcslen (wchar_t * wcs)
{
	const wchar_t *eos = wcs;
	
	while ( *eos++ )
		;
	
	return (size_t) (eos - wcs - 1);
}
