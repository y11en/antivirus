#include <stdlib.h>//"malloc.h"
//#include <conio.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
//#include <share.h>
//#include <io.h>
//#include <direct.h>
//#include <bios.h>
//#include <dos.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "resource.h"

//#ifdef SYSTOS2
#define SYSTOS2
#include "typedef.h"
//#endif
#include "avp_os2.h"
#include "kernel.h"
#include "profile.h"
#include "start.h"
//#include "disk.h"
#include "../ScanAPI/AVPMEM.H"
//#include "../tchar.h"
#include "../TYPEDEF.H"
#include "../ScanAPI/BaseAPI.h"

#define MAKEWORD(low, high) ((WORD)((((WORD)(high)) << 8) | ((BYTE)(low))))
#define CARRY_FLAG  0x0001

//------------ string -------------------
#define __stdcall
//extern "C" int _vsprintf(char *__s,const char *__format, __va_list __arg );

//int _vsprintf(char *str,const char *__format, _G_va_list __arg )
// {
//  return vsprintf(str,__format,__arg);
// }

char* __stdcall AvpStrrchr(char* str_,int chr_)
 {
  return strrchr(str_,chr_);
 }

char* __stdcall AvpStrlwr(char* str_)
 {
 // return strlwr(str_);
  if(str_)
   {
    char *c=str_;
    while(*c)
     {
      *c|=0x20;
      c++;
     }
   }
  return str_;
 }

char* __stdcall AvpConvertChar(char* str_,int ct, char* cp_)
 {
  str_=str_; ct=ct; cp_=cp_;
  return str_;
 }

