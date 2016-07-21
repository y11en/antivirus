// filecallback.cpp -- FPI file callback functions
//
// Copyright (c) 1997-2000 F-Secure Corporation

#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>

#if defined OS2
#include "myos2.h"
#elif defined UNIX
#include <unistd.h>
#else
#include <windows.h>
#endif

#ifdef _WIN32
#include <winioctl.h>
#endif

extern "C"
{
#include "fpiapi.h"
#include "fpirun.h"

#if defined USE_FM
#if defined(_MSC_VER) && _MSC_VER < 900
#pragma pack(8)
#else
#pragma pack(push,8)
#endif
#include "fmngm.h"
#if defined(_MSC_VER) && _MSC_VER < 900
#pragma pack()
#else
#pragma pack(pop)
#endif
#endif
}

#include "uscanman.h"


#ifdef TRACK_FM
void TrackFMExtra(FPIFILEHANDLE handle, long pos, DWORD bytestoread);
#endif


WORD  FPIFILEIOFN FN_GETLASTERROR (void)
{
#ifdef USE_FM
    ulong fmerror = fmGetLastError();
    WORD fpierror;
    switch (fmerror)
    {
    case FM_ERR_NONE:                   // no error
    case FM_ERR_FILEOPENED:             // no error
    case FM_ERR_ARCHIVEOPENED:          // no error
        fpierror = FPIFILEIO_ERROR_NONE;
        break;
    case FM_ERR_NOMEM:                  // not enough memory
        fpierror = FPIFILEIO_ERROR_NOMEM;
        break;
    case FM_ERR_OPENFAILED:             // error opening file
        fpierror = FPIFILEIO_ERROR_OPENFAILED;
        break;
    case FM_ERR_NOMOREHANDLES:          // no more handles available
        fpierror = FPIFILEIO_ERROR_OPENFAILED;
        break;
    case FM_ERR_FILEIO:                 // file I/O error
        fpierror = FPIFILEIO_ERROR_IOERROR;
        break;
    case FM_ERR_BADARCHIVE:             // archive is corrupted
        fpierror = FPIFILEIO_ERROR_BADARCHIVE;
        break;
    case FM_ERR_NOMOREFILES:            // no more files in archive
        fpierror = FPIFILEIO_ERROR_NOTINARCHIVE;
        break;
    case FM_ERR_NOTINARCHIVE:           // no such file in archive
        fpierror = FPIFILEIO_ERROR_NOTINARCHIVE;
        break;
    case FM_ERR_BADFILE:                // file is corrupted in archive
        fpierror = FPIFILEIO_ERROR_BADFILE;
        break;
    case FM_ERR_BADHANDLE:              // invalid handle passed
        fpierror = FPIFILEIO_ERROR_BADHANDLE;
        break;
    case FM_ERR_BADOFFSET:
        fpierror = FPIFILEIO_ERROR_BADARGUMENT;         // jmk: Is this the right code?
        break;
    case FM_ERR_ENCRYPTED:
        fpierror = FPIFILEIO_ERROR_ENCRYPTED;
        break;
    case FM_ERR_BADARGUMENT:
        fpierror = FPIFILEIO_ERROR_BADARGUMENT;
        break;
    case FM_ERR_UNIMPLEMENTED:          // feature is not implemented yet
        fpierror = FPIFILEIO_ERROR_UNKNOWN;
        break;
    case FM_ERR_UNKNOWN:                // unknown error
        fpierror = FPIFILEIO_ERROR_UNKNOWN;
        break;

    default:
        fpierror = FPIFILEIO_ERROR_UNKNOWN;
        break;
    }

    return fpierror;
#else
    if (errno == 0)
        return FPIFILEIO_ERROR_NONE;
    else
        return FPIFILEIO_ERROR_UNKNOWN;   // TODO: Add the error information.
#endif
}   

