#include <wchar.h>

#include <libutf/utf.h>

wchar_t* wcscat ( wchar_t *dest, 
		  const wchar_t *src )
{
	return (wchar_t*) runestrcat ( (Rune*) dest, (Rune*) src );
}
