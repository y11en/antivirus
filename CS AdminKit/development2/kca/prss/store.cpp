/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file 
 * \author Андрей Казачков
 * \date 2002
 * \brief 
 *
 */

#include "build/general.h"
#include "std/base/klbase.h"
#include "std/err/errloc_intervals.h"
#include "std/err/error_localized.h"
#include "std/conv/klconv.h"
#include "std/trc/trace.h"
#include "std/tmstg/timeoutstore.h"
#include "std/par/parstrdata.h"
#include "std/par/par_conv.h"
#include "kca/prss/settingsstorage.h"
#include "kca/prss/errors.h"
#include "kca/prss/store.h"
#include "std/par/paramsi.h"
#include "std/par/errors.h"
#include "std/par/paramslogger.h"
#include "std/par/parserialize.h"
#include "kca/prss/helpersi.h"
#include "kca/prss/errlocids.h"
#include <avp/text2bin.h>
#include <common/reporter.h>
#include <common/measurer.h>
#include <sstream>
#include <common/ak_cached_data2.h>
#include "openssl/md5.h"



#define KLCS_MODULENAME L"KLPRSS"

using namespace std;
using namespace KLSTD;
using namespace KLERR;
using namespace KLPAR;

KLCSC_DECL bool KLSTD_Unlink2(const wchar_t* szwFileName, bool bThrowExcpt);
KLCSC_DECL void KLSTD_Rename2(const wchar_t* szwOldFileName, const wchar_t* szwNewFileName);
KLCSC_DECL bool KLSTD_IfExists2(const wchar_t* szwFileName);

namespace KLPAR
{    
	//const bool c_bEncode=false;
	//void SerializeToFileID(int id, const Params* pParams, bool bEncode=c_bEncode);
	//void SerializeToMemory(const Params* pParams, KLSTD::MemoryChunk** ppChunk, bool bEncode=c_bEncode);
	//void DeserializeFromFileID(int id, Params** ppParams, bool bEncoded=c_bEncode);
	//void DeserializeFromMemory(const void* pData, size_t nData, Params** ppParams, bool bEncoded=c_bEncode);
	KLCSC_DECL void Write(ValuesFactory* pFactory, Params* pOldParams, Params* pNewParams, AVP_dword dwFlags);

    KLCSC_DECL void SerCreateBinaryValue(KLSTD::MemoryChunkPtr pChunk, BinaryValue** pValue);
};

#define KLPRSS_TRACE_CACHE_STATE(_p)    \
        KLSTD_TRACE2(4, L"SsCache: size=%u, count=%u\n", (_p)->GetFullSize(), (_p)->GetCount())

namespace KLPRSS
{
    const size_t c_nSsInMemoryLimit = 10*1024*1024;
    const long c_lCacheTimeout = 1000*60*1; //1 minute
    const long c_lFlagCacheTimeout = 1000*3600*2; //2 hours
    
    volatile bool    g_bUseCaching = false;
    volatile bool    g_bForceCaching = false; //for tests only !!!
    volatile size_t  g_nMaxCacheSize = 10*1024*1024;

    void WriteStoreFmtId(KLPRSS::Storage* pStorage, store_fmt_type_t nID);

    std::string& MakeCacheKey(const wchar_t* szwName, std::string& strResult)
    {
        KLSTD_CHK(szwName, szwName && szwName[0]);
        ;
        std::wstring wstrBuffer;        
        bool bPrevSlash = false;
        const size_t nSize = wcslen(szwName);
        wstrBuffer.reserve(nSize*3/2);
        for(size_t i= 0; i < nSize; ++i)
        {
            wchar_t ch = szwName[i];
    #ifdef _WIN32 //win32 only
            if( L'/' == ch )
                ch = L'\\';
    #endif
            if(L'\\' == ch)
            {
                if(bPrevSlash)
                    continue;
                else
                    bPrevSlash = true;
            }
            else
                bPrevSlash = false;
#ifdef _WIN32 //win32 only
            ch = towupper(ch);
#endif
            wstrBuffer += ch;
        };        

        MD5_CTX ctxMd5;
        MD5_Init(&ctxMd5);
        MD5_Update(&ctxMd5, wstrBuffer.c_str(),  wstrBuffer.size()*sizeof(wchar_t));
        unsigned char digest[MD5_DIGEST_LENGTH];
        KLSTD_ZEROSTRUCT(digest);
        MD5_Final(digest, &ctxMd5);

        const size_t c_nBuffer = B2T_LENGTH(KLPAR::c_Md5DigestSize)+1;
        char szBuffer[c_nBuffer];
        BinToText(digest, KLPAR::c_Md5DigestSize, szBuffer, c_nBuffer);
        szBuffer[ c_nBuffer - 1 ] = 0;

        strResult = szBuffer;

        KLSTD_ASSERT(!strResult.empty());

        KLSTD_TRACE2(4, L"Path: '%ls' -- key '%hs'\n", szwName, strResult.c_str());
        
        return strResult;
    };    

    class CachedSsObj;
    class SsCache;

	class CachedSsObj
        :   public KLSTD::KLBaseImpl<KLSTD::KLBase>
	{
	public:
        CachedSsObj(SsCache* pSsCache, KLSTD::MemoryChunkPtr pStorage);
        virtual ~CachedSsObj();
        KLSTD::MemoryChunkPtr GetStorage();
    protected:
        KLSTD::MemoryChunkPtr   m_pStorage;
        SsCache*                m_pSsCache;
    };

    typedef KLSTD::CAutoPtr<CachedSsObj>    CCachedSsObjectPtr;
    typedef KLSTD::KLAdapt<CCachedSsObjectPtr>  CachedSsObjectAdapted;

    class CallFile2Data
    {
    public:
        CallFile2Data()
        {;};
        CachedSsObjectAdapted operator() (
                        int, 
                        const std::string& strName);
    };
    
    typedef KLSTD::PairCacheTmt<
        KLSTD::PairCacheTmtBtree<
                        CCachedSsObjectPtr, 
                        int, 
                        std::string, 
                        CallFile2Data> >  CacheFile2Data;

    struct ss_flags_t
    {
        ss_flags_t()
            :   m_bChanged(false)
            ,   m_bNew(false)
        {
            ;
        };

        KLPAR::string_t m_wstrServerId, m_wstrNagentId;
        bool            m_bChanged, m_bNew;        
    };

    class SsFlags
        :   public KLSTD::KLBaseImpl<KLSTD::KLBase>
        ,   public ss_flags_t
    {
    public:
        ss_flags_t& get()
        {
            return *this;
        };
        virtual ~SsFlags()
        {;};
    };

    typedef KLSTD::CAutoPtr<SsFlags> SsFlagsPtr;

    typedef KLSTD::KLAdapt<SsFlagsPtr>  SsFlagsAdapt;

    class CallFile2Flag
    {
    public:
        CallFile2Flag()
        {;};
        SsFlagsAdapt operator() (
                        int, 
                        const std::string& strName);
    };