FPIFILEHANDLE  FPIFILEIOFN FN_OPEN (FPICONTAINERHANDLE  parent_ , LPCSTR  filename_ , DWORD  accessmode_ , DWORD  reservedA_ , DWORD  reservedB_ , DWORD  reservedC_ ) 
{
#ifdef USE_FM
	HFMFILE     handle = 0;
	FMOFSTRUCT  ofstruct;
	FMFILEINFO  fileinfo;
	int fmaccessmode;

	switch (accessmode_)
	{
		case FPIFILEIO_READ:
			fmaccessmode = FM_READONLY_ACCESS;
			break;
		case FPIFILEIO_WRITE:
			fmaccessmode = FM_WRITEONLY_ACCESS;
			break;
		case FPIFILEIO_RDWR:
			fmaccessmode = FM_RDWR_ACCESS;
			break;
		case FPIFILEIO_CREATE:
			fmaccessmode = FM_CREATE_ACCESS | FM_RDWR_ACCESS;
			break;

		default:
			return FPIFILEHANDLE_ERROR;
	}

	strcpy(ofstruct.pszPassword, "");

	if (fmOpenFile(&handle, parent_, filename_, fmaccessmode, FALSE, 
		FM_SHARE_COMPAT, 0, &ofstruct, &fileinfo))
	{
		return handle;
	}
	else
	{
		return FPIFILEHANDLE_ERROR;
	}
#else
	int mode;

	switch (accessmode_)
	{
		case FPIFILEIO_READ:
			mode = _O_RDONLY | _O_BINARY;
			break;
		case FPIFILEIO_WRITE:
			mode = _O_WRONLY | _O_BINARY | _O_CREAT;
			break;
		case FPIFILEIO_RDWR:
			mode = _O_RDWR | _O_BINARY;
			break;
		case FPIFILEIO_CREATE:
			mode = _O_RDWR | _O_BINARY | _O_CREAT;
			break;
		default:
			return FPIFILEHANDLE_ERROR;
	}

	return (FPIFILEHANDLE)(open(filename_, mode, _S_IREAD | _S_IWRITE));
#endif
}

BOOL  FPIFILEIOFN FN_CLOSE (FPIFILEHANDLE  handle_ ) 
{
#ifdef USE_FM
    return fmCloseFile(handle_);
#else
    return close(handle_) != -1;
#endif
}

BOOL  FPIFILEIOFN FN_READ(FPIFILEHANDLE  handle_ , LPVOID  buf_ , DWORD  bytestoread_ , LPDWORD  bytesread_ ) 
{
	if (bytestoread_ > FPIMAX_READLENGTH)
	{
		// TODO: record the FPI API violation
		return FALSE;
	}

#ifdef USE_FM

#ifdef TRACK_FM
	long pos = 0;
	if (options.track_fm)
		fmTellFile(handle_, &pos);
	DWORD tickStart = GetTickCount();
#endif

	BOOL ret = fmReadFile(handle_, buf_, bytestoread_, bytesread_);

#ifdef TRACK_FM
	DWORD ticks = GetTickCount() - tickStart;
	if (options.track_fm)
	{
		printlog("read:\t%d\t%s\t%ld\t%lu\t%lu\n",
			handle_,
			fmGetFileName(handle_),
			pos,
			bytestoread_,
			ticks);
		// printlog("read,%d,%lu,0,0,%lu\n", handle_, bytestoread_, ticks);
	}

	if (options.track_fm_extra)
		TrackFMExtra(handle_, pos, bytestoread_);
#endif

	return ret;
#else
	int bytesread = read(handle_, buf_, bytestoread_);
	if (bytesread >= 0)
		*bytesread_ = bytesread;
	else
		*bytesread_ = 0;

	if (bytesread != -1)
		return TRUE;

	return FALSE;
//    return ((DWORD)bytesread == bytestoread_);
#endif
}

