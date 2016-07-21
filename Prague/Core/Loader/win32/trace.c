#if defined (_WIN32)

#include <Prague/prague.h>
#include <windows.h>

#define LETTER(a,l) ( ((a)==l) || ((a)==(l-'a'+'A')) )
#define ENTER(a)    ( LETTER(a[0],'e') && LETTER(a[1],'n') && LETTER(a[2],'t') && LETTER(a[3],'e') && LETTER(a[4],'r') )
#define LEAVE(a)    ( LETTER(a[0],'l') && LETTER(a[1],'e') && LETTER(a[2],'a') && LETTER(a[3],'v') && LETTER(a[4],'e') )


// ---
tVOID Trace( tSTRING str, hOBJECT obj, tTRACE_LEVEL level ) {
	static char b[0x1000];
	//static  indent = 0;
    //int     c = 0;
	int     len;
	tSTRING cr;
	//tIID    iid = CALL_SYS_PropertyGetDWord( obj, pgINTERFACE_ID );
	//tPID    pid = CALL_SYS_PropertyGetDWord( obj, pgPLUGIN_ID );
	len = lstrlen( str );
	memcpy( b, str, len+1 );
	if ( (b[len] == '\n') || (b[len] == '\r') )
		cr = "";
	else
		cr = "\n";
	
	/*
  if ( indent && LEAVE(b) )
    indent--;
	
  if ( indent ) {
    if ( indent > 20 )
      c = 40;
    else
      c = indent * 2;
		if ( c ) {
			memmove( b+c, b, len+1 );
			memset( b, ' ', c );
		}
  }
	
  if ( ENTER((b+c)) )
    indent++;
	*/
	
	//printf( "[%u,%u],%u - %s%s", iid, pid, level, b, cr );
	//pprint( 0, "[%u,%u],%u - %s%s", iid, pid, level, b, cr );
	//wsprintf( b, "[%5u,%5u],%4u - %s%s", iid, pid, level, str, cr );
	wsprintf( b, "%s%s", str, cr );
	OutputDebugString( b );
}


#endif //_WIN32