    typedef KLSTD::PairCacheTmt<
        KLSTD::PairCacheTmtBtree<
                        SsFlagsAdapt, 
                        int, 
                        std::string, 
                        CallFile2Flag> > CacheFile2Flag;


    class SsCache
        :   public KLSTD::KLBaseImpl<KLTP::ThreadsPool::Worker>
    {
    public:
        SsCache();        
        virtual ~SsCache();        
        void Create();        
        KLSTD_NOTHROW void Destroy() throw();

        virtual int RunWorker( KLTP::ThreadsPool::WorkerId wId )
        {
            if(m_oCacheFile2Data.RemoveOld(c_lCacheTimeout/1000))
            {
                KLPRSS_TRACE_CACHE_STATE(this);
            };
            m_oCacheFile2Flag.RemoveOld(c_lFlagCacheTimeout/1000);
            return 0;
        };

        void OnAdded(size_t nSize)
        {
            KLSTD::AutoCriticalSection acs(m_pDataCS);
            ++m_lStoresInCache;
            m_lFullSize += nSize;
        };

        void OnRemoved(size_t nSize)
        {
            KLSTD::AutoCriticalSection acs(m_pDataCS);
            --m_lStoresInCache;
            m_lFullSize -= nSize;
        };

        size_t GetFullSize()
        {
            return m_lFullSize;
        };

        size_t GetCount()
        {
            return m_lStoresInCache;
        };

        KLSTD::MemoryChunkPtr AcquireFromCache(const wchar_t* szwPath);
        
        bool AcquireFromCache(
                    const wchar_t* szwPath, 
                    KLPAR::ParamsPtr& pData_, 
                    KLSTD::MemoryChunkPtr& pChunk_);
        
        bool IfInCache(
                    const wchar_t* szwPath);
        
        void UpdateCache(
                    const wchar_t*          szwPath, 
                    KLSTD::MemoryChunkPtr   pStorage,
                    const ss_flags_t&       flags);

        bool GetStoreChanged(
                    const wchar_t*          szwPath, 
                    ss_flags_t&             flags);
    protected:
        KLSTD::CAutoPtr<KLSTD::CriticalSection> m_pDataCS;
        volatile long                           m_lStoresInCache;
        volatile long                           m_lFullSize;
        KLTP::ThreadsPool::WorkerId             m_idWorker;
    protected:
        CacheFile2Data                      m_oCacheFile2Data;
        CacheFile2Flag                      m_oCacheFile2Flag;
    };

    CachedSsObjectAdapted CallFile2Data::operator() (
                            int, 
                            const std::string& strName)
    {
        // not used
        KLSTD_THROW(KLSTD::STDE_NOTFOUND);
    };

    SsFlagsAdapt CallFile2Flag::operator() (
                    int, 
                    const std::string& strName)
    {
        // not used
        KLSTD_THROW(KLSTD::STDE_NOTFOUND);
    };

    SsCache::SsCache()
        :   m_lStoresInCache(0)
        ,   m_lFullSize(0)
        ,   m_idWorker(0)
    {
        KLSTD_CreateCriticalSection(&m_pDataCS);
        m_oCacheFile2Data.Create(CallFile2Data(), m_pDataCS);
        m_oCacheFile2Flag.Create(CallFile2Flag(), m_pDataCS);
    };
    
    SsCache::~SsCache()
    {
        ;
    };

    void SsCache::Create()
    {
        Destroy();
        KLTP_GetThreadsPool()->AddWorker(
                &m_idWorker,
                L"SsCache",
                this,
                KLTP::ThreadsPool::RunNormal,
                true,
                c_lCacheTimeout/2);
    };
    
    KLSTD_NOTHROW void SsCache::Destroy() throw()
    {
        if(m_idWorker)
        {
            KLTP_GetThreadsPool()->DeleteWorker(m_idWorker);
            m_idWorker = 0;
        };
    };

    KLSTD::MemoryChunkPtr SsCache::AcquireFromCache(const wchar_t* szwPath)
    {
        KLSTD::MemoryChunkPtr pResult;
        if(g_bUseCaching)
        {
        KLERR_BEGIN
            std::string strKey;
            pResult = m_oCacheFile2Data(0, MakeCacheKey(szwPath, strKey))->GetStorage();
            m_oCacheFile2Data.Prolong(strKey);
        KLERR_END        
            if(pResult)
            {
                KLSTD_TRACE1(3, L"Storage '%ls' acquired from ss cache\n", szwPath);
            };
        };
        return pResult;
    };
    
    bool SsCache::AcquireFromCache(
                const wchar_t* szwPath, 
                KLPAR::ParamsPtr& pData_, 
                KLSTD::MemoryChunkPtr& pChunk_)
    {
        bool bResult = false;
        if(g_bUseCaching)
        {
        KL_TMEASURE_BEGIN(L"KLPRSS::AcquireFromCache", 4)
        KLERR_BEGIN
            KLPAR::ParamsPtr pResult;
            KLSTD::MemoryChunkPtr pChunk = AcquireFromCache(szwPath);
            if(pChunk)
            {
                KLPAR_DeserializeFromMemory(
                        pChunk->GetDataPtr(),
                        pChunk->GetDataSize(),
                        &pResult);
                if(!pResult)
                    KLPAR_CreateParams(&pResult);
                pData_ = pResult;
                pChunk_ = pChunk;
                bResult = true;
                KLPRSS_TRACE_CACHE_STATE(this);
            };
        KLERR_ENDT(1)
        KL_TMEASURE_END()
        };
        return bResult;
    };
    
    bool SsCache::IfInCache(
                const wchar_t* szwPath)
    {
        bool bResult = false;
        if(g_bUseCaching)
        {
        KLERR_BEGIN
            std::string strKey;
            bResult = m_oCacheFile2Data.IfExists(MakeCacheKey(szwPath, strKey));
            if(bResult)
                m_oCacheFile2Data.Prolong(strKey);
        KLERR_END
        };
        return bResult;
    };
    
    void SsCache::UpdateCache(
                const wchar_t*          szwPath, 
                KLSTD::MemoryChunkPtr   pStorage,
                const ss_flags_t&       flags)
    {
    KLERR_BEGIN
        KLSTD_CHK(szwPath, szwPath && szwPath[0]);
        if(g_bUseCaching)
        {
        KL_TMEASURE_BEGIN(L"KLPRSS::UpdateCache", 4)            
            std::string strKey;
            MakeCacheKey(szwPath, strKey);
            m_oCacheFile2Data.ResetCache(strKey);
            m_oCacheFile2Flag.ResetCache(strKey);
            if(pStorage)
            {
                if(m_lFullSize + pStorage->GetDataSize() <= g_nMaxCacheSize)
                {
                    CCachedSsObjectPtr pSsObject;
                    pSsObject.Attach(new CachedSsObj(this, pStorage));
                    m_oCacheFile2Data.UpdateCache(strKey, pSsObject);
                    KLSTD_TRACE2(3, L"Storage '%ls' put into ss cache (bStoreChanged=%u)\n", szwPath, flags.m_bChanged);
                }
                else
                {
                    KLSTD_TRACE2(3, L"Storage '%ls' wasn't put into ss cache due to memory limitations (bStoreChanged=%u)\n", szwPath, flags.m_bChanged);
                };
                SsFlagsPtr pFlags;
                pFlags.Attach(new SsFlags);
                KLSTD_CHKMEM(pFlags);
                pFlags->get() = flags;
                m_oCacheFile2Flag.UpdateCache(strKey, pFlags);
            };
            KLPRSS_TRACE_CACHE_STATE(this);
        KL_TMEASURE_END()
        };
    KLERR_END
    };

