//#include "tv32def.h"
#define SYSTOS2
#ifdef SYSTOS2
#include <os2.h>
#endif
#include "memalloc.h"
#include <conio.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <share.h>
#include <io.h>
#include <direct.h>
#include <bios.h>
//#include <bios98.h>
#include <dos.h>
#include <c:\lang\watcom\h\sys\types.h>
#include <c:\lang\watcom\h\sys\stat.h>
#include "resource.h"
#include "avp_os2.h"
#include "profile.h"
#include "start.h"
#include "disk.h"
#include "..\\typedef.h"
#include "..\\tchar.h"
#define AVPIOSTATIC
#include "../../Common.Fil/_avpio.h"
#include "../../Common.Fil/ScanAPI/BaseAPI.h"
#include "../../Common.Fil/ScanAPI/avpmem.h"

//--------------- filefunk --------------
int OpenFiles=0;
BOOL WINAPI AvpReadFile(
    HANDLE  hFile,      // handle of file to read
    LPVOID  lpBuffer,   // address of buffer that receives data
    DWORD  nNumberOfBytesToRead,  // number of bytes to read
    LPDWORD  lpNumberOfBytesRead, // address of number of bytes read
    OVERLAPPED *  lpOverlapped)   // address of structure for data BYTE Disk, WORD Sector ,BYTE Head ,LPBYTE Buffer)
 {
  int err=0;
  BOOL  ret=FALSE;
  RMI   rmi;
  DWORD ulBytesToRead,ulNumberOfBytesToRead;
  char* ptrBuffer;

  if(hFile==(HANDLE)-1) return FALSE;

  ptrBuffer=(char*)lpBuffer;
  ulNumberOfBytesToRead=nNumberOfBytesToRead;
  *lpNumberOfBytesRead=0;
read:
  if(ulNumberOfBytesToRead>SIZEALLOC)
    ulBytesToRead=SIZEALLOC;
  else
    ulBytesToRead=ulNumberOfBytesToRead;
  ulNumberOfBytesToRead-=ulBytesToRead;

//  if(pmResizeMem((USHORT)ulBytesToRead,SS.selector))
   {
    SS.offset=0;

    memset(&rmi,0,sizeof(RMI));
    rmi.EAX = 0x3f00;
    rmi.EBX = (WORD)hFile;
    rmi.ECX = ulBytesToRead;
    rmi.EDX = 0;
    rmi.DS  = SS.segment;
    if(SimulateRMInt(0x21,&rmi))
     {
      *lpNumberOfBytesRead+=rmi.EAX&0xffff;
      _fmemcpy(ptrBuffer,SS.str,rmi.EAX&0xffff);
      ptrBuffer+=ulBytesToRead;
      ret=TRUE;
      //cprintf("Read file %d %d\r\n",rmi.EAX,rmi.ECX);
     }
    else
     {
      //rc=rmi.EAX;
      ret=FALSE;
      //cprintf("Read file Error %d \r\n",rmi.EAX);
     }
   }
  //else cprintf("Resize Error\r\n");

  if(ulNumberOfBytesToRead>0) goto read;

  lpOverlapped=lpOverlapped;
  return ret;
 }

BOOL WINAPI AvpWriteFile(
    HANDLE  hFile,      // handle to file to write to
    LPCVOID lpBuffer,  // pointer to data to write to file
    DWORD   nNumberOfBytesToWrite,    // number of bytes to write
    LPDWORD lpNumberOfBytesWritten, // pointer to number of bytes written
    OVERLAPPED FAR*  lpOverlapped)   // addr. of structure needed for overlapped I/O
 {
  int err=0;
  BOOL ret=FALSE;
  RMI   rmi;
  DWORD ulBytesToWrite,ulNumberOfBytesToWrite;
  char* ptrBuffer;

  if(hFile==(HANDLE)-1) return FALSE;

  ptrBuffer=(char*)lpBuffer;
  ulNumberOfBytesToWrite=nNumberOfBytesToWrite;
  *lpNumberOfBytesWritten=0;
write:
  if(ulNumberOfBytesToWrite>SIZEALLOC)
    ulBytesToWrite=SIZEALLOC;
  else
    ulBytesToWrite=ulNumberOfBytesToWrite;
  ulNumberOfBytesToWrite-=ulBytesToWrite;

  //if(pmResizeMem((USHORT)ulBytesToWrite,SS.selector))
   {
    SS.offset=0;
    _fmemcpy(SS.str,ptrBuffer,ulBytesToWrite);

    memset(&rmi,0,sizeof(RMI));
    rmi.EAX = 0x4000;
    rmi.EBX = (WORD)hFile;
    rmi.ECX = ulBytesToWrite;
    rmi.EDX = 0;
    rmi.DS  = SS.segment;

//printf("Write file: %d\r\n",(WORD)hFile);
    if(SimulateRMInt(0x21,&rmi))
     {
      *lpNumberOfBytesWritten+=rmi.EAX&0xffff;
      ptrBuffer+=ulBytesToWrite;
      ret=TRUE;
      //cprintf("Write file %d %d\r\n",rmi.EAX,rmi.ECX);
     }
    else
     {
      //rc=rmi.EAX;
      ret=FALSE;
      //cprintf("Write file Error %d \r\n",rmi.EAX);
     }
   }
  //else cprintf("Resize Error\r\n");

  if(ulNumberOfBytesToWrite>0) goto write;

  lpOverlapped=lpOverlapped;
  return TRUE;
 }

