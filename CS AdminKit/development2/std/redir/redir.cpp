// -*- C++ -*- Time-stamp: <03/01/09 11:01:53 ptr>

/*!
 *
 * DoOxigenize here...
 *
 */

#include <std/base/kldefs.h>
#include <std/base/klbase.h>
#include <std/thr/sync.h>

#include <std/trc/trace.h>

#include <map>

#ifdef __unix
#  ifdef __HP_aCC
#pragma VERSIONID "@(#)$Id: redir.cpp,v 1.1.1.1 2003/01/20 13:27:55 ptr Exp $"
#  else
#ident "@(#)$Id: redir.cpp,v 1.1.1.1 2003/01/20 13:27:55 ptr Exp $"
#  endif
#endif

#include "typedef.h"
#ifdef __unix__

#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <errno.h>

#include <iostream>
#include <iomanip>

#ifdef __FreeBSD__
#define __ptr_t LPVOID
#endif

#endif

#ifdef N_PLAT_NLM
#include <unistd.h>
#include <fcntl.h>
#include <stat.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <nwerrno.h>
#include <time.h>
#define __ptr_t LPVOID
#endif

#define KLCS_MODULENAME L"REDIR"

class FileLocker
{
public:
	FileLocker()
	{
		KLSTD_CreateCriticalSection( &m_pCricsec );
	}

	int LockFile( int fdesc, DWORD shareMode, DWORD access )
	{
		struct stat st;

		if ( fstat( fdesc, &st )!=0 )
		{
			KLSTD_TRACE0( 1, L"FileLocker::LockFile error. ( fstat!=0 )\n" )
			return (-1);
		}


		int fsid = st.st_dev;
		int fid = st.st_ino;

		KLSTD_TRACE4(
            5, 
            L"FileLocker::LockFile fdesc - %d.%d shareMode - %d acess - %X\n", 
            fsid, 
            fid, 

            shareMode, 
            access );
		KLSTD_TRACE1(
            5, 
            L"FileLocker::LockFile id - %d\n", 
			fdesc );

		KLSTD::AutoCriticalSection unlocker( m_pCricsec );

		FileSystemMap::iterator fit = m_fileLocks.find( fsid );

		if ( fit==m_fileLocks.end() )
		{
			FileLockMap lock;
			m_fileLocks[fsid] = lock;
			fit = m_fileLocks.begin();
		}

		FileLockMap &locks = fit->second;

		FileLockMap::iterator flit = locks.find( fid );
		if ( flit==locks.end() )
		{
#ifdef __unix
			if ( flock( fdesc, LOCK_EX | LOCK_NB )==(-1) )
			{
				KLSTD_TRACE1( 1, L"FileLocker::LockFile error. ( flock==(-1) ) errno - %d\n",				
					errno );
				return 0;			
			}
#else		
			if ( (access & GENERIC_WRITE) )
			{		
				struct flock flptr;
				flptr.l_start=0; 
				flptr.l_whence=SEEK_SET; 
				flptr.l_len=0; 
				flptr.l_type=F_WRLCK;
				if ( fcntl( fdesc, F_SETLK, &flptr )==(-1) ) 
				{
					KLSTD_TRACE1( 1, L"FileLocker::LockFile error. ( fcntl==(-1) ) errno - %d\n",				
						errno );
					return 0;			
				}
			}
#endif				

			FileLockDesc flock;
			flock.lockCounter = 1;
			flock.shareMode = shareMode;
			flock.access = access;
			locks[fid]=flock;
			KLSTD_TRACE3( 
                5, 
                L"FileLocker::LockFile fdesc - %d.%d shareMode - %d (add new item ) return 1\n", 
				fsid, 
                fid, 
                shareMode );
			return 1;
		}
		else
		{
			FileLockDesc &flock = flit->second;			
			bool shareModeFlag = (flock.shareMode==0)?false:((flock.shareMode & shareMode)==shareMode);			
			if ( shareModeFlag==false && access==GENERIC_READ && flock.shareMode==FILE_SHARE_READ )
			{
				shareModeFlag = true;
			}
			bool accessModeFlag = false;
			if ( shareModeFlag )
			{	
				if ( (flock.shareMode==FILE_SHARE_READ) && (access & GENERIC_WRITE) ) 
				{
					accessModeFlag = false;
				}
				else
					accessModeFlag = true;
			}
			if ( !shareModeFlag ||
				!accessModeFlag ) 
			{
				KLSTD_TRACE3( 
                    1, 
                    L"!!! FileLocker::LockFile fdesc - %d.%d shareMode - %d ( file locked ) return 0\n", 
                    fsid, 
                    fid, 
                    shareMode );	
				return 0;
			}
			else 
			{
				flock.lockCounter++;
				//KLSTD_TRACE2( 1, L"FileLocker::LockFile fdesc - %d shareMode - %d ( same sharedmode ) return 1\n", 
				//fdesc, shareMode );
				if ( shareMode < flock.shareMode ) 
				{
					flock.shareMode = shareMode;
					if ( flock.shareMode==FILE_SHARE_READ ) flock.shareOnlyReadHandlesList.push_back( fdesc );
				}
				return 1;
			}
		}
	}