    bool SsCache::GetStoreChanged(
                const wchar_t*    szwPath, 
                ss_flags_t&       flags)
    {
        bool bResult = false;
        if(g_bUseCaching)
        {
        KLERR_BEGIN
            std::string strKey;
            flags = m_oCacheFile2Flag(0, MakeCacheKey(szwPath, strKey)).m_T->get();
            bResult = true;
            m_oCacheFile2Flag.Prolong(strKey);
        KLERR_END        
            if(bResult)
            {
                KLSTD_TRACE2(3, L"Storage's '%ls' changed flag (%u) acquired from ss cache\n", szwPath, flags.m_bChanged);
            };
        };
        return bResult;
    };

    KLSTD::CAutoPtr<SsCache>    g_pSsCache;

    CachedSsObj::CachedSsObj(SsCache* pSsCache, KLSTD::MemoryChunkPtr pStorage)
        :   m_pStorage(pStorage)
        ,   m_pSsCache(pSsCache)
    {
        KLSTD_CHKINPTR(pStorage);
        KLSTD_CHKINPTR(pSsCache);
        m_pSsCache->OnAdded(pStorage->GetDataSize());
    };

    CachedSsObj::~CachedSsObj()
    {
        m_pSsCache->OnRemoved(m_pStorage->GetDataSize());
    };

    KLSTD::MemoryChunkPtr CachedSsObj::GetStorage()
    {
        return m_pStorage;
    };

    void InitCache()
    {
#ifndef KLCSC_STATIC
        g_pSsCache = NULL;
        g_pSsCache.Attach(new SsCache);
        g_pSsCache->Create();
#endif
    };
    
    void DeinitCache()
    {
#ifndef KLCSC_STATIC
        if(g_pSsCache)
        {
            g_pSsCache->Destroy();
            g_pSsCache = NULL;
        };
#endif
    };


    bool AcquireFromCache(
                const wchar_t* szwPath, 
                KLPAR::ParamsPtr& pData_, 
                KLSTD::MemoryChunkPtr& pChunk_)
    {
#ifndef KLCSC_STATIC
        return g_pSsCache->AcquireFromCache(szwPath, pData_, pChunk_);
#else
        return false;
#endif
    };
    
    bool IfInCache(
                const wchar_t* szwPath)
    {
#ifndef KLCSC_STATIC
        return g_pSsCache->IfInCache(szwPath);
#else
        return false;
#endif        
    };
    
    void UpdateCache(
                const wchar_t*          szwPath, 
                KLSTD::MemoryChunkPtr   pStorage,
                const ss_flags_t&       flags)
    {
#ifndef KLCSC_STATIC
        g_pSsCache->UpdateCache(szwPath, pStorage, flags);
#else
        ;
#endif
    };

    bool GetStoreChangedCached(
            const wchar_t*          szwPath, 
            ss_flags_t&             flags)
    {
    #ifndef KLCSC_STATIC
            return g_pSsCache->GetStoreChanged(szwPath, flags);
    #else
            return false;
    #endif
    };

    KLPAR::ParamsValuePtr UnpackSection(KLPAR::BinaryValuePtr pVal)
    {
        KLSTD_CHKINPTR(pVal);
        KLPAR::ParamsPtr pDeserializedParams;
        KLPAR_DeserializeFromMemory(
                pVal->GetValue(),
                pVal->GetSize(),
                &pDeserializedParams);
        KLSTD_TRACE0(4, L"Unpacking section. Overhead !!!\n");
        KLSTD::CAutoPtr<KLPAR::ParamsValue> pResVal;
        KLPAR::CreateValue(pDeserializedParams, &pResVal);
        return pResVal;
    };

#ifdef _DEBUG
    std::wstring& CalcParamsHash(KLPAR::ParamsPtr pData, std::wstring& wstrHash)
    {
        ;
        unsigned char digest[KLPAR::c_Md5DigestSize];
        KLPAR::CalcParamsMd5Hash(pData, digest);
        const size_t nBuffer = B2T_LENGTH(KLPAR::c_Md5DigestSize)+1;
        char szBuffer[nBuffer];
        BinToText(digest, KLPAR::c_Md5DigestSize, szBuffer, nBuffer);
        szBuffer[nBuffer-1]=0;

        wstrHash = (const wchar_t*)KLSTD_A2CW2(szBuffer);
        return wstrHash;
    };

    std::wstring& CalcDataHash(KLPAR::ValuePtr pVal, std::wstring& wstrHash)
    {
        KLPAR::ParamsValuePtr pParVal;
        if(pVal && KLPAR::Value::BINARY_T == pVal->GetType())
        {
            KLPAR::BinaryValuePtr pBinVal = 
                        (KLPAR::BinaryValue*)(KLPAR::Value*)pVal;
            pParVal = UnpackSection(pBinVal);
            //KLSTD_TRACE0(4, L"CalcDataHash: Unpacked params\n");
            //KLPARLOG_LogParams(4, pParVal?pParVal->GetValue():NULL);
        }
        else if(!pVal  || KLPAR::Value::PARAMS_T == pVal->GetType())
        {
            pParVal = 
                        (KLPAR::ParamsValue*)(KLPAR::Value*)pVal;
        };
        return CalcParamsHash(pParVal?pParVal->GetValue():NULL, wstrHash);
    };
#endif
};

namespace
{
    void Fill_ss_flags(KLPRSS::Storage* pStorage, KLPRSS::ss_flags_t& flags)
    {
        KLERR_TRY
            flags.m_bNew = KLPRSS::GetStoreNew(pStorage);
            flags.m_bChanged = KLPRSS::GetStoreChanged(pStorage);        
            std::wstring wstrIdServer, wstrIdNagent;
            KLPRSS::GetStoreServerId(pStorage, wstrIdServer);
            KLPRSS::GetStoreClientId(pStorage, wstrIdNagent);
            flags.m_wstrNagentId = wstrIdNagent.c_str();
            flags.m_wstrServerId = wstrIdServer.c_str();
        KLERR_CATCH(pError)
            KLERR_SAY_FAILURE(4, pError);
            KLSTD_ASSERT(!"Unexpected error");
            KLERR_RETHROW();
        KLERR_ENDTRY
    };
}

