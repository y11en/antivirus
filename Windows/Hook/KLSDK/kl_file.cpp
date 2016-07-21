#include "kl_file.h"

CKl_File::CKl_File(char* filename /* = NULL */) : m_CurrentFile(filename)
{
    m_Handle        = NULL;
    m_ReadOffset    = 0;
    m_WriteOffset   = 0;
    m_FileState     = FILE_STATE_CLOSED;
}

CKl_File::~CKl_File()
{
}

#ifdef USER_MODE
//-------------------------------------------
//------------- USER_MODE	Section  --------
//-------------------------------------------
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>


KLSTATUS
CKl_File::Open(char* filename /* = NULL */)
{
    m_CurrentFile = filename ? filename : m_CurrentFile;

    if ( !m_CurrentFile || strlen( m_CurrentFile ) > 900 ) 
        return KL_UNSUCCESSFUL;

    m_Handle = CreateFile(  m_CurrentFile, 
                            GENERIC_READ | GENERIC_WRITE | SYNCHRONIZE , 
                            FILE_SHARE_READ,
                            NULL,
                            OPEN_ALWAYS,
                            FILE_ATTRIBUTE_NORMAL,
                            NULL);

    if ( m_Handle != INVALID_HANDLE_VALUE )
    {
        m_FileState = FILE_STATE_OPENED;

        m_ReadOffset  = 0;
        m_WriteOffset = 0;
        return KL_SUCCESS;
    }
#ifdef USER_MODE
    DWORD   err = GetLastError();
#endif

    return KL_UNSUCCESSFUL;
}

KLSTATUS
CKl_File::Close()
{
    if ( m_FileState != FILE_STATE_CLOSED )
    {
        m_FileState = FILE_STATE_CLOSED;
        CloseHandle(m_Handle);
    }
    
    return KL_SUCCESS;
}

KLSTATUS
CKl_File::Read(char* DstBuffer, ULONG Offset, ULONG BytesToRead, ULONG* BytesRead )
{
    ULONG   bRead;
    Seek ( Offset );
    
    if ( ReadFile( m_Handle, DstBuffer, BytesToRead, &bRead, NULL ) )
    {
        if ( bRead == 0)
            return KL_UNSUCCESSFUL;
        
        if ( BytesRead )        
            *BytesRead = bRead;        

        return KL_SUCCESS;
    }    

    return KL_UNSUCCESSFUL;
}

KLSTATUS
CKl_File::Write(char* SrcBuffer, ULONG Offset, ULONG BytesToWrite, ULONG* BytesWritten )
{
    ULONG   bWritten;
    Seek ( Offset );
    
    if ( WriteFile( m_Handle, SrcBuffer, BytesToWrite, &bWritten, NULL ) )
    {
        if ( BytesWritten )
            *BytesWritten = bWritten;

        return KL_SUCCESS;
    }    

    return KL_UNSUCCESSFUL;
}

KLSTATUS
CKl_File::Seek(ULONG Offset)
{
    SetFilePointer ( m_Handle, Offset, 0, FILE_BEGIN );


    m_WriteOffset   = Offset;
    m_ReadOffset    = Offset;
    
    return KL_SUCCESS;
}

ULONG
CKl_File::GetSize()
{
    return GetFileSize( m_Handle, NULL );    
}

#else // USER_MODE

