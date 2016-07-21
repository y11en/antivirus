/**************************************************************************** 
  FILE..........: DBCleanUp.cpp
  COPYRIGHT.....: Copyright (c) 2000-01 F-Secure Ltd. 
  VERSION.......: 1.00 
  DESCRIPTION...: Implementaion deletion of unused AVP files, generic logic
  NOTE..........: Must be compliled for 
				  Win32 
				  WinNT Kernel
				  Win95 Vxd
				  

  HISTORY.......: DATE     COMMENT 
                  -------- ------------------------------------------------ 
                  2001-04-11 Created. 
****************************************************************************/

#include "DBCleanUp.h"

#if defined AVP_CLEANUP_DB_UNDER_VXD 
	#include <windows.h>

#define my__try 
#define my__finally  finally:
#define my__leave     goto finally;


#elif defined AVP_CLEANUP_DB_UNDER_WINNTKERNEL

	#include <ntddk.h>
#define my__try  __try
#define my__finally  __finally
#define my__leave     __leave;

#elif defined AVP_CLEANUP_DB_UNDER_DLL

	#include <windows.h>
#define my__try  __try
#define my__finally  __finally
#define my__leave     __leave;

#endif


extern "C"
{
	#include <_avpio.h>
}

#include "stdio.h"



#define MAX_AVP_SET_STRING_LENGTH 0x200


/*-----------------------------------------------------------
*	AVP Set funtions
-----------------------------------------------------------*/

AVP_CLEANUP_DB_RETURN_VALUE  CALL_CONV 
	AVPCleanUpUnUsedFiles(const char *strAVPSetFile, const char *strAVPDBPath);

AVP_CLEANUP_DB_RETURN_VALUE  CALL_CONV 
	OpenAVPSetFile( const char *strAVPSetFile, HANDLE  *pAVPSetFileHandle  );

AVP_CLEANUP_DB_RETURN_VALUE  CALL_CONV 
	LookUpFile( HANDLE  pAVPSetFileHandle, const char *strFileToLookUp );

AVP_CLEANUP_DB_RETURN_VALUE  CALL_CONV 
	CloseAVPSetFile( HANDLE  pAVPSetFileHandle );

/*-----------------------------------------------------------
*	Directory interaction funtions, platform specific
-----------------------------------------------------------*/

AVP_CLEANUP_DB_RETURN_VALUE  CALL_CONV 
	DirIteratorFindFirst( char *Directory, HANDLE *Iterator, char **FileInDir, int *is_directory );

AVP_CLEANUP_DB_RETURN_VALUE  CALL_CONV 
	DirIteratorFindNext( HANDLE Iterator, char **FileInDir, int *is_directory);

AVP_CLEANUP_DB_RETURN_VALUE  CALL_CONV 
	DirIteratorFindClose( HANDLE Iterator );

/*-----------------------------------------------------------
*	Delete file funtions
-----------------------------------------------------------*/
AVP_CLEANUP_DB_RETURN_VALUE  CALL_CONV 
	DeleteDatabaseFile( const char *directory, const char *file );


/*----------------------------------------------------------
*  Support routines 
----------------------------------------------------------*/
int			MyReadString(char* buf,DWORD size, HANDLE f);
int			IsEqualStringIgnoreCase( const char *str1, const char *str2 );
BOOL		IsExtensionOKA(   const CHAR* filename_, const CHAR* extensionlist_) ;
int			FindExtensionMatchCIA(const CHAR * F, const CHAR * L) ;

/*----------------------------------------------------------
*  String routines, platform specific 
----------------------------------------------------------*/
char *  MY_strchr (const char * string, int ch);
char *  MY_strrchr ( const char * string, int ch );
char	MY_toupper( char c );



/*----------------------------------------------------------
*  memory support routines, platform specific 
----------------------------------------------------------*/
void *DBClenupAlloc( int Size );
void  DBClenupFree(  void* tobe_freed );

// check memory leak
#ifdef _DEBUG
	void DBClenupMemchkStart(  );
	void DBClenupMemchkEnd(  );
#else
	#define DBClenupMemchkStart() ;
	#define DBClenupMemchkEnd() ;