extern char scan_goes; // продолжать ли процесс сканирования
extern int  ExitCode;
char *RotateSymbols[]={"|","/","-","\\"};
//VOID __stdcall AvpSleep(DWORD ms_)
VOID AvpSleep(DWORD ms_)
 {
  static int r=0;
  static int t=0,tt=0;

  t++;
  if(t>100)
   { // --- begin code block ------
    t=0;
    if(cmdline.abort_disable!=1)
//     if(!feof(stdin))//kbhit())
//      if(getchar()==27)
//       if(scan_goes!=0)
        {
//         char Chr;
//         printf("\n%s (Yes/No)",LoadString(IDS_SCANPROC_QUESTION,"Cancel scan process?"));
//         SetCursorOff(TRUE);
//         Chr=(char)getchar();
//         printf("\n");
//         if((Chr=='y')||(Chr=='Y'))
//          {
//           prf_data.ShowOK=0;
//           ExitCode=1;
//           scan_goes=0;
//           BAvpCancelProcessObject(1);
//          }
//         SetCursorOff(FALSE);
        }
   } // --- end code block ------

  tt++;
  if(tt>30)
   {
    putchar(0x08);
    putchar(*RotateSymbols[r++]);

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
BOOL WINAPI AvpReadFile(
    HANDLE  hFile,      // handle of file to read
    LPVOID  lpBuffer,   // address of buffer that receives data
    DWORD  nNumberOfBytesToRead,  // number of bytes to read
    LPDWORD  lpNumberOfBytesRead, // address of number of bytes read
    OVERLAPPED *  lpOverlapped)   // address of structure for data BYTE Disk, WORD Sector ,BYTE Head ,LPBYTE Buffer)
 {
  lpOverlapped=lpOverlapped;
  if(hFile==((HANDLE)-1)) return FALSE;

  int realread;;
  if((realread=read((int)hFile,lpBuffer,(size_t)nNumberOfBytesToRead))!=-1)
  {
   *lpNumberOfBytesRead=realread;
//   printf("read file %d: %ld %ld\n",(int)hFile,nNumberOfBytesToRead,*lpNumberOfBytesRead);
   return TRUE;
  }
  else
  {
   *lpNumberOfBytesRead=0;
//   printf("read file %d error\n",(int)hFile);
   return FALSE;
  }

//  BOOL  ret=FALSE;
//  DWORD ulBytesToRead,ulNumberOfBytesToRead;
//  char* ptrBuffer;

//  ptrBuffer=(char*)lpBuffer;
//  ulNumberOfBytesToRead=nNumberOfBytesToRead;
//  *lpNumberOfBytesRead=0;
//read:
//  if(ulNumberOfBytesToRead>0x8000)
//    ulBytesToRead=0x8000;
//  else
//    ulBytesToRead=ulNumberOfBytesToRead;
//  ulNumberOfBytesToRead-=ulBytesToRead;
//
//  if((realread=read((int)hFile,&ptrBuffer[*lpNumberOfBytesRead],(size_t)ulBytesToRead))!=-1)
//   {
//    *lpNumberOfBytesRead+=realread;
//    ret=TRUE;
//    printf("Read file %d %d\r\n",realread,*lpNumberOfBytesRead);
//   }
//  else
//   {
//    ret=FALSE;
//    printf("Read file Error %d \r\n",realread);
//   }
//
//  if(ulNumberOfBytesToRead>0) goto read;
//
//  return ret;
 }

BOOL WINAPI AvpWriteFile(
    HANDLE  hFile,      // handle to file to write to
    LPCVOID lpBuffer,  // pointer to data to write to file
    DWORD   nNumberOfBytesToWrite,    // number of bytes to write
    LPDWORD lpNumberOfBytesWritten, // pointer to number of bytes written
    OVERLAPPED FAR*  lpOverlapped)   // addr. of structure needed for overlapped I/O
 {
  if(hFile==((HANDLE)-1)) return FALSE;
  int ret;
  lpOverlapped=lpOverlapped;
  if((ret=write((int)hFile,lpBuffer,(size_t)nNumberOfBytesToWrite))!=-1)
   {
    *lpNumberOfBytesWritten=ret;
    //printf("write file %d: %ld %ld\n",(int)hFile,nNumberOfBytesToWrite,*lpNumberOfBytesWritten);
    return TRUE;
   }
  else
   {
    *lpNumberOfBytesWritten=0;
    //printf("write file %d error\n",(int)hFile);
    return FALSE;
   }
 }

BOOL __stdcall AvpCloseHandle(HANDLE hObject)     // handle to object to close
 {
  if(hObject==(HANDLE)-1) return FALSE;
  if(close((int)hObject)==0)
 return TRUE;
  else return FALSE;
 }

#define CREATE_NEW          1 //OPEN_ACTION_CREATE_IF_NEW
#define CREATE_ALWAYS       2 //OPEN_ACTION_OPEN_IF_EXISTS|OPEN_ACTION_CREATE_IF_NEW
#define OPEN_EXISTING       3 //OPEN_ACTION_OPEN_IF_EXIST
//#define OPEN_ALWAYS         4
//#define TRUNCATE_EXISTING   5

HANDLE WINAPI AvpCreateFile(
    LPCTSTR  lpFileName,        // pointer to name of the file
    DWORD    dwDesiredAccess,   // access (read-write)
    DWORD    dwShareMode,       // share mode
    SECURITY_ATTRIBUTES FAR* lpSecurityAttributes,     // pointer to security descriptor
    DWORD    dwCreationDistribution,      // how to create
    DWORD    dwFlagsAndAttributes,        // file attributes
    HANDLE   hTemplateFile)       // handle to file with attributes to copy
 {
  lpSecurityAttributes=lpSecurityAttributes; hTemplateFile=hTemplateFile
; dwFlagsAndAttributes=dwFlagsAndAttributes;
 dwShareMode=dwShareMode;
  ULONG EBX=0,EDX=0,attr=0;
  if(dwDesiredAccess==GENERIC_READ)
   {
    EBX|=O_RDONLY;
    attr|=S_IRUSR|S_IRGRP|S_IROTH;
   }
  else
   if(dwDesiredAccess==GENERIC_WRITE)
    {
     EBX|=O_WRONLY;
     attr|=S_IWUSR|S_IWGRP|S_IWOTH;
    }
   else
     //if((dwDesiredAccess & GENERIC_READ)&&(dwDesiredAccess & GENERIC_WRITE))
     {
      EBX|=O_RDWR;
      attr=S_IRUSR|S_IRGRP|S_IROTH|S_IWUSR|S_IWGRP|S_IWOTH;//|S_IXUSR|S_IXGRP|S_IXOTH
     }

//  EBX|=SH_COMPAT;
//  if(dwShareMode==FILE_SHARE_READ) EBX|=SH_DENYWR;
//   else
//    if(dwShareMode==FILE_SHARE_WRITE) EBX|=SH_DENYRD;
//     else
//      if(dwShareMode==(FILE_SHARE_READ|FILE_SHARE_WRITE)) EBX|=SH_DENYNO;
//        else EBX|=SH_DENYRW;

//O_APPEND    0x0010 0001 0000  /* writes done at end of file */
//O_CREAT     0x0020 0010 0000 /* create new file */
//O_TRUNC     0x0040 0100 0000 /* truncate existing file */
//O_NOINHERIT 0x0080 1000 0000 /* file is not inherited by child process */

  switch(dwCreationDistribution)
   {  //0x00x0
    case CREATE_NEW:
      EDX|=O_CREAT|O_TRUNC;
      //rmi.ECX|=S_IWRITE;
      break;
    case CREATE_ALWAYS:
      EDX|=O_CREAT;
      //ECX|=S_IWRITE;
      //attr|=DELETE_FILE_ON_CREATE_BIT;
      break;
    case OPEN_EXISTING:
      //EDX|=O_CREAT;
      break;
    case OPEN_ALWAYS:
      EDX|=O_CREAT;
      break;
    case TRUNCATE_EXISTING:
      EDX|=O_TRUNC;         //0000 0010 02
      break;
    default:  break;
   }

  int handle;
  if((handle=open((char*)lpFileName,EBX|EDX,attr))==(-1))
   {
//    printf("Error open file <%s>\n",lpFileName);
    return (HANDLE)-1;
   }
//  printf("open file <%s>\n",lpFileName);

  return (HANDLE)handle;
 }

DWORD WINAPI AvpSetFilePointer(
    HANDLE hFile,                // handle of file
    LONG   lDistanceToMove,      // number of bytes to move file pointer
    PLONG  lpDistanceToMoveHigh, // address of high-order word of distance to move
    DWORD  dwMoveMethod)         // how to move
 {
  int off=0;
  if(hFile==(HANDLE)-1) return FALSE;
  struct stat FileParam;
  if(fstat((int)hFile,&FileParam)!=-1)
   {
    switch (dwMoveMethod)
     {
      case FILE_BEGIN:   //0
        if(lDistanceToMove>FileParam.st_size) off=lseek((int)hFile,0,SEEK_END);
        else off=lseek((int)hFile,lDistanceToMove,dwMoveMethod);
        break;
      case FILE_CURRENT: //1
        if((lseek((int)hFile,0,SEEK_CUR)+lDistanceToMove)>FileParam.st_size)
          off=lseek((int)hFile,0,SEEK_END);
        else
          off=lseek((int)hFile,lDistanceToMove,dwMoveMethod);
        break;
      case FILE_END:     //2
        if((lseek((int)hFile,0,SEEK_END)+lDistanceToMove)>FileParam.st_size)
          off=lseek((int)hFile,0,SEEK_END);
        else
          off=lseek((int)hFile,lDistanceToMove,dwMoveMethod);
        break;
     }
   }
  //printf("(seek) h=%d toMove=%d method=%d \n",hFile,lDistanceToMove,dwMoveMethod);
  lpDistanceToMoveHigh=lpDistanceToMoveHigh;
  return off;
 }

BOOL WINAPI AvpSetEndOfFile(HANDLE hFile)       // handle of file
 {
  if(hFile==(HANDLE)-1) return FALSE;
  long size=lseek((int)hFile,0,SEEK_CUR);
  //truncate(path,size);
  if(ftruncate((int)hFile,size)!=-1)
  //printf("SetEndOfFile %d \n",hFile);
  //if(write((int)hFile,"",0)!=-1)
//   if(chsize((int)hFile,FileParam.st_size)==0)
    return TRUE;
  return FALSE;
 }

DWORD WINAPI AvpGetFileSize(HANDLE  hFile,      // handle of file
                            LPDWORD lpHigh)
 {
  if(hFile==(HANDLE)-1) return FALSE;
  if(lpHigh!=NULL) *lpHigh=0;
  struct stat FileParam;
  //printf("GetFileSize %d \n",hFile);
  if(fstat((int)hFile,&FileParam)!=-1) return FileParam.st_size;
  else return FALSE;
 }

BOOL WINAPI AvpGetDiskFreeSpace(
    LPCTSTR lpRootPathName,         // address of root path
    LPDWORD lpSectorsPerCluster,    // address of sectors per cluster
    LPDWORD lpBytesPerSector,       // address of bytes per sector
    LPDWORD lpNumberOfFreeClusters, // address of number of free clusters
    LPDWORD lpTotalNumberOfClusters)// address of total number of clusters
 {
//  unsigned err=0;
//  struct diskfree_t diskInfo;

  //printf("GetDiskFreeSpace \n");
//  LONG DiskSpace=0,ID;
//  char buf[256],vol[16];

//  if(Fl_Remote)
//   {
//    getcwd(buf,255);
//    ID=SetCurrentFileServerID(0);
//   }

//  if(lpRootPathName==NULL)
//   if(_dos_getdiskfree(tolower(lpRootPathName[0])-'a'+1,&diskInfo)==0)
    {
//     *lpSectorsPerCluster=diskInfo.sectors_per_cluster;
//     *lpBytesPerSector=diskInfo.bytes_per_sector;
//     *lpNumberOfFreeClusters=diskInfo.avail_clusters;
//     *lpTotalNumberOfClusters=diskInfo.total_clusters;
//     return TRUE;
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
  *lpFilePart=AvpStrrchr(lpBuffer,'/');
  if(*lpFilePart) (*lpFilePart)++;
  else            *lpFilePart=lpBuffer; //+ l;
  //printf("FullFileName: %s %d %s %s\r\r",lpFileName,nBufferLength,lpBuffer,*lpFilePart);
  return l;
 }

DWORD GetFileAttr(char* lpFileName)
 {
  struct stat fStat;
  if(lstat(lpFileName,&fStat)==-1) return FALSE;
  return fStat.st_mode;
 }

BOOL SetFileAttr(char* lpFileName,WORD FileAttr)
 {
  struct stat fStat;
  //printf("SetFileAttr \n");
  if(lstat(lpFileName,&fStat)==-1) return FALSE;
  return TRUE;
 //fStat.st_mode;
 }

BOOL __stdcall AvpDeleteFile(LPCTSTR lpFileName) // pointer to name of file to delete
 {
  //chmod(lpFileName,0);
  //printf("DeleteFile %s\n",lpFileName);
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
//    if(copyfile((char*)lpExistingFileName,(char*)lpNewFileName)==0)
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
  char errTemp[]="/root";
  char *path;
  if((path=getenv("TEMP"))==NULL)
   if((path=getenv("TMP"))==NULL)
    if((path=getenv("HOME"))==NULL)
      path=errTemp;
//    return FALSE;
  strncpy(lpBuffer,path,nBufferLength);
  lpBuffer[strlen(path)]=0;
  if(lpBuffer[strlen(lpBuffer)-1]=='/')
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
  time_t Timer=0;
  if((Timer=time((time_t*)NULL))!=0)
   {
    MSec[0]=(DWORD)Timer;
    MSec[1]=Timer>>16;
    return TRUE;
   }
  else
   {
    MSec[0]=MSec[1]=0;
    return FALSE;
   }
 }

void gen_name(char *text)
 {
  unsigned long name,ver=0;
  int    i;
  char   text1[11];

  DWORD  mSec[2];

  GetSystemTime(mSec);
  name=mSec[0];
  // if(old_millisec==name) name++;
  // old_millisec=name;
  for(i=0;((unsigned long)i)<strlen(text);i++) ver+=text[i];
  ver=ver&0x000000ff;
  for(i=0;((unsigned long)i)<ver;i++) name=rand_m(name);

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
  for(i=0;i<(int)strlen(lpPrefixString);i++) TempStr[i]=lpPrefixString[i];
  sprintf(lpTempFileName,"%s/%s",lpPathName,TempStr);
  if(access(lpTempFileName,0)!=-1)
   do
    {
     gen_name(TempStr);
     for(i=0;i<(int)strlen(lpPrefixString);i++) TempStr[i]=lpPrefixString[i];
     sprintf(lpTempFileName,"%s/%s",lpPathName,TempStr);
    //if(uUnique!=NULL) return uUnique;
    }while(access(lpTempFileName,0)!=-1);
  return uUnique;
 };

//----------------- memory and disk-----------------------
void* _AvpMemAlloc( unsigned int cb )
 {
  void *t;
  t=NULL;
  t=malloc(cb+1);
  if(t==NULL) printf("Error in memalloc size: %d\r\n",cb);
else
 {
//  memset(t,0,cb);
//  printf("memalloc size: %d\n",cb);
 } 
  return t;
 }

void  _AvpMemFree( void* buf)
 {
  if(buf!=NULL) //cprintf("Error in memfree\r\n");
  free(buf);
  buf=NULL;
 }

//-------------
/*
BOOL WINAPI _AvpGetDiskParam( BYTE disk, BYTE drive, WORD* CX, BYTE* DH)
 {
  DWORD Num_Cylinders;
  DWORD Num_Sec_Per_Track;
  DWORD Num_Heads;
  DISK_GEOMETRY Geometry;
  HANDLE hDevice;
  BOOL ret=FALSE;
  BYTE Buffer[0x200];
  char dev[0x20];

  if(CX)*CX=0;
  if(DH)*DH=0;

  if(!ret)if(_AvpRead13(disk,1,0,1,Buffer)) ret=TRUE;
  if(!ret)if(_AvpRead25(drive,0,1,Buffer)) ret=TRUE;

  MakeDeviceName(disk,dev,0x20);
  hDevice = CreateFile(dev,GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,OPEN_EXISTING,0,NULL);

  if(hDevice!=INVALID_HANDLE_VALUE)
   {
    LockVolume(hDevice);
    DWORD ReturnedByteCount;
    ret=DeviceIoControl( hDisk,IOCTL_DISK_GET_DRIVE_GEOMETRY,
    NULL, 0,&Geometry, sizeof(DISK_GEOMETRY),&ReturnedByteCount, NULL );
    if(!ret || ReturnedByteCount!=sizeof(DISK_GEOMETRY) || Geometry.BytesPerSector==0)
     {
      Geometry.BytesPerSector=512;
      Geometry.TracksPerCylinder=80;
      Geometry.SectorsPerTrack=18;
      Geometry.Cylinders.LowPart=2;
     }
    //if(ret)
     {
      Num_Cylinders = Geometry.Cylinders.LowPart;
      Num_Sec_Per_Track = Geometry.SectorsPerTrack;
      Num_Heads = Geometry.TracksPerCylinder;

      if((Num_Cylinders) < 0x400
        && (Num_Sec_Per_Track) < 0x40
        && (Num_Heads) <0x100)
       {
        WORD w=((BYTE)(Num_Cylinders -1))<<8;
        w|=(0xC0)&((Num_Cylinders -1)>>2);
        w|=(0x3F)&(BYTE)(Num_Sec_Per_Track);
        if(CX)*CX=w;
        if(DH)*DH=(BYTE)(Num_Heads -1);
        ret=TRUE;
       }
     }
    UnlockVolume(hDevice);
    CloseHandle(hDevice);
   }
  return ret;
 }
*/
#ifdef
 CHECK_SECT

#include <sys/ioctl.h>

//#include <linux/types.h>
//#include <linux/genhd.h>
//#include <linux/hdreg.h>
//#include <scsi/scsi.h>
//#include <linux/major.h>
//#include <linux/fs.h>

//#if 0
//#include <linux/proc_fs.h>
//#endif

//#include "fdisk/fdisk.h"

//extern char buffer[SECTOR_SIZE];
//uint rounded(uint calcul, uint start);
//int getBoot(int action,char *device);


#include "LibDisk/sys/disklabel.h"
#include <err.h>
#include <errno.h>

extern unsigned char BinMBR[512];

#define MAX_SEC_SIZE 2048       // maximum section size that is supported
#define MIN_SEC_SIZE 512        // the sector size to start sensing at
int secsize = 0;                // the sensed sector size

int iotest;

char CurChkDev[40];
const char *disks[] = { "/dev/rwd0", "/dev/rda0", "/dev/rod0", 0 };

struct disklabel disklabel;             // disk parameters

int cyls, sectors, heads, cylsecs, disksecs;
int dos_cyls,dos_sectors,dos_heads,dos_cylsecs;

#define DOSSECT(s,c) ((s & 0x3f) | ((c >> 2) & 0xc0))
#define DOSCYL(c)       (c & 0xff)

#define ACTIVE 0x80
#define BOOT_MAGIC 0xAA55

struct mboot
 {
  unsigned char padding[2]; // force the longs to be long aligned
  unsigned char bootinst[DOSPARTOFF];
  struct  dos_partition parts[4];
  unsigned short int      signature;
  // room to read in MBRs that are bigger then DEV_BSIZE
  unsigned char large_sector_overflow[MAX_SEC_SIZE-MIN_SEC_SIZE];
 } __attribute__ ((packed)) ;
 struct mboot mboot;

static int partition = -1;

struct part_type
{
 unsigned char type;
 char *name;
} __attribute__ ((packed)) part_types[] =
 {
   {0x00, "unused"}
  ,{0x01, "Primary DOS with 12 bit FAT"}
  ,{0x02, "XENIX / filesystem"}
  ,{0x03, "XENIX /usr filesystem"}
  ,{0x04, "Primary DOS with 16 bit FAT (<= 32MB)"}
  ,{0x05, "Extended DOS"}
  ,{0x06, "Primary 'big' DOS (> 32MB)"}
  ,{0x07, "OS/2 HPFS, NTFS, QNX or Advanced UNIX"}
  ,{0x08, "AIX filesystem"}
  ,{0x09, "AIX boot partition or Coherent"}
  ,{0x0A, "OS/2 Boot Manager or OPUS"}
  ,{0x0B, "DOS or Windows 95 with 32 bit FAT"}
  ,{0x0C, "DOS or Windows 95 with 32 bit FAT, LBA"}
  ,{0x0E, "Primary 'big' DOS (> 32MB, LBA)"}
  ,{0x0F, "Extended DOS, LBA"}
  ,{0x10, "OPUS"}
  ,{0x40, "VENIX 286"}
  ,{0x50, "DM"}
  ,{0x51, "DM"}
  ,{0x52, "CP/M or Microport SysV/AT"}
  ,{0x56, "GB"}
  ,{0x61, "Speed"}
  ,{0x63, "ISC UNIX, other System V/386, GNU HURD or Mach"}
  ,{0x64, "Novell Netware 2.xx"}
  ,{0x65, "Novell Netware 3.xx"}
  ,{0x75, "PCIX"}
  ,{0x80, "Minix 1.1 ... 1.4a"}
  ,{0x81, "Minix 1.4b ... 1.5.10"}
  ,{0x82, "Linux swap or Solaris x86"}
  ,{0x83, "Linux filesystem"}
  ,{0x93, "Amoeba filesystem"}
  ,{0x94, "Amoeba bad block table"}
  ,{0x9F, "BSD/OS"}
  ,{0xA5, "FreeBSD/NetBSD/386BSD"}
  ,{0xA6, "OpenBSD"}
  ,{0xA7, "NEXTSTEP"}
  ,{0xA9, "NetBSD"}
  ,{0xB7, "BSDI BSD/386 filesystem"}
  ,{0xB8, "BSDI BSD/386 swap"}
  ,{0xDB, "Concurrent CPM or C.DOS or CTOS"}
  ,{0xE1, "Speed"}
  ,{0xE3, "Speed"}
  ,{0xE4, "Speed"}
  ,{0xF1, "Speed"}
  ,{0xF2, "DOS 3.3+ Secondary"}
  ,{0xF4, "Speed"}
  ,{0xFF, "BBT (Bad Blocks Table)"}
 };

// Change real numbers into strange dos numbers
static void dos(int sec,int size,unsigned char *c,unsigned char *s,
     unsigned char *h)
 {
  int cy;
  int hd;

  if(sec==0 && size==0)
   {
    *s = *c = *h = 0;
    return;
   }

  cy = sec / ( dos_cylsecs );
  sec = sec - cy * ( dos_cylsecs );

  hd = sec / dos_sectors;
  sec = (sec - hd * dos_sectors) + 1;

  *h=hd;
  *c=cy & 0xff;
  *s=(sec & 0x3f) | ( (cy & 0x300) >> 2);
 }

static void print_params()
 {
  printf("parameters extracted from in-core disklabel are:\n");
  printf("cylinders=%d heads=%d sectors/track=%d (%d blks/cyl)\n\n"
                  ,cyls,heads,sectors,cylsecs);
  if((dos_sectors > 63) || (dos_cyls > 1023) || (dos_heads > 255))
          printf("Figures below won't work with BIOS for partitions not in cyl 1\n");
  printf("parameters to be used for BIOS calculations are:\n");
  printf("cylinders=%d heads=%d sectors/track=%d (%d blks/cyl)\n\n"
          ,dos_cyls,dos_heads,dos_sectors,dos_cylsecs);
 }

static struct dos_partition mtpart = { 0 };

static char *get_type(int type)
 {
  int     numentries = (sizeof(part_types)/sizeof(struct part_type));
  int     counter = 0;
  struct  part_type *ptr = part_types;

  while(counter <numentries)
   {
    if(ptr->type == type)
     {
      return(ptr->name);
     }
    ptr++;
    counter++;
   }
  return("unknown");
 }

static void print_part(int i)
 {
  struct    dos_partition *partp;
  u_int64_t part_mb;

  partp = ((struct dos_partition *) &mboot.parts) + i - 1;

  if (!bcmp(partp, &mtpart, sizeof (struct dos_partition)))
   {
    printf("<UNUSED>\n");
    return;
   }
  // * Be careful not to overflow.
  part_mb = partp->dp_size;
  part_mb *= secsize;
  part_mb /= (1024 * 1024);
  printf("sysid %d,(%s)\n", partp->dp_typ, get_type(partp->dp_typ));
  printf("    start %lu, size %lu (%qd Meg), flag %x%s\n",
          (u_long)partp->dp_start,
          (u_long)partp->dp_size,
          part_mb,
          partp->dp_flag,
          partp->dp_flag == ACTIVE ? " (active)" : "");
  printf("\tcyl: from %d to %d; sector: from %d to %d;head: from %d to %d\n"
          ,DPCYL(partp->dp_scyl,partp->dp_ssect),DPCYL(partp->dp_ecyl,partp->dp_esect)

          ,DPSECT(partp->dp_ssect),DPSECT(partp->dp_esect)

          ,partp->dp_shd
,partp->dp_ehd);
 }

static void print_s0(int which)
 {
  int     i;

  print_params();
  printf("Information from DOS bootblock is:\n");
  if (which == -1)
   for (i = 1; i <= NDOSPART; i++)
           printf("%d: ",i), print_part(i);
  else
   print_part(which);
 }

int fd;

static int get_params()
 {
  if (ioctl(fd, DIOCGDINFO, &disklabel) == -1)
   {
    warnx("can't get disk parameters on %s; supplying dummy ones",CurChkDev);
    dos_cyls = cyls = 1;
    dos_heads = heads = 1;
    dos_sectors = sectors = 1;
    dos_cylsecs = cylsecs = heads * sectors;
    disksecs = cyls * heads * sectors;
    return disksecs;
   }

  dos_cyls = cyls = disklabel.d_ncylinders;
  dos_heads = heads = disklabel.d_ntracks;
  dos_sectors = sectors = disklabel.d_nsectors;
  dos_cylsecs = cylsecs = heads * sectors;
  disksecs = cyls * heads * sectors;

  return (disksecs);
 }

static ssize_t read_disk(off_t sector, void *buf)
 {
  lseek(fd,(sector * 512), 0);
  if( secsize == 0 )
   for( secsize = MIN_SEC_SIZE; secsize <= MAX_SEC_SIZE; secsize *= 2 )
    {
     // try the read
     int size = read(fd, buf, secsize);
     if( size == secsize )
            // it worked so return
            return secsize;
    }
  else
    return read( fd, buf, secsize );

  // we failed to read at any of the sizes
  return -1;
 }

static ssize_t write_disk(off_t sector, void *buf)
 {
  lseek(fd,(sector * 512), 0);
  // write out in the size that the read_disk found worked
  return write(fd, buf, secsize);
 }

static int read_s0()
 {
  if(read_disk(0,(char *)mboot.bootinst) == -1)
   {
    warnx("can't read fdisk partition table");
    return -1;
   }
  if(mboot.signature != BOOT_MAGIC)
   {
    warnx("invalid fdisk partition table found");
    // So should we initialize things
    return -1;
   }
  return 0;
 }

static int write_s0()
 {
  int     flag;
  if(iotest)
   {
    print_s0(-1);
    return 0;
   }
  // * write enable label sector before write (if necessary),
  // * disable after writing.
  // * needed if the disklabel protected area also protects
  // * sector 0. (e.g. empty disk)
  flag = 1;
#ifdef NOT_NOW
  if (ioctl(fd, DIOCWLABEL, &flag) < 0)
          warn("ioctl DIOCWLABEL");
#endif
  if (write_disk(0, (char *) mboot.bootinst) == -1)
   {
    warnx("can't write fdisk partition table");
    return -1;
    flag = 0;
#ifdef NOT_NOW
    (void) ioctl(fd, DIOCWLABEL, &flag);
#endif
   }
  return(0);
 }

static int open_disk(int u_flag)
 {
  struct stat     st;
u_flag
=u_flag;

  if(stat(CurChkDev,&st)==-1)
   {
    warnx("can't get file status of %s",CurChkDev);
    return -1;
   }
  if( !(st.st_mode & S_IFCHR) )
          warnx("device %s is not character special",CurChkDev);
//  if((fd=open(CurChkDev,a_flag || b_flag || u_flag ? O_RDWR : O_RDONLY)) == -1)
  if((fd=open(CurChkDev,O_RDONLY)) == -1)
   {
    if(errno==ENXIO) return -2;
    warnx("can't open device %s",CurChkDev);
    return -1;
   }
  if(get_params()==-1)
   {
    warnx("can't get disk parameters on %s",CurChkDev);
    return -1;
   }
  return fd;
 }

static void init_boot(void)
 {
  memcpy(mboot.bootinst,BinMBR,sizeof(BinMBR));
  mboot.signature = BOOT_MAGIC;
 }

static void init_sector0(unsigned long start)
 {
  struct dos_partition *partp = (struct dos_partition *) (&mboot.parts[3]);
  unsigned long size=disksecs-start;

  init_boot();

  partp->dp_typ = DOSPTYP_386BSD;
  partp->dp_flag = ACTIVE;
  partp->dp_start = start;
  partp->dp_size = size;

  dos(partp->dp_start, partp->dp_size,
      &partp->dp_scyl, &partp->dp_ssect, &partp->dp_shd);
  dos(partp->dp_start + partp->dp_size - 1, partp->dp_size,
      &partp->dp_ecyl, &partp->dp_esect, &partp->dp_ehd);
 }

static void change_part(int i)
 {
  printf("The data for partition %d is:\n", i);
  print_part(i);

//  if(u_flag && ok("Do you want to change it?"))
//   {
//    struct dos_partition *partp = ((struct dos_partition *) &mboot.parts) + i - 1;

//    int tmp;

//    if (i_flag)
//     {
//      bzero((char *)partp, sizeof (struct dos_partition));
//      if (i == 4)
//       {
//        init_sector0(1);
//        printf("\nThe static data for the DOS partition 4 has been reinitialized to:\n");
//        print_part(i);
//       }
//     }

//    do
//     {
//      Decimal("sysid (165=FreeBSD)", partp->dp_typ, tmp);
//      Decimal("start", partp->dp_start, tmp);
//      Decimal("size", partp->dp_size, tmp);

//      if (ok("Explicitly specify beg/end address ?"))
//       {
//        int     tsec,tcyl,thd;
//        tcyl = DPCYL(partp->dp_scyl,partp->dp_ssect);
//        thd = partp->dp_shd;
//        tsec = DPSECT(partp->dp_ssect);
//        Decimal("beginning cylinder", tcyl, tmp);
//        Decimal("beginning head", thd, tmp);
//        Decimal("beginning sector", tsec, tmp);
//        partp->dp_scyl = DOSCYL(tcyl);
//        partp->dp_ssect = DOSSECT(tsec,tcyl);
//        partp->dp_shd = thd;
//
//        tcyl = DPCYL(partp->dp_ecyl,partp->dp_esect);
//        thd = partp->dp_ehd;
//        tsec = DPSECT(partp->dp_esect);
//        Decimal("ending cylinder", tcyl, tmp);
//        Decimal("ending head", thd, tmp);
//        Decimal("ending sector", tsec, tmp);
//        partp->dp_ecyl = DOSCYL(tcyl);
//        partp->dp_esect = DOSSECT(tsec,tcyl);
//        partp->dp_ehd = thd;
//       }
//      else
//       {
//        dos(partp->dp_start, partp->dp_size,
//            &partp->dp_scyl, &partp->dp_ssect, &partp->dp_shd);
//        dos(partp->dp_start + partp->dp_size - 1, partp->dp_size,
//            &partp->dp_ecyl, &partp->dp_esect, &partp->dp_ehd);
//       }

//      print_part(i);
//     } while (!ok("Are we happy with this entry?"));
//   }
 }
#endif

BOOL WINAPI AvpGetDiskParam(BYTE disk,BYTE drive, WORD* CX,BYTE* DH)
 {
  BOOL  ret=FALSE;

#ifdef
 CHECK_SECT
  DWORD Num_Cylinders;
  DWORD Num_Sec_Per_Track;
  DWORD Num_Heads;

  if(CX)*CX=0;
  if(DH)*DH=0;

  int rv=0,i;

//  for(i=0;disks[i]; i++)
//   {
//    CurChkDev=SectChk[ulSectChk-1].sDev;//disks[i];
    rv=open_disk(0);//u_flag);
//    if(rv!=-2) break;
//   }
  if(rv<0) err(1,"cannot open any disk");

//  print_params();

  if(read_s0()) init_sector0(1);

//  printf("Media sector size is %d\n", secsize);
//  printf("Warning: BIOS sector numbering starts with sector 1\n");
//  printf("Information from DOS bootblock is:\n");
  if(partition==-1) for(i=1;i<=NDOSPART;i++) change_part(i);
  else               change_part(partition);

//  getBoot(0,device);
//  if(strlen(CurChkDev)>8)
   {
//    Num_Heads=//devInfo.heads;
//    Num_Sec_Per_Track=//devInfo.sectors;
//    int part;
//    part=SectChk[ulSectChk-1].sDev[8]-'0';
//    struct partition *p;
//    //if(!sun_label)
//     if((p=part_table[part-1])->sys_ind)
//      {
       //       printf("%9d%9d%9d\n",
       /// begin /        cround(rounded(calculate(p->head,p->sector,p->cyl),
       //                       SWAP32(p->start_sect) +devInfo.offsets[part-1])),
       /// start /        cround(SWAP32(p->start_sect)+devInfo.offsets[part-1]),
       /// end /          cround(SWAP32(p->start_sect)+devInfo.offsets[part-1] + SWAP32(p->nr_sects)
       //                 - (p->nr_sects ? 1: 0)));
       //check_consistency(p, i);
//      }
//    Num_Cylinders=
//          cround(SWAP32(p->start_sect)+devInfo.offsets[part-1]
//            + SWAP32(p->nr_sects)-(p->nr_sects ? 1: 0))
//          -cround(SWAP32(p->start_sect)+devInfo.offsets[part-1]);
   }
//else
   {
    Num_Heads=heads;//devInfo.heads;
    Num_Sec_Per_Track=sectors;//devInfo.sectors;
    Num_Cylinders=cyls;//devInfo.cylinders;
   }

  if((Num_Cylinders) < 0x400 && (Num_Sec_Per_Track) < 0x40
             && (Num_Heads) <0x100)
   {
    WORD w=((BYTE)(Num_Cylinders -1))<<8;
    w|=(0xC0)&((Num_Cylinders -1)>>2);
    w|=(0x3F)&(BYTE)(Num_Sec_Per_Track);
    if(CX)*CX=w;
    if(DH)*DH=(BYTE)(Num_Heads -1);
    ret=TRUE;
    printf("AvpGetDiskParam %s %ld %ld %ld\n",CurChkDev,Num_Heads,Num_Sec_Per_Track,Num_Cylinders);
   }
#endif
  return ret;
 }

#ifdef
 CHECK_SECT
#define table_check(b)  ((unsigned short *)((b) + 0x1fe))
#define offset(b, n)    ((struct partition *)((b)+0x1be +(n)*sizeof(struct partition)))
#define sector(s)       ((s) & 0x3f)
#define cylinder(s, c)  ((c) | (((s) & 0xc0) << 2))

#define calculate(h,s,c) (sector(s) - 1 + devInfo.sectors *((h) + devInfo.heads * cylinder(s,c)))
#define set_hsc(h,s,c,sector) {                          \
                               s = sector % sectors + 1; \
                               sector /= sectors;        \
                               h = sector % heads;       \
                               sector /= heads;          \
                               c = sector & 0xff;        \
                               s |= (sector >> 2) & 0xc0;\
                              }
#endif

BOOL IO13(BYTE Fn,BYTE Disk,WORD Sector,BYTE Head,WORD NumSectors,LPBYTE Buffer)
 {
  BOOL  ret=FALSE;
#ifdef
 CHECK_SECT
  
  int fd;
  if((fd=open(CurChkDev,(Fn==0x03) ? O_WRONLY: O_RDONLY))!=-1)
   {
    DWORD sector;
    DWORD SecNo;
    DWORD CylNo;
    SecNo=Sector & 0x003F;
    CylNo=Sector<<2;
    CylNo&=0x0300;
    CylNo|=Sector>>8;
    //sector=(CylNo*TracksPerCylinder + Head)*SectorsPerTrack+ SecNo - 1;
    //sector=(CylNo*devInfo.heads+Head)*devInfo.sectors+SecNo-1;
    sector=(CylNo*heads+Head)*sectors+SecNo-1;

    //printf("io13 Disk %d,Sector %d,Head %d, NumSectors %d\n",Disk,Sector,Head, NumSectors);
    //printf("devInfo.heads %d,devInfo.sectors %d\n",devInfo.heads,devInfo.sectors);
    //printf("sector %d,CylNo %d,SecNo %d\n",sector,CylNo,SecNo);
    long off=lseek(fd,sector*secsize,SEEK_SET);
    if((off==(long)(sector*secsize))!=-1)
     if(Fn==0x03)
      { if(write(fd,Buffer,(size_t)NumSectors*secsize)!=-1) ret=TRUE;}
     else
      { if( read(fd,Buffer,(size_t)NumSectors*secsize)!=-1) ret=TRUE; }
    close(fd);
   }
  printf(" %s \n",CurChkDev);

#endif
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

BOOL IO2x(BYTE Int,BYTE Drive,DWORD Sector,WORD NumSectors,LPBYTE Buffer)
 {
  BOOL   ret=FALSE;

#ifdef
 CHECK_SECT

  int fd;

  //printf("io2x\n");
  if((fd=open(CurChkDev,(Int==0x26) ? O_WRONLY: O_RDONLY))!=-1)
   {
    long off=lseek(fd,Sector*secsize,SEEK_SET);
    if((off==(long)(Sector*secsize))!=-1)
     if(Int==0x26)
      { if(write(fd,Buffer,(size_t)NumSectors*secsize)!=-1) ret=TRUE;}
     else
      { if( read(fd,Buffer,(size_t)NumSectors*secsize)!=-1) ret=TRUE; }
    close(fd);
   }
  //printf("io%x Drive:%x Sector:%ld NumSectors:%d",Int,Drive,Sector,NumSectors);
  //printf(" %s \n",CurChkDev);
#endif
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
//  _AvpMemoryRead(0x413,2,(BYTE*)&memSize);
//  memSize<<=10;
//  if(memSize>0xA0000)memSize=0xA0000;
  //cprintf("GetDosMemSize: %d\r\n",memSize);
  return memSize; // size of DOS memory in bytes
 }

DWORD __stdcall AvpGetFirstMcbSeg(void)
 {
//  union REGPACK reg;
//  RMI    rmi;

//  memset(&rmi,0,sizeof(RMI));
//  rmi.EAX=0x5200;;     //al drive number (00h = A:, 01h = B:, etc)
  //reg.w.flags = CARRY_FLAG;

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
//   {//es:bx
//    //cprintf("GetFirstMcbSeg Yes\r\n");
//    //(DWORD)*(WORD*)(rmi.EBX-2);
//    return (DWORD)*(WORD*)(rmi.ES*0x10+rmi.EBX-2);// TRUE;
//   }
//  else
   {
    //cprintf("GetFirstMcbSeg No\r\n");
    return FALSE;
   }
 }


DWORD __stdcall AvpGetIfsApiHookTable(DWORD* table,
                                   DWORD size) //size in DWORDS !!!!!!!!
 {
  DWORD count=0;
  table=table; size=size;
  //cprintf("GetIfsApiHookTable\r\n");
  //GetNextIfsApiHook(count,table, maxCount);
//  table=table; size=size;
  return count;
 }

DWORD __stdcall AvpGetDosTraceTable(
       DWORD* table,
       DWORD size) //size in DWORDS !!!!!!!!
 {
  DWORD count=0;
  table=table; size=size;
/*
  DWORD val;
  //cprintf("GetDosTraceTable\r\n");
  if(4==_AvpMemoryRead(0x04c,4,(BYTE*)&val)) table[count++]=val;
  if(4==_AvpMemoryRead(0x084,4,(BYTE*)&val)) table[count++]=val;
  if(4==_AvpMemoryRead(0x100,4,(BYTE*)&val)) table[count++]=val;

  size=size;
*/
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
  dwOffset=dwOffset; dwSize=dwSize; lpBuffer=lpBuffer;
  return ret;
 }

DWORD __stdcall AvpMemoryWrite(
   DWORD  dwOffset,   // offset
   DWORD  dwSize,     // size in bytes
   LPBYTE lpBuffer)   // pointer to buffer to write from
 {
  DWORD ret=0;
  dwOffset=dwOffset; dwSize=dwSize; lpBuffer=lpBuffer;
  return ret;
 }

LONG avp_mess(DWORD mp1,DWORD mp2);
LRESULT __stdcall AvpCallback(WPARAM wParam,LPARAM lParam)
 {
  return (LRESULT)avp_mess(wParam,lParam);
 }

DWORD WINAPI AvpExecSpecial(
                char* FuncName,
                DWORD wParam,
                DWORD lParam
                )
{
        char buf[0x200];
//        char DllName[0x200];
//        char* lpFilePart;
        if(!FuncName || !*FuncName) return 0;

wParam=wParam; lParam=lParam;
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

BOOL LoadIO()
 {
  #ifdef _MBCS
   setlocale ( LC_ALL, "" );
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

