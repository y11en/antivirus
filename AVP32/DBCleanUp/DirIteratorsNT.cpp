/**************************************************************************** 
  FILE..........: DirIteratorsNT.cpp
  COPYRIGHT.....: Copyright (c) 2000-01 F-Secure Ltd. 
  VERSION.......: 1.00 
  DESCRIPTION...: Implementaion of Dir Iterators and staff For NT 
  NOTE..........: Must be compliled for 
				  WinNT Kernel
				  

  HISTORY.......: DATE     COMMENT 
                  -------- ------------------------------------------------ 
                  2001-04-11 Created. 
****************************************************************************/

#include "DBCleanUp.h"

#if defined AVP_CLEANUP_DB_UNDER_WINNTKERNEL

extern "C" //this is needed as FSAVP 4002 expects C conversions
{

#include <ntddk.h>
#define NT_INCLUDED //to avoid types redefinition
#include <basetsd.h>
#include <windef.h>
	
	NTSYSAPI
	NTSTATUS
	NTAPI
	ZwCreateEvent (
		OUT PHANDLE EventHandle,
		IN ACCESS_MASK DesiredAccess,
		IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
		IN EVENT_TYPE EventType,
		IN BOOLEAN InitialState
		);


	NTKERNELAPI NTSTATUS
	ZwQueryDirectoryFile(
		IN HANDLE FileHandle,
		IN HANDLE Event OPTIONAL,
		IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
		IN PVOID ApcContext OPTIONAL,
		OUT PIO_STATUS_BLOCK IoStatusBlock,
		OUT PVOID FileInformation,
		IN ULONG Length,
		IN FILE_INFORMATION_CLASS FileInformationClass,
		IN BOOLEAN ReturnSingleEntry,
		IN PUNICODE_STRING FileName OPTIONAL,
		IN BOOLEAN RestartScan
		);


	// NtQueryDirectoryFile return types:
	//
	//      FILE_DIRECTORY_INFORMATION
	//      FILE_FULL_DIR_INFORMATION
	//      FILE_BOTH_DIR_INFORMATION
	//      FILE_NAMES_INFORMATION
	//

	typedef struct _FILE_DIRECTORY_INFORMATION {
		ULONG NextEntryOffset;
		ULONG FileIndex;
		LARGE_INTEGER CreationTime;
		LARGE_INTEGER LastAccessTime;
		LARGE_INTEGER LastWriteTime;
		LARGE_INTEGER ChangeTime;
		LARGE_INTEGER EndOfFile;
		LARGE_INTEGER AllocationSize;
		ULONG FileAttributes;
		ULONG FileNameLength;
		WCHAR FileName[1];
	} FILE_DIRECTORY_INFORMATION, *PFILE_DIRECTORY_INFORMATION;

	typedef struct _FILE_FULL_DIR_INFORMATION {
		ULONG NextEntryOffset;
		ULONG FileIndex;
		LARGE_INTEGER CreationTime;
		LARGE_INTEGER LastAccessTime;
		LARGE_INTEGER LastWriteTime;
		LARGE_INTEGER ChangeTime;
		LARGE_INTEGER EndOfFile;
		LARGE_INTEGER AllocationSize;
		ULONG FileAttributes;
		ULONG FileNameLength;
		ULONG EaSize;
		WCHAR FileName[1];
	} FILE_FULL_DIR_INFORMATION, *PFILE_FULL_DIR_INFORMATION;

	typedef struct _FILE_BOTH_DIR_INFORMATION {
		ULONG NextEntryOffset;
		ULONG FileIndex;
		LARGE_INTEGER CreationTime;
		LARGE_INTEGER LastAccessTime;
		LARGE_INTEGER LastWriteTime;
		LARGE_INTEGER ChangeTime;
		LARGE_INTEGER EndOfFile;
		LARGE_INTEGER AllocationSize;
		ULONG FileAttributes;
		ULONG FileNameLength;
		ULONG EaSize;
		CCHAR ShortNameLength;
		WCHAR ShortName[12];
		WCHAR FileName[1];
	} FILE_BOTH_DIR_INFORMATION, *PFILE_BOTH_DIR_INFORMATION;

	typedef struct _FILE_NAMES_INFORMATION {
		ULONG NextEntryOffset;
		ULONG FileIndex;
		ULONG FileNameLength;
		WCHAR FileName[1];
	} FILE_NAMES_INFORMATION, *PFILE_NAMES_INFORMATION;

}


struct DIR_ITERATOR_HELPER
{
	PFILE_BOTH_DIR_INFORMATION m_DirInfo;
	PBYTE					   m_Info;
	PBYTE					   m_InfoPos;
	HANDLE					   m_Handle;
};

#define MAX_FILE_LENGTH		4096
#define MAX_DIR_INFO_LENGTH	4096


