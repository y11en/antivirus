#include <wchar.h>

#include <libutf/utf.h>

wchar_t* wcsrchr ( const wchar_t *wcs, 
		   wchar_t wc)
{
	return (wchar_t*) runestrrchr( (Rune*) wcs, (Rune) wc);
}