void KLCSKCA_DECL KLPRSS_GetStoreFlags(
        const wchar_t*          szwPath, 
        bool&                   bNew,
        bool&                   bChanged,
        std::wstring&           wstrServerId,
        std::wstring&           wstrClientId,
        long                    lTimeout)
{
    KLSTD_CHK(szwPath, szwPath && szwPath[0]);
    KLPRSS::ss_flags_t flags;
    if(!KLPRSS::GetStoreChangedCached(szwPath, flags))
    {
        KLSTD_TRACE1(1, L"KLPRSS_GetStoreFlags: Loading from disk ('%ls')\n", szwPath);
        CAutoPtr<KLPRSS::Storage> pStorage;
        KLPRSS::CreateStorage(szwPath, CF_OPEN_EXISTING | KLPRSS_CF_USE_CACHE, AF_READ, &pStorage);
        pStorage->trans_begin(false, lTimeout, KLPRSS::STGLT_NOLOCK);
        Fill_ss_flags(pStorage, flags);        
        pStorage->trans_end();
    };
    bNew = flags.m_bNew;
    bChanged = flags.m_bChanged;
    wstrServerId = flags.m_wstrServerId.get();
    wstrClientId = flags.m_wstrNagentId.get();
};


void KLCSKCA_DECL KLPRSS_GetStoreChangedCached(
        const wchar_t*          szwPath, 
        bool&                   bStoreChanged,
        long                    lTimeout)
{
    bool                    bNew;
    bool                    bChanged;
    std::wstring            wstrServerId;
    std::wstring            wstrClientId;

    KLPRSS_GetStoreFlags(szwPath, bNew, bChanged, wstrServerId, wstrClientId, lTimeout);
    bStoreChanged = bChanged;
};

namespace KLPRSS
{    
    std::wstring backup_name(const wstring& dir, const wstring& name, const wstring& ext)
    {
        std::wstring res;
        KLSTD_MakePath(
                    dir, 
                    wstring(c_szwFilePref_Backup)+name, 
                    ((ext.size() > 0)
                        ? wstring(L".") + c_szwFilePref_Backup + &ext[1] + c_szwFilePref_Backup2
                        : wstring(L".") + c_szwFilePref_Backup2),
                    res);
        return res;
    };

	void MakeBackupAndSemaphoreNames(
					const wstring&	wstrFileName,
					wstring&		wstrBackupName,
					wstring&		wstrSemaphoreName,
                    wstring&		wstrBackupName2)
	{
		wstring dir, name, ext;
		KLSTD_SplitPath(wstrFileName, dir, name, ext);
		KLSTD_MakePath(dir, wstring(c_szwFilePref_Backup)+name, ext+c_szwFilePref_Backup, wstrBackupName);
        wstrBackupName2 = backup_name(dir, name, ext);
		KLSTD_MakePath(dir, name, c_szwFileExt_Sem, wstrSemaphoreName);
	};

	std::wstring GetReadName(
							const std::wstring wstrFileName,
							const std::wstring wstrBackupName)
	{
		if(KLSTD_IfExists2(wstrBackupName.c_str()))
			return wstrBackupName;
		else
			return wstrFileName;
	};

	std::wstring GetWriteName(
							const std::wstring wstrFileName,
							const std::wstring wstrBackupName)
	{
		if(KLSTD_IfExists2(wstrBackupName.c_str()))
			KLSTD_Unlink2(wstrFileName.c_str(), true);
		else
		if(KLSTD_IfExists2(wstrFileName.c_str()))
			KLSTD_Rename2(wstrFileName.c_str(), wstrBackupName.c_str());
		return wstrFileName;
	};

#ifdef _DEBUG
    bool DebugCheckHash(KLPAR::ParamsPtr pSec)
    {
        bool bResult = true;
        KLSTD::CAutoPtr<KLPAR::Value> pVal;
        if(pSec)
            pSec->GetValueNoThrow(c_wstrData, &pVal);
        if(pSec && pVal && pSec->DoesExist(c_wstrHash))
        {                
            const wchar_t* szwOldHash = KLPAR::ParVal(KLPAR::GetVal(pSec, c_wstrHash), L"");
            std::wstring wstrNewHash;
            CalcDataHash(pVal, wstrNewHash);
            bResult = (klstd_wcscmp(wstrNewHash.c_str(), szwOldHash) == 0);
            if(!bResult)
            {
                KLSTD_TRACE2(
                            4, 
                            L"Checksum failure: szwOldHash='%ls', wstrNewHash='%ls'\n", 
                            szwOldHash, 
                            wstrNewHash.c_str());
                KLPARLOG_LogParams(4, pSec);
                KLSTD_ASSERT(!"DebugCheckHash failed !!!");
            };            
        };
        return bResult;
    };

    bool DebugCheckHash(KLPAR::ParamsPtr pSec, KLPAR::ValuePtr pVal)
    {
        bool bResult = true;
        if(pSec && pVal && pSec->DoesExist(c_wstrHash))
        {                
            const wchar_t* szwOldHash = KLPAR::ParVal(KLPAR::GetVal(pSec, c_wstrHash), L"");
            std::wstring wstrNewHash;
            CalcDataHash(pVal, wstrNewHash);
            bResult = (klstd_wcscmp(wstrNewHash.c_str(), szwOldHash) == 0);
        };
        return bResult;
    };
#endif
    
    void ConvertSectionIntoSemiPacked(KLPAR::ParamsPtr pSec, bool bSemiPacked, bool& bChanged)
    {
        KLSTD::CAutoPtr<KLPAR::Value> pVal;
        pSec->GetValueNoThrow(c_wstrData, &pVal);
        if(pVal && KLPAR::Value::PARAMS_T == pVal->GetType() && bSemiPacked)
        {
            //serialize params
            KLPAR::ParamsPtr pParams2Serialize;
            pParams2Serialize = ((KLPAR::ParamsValue*)(KLPAR::Value*)pVal)->GetValue();
            KLSTD::MemoryChunkPtr pChunk;
            KLPAR_SerializeToMemory2(KLPAR_SF_BIN_COMPRESS, pParams2Serialize, &pChunk);
            KLPAR::BinaryValuePtr pBinary;
            //KLPAR::CreateValue(KLPAR::binary_wrapper_t(pChunk), &pBinary);
            SerCreateBinaryValue(pChunk, &pBinary);
            pSec->ReplaceValue(c_wstrData, pBinary);
            bChanged = true;                
        }
        else if(pVal && KLPAR::Value::BINARY_T == pVal->GetType() && !bSemiPacked)
        {
            //deserialize params
            KLSTD::CAutoPtr<KLPAR::ParamsValue> pResVal = UnpackSection(
                        (KLPAR::BinaryValue*)(KLPAR::Value*)pVal);
            pSec->ReplaceValue(c_wstrData, pResVal);            
            bChanged = true;
        };            
        KLSTD_ASSERT(DebugCheckHash(pSec));
	};

