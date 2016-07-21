#if (defined(__WATCOMC__) && (defined(__DOS__)||defined(__OS2__)))
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
#include "..\locale\resource.h"
#include "avp_os2.h"
#include "profile.h"
#include "start.h"
#include "disk.h"
#include "..\\tchar.h"
#include "..\\typedef.h"
#include "../../Common.Fil/ScanAPI/BaseAPI.h"
#include "../../Common.Fil/ScanAPI/avpmem.h"

#define MAKEWORD(low, high) ((WORD)((((WORD)(high)) << 8) | ((BYTE)(low))))
#define CARRY_FLAG  0x0001

DWORD SectorSize=512;

//SimulateRM_Int()
//Allows protected mode software to execute real mode interrupts such
//as calls to DOS TSRs, DOS device drivers, etc.
//
//This function implements the "Simulate Real Mode Interrupt" function
//of the DPMI specification v0.9.
//
//Parameters
//   bIntNum      Number of the interrupt to simulate
//   lpCallStruct Call structure that contains params (register values) for
//                bIntNum.
//
//Return Value
//   SimulateRM_Int returns TRUE if it succeeded or FALSE if it failed.
//
//Comments
//   lpCallStruct is a protected-mode selector:offset address, not a
//   real-mode segment:offset address.
//BOOL _SimulateRMInt(UCHAR Int,RMI far* rmi)
// {
//  union REGPACK regs;
//
//  // Use DMPI call 300h to issue the DOS interrupt
//  //push di
//  memset(&regs,0,sizeof(REGPACK));
//  regs.w.ax =0x0300;        //DPMI Simulate Real Mode Int
//  regs.h.bl =Int;           //Number of the interrupt to simulate
//  regs.h.bh =0;             //Bit 0 = 1; all other bits must be 0
//  regs.w.cx =0;             //No words to copy
//  regs.w.es =FP_SEG(rmi); .w.es=
//  regs.x.edi=FP_OFF(rmi);
//  intr(0x31,&regs);
//  if(((regs.w.flags&0x1)==0)&&((rmi->flags&0x1)==0))
//    return TRUE; //CF clear if successful
//  return FALSE; //CF set on error
//  //pop di
// }

//BOOL pmAllocMem(USHORT Size,short *selector,short *segment)
// {// DPMI call 100h allocates DOS memory
//  union REGPACK regs;
//  memset(&regs,0,sizeof(REGPACK));
//  regs.w.ax=0x0100;
//  regs.w.bx=(WORD)((Size>>4)+1);   //количество 16 битовых параграфов
//  intr(0x31,&regs);
//  if((regs.w.flags&0x1)==0)
//   {
//    *segment=regs.w.ax;
//    *selector=regs.w.dx;
//    return TRUE;
//   }
// else
//   return FALSE;
// }

//------------ string -------------------
extern "C" int _vsprintf(char *__s,const char *__format, __va_list __arg );

#include <stdarg.h>
//#include <varargs.h>
int _VSPrintf(char far *buf, const char *fmt, va_list args);
int _vsprintf(char *__s,const char *__format, __va_list __arg)
 {
  return _VSPrintf(__s,__format,__arg );
 }

char* __stdcall AvpStrrchr(char* str_,int chr_)
 {
  return strrchr(str_,chr_);
 }

char* __stdcall AvpStrlwr(char* str_) {return strlwr(str_);}

char* __stdcall AvpConvertChar(char* str_,int ct, char* cp_)
 {
  str_=str_; ct=ct; cp_=cp_;
  return str_;
 }

extern char scan_goes; // продолжать ли процесс сканирования
extern int  ExitCode;
char *RotateSymbols[]={"|","/","-","\\"};
VOID AvpSleep(DWORD ms_)
 {
//printf("!Sleep\r\n");
  static int r=0;
  static int t=0,tt=0;
  char Chr;

  t++;
  if(t>100)
   { // --- begin code block ------
    t=0;
    if(cmdline.abort_disable!=1)
     if(kbhit())
      if(getch()==27)
       if(scan_goes!=0)
        {
         _puts("\n");
         _puts(LoadString(IDS_SCANPROC_QUESTION,"Cancel scan process?"));
         _puts(" (Yes/No)");
         //cprintf("\r\nCancel scan process? (Yes/No)");
         SetCursorOff(TRUE);
         Chr=(char)getch();
         cprintf("\r\n");
         if((Chr=='y')||(Chr=='Y'))
          {
           prf_data.ShowOK=0;
           ExitCode=1;
           scan_goes=0;
           BAvpCancelProcessObject(1);
          }
         SetCursorOff(FALSE);
        }
   } // --- end code block ------

  tt++;
  if(tt>30)
   {
    putch(0x08);
    putch(*RotateSymbols[r++]);
    if(r>3) r=0;
    tt=0;
   }

//  if(GetServerUtilization()>90)
//   {
//     if(dUtil==0) delay(1);
//     else delay(dUtil*dUtil*2);
//   }
//  if(Fl_Exit==0xff || Fl_Cancel==1) BAvpCancelProcessObject(1);
  ms_=ms_;
 }

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
     if(rc==ERROR_TOO_MANY_OPEN_FILES) cprintf("Error file open (too many open files)\r\n"); //errno = EMFILE;
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

DWORD WINAPI AvpSetFilePointer(
    HANDLE hFile,                // handle of file
    LONG   lDistanceToMove,      // number of bytes to move file pointer
    PLONG  lpDistanceToMoveHigh, // address of high-order word of distance to move
    DWORD  dwMoveMethod)         // how to move
 {
  int off;
  if(hFile==(HANDLE)-1) return FALSE;
  //printf("(seek) h=%d toMove=%d method=%d \n",hFile,lDistanceToMove,dwMoveMethod);
  off=lseek((int)hFile,lDistanceToMove,dwMoveMethod);
  //cprintf("SetFilePointer off=%d\r\n",off);
  lpDistanceToMoveHigh=lpDistanceToMoveHigh;
  return off;
 }