	void UnlockFile( int fdesc )
	{
		struct stat st;

		if ( fstat( fdesc, &st )!=0 ) return;
		int fsid = st.st_dev;
		int fid = st.st_ino;

		KLSTD_TRACE3( 
                5, 

                L"FileLocker::UnlockFile fdesc - %d.%d id - %d\n", 
				fsid, 
                fid, 
                fdesc );

		KLSTD::AutoCriticalSection unlocker( m_pCricsec );

		FileSystemMap::iterator fit = m_fileLocks.find( fsid );

		if ( fit==m_fileLocks.end() ) 
		{
			KLSTD_TRACE1( 1, L"FileLocker::UnlockFile fdesc - %d ( not found )\n", 
				fdesc );
			return;
		}
		FileLockMap &locks = fit->second;

		FileLockMap::iterator flit = locks.find( fid );
		if ( flit==locks.end() ) 
		{
			KLSTD_TRACE1( 1, L"FileLocker::UnlockFile fdesc - %d ( not found 2 )\n", 
				fdesc );
			return;
		}
		
		FileLockDesc &flocker = flit->second;
		flocker.lockCounter--;
		HandleList::iterator forlit = std::find( flocker.shareOnlyReadHandlesList.begin(), 
			flocker.shareOnlyReadHandlesList.end(), fdesc );
		if ( forlit!=flocker.shareOnlyReadHandlesList.end() ) flocker.shareOnlyReadHandlesList.erase( forlit );
		
		if ( flocker.lockCounter==0 ) 
		{
#ifdef __unix
			if ( flock( fdesc, LOCK_UN | LOCK_NB  )==(-1) )
			{
				KLSTD_TRACE1( 1, L"FileLocker::UnlockFile error. ( flock==(-1) ) errno - %d\n",				
					errno );
			}
#else			
			struct flock fvar;
			fvar.l_type = F_UNLCK;
			fvar.l_whence = SEEK_SET;
			fvar.l_start = 0;
			fvar.l_len = 0;

			fcntl( fdesc, F_SETLKW, &fvar );
			//KLSTD_TRACE2( 1, L"FileLocker::UnlockFile fdesc - %d.%d ( unlocked )\n", 
			//	fsid, fid );
#endif
			
			locks.erase( flit  );
		}
		else
		{
			if ( flocker.shareOnlyReadHandlesList.size()==0 )
			{
				flocker.shareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
			}
		}

		KLSTD_TRACE2( 1, L"FileLocker::UnlockFile fdesc - %d.%d OK\n", 
				fsid, fid );

	}


private:

	typedef std::list<int> HandleList;

	struct FileLockDesc
	{		
		HandleList		shareOnlyReadHandlesList;
		int		lockCounter;
		DWORD	shareMode;
		DWORD	access;
	};

	typedef std::map<int, FileLockDesc> FileLockMap;
	typedef std::map<int, FileLockMap> FileSystemMap;

	KLSTD::CAutoPtr<KLSTD::CriticalSection> m_pCricsec;

	FileSystemMap m_fileLocks;
};

FileLocker *g_FileLocker = NULL;

void KLSTD_InitAlternativeFileLocker()
{
	g_FileLocker = new FileLocker();
}

void KLSTD_DeInitAlternativeFileLocker()
{
	if ( g_FileLocker ) 
	{
		delete g_FileLocker;
		g_FileLocker = NULL;		
	}
}

