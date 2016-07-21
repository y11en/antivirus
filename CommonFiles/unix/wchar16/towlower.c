#include <wchar.h>
#include <libutf/utf.h>

wint_t towlower ( wint_t wc )
{
	return (Rune) tolowerrune((Rune) wc);
}
