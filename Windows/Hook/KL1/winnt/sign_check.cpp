
#include "sign_check.h"


extern "C" {    
    #include "..\..\..\..\CommonFiles\Dskm\dskm.h"
};

#include "..\sign\sigdet\sigdet.h"
#include "pe.h"

#define  CRYPTOC_SIGN

#ifdef CRYPTOC_SIGN

#ifdef _DEBUG
extern "C" int __cdecl _CrtDbgReport(
        __in int _ReportType,
        __in_z_opt const char * _Filename,
        __in int _Linenumber,
        __in_z_opt const char * _ModuleName,
        __in_z_opt const char * _Format,
        ...)
{
    return 0;
}
extern "C" int __cdecl _CrtDbgReportW(
        __in int _ReportType,
        __in_z_opt const wchar_t * _Filename,
        __in int _LineNumber,
        __in_z_opt const wchar_t * _ModuleName,
        __in_z_opt const wchar_t * _Format,
        ...)
{
    return 0;
}
#endif //_DEBUG

void*
__cdecl
Kl1Alloc( AVP_size_t size )
{
    return ExAllocatePoolWithTag( NonPagedPool, size, '-1lk' );
}

void
__cdecl
Kl1Free( void* mem )
{
    if ( mem )
    {
        ExFreePool( mem );
    }
}

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

bool
SignCheck( 
          PCHAR pBasePtr, 
          ULONG DrvSize,
          wchar_t* ModuleName
          )
{
    ULONG Size = DrvSize;
    PCHAR pKey1, pKey2, pSign;
    ULONG Key1Size, Key2Size, SignSize, FileSize;

    CheckForSignOld( pBasePtr, Size, &Size );
        
    if  ( ! GetCryptoCPtrs( pBasePtr, Size, &pKey1, &Key1Size, &pKey2, &Key2Size, &pSign, &SignSize, &FileSize ) )
        return false;

    PIMAGE_DOS_HEADER   Dos_header = (PIMAGE_DOS_HEADER)pBasePtr;
    if (FileSize < sizeof(IMAGE_DOS_HEADER))
        return false;
    if ((ULONG)Dos_header->e_lfanew > FileSize)
        return false;

    PIMAGE_NT_HEADERS   PE_header = (PIMAGE_NT_HEADERS)( (char*)pBasePtr + Dos_header->e_lfanew );
    if ((ULONG)Dos_header->e_lfanew + sizeof(IMAGE_NT_HEADERS) > FileSize)
        return false;

    PE_header->OptionalHeader.CheckSum = 0;

    bool RetVal = false;
    HDSKM libDSKM;
    ULONG dskm_err = DSKM_ERR_OK ;

    libDSKM = DSKM_InitLibraryEx(Kl1Alloc, Kl1Free, NULL, TRUE) ;
    if (libDSKM)
    {
        HDSKMLIST list1;
        dskm_err = DSKM_ParList_Create(&list1);
        if (dskm_err == DSKM_ERR_OK)
        {
            CReadObj Key1( pKey1, Key1Size );
            CReadObj Key2( pKey2, Key2Size );
            CReadObj Sign( pSign, SignSize );
            
            HDSKMLISTOBJ param1 = DSKM_ParList_AddBufferedReg(list1, 0, Key1.m_pBase, Key1.m_Size, (pfnDSKM_GetBuffer_CallBack)ReadBuffByFile, &Key1 ) ;
            HDSKMLISTOBJ param2 = DSKM_ParList_AddBufferedReg(list1, 0, Key2.m_pBase, Key2.m_Size, (pfnDSKM_GetBuffer_CallBack)ReadBuffByFile, &Key2 ) ;
            HDSKMLISTOBJ param3 = DSKM_ParList_AddBufferedReg(list1, 0, Sign.m_pBase, Sign.m_Size, (pfnDSKM_GetBuffer_CallBack)ReadBuffByFile, &Sign ) ;
            if (param1 && param2 && param3)
            {
                dskm_err = DSKM_PrepareRegsSet(libDSKM, list1, 0);

                if (dskm_err == DSKM_ERR_OK)
                {
                    // Check

                    HDSKMLIST list2 = 0 ;
                    dskm_err = DSKM_ParList_Create(&list2);
                    if (dskm_err == DSKM_ERR_OK)
                    {
                        CReadObj File( pBasePtr, FileSize );

                        HDSKMLISTOBJ param4 = DSKM_ParList_AddBufferedObject(list2, 0, File.m_pBase, File.m_Size, (pfnDSKM_GetBuffer_CallBack)ReadBuffByFile, &File ) ;
                        if (param4)
                        {
                            dskm_err = DSKM_ParList_SetObjectHashingProp(list2, param4, ModuleName, 2 *( wcslen ( ModuleName ) + 1 ) ) ;
                            if (dskm_err == DSKM_ERR_OK)
                            {
                                dskm_err = DSKM_CheckObjectsUsingRegsSet( libDSKM, list2, 0) ;

                                if ( dskm_err == DSKM_ERR_OK )
                                {
                                    RetVal = true;
                                    /*
                                    AVP_dword ret;
                                    retsize = sizeof(AVP_dword);
                                    DSKM_ParList_GetObjectProp(list2, param4, DSKM_OBJ_PROCESSING_ERROR, &ret, &retsize);
                                    */
                                }
                            }
                        }
                    }
                    DSKM_ParList_Delete(list2) ;
                }
            }
            DSKM_ParList_Delete(list1) ;
        }
        DSKM_DeInitLibrary( libDSKM, TRUE ) ;
    }

    return RetVal;
}

#else // CRYPTOC_SIGN

/*
#include "sign_check.h"
#include "pe.h"

#ifndef _WIN64

extern "C"
{
    void __cdecl free(void * m)
    {
        KL_MEM_FREE( m );
    }
    void *  __cdecl malloc(size_t s)
    {
        return KL_MEM_ALLOC( s );
    }


    long __cdecl time(long *)
    {
        return 0;
    }


    DWORD GetTickCount(VOID )
    {
        return 0;
    }

#include "..\..\..\..\CommonFiles\Sign\sign.h"
}

bool
SignCheck(PVOID pBasePtr, ULONG Size)
{
    int s;
    int r = 0;
    PIMAGE_DOS_HEADER   Dos_header = (PIMAGE_DOS_HEADER)pBasePtr;
    PIMAGE_NT_HEADERS   PE_header = (PIMAGE_NT_HEADERS)( (char*)pBasePtr + Dos_header->e_lfanew );

    PE_header->OptionalHeader.CheckSum = 0;

    r = sign_check_buffer( pBasePtr, Size, &s, 1,0,0, 0 );

    return ( r == SIGN_OK ) ? true : false;
}

#endif // _WIN64

*/
#endif // CRYPTOC_SIGN