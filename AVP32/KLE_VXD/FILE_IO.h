/*
    fileio.h

    Copyright (c) 1997, Data Fellows, Ltd.

    F-PROT

    Project     : Apollo
    Module      : File I/O
    File        : Module Interface

    This header file presents the function prototypes and other elements
    of the File I/O API.  This API defines a platform-independent interface to
    file I/O routines.

    Notes:


    History:
            970209  : Urmas Rahu        Created

*/

#ifndef __FILEIO_H
#define __FILEIO_H

#ifdef __cplusplus
extern "C" {
#endif


// file open access flags

#define FO_RDONLY                   0x0001
#define FO_WRONLY                   0x0002
#define FO_RDWR                     0x0004

#define FO_CREATE                   0x0100
#define FO_EXCL                     0x0400

#define FO_GKAWARE                  0x1000

// file open sharing flags

#define FO_SHARE_COMPAT             0x0000
#define FO_SHARE_DENYRW             0x0010
#define FO_SHARE_DENYALL            FO_SHARE_DENYRW
#define FO_SHARE_DENYWR             0x0020
#define FO_SHARE_DENYRD             0x0030
#define FO_SHARE_DENYNONE           0x0040


// file seek base values

#define FILESEEK_SET                 0
#define FILESEEK_CUR                 1
#define FILESEEK_END                 2


// file attributes

#define FILEATTR_RDONLY             0x0001
#define FILEATTR_HIDDEN             0x0002
#define FILEATTR_SYSTEM             0x0004
#define FILEATTR_LABEL              0x0008
#define FILEATTR_DIREC              0x0010
#define FILEATTR_ARCH               0x0020
#define FILEATTR_TEMPORARY          0x0100          // (not supported on all paltforms)
#define FILEATTR_COMPRESSED         0x0800          // (not supported on all paltforms)


// max filename length

#define FILENAME_MAXLEN             256


// error codes (values derived from DOS error codes)

#define FILEERROR_OK                 0              // no error
#define FILEERROR_NOFILE             2              // no such file
#define FILEERROR_MFILE              4              // too many open files
#define FILEERROR_ACCESS             5              // access denied
#define FILEERROR_BADF               6              // invalid file handle
#define FILEERROR_BADH               6              // invalid handle (same as FILEERROR_BADF)
#define FILEERROR_INVACC            12              // invalid access code
#define FILEERROR_INVDRV            15              // invalid drive
#define FILEERROR_NODEV             15              // no such device (same as FILEERROR_INVDRV)
#define FILEERROR_NOTSAM            17              // not the same device
#define FILEERROR_NMFILE            18              // no more files
#define FILEERROR_INVAL             19              // invalid argument


/*
    fileerror()

    Returns the error code (FILEERROR_xxx) of the last file I/O call.
    The error value is maintained in per-thread basis.

    Parameters:
        None

    Returns:
        Last error code (FILEERROR_OK if no error).
*/
int fileerror(void);
    


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
int fileopen(const char * path_, int access_, int sharemode_);



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
int fileclose(int handle_);



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
long fileseek(int handle_, long offset_, int fromwhere_);



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
unsigned long filerdwr(int handle_, void * buf_, unsigned long len_, int write_);


/*
    fileread() and filewrite()

    These macros translate to filerdwr() calls.  See filerdwr() documentation
    for parameters and return values information.
*/
#define fileread(handle_, buf_, len_) (filerdwr((handle_), (buf_), (len_), 0))
#define filewrite(handle_, buf_, len_) (filerdwr((handle_), (buf_), (len_), 1))



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
unsigned long filegetsize(int handle_);



/*
    filedelete()

    Deletes a file.

    Parameters:
        path_                       : points to the path name of the file

    Returns:
        0 on success, -1 on error.           

    Error codes:
        On error, fileerror() returns one of:
            FILEERROR_ACCESS
            FILEERROR_NOFILE
*/
int filedelete(const char * path_);


/*
    filerename()

    Renames a file.

    Parameters:
        oldname_                    : points to the current path name of the file
        newname_                    : points to the new path name of the file

    Returns:
        0 on success, -1 on error.           

    Error codes:
        On error, fileerror() returns one of:
            FILEERROR_NOFILE
            FILEERROR_ACCESS
            FILEERROR_NOTSAM

    Notes:
        The directories in <oldname_> and <newname_> may be different, but
        they must be on the same drive.

*/
int filerename(const char * oldname_, const char * newname_);



/*
    fileattributes()

    Queries or sets file attributes.

    Parameters:
        path_                       : points to the path name of the file
        set_                        : nonzero to set the attributes, zero to query
        attr_                       : if <set_> is nonzero, contains the new file
                                      attributes (a combination of FILEATTR_xxx values)
    Returns:
        File attributes on success, -1 on error.           

    Error codes:
        On error, fileerror() returns one of:
            FILEERROR_NOFILE
            FILEERROR_ACCESS
*/
int fileattributes(const char * path_, int set_, int attr_);



/*
    WIN32FILETIME

    File time data in Win32 format: a 64-bit value representing the number of
    100-nanosecond intervals since January 1, 1601.
*/
typedef struct WIN32FILETIME_struct
    {
    unsigned long   fLowDataTime;                   // lower 32 bits
    unsigned long   fHighDataTime;                  // higher 32 bits 
    }
    WIN32FILETIME;

/*
    FILEFINDDATA

    The structure being filled by filefindfirst() and filefindnext().
*/
typedef struct FILEFINDDATA_struct
    {
    int             fFileAttributes;                // file attributes (a combination of FILEATTR_xxx values)
    unsigned long   fDosFileTime;                   // file time in DOS format, (unsigned long)-1L if not valid
    unsigned long   fDosFileDate;                   // file date in DOS format, (unsigned long)-1L if not valid
    WIN32FILETIME   fWin32FileTimeCreation;         // file creation time (Win32 only)
    WIN32FILETIME   fWin32FileTimeLastAccess;       // last access time (Win32 only)
    WIN32FILETIME   fWin32FileTimeLastWrite;        // last write time (Win32 only)
    unsigned long   fFileSizeHigh;                  // high 32 bits of file size
    unsigned long   fFileSizeLow;                   // low 32 bits of file size
    char            fFileName [FILENAME_MAXLEN];    // file name (empty in 16-bit Windows/DOS)
    char            fShortFileName [14];            // file name in 8.3 format 
    }
    FILEFINDDATA;


/*
    filefindfirst()

    Searches a directory for a file whose name matches the specified filename.

    Parameters:
        path_                       : points to a string that specifies a valid directory or
                                      path and filename, which can contain wildcards
        finddata_                   : on success, gets filled with the information about the found file  

    Returns:
        Handle to the opened search, -1 on error.  The handle is to be passed to subsequent
        calls to filefindnext() and filefindclose().

    Error codes:
        On error, fileerror() returns one of:
            FILEERROR_NOFILE
            FILEERROR_NMFILE
*/
int filefindfirst(const char * path_, FILEFINDDATA * finddata_);


/*
    filefindnext()

    Continues a file search from a previous call to filefindfirst().

    Parameters:
        handle_                     : search handle returned by filefindfirst()
        finddata_                   : on success, gets filled with the information about the found file

    Returns:
        0 on success, -1 on error.

    Error codes:
        On error, fileerror() returns one of:
            FILEERROR_BADH
            FILEERROR_NMFILE
*/
int filefindnext(int handle_, FILEFINDDATA * finddata_);


/*
    filefindclose()

    Closes a search handle.

    Parameters:
        handle_                     : search handle returned by filefindfirst()

    Returns:
        0 on success, -1 on error.

    Error codes:
        On error, fileerror() returns one of:
            FILEERROR_BADH
*/
int filefindclose(int handle_);



/*
    fileexists()

    Determines whether a file or directory matching the specified pathname exists.

    Parameters:
        path_                       : points to a string that specifies a valid directory or
                                      path and filename, which can contain wildcards
    Returns:
        Nonzero if a matching file or directory is found, zero if not.

    Error codes:
        If no match is found, fileerror() returns:
            FILEERROR_NOFILE
*/
int fileexists(const char * path_);


/*
    diskgetfreespace()

    Determines the amount of free space on given drive.

    Parameters:
        rootpath_                   : points to a string that specifies the root directory of the
                                      disk to return information about
        secPerClu_                  : receives the number of sectors per cluster
        bytPerSec_                  : receives the number of bytes per sector
        cluFree_                    : receives the number of free clusters
        cluTotal_                   : receives the total number of clusters

    Returns:
        Zero on success, nonzero on error.

    Error codes:
        On error, fileerror() returns one of:
           FILEERROR_INVDRV
*/
int diskgetfreespace(const char * rootpath_, unsigned long * secPerClu_, unsigned long * bytPerSec_,
                     unsigned long * cluFree_, unsigned long * cluTotal_);



#ifdef __cplusplus
}
#endif

#endif //__FILEIO_H