    //! returns true if smth changed
    bool ConvertIntoSemiPacked(KLPAR::ParamsPtr pData, bool bSemiPacked)
    {
        bool bChanged = false;
        //Enumerate all sections and convert Params->Binary or Binary->Params
        KLSTD::klwstrarr_t arrProducts;
        pData->GetNames(arrProducts.outref());
        for(size_t iProd = 0; iProd < arrProducts.size(); ++iProd)
        {
            KLPAR::ParamsPtr pProd = 
                KLPAR::GetParamsValue(pData, arrProducts[iProd]);
            if(!pProd)
                continue;

            KLSTD::klwstrarr_t arrVersions;
            pProd->GetNames(arrVersions.outref());
            for(size_t iVer = 0; iVer < arrVersions.size(); ++iVer)
            {
                KLPAR::ParamsPtr pVer = 
                    KLPAR::GetParamsValue(pProd, arrVersions[iVer]);
                if(!pVer)
                    continue;

                KLSTD::klwstrarr_t arrSections;
                pVer->GetNames(arrSections.outref());
                for(size_t iSec = 0; iSec < arrSections.size(); ++iSec)
                {
                    KLPAR::ParamsPtr pSec = 
                        KLPAR::GetParamsValue(pVer, arrSections[iSec]);
                    if(!pSec)
                        continue;
                    if( 0 == klstd_wcscmp(arrProducts[iProd], KLPRSS_PRODUCT_CORE) &&
                        0 == klstd_wcscmp(arrVersions[iVer], KLPRSS_VERSION_INDEPENDENT) &&
                        0 == klstd_wcscmp(arrSections[iSec], KLPRSS_VERSION_INFO) )
                    {
                        continue;
                    };
                    ConvertSectionIntoSemiPacked(pSec, bSemiPacked, bChanged);                        
                };
            };
        };
        return bChanged;
    };
	
	class CStorage: public KLBaseImpl<Storage>
	{
	public:
		CStorage(
                const std::wstring&     wstrFileName,
                AVP_dword               dwCreation,
                AVP_dword               dwAccess,
                const ss_format_t*      pFormat)
            :   m_wstrFileName(wstrFileName)
            ,   m_dwCreation(dwCreation)
            ,   m_dwAccess(dwAccess)
            ,   m_bDataWasChanged(false)
            ,   m_bPseudoTrans(false)
            ,   m_bCorrupted(false)
            ,   m_pFormat(DupFormat(pFormat))
            ,   m_dwDeserializeFlags(0)
		{
            if(g_bForceCaching)
                const_cast<AVP_dword&>(m_dwCreation) |= KLPRSS_CF_USE_CACHE;
			MakeBackupAndSemaphoreNames(m_wstrFileName, m_wstrBackupFileName, m_wstrSemaphoreName, m_wstrBackupFileName2);
			::KLSTD_CreateFileSemaphoreNoReadLock(m_wstrSemaphoreName, &m_pSemaphore);
		};
		virtual ~CStorage()
		{
			KLERR_BEGIN
				trans_end(false);
			KLERR_ENDT(2)
                ;
            if(m_pFormat)
                free(m_pFormat);
		};
        
        void remove(long lTimeout)
        {
            KLERR_IGNORE(trans_end(false));
            KLERR_TRY
                m_pSemaphore->EnterForDelete(lTimeout);
                KLSTD_Unlink2(m_wstrFileName.c_str(), true);
                KLSTD_Unlink2(m_wstrBackupFileName.c_str(), true);
                KLSTD_Unlink2(m_wstrBackupFileName2.c_str(), true);
            KLERR_CATCH(pError)
                KLERR_SAY_FAILURE(3, pError);
            KLERR_FINALLY
                m_pSemaphore->Leave();                 
                UpdateCache(m_wstrFileName.c_str(), NULL, KLPRSS::ss_flags_t());
                //KLERR_IGNORE(KLSTD_Unlink2(m_wstrSemaphoreName.c_str(), false));
                KLERR_RETHROW();
            KLERR_ENDTRY
        };

        void FixCorrupted()
        {//
            if(KLSTD_IfExists2(m_wstrBackupFileName2.c_str()))
            {
                //we must move m_wstrBackupFileName2 into m_wstrBackupFileName
                if(KLSTD_IfExists2(m_wstrBackupFileName.c_str()))
                {
                    if( KLSTD_IfExists2(m_wstrFileName.c_str()))
                    {
                        KLSTD_Unlink2(m_wstrFileName.c_str(), true);
                        // now m_wstrBackupFileName still exist so Ss is consistent
                    };
                    KLSTD_Rename2(m_wstrBackupFileName.c_str(), m_wstrFileName.c_str());
                    // now m_wstrBackupFileName not exist but m_wstrFileName exists so Ss is consistent 
                };
                KLSTD_Rename2(m_wstrBackupFileName2.c_str(), m_wstrBackupFileName.c_str());

                std::wostringstream os;
                os << L"Storage '" << m_wstrFileName << L"' was restored\n";
                KLSTD_ReportInfo(os.str().c_str());
            };
        };

