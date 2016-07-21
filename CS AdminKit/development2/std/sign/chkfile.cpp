#include <std/base/klstd.h>
#include <std/memory/klmem.h>
#include <std/thr/locks.h>

#include "../avp/sign/sigdet/sigdet.h"
#include "dskm/dskm.h"

#define KLCS_MODULENAME L"KLSTDSIGN"

using namespace KLSTD;

namespace KLSTDSIGN
{
    namespace
    {
        void* __cdecl Alloc( AVP_size_t size )
        {
            return KLSTD_AllocTmp(size);
        };

        void __cdecl Free( void* mem )
        {
            if ( mem )
            {
                KLSTD_FreeTmp( mem );
            };
        };

        struct CReadObj
        {
            CReadObj(
                        void*   pData,
                        size_t  nData)
                :   m_pBase(pData)
                ,   m_Size(nData)
                ,   m_nPos(0)
            {;}

            void*   m_pBase;
            size_t  m_Size;
            size_t  m_nPos;
        };
    
        KLSTD::CAutoPtr<KLSTD::CriticalSection> g_pCS;
    };

    void DoInitialize()
    {
        g_pCS = KLSTD::CreateCS();
    };

    void DoDeinitialize()
    {
        g_pCS = NULL;
    };

    void KLSTD_FASTCALL ThrowSignFail(int nSubCode, const wchar_t* szwFileName, const char* file, int line)
    {
        KLERR_TRY
            KLERR_throwError(L"KLSTD", KLSTD::STDE_SIGNFAIL, file, line, NULL, szwFileName);
        KLERR_CATCH(pError)
            KLERR_SAY_FAILURE(1, pError);
            pError->SetErrorSubcode(nSubCode);
            KLERR_RETHROW();
        KLERR_ENDTRY
    };

    void KLSTD_FASTCALL CheckDskm(AVP_dword dwError, const wchar_t* szwFileName, const char* file, int line)
    {
        if(dwError && DSKM_ERR_OK != dwError)
        {
            ThrowSignFail(dwError, szwFileName, file, line);
        };
    };

    void KLSTD_FASTCALL DskmLeCheck(bool bOK, const wchar_t* szwFileName, HDSKMLIST hList, const char* file, int line)
    {
        if(!bOK)
        {
            AVP_dword dwCode = DSKM_ParList_GetLastError(hList);
            if(!dwCode)
                dwCode = DSKM_UNKNOWN_ERROR;
            ThrowSignFail(dwCode, szwFileName, file, line);
        };
    };

    #define KLSTDSIGN_CHKDSKM(_code) CheckDskm((_code), szwFileName, __FILE__, __LINE__)

    #define KLSTDSIGN_THROW(_code) ThrowSignFail((_code), szwFileName, __FILE__, __LINE__)
    
    #define KLSTDSIGN_LECHK(_val, _list) DskmLeCheck(!!(_val), szwFileName, (_list), __FILE__, __LINE__)
        
        

