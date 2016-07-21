//	uscanman.cpp
//	Urmas Rahu, Data Fellows, 1993-97

// Jukka980424{
// Stripped down a lot for FPIRUN -- we just need the sector routines
// Jukka980424}

#include <stdio.h>
#include <windows.h>


//Kikka970910{
// Returns TRUE on success, 0 if the function fails
BOOL ReadFloppyBootSector(unsigned char * buffer_, char drivespec_) // Drivespec as "A:"
{
    HANDLE hDrive;
    int ret = 1;

    char helpbuf[12];
    sprintf(helpbuf,"\\\\.\\%c:", drivespec_);

    hDrive = CreateFile(
           helpbuf,
           GENERIC_READ,
           FILE_SHARE_WRITE,        // jmk: was 0
           NULL,
           OPEN_EXISTING,
           0,
           NULL
           );

    if ( hDrive == INVALID_HANDLE_VALUE )
    {
        ret = 0;
    }
    else
    {
        DWORD totalread;
        if (!ReadFile(hDrive, buffer_, 512, &totalread,NULL))
            ret = 0;
        CloseHandle(hDrive);
    }

    return ret;
}
//Kikka970910}

//Kikka980319{
// Returns TRUE on success, 0 if the function fails
BOOL ReadMBR(unsigned char * buffer_, int nr_) // # of phys drive as 0, 1, 2...
{
    HANDLE hDrive;
    int ret = 1;

    char helpbuf[32];
    sprintf(helpbuf,"\\\\.\\physicaldrive%d", nr_);

    hDrive = CreateFile(
           helpbuf,
           GENERIC_READ | GENERIC_WRITE | FILE_SHARE_READ | FILE_SHARE_WRITE,
           0,
           NULL,
           OPEN_EXISTING,
           0,
           NULL
           );

    if ( hDrive == INVALID_HANDLE_VALUE )
    {
        ret = 0;
    }
    else
    {
        DWORD totalread;
        if (!ReadFile(hDrive, buffer_, 512, &totalread,NULL))
            ret = 0;
        CloseHandle(hDrive);
    }

    return ret;
}
//Kikka980319}