		void trans_begin(bool bForChange, long nTimeout, AVP_byte nLockType)
		{
        KL_TMEASURE_BEGIN(L"CStorage::trans_begin", 4)
			if(bForChange && !(m_dwAccess & AF_WRITE))
				KLSTD_THROW(STDE_NOTPERM);
			trans_end(false);
            m_bPseudoTrans = false;
			if( !bForChange && STGLT_EXCLUSIVE == nLockType)
            {
                //KLSTD_ASSERT(!"Cannot exclusively lock read-only storage");
                //No, it is locked while copying ss
                bForChange = true;
            };
            ;
            std::wstring wstrName;
			KLERR_TRY
				//KLERR_TRY
                if( !bForChange && (AcquireFromCache(m_wstrFileName.c_str(), m_pRootData, m_pReadData)) )
                {
                    m_bPseudoTrans = true;
                    KLSTD_TRACE1(4, L"Storage '%ls' acquired from cache\n", m_wstrFileName.c_str());
                }
                else
                {
                    m_pSemaphore->Enter(!bForChange, nTimeout);
                    if( !(AcquireFromCache(m_wstrFileName.c_str(), m_pRootData, m_pReadData)) )
                    {
                        for(size_t nAttempt = 0; !m_pRootData && nAttempt < 2; ++nAttempt)
                        {
                        KLERR_TRY
				            CAutoPtr<File> pFile;
                            if(!nAttempt)
                            {
				                KLSTD_CreateFile(
					                wstrName=GetReadName(m_wstrFileName, m_wstrBackupFileName),
					                SF_READ,
					                m_dwCreation & (~KLPRSS_CF_USE_CACHE),//clear flag KLPRSS_CF_USE_CACHE
					                AF_READ,
					                EF_SEQUENTIAL_SCAN,
					                &pFile,
					                nTimeout);
                                // Now pFile cannot be NULL
                            }
                            else if(KLSTD_IfExists2(m_wstrBackupFileName2.c_str()))
                            {
				                KLSTD_CreateFile(
					                wstrName = m_wstrBackupFileName2,
					                SF_READ,
					                m_dwCreation & (~KLPRSS_CF_USE_CACHE),//clear flag KLPRSS_CF_USE_CACHE
					                AF_READ,
					                EF_SEQUENTIAL_SCAN,
					                &pFile,
					                nTimeout);

                                /*
                                if(bForChange)
                                {
                                    KLSTD_Unlink2(m_wstrFileName.c_str(), true);
                                    KLSTD_Unlink2(m_wstrBackupFileName.c_str(), true);
                                };*/
                            }
                            else
                            {
                                KLSTD_TRACE0(1, L"No backup copy to restore from\n");
                            };
                            // Now pFile can be NULL if and only if storage is corrupt and backup does not exists
                            if(pFile)
                            {
                                m_pReadData = NULL;
                                m_pRootData = NULL;
                                const AVP_qword qwSize = pFile->GetSize();
                                if( qwSize && qwSize <= (AVP_qword)c_nSsInMemoryLimit )
                                {//read file contents into memory (possibly better to map it)
                                    KLSTD_AllocMemoryChunk(size_t(qwSize), &m_pReadData);
                                    KLSTD_ASSERT_THROW(
                                        size_t(qwSize) == pFile->Read(
                                                    m_pReadData->GetDataPtr(), 
                                                    m_pReadData->GetDataSize()));
                                    KLPAR_DeserializeFromMemory2(
                                                    m_pReadData->GetDataPtr(),
                                                    m_pReadData->GetDataSize(),
                                                    m_dwDeserializeFlags,
                                                    &m_pRootData);
                                }else if(pFile->GetSize())
                                {
                                    KLPAR_DeserializeFromFileID2(
                                                            pFile->GetFileID(),
                                                            m_dwDeserializeFlags,
                                                            &m_pRootData);
                                }
				                else
					                KLPAR_CreateParams(&m_pRootData);
                            };
                        KLERR_CATCH(pError)
                            if(pError->GetId() != KLSTD::STDE_BADFORMAT)
                            {
                                KLERR_RETHROW();
                            };
                            KLSTD_TRACE1(1, L"Storage '%ls' seems to be corrupted -- restoring...\n", m_wstrFileName.c_str());
                            std::wostringstream os;
                            os << L"Storage '" << m_wstrFileName << L"' seems to be corrupted and will be restored\n";
                            KLSTD_ReportWarning(os.str().c_str());
                            m_bCorrupted = true;
                        KLERR_ENDTRY
                        };
                        if(!m_pRootData)
                        {
                            KLSTD_THROW(KLSTD::STDE_BADFORMAT);
                        };
                        if(bForChange && m_bCorrupted)
                        {
                            FixCorrupted();
                            m_bCorrupted = false;
                        };
                    }
                    else
                    {
                        KLSTD_TRACE1(4, L"Storage '%ls' acquired from cache\n", m_wstrFileName.c_str());
                    };
                    if(!bForChange && STGLT_NOLOCK == nLockType)
                    {                        
                        m_pSemaphore->Leave();
                        m_bPseudoTrans = true;
                    };
                };
			KLERR_CATCH(pError)
                m_pRootData = NULL;
                m_pReadData = NULL;
				trans_end(false);
                if(pError->GetId() == KLSTD::STDE_BADFORMAT)
                {
                    KLERR_SetErrorLocalization(
                                pError,
                                KLPRSS::PRSSEL_STORAGE_CORRUPT,
                                KLERR_LOCMOD_PRSS,
                                wstrName.c_str());
                    if( KLERR_IsErrorLocalized(pError) )
                    {
                        KLSTD::klwstr_t wstrDesc;
                        if(KLERR_LocFormatErrorString(pError, wstrDesc.outref()))
                            KLSTD_ReportError(wstrDesc.c_str());
                    };                    
                };
				KLERR_RETHROW();
			KLERR_ENDTRY
        KL_TMEASURE_END()
		};

		bool region_load(
				const wchar_t**	pszAbsPath,
				bool			bForChange,
				bool bExceptIfNotExist)
		{
			if(bForChange && !(m_dwAccess & AF_WRITE))
				KLSTD_THROW(STDE_NOTPERM);
			region_unload();
			KLERR_TRY
				// Search
				CAutoPtr<Params> pRoot=m_pRootData;
				CAutoPtr<Value> pValue;
				for(const wchar_t** ppName=pszAbsPath; *ppName; ++ppName)
				{
                    KLSTD_ASSERT_THROW(pRoot != NULL);
					pValue=NULL;
					if(!pRoot->GetValueNoThrow(*ppName, &pValue))
					{
						if(bExceptIfNotExist)
							KLERR_MYTHROW1(KLPRSS::NOT_EXIST, *ppName);
						else
							return false;
					};
					if(pValue->GetType()!=Value::PARAMS_T)
						KLERR_MYTHROW1(KLPRSS::ERR_UNEXPECTED_TYPE, *ppName);
					pRoot=static_cast<ParamsValue*>((Value*)pValue)->GetValue();
				};
				m_pCurrentValue=(ParamsValue*)(Value*)pValue;
				m_pCurrent=pRoot;
                KLSTD_ASSERT(DebugCheckHash(m_pCurrent));
			KLERR_CATCH(pError)
				region_unload();
				KLERR_RETHROW();
			KLERR_ENDTRY
			return true;
		};

		void region_unload()
		{
			m_pCurrentValue=NULL;			
			m_pCurrent=NULL;
		};

