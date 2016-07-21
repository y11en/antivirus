//
// main.cpp
//

#pragma warning(disable : 4786)

#include <windows.h>

#include "Screensaver.h"

//--------------------------- WinMain -------------------------------

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    CScreensaver saver ;
    saver.Run(hInstance) ;

    return 0 ;
}

//-------------------------------------------------------------------