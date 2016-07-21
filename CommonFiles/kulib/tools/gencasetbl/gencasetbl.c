


#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <stdio.h>
#include <time.h>


static const char* get_os_name()
{
	static char buf[ 50 ];

    OSVERSIONINFO os_info;
    os_info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx( &os_info );

    if( os_info.dwMajorVersion == 5 )
    {
        switch( os_info.dwMinorVersion )
        {
            case 0: return "windows 2000";
			case 1: return "windows xp";
			case 2: return "windows dot-net-server";
        }
    }
    else if( os_info.dwMajorVersion == 4 )
    {
        switch( os_info.dwMinorVersion )
        {
		case 0: 
            if( os_info.dwPlatformId == VER_PLATFORM_WIN32_NT )
            {
                return "windows nt";
            }
            else if( os_info.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS )
            {
                return "windows 95";
            }
			break;

		case 10:
            return "windows 98";
        case 90:
            return "windows me";
		}
    }
		
    return "unknown";
}

int main( int argc, char* argv[] )
{
	int uchar, res;
	FILE* fp;

	char* out_filename = ( argc > 1 ) ? argv[ 1 ] : "case.map";

    time_t cur_time;
    char time_str[ 100 ];
	
	fp = fopen( out_filename, "wt" );
	if( fp == NULL )	
	{
		fprintf( stderr, "error: can't open file '%s' for writing.\n", out_filename );
		return 1;
	}




    time( &cur_time );
    strftime( time_str, sizeof(time_str), "%b %d %H:%M %Z %Y", localtime( &cur_time ) );
    
    fprintf( fp, "#\n");
    fprintf( fp, "# Created: %s\n", time_str );
    fprintf( fp, "# Created by gencasetbl tool.\n", time_str );
    fprintf( fp, "# Created on %s.\n", get_os_name() );
    fprintf( fp, "#\n");
    fprintf( fp, "\n");



	fprintf( fp, "\n");
	fprintf( fp, "BEGIN UPPERCASE\n");
	fprintf( fp, "\n");
	for( uchar = 0; uchar < 0x10000; ++uchar )
	{

		res = (int) CharUpperW( (LPWSTR) uchar );

		if( uchar != res )
		{
			fprintf( fp, "U%04x U%04x\n", uchar, res );
		}
	}
	fprintf( fp, "\n");
	fprintf( fp, "END UPPERCASE\n");


	fprintf( fp, "\n");
	fprintf( fp, "\n");
	fprintf( fp, "BEGIN LOWERCASE\n");
	fprintf( fp, "\n");
	for( uchar = 0; uchar < 0x10000; ++uchar )
	{

		res = (int) CharLowerW( (LPWSTR) uchar );

		if( uchar != res )
		{
			fprintf( fp, "U%04x U%04x\n", uchar, res );
		}
	}

	fprintf( fp, "\n");
	fprintf( fp, "END LOWERCASE\n");
	
	fclose( fp );
	
	return 0;
}