		void trans_end(bool bCommit)
		{
        KL_TMEASURE_BEGIN(L"CStorage::trans_end", 4)            
			KLERR_TRY                
				if(m_bDataWasChanged && bCommit && m_pRootData && (m_dwAccess & AF_WRITE) && !m_bPseudoTrans)
				{
                    {
                        bool bSemipacked = false;
                        if( (m_pFormat && SSF_ID_SEMIPACKED == m_pFormat->id))
                            bSemipacked = true;
                        else if((m_pFormat && SSF_ID_DEFAULT != m_pFormat->id))
                            bSemipacked = false;
                        else
                        {
                            //fill  m_bLoadedSemiPacked
                            const wchar_t* path[] = 
                            {
                                KLPRSS_PRODUCT_CORE,
                                KLPRSS_VERSION_INDEPENDENT,
                                KLPRSS_VERSION_INFO,
                                c_wstrData,
                                NULL
                            };
                            KLPAR::ValuePtr pVal;
                            KLERR_IGNORE(KLPAR_GetValue(m_pRootData, path, c_szwSsFmtType, &pVal));                            
                            bSemipacked =( KLPAR::ParVal(pVal, (long)SFT_DEFAULT) == SFT_SEMIPACKED);
                        };                        
                        WriteStoreFmtId(this, bSemipacked?SFT_SEMIPACKED:SFT_DEFAULT);
                        ConvertIntoSemiPacked(m_pRootData, bSemipacked);
                    };
                    
                    ;                    
                    KLPRSS::ss_flags_t flags;
                    if(KLPRSS::g_bUseCaching)
                        Fill_ss_flags(this, flags);
					m_bDataWasChanged=false;

                    if(m_bCorrupted)
                    {
                        FixCorrupted();
                    };

					CAutoPtr<File> pFile;
					KLSTD_CreateFile(
						GetWriteName(m_wstrFileName, m_wstrBackupFileName),
						0,
						CF_CREATE_ALWAYS,
						AF_WRITE,
						EF_WRITE_THROUGH,
						&pFile,
						0);
                    m_pReadData = NULL;
                    ;
                    KLPAR_SerializeToMemory2(
                                KLPAR_SF_BINARY,
                                m_pRootData,
                                &m_pReadData);
                    KLSTD_ASSERT_THROW(
                            size_t(m_pReadData->GetDataSize()) == 
                                pFile->Write(
                                    m_pReadData->GetDataPtr(), 
                                    m_pReadData->GetDataSize()));
                    pFile->Flush(true);
                    pFile = NULL;

                    KLSTD_Unlink2(m_wstrBackupFileName2.c_str(), true);
                    if(KLSTD_IfExists2(m_wstrBackupFileName.c_str()))
                        KLSTD_Rename2(m_wstrBackupFileName.c_str(), m_wstrBackupFileName2.c_str());

                    if( (m_dwCreation & KLPRSS_CF_USE_CACHE) || 
                        IfInCache(m_wstrFileName.c_str()) )
                    {
                        UpdateCache(m_wstrFileName.c_str(), m_pReadData, flags);
                    }
                    else
                    {
                        UpdateCache(m_wstrFileName.c_str(), NULL, flags);
                    };
				}
                else if(    !m_bDataWasChanged && 
                            KLPRSS::g_bUseCaching && 
                            (m_dwCreation & KLPRSS_CF_USE_CACHE) && 
                            m_pRootData)
                {
                    KLPRSS::ss_flags_t flags;
                    if(KLPRSS::g_bUseCaching)
                        Fill_ss_flags(this, flags);

                    if(!m_pReadData)
                        KLPAR_SerializeToMemory2(
                                    KLPAR_SF_BINARY,
                                    m_pRootData,
                                    &m_pReadData);
                    if(!IfInCache(m_wstrFileName.c_str()))
                        UpdateCache(m_wstrFileName.c_str(), m_pReadData, flags);
                };
			KLERR_CATCH(pError)
				KLERR_SAY_FAILURE(1, pError);
                UpdateCache(m_wstrFileName.c_str(), NULL, KLPRSS::ss_flags_t());
			KLERR_FINALLY
				m_pRootData=NULL;
                m_pReadData = NULL;
                m_bCorrupted = false;
				KLERR_BEGIN
					region_unload();
				KLERR_ENDT(2)
                if(!m_bPseudoTrans)
                {
					KLERR_IGNORE(m_pSemaphore->Leave());
                    m_bPseudoTrans = false;
                };
				KLERR_RETHROW();
			KLERR_ENDTRY
        KL_TMEASURE_END()
		};


	bool entry_create(
				const wchar_t*	szEntry,
				bool bExceptIfExist)
	{
		if(!m_pCurrent)
			KLSTD_THROW(STDE_NOTPERM);		
		CAutoPtr<ValuesFactory> pFactory;
		KLPAR_CreateValuesFactory(&pFactory);
		CAutoPtr<ParamsValue> pParamsVaue;
		pFactory->CreateParamsValue(&pParamsVaue);
		if(!m_pCurrent->DoesExist(szEntry))
		{
			m_pCurrent->AddValue(szEntry, pParamsVaue);
			m_bDataWasChanged=true;
		}
		else{
			if(bExceptIfExist)
				KLERR_MYTHROW1(KLPRSS::ALREADY_EXIST, szEntry);
			return false;
		}
		return true;
	};

	bool entry_destroy(
				const wchar_t*	szEntry,
				bool bExceptIfNotExist)
	{
		if(!m_pCurrent)
			KLSTD_THROW(STDE_NOTPERM);
		if(m_pCurrent->DoesExist(szEntry))
		{
			m_pCurrent->DeleteValue(szEntry, true);
			m_bDataWasChanged=true;
		}
		else{
			if(bExceptIfNotExist)
				KLERR_MYTHROW1(KLPRSS::NOT_EXIST, szEntry);
			return false;
		}
		return true;
	};

	void entry_read(KLPAR::Params** ppParams)
	{
		KLSTD_CHKOUTPTR(ppParams);
		if(!m_pCurrent)
			KLSTD_THROW(STDE_NOTPERM);

		*ppParams=NULL;
		CAutoPtr<ParamsValue> pParamsValue;
		if(!attr_get(c_wstrData, (Value**)&pParamsValue))
		{
			CAutoPtr<ValuesFactory> pFactory;
			KLPAR_CreateValuesFactory(&pFactory);
			pFactory->CreateParamsValue(&pParamsValue);
			attr_put(c_wstrData, pParamsValue);			
		};
		CAutoPtr<Params> pParams=pParamsValue->GetValue();
        //pParams->Duplicate(ppParams);
        pParams->Clone(ppParams);
        //FIX Duplicate
		//pParams.CopyTo(ppParams);
	};

	void entry_write(AVP_dword dwFlags, KLPAR::Params* pParamsToWrite)
	{
        KLSTD_USES_CONVERSION;
		KLSTD_CHKINPTR(pParamsToWrite);
		if(!m_pCurrent || !m_pCurrentValue)
			KLSTD_THROW(STDE_NOTPERM);
		;
		CAutoPtr<Params> pOldParams;
		{
			CAutoPtr<ParamsValue> pOldParamsValue;
			if(attr_get(c_wstrData, (Value**)&pOldParamsValue))
			{
				KLPAR_CHKTYPE(pOldParamsValue, PARAMS_T, c_wstrData);
				pOldParams=pOldParamsValue->GetValue();
			};
		};
		if(!pOldParams)
			KLPAR_CreateParams(&pOldParams);
        
        unsigned char   digest_old[KLPAR::c_Md5DigestSize],
                        digest_new[KLPAR::c_Md5DigestSize];
        ;
        KLPAR::CalcParamsMd5Hash(pOldParams, digest_old);//old hash
		CAutoPtr<ValuesFactory> pFactory;
		KLPAR_CreateValuesFactory(&pFactory);
		Write(pFactory, pOldParams, pParamsToWrite, dwFlags);
        KLPAR::CalcParamsMd5Hash(pOldParams, digest_new);//new hash
        ;
        if(memcmp(digest_new, digest_old, KLPAR::c_Md5DigestSize) != 0)
        {
		    CAutoPtr<ParamsValue> pParamsValue;
		    CreateValue(pOldParams, &pParamsValue);
		    attr_put(c_wstrData, pParamsValue);
		    ;
		    CAutoPtr<BoolValue> pChanged;
		    CreateValue(true, &pChanged);
		    attr_put(c_wstrChanged, pChanged);
		    ;
            const size_t nBuffer=B2T_LENGTH(KLPAR::c_Md5DigestSize)+1;
            char szBuffer[nBuffer];
            BinToText(digest_new, KLPAR::c_Md5DigestSize, szBuffer, nBuffer);
            szBuffer[nBuffer-1]=0;
            ;
		    CAutoPtr<StringValue> pHash;
		    CreateValue(KLSTD_A2CW(szBuffer), &pHash);
		    attr_put(c_wstrHash, pHash);
		    m_bDataWasChanged=true;

            //KLSTD_TRACE1(4, L"entry_write: hash='%hs'\n", szBuffer);
            //KLPARLOG_LogParams(4, pParamsValue?pParamsValue->GetValue():NULL);

        }
        else
        {
            KLSTD_TRACE0(5, L"No difference found while changing section\n");
        }
	};