BOOL  FPIFILEIOFN FN_WRITE (FPIFILEHANDLE  handle_ , LPCVOID  buf_ , DWORD  bytestowrite_ , LPDWORD  byteswritten_ ) 
{
    if (bytestowrite_ > FPIMAX_WRITELENGTH)
    {
        // TODO: record the FPI API violation
        return FALSE;
    }

#ifdef USE_FM
    return fmWriteFile(handle_, buf_, bytestowrite_, byteswritten_);
#else
    int byteswritten = write(handle_, buf_, bytestowrite_);
    if (byteswritten >= 0)
        *byteswritten_ = byteswritten;
    else
        *byteswritten_ = 0;

    return ((DWORD)byteswritten == bytestowrite_);
#endif
}

DWORD  FPIFILEIOFN FN_GETSIZE (FPIFILEHANDLE  handle_ ) 
{
#ifdef USE_FM
    long size = 0;
    if (fmGetFileSize(handle_, &size))
        return size;
    else
        return FPIFILESIZE_ERROR;
#else
    return filelength(handle_);
#endif
}

DWORD  FPIFILEIOFN FN_GETPOS (FPIFILEHANDLE  handle_ ) 
{
#ifdef USE_FM
    long pos = 0;
    if (fmTellFile(handle_, &pos))
        return pos;
    else
        return FPIFILESIZE_ERROR;
#else
    return tell(handle_);
#endif
}

DWORD  FPIFILEIOFN FN_SETPOS (FPIFILEHANDLE  handle_ , LONG  distance_ , WORD  fromwhere_ ) 
{
#ifdef USE_FM
    DWORD ret;

    int origin;
    switch (fromwhere_)
    {
    case FPIFILEIO_CURRENT:
        origin = FILESEEK_CUR;
        break;
    case FPIFILEIO_END:
        origin = FILESEEK_END;
        break;
    case FPIFILEIO_BEGIN:
        origin = FILESEEK_SET;
        break;
    default:
        return FPIFILESIZE_ERROR;
    }

    if (fmSeekFile(handle_, distance_, origin))
        ret = FN_GETPOS(handle_);
    else
    {
        DWORD code = fmGetLastError();
        ret = FPIFILESIZE_ERROR;
    }

    return ret;

#else
    int origin;
    switch (fromwhere_)
    {
    case FPIFILEIO_CURRENT:
        origin = SEEK_CUR;
        break;
    case FPIFILEIO_END:
        origin = SEEK_END;
        break;
    case FPIFILEIO_BEGIN:
        origin = SEEK_SET;
        break;
    default:
        return FPIFILESIZE_ERROR;
    }

    return lseek(handle_, distance_, origin);
#endif
}

BOOL  FPIFILEIOFN FN_SETEND (FPIFILEHANDLE  handle_ ) 
{
#ifdef USE_FM
    DWORD pos = FN_GETPOS(handle_);
    if (pos == FPIFILESIZE_ERROR)
        return FALSE;
    if (pos > LONG_MAX)
        return FALSE;
    if (!fmSetFileSize(handle_, (long)pos))
        return FALSE;
    return TRUE;
#else
    return _chsize(handle_, tell(handle_)) != -1;
#endif
}

DWORD FPIFILEIOFN FN_GETATTR (FPIFILEHANDLE handle_)
{
#ifdef USE_FM
    // TODO: Use fmGetFileStat
    return FPIFILEATTR_ERROR;
#else
    return FPIFILEATTR_ERROR;
#endif
}

BOOL  FPIFILEIOFN FN_RWBOOTBLOCK (WORD drive_, WORD blocktype_, BOOL write_, LPVOID buf_)
{
#ifdef _WIN32
    if (write_)
    {
        return FALSE;       // write not yet implemented
    }

    if (blocktype_ == FPIBOOTBLOCK_MBR)
    {
        if (drive_ < 0x80)
            return FALSE;   // invalid parameter
        int nr = drive_ - 0x80;

        BOOL ret = ReadMBR((unsigned char*)buf_, nr);
        return ret;
    }
    else if (blocktype_ == FPIBOOTBLOCK_BOOT)
    {
        if (drive_ > 0x25)
            return FALSE;   // invalid parameter
        char drivespec = 'A' + drive_;

        BOOL ret = ReadFloppyBootSector((unsigned char*)buf_, drivespec);
        return ret;
    }
    else
    {
        printboth("FPI violation: Invalid parameter to RWBootBlock\n");
        nApiErrors++;
        // invalid parameter
        return FALSE;
    }
#else
    // not supported yet
    return FALSE;
#endif
}

