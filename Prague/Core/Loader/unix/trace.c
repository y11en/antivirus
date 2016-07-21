#if defined (__unix__)

#include <string.h>
#include <stdio.h>

#include <Prague/prague.h>

// ---
tVOID Trace( tSTRING str, hOBJECT obj, tTRACE_LEVEL level ) {
#if defined (_PRAGUE_TRACE_)
	static char b[0x1000];
	int     len;
	tSTRING cr;
	len = strlen( str );
	memcpy( b, str, len+1 );
	if ( (b[len] == '\n') || (b[len] == '\r') )
		cr = "";
	else
		cr = "\n";
	sprintf( b, "%s%s", str, cr );
	fprintf ( stderr, b );
#endif //_PRAGUE_TRACE_
}

#endif //__unix__