	void entry_readall(KLPAR::Params** ppParams)
	{
        entry_readall2(ppParams, false);
	};

    void entry_readall2(KLPAR::Params** ppParams, bool bSupportsSemiPacked)
    {        
		KLSTD_CHKOUTPTR(ppParams);
		if(!m_pCurrent)
			KLSTD_THROW(STDE_NOTPERM);
        KLPAR::ParamsPtr pRes;
        m_pCurrent->Duplicate(&pRes);
        if(!bSupportsSemiPacked)
        {
            bool bChanged = false;
            ConvertSectionIntoSemiPacked(pRes, false, bChanged);
        };
        *ppParams = pRes.Detach();
    };

	void entry_writeall(KLPAR::Params* pParamsToWrite)
	{
		KLSTD_CHKINPTR(pParamsToWrite);
		if(!m_pCurrent || !m_pCurrentValue)
			KLSTD_THROW(STDE_NOTPERM);
		;
		CAutoPtr<ValuesFactory> pFactory;
		KLPAR_CreateValuesFactory(&pFactory);
		if(pParamsToWrite!=m_pCurrent)
			Write(pFactory, m_pCurrent, pParamsToWrite, CF_CREATE_ALWAYS);
		m_bDataWasChanged=true;
	};
	
	void entry_enumerate(std::vector<std::wstring>& names)
	{
		if(!m_pCurrent)
			KLSTD_THROW(STDE_NOTPERM);
		m_pCurrent->GetNames(names);
	};

	bool attr_get(const wchar_t* szwName, KLPAR::Value** ppAttribute)
	{        
		KLSTD_CHKOUTPTR(ppAttribute);
		if(!m_pCurrent)
			KLSTD_THROW(STDE_NOTPERM);
		if(!m_pCurrent->DoesExist(szwName))
			return false;
        
        KLPAR::ValuePtr pResult;
		m_pCurrent->GetValue(szwName, &pResult);
        if( 0 == klstd_wcscmp(szwName, c_wstrData) && 
            KLPAR::Value::BINARY_T == pResult->GetType() )
        {            
            KLPAR::BinaryValuePtr pBinaryValue = 
                (KLPAR::BinaryValue*)(KLPAR::Value*)pResult;            
            KLPAR::ParamsValuePtr pVal = UnpackSection(pBinaryValue);;
            pResult = (KLPAR::Value*)(KLPAR::ParamsValue*)pVal;
            KLSTD_ASSERT(DebugCheckHash(m_pCurrent, pResult));            
        };        
        *ppAttribute = pResult.Detach();
		return true;			
	};

	void attr_put(const wchar_t* szwName, KLPAR::Value* pAttribute)
	{
		KLSTD_CHKINPTR(pAttribute);
		if(!m_pCurrent)
			KLSTD_THROW(STDE_NOTPERM);
		m_pCurrent->ReplaceValue(szwName, pAttribute);
		m_bDataWasChanged=true;
	};

    bool get_changed()
    {
        return m_bDataWasChanged;
    };
	protected:
		const std::wstring	m_wstrFileName;
		std::wstring		    m_wstrBackupFileName, m_wstrBackupFileName2, m_wstrSemaphoreName;
        KLSTD::MemoryChunkPtr   m_pReadData;
		const AVP_dword		m_dwCreation, m_dwAccess;
		bool				m_bDataWasChanged;
        bool				m_bPseudoTrans;
        bool				m_bCorrupted;
		// lockdata
		KLSTD::CAutoPtr<KLPAR::Params>					m_pCurrent,
														m_pRootData;
		KLSTD::CAutoPtr<KLPAR::ParamsValue>				m_pCurrentValue;
		KLSTD::CAutoPtr<KLSTD::FileSemaphoreNoReadLock>	m_pSemaphore;
        ss_format_t*                                    m_pFormat;
        AVP_dword                                       m_dwDeserializeFlags;
	};

	void CreateStorage(
				const std::wstring&	wstrFileName,
				AVP_dword			dwCreation,
				AVP_dword			dwAccess,
				Storage**			ppStore,
                const ss_format_t*  pFormat)
	{
		KLSTD_CHKOUTPTR(ppStore);
        *ppStore=new CStorage(wstrFileName, dwCreation, dwAccess, pFormat);
		KLSTD_CHKMEM(*ppStore);
	};

};

void KLCSKCA_DECL KLPRSS_SetMaxCacheSize(size_t nCacheSize)
{
#ifndef KLCSC_STATIC
    if(nCacheSize && nCacheSize < 1024*1024*512)
    {
        KLSTD_TRACE1(2, L"KLPRSS_SetMaxCacheSize(%u)\n", nCacheSize);
        KLPRSS::g_nMaxCacheSize = nCacheSize;
    };
#endif
};

size_t KLCSKCA_DECL KLPRSS_GetMaxCacheSize()
{
    return KLPRSS::g_nMaxCacheSize;
};

void KLCSKCA_DECL KLPRSS_UseCaching(bool bUse)
{
#ifndef KLCSC_STATIC
    KLSTD_TRACE1(2, L"KLPRSS_UseCaching(%u)\n", bUse);
    KLPRSS::g_bUseCaching = bUse;
#endif
}

bool KLCSKCA_DECL KLPRSS_IfUseCaching()
{
    return KLPRSS::g_bUseCaching;
};

void KLCSKCA_DECL KLPRSS_ForceCaching(bool bUse)
{
#ifndef KLCSC_STATIC
    KLSTD_TRACE1(2, L"KLPRSS_UseCaching(%u)\n", bUse);
    KLPRSS::g_bForceCaching = bUse;
#endif
}

bool KLCSKCA_DECL KLPRSS_IfForceCaching()
{
    return KLPRSS::g_bForceCaching;
};