// returns 0 - already is locked, 1 - succesefuly locked, -1 - error
int KLSTD_AlternativeLockFile( int fdesc, DWORD shareMode, DWORD access )
{
	if ( g_FileLocker ) return g_FileLocker->LockFile( fdesc, shareMode, access );
	else return 1;
}

int KLSTD_AlternativeUnLockFile( int fdesc )
{
	if ( g_FileLocker ) g_FileLocker->UnlockFile( fdesc );
}

BOOL WINAPI ReadFile(
    HANDLE  hFile,      // handle of file to read
    LPVOID  lpBuffer,   // address of buffer that receives data
    DWORD  nNumberOfBytesToRead,  // number of bytes to read
    LPDWORD  lpNumberOfBytesRead, // address of number of bytes read
    OVERLAPPED *  lpOverlapped)   // address of structure for data BYTE Disk, WORD Sector ,BYTE Head ,LPBYTE Buffer)
 {
  lpOverlapped=lpOverlapped;
  if(hFile==((HANDLE)-1)) return FALSE;
  int ret;  
  if((ret=read((int)hFile,(__ptr_t)lpBuffer,(size_t)nNumberOfBytesToRead))!=-1)
  {
   *lpNumberOfBytesRead=ret;
   return TRUE;
  }
  else
  { 
   *lpNumberOfBytesRead=0;
   return FALSE; 
  } 
 }

BOOL WINAPI WriteFile(
    HANDLE  hFile,      // handle to file to write to
    LPCVOID lpBuffer,  // pointer to data to write to file
    DWORD   nNumberOfBytesToWrite,    // number of bytes to write
    LPDWORD lpNumberOfBytesWritten, // pointer to number of bytes written
    OVERLAPPED FAR*  lpOverlapped)   // addr. of structure needed for overlapped I/O
 {
  if(hFile==((HANDLE)-1)) return FALSE;
  int ret;
  if((ret=write((int)hFile,(__ptr_t)lpBuffer,(size_t)nNumberOfBytesToWrite))!=-1)
   {
    *lpNumberOfBytesWritten=ret;
    return TRUE;
   }
  else
   {
    *lpNumberOfBytesWritten=0;
    return FALSE; 
   }    
 }

BOOL __stdcall CloseHandle(HANDLE hObject)     // handle to object to close
 {
  if(hObject==(HANDLE)-1)
      return FALSE;

  KLSTD_AlternativeUnLockFile( (int)hObject );

  if(close((int)hObject)==0) 
      return TRUE;
  else
  {
      KLSTD_TRACE1(1, L"close(%d) failed\n", (int)hObject);
      KLSTD_ASSERT(!"close failed");
      return FALSE;
  };
 }

// following comes from Win?

#define CREATE_NEW          1 //OPEN_ACTION_CREATE_IF_NEW
#define CREATE_ALWAYS       2 //OPEN_ACTION_OPEN_IF_EXISTS|OPEN_ACTION_CREATE_IF_NEW
#define OPEN_EXISTING       3 //OPEN_ACTION_OPEN_IF_EXIST
//#define OPEN_ALWAYS         4
//#define TRUNCATE_EXISTING   5

// Not very good: we have here unbuffered file operations, performance
// will be bad...

