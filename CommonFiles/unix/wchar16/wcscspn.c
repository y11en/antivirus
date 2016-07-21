#include <wchar.h>

#include <libutf/utf.h>

size_t wcscspn(const wchar_t *wcs, const wchar_t *reject)
{
	const wchar_t* buf = wcs;
	int len = wcslen ( reject );
	while ( *buf ) {
		int i;
		for ( i=0; i < len; i++ )
			if ( *buf == reject [ i ] )
				break;
	}
	return buf - wcs;
}