#endif


/*----------------------------------------------------------
*  Debug support
----------------------------------------------------------*/
#ifdef _DEBUG
	void PrintString( char *str );
#else //non debug
	#define  PrintString( x ) ;
#endif


/*------------------------------------------------------------
 *
 * NAME:        
 *
 * DESCRIPTION: 
 *				
 *				
 *
 * RETURNS:     
 *
 * NOTES:       
 *
 *------------------------------------------------------------*/

AVP_CLEANUP_DB_RETURN_VALUE  CALL_CONV 
	AVPCleanUpUnUsedFiles( const char *strAVPSetFile )
{
	HANDLE  hAVPSetFileHandle = INVALID_HANDLE_VALUE;
	HANDLE  DirIterator       = INVALID_HANDLE_VALUE;

	char*						short_file_name;
	char*						strDBDir	 = NULL;
	char*						strFileInDir = NULL;
	AVP_CLEANUP_DB_RETURN_VALUE ReturnValue  = AVP_CLEANUP_DB_OK;

	DBClenupMemchkStart();

	my__try
	{
		strDBDir = (char *)DBClenupAlloc ( MAX_AVP_SET_STRING_LENGTH  );
		if( strDBDir == NULL )
		{
			ReturnValue = AVP_CLEANUP_DB_INTERNAL_ERROR;
			my__leave;
		}

		strFileInDir = (char *)DBClenupAlloc (  MAX_AVP_SET_STRING_LENGTH );
		if( strFileInDir == NULL )
		{
			ReturnValue = AVP_CLEANUP_DB_INTERNAL_ERROR;
			my__leave;
		}

		if( AvpGetFullPathName(strAVPSetFile, MAX_AVP_SET_STRING_LENGTH, strDBDir, &short_file_name) == 0 )
		{
			ReturnValue = AVP_CLEANUP_DB_INTERNAL_ERROR  ;
			my__leave;
		}

		// create DB path trail slash(\) is availiable 
		strDBDir[ short_file_name - strDBDir ] = 0;

		if( OpenAVPSetFile( strAVPSetFile, &hAVPSetFileHandle) != AVP_CLEANUP_DB_OK )
		{
			ReturnValue = AVP_CLEANUP_DB_INTERNAL_ERROR ;
			my__leave;
		}

		int is_directory ;
		if( DirIteratorFindFirst( strDBDir, &DirIterator, &strFileInDir, &is_directory ) != AVP_CLEANUP_DB_OK )
		{
			ReturnValue = AVP_CLEANUP_DB_INTERNAL_ERROR ; //no file in dir?!
			my__leave;
		}

		// main routine for directory iteration
		do
		{
			PrintString( strFileInDir );
			if( is_directory == 0 && IsExtensionOKA(strFileInDir, database_extension_list)) //non directory and match extension
			{
				// check that file is listed in avp.set
			    if( LookUpFile( hAVPSetFileHandle, strFileInDir ) == AVP_CLEANUP_DB_FAILURE )
				{
					if( DeleteDatabaseFile( strDBDir, strFileInDir ) != AVP_CLEANUP_DB_OK )
						ReturnValue = AVP_CLEANUP_DB_FAILURE;
				}
			}


		}while( DirIteratorFindNext( DirIterator, &strFileInDir, &is_directory ) == AVP_CLEANUP_DB_OK);


	}
	my__finally
	{
		if( strDBDir  )
			DBClenupFree( strDBDir );
		strDBDir = NULL;

		if( strFileInDir  )
			DBClenupFree( strFileInDir );
		strFileInDir = NULL;

		if( hAVPSetFileHandle != INVALID_HANDLE_VALUE )
			CloseAVPSetFile( hAVPSetFileHandle );

		hAVPSetFileHandle = INVALID_HANDLE_VALUE ;

		if( DirIterator      != INVALID_HANDLE_VALUE )
			DirIteratorFindClose( DirIterator );

		DirIterator		 = INVALID_HANDLE_VALUE;
	}
	
	DBClenupMemchkEnd();


	return ReturnValue ;
} 
  