HANDLE WINAPI CreateFile(
    LPCTSTR  name,        // pointer to name of the file
    DWORD    dwDesiredAccess,   // access (read-write)
    DWORD    dwShareMode,       // share mode
    SECURITY_ATTRIBUTES FAR* lpSecurityAttributes,     // pointer to security descriptor
    DWORD    dwCreationDistribution,      // how to create
    DWORD    dwFlagsAndAttributes,        // file attributes
    HANDLE   hTemplateFile,
    bool&    bFileLocked )       // handle to file with attributes to copy
{
	bFileLocked = false;
  int flags = 0;
  mode_t mode;
  bool _with_mode = false;

  if ( dwDesiredAccess == GENERIC_READ ) {
    flags |= O_RDONLY; 
  } else if ( dwDesiredAccess == GENERIC_WRITE ) {
    flags |= O_WRONLY;
  } else { // ? may be GENERIC_RW?
    flags |= O_RDWR;
  } 

//  EBX|=SH_COMPAT;
//  if(dwShareMode==FILE_SHARE_READ) EBX|=SH_DENYWR;
//   else
//    if(dwShareMode==FILE_SHARE_WRITE) EBX|=SH_DENYRD;
//     else
//      if(dwShareMode==(FILE_SHARE_READ|FILE_SHARE_WRITE)) EBX|=SH_DENYNO;
//        else EBX|=SH_DENYRW;

  switch ( dwCreationDistribution ) {
    case CREATE_NEW:
      flags |= O_CREAT | O_EXCL; // will be broken on NFS!
      mode = S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR| S_IWGRP | S_IWOTH; // 0666
      _with_mode = true;
      break;
    case CREATE_ALWAYS:
      flags |= O_CREAT | O_TRUNC;
      mode = S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR| S_IWGRP | S_IWOTH; // 0666
      _with_mode = true;
      break;
    case OPEN_EXISTING:	
      break;
    case OPEN_ALWAYS:
      flags |= O_CREAT;
      mode = S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR| S_IWGRP | S_IWOTH; // 0666
      _with_mode = true;
      break;
    case TRUNCATE_EXISTING:
      flags |= O_TRUNC;
      break;
    default:
      break;
  }



  int retHandle = ( _with_mode ? open( name, flags, mode ) : open( name, flags ) );  
  if ( retHandle==(-1) ) 
#ifdef N_PLAT_NLM
	  return (HANDLE)retHandle;
#else
	  return reinterpret_cast<HANDLE>(retHandle);
#endif  
  int lockRes = KLSTD_AlternativeLockFile( retHandle, dwShareMode, dwDesiredAccess );
  if ( lockRes==1 )
#ifdef N_PLAT_NLM
	  return (HANDLE)retHandle;
#else
	  return reinterpret_cast<HANDLE>(retHandle);
#endif  

  if( retHandle!=-1 ) 
  {
	  if(close((int)retHandle) != 0)
      {
          KLSTD_TRACE1(1, L"close(%d) failed\n", (int)retHandle);
          KLSTD_ASSERT(!"close failed");
      };
  }
  
  if ( lockRes==0 ) 
  	errno = EACCES;
  bFileLocked = true;
  return (HANDLE)(-1);


}

DWORD WINAPI GetFileSize(HANDLE  hFile,      // handle of file
                            LPDWORD lpHigh)
 {
  if(hFile==(HANDLE)-1) return FALSE;
  if(lpHigh!=NULL) *lpHigh=0;
  struct stat FileParam;
  //printf("GetFileSize %d \n",hFile);
  if(fstat((int)hFile,&FileParam)!=-1) return FileParam.st_size;
  else return FALSE;
 }

