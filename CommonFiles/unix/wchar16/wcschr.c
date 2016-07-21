#include <wchar.h>

#include <libutf/utf.h>

wchar_t* wcschr ( const wchar_t *wcs, 
		  wchar_t wc)
{
	return (wchar_t*) runestrchr( (Rune*) wcs, (Rune) wc);
}
