/**************************************************************************** 
  FILE..........: DirIteratorsNT.cpp
  COPYRIGHT.....: Copyright (c) 2000-01 F-Secure Ltd. 
  VERSION.......: 1.00 
  DESCRIPTION...: Implementaion of Dir Iterators and staff For 9x
  NOTE..........: Must be compliled for 
				  Win9x Kernel
				  

  HISTORY.......: DATE     COMMENT 
                  -------- ------------------------------------------------ 
                  2001-04-11 Created. 
****************************************************************************/

#include "DBCleanUp.h"

#if defined AVP_CLEANUP_DB_UNDER_VXD

//#include "windows.h" 

#include <vtoolsc.h>


#ifdef _DEBUG

	#include "stdio.h"

	// custom trace function
	void PrintString( char *str )
	{
		char *buf = (char *)new char[1024];
		if( buf == NULL )
		{
			return;
		}

		strcpy(buf, "DB Clean: ");
		strcat(buf, str);
		dprintf( buf );
		delete		buf;

	}

	void DBClenupMemchkStart(  )
	{
	}

	void DBClenupMemchkEnd(  )
	{
	}

#endif


void *DBClenupAlloc( int Size )
{
	return (void *)new BYTE[ Size ];
}

void  DBClenupFree(  void* tobe_freed )
{
	delete [] tobe_freed;
}


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


// Windows 9X specific functions
int filefindfirst(const char * path_, FILEFINDDATA * finddata_);
int filefindnext(int handle_, FILEFINDDATA * finddata_);
int filefindclose(int handle_);



// helper structure to store dir iteration values
struct DIR_ITERATOR_HELPER_WIN_9X
{
	int			  find_file_handle;
    FILEFINDDATA* pFindData;
};


AVP_CLEANUP_DB_RETURN_VALUE  CALL_CONV 
	DirIteratorFindFirst( char *Directory, HANDLE *Iterator, char **FileInDir, int *is_directory )
{
    FILEFINDDATA* pFindData;

	char *path_with_mask = (char *)DBClenupAlloc ( strlen( Directory ) + strlen( avc_mask ) + 2  );

	if(path_with_mask == NULL )
		return AVP_CLEANUP_DB_FAILURE;

	strcpy( path_with_mask, Directory);
	strcat( path_with_mask, avc_mask);


    pFindData = (FILEFINDDATA*) DBClenupAlloc( sizeof(FILEFINDDATA) );
    if (pFindData == NULL)
	{
		DBClenupFree( path_with_mask );
		return AVP_CLEANUP_DB_FAILURE;
	}
	
	DIR_ITERATOR_HELPER_WIN_9X *pHelper;
	pHelper  = (DIR_ITERATOR_HELPER_WIN_9X *)DBClenupAlloc( sizeof(DIR_ITERATOR_HELPER_WIN_9X) );
	if( pHelper == NULL )
	{
		DBClenupFree( path_with_mask );
		DBClenupFree( pFindData );
		return AVP_CLEANUP_DB_FAILURE;
	}



    int    hFF;

	hFF  = filefindfirst( path_with_mask,  pFindData	);

	if( hFF  == -1 )
	{
		DBClenupFree( path_with_mask );
		DBClenupFree( pFindData );

		return AVP_CLEANUP_DB_FAILURE;
	}

	DBClenupFree( path_with_mask );

	pHelper -> find_file_handle = hFF;
	pHelper -> pFindData	    = pFindData;

	

	// output of findfirst handle
	*Iterator = (HANDLE)pHelper;

	if( (pFindData -> fFileAttributes &  FILEATTR_DIREC ) || 
		 pFindData -> fFileName[0] == '.' ) //looking for a file
	{
		*is_directory = 1;
	}
	else
	{
		*is_directory = 0;
	}


	//copy the target size is bigger then source (MAX_PATH)
	strcpy( *FileInDir, pFindData -> fFileName );
	return AVP_CLEANUP_DB_OK ;


}

AVP_CLEANUP_DB_RETURN_VALUE  CALL_CONV 
	DirIteratorFindNext( HANDLE Iterator, char **FileInDir, int *is_directory )
{

	DIR_ITERATOR_HELPER_WIN_9X *pHelper = (DIR_ITERATOR_HELPER_WIN_9X *)Iterator;


	if( filefindnext( pHelper->find_file_handle ,  pHelper->pFindData  ) == -1 ) 
		return AVP_CLEANUP_DB_FAILURE; //no more files

	if( (pHelper -> pFindData -> fFileAttributes &  FILEATTR_DIREC ) || 
		 pHelper -> pFindData -> fFileName[0] == '.' ) //looking for a file
		*is_directory = 1;
	else
		*is_directory = 0;


	strcpy( *FileInDir, pHelper->pFindData -> fFileName );
	return AVP_CLEANUP_DB_OK ;

}