DWORD WINAPI SetFilePointer(
    HANDLE hFile,                // handle of file
    LONG   lDistanceToMove,      // number of bytes to move file pointer
    PLONG  lpDistanceToMoveHigh, // address of high-order word of distance to move
    DWORD  dwMoveMethod)         // how to move
 {
  int off=0;
  //dprintf(("SetFilePointer handle<%d> toMove=%ld method=%ld \n",(int)hFile,lDistanceToMove,dwMoveMethod));
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

BOOL WINAPI SetEndOfFile(HANDLE hFile)       // handle of file
 {
  //dprintf(("SetEndOfFile handle<%d>\n",(int)hFile));
  if(hFile==(HANDLE)-1) return FALSE;
   {   
    long size=lseek((int)hFile,0,SEEK_CUR);
#ifndef N_PLAT_NLM    
    if(ftruncate((int)hFile,size)!=-1)
    	return TRUE;
#else
	return TRUE;
#endif        
   }      
  return FALSE;
 }

DWORD WINAPI GetTempPath(
        DWORD nBufferLength,  // size, in characters, of the buffer
        LPTSTR lpBuffer)      // address of buffer for temp. path
 {
  char *path;
  lpBuffer[0]=0;

#ifdef __UNIX__
//  char errTemp[]="/root";
  if(access("/tmp",0)!=-1) 	  path="/tmp";
  else
   if(access("/usr/tmp",0)!=-1) path="/usr/tmp";
   else
//    if(access("/root",0)!=-1)  path="/root";
    if((lpBuffer[0]!=0) && (access(lpBuffer,0)!=-1)) return strlen(lpBuffer);
    else
     if((path=getenv("TEMP"))==NULL)
      if((path=getenv("TMP"))==NULL)
       if((path=getenv("HOME"))==NULL)
//        path=errTemp;
        return FALSE;
  strncpy(lpBuffer,path,nBufferLength-1);
  lpBuffer[nBufferLength-1]=0;
  if(lpBuffer[strlen(lpBuffer)-1]=='/') lpBuffer[strlen(lpBuffer)-1]=0;
  //dprintf(("GetTempPathName %s\n",lpBuffer));
#endif  
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
  DWORD  mSec[2];
  GetSystemTime(mSec);
  unsigned long name=(mSec[0]<<4)+mSec[1];
  // if(old_millisec==name) name++;
  // old_millisec=name;
  int    i;
  unsigned long ver=0;
  for(i=0;((unsigned long)i)<strlen(text);i++) ver+=text[i];
  ver=ver&0x000000ff;
  for(i=0;((unsigned long)i)<ver;i++) name=rand_m(name);

  char   text1[11];
  sprintf(text1,"%08lu",name);
  for(i=0;i<8;i++) text[i]=text1[i];
  sprintf(&text[8],"%s",".tmp");
  text[12]=0;
 }

int AccessTmpFile(char *TmpName,int flag)
 {
  if(access(TmpName,0)!=-1) return 0;
 return -1;
 }

static DWORD old_millisec=0;
UINT WINAPI GetTempFileName(
        LPCTSTR lpPathName,     // address of directory name for temporary file
        LPCTSTR lpPrefixString, // address of filename prefix
        UINT uUnique,           // number used to create temporary filename
        LPTSTR lpTempFileName)  // address of buffer that receives the new filename
 {
  DWORD  mSec[2];
  if(old_millisec==0)
   {
    GetSystemTime(mSec);
    old_millisec=(mSec[0]<<4)+mSec[1];
   }
  else old_millisec++;

  int i;
  CHAR TempStr[13];
  sprintf(TempStr,"%08lx",old_millisec);
  strcat(&TempStr[8],".tmp");
  for(i=0;i<(int)strlen(lpPrefixString);i++) TempStr[i]=lpPrefixString[i];
  sprintf(lpTempFileName,"%s/%s",lpPathName,TempStr);
  while(AccessTmpFile(lpTempFileName,0)!=-1)
    {
     gen_name(TempStr);
     for(i=0;i<(int)strlen(lpPrefixString);i++) TempStr[i]=lpPrefixString[i];
     sprintf(lpTempFileName,"%s/%s",lpPathName,TempStr);
     //if(uUnique!=NULL) return uUnique;
    }

  //lpTempFileName[0]=0;
  return uUnique;
 };

BOOL WINAPI FlushFileBuffers (
	HANDLE hFile )
{
	return (0 == fsync((int)hFile));
}

BOOL WINAPI LockFile(
		HANDLE hFile,                   // handle to file
		DWORD dwFileOffsetLow,          // low-order word of offset
		DWORD dwFileOffsetHigh,         // high-order word of offset
		DWORD nNumberOfBytesToLockLow,  // low-order word of length
		DWORD nNumberOfBytesToLockHigh  // high-order word of length
)
{
	long offset = dwFileOffsetLow;
	long len	= nNumberOfBytesToLockLow;

	struct flock fvar;

	fvar.l_type = F_WRLCK;
	fvar.l_whence = SEEK_SET;
	fvar.l_start = offset;
	fvar.l_len = len;

	int res = fcntl( (int)hFile, F_SETLK, &fvar );
	
	if ( res==(-1) ) return FALSE;
	else return TRUE;
}

BOOL WINAPI UnlockFile(
		HANDLE hFile,                    // handle to file
		DWORD dwFileOffsetLow,           // low-order word of start
		DWORD dwFileOffsetHigh,          // high-order word of start
		DWORD nNumberOfBytesToUnlockLow, // low-order word of length
		DWORD nNumberOfBytesToUnlockHigh // high-order word of length
)
{
	long offset = dwFileOffsetLow;
	long len	= nNumberOfBytesToUnlockLow;

	struct flock fvar;

	fvar.l_type = F_UNLCK;
	fvar.l_whence = SEEK_SET;
	fvar.l_start = offset;
	fvar.l_len = len;

	int res = fcntl( (int)hFile, F_SETLKW, &fvar );
	
	if ( res==(-1) ) return FALSE;
	else return TRUE;
}


DWORD GetLastError(VOID)
 {
  return errno;
 }
