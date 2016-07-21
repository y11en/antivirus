/*
    fileio.c

    File I/O library implementation.

    Platform    : Windows 95 VxD
    Required    : VC++ 4.2, VtoolsD for Windows 95 v2.04.

    Copyright (c) 1997, Data Fellows, Ltd.


    Notes:


    History:
            970912  : Urmas Rahu        Created: implemented a subset of File I/O API for Win95 VxD.


*/


#include "file_io.h"


#include <vtoolsc.h>

/*
    ==========================================================================
    Internally used TFIleData structure and functions
    ==========================================================================
*/

typedef struct TFileData_
    {
    struct TFileData_ *     fNext;

    int                     fHandle;
    int                     fAccess;
    int                     fShareMode;

    long                    fCurrPos;

    HANDLE                  fInternalHandle;
    }
    TFileData;


static TFileData * gsOpenFiles = 0;

static TFileData * TFileData_New(TFileData ** prev_)
    {
    TFileData * fd = (TFileData *)malloc(sizeof(TFileData));
    if (!fd)
        return 0;

    memset(fd, 0, sizeof(TFileData));

    if (!gsOpenFiles)
        {
        gsOpenFiles = fd;
        if (prev_)
            *prev_ = 0;
        }
    else
        {
        TFileData * current;
        for (current = gsOpenFiles; current->fNext; current = current->fNext)
            ;
        current->fNext = fd;
        if (prev_)
            *prev_ = current;
        }

    return fd;
    }

static TFileData * TFileData_Find(int handle_, TFileData ** prev_)
    {
    TFileData * current, * prev=0;

    if (!gsOpenFiles)
        return 0;

    for (current = gsOpenFiles; current; prev = current, current = current->fNext)
        if (current->fHandle == handle_)
            {
            if (prev_)
                *prev_ = prev;
            return current;
            }

    return 0;
    }

static int TFileData_GenerateHandle()
    {
    int handle = 1;

    while (TFileData_Find(handle, 0))
        ++handle;

    return handle;
    }

static void TFileData_Remove(TFileData * prev_)
    {
    TFileData * current;
    if (!prev_)
        {
        current = gsOpenFiles;
        gsOpenFiles = gsOpenFiles->fNext;
        }
    else
        {
        current = prev_->fNext;
        prev_->fNext = current->fNext;
        }

    free(current);
    }



/*
    ==========================================================================
    Internally used data
    ==========================================================================
*/

static int gsLastError = FILEERROR_OK;





/*
    ==========================================================================
    Public functions
    ==========================================================================
*/


/*
    fileerror()

    Returns the error code (FILEERROR_xxx) of the last file I/O call.
    The error value is maintained in per-thread basis.

    Parameters:
        None

    Returns:
        Last error code (FILEERROR_OK if no error).
*/
int fileerror(void)
    {
    return gsLastError;
    }



