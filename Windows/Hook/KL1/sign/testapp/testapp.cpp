// testapp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

extern "C" {    
    #include "..\..\..\..\..\CommonFiles\Dskm\dskm.h"
};

ULONG
GetFileSize( const char* FileName )
{
    ULONG  dwSize;
    HANDLE hFile;
    
    hFile = CreateFileA( FileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

    if ( hFile == INVALID_HANDLE_VALUE )
        return -1;
    
    dwSize = GetFileSize (hFile, NULL) ; 

    CloseHandle ( hFile );

    return dwSize;
}

int
MyReadFile( char* filename, std::vector<char>& Data, ULONG* pSize )
{
    *pSize = GetFileSize( filename );

    FILE* ff = fopen ( filename, "rb" );
    std::vector<char> TheFile( *pSize );

    fread( &TheFile[0], 1, *pSize, ff );

    fclose( ff );

    Data = TheFile;

    return *pSize;
}

std::vector<char> g_File;

void*
__cdecl
Kl1Alloc( AVP_size_t size )
{
    return malloc(size);
}

void
__cdecl
Kl1Free( void* mem )
{
    free(mem);
}

struct CReadObj {
    PCHAR m_pBase;
    ULONG m_Size;
    ULONG m_Offset;

    CReadObj( PCHAR base, ULONG size ) : m_Offset(0), m_pBase(base), m_Size(size) {};


    ULONG Read( PCHAR TargetBuffer, ULONG Size )
    {
        ULONG CopySize = Size;

        if ( m_Offset + Size > m_Size )
            CopySize = m_Size - m_Offset;

        if ( CopySize )
        {
            memcpy ( TargetBuffer, m_pBase + m_Offset, CopySize );
            m_Offset += CopySize;
            return CopySize;
        }

        return 0;
    }
};

static int DSKMAPI ReadBuffByFile(CReadObj *Object, void *pvBuffer, AVP_dword dwBufferSize, AVP_dword dwCommand)
{
    if (Object == 0)
    {
        return -1 ;
    }

    if ( dwCommand == DSKM_CB_OPEN )
    {   
        Object->m_Offset = 0;
        return 0 ;
    }

    return Object->Read( (PCHAR)pvBuffer, dwBufferSize );
}

int main(int argc, char* argv[])
{
    wchar_t* ModuleName = L"klick.sys";
    
    ULONG FileSize;

    MyReadFile ( argv[1], g_File, &FileSize );

    PCHAR pKey1, pKey2, pSign;
    ULONG Key1Size, Key2Size, RegSize;
    char *pBasePtr = &g_File[0];

    CheckForSignOld( pBasePtr, g_File.size(), &FileSize );        

    GetCryptoCPtrs( pBasePtr, FileSize, &pKey1, &Key1Size, &pKey2, &Key2Size, &pSign, &RegSize, &FileSize );        
    
    PIMAGE_DOS_HEADER   Dos_header = (PIMAGE_DOS_HEADER)pBasePtr;
    PIMAGE_NT_HEADERS   PE_header = (PIMAGE_NT_HEADERS)( (char*)pBasePtr + Dos_header->e_lfanew );

    PE_header->OptionalHeader.CheckSum = 0;

    bool RetVal = false;
    HDSKM libDSKM;
    ULONG dskm_err = DSKM_ERR_OK ;

    libDSKM = DSKM_InitLibraryEx(Kl1Alloc, Kl1Free, NULL, TRUE) ;

    HDSKMLIST list1;
    dskm_err = DSKM_ParList_Create(&list1);

    CReadObj Key1( pKey1, Key1Size );
    CReadObj Key2( pKey2, Key2Size );
    CReadObj Sign( pSign, RegSize );
    
    HDSKMLISTOBJ param1 = DSKM_ParList_AddBufferedReg(list1, 0, Key1.m_pBase, Key1.m_Size, (pfnDSKM_GetBuffer_CallBack)ReadBuffByFile, &Key1 ) ;
    HDSKMLISTOBJ param2 = DSKM_ParList_AddBufferedReg(list1, 0, Key2.m_pBase, Key2.m_Size, (pfnDSKM_GetBuffer_CallBack)ReadBuffByFile, &Key2 ) ;
    HDSKMLISTOBJ param3 = DSKM_ParList_AddBufferedReg(list1, 0, Sign.m_pBase, Sign.m_Size, (pfnDSKM_GetBuffer_CallBack)ReadBuffByFile, &Sign ) ;

    dskm_err = DSKM_PrepareRegsSet(libDSKM, list1, 0);

    dskm_err = DSKM_ParList_Delete(list1) ;

    // Check

    HDSKMLIST list2 = 0 ;
    DSKM_ParList_Create(&list2);

    CReadObj File( pBasePtr, FileSize );

    HDSKMLISTOBJ param4 = DSKM_ParList_AddBufferedObject(list2, 0, File.m_pBase, File.m_Size, (pfnDSKM_GetBuffer_CallBack)ReadBuffByFile, &File ) ;

    dskm_err = DSKM_ParList_SetObjectHashingProp(list2, param4, ModuleName, 2 *( wcslen ( ModuleName ) + 1 ) ) ;

    dskm_err = DSKM_CheckObjectsUsingRegsSet( libDSKM, list2, 0) ;

    if ( dskm_err == DSKM_ERR_OK )
        RetVal = true;

    DSKM_ParList_Delete(list2) ;

    DSKM_DeInitLibrary( libDSKM, TRUE ) ;

	return 0;
}