    void DoCheckFileSignature(const wchar_t* szwFileName)
    {
        int nCode = 0;
        KLSTD_CHK(szwFileName, szwFileName && szwFileName[0]);
        HDSKMLIST list1 = NULL;
        HDSKMLIST list2 = NULL;

        KLSTD::AutoCriticalSection acs(g_pCS);

        HDSKM hHDSKM = NULL;
        KLERR_TRY
            hHDSKM = DSKM_InitLibraryEx(Alloc, Free, NULL, true) ;
            CAutoPtr<File> pFile;
            KLSTD_CreateFile(
                        szwFileName, 
                        SF_READ, 
                        CF_OPEN_EXISTING, 
                        AF_READ, 
                        EF_SEQUENTIAL_SCAN, 
                        &pFile);
            const AVP_qword qwSize = pFile->GetSize();
            if(qwSize > INT_MAX/16)
            {
                KLSTDSIGN_THROW(KLSTD::STDE_MOREDATA);
            };
            if(0 == qwSize)
            {
                KLSTDSIGN_THROW(KLSTD::STDE_BADFORMAT);
            };
            CAutoPtr<FileMapping> pFileMapping;
            pFile->Map(AF_READ, 0, (AVP_dword)qwSize, &pFileMapping);
            ;
            void* pData = pFileMapping->GetPointer();
            size_t nData = (size_t)pFileMapping->GetSize();
            ;
            char*        pKey1 = NULL;
            AVP_dword    nKey1Size = 0;

            char*        pKey2 = NULL;
            AVP_dword    nKey2Size = 0;

            char*        pReg = NULL;
            AVP_dword    nRegSize = 0;

            AVP_dword    nFileSize = 0;

            if(!GetCryptoCPtrs(
                        (char*)pData, 
                        nData, 
                        &pKey1,
                        &nKey1Size,
                        &pKey2,
                        &nKey2Size,
                        &pReg,
                        &nRegSize,
                        &nFileSize))
            {
                KLSTDSIGN_THROW(KLSTD::STDE_BADFORMAT);
            };
            KLSTDSIGN_CHKDSKM(DSKM_ParList_Create(&list1));

            CReadObj Key1( pKey1, nKey1Size );
            CReadObj Key2( pKey2, nKey2Size );
            CReadObj Sign( pReg, nRegSize );

            HDSKMLISTOBJ param1 = DSKM_ParList_AddBufferedReg(
                                    list1, 
                                    0, 
                                    Key1.m_pBase, 
                                    Key1.m_Size, 
                                    NULL, 
                                    NULL );
            KLSTDSIGN_LECHK(param1, list1);

            HDSKMLISTOBJ param2 = DSKM_ParList_AddBufferedReg(
                                    list1, 
                                    0, 
                                    Key2.m_pBase, 
                                    Key2.m_Size, 
                                    NULL, 
                                    NULL );
            KLSTDSIGN_LECHK(param2, list1);

            HDSKMLISTOBJ param3 = DSKM_ParList_AddBufferedReg(
                                    list1, 
                                    0, 
                                    Sign.m_pBase, 
                                    Sign.m_Size, 
                                    NULL, 
                                    NULL );
            KLSTDSIGN_LECHK(param3, list1);

            KLSTDSIGN_CHKDSKM(DSKM_PrepareRegsSet(hHDSKM, list1, 0));

            KLSTDSIGN_CHKDSKM(DSKM_ParList_Create(&list2));

            CReadObj File( pData, nFileSize );
            HDSKMLISTOBJ param4 = DSKM_ParList_AddBufferedObject(
                                    list2, 
                                    0, 
                                    File.m_pBase, 
                                    File.m_Size, 
                                    NULL, 
                                    NULL ) ;
            KLSTDSIGN_LECHK(param4, list2);

            std::wstring wstrFileName2Hash;
            {
                std::wstring wstrDir, wstrFile, wstrExt;
                KLSTD_SplitPath(szwFileName, wstrDir, wstrFile, wstrExt);
                wstrFileName2Hash = wstrFile + wstrExt;
                _wcslwr(&*wstrFileName2Hash.begin());
            };

            KLSTDSIGN_CHKDSKM(DSKM_ParList_SetObjectHashingProp(
                                    list2, 
                                    param4, 
                                    const_cast<wchar_t*>(wstrFileName2Hash.c_str()), 
                                    (wstrFileName2Hash.size() + 1) * sizeof(wchar_t) ) ) ;

            KLSTDSIGN_CHKDSKM(DSKM_CheckObjectsUsingRegsSet( hHDSKM, list2, 0));
        KLERR_CATCH(pError)
            KLERR_SAY_FAILURE(4, pError);
        KLERR_FINALLY
            if(list2)
                DSKM_ParList_Delete(list2);
            if(list1)
                DSKM_ParList_Delete(list1);

            if(hHDSKM)
            {
                AVP_dword dwCode = DSKM_DeInitLibrary(hHDSKM, true) ;
                KLSTD_ASSERT(DSKM_ERR_OK == dwCode);
                hHDSKM = NULL;
            };
            KLERR_RETHROW();
        KLERR_ENDTRY
    };
};