/*
    fileopen()

    Opens an existing file, or creates a new file.  The file is always
    opened in binary mode.

    Parameters:
        path_                       : points to the path name of the file
        access_                     : access mode flags, see notes below
        sharemode_                  : share mode flags, see notes below

    Returns:
        A file handle on success, -1 on error.

    Error codes:
        On error, fileerror() returns one of:
            FILEERROR_NOFILE
            FILEERROR_MFILE
            FILEERROR_ACCESS
            FILEERROR_INVACC

    Notes:
        The <access_> parameter may be a combination of the following flags:
            (1) One of FO_RDONLY, FO_WRONLY and FO_RDWR.
            (2) FO_CREATE:  If FO_CREATE is not set and the file does not exist
                then the function will fail.  If FO_CREATE is set and the file
                does not exist then the file will be created and opened.  If
                FO_CREATE is set and the file already exists then the behavior
                of the function depends on the FO_EXCL flag.
            (3) FO_EXCL: Only used with FO_CREATE.  If FO_EXCL is set then the
                function will fail if an attempt is made to create a file which
                already exists.
            (4) FO_GKAWARE: Open in Gatekeeper-aware mode: file open will be
                ignored by Gatekeeper and the file will not be scanned for
                viruses.

        The <sharemode_> parameter may be one of the following values:
            FO_SHARE_COMPAT        : other programs have access to file
            FO_SHARE_DENYRW        : other programs can't open file
            FO_SHARE_DENYALL       : same as FO_SHARE_DENYRW
            FO_SHARE_DENYWR        : other programs can't write to file
            FO_SHARE_DENYRD        : other programs can't read file
            FO_SHARE_DENYNONE      : other programs have read/write access but
                                     can't open in compatibility mode

*/
int fileopen(const char * path_, int access_, int sharemode_)
    {
    TFileData * prev;
    TFileData * fd = TFileData_New(&prev);
    WORD mode;
    BYTE action;
    BYTE ret_action;
    WORD ret_error;

    if (!fd)
        {
        gsLastError = FILEERROR_MFILE;
        return -1;
        }

    fd->fHandle = TFileData_GenerateHandle();

    gsLastError = FILEERROR_OK;

    if (access_ & FO_WRONLY)
        mode = OPEN_ACCESS_WRITEONLY;
    else if (access_ & FO_RDWR)
        mode = OPEN_ACCESS_READWRITE;
    else
        mode = OPEN_ACCESS_READONLY;

    if (sharemode_ & FO_SHARE_DENYRW)
        mode |= OPEN_SHARE_DENYREADWRITE;
    else if (sharemode_ & FO_SHARE_DENYWR)
        mode |= OPEN_SHARE_DENYWRITE	;
    else if (sharemode_ & FO_SHARE_DENYRD)
        mode |= OPEN_SHARE_DENYREAD	;
    else if (sharemode_ & FO_SHARE_DENYNONE)
        mode |= OPEN_SHARE_DENYNONE;
    else
        mode |= OPEN_SHARE_COMPATIBLE;


    if (access_ & FO_CREATE)
        {
        action = ACTION_IFNOTEXISTS_CREATE;

        if (access_ & FO_EXCL)
            action |= ACTION_IFEXISTS_FAIL;
        else
            action |= ACTION_IFEXISTS_OPEN;
        }
    else
        {
        action = ACTION_IFNOTEXISTS_FAIL | ACTION_IFEXISTS_OPEN;
        }

    fd->fInternalHandle = R0_OpenCreateFile(FALSE, (PCHAR)path_, mode, ATTR_NORMAL, action, (BYTE)R0_NO_CACHE, &ret_error, &ret_action);

    if (ret_error)
        {
        switch (ret_error)
            {
            case FILEERROR_NOFILE:
            case FILEERROR_MFILE:
            case FILEERROR_ACCESS:
            case FILEERROR_INVACC:
                gsLastError = ret_error;
                break;

            default:
                gsLastError = FILEERROR_ACCESS;
                break;
            }

        TFileData_Remove(prev);

        return -1;
        }

    fd->fAccess = access_;
    fd->fShareMode = sharemode_;

    return fd->fHandle;
    }



/*
    fileclose()

    Closes a file.

    Parameters:
        handle_                     : file handle

    Returns:
        0 on success, -1 on error.           

    Error codes:
        On error, fileerror() returns one of:
            FILEERROR_BADF
*/
int fileclose(int handle_)
    {
    TFileData * prev;
    TFileData * fd = TFileData_Find(handle_, &prev);
    WORD error;

    if (!fd)
        {
        gsLastError = FILEERROR_BADF;
        return -1;
        }

    if (!R0_CloseFile(fd->fInternalHandle, &error))
        gsLastError = FILEERROR_BADF;
    else
        gsLastError = FILEERROR_OK;


    TFileData_Remove(prev);

    return gsLastError == FILEERROR_OK ? 0 : -1;
    }



