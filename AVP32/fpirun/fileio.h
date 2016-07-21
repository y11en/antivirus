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
// there is some confusion about "name" fields !!!
    char            fName [FILENAME_MAXLEN];        // file pathname
    char            fFileName [FILENAME_MAXLEN];    // file name (empty in 16-bit Windows/DOS)
    char            fShortFileName [14];            // file name in 8.3 format 
    }
    FILEFINDDATA;

#ifdef __cplusplus
}
#endif

#endif //__FILEIO_H