BOOL WINAPI AvpSetEndOfFile(HANDLE hFile)       // handle of file
 {
  if(hFile==(HANDLE)-1) return FALSE;
  long size=tell((int)hFile);
  if(chsize((int)hFile,size)==0) return TRUE;
  else return FALSE;
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

BOOL WINAPI AvpGetDiskFreeSpace(
    LPCTSTR lpRootPathName,         // address of root path
    LPDWORD lpSectorsPerCluster,    // address of sectors per cluster
    LPDWORD lpBytesPerSector,       // address of bytes per sector
    LPDWORD lpNumberOfFreeClusters, // address of number of free clusters
    LPDWORD lpTotalNumberOfClusters)// address of total number of clusters
 {
  unsigned err=0;
  struct diskfree_t diskInfo;

//  LONG DiskSpace=0,ID;
//  char buf[256],vol[16];

//  if(Fl_Remote)
//   {
//    getcwd(buf,255);
//    ID=SetCurrentFileServerID(0);
//   }

  if(lpRootPathName==NULL)
   if(_dos_getdiskfree(tolower(lpRootPathName[0])-'a'+1,&diskInfo)==0)
    {
     *lpSectorsPerCluster=diskInfo.sectors_per_cluster;
     *lpBytesPerSector=diskInfo.bytes_per_sector;
     *lpNumberOfFreeClusters=diskInfo.avail_clusters;
     *lpTotalNumberOfClusters=diskInfo.total_clusters;
     return TRUE;
    }
  return FALSE;
//  if(Fl_Remote)
//   {
//    SetCurrentFileServerID(ID);
//    chdir(buf);
//   }
 }

DWORD WINAPI AvpGetFullPathName(
    LPCTSTR lpFileName, // address of name of file to find path for
    DWORD nBufferLength,// size, in characters, of path buffer
    LPTSTR lpBuffer,    // address of path buffer
    LPTSTR *lpFilePart) // address of filename in path
 {
  DWORD l;
  strncpy(lpBuffer,lpFileName,nBufferLength);
  l=strlen(lpBuffer);
  *lpFilePart=AvpStrrchr(lpBuffer,'\\');
  if(*lpFilePart) (*lpFilePart)++;
  else            *lpFilePart=lpBuffer; //+ l;
  //cprintf("FullFileName: %s %d %s %s\r\r",lpFileName,nBufferLength,lpBuffer,*lpFilePart);
  return l;
 }

DWORD GetFileAttr(char* lpFileName)
 {
  union REGPACK regs;
  memset(&regs,0,sizeof(regs));
  regs.w.ax=0x4300;
  regs.x.edx=FP_OFF(lpFileName);
  regs.w.ds=FP_SEG(lpFileName);
  intr(0x21,&regs);
  if((regs.w.flags&0x1)==0)
   {//CF clear if successful
    //cprintf("GetFileAttrib(int): %x\r\n",regs.w.cx);
    return regs.w.cx;
   }
  else
   {//CF set on error
    //cprintf("GetFileAttrib(int): Error %s\r\n",lpFileName);
    return FALSE;
   }
 }

BOOL SetFileAttr(char* lpFileName,WORD FileAttr)
 {
  union REGPACK regs;
  memset(&regs,0,sizeof(regs));
  regs.w.ax=0x4301;
  regs.x.edx=FP_OFF(lpFileName);
  regs.w.ds=FP_SEG(lpFileName);
  regs.w.cx=FileAttr;
  intr(0x21,&regs);
  if((regs.w.flags&0x1)==0)
   {//CF clear if successful
    //cprintf("GetFileAttrib(int): %x\r\n",regs.w.cx);
    return TRUE;
   }
  else
   {//CF set on error
    //cprintf("GetFileAttrib(int): Error %s\r\n",lpFileName);
    return FALSE;
   }
 }

BOOL __stdcall AvpDeleteFile(LPCTSTR lpFileName) // pointer to name of file to delete
 {
  //chmod(lpFileName,0);
//  SetFileAttr((char*)lpFileName,(WORD)GetFileAttr((char*)lpFileName)&(~_A_RDONLY));
  if(unlink(lpFileName)==0) return TRUE;
  else return FALSE;
 }

BOOL WINAPI AvpMoveFile(
        LPCTSTR lpExistingFileName,// address of name of the existing file
        LPCTSTR lpNewFileName)     // address of new name for the file
 {
  if(rename(lpExistingFileName,lpNewFileName)==(-1))
   {
    if(copyfile((char*)lpExistingFileName,(char*)lpNewFileName)==0)
     if(unlink(lpExistingFileName)==0) return TRUE;
    return FALSE;
   }
  return TRUE;
 }

BOOL WINAPI AvpSetFileAttributes(
        LPCTSTR lpFileName,        // address of filename
        DWORD dwFileAttributes)    // address of attributes to set
 {
  return SetFileAttr((char*)lpFileName,(WORD)dwFileAttributes);
 }

DWORD WINAPI AvpGetFileAttributes(
        LPCTSTR lpFileName)      // address of the name of a file or directory
 {
  return GetFileAttr((char*)lpFileName);
 }

DWORD WINAPI AvpGetTempPath(
        DWORD nBufferLength,  // size, in characters, of the buffer
        LPTSTR lpBuffer)      // address of buffer for temp. path
 {
  char *path;
  if((path=getenv("TEMP"))==NULL)
   if((path=getenv("TMP"))==NULL)
    //strncpy(path,"c:",nBufferLength);
    return FALSE;
  strncpy(lpBuffer,path,nBufferLength);
  lpBuffer[strlen(path)]=0;
  if(lpBuffer[strlen(lpBuffer)-1]=='\\')
   lpBuffer[strlen(lpBuffer)-1]=0;
  //printf("GettempPathName %s\n",lpBuffer);
  return strlen(lpBuffer);
 }

unsigned long rand_m(unsigned long rand0)
 {
  unsigned long r0;
  rand0=rand0<<1;
  r0=rand0 & 0x00400400l;
  if( (r0!=0x00000400l) ^ (r0!=0x00400000l) ) rand0++;
  return rand0;
 }

BOOL GetSystemTime(DWORD *MSec)
 {
  union REGPACK regs;
  memset(&regs,0,sizeof(regs));
  regs.h.ah=0x00;
  intr(0x1a,&regs);
  if((regs.w.flags&0x1)==0)
   {//CF clear if successful
    MSec[0]=regs.w.cx;
    MSec[1]=regs.w.dx;
    return TRUE;
   }
  else
   {//CF set on error
    MSec[0]=MSec[1]=0;
    return FALSE;
   }
 }

void gen_name(char *text)
 {
  unsigned long name,ver;
  int    i;
  char   text1[11];

  DWORD  mSec[2];

  GetSystemTime(mSec);
  name=mSec[0];
  // if(old_millisec==name) name++;
  // old_millisec=name;
  for(i=0;i<strlen(text);i++) ver+=text[i];
  ver=ver&0x000000ff;
  for(i=0;i<ver;i++) name=rand_m(name);

  sprintf(text1,"%08lu",name);
  for(i=0;i<8;i++) text[i]=text1[i];
  sprintf(&text[8],"%s",".tmp");
  text[12]=0;
 }

static DWORD old_millisec=0;
UINT WINAPI AvpGetTempFileName(
        LPCTSTR lpPathName,     // address of directory name for temporary file
        LPCTSTR lpPrefixString, // address of filename prefix
        UINT uUnique,           // number used to create temporary filename
        LPTSTR lpTempFileName)  // address of buffer that receives the new filename
 {
  CHAR TempStr[13];
  int i;

  DWORD  mSec[2];

  if(old_millisec==0)
   {
    GetSystemTime(mSec);
    old_millisec=mSec[0];
   }
  else old_millisec++;
  sprintf(TempStr,"%08lx",old_millisec);
  strcat(&TempStr[8],".tmp");
  for(i=0;i<strlen(lpPrefixString);i++) TempStr[i]=lpPrefixString[i];
  sprintf(lpTempFileName,"%s\\%s",lpPathName,TempStr);
  if(access(lpTempFileName,0)!=-1)
   do
    {
     gen_name(TempStr);
     for(i=0;i<strlen(lpPrefixString);i++) TempStr[i]=lpPrefixString[i];
     sprintf(lpTempFileName,"%s\\%s",lpPathName,TempStr);
    //if(uUnique!=NULL) return uUnique;
    }while(access(lpTempFileName,0)!=-1);
  return uUnique;
 };

//----------------- memory and disk-----------------------
void* _AvpMemAlloc( unsigned int cb )
 {
  void *t;
  t=NULL;
  t=Malloc(cb+1);
  if(t==NULL) cprintf("Error in memalloc size: %d\r\n",cb);
  return t;
 }

void  _AvpMemFree( void* buf)
 {
  if(buf!=NULL) //cprintf("Error in memfree\r\n");
  Free(buf);
 }

BOOL WINAPI AvpGetDiskParam(BYTE disk,BYTE drive, WORD* CX,BYTE* DH)
 {//INT 13 - DISK - GET DRIVE PARAMETERS (PC,XT286,CONV,PS,ESDI,SCSI)
  RMI   rmi;

  if(CX)*CX=0;
  if(DH)*DH=0;

  memset(&rmi,0,sizeof(RMI));
  rmi.EAX = 0x0800;              // 0x08;
  rmi.EDX = disk;                //dl=disk; //(bit 7 set for hard disk)
  if(SimulateRMInt(0x13,&rmi))
                                 //CF clear if successful (AH = status (07h) )
   {                             //AH = 00h
    *CX=(WORD)rmi.ECX;           //ch low eight bits of maximum cylinder number
                                 //cl maximum sector number (bits 5-0)
                                 //   high two bits of maximum cylinder number (bits 7-6)
    *DH=(BYTE)(rmi.EDX/0x100);   //dh = maximum head number
                                 //DL = number of drives
                                 //ES:DI -> drive parameter table (floppies only)
    //printf("GetDiskParam: disk:%x drive:%x cx:%x dx:%x \r\n",disk,drive,*CX,*DH);
    return TRUE;
   }
  else
   {
    //printf("GetDiskParam: disk:%x drive:%x ax:%x\r\n",disk,drive,rmi.EAX);//Regs.w.ax);
//    if(disk<0x20)
     {
      BYTE Buffer[0x200];
      //if(Disk & 0x80)
      // {
      //  drive=Disk-0x80+2; //2-c:\ 3-d:\
      // }

      if(_AvpRead25(drive,0,1,Buffer)==TRUE)
       {//only supports int25; DH, CX are 0.
        //printf("GetDiskParam25:Yes\r\n");
        return TRUE;
       }
      else
       {//drive not ready.
        //printf("GetDiskParam25:No\r\n");
        return FALSE;
       }
     }
   }
 }

BYTE _LockPhysical(BYTE Disk,BYTE Level)
 {
  RMI   rmi;

  memset(&rmi,0,sizeof(RMI));
  rmi.EAX = 0x440d;
  rmi.EBX = MAKEWORD(Disk,Level); //lock level
  rmi.ECX = 0x084b;
  rmi.EDX = (Level==1)?1:0;
  return (BYTE)SimulateRMInt(0x21,&rmi);
 }

BOOL _UnLockPhysical(BYTE Disk)
 {
  RMI   rmi;

  memset(&rmi,0,sizeof(RMI));
  rmi.EAX = 0x440d;
  rmi.EBX = MAKEWORD(Disk,0); //lock level
  rmi.ECX = 0x086b;
  return SimulateRMInt(0x21,&rmi);
 }

int LockPhysical(BYTE Disk,BYTE Fn)
 {
  union REGPACK regs;

  memset(&regs,0,sizeof(REGPACK));
  regs.w.ax=0x4a33;
  intr(0x2f,&regs);
  if(regs.w.ax==0)
   {//Dos 7.0
    if(_LockPhysical(Disk,1)==0) return 0;
    if(_LockPhysical(Disk,2)==0) return 1;
    if(Fn==0x03)
      if(_LockPhysical(Disk,3)!=0) return 3;
    return 2;
   }
  else return -1;
 }

void UnLockPhysical(BYTE Disk,BYTE Fn,int Lock)
 {
  union REGPACK regs;

  memset(&regs,0,sizeof(REGPACK));
  regs.w.ax=0x4a33;
  intr(0x2f,&regs);
  if(regs.w.ax==0)
   {//Dos 7.0
    switch(Lock)
     {
      case 3:
        if(Fn==3) _UnLockPhysical(Disk);
      case 2:
        _UnLockPhysical(Disk);
      case 1:
        _UnLockPhysical(Disk);
        break;
     }
   }
 }

BOOL _LockLogical(BYTE Drive,BYTE Level)
 {
  RMI   rmi;

  memset(&rmi,0,sizeof(RMI));
  rmi.EAX = 0x440d;
  rmi.EBX = MAKEWORD(Drive+1,Level); //lock level
  rmi.ECX = 0x084a;
  rmi.EDX = (Level==1)?1:0;
  return SimulateRMInt(0x21,&rmi);
 }

BOOL _UnLockLogical(BYTE Drive)
 {
  RMI   rmi;

  memset(&rmi,0,sizeof(RMI));
  rmi.EAX = 0x440d;
  rmi.EBX = MAKEWORD(Drive+1,0); //unlock level
  rmi.ECX = 0x086a;
  return SimulateRMInt(0x21,&rmi);
 }

int LockLogical(BYTE Drive,BYTE Int)
 {
  union REGPACK regs;

  memset(&regs,0,sizeof(REGPACK));
  regs.w.ax=0x4a33; //122f 4010
  intr(0x2f,&regs);
  if(regs.w.ax==0)
   {
    if(_LockLogical(Drive,1)==0) return 0;
    if(_LockLogical(Drive,2)==0) return 1;
    if(Int==0x26)
      if(_LockLogical(Drive,3)!=0) return 3;
    return 2;
   }
  else
   return -1;
 }

void UnLockLogical(BYTE Drive,BYTE Int,int Lock)
 {
  union REGPACK regs;

  memset(&regs,0,sizeof(REGPACK));
  regs.w.ax=0x4a33; //122f 4010
  intr(0x2f,&regs);
  if(regs.w.ax==0)
   {
    switch(Lock)
     {
      case 3:
        if(Int==0x26) _UnLockLogical(Drive);
      case 2:
        _UnLockLogical(Drive);
      case 1:
        _UnLockLogical(Drive);
        break;
     }
   }
 }

//  Calls DPMI to call the BIOS Int 13h Read Track function to read the
//  first physical sector of a physical drive. This function is used to
//  read partition tables, for example.

//  Parameters
//     Disk
//        The Int 13h device unit,
//           0x00 for floppy drive 0
//           0x00 for floppy drive 1
//           0x80 for physical hard disk 0
//           0x81 for physical hard disk 1
//           etc.
//
//     Buffer
//        Pointer to a buffer that receives the sector data.  The buffer
//        must be at least SECTOR_SIZE bytes long.
//
//  Return Value
//     Returns TRUE if the first sector was read into the buffer pointed
//     to by Buffer, or FALSE otherwise.
//
//  Assumptions
//     Assumes that sectors are at least SECTOR_SIZE bytes long.
BOOL IO13(BYTE Fn,BYTE Disk,WORD Sector,BYTE Head,WORD NumSectors,LPBYTE Buffer)
 {
  int   ntry=5,Lock;
  BOOL  ret=FALSE;
  RMI   rmi;
  USHORT selector,segment;
  char  far *str;

  //intr(0x3,&regs);

  if(Buffer==NULL) return FALSE;
  // DPMI call 100h allocates DOS memory
  //
  // Allocate the buffer that the Int 13h function will put the sector
  // data into. As this function uses DPMI to call the real-mode BIOS, it
  // must allocate the buffer below 1 MB, and must use a real-mode
  // paragraph-segment address.
  //
  // After the memory has been allocated, create real-mode and
  // protected-mode pointers to the buffer. The real-mode pointer
  // will be used by the BIOS, and the protected-mode pointer will be
  // used by this function because it resides in a Windows 16-bit DLL,
  // which runs in protected mode.

  if(pmAllocMem((USHORT)SectorSize,&selector,&segment))
   {
    // Move string to DOS real-mode memory
    str=(char far*)(((USHORT)(selector)):>((void __near *)(0)));

    Lock=LockPhysical(Disk,(BYTE)Fn);

ntry:
    if(Fn==0x03) // Move string to DOS real-mode memory
     _fmemcpy(str,(char*)Buffer,SectorSize*NumSectors);

    //_printf("int Read13 before%x\n");
    memset(&rmi,0,sizeof(RMI));
    // Set up real-mode call structure
    //.edx = MAKEWORD(Disk,Head);   // Head 0, Drive #
    rmi.EDX=Head*0x100+Disk;//&0xff//dh= head number
                            //dl= drive number (bit 7 set for hard disk)
    //rmi.ch=               //low eight bits of cylinder number
    //rmi.cl=               //sector number 1-63 (bits 0-5)
                            //high two bits of cylinder(bits 6-7, hard disk only)
    rmi.ECX=Sector; //&0xff // Sector 1, Cylinder 0
    //.eax = MAKEWORD(NumSectors,Fn);
    rmi.EAX=MAKEWORD(NumSectors,Fn);//0x0200+(BYTE)NumSectors;//number of sectors to read (must be nonzero)
    rmi.EBX=0;              //FP_OFF(Buffer); //data buffer
    rmi.ES=segment;         //FP_SEG(Buffer);

    if(SimulateRMInt(0x13,&rmi))              //&&(regs.h.ah==0x00)
     {//CF clear if successful
      //AH=status (see #0142)
      //AL=number of sectors transferred (only valid if CF set for some BIOSes)
      if(Fn==0x02)
       _fmemcpy((char*)Buffer,str,SectorSize*NumSectors);
      //_printf("int Read13 Ok:\n");//%x\n",regs.w.ax);
//      if(Fn==8)
//       {
//        *(WORD*)Buffer=(WORD)(callStruct.ecx);
//        Buffer[2]=((BYTE*)&(callStruct.edx))[1];
//       }
      ret=TRUE;
     }
    else
     {//CF set on error
      //_printf("int Read13 Error\n");
      //для дисковвводов(dribe<0x80) можно повторить папу раз?
      //if AH=11h (corrected ECC error), AL = burst length
      //printf("int Read13 Error:%x\n",regs.w.ax);
      ret=FALSE;
     }

    if(ret==FALSE)
     {
      memset(&rmi,0,sizeof(RMI));
      rmi.EAX=0;
      rmi.EDX=MAKEWORD(Disk,0);   // Head 0, Drive #
      SimulateRMInt(0x13,&rmi);
      if(--ntry)goto ntry;
     }

    if(Lock>0) UnLockPhysical(Disk,(BYTE)Fn,Lock);
    pmFreeMem(selector);
   }
  return ret;
 }

BOOL __stdcall AvpRead13(BYTE Disk,WORD Sector,BYTE Head,WORD NumSectors,LPBYTE Buffer)
 {//INT 13 - DISK - READ SECTOR(S) INTO MEMORY
  return IO13(0x02,Disk,Sector,Head,NumSectors,Buffer);
 }

BOOL __stdcall AvpWrite13(BYTE Disk,WORD Sector,BYTE Head,WORD NumSectors,LPBYTE Buffer)
 {//INT 13 - DISK - WRITE DISK SECTOR(S)
  return IO13(0x03,Disk,Sector,Head,NumSectors,Buffer);
 }

typedef struct _ReqPacket
 {
  ULONG   sector;
  USHORT  nsectors;
//  ULONG  nsectors;
  union
   {
    char*   buffer;
    struct
     {
      USHORT offset;
      USHORT segment;
     };
   };
 } ReqPacket;

#define FS_DOS   1
#define FS_FAT   2
#define FS_FAT32 3
#define FS_NTFS  4

typedef struct _INT21FUNC6F
 {
  BYTE  inLen;      //(call) length of this buffer (in bytes)
  BYTE  outLen;     //(ret) number of bytes in parameter block actually used
  BYTE  DriveFlags; //(ret) drive flags (see #0879)
  BYTE  DriveNumber;//(ret) physical drive number
                    //  00h-7Fh floppy
                    //  80h-FEh hard
                    //  FFh no physical drive
  DWORD DriveMap;   //(ret) bitmap of logical drives associated with physical drive
                    //  bit 0 = drive A:, etc.
  QWORD StartBlock; //(ret) relative block address of partition start
 } INT21FUNC6F;
/*
BOOL GetStartBlock(BYTE Drive,BYTE *Disk,QWORD* StartBlock)
 {
  BOOL   ret=FALSE;
  RMI    rmi;
  USHORT  RPselector,RPsegment;
  INT21FUNC6F far *ReqPack;
//  USHORT DosDataSegm;

//  memset(&rmi,0,sizeof(RMI));
//  rmi.EAX=0x1203;
//  if(SimulateRMInt(0x2f,&rmi))
//  DosDataSegm=rmi.DS;

  if(pmAllocMem(sizeof(INT21FUNC6F),&RPselector,&RPsegment))
   {
    memset(&rmi,0,sizeof(RMI));
//    rmi.EAX=0x122b;
//    rmi.EBP=0x440D;
//    rmi.SS =DosDataSegm;
    rmi.EAX=0x440D;
    rmi.ECX=0x086f;
    rmi.EBX=Drive+1; //BL = drive number (00h=default,01h=A:,etc)

    ReqPack=(INT21FUNC6F far*)(((USHORT)(RPselector)):>((void __near *)(0)));
    ReqPack->inLen=sizeof(INT21FUNC6F);
    rmi.EDX=0;
    rmi.DS =RPsegment;
//    if(SimulateRMInt(0x2f,&rmi))
    if(SimulateRMInt(0x21,&rmi))
     {//CF clear if successful
      //cprintf("%x DriveNumber:%x DriveMap:%x StartBlock:%x \r\n",
      //   Drive,ReqPack->DriveNumber,ReqPack->DriveMap,ReqPack->StartBlock);
      *Disk=ReqPack->DriveNumber;
      StartBlock->ulLo=ReqPack->StartBlock.ulLo;
      StartBlock->ulHi=ReqPack->StartBlock.ulHi;
      ret=TRUE;
     }
    else
     {
      //cprintf("int:%x(FAT) Error\r\n",Int);
      ret=FALSE;
     }
    pmFreeMem(RPselector);
   }
  return ret;
 }

BOOL Int25To13(BYTE Int,BYTE Drive,DWORD Sector,WORD NumSectors,LPBYTE Buffer)
 {
  RMI  rmi;
  BYTE  Disk;
  QWORD StartBlock;

  if(GetStartBlock(Drive,&Disk,&StartBlock))
   {
    memset(&rmi,0,sizeof(RMI));
    rmi.EAX = 0x0800;
    rmi.EDX = Disk;
    if(SimulateRMInt(0x13,&rmi))
     {
      DWORD Cylinders,Heads,SectorsPerTrack;

      Cylinders=((rmi.ECX&0xc0)<<2)+(rmi.ECX>>8);
      Heads=rmi.EDX/0x100;
      SectorsPerTrack=rmi.ECX&0x3f;

      if(Cylinders>1023)
       {
        int  x,y;
        //cprintf("Cylinders too high %d (more then 1024)\r\n",Cylinders+1);
        y=15-10;
        if(Cylinders<((1<<14)-1))//16384-1
          y=14-10;
        if(Cylinders<((1<<13)-1))//8192-1
          y=13-10;
        if(Cylinders<((1<<12)-1))//4096-1
          y=12-10;
        if(Cylinders<((1<<11)-1))//2048-1
          y=11-10;
        x=(1<<y)-1;

        Cylinders = Cylinders >> y;    //+1         //  логический номеp цилиндpа
        Heads = (Heads + x*(Heads+1));      //  логический номеp головки
       }

      Heads++;

      BYTE Sect,Head;
      WORD Cyl;//,CylLo,CylHi; // Sect;
      ULONG Start;

      Start=StartBlock.ulLo+Sector;

      Sect=(BYTE)((Start % (LONG)SectorsPerTrack) + 1);  //+1 ?
      Head=(BYTE)((Start /SectorsPerTrack) % Heads);
      Cyl =(WORD)(Start /(SectorsPerTrack*Heads));
//    Cyl =MAKEWORD(HIBYTE(Cyl),LOBYTE(Cyl));
//    CylLo=LOBYTE(Cyl);
//    CylHi=HIBYTE(Cyl);
      Cyl=(WORD)( (Cyl<<8)+((Cyl&0x300)>>2)+Sect);
//    CylLo<<=6;
//    CylLo|=Sect;

      return IO13((Int==0x26)? 0x03: 0x02,Disk,Cyl,Head,NumSectors,Buffer);
     }
   }
  return FALSE;
 }

ULONG GetFSType(BYTE Drive)
 {
  ULONG  retFS=FS_DOS;
  RMI    rmi;
  char   DrName[]=" :\\";
  USHORT RootSelector,RootSegment,NameSelector,NameSegment;
  if(pmAllocMem(10,&RootSelector,&RootSegment))
   {
    DrName[0]=(char)(Drive+'A');
    _fstrcpy((char far*)(((USHORT)(RootSelector)):>((void __near *)(0))),DrName);
    if(pmAllocMem(10,&NameSelector,&NameSegment))
     {
      memset(&rmi,0,sizeof(RMI));
      rmi.EAX= 0x71A0;
      rmi.DS = RootSegment; //DS:SI -> ASCIZ root name (e.g. "C:\")
      rmi.ESI= 0;
      rmi.ES = NameSegment; //ES:DI -> buffer for file system name
      rmi.EDI= 0;
      rmi.ECX= 10;          //size of ES:DI buffer
      if(SimulateRMInt(0x21,&rmi))
       {
        char far*FSType=(char far*)(((USHORT)(NameSelector)):>((void __near *)(0)));
//        _fstrcpy(FSType,
//           (char far*)(((USHORT)(NameSelector)):>((void __near *)(0))));
        if(_fstrcmp(FSType,"FAT")==0)
         {
          cprintf("FileSystem: FAT\r\n");
          retFS=FS_FAT;
         }
        else
         if(_fstrcmp(FSType,"FAT32")==0)
          {
           cprintf("FileSystem: FAT32\r\n");
           retFS=FS_FAT32;
          }
         else
          if(_fstrcmp(FSType,"NTFS")==0)
           {
            cprintf("FileSystem: NTFS\r\n");
            retFS=FS_NTFS;
           }
          else cprintf("FileSystem: ?\r\n");
       }
      else cprintf("GetFileSystem: %s Error\r\n",DrName);
      pmFreeMem(NameSelector);
     }
    pmFreeMem(RootSelector);
   }
  return retFS; //FSType;
 }
*/
BOOL RMCallInt(BYTE Int, RMI far *rmi)
 {
  union REGS regs;
  struct SREGS sregs;
  regs.x.eax=0x0200;
  regs.x.ebx=Int;
  int386(0x31,&regs,&regs);
  if(regs.x.cflag & INTR_CF)
          return FALSE;
  rmi->CS=regs.w.cx;
  rmi->IP=regs.w.dx;
  regs.x.eax=0x0302;
  regs.x.ebx=regs.x.ecx=0;
  sregs.ds=sregs.es=FP_SEG(rmi);
  regs.x.edi=FP_OFF(rmi);
  int386x(0x31,&regs,&regs,&sregs);
  if((regs.x.cflag & INTR_CF) || (rmi->flags & INTR_CF))
   {
    return FALSE;
   }
  return TRUE;
 }

BOOL IO2x(BYTE Int,BYTE Drive,DWORD Sector,WORD NumSectors,LPBYTE Buffer)
 {
  BOOL   ret=FALSE;
  RMI    rmi;
  USHORT  selector,segment,RPselector,RPsegment;
  ReqPacket far *ReqPack;
  char      far *str;
  int    Lock;
  ULONG  Type=FS_DOS;//GetFSType(Drive);

  if(NumSectors>1) return FALSE;

  //cprintf("Win version: %d.%d\r\n",_osminor,_osmajor); //0x40a Win98
  if((_osmajor==7)&&(_osminor>=10)) Type=FS_FAT32;
  //ResetDrive(Drive);
  //memset(&rmi,0,sizeof(RMI));
  //rmi.EAX=0x160a;
  //if(SimulateRMInt(0x2f,&rmi))
  // {
  //  cprintf("Win version: %d.%d\r\n",(rmi.EBX&0xff00)>>8,rmi.EBX&0x00ff); //0x40a Win98
  //  switch (rmi.EBX)
  //   {
  //    case 0x40a:
  //      Type=FS_FAT32;
  //      break;
  //    case 0x400:
  //    default:
  //      Type=FS_FAT;
  //      break;
  //   }
//    if((rmi.EBX&0xff00)==0x400)
//     {
//      if((Drive==0)||(Drive==1)) return FALSE; else
//      return Int25To13(Int,Drive,Sector,NumSectors,Buffer);
//     }
  // }

  if(pmAllocMem((USHORT)SectorSize,&selector,&segment))
   {
    if(pmAllocMem(sizeof(ReqPacket),&RPselector,&RPsegment))
     {
      // Move string to DOS real-mode memory
      str=(char far*)(((USHORT)(selector)):>((void __near *)(0)));
      if(Int==0x26) _fmemcpy(str,(char*)Buffer,SectorSize);
      ReqPack=(ReqPacket far*)(((USHORT)(RPselector)):>((void __near *)(0)));

      Lock=LockLogical(Drive,Int);

      ReqPack->sector=Sector;
      ReqPack->nsectors=NumSectors;
      ReqPack->segment=segment;
      ReqPack->offset=0;
      memset(&rmi,0,sizeof(RMI));
      rmi.EBX=0;        //LOWORD(RMlpPacket); //FP_OFF(&ReqPack);
      rmi.DS =RPsegment;//HIWORD(RMlpPacket); //FP_SEG(&ReqPack);     //DS:BX -> buffer for data
      switch (Type)
       {
        case FS_DOS:
        case FS_FAT:
          rmi.ECX=0xffff;
          rmi.EAX=Drive;     //al drive number (00h = A:, 01h = B:, etc)
//          _printf("io2x (Fat) %x: %d %d %d\r\n",Int,Drive,Sector,NumSectors);
          ret=SimulateRMInt(Int,&rmi);              //&&(regs.h.ah==0x00)
          break;
        case FS_FAT32:
          //if(Int==0x26) if(!LockLogical(Drive)) return FALSE;
          rmi.EAX=0x7305;
          rmi.ECX=0xFFFFFFFF;
          rmi.EDX=Drive+1;
          rmi.ESI=(Int==0x26)? 1: 0;
//          _printf("io2x (Fat32) int(7305):%x \r\n",Int);
          if(!RMCallInt(0x21,&rmi)) ret=SimulateRMInt(0x21,&rmi);
          else  ret=TRUE;
          //if(Int==0x26) UnLockLogical(Drive);
          break;
       }
      if(ret)
       {//CF clear if successful  //AH = 00h
//        cprintf("int :%x(FAT) Ok\r\n",Int);
        if(Int==0x25) _fmemcpy((char*)Buffer,str,SectorSize*NumSectors);
       }
//      else
//       cprintf("int:%x(FAT) Error\r\n",Int);

      if(Lock>0) UnLockLogical(Drive,Int,Lock);

      pmFreeMem(RPselector);
     }
    pmFreeMem(selector);
   }
  return ret;
 }

BOOL __stdcall AvpRead25(BYTE Drive,DWORD Sector,WORD NumSectors,LPBYTE Buffer)
 {//INT 25 - DOS 3.31+ - ABSOLUTE DISK READ (>32M hard-disk partition)
  return IO2x(0x25,Drive,Sector,NumSectors,Buffer);
 }

BOOL __stdcall AvpWrite26(BYTE Drive,DWORD Sector,WORD NumSectors,LPBYTE Buffer)
 {//INT 26 - DOS 3.31+ - ABSOLUTE DISK WRITE (>32M hard-disk partition)
  return IO2x(0x26,Drive,Sector,NumSectors,Buffer);
 }

DWORD __stdcall AvpGetDosMemSize(void)
 {
  DWORD memSize=0;
  _AvpMemoryRead(0x413,2,(BYTE*)&memSize);
  memSize<<=10;
  if(memSize>0xA0000)memSize=0xA0000;
  //cprintf("GetDosMemSize: %d\r\n",memSize);
  return memSize; // size of DOS memory in bytes
 }

DWORD __stdcall AvpGetFirstMcbSeg(void)
 {
//  union REGPACK reg;
  RMI    rmi;

  memset(&rmi,0,sizeof(RMI));
  rmi.EAX=0x5200;;
  //reg.w.flags=CARRY_FLAG;

  // Use DMPI call 300h to issue the DOS interrupt
//  memset(&reg,0,sizeof(REGPACK));
//  reg.w.ax =0x0300;
//  reg.h.bl =0x21;
//  reg.h.bh =0;
//  reg.w.cx =0;
//  reg.w.es =FP_SEG(&rmi);
//  reg.x.edi=FP_OFF(&rmi);
//  intr(0x31,&reg);
//  if(((reg.w.flags&0x1)==0)&&((rmi.flags&0x1)==0)) //&&(regs.h.ah==0x00)
  if(SimulateRMInt(0x21,&rmi))
   {//es:bx
    if((rmi.flags&0x1)==0)
     {//cprintf("GetFirstMcbSeg Yes\r\n");
      return (DWORD)*(WORD*)(rmi.ES*0x10+rmi.EBX-2);// TRUE;
     }
    else return FALSE;
   }
  else
   {//cprintf("GetFirstMcbSeg No\r\n");
    return FALSE;
   }
 }

//ppIFSFileHookFunc pPrevHook;

//int _cdecl MyIfsHook(pIFSFunc pfn, int fn, int Drive, int ResType,
//                int CodePage, pioreq pir)
// {
//  return (*pPrevHook)(pfn, fn, Drive, ResType, CodePage, pir);
// }

//static DWORD GetNextIfsApiHook(DWORD &count,DWORD* &hookPtr,DWORD maxCount)
// {
//  PVOID base;
//  char buf[0x80];
//  pIFSFileHookFunc prevHook;

//  if(count>=maxCount)return count;
//  pPrevHook = IFSMgr_InstallFileSystemApiHook(MyIfsHook);
//  if(pPrevHook==0)return count;
//  if(0!=IFSMgr_RemoveFileSystemApiHook(MyIfsHook))return count;

//  prevHook = *pPrevHook;
//  *hookPtr=(DWORD)*pPrevHook;

//  base=GetVxDName((DWORD)*hookPtr,buf);
//  cprintf("AVP_IO: Hook %d %08X  %s\r\n",count,*hookPtr,base?buf:"???");

//  hookPtr++;
//  count++;

//  if(base)
//   {
//    if(0==strnicmp("GK95",buf,4))
//     {
//      if(!IFSMgr_RemoveFileSystemApiHook(prevHook))
//       {
//        GetNextIfsApiHook(count,hookPtr,maxCount);
//        IFSMgr_InstallFileSystemApiHook(prevHook);
//       }
//     }
//    else
   //if(strnicmp("IFSMGR",buf,6))
   //  {
   //   if(!IFSMgr_RemoveFileSystemApiHook(prevHook))
   //    {
   //     GetNextIfsApiHook(count,hookPtr,maxCount);
   //     IFSMgr_InstallFileSystemApiHook(prevHook);
   //    }
   //  }
   // else
//     {
//      DWORD addr;
//      addr=(DWORD)*IFSMgr_InstallFileSystemApiHook;

      //addr=*(DWORD*)(addr+2);
      //*hookPtr=*(DWORD*)addr;

//      _AvpMemoryRead(addr+2,4,(BYTE*)&addr);
//      _AvpMemoryRead(addr,4,(BYTE*)hookPtr);

//      base=GetVxDName((DWORD)*hookPtr,buf);
//      cprintf("AVP_IO: Hook %d %08X  %s\r\n",count,*hookPtr,base?buf:"???");

//      hookPtr++;
//      count++;
//     }
//   }
//  return count;
// }

DWORD __stdcall AvpGetIfsApiHookTable(DWORD* table,
                                   DWORD size) //size in DWORDS !!!!!!!!
 {
  DWORD count=0;
  //cprintf("GetIfsApiHookTable\r\n");
//  GetNextIfsApiHook(count,table, maxCount);
  table=table; size=size;
  return count;
 }

DWORD __stdcall AvpGetDosTraceTable(
       DWORD* table,
       DWORD size) //size in DWORDS !!!!!!!!
 {
  DWORD val;
  DWORD count=0;
  //cprintf("GetDosTraceTable\r\n");
  if(4==_AvpMemoryRead(0x04c,4,(BYTE*)&val)) table[count++]=val;
  if(4==_AvpMemoryRead(0x084,4,(BYTE*)&val)) table[count++]=val;
  if(4==_AvpMemoryRead(0x100,4,(BYTE*)&val)) table[count++]=val;

  size=size;
  return count;
 }

#define PAGENUM(p)  (((ULONG)(p)) >> 12)
#define PAGEOFF(p)  (((ULONG)(p)) & 0xFFF)
#define PAGEBASE(p) (((ULONG)(p)) & ~0xFFF)
#define _NPAGES_(p, k)  ((PAGENUM((char*)p+(k-1)) - PAGENUM(p)) + 1)

DWORD __stdcall AvpMemoryRead(
   DWORD  dwOffset,   // offset
   DWORD  dwSize,     // size in bytes
   LPBYTE lpBuffer)   // pointer to buffer to read to
 {
  DWORD ret=0;
//  DWORD pageNo;
//  DWORD pageCount;
//  DWORD lBase;

//  cprintf("MemoryRead - Offset:%x, Size:%x\r\n",dwOffset,dwSize);
  if(dwOffset>0x100000) goto ret;
//  if(dwOffset>0xC0000000) goto ret;
//  if(dwOffset<=0x100000)
//   {
//    if((dwOffset+dwSize)>0xA0000) return ret;
//    dwOffset+=Get_Cur_VM_Handle()->CB_High_Linear;
//   }
//  if(dwOffset<0xC0000000) goto ret;

//  pageNo=PAGENUM(dwOffset);
//  pageCount=_NPAGES_(dwOffset,dwSize);
//  if(pageCount != PageCheckLinRange(pageNo,pageCount,0)) goto ret;

//  lBase=LinPageLock(pageNo,pageCount,PAGEMAPGLOBAL);
//  if(!lBase) goto ret;
//  if(VWIN32_CopyMem((void*)(lBase + PAGEOFF(dwOffset)),lpBuffer,dwSize,NULL))
//          ret=dwSize;
//  LinPageUnLock(PAGENUM(lBase),pageCount,PAGEMAPGLOBAL);

//if(VWIN32_CopyMem((void*)offset,buffer,size,NULL))
  if(_fmemcpy(lpBuffer,(void*)dwOffset,dwSize)!=NULL)
        ret=dwSize;
ret:
  return ret;
 }

DWORD __stdcall AvpMemoryWrite(
   DWORD  dwOffset,   // offset
   DWORD  dwSize,     // size in bytes
   LPBYTE lpBuffer)   // pointer to buffer to write from
 {
  DWORD ret=0;

  //cprintf("MemoryWrite\r\n");
  if(dwOffset>0x100000) goto ret;
  //if(dwOffset>0xC0000000) goto ret;
  if(_fmemcpy((void*)dwOffset,lpBuffer,dwSize)!=NULL)
        ret=dwSize;
ret:
  return ret;
 }

DWORD WINAPI AvpExecSpecial(char* FuncName,DWORD wParam,DWORD lParam)
{
wParam=wParam;lParam=lParam;
        char buf[0x200];
//        char DllName[0x200];
//        char* lpFilePart;
        if(!FuncName || !*FuncName) return 0;

/*
        AvpGetModuleFileName(NULL,DllName,0x200);
        AvpGetFullPathName(DllName,0x200,DllName,&lpFilePart);
        strcpy(lpFilePart,"AVC*.DLL");

        HINSTANCE hi;
        WIN32_FIND_DATA fd;

        HANDLE fi=FindFirstFile(  DllName, &fd);
        if(fi!=INVALID_HANDLE_VALUE)
        {
                do{
                        strcpy(lpFilePart,fd.cFileName);
                        hi=LoadLibrary(DllName);
                        if(!hi){
                                strcpy(lpFilePart,fd.cAlternateFileName);
                                hi=LoadLibrary(DllName);
                        }
                        if(hi){
                                void* ptr=GetProcAddress(hi,FuncName);
                                if(ptr){
                                        DWORD ret=0;
                                        if( sign_check_file( DllName, 1, vndArray, vndArrayCount, 0) )
                                                _AvpCallback(AVP_CALLBACK_FILE_INTEGRITY_FAILED,(long)DllName);
                                        else
                                                ret=((DWORD (*)(DWORD,DWORD))ptr)(wParam, lParam);
                                        FreeLibrary(hi);
                                        return ret;
                                }
                        }
                        FreeLibrary(hi);
                }while(FindNextFile(fi,&fd));
        }
*/
        sprintf(buf,"ExecSpecial can't resolve name: %s",FuncName);
//      _AvpCallback(AVP_CALLBACK_PUT_STRING,(long)buf);
        return 0;
}

LONG avp_mess(DWORD mp1,DWORD mp2);
LRESULT __stdcall AvpCallback(WPARAM wParam,LPARAM lParam)
 {
  return (LRESULT)avp_mess(wParam,lParam);
 }

BOOL LoadIO()
 {
  #ifdef _MBCS
   setlocale(LC_ALL,"");
   _setmbcp(_MB_CP_LOCALE);
  #endif

   _AvpCallback          =AvpCallback;
   _AvpStrrchr           =AvpStrrchr;
   _AvpStrlwr            =AvpStrlwr;
//   _AvpSleep             =AvpSleep;
   _AvpConvertChar       =AvpConvertChar;

   _AvpRead13            =AvpRead13;
   _AvpWrite13           =AvpWrite13;
   _AvpRead25            =AvpRead25;
   _AvpWrite26           =AvpWrite26;
   _AvpGetDiskParam      =AvpGetDiskParam;

   _AvpReadFile          =AvpReadFile;
   _AvpWriteFile         =AvpWriteFile;
   _AvpCreateFile        =AvpCreateFile;
   _AvpCloseHandle       =AvpCloseHandle;
   _AvpSetFilePointer    =AvpSetFilePointer;
   _AvpSetEndOfFile      =AvpSetEndOfFile;
   _AvpGetFileSize       =AvpGetFileSize;
   _AvpGetDiskFreeSpace  =AvpGetDiskFreeSpace;
   _AvpGetFullPathName   =AvpGetFullPathName;
   _AvpDeleteFile        =AvpDeleteFile;
//   _AvpMoveFile          =AvpMoveFile;
   _AvpSetFileAttributes =AvpSetFileAttributes;
   _AvpGetFileAttributes =AvpGetFileAttributes;
   //AvpGetVersionEx      =AvpGetVersionEx;
   _AvpGetTempPath       =AvpGetTempPath;
   _AvpGetTempFileName   =AvpGetTempFileName;
   _AvpExecSpecial       =AvpExecSpecial;

   _AvpGetDosMemSize     =AvpGetDosMemSize;
   _AvpGetFirstMcbSeg    =AvpGetFirstMcbSeg;
   _AvpGetIfsApiHookTable=AvpGetIfsApiHookTable;
   _AvpGetDosTraceTable  =AvpGetDosTraceTable;
   _AvpMemoryRead        =AvpMemoryRead;
   _AvpMemoryWrite       =AvpMemoryWrite;

   return 1;
 }

