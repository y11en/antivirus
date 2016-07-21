#include <wchar.h>

#include <libutf/utf.h>

wchar_t* wcscpy ( wchar_t *dest, 
		  const wchar_t *src )
{
	return (wchar_t*) runestrcpy ( (Rune*) dest, (Rune*) src );
}