/*
    fileseek()

    Moves the file pointer.

    Parameters:
        handle_                     : file handle
        offset_                     : new offset
        fromwhere_                  : base for offset (FILESEEK_xxx)

    Returns:
        New offset from the beginning of file, -1L on error.         

    Error codes:
        On error, fileerror() returns one of:
            FILEERROR_BADF
            FILEERROR_INVAL
*/
long fileseek(int handle_, long offset_, int fromwhere_)
    {
    TFileData * fd = TFileData_Find(handle_, 0);
    DWORD filesize;
    WORD error;
    long pos;

    gsLastError = FILEERROR_OK;

    if (!fd)
        {
        gsLastError = FILEERROR_BADF;
        return -1L;
        }

    filesize = R0_GetFileSize(fd->fInternalHandle, &error);

    if (error)
        {
        gsLastError = FILEERROR_BADF;
        return -1L;
        }

    if (fromwhere_ == FILESEEK_CUR)
        pos = fd->fCurrPos + offset_;
    else if (fromwhere_ == FILESEEK_END)
        pos = filesize - offset_;
    else
        pos = offset_;

    if (pos < 0 )// commented by Graf;-) || pos > filesize)
        {
        gsLastError = FILEERROR_INVAL;
        return -1L;
        }

    fd->fCurrPos = pos;
    return pos;
    }



/*
    filerdwr()

    Reads from or writes to a file.

    Parameters:
        handle_                     : file handle
        buf_                        : read/write buffer
        len_                        : number of bytes to read or write
        write_                      : nonzero to write to the file, zero to read

    Returns:
        Number of bytes read or written, (unsigned long)-1L on error.  A return
        value which is less than the <len_> parameter indicates that the file
        ended before the desired number of bytes could be read, or that the disk
        became full before the desird number of bytes could be written.

    Error codes:
        On error, fileerror() returns one of:
            FILEERROR_BADF
            FILEERROR_ACCESS

    Notes:
        The maximum size of the buffer is 0xfffffffe bytes.
*/
unsigned long filerdwr(int handle_, void * buf_, unsigned long len_, int write_)
    {
    TFileData * fd = TFileData_Find(handle_, 0);
    DWORD bytesrw;
    WORD error;

    gsLastError = FILEERROR_OK;

    if (!fd)
        {
        gsLastError = FILEERROR_BADF;
        return (unsigned long)-1L;
        }

    if ((fd->fAccess & FO_WRONLY) && !write_ || (fd->fAccess & FO_RDONLY) && write_)
        {
        gsLastError = FILEERROR_ACCESS;
        return (unsigned long)-1L;
        }

    if (!write_)
        {
        bytesrw = R0_ReadFile(FALSE, fd->fInternalHandle, buf_, len_, fd->fCurrPos, &error);

        if (error)
            {
            gsLastError = FILEERROR_ACCESS;
            return -1L;
            }

        fd->fCurrPos += bytesrw;
        }
    else
        {
        bytesrw = R0_WriteFile(FALSE, fd->fInternalHandle, buf_, len_, fd->fCurrPos, &error);

        if (error)
            {
            gsLastError = FILEERROR_ACCESS;
            return -1L;
            }

        fd->fCurrPos += bytesrw;
        }

    return (unsigned long)bytesrw;
    }



/*
    filegetsize()

    Returns the length of an open file in bytes.

    Parameters:
        handle_                     : file handle

    Returns:
        Length of the file, or (unsigned long)-1L on error.

    Error codes:
        On error, fileerror() returns one of:
            FILEERROR_BADF
*/
unsigned long filegetsize(int handle_)
    {
    TFileData * fd = TFileData_Find(handle_, 0);
    DWORD filesize;
    WORD error;

    gsLastError = FILEERROR_OK;

    if (!fd)
        {
        gsLastError = FILEERROR_BADF;
        return (unsigned long)-1L;
        }

    filesize = R0_GetFileSize(fd->fInternalHandle, &error);

    if (error)
        {
        gsLastError = FILEERROR_BADF;
        return -1L;
        }

    return (unsigned long)filesize;
    }