#ifndef ISWIN9X
//-------------------------------------------
//------------- WINNT	Section  ------------
//-------------------------------------------
KLSTATUS
CKl_File::Open(wchar_t* filename /* = NULL */)
{
    IO_STATUS_BLOCK     ioStatus;
    NTSTATUS            Status;
    UNICODE_STRING      FileName;
    OBJECT_ATTRIBUTES   ObjAttr;
    wchar_t             Buf[1000];

    m_CurrentFileW = filename ? filename : m_CurrentFileW;

    if ( !m_CurrentFileW || wcslen( m_CurrentFileW ) > 900 ) 
        return KL_UNSUCCESSFUL;

    swprintf ( Buf, L"\\??\\%s", m_CurrentFileW);

    RtlInitUnicodeString(&FileName, Buf);
    
    InitializeObjectAttributes(&ObjAttr, &FileName, OBJ_CASE_INSENSITIVE, NULL, NULL);	

    Status = ZwCreateFile(
        &m_Handle, 
        FILE_READ_DATA | FILE_WRITE_DATA | SYNCHRONIZE, 
        &ObjAttr, 
        &ioStatus, 
        NULL, 
        FILE_ATTRIBUTE_NORMAL,
        FILE_SHARE_READ,
        FILE_OPEN_IF,
        FILE_SYNCHRONOUS_IO_NONALERT,
        NULL,
        0
        );

    if ( NT_SUCCESS(Status) )
    {
        m_FileState = FILE_STATE_OPENED;

        m_ReadOffset  = 0;
        m_WriteOffset = 0;
        return KL_SUCCESS;
    }
    
    return KL_UNSUCCESSFUL;
}

KLSTATUS
CKl_File::Open(char* filename /* = NULL */)
{
    NTSTATUS            Status;    
    OBJECT_ATTRIBUTES   ObjAttr;	
    UNICODE_STRING      FileName;
    IO_STATUS_BLOCK     ioStatus;
    ANSI_STRING         AnsiStr;
    char                Buf[1000];

    m_CurrentFile = filename ? filename : m_CurrentFile;

    if ( !m_CurrentFile || strlen( m_CurrentFile ) > 900 ) 
        return KL_UNSUCCESSFUL;

    sprintf ( Buf, "\\??\\%s", m_CurrentFile);

    RtlInitAnsiString           ( &AnsiStr,  Buf );

    RtlAnsiStringToUnicodeString( &FileName, &AnsiStr, TRUE  );
    
    InitializeObjectAttributes(&ObjAttr, &FileName, OBJ_CASE_INSENSITIVE, NULL, NULL);	
    
    Status = ZwCreateFile(
        &m_Handle, 
        FILE_READ_DATA | FILE_WRITE_DATA | SYNCHRONIZE, 
        &ObjAttr, 
        &ioStatus, 
        NULL, 
        FILE_ATTRIBUTE_NORMAL,
        FILE_SHARE_READ,
        FILE_OPEN_IF,
        FILE_SYNCHRONOUS_IO_NONALERT,
        NULL,
        0
        );

    RtlFreeUnicodeString( &FileName );

    if ( NT_SUCCESS(Status) )
    {
        m_FileState = FILE_STATE_OPENED;

        m_ReadOffset  = 0;
        m_WriteOffset = 0;
        return KL_SUCCESS;
    }
    
    return KL_UNSUCCESSFUL;
}

KLSTATUS
CKl_File::Close()
{
    if ( m_FileState != FILE_STATE_CLOSED )
    {
        m_FileState = FILE_STATE_CLOSED;
        ZwClose( m_Handle );

        return KL_SUCCESS;
    }
    
    return KL_UNSUCCESSFUL;
}

KLSTATUS
CKl_File::Read(char* DstBuffer, ULONG Offset, ULONG BytesToRead, ULONG* BytesRead )
{
    NTSTATUS            Status;
    IO_STATUS_BLOCK     ioStatus;
    LARGE_INTEGER       qwOffset = { Offset, 0 };

    Status = ZwReadFile(m_Handle, NULL, NULL, NULL, &ioStatus, DstBuffer, BytesToRead, &qwOffset, NULL);

    if ( NT_SUCCESS( Status ) )
    {
        if ( BytesRead )        
            *BytesRead = (ULONG)ioStatus.Information;
        
        m_ReadOffset = (ULONG) ( Offset + ioStatus.Information );

        return KL_SUCCESS;
    }

    return KL_UNSUCCESSFUL;
}

KLSTATUS
CKl_File::Write(char* SrcBuffer, ULONG Offset, ULONG BytesToWrite, ULONG* BytesWritten )
{
    NTSTATUS            Status;
    IO_STATUS_BLOCK     ioStatus;
    LARGE_INTEGER       qwOffset = { Offset, 0 };

    Status = ZwWriteFile( m_Handle, NULL, NULL, NULL, &ioStatus, SrcBuffer, BytesToWrite, &qwOffset, NULL );

    if ( NT_SUCCESS(Status) )
    {
        m_WriteOffset = Offset + BytesToWrite;
        
        if ( BytesWritten )
            *BytesWritten = BytesToWrite;

        return KL_SUCCESS;
    }

    return KL_UNSUCCESSFUL;
}