AVP_CLEANUP_DB_RETURN_VALUE  CALL_CONV 
	DirIteratorFindClose( HANDLE Iterator )
{

	DIR_ITERATOR_HELPER_WIN_9X *pHelper = (DIR_ITERATOR_HELPER_WIN_9X *)Iterator;

	filefindclose( pHelper->find_file_handle );

	DBClenupFree( pHelper->pFindData  );
	DBClenupFree( pHelper );

	return AVP_CLEANUP_DB_OK ;
}

static void ConvertWin32FindDataToFileFindData(_WIN32_FIND_DATA_BCS * win32_finddata, FILEFINDDATA * finddata_)
{
    finddata_->fFileAttributes = win32_finddata->dwFileAttributes;

    finddata_->fWin32FileTimeCreation.fLowDataTime  = win32_finddata->ftCreationTime.dwLowDateTime;
    finddata_->fWin32FileTimeCreation.fHighDataTime = win32_finddata->ftCreationTime.dwHighDateTime;

    finddata_->fWin32FileTimeLastAccess.fLowDataTime  = win32_finddata->ftLastAccessTime.dwLowDateTime;
    finddata_->fWin32FileTimeLastAccess.fHighDataTime = win32_finddata->ftLastAccessTime.dwHighDateTime;

    finddata_->fWin32FileTimeLastWrite.fLowDataTime  = win32_finddata->ftLastWriteTime.dwLowDateTime;
    finddata_->fWin32FileTimeLastWrite.fHighDataTime = win32_finddata->ftLastWriteTime.dwHighDateTime;

    finddata_->fFileSizeHigh    = win32_finddata->nFileSizeHigh;
    finddata_->fFileSizeLow     = win32_finddata->nFileSizeLow;
    strncpy(finddata_->fFileName, (char *)win32_finddata->cFileName, MAX_PATH);
    strncpy(finddata_->fShortFileName, (char *)win32_finddata->cAlternateFileName, 14);    

}


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
            FILEERROR_NOPATH
            FILEERROR_NMFILE
*/
int filefindfirst(const char * path_, FILEFINDDATA * finddata_)
{
    HANDLE handle;
    WORD error, matchattrs;   
    _WIN32_FIND_DATA_BCS win32_finddata;

    // NOTE: we must must clear FILE_ATTRIBUTE_LABEL otherwise we
    //       get only volume label when doing R0_FindFirstFile()
    //       on a network drive
    matchattrs = FILE_ATTRIBUTE_EVERYTHING & ~FILE_ATTRIBUTE_LABEL;
            
     handle = R0_FindFirstFile((PCHAR)path_, matchattrs, &win32_finddata, &error);
    
    if (!error)
        {
        ConvertWin32FindDataToFileFindData(&win32_finddata, finddata_);
        return (int) handle;
        }


    return -1;    
}


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
int filefindnext(int handle_, FILEFINDDATA * finddata_)
{
    WORD error;
    BOOL success;

    _WIN32_FIND_DATA_BCS win32_finddata;


	success = R0_FindNextFile((HANDLE)handle_, &win32_finddata, &error);

    if (success)
        {
        ConvertWin32FindDataToFileFindData(&win32_finddata, finddata_);        
        return 0;
        }



    return -1;        
}


/*
    filefindclose()

    Closes a search handle.

    Parameters:
        handle_                     : search handle returned by filefindfirst()

    Returns:
        0 on success, -1 on error.

*/
int filefindclose(int handle_)
{
    WORD error;
    BOOL success;

     success = R0_FindCloseFile((HANDLE)handle_, &error);
    
	 return success ? 0: -1;
}


/*-------------------------------------------------------------------------*
* Under AVP95 Vxd string routines doen't included into the project 
* use private
--------------------------------------------------------------------------*/
char MY_toupper( char c )
{
	// I suppose that there is no non english chars in the DB file name
    if ( (c >= 'a') && (c <= 'z') )
            c = c - ('a' - 'A');
    return c;
}
 
char * MY_strchr (
        const char * string,
        int ch
        )
{
        while (*string && *string != (char)ch)
                string++;

        if (*string == (char)ch)
                return((char *)string);
        return(NULL);
}
char *  MY_strrchr (
        const char * string,
        int ch
        )
{
        char *start = (char *)string;

        while (*string++)                       /* find end of string */
                ;
                                                /* search towards front */
        while (--string != start && *string != (char)ch)
                ;

        if (*string == (char)ch)                /* char found ? */
                return( (char *)string );

        return(NULL);
}


#endif