/*-----------------------------------------------------------
*	AVP Set functions, implementatios
-----------------------------------------------------------*/



/*-----------------------------------------------------------
*	OpenAVPSetFile 
*
*	Opens AVP.SET file and returns file handle
*
-----------------------------------------------------------*/

AVP_CLEANUP_DB_RETURN_VALUE  CALL_CONV 
OpenAVPSetFile( const char *strAVPSetFile, HANDLE  *pAVPSetFileHandle )
{


	*pAVPSetFileHandle = AvpCreateFile( strAVPSetFile,
									GENERIC_READ,
									FILE_SHARE_READ,
									NULL,
									OPEN_EXISTING,
									0,
									NULL);

	if( *pAVPSetFileHandle == INVALID_HANDLE_VALUE )
		return AVP_CLEANUP_DB_FAILURE;

	return AVP_CLEANUP_DB_OK;

}

/*-----------------------------------------------------------
*	CloseAVPSetFile 
*
*	Closes AVP.SET file handle
*
-----------------------------------------------------------*/
AVP_CLEANUP_DB_RETURN_VALUE  CALL_CONV 
	CloseAVPSetFile( HANDLE  pAVPSetFileHandle )
{

	if( pAVPSetFileHandle !=INVALID_HANDLE_VALUE )
		return AvpCloseHandle(pAVPSetFileHandle) == TRUE ? AVP_CLEANUP_DB_OK : AVP_CLEANUP_DB_FAILURE;
	
	return AVP_CLEANUP_DB_INVALID_PARAMETER;
}


/*-----------------------------------------------------------
*	LookUpFile 
*
*	Check that AVP.SET file contains specified string 
*		AVP_CLEANUP_DB_OK		string exits in AVP.SET
*		AVP_CLEANUP_DB_FAILURE	string not exits in AVP.SET
*		AVP_CLEANUP_DB_INTERNAL_ERROR system error
*
-----------------------------------------------------------*/
AVP_CLEANUP_DB_RETURN_VALUE  CALL_CONV 
LookUpFile( HANDLE  hAVPSetFileHandle, const char *strFileToLookUp )
{
	if( hAVPSetFileHandle == INVALID_HANDLE_VALUE )
		return AVP_CLEANUP_DB_INVALID_PARAMETER;


	// prepare string for a file
	char *file_name = (char *)DBClenupAlloc (  MAX_AVP_SET_STRING_LENGTH );
	if( file_name == NULL )
		return AVP_CLEANUP_DB_INTERNAL_ERROR;


	//go to begin
	if( AvpSetFilePointer( hAVPSetFileHandle, 0, 0, FILE_BEGIN) == 0xFFFFFFFF )
		return AVP_CLEANUP_DB_INTERNAL_ERROR;


	// string is not found by default
	AVP_CLEANUP_DB_RETURN_VALUE ReturnValue = AVP_CLEANUP_DB_FAILURE;
	while( MyReadString(file_name, MAX_AVP_SET_STRING_LENGTH , hAVPSetFileHandle) )
	{
		if( *file_name == ';')  continue;
		if( *file_name == ' ') continue;
		if( *file_name == 0 )  continue;

		if( IsEqualStringIgnoreCase( file_name, strFileToLookUp ) )
		{
			//string found
			ReturnValue = AVP_CLEANUP_DB_OK;
			break;
		}
		
	}

	DBClenupFree( file_name );

	return ReturnValue;
}


/*-----------------------------------------------------------
*	Delete file funtions
-----------------------------------------------------------*/

AVP_CLEANUP_DB_RETURN_VALUE  CALL_CONV 
	DeleteDatabaseFile( const char *directory, const char *file )
{
	char *full_file_path =  (char *)DBClenupAlloc ( strlen( directory ) + strlen( file )  + 1 );
	if( full_file_path  ==  NULL )
		return AVP_CLEANUP_DB_INTERNAL_ERROR;

	strcpy( full_file_path, directory );
	strcat( full_file_path, file );

	AVP_CLEANUP_DB_RETURN_VALUE ReturnValue = 
						AvpDeleteFile( full_file_path )  ?
						AVP_CLEANUP_DB_OK : AVP_CLEANUP_DB_FAILURE ;

	DBClenupFree( full_file_path );
	
	return AVP_CLEANUP_DB_OK;
}