KLSTATUS
CKl_File::Seek(ULONG Offset)
{
    m_WriteOffset   = Offset;
    m_ReadOffset    = Offset;

    return KL_SUCCESS;
}

ULONG
CKl_File::GetSize()
{
    NTSTATUS                    Status;
    FILE_STANDARD_INFORMATION   fi;
    IO_STATUS_BLOCK             ioStatus;

    Status = ZwQueryInformationFile(m_Handle, &ioStatus, &fi, sizeof (fi), FileStandardInformation);


    if ( NT_SUCCESS(Status))
    {
        return fi.EndOfFile.LowPart;
    }

    return 0xFFFFFFFF;
}

#else
//-------------------------------------------
//------------- WIN9X	Section  ------------
//-------------------------------------------
KLSTATUS
CKl_File::Open(char* filename /* = NULL */)
{
    WORD Error;
    BYTE Action;
    
    m_CurrentFile = filename ? filename : m_CurrentFile;

    if ( !m_CurrentFile || strlen( m_CurrentFile ) > 900 ) 
        return KL_UNSUCCESSFUL;
    
    m_Handle = R0_OpenCreateFile(
        TRUE, 
        m_CurrentFile, 
        OPEN_ACCESS_READWRITE | OPEN_SHARE_COMPATIBLE | OPEN_FLAGS_COMMIT,
        ATTR_NORMAL,
        ACTION_IFNOTEXISTS_CREATE | ACTION_IFEXISTS_OPEN,
        0,
        &Error,
        &Action);

    if ( Error == 0 )
    {
        m_FileState = FILE_STATE_OPENED;

        m_ReadOffset  = 0;
        m_WriteOffset = 0;
        return KL_SUCCESS;
    }
    
    return KL_UNSUCCESSFUL;
}

ULONG
CKl_File::GetSize()
{
    WORD    Error;
    ULONG   size;

    size = R0_GetFileSize(m_Handle, &Error);

    if ( Error == 0 )
    {
        return size;
    }

    return 0xFFFFFFFF;
}

KLSTATUS
CKl_File::Close()
{
    WORD    Error;

    R0_CloseFile(m_Handle, &Error);

    if ( Error == 0 )
    {
        m_FileState = FILE_STATE_CLOSED;
        return KL_SUCCESS;
    }

    return KL_UNSUCCESSFUL;
}

KLSTATUS
CKl_File::Seek(ULONG Offset)
{
    m_WriteOffset   = Offset;
    m_ReadOffset    = Offset;

    return KL_SUCCESS;
}

KLSTATUS
CKl_File::Read(char* DstBuffer, ULONG Offset, ULONG BytesToRead, ULONG* BytesRead )
{
    DWORD   bRead;
    WORD    Error;

    bRead = R0_ReadFile( FALSE, m_Handle, DstBuffer, BytesToRead, Offset, &Error);

    if ( Error == 0 )
    {
        if ( BytesRead )
            *BytesRead = bRead;

        m_ReadOffset = Offset + bRead;
        
        return KL_SUCCESS;
    }

    return KL_UNSUCCESSFUL;
}

KLSTATUS
CKl_File::Write(char* SrcBuffer, ULONG Offset, ULONG BytesToWrite, ULONG* BytesWritten )
{
    DWORD   Written;
    WORD    Error;

    Written = R0_WriteFile( FALSE, m_Handle, SrcBuffer, BytesToWrite, Offset, &Error );

    if ( Error == 0 )
    {
        if ( BytesWritten )
        {
            *BytesWritten = BytesToWrite;
        }
        m_WriteOffset = Offset + BytesToWrite;
        return KL_SUCCESS;
    }
    
    return KL_UNSUCCESSFUL;
}

#endif

#endif // USER_MODE