BOOL  FPIFILEIOFN FN_RWSECTORPHYS (WORD physdrive_, WORD head_, WORD cylinder_, WORD physsector_, BOOL write_, LPVOID buf_, WORD size_)
{
    return FALSE;       // TODO: Implement
}

BOOL  FPIFILEIOFN FN_RWSECTORLOGC (WORD drive_, DWORD sector_, BOOL write_, LPVOID buf_, WORD size_)
{                                                             
#ifdef _WIN32
    HANDLE hDrive;
    int ret = 1;
    __int64 linear64 = size_ * (__int64)sector_;

    int mode = (write_ ? GENERIC_WRITE : GENERIC_READ);

    char helpbuf[12];
    sprintf(helpbuf,"\\\\.\\%c:", drive_ + 'A');

    hDrive = CreateFile(
           helpbuf,
           mode,
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
        DWORD totalcount;
        SetFilePointer(hDrive, (long) linear64, (PLONG) &linear64 + 1, FILE_BEGIN);
        if (write_)
        {
            if (!WriteFile(hDrive, buf_, size_, &totalcount,NULL) || totalcount < size_)
                ret = 0;
        }
        else
        {
            if (!ReadFile(hDrive, buf_, size_, &totalcount,NULL) || totalcount < size_)
                ret = 0;
        }
        CloseHandle(hDrive);
    }

    return ret;
#else 
    // Not supported on Win16
    return FALSE;
#endif
}

BOOL  FPIFILEIOFN FN_GETDRIVEGEOMETRY (WORD drive_, LPFPI_DISK_GEOMETRY geom_)
{
#if defined _WIN32
    HANDLE hf;
    DWORD noutbytes;

    char drivestr[32];
    
    if (drive_ < 0x80)
        sprintf(drivestr,"\\\\.\\%c:", drive_ + 'A');
    else
        sprintf(drivestr,"\\\\.\\physicaldrive%d", drive_ - 0x80 + '0');

    hf = CreateFile(drivestr, GENERIC_READ, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hf != INVALID_HANDLE_VALUE)
    {
        DeviceIoControl(hf, IOCTL_DISK_GET_DRIVE_GEOMETRY, NULL, 0, geom_, sizeof(*geom_), &noutbytes, NULL);
        CloseHandle(hf);
        return TRUE;
    }

    return FALSE;
#else 
    // Not supported on Win16
    return FALSE;
#endif
}


#ifdef TRACK_FM
int* count = NULL;
int ncount = 0;
char trackname[300]="";

void BeginTrackFMExtra(const char* name)
{
	strcpy(trackname, name);
}

void TrackFMExtra(FPIFILEHANDLE handle, long pos, DWORD bytestoread)
{
	if (strcmp(fmGetFileName(handle), trackname) != 0)
		return;

	int lastpos = pos + bytestoread - 1;
	if (lastpos >= ncount)
	{
		int newn = lastpos+100000;
		count = (int*)realloc(count, newn * sizeof(int));
		for (int j=ncount; j<newn; j++)
			count[j] = 0;
		ncount = newn;
	}

	for (int i=pos; i<=lastpos; i++)
	{
		count[i]++;
	}
}

void EndTrackFMExtra(const char* name)
{
	int i,j;
	printf("=== File access statistics:\n");
	i = 0;
	while (i<ncount)
	{
		for (j=i+1; j<ncount; j++)
		{
			if (count[j] != count[i])
				break;
		}
		printf("%d\t%d\t%d\t%d\n", i, j-1, j-i, count[i]);
		i = j;
	}
	/*
	for (int i=0; i<ncount; i++)
	{
		printf("%d\t%d\n", i, count[i]);
	}
	*/
	printf("=== End of file access statistics\n");
}
#endif