BOOL __stdcall AvpCloseHandle(HANDLE hObject)     // handle to object to close
 {
  if(hObject==(HANDLE)-1) return FALSE;
  if(close((int)hObject)==0)
   {
    --OpenFiles;
    return TRUE;
   }
  else return FALSE;
 }

#define CREATE_NEW          1 //OPEN_ACTION_CREATE_IF_NEW
#define CREATE_ALWAYS       2 //OPEN_ACTION_OPEN_IF_EXISTS|OPEN_ACTION_CREATE_IF_NEW
#define OPEN_EXISTING       3 //OPEN_ACTION_OPEN_IF_EXIST
//#define OPEN_ALWAYS         4
//#define TRUNCATE_EXISTING   5

#define ERROR_FILE_NOT_FOUND         2
#define ERROR_PATH_NOT_FOUND         3
#define ERROR_TOO_MANY_OPEN_FILES    4
#define ERROR_ACCESS_DENIED          5
#define ERROR_INVALID_ACCESS        12
#define ERROR_SHARING_VIOLATION     32
#define ERROR_OPEN_FAILED          110
#define ERROR_DISK_FULL            112
#define ERROR_FILENAME_EXCED_RANGE 206

int FileOpen32(
    char *   lpFileName,        // pointer to name of the file
    DWORD    dwDesiredAccess,   // access (read-write)
    DWORD    dwShareMode,       // share mode
    DWORD    dwCreationDistribution)      // how to create
 {
  ULONG rc;
  int handle;
  WORD ActionTaken;

//  if(*lpFileName==0)
//   {
//    //errno = EINVAL;
//   return (-1);
//   }

  RMI   rmi;
  memset(&rmi,0,sizeof(RMI));
  rmi.EAX = 0x6c00;
  rmi.EBX = 0;//x4000;//0x42;
  rmi.ECX = FILE_NORMAL;//0x00;
  rmi.EDX = 0;//0x11;

  //readonly 0000 0001 0x01
  //arch     0010 0000 0x20
  if(dwDesiredAccess==0) rmi.EBX|=O_RDWR; //GENERIC_READ RDONLY
   else
    if(dwDesiredAccess==GENERIC_READ)
     {
      rmi.EBX|=O_RDONLY; //GENERIC_WRITE WRONLY
      //cx|=S_IREAD;
     }
     else
      if(dwDesiredAccess==GENERIC_WRITE)
       {
        rmi.EBX|=O_WRONLY;
        rmi.ECX|=0x20;//S_IWRITE; 0x20
       }
      else
       //if((dwDesiredAccess & GENERIC_READ)&&(dwDesiredAccess & GENERIC_WRITE))
        rmi.EBX|=O_RDWR;

  rmi.EBX|=SH_COMPAT;
  if(dwShareMode==FILE_SHARE_READ) rmi.EBX|=SH_DENYWR;
   else
    if(dwShareMode==FILE_SHARE_WRITE) rmi.EBX|=SH_DENYRD;
     else
      if(dwShareMode==(FILE_SHARE_READ|FILE_SHARE_WRITE)) rmi.EBX|=SH_DENYNO;
        else rmi.EBX|=SH_DENYRW;

//O_APPEND    0x0010 0001 0000  /* writes done at end of file */
//O_CREAT     0x0020 0010 0000 /* create new file */
//O_TRUNC     0x0040 0100 0000 /* truncate existing file */
//O_NOINHERIT 0x0080 1000 0000 /* file is not inherited by child process */

//O_BINARY    0x0200  /* binary file */
  switch(dwCreationDistribution)
   {  //0x00x0
    case CREATE_NEW:
      rmi.EDX|=0x12;//O_CREAT|O_TRUNC; //0001 0010 12
      rmi.ECX|=0x20;//S_IWRITE;
      break;
    case CREATE_ALWAYS:
      rmi.EDX|=0x10;//O_CREAT;         //0001 0000 10
      rmi.ECX|=0x20;//S_IWRITE;
//      attr|=DELETE_FILE_ON_CREATE_BIT;
      break;
    case OPEN_EXISTING:
      rmi.EDX|=0x01;//O_CREAT;         //0000 0001 01
      break;
    case OPEN_ALWAYS:
      rmi.EDX|=0x11;//O_CREAT;
      break;
    case TRUNCATE_EXISTING:
      rmi.EDX|=0x02;//O_TRUNC;         //0000 0010 02
      break;
    default:  break;
   }

  //if(pmResizeMem((USHORT)strlen(lpFileName),SS.selector))
   {
    SS.offset=0;

    _fstrcpy(SS.str,lpFileName);
    //_fstrcpy((char far*)(((USHORT)(SS.selector)):>((void __near *)(0))),lpFileName);
    rmi.ESI = 0;
    rmi.DS  = SS.segment;
    if(SimulateRMInt(0x21,&rmi))
     {
      rc=0;
      handle=rmi.EAX&0xffff;
      ActionTaken=(WORD)rmi.ECX;
      //cprintf("open file32 %d <%s>\r\n",handle,lpFileName);
     }
    else
     {
      handle=ActionTaken=0;
      rc=rmi.EAX&0xffff;
      //cprintf("open file32 Error %d <%s>\r\n",rc,lpFileName);
     }
   }
  //else cprintf("Resize Error\r\n");

  if(rc)
   {
    if(rc==ERROR_FILE_NOT_FOUND || rc==ERROR_PATH_NOT_FOUND
         || rc == ERROR_OPEN_FAILED)   ; //errno = ENOENT;
    else
     if(rc==ERROR_TOO_MANY_OPEN_FILES) ; //errno = EMFILE;
      else
       if(rc==ERROR_ACCESS_DENIED || rc==ERROR_INVALID_ACCESS ||
         rc==ERROR_SHARING_VIOLATION)  ; //errno = EACCES;
       else
        if(rc==ERROR_DISK_FULL)        ; //errno = ENOSPC;
        else
         if(rc==ERROR_FILENAME_EXCED_RANGE) ;//errno = ENAMETOOLONG;
         else                               ;//errno = EIO;
    return (-1);
   }

  return (handle);
 }

