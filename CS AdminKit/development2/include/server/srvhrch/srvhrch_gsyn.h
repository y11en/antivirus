#ifndef __SRVHRCH_GSYN_H__6JtW3T5V7Kfw6FMquRj43
#define __SRVHRCH_GSYN_H__6JtW3T5V7Kfw6FMquRj43

#include <std/par/parserialize.h>

typedef enum
{
    KLSRVH_GSYN_UNKNOWN = -1,
    KLSRVH_GSYN_POLICY = 0,
    KLSRVH_GSYN_GTASK = 1,
    KLSRVH_GSYN_SRV_TASK = 2,
    KLSRVH_GSYN_POLICY_RM = 3
}KLSRVH_GSYN_TYPE;

KLCSSRV_DECL void KLSRVH_MakeSsTypeDigest(   
                            const std::wstring& wstrType,
                            unsigned char digest[KLPAR::c_Md5DigestSize],
                            KLSRVH_GSYN_TYPE&   nType);


KLCSSRV_DECL void KLSRVH_ReencryptParams(
                KLSTD::CAutoPtr<KLPAR::Params>& pData, 
                const void*                     pOldPrvKey,
                size_t                          nOldPrvKey,
                const void*                     pNewPubKey,
                size_t                          nNewPubKey,
                long&                           cntReencrypt,
                bool                            bIsPrvKeyGlobal = true);

KLCSSRV_DECL void KLSRVH_PutMasterGlobalKeys(
            const void*                             pPrvServerKey,
            size_t                                  nPubServerKey,
            const void*                             pKey1,
            size_t                                  nKey1,
            const void*                             pKey2,
            size_t                                  nKey2,
            KLSTD::CAutoPtr<KLSTD::MemoryChunk>&    pMasterPrivateKeyCh, 
            KLSTD::CAutoPtr<KLSTD::MemoryChunk>&    pMasterPublicKeyCh);

KLCSSRV_DECL void KLSRVH_GetMasterGlobalKeys(
            KLSTD::MemoryChunk**    ppMasterPrv, 
            KLSTD::MemoryChunk**    ppMasterPub);

KLCSSRV_DECL void KLSRVH_CreateSuperGsynSs(
            const void*         pMasterPrvKey,
            size_t              nMasterPrvKey,
            const void*         pServerPubKey,
            size_t              nServerPubKey,
            const wchar_t*      szwSsSrc,
            const wchar_t*      szwSsDst,
            long                lAccessTimeout);

#endif //__SRVHRCH_GSYN_H__6JtW3T5V7Kfw6FMquRj43