/*-----------------------------------------------------------
*	Read string from a file 
-----------------------------------------------------------*/
int MyReadString(char* buf, DWORD size, HANDLE f)
{
	DWORD bread;
	*buf=0;
	if( f==INVALID_HANDLE_VALUE)return 0;
	if( !AvpReadFile(f,buf,size-1,&bread,NULL) || bread>size )return 0;
	buf[bread]=0;
	DWORD i;
	for( i=0;i<bread;i++ ,buf++){
		if(*buf==0x0D || *buf==0x0A || *buf==0){
			*buf=0;
			buf++;i++;
			if(*buf==0x0D || *buf==0x0A )i++;
			AvpSetFilePointer(f,i-bread,NULL,FILE_CURRENT);
			break;
		}
	}
	return i;
}

int IsEqualStringIgnoreCase( const char *str1, const char *str2 )
{
	if( strlen( str1 ) != strlen( str2 ) ) //no equal length
		return 0;

	while( *str1  && *str2 )
	{
		if( MY_toupper( *str1 ) != MY_toupper( *str2 ) )
			return 0;
		str1 ++; 
		str2 ++;
	}

	return 1;
}

BOOL		IsExtensionOKA(    const CHAR* filename_,
                               const  CHAR* extensionlist_) 
{
    const CHAR* pDot = ".";
    BOOL  bMatch = TRUE;
    const CHAR* pExtension;

    pExtension = MY_strrchr(filename_, '.' );    // last dot
    if (pExtension)
        pExtension++; // extension exists in filename -> jump over "."
    else
        pExtension =  pDot; // no extension in filename, pass "." to FindExtensionMatchCI()

    return FindExtensionMatchCIA(pExtension, extensionlist_);
}



int FindExtensionMatchCIA(const CHAR * F, const CHAR * L) 
{
    int i=0;

    start:

    if (!L[i])                                      // end of list
        {
        if (!F[i])
            {                                       // end of ext
            if (i)
                {                                   // not 0-length ext: match found
                return 1;
                }

            return 0;                               // match not found
            }

        return 0;                                   // ext continues: no match
        }

    if (L[i] == ' ' )
        {                                           // space in list
        if (!i)
            {                                       // find next ext in list
            L++;
            goto start;
            }

        if (!F[i])
            {                                       // ext ends at the same time: match
            return 1;
            }

        L += i;                                     // ext continues: no match yet, find next ext in list
        i = 0;
        goto start;
        }

    if (L[i] == '*' )
        {                                           // this matches all
        return 1;
        }

    // At this point, L[i] is neither '\0', ' ', nor '*'

    if (F[i])
    {                                           // file ext not finished
        if (L[i] == '?' || MY_toupper(L[i]) == MY_toupper(F[i]))
            {                                       // current char matches: move on
            i++;
            goto start;
            }

        L += i;                                     // does not match, take next ext in list
        i = 0;
        L = MY_strchr(L, ' ');                         // find the separating space
        if (!L)
        {                                       // no space: this was the last in list
            return 0;
        }

        goto start;                                 // space found: continue
    }

    // At this point, ext is ended, but not the ext in list.
    // Match found if only wildcards until the end of L 

    fend:

    if (!L[i] || L[i] == '*' || L[i] == ' ' )
        {                                           // never true in the first loop, but could be later on
        return 1;
        }

    if (L[i] == '?' )
        {                                           // take next
        i++;
        goto fend; 
        }

    // not a wildcard: this ext in list does not match

    L += i;
    i = 0;

    L = MY_strchr(L, ' ' );

    if (!L)
    {                                           // L finished
        return 0;
    }

    goto start;                                     // take next ext in list
}

////////////////////////////////////////////////////////////////////////////////////
//
//		WIN 32 DLL functions 
//
////////////////////////////////////////////////////////////////////////////////////
#if defined AVP_CLEANUP_DB_UNDER_DLL

