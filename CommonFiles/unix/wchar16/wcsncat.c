#include <wchar.h>

#include <libutf/utf.h>

wchar_t* wcsncat ( wchar_t *dest, 
		   const wchar_t *src,
		   size_t n )
{
	return (wchar_t*) runestrncat ( (Rune*) dest, (Rune*) src, n );
}