#ifdef _DEBUG

	#include "stdio.h"

	// custom trace function
	void PrintString( char *str )
	{
		char *buf = (char *)ExAllocatePool(NonPagedPool, 1024 );
		if( buf == NULL )
		{
			DbgPrint( "Cannt alloc buf for print\n"  );
			return;
		}

		strcpy(buf, "DB Clean: ");
		strcat(buf, str);
		strcat(buf, "\n" );
		DbgPrint( buf );
		

		ExFreePool(buf);
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

#endif


void *DBClenupAlloc( int Size )
{
#ifdef _DEBUG

	// first int will keep a size
	void *pPtr =  ExAllocatePool(NonPagedPool,  Size + sizeof(int)   );
	if( pPtr == NULL )
		return pPtr; 
		
	gDBClenupMemTrack += Size;

	// save size
	RtlCopyMemory(pPtr, &Size, sizeof(Size) );
	return ((char *)pPtr) + sizeof(Size);

#else
	return ExAllocatePool(NonPagedPool,  Size  );

#endif

}

void  DBClenupFree(  void* tobe_freed )
{
#ifdef _DEBUG

	int Size = 0;

	// first int keeps a size, restore org pointer
	void *pPtr =  ((char *)tobe_freed) - sizeof(Size);
	RtlCopyMemory(&Size, pPtr, sizeof(Size) );

	// decrement the size
	gDBClenupMemTrack -= Size;

	// clean or memory
	ExFreePool( pPtr );

#else
	ExFreePool( tobe_freed );
#endif
}


void FillEntry( DIR_ITERATOR_HELPER *pHelper, char **FileInDir, int *is_directory )
{
	UNICODE_STRING uniFileName;
	ANSI_STRING    ansiFilename;

	*is_directory = 1;

	if ( pHelper -> m_DirInfo )
	{
		PWCHAR Null = (PWCHAR)((PBYTE) pHelper -> m_DirInfo->FileName + pHelper -> m_DirInfo->FileNameLength);
		
		// the name is WCHAR that is *not* NULL terminated
		// to be safe we copy it to a new NULL terminated buffer and process that
		PWCHAR buf = (PWCHAR)DBClenupAlloc( pHelper -> m_DirInfo->FileNameLength + sizeof(WCHAR) );
		if( buf )
		{

			RtlZeroMemory(buf, pHelper -> m_DirInfo->FileNameLength + sizeof(WCHAR));
			RtlCopyMemory(buf, pHelper -> m_DirInfo->FileName, pHelper -> m_DirInfo->FileNameLength);


			// Allocate for filename, path, optional backslash and the terminating zero
			// However, Unicode->MBCS conversion may not exactly halve the space requirements,
			// so we allocate room for the worst case (2 bytes per each character)

			RtlInitUnicodeString( &uniFileName, buf );
			RtlUnicodeStringToAnsiString( &ansiFilename, &uniFileName, TRUE );

			strncpy( *FileInDir, ansiFilename.Buffer, ansiFilename.Length);
			(*FileInDir)[ansiFilename.Length] = 0;

			RtlFreeAnsiString( &ansiFilename );
			DBClenupFree( buf );
					
			if( pHelper -> m_DirInfo->FileAttributes & FILE_ATTRIBUTE_DIRECTORY ) 
				*is_directory   = 1;
			else
				*is_directory   = 0;
		}
	}
}



AVP_CLEANUP_DB_RETURN_VALUE  CALL_CONV 
	DirIteratorFindFirst( char *Directory, HANDLE *Iterator, char **FileInDir, int *is_directory )
{

	NTSTATUS	status;
	ANSI_STRING ansiString;
	OBJECT_ATTRIBUTES objectAttributes;
        
	IO_STATUS_BLOCK ioStatus;
	ioStatus.Status = 0;
	ioStatus.Information = 0;
	AVP_CLEANUP_DB_RETURN_VALUE ReturnValue = AVP_CLEANUP_DB_FAILURE;

	DIR_ITERATOR_HELPER *pHelper = NULL;

	HANDLE DirHandle = NULL;

	PBYTE					   pInfo    = NULL;
	PWCHAR UnicodeStringBuffer			= NULL;

	__try
	{
		pHelper  = (DIR_ITERATOR_HELPER *)DBClenupAlloc( sizeof(DIR_ITERATOR_HELPER) );
		if( pHelper == NULL )
		{
			__leave;
		}

		RtlZeroMemory(pHelper, sizeof(DIR_ITERATOR_HELPER) );

		RtlInitAnsiString(&ansiString, Directory);
		
		UNICODE_STRING UnicodeString;
		UnicodeString.MaximumLength = (strlen(Directory)+1) * sizeof(WCHAR);

		UnicodeStringBuffer = (PWCHAR)DBClenupAlloc( UnicodeString.MaximumLength );
		if( UnicodeStringBuffer == NULL )
		{
			__leave;
		}

		UnicodeString.Buffer = UnicodeStringBuffer;
		objectAttributes.ObjectName = &UnicodeString;

		status = RtlAnsiStringToUnicodeString(objectAttributes.ObjectName, &ansiString, FALSE);

		objectAttributes.Length					  = sizeof(OBJECT_ATTRIBUTES);
		objectAttributes.RootDirectory			  = NULL;
		objectAttributes.Attributes				  = OBJ_CASE_INSENSITIVE;
		objectAttributes.SecurityDescriptor		  = NULL;
		objectAttributes.SecurityQualityOfService = NULL;
		

		status = ZwCreateFile(
					&DirHandle,			
					FILE_READ_DATA,		
					&objectAttributes,	
					&ioStatus,			
					NULL,				// AllocationSize
					FILE_ATTRIBUTE_DIRECTORY,
					FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
					FILE_OPEN,
					FILE_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT,
					NULL,				// EaBuffer
					0);					// EaLength



		if (NT_SUCCESS(status)) //dir was successfully opened
		{
			pInfo = (PBYTE)DBClenupAlloc( MAX_DIR_INFO_LENGTH );

			status = ZwQueryDirectoryFile(
						DirHandle,
						NULL,
						NULL,
						NULL,
						&ioStatus,
						pInfo,
						MAX_DIR_INFO_LENGTH,
						FileBothDirectoryInformation,
						FALSE,
						NULL,	// defaults search value to *
						TRUE);


			if (NT_SUCCESS(status) && status != STATUS_NO_MORE_FILES && 
									  status != STATUS_NO_SUCH_FILE  )
			{
				pHelper -> m_DirInfo = (PFILE_BOTH_DIR_INFORMATION)pInfo;
				pHelper -> m_Info	 = pInfo;
				pHelper -> m_InfoPos = pInfo;
				pHelper -> m_Handle  = DirHandle;


				FillEntry( pHelper, FileInDir, is_directory );
				ReturnValue = AVP_CLEANUP_DB_OK;
			}
		}
		else
		{
			DirHandle = NULL; //cannot open a dir
		}
	}
	__finally
	{
		if( UnicodeStringBuffer )
			DBClenupFree( UnicodeStringBuffer );

		if( ReturnValue == AVP_CLEANUP_DB_OK )
		{
				*Iterator = (HANDLE)pHelper;
		}
		else
		{
			if( pHelper ) 
				DBClenupFree( pHelper );

			if( pInfo ) 
				DBClenupFree( pInfo );
			
			if( DirHandle )
				ZwClose( DirHandle ) ;

			*Iterator = NULL;
		}

	}

	return ReturnValue ;

}

AVP_CLEANUP_DB_RETURN_VALUE  CALL_CONV 
	DirIteratorFindNext( HANDLE Iterator, char **FileInDir, int *is_directory )
{

	NTSTATUS status = STATUS_SUCCESS;
	IO_STATUS_BLOCK ioStatus;
	ioStatus.Status		 = 0;
	ioStatus.Information = 0;
	DIR_ITERATOR_HELPER *pHelper = (DIR_ITERATOR_HELPER *)Iterator;

	AVP_CLEANUP_DB_RETURN_VALUE ReturnValue = AVP_CLEANUP_DB_FAILURE;

	if (  pHelper -> m_DirInfo )
	{
		if ( pHelper -> m_DirInfo->NextEntryOffset )
			pHelper -> m_InfoPos += pHelper -> m_DirInfo->NextEntryOffset;
		else
			pHelper -> m_InfoPos = 0;

		pHelper -> m_DirInfo = (PFILE_BOTH_DIR_INFORMATION)pHelper -> m_InfoPos;
	}

	if (NULL != pHelper -> m_DirInfo)
	{
		FillEntry( pHelper, FileInDir, is_directory );
		ReturnValue = AVP_CLEANUP_DB_OK;
	}
	else
	{
		status = ZwQueryDirectoryFile(
			        pHelper -> m_Handle,
				    NULL,
					NULL,
					NULL,
					&ioStatus,
					pHelper -> m_Info,
					MAX_DIR_INFO_LENGTH,
					FileBothDirectoryInformation,
					FALSE,
					NULL,
					FALSE);

		if (NT_SUCCESS(status) && status != STATUS_NO_MORE_FILES && 
								  status != STATUS_NO_SUCH_FILE  )
		{
			pHelper -> m_DirInfo = (PFILE_BOTH_DIR_INFORMATION)pHelper -> m_Info;
			pHelper -> m_InfoPos = pHelper -> m_Info;

			FillEntry( pHelper, FileInDir, is_directory );
			ReturnValue = AVP_CLEANUP_DB_OK;
		}

	}

	return ReturnValue ;
}

AVP_CLEANUP_DB_RETURN_VALUE  CALL_CONV 
	DirIteratorFindClose( HANDLE Iterator )
{
	DIR_ITERATOR_HELPER *pHelper = (DIR_ITERATOR_HELPER *)Iterator;

	if( pHelper -> m_Info )
		DBClenupFree( pHelper -> m_Info );

	if( pHelper -> m_Handle )
		ZwClose( pHelper -> m_Handle ) ;

	if( pHelper ) 
		DBClenupFree( pHelper );

			

	return AVP_CLEANUP_DB_OK ;
}

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