HANDLE WINAPI AvpCreateFile(
    LPCTSTR  lpFileName,        // pointer to name of the file
    DWORD    dwDesiredAccess,   // access (read-write)
    DWORD    dwShareMode,       // share mode
    SECURITY_ATTRIBUTES FAR* lpSecurityAttributes,     // pointer to security descriptor
    DWORD    dwCreationDistribution,      // how to create
    DWORD    dwFlagsAndAttributes,        // file attributes
    HANDLE   hTemplateFile)       // handle to file with attributes to copy
 {
  int r;

  if((r=FileOpen32((char*)lpFileName,dwDesiredAccess,dwShareMode,
    dwCreationDistribution))!=(-1)) OpenFiles++;
  else
   {
    //cprintf("open file(Error Reset) %d <%s>\r\n",r,lpFileName);
    //reportPutS("Error open file");
    //reportPutS((char*)lpFileName);
    //reportPutS("\r\n");
   }
//printf("open file %d <%s>\r\n",r,lpFileName);

  lpSecurityAttributes=lpSecurityAttributes; hTemplateFile=hTemplateFile;dwFlagsAndAttributes=dwFlagsAndAttributes;
  return (HANDLE)r;
 }

DWORD WINAPI AvpGetFileSize(HANDLE  hFile,      // handle of file
                            LPDWORD lpHigh)
 {
  int len;
  if(hFile==(HANDLE)-1) return FALSE;
  if(lpHigh!=NULL) *lpHigh=0;
  if((len=filelength((int)hFile))!=(-1))
    return len;
  else return FALSE;
 }

//#define _AvpReadFile
//   extern "C" t_AvpReadFile AvpReadFile;
//   extern "C" t_AvpReadFile *_AvpReadFile; //F_INIT(t_AvpReadFile *);
//   extern "C" t_AvpWriteFile *_AvpWriteFile; //F_INIT(t_AvpReadFile *);

BOOL LoadIO()
 {

  _AvpReadFile          =NULL;//AvpReadFile;
  _AvpWriteFile         =AvpWriteFile;
  _AvpCreateFile        =AvpCreateFile;
  _AvpCloseHandle       =AvpCloseHandle;
  _AvpGetFileSize       =AvpGetFileSize;

  return 1;
 }

