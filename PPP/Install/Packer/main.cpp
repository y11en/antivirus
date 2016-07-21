


#include <windows.h>
#include <stdio.h>
#include <string>

#include "common.h"



#define PACK_INDICATION     "$$$"



int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
    if (strncmp(lpCmdLine, PACK_INDICATION, strlen(PACK_INDICATION)) == 0)
    {
        if (!DoPack(lpCmdLine))
        {
            return 1 ;
        }
    }
    else
    {
        if (!DoInstall(lpCmdLine))
        {
            return 1 ;
        }
    }

	return 0;
}



