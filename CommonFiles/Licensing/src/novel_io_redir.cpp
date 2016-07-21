#define AVPIOSTATIC

#include "../include/novell_io_redir.h"

#if defined (__MWERKS__)

/*
#include <stdlib.h>
#include <stdio.h>
#include <_avpio.h>
#include <fcntl.h>
#include <mode.h>
#include <nwfattr.h>
#include <nwfileng.h>
#include <nwfileio.h>
#include <platform_compat.h>
*/

//-----------------------------------------------------------------------------

HANDLE WINAPI nlmAvpCreateFile (
    LPCTSTR  lpFileName,                            // pointer to name of the file
    DWORD  dwDesiredAccess,                         // access (read-write)
    DWORD  dwShareMode,                             // share mode
    SECURITY_ATTRIBUTES FAR*  lpSecurityAttributes, // pointer to security descriptor
    DWORD  dwCreationDistribution,                  // how to create
    DWORD  dwFlagsAndAttributes,                    // file attributes
    HANDLE  hTemplateFile                           // handle to file with attributes to copy
        )
{
 int access_=0;
 int r=0;
 int permission=0;
 int attr=0;
 
/* !!! Zubrilin
	if(stackavail()<8192) {
		BAvpCancelProcessObject(1);
 	return (HANDLE)-1;}
*/

 access_=O_BINARY;

// attr=NO_RIGHTS_CHECK_ON_OPEN_BIT | LEAVE_FILE_COMPRESSED_DATA_BIT;

 if(dwDesiredAccess==0) access_|=O_RDWR;
 else
 {
  if(dwDesiredAccess & GENERIC_READ)
  {
   if(dwDesiredAccess & GENERIC_WRITE) access_|=O_RDWR;
   else
   {
   	access_|=O_RDONLY;permission|=S_IRUSR;
   }
  }
  else if(dwDesiredAccess & GENERIC_WRITE){access_|=O_WRONLY;permission|=S_IWUSR;}
 }

 switch(dwCreationDistribution)
 {
  case CREATE_NEW:    access_|=O_CREAT; break;
  case CREATE_ALWAYS: access_|=O_CREAT;attr|=DELETE_FILE_ON_CREATE_BIT;break;
  case OPEN_EXISTING: break;
  case OPEN_ALWAYS:   access_|=O_CREAT;break;
  case TRUNCATE_EXISTING: access_|=O_TRUNC;break;
  default:  break;
 }
// dwShareMode=SH_DENYWR /*RW*/;
 dwShareMode=0;
 r= static_cast<int> (FEsopen((char *)lpFileName,access_,dwShareMode,permission,attr,0));
// Zubrilin if(r!=-1) OpenFiles++; 
 
 return (HANDLE)r;
}


//-----------------------------------------------------------------------------

BOOL __stdcall nlmAvpCloseHandle(HANDLE  hObject)     // handle to object to close
{
	int h=(int)hObject;
	int ret = close (h);
	return (0 != ret);
}

//-----------------------------------------------------------------------------

DWORD WINAPI nlmAvpGetFileSize(HANDLE  hFile,      // handle of file
                               LPDWORD  lpHigh)
{
 DWORD len;
 if(lpHigh!=NULL) *lpHigh=0;
 len=filelength(reinterpret_cast<int> (hFile));
 return len;
}

//-----------------------------------------------------------------------------

BOOL WINAPI nlmAvpReadFile(
    HANDLE  hFile,                      // handle of file to read
    LPVOID  lpBuffer,                   // address of buffer that receives data
    DWORD  nNumberOfBytesToRead,        // number of bytes to read
    LPDWORD  lpNumberOfBytesRead,       // address of number of bytes read
    OVERLAPPED *  lpOverlapped          // address of structure for data BYTE Disk, WORD Sector ,BYTE Head ,LPBYTE Buffer)
        )
{
 int err=0;
 BOOL ret=false;
 
 err=read(reinterpret_cast<int> (hFile),lpBuffer,nNumberOfBytesToRead);

 if(err==-1) {err=0;goto ret;}
 ret=true;
ret:
 if(lpNumberOfBytesRead) *lpNumberOfBytesRead=err;
 return ret;
}

//-----------------------------------------------------------------------------

BOOL WINAPI nlmAvpWriteFile (
    HANDLE  hFile,                      // handle to file to write to
    LPCVOID  lpBuffer,                  // pointer to data to write to file
    DWORD  nNumberOfBytesToWrite,       // number of bytes to write
    LPDWORD  lpNumberOfBytesWritten,    // pointer to number of bytes written
    OVERLAPPED FAR*  lpOverlapped       // addr. of structure needed for overlapped I/O
        )
{
 int err=0;
 
 err=write(reinterpret_cast<int> (hFile),lpBuffer,nNumberOfBytesToWrite);
 if(err==-1) {if(lpNumberOfBytesWritten) *lpNumberOfBytesWritten=0;return false;}
 if(lpNumberOfBytesWritten) *lpNumberOfBytesWritten=err;
 return true;
}


//-----------------------------------------------------------------------------

#endif