/*-----------------------------------------------------------
*	memory funtions, implementation
*   Win 32 ONLY
-----------------------------------------------------------*/

void *DBClenupAlloc( int Size )
{
	return (void *)new BYTE[ Size ];
}

void  DBClenupFree(  void* tobe_freed )
{
	delete [] tobe_freed;
}



#ifdef _DEBUG

	#include "stdio.h"

	// custom trace function
	void PrintString( char *str )
	{
		char *buf = new char[ 1024 ];
		if( buf == NULL )
		{
			OutputDebugString( "Cannt alloc buf for print\n"  );
			return;
		}

		strcpy(buf, "DB Clean: ");
		strcat(buf, str);
		strcat(buf, "\n" );
		OutputDebugString( buf );
		

		delete buf;
	}

	// global for memory tracking
	int  gDBClenupMemTrack;

	void DBClenupMemchkStart(  )
	{
		gDBClenupMemTrack = 0;
	}

	void DBClenupMemchkEnd(  )
	{
		char test[10];
		sprintf( test, "Mem=%ld\n", gDBClenupMemTrack);
		PrintString( test );
	}

#endif //DEBUG


/*-----------------------------------------------------------
*	Directory interaction funtions, implementation
*   Win 32 ONLY
-----------------------------------------------------------*/

AVP_CLEANUP_DB_RETURN_VALUE  CALL_CONV 
	DirIteratorFindFirst( char *Directory, HANDLE *Iterator, char **FileInDir, int *is_directory )
{
	WIN32_FIND_DATA FindFileData ;

	char *path_with_mask = (char *)DBClenupAlloc ( strlen( Directory ) + strlen( avc_mask ) + 2  );

	if(path_with_mask == NULL )
		return AVP_CLEANUP_DB_FAILURE;

	strcpy( path_with_mask, Directory);
	strcat( path_with_mask, avc_mask);

	HANDLE hFF  = FindFirstFile( path_with_mask,  &FindFileData	);

	if( hFF  == INVALID_HANDLE_VALUE )
	{
		DBClenupFree( path_with_mask );
		return AVP_CLEANUP_DB_FAILURE;
	}

	DBClenupFree( path_with_mask );

	// output of findfirst handle
	*Iterator = hFF;


	if( (FindFileData.dwFileAttributes &  FILE_ATTRIBUTE_DIRECTORY) || 
		 FindFileData.cFileName[0] == '.' ) 
		*is_directory = 1;
	else
		*is_directory = 0;


	//copy the target size is bigger then source (MAX_PATH)
	strcpy( *FileInDir, FindFileData.cFileName );
	return AVP_CLEANUP_DB_OK ;

}

AVP_CLEANUP_DB_RETURN_VALUE  CALL_CONV 
	DirIteratorFindNext( HANDLE Iterator, char **FileInDir, int *is_directory )
{
	WIN32_FIND_DATA FindFileData ;

	if( FindNextFile( Iterator,  &FindFileData ) == FALSE ) 
		return AVP_CLEANUP_DB_FAILURE; //no more files

	if( (FindFileData.dwFileAttributes &  FILE_ATTRIBUTE_DIRECTORY) || 
		 FindFileData.cFileName[0] == '.' ) //looking for a file
		*is_directory = 1;
	else
		*is_directory = 0;
	

	strcpy( *FileInDir, FindFileData.cFileName );

	return AVP_CLEANUP_DB_OK ;


}

AVP_CLEANUP_DB_RETURN_VALUE  CALL_CONV 
	DirIteratorFindClose( HANDLE Iterator )
{
	return  FindClose( Iterator ) == TRUE ? AVP_CLEANUP_DB_OK : AVP_CLEANUP_DB_FAILURE;
}

/*-----------------------------------------------------------
*	String routines functions
-----------------------------------------------------------*/

char *  MY_strchr (
        const char * string,
        int ch
        )
{
	return strchr( string, ch );
}

char * MY_strrchr (
        const char * string,
        int ch
        )
{
	return strrchr( string, ch );
}

char MY_toupper( char c )
{
	return toupper(  c );
}

#endif



