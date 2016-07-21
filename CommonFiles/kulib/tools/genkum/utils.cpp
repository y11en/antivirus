
#include "utils.h"

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>

const char* get_os_name()
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

#endif

