/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	paramsimpl.cpp
 * \author	Andrew Kazachkov
 * \date	08.09.2004 10:04:14
 * \brief	
 * 
 */

#include <build/general.h>
#include <std/base/klstd.h>
#include <std/memory/klmem.h>
#include "./paramsimpl.h"
#include "./valuesimpl.h"
#include "std/base/klstdutils.h"

#include <common/measurer.h>

#include <algorithm>

#define KLCS_MODULENAME L"KLPAR"

//#ifdef _WIN32
//    #include <atlbase.h>
//	#include <crtdbg.h>    
//#endif

using namespace KLSTD;
using namespace std;
using namespace KLERR;
using namespace KLPAR;

DECLARE_MODULE_INIT_DEINIT(KLSTD);
IMPLEMENT_MODULE_INIT_DEINIT(KLPAR)

namespace KLPAR
{
    namespace
    {
	    inline bool KLSTD_FASTCALL CheckName(const wchar_t* szwName)
	    {
            return (szwName && szwName[0]);
	    };

        class CSimpleEnumValues
            :   public KLPAR::EnumValuesCallback
        {
        public:
		    unsigned long AddRef()
            {
                return 0;
            };

		    unsigned long Release()                
            {
                return 0;
            };

            bool QueryInterface(KLSTD_IID iid, void** ppObject)
            {
                return false;
            };

            CSimpleEnumValues(ParamsNames * names)
                :   m_pNames(names)
            {;};

            virtual ~CSimpleEnumValues()
            {;};

            void OnEnumeratedValue(const wchar_t* szwName, KLPAR::Value*)
            {
                m_pNames->push_back(szwName);
            };
        protected:
            ParamsNames * m_pNames;
        };
    };

    KLCSC_DECL void _GetParamsNames(const KLPAR::Params* p, ParamsNames& names)
    {
        KLSTD_CHKINPTR(p);
        names.clear();
        const size_t nSize = p->GetSize();
        if(nSize)
        {
            names.reserve(nSize);
            CSimpleEnumValues en(&names);
            KLSTD::CAutoPtr<KLPAR::EnumValues> pEnum;
            const_cast<KLPAR::Params*>(p)->QueryInterface(
                    KLSTD_IIDOF(KLPAR::EnumValues), 
                    (void**)&pEnum);
            KLSTD_ASSERT_THROW(pEnum);
            pEnum->EnumerateContents(&en);
        };
    };

	//*********************************************************************
	//*
	//*	class CParamsImpl
	//*
	//*********************************************************************

#define KLPAR_LOCK_R()   \
        KLSTD::CAutoReentProtectRW arp(m_rp, false); \
        KLSTD_ASSERT_THROW(arp);

#define KLPAR_LOCK_W()   \
        KLSTD::CAutoReentProtectRW arp(m_rp, true); \
        KLSTD_ASSERT_THROW(arp);


    CParamsImpl::CParamsImpl()
        :   m_bIsReadOnly(false)
#ifdef KLPAR_USE_HASHMAP
        ,   m_data(11)
#endif
    {
        ;
    };

	CParamsImpl::~CParamsImpl()
    {
        entry_clear();
    };

    //EnumValues
    void CParamsImpl::EnumerateContents(EnumValuesCallback* pCallback)
    {
        KLSTD_CHKINPTR(pCallback);
        KLPAR_LOCK_R();
#ifdef KLPAR_USE_HASHMAP
        std::vector<KLPAR::string_t> vecStrings;
        vecStrings.reserve(m_data.size());
        for(contents_t::iterator it = m_data.begin(); it != m_data.end(); ++it)
            vecStrings.push_back(it->first);
        std::sort(vecStrings.begin(), vecStrings.end());
        const size_t c_nSize = vecStrings.size();
        for(size_t i = 0; i < c_nSize; ++i)
            pCallback->OnEnumeratedValue(vecStrings[i].get(), m_data.find(vecStrings[i])->second.m_T);
#else
        for(contents_t::iterator it = m_data.begin(); it != m_data.end(); ++it)
            pCallback->OnEnumeratedValue(it->first.get(), it->second.m_T);
#endif
    };

    //Params
	void CParamsImpl::Clone(Params** ppParams) const
    {
    KL_TMEASURE_BEGIN(L"CParamsImpl::Clone", 4)
        KLSTD_CHKOUTPTR(ppParams);
        KLPAR_LOCK_R();		
        ;
        KLSTD::CAutoPtr<KLPAR::Params> pParams;		
		::KLPAR_CreateParams(&pParams);
		for(contents_t::const_iterator it=m_data.begin(); it!=m_data.end(); ++it)
		{
			CAutoPtr<Value> pValue;
			it->second.m_T->Clone(&pValue);
			pParams->AddValue(it->first.get(), pValue);
		};
        pParams.CopyTo(ppParams);
    KL_TMEASURE_END()
    };

	void CParamsImpl::Duplicate(Params** ppParams)
    {
    //KL_TMEASURE_BEGIN(L"CParamsImpl::Duplicate", 4)
        KLSTD_CHKOUTPTR(ppParams);
        KLPAR_LOCK_R();
        ;		
		CAutoPtr<Params> pResult;
		::KLPAR_CreateParams(&pResult);
		for(contents_t::const_iterator it=m_data.begin(); it!=m_data.end(); ++it)
		{			
			if(it->second.m_T->GetType() == Value::PARAMS_T)
			{// if not null params call Duplicate which will create new copy
				CAutoPtr<Params> pCopy;
				if(((ParamsValue*)(Value*)it->second.m_T)->GetValue())
					(((ParamsValue*)(Value*)it->second.m_T)->GetValue())->Duplicate(&pCopy);
				CAutoPtr<ParamsValue> pValue;
				KLPAR::CreateValue(pCopy, &pValue);
				pResult->AddValue(it->first.get(), pValue);
			}
            else
            {
                // if not params add simply add value
				pResult->AddValue(it->first.get(), it->second.m_T);
            };
		};
		pResult.CopyTo(ppParams);
    //KL_TMEASURE_END()
    };

	int CParamsImpl::Compare(const Params* pParams) const
    {
        KLSTD_CHKINPTR(pParams);
        KLPAR_LOCK_R();		
        ;
		const contents_t& val1=m_data;
		const contents_t& val2=static_cast<const CParamsImpl*>(pParams)->m_data;
		int nSize1=val1.size();
		int nSize2=val2.size();
		int nResult=nSize1-nSize2;
		if(nResult)
			return nResult;
		contents_t::const_iterator i, j;
		for((i=val1.begin()), j=val2.begin(); i!=val1.end(); ++i, ++j)
		{
			int nResult = i->first.compare(j->first);
			if(nResult)
				return nResult;
			nResult=i->second.m_T->Compare(j->second.m_T);
			if(nResult)
				return nResult;
		};
		return 0;
    };

    void CParamsImpl::AddValue(const wchar_t* name, Value * value)
    {
        KLSTD_CHKINPTR(value);
        KLSTD_CHK(name, CheckName(name));
        ;        
        KLPAR_LOCK_W();
		entry_add(name, value, false);
    };

    void CParamsImpl::ReplaceValue(const wchar_t* name, Value * value)
    {
        KLSTD_CHKINPTR(value);
        KLSTD_CHK(name, CheckName(name));
        ;        
        KLPAR_LOCK_W();
		entry_add(name, value, true);
    };

    bool CParamsImpl::DeleteValue(const wchar_t* name, int bThrow)
    {
        KLSTD_CHK(name, CheckName(name));
        ;        
        KLPAR_LOCK_W();
        return entry_delete(name, bThrow?true:false);
    };

    void CParamsImpl::SetValue(const wchar_t* name, Value * value)
    {
        KLSTD_CHKINPTR(value);
        KLSTD_CHK(name, CheckName(name));
        ;        
        KLPAR_LOCK_W();
		entry_setval(name, value);
    };

	void CParamsImpl::GetValue(const wchar_t* name, Value** ppValue)
    {
        KLSTD_CHKOUTPTR(ppValue);
        KLSTD_CHK(name, CheckName(name));
        ;
        strdata_stat_t nameval(name);
        KLPAR_LOCK_R();
		entry_getval(&nameval, ppValue, true);
    };

	bool CParamsImpl::GetValueNoThrow(const wchar_t* name, Value** ppValue)
    {
        KLSTD_CHKOUTPTR(ppValue);
        KLSTD_CHK(name, CheckName(name));
        ;
        strdata_stat_t nameval(name);
        KLPAR_LOCK_R();
        return entry_getval(&nameval, ppValue, false);
    };

    void CParamsImpl::GetNames(names_t &names) const
    {
        KLPAR_LOCK_R();
		entry_getnames(names);
    };

    void CParamsImpl::Clear()
    {
        KLPAR_LOCK_W();
        KLSTD_ASSERT_THROW(!m_bIsReadOnly);
		entry_clear();
    };

    bool CParamsImpl::DoesExist(const wchar_t* name) const
    {
        KLSTD_CHK(name, CheckName(name));
        ;
        strdata_stat_t nameval(name);
        KLPAR_LOCK_R();
		return m_data.find(string_t(&nameval)) != m_data.end();
    };

	void CParamsImpl::CopyFrom(const Params* pSrc)
    {
        KLPAR_LOCK_R();
		KLSTD_CHKINPTR(pSrc);
		MoveOrCopy(const_cast<Params*>(pSrc), false);			
    };

	void CParamsImpl::MoveFrom(Params* pSrc)
    {
        KLPAR_LOCK_W();
		KLSTD_CHKINPTR(pSrc);
		MoveOrCopy(pSrc, true);
    };

    size_t CParamsImpl::GetSize() const
    {
        KLPAR_LOCK_R();
        return m_data.size();
    };

    void CParamsImpl::MakeReadOnly()
    {
        KLPAR_LOCK_R();
        m_bIsReadOnly = true;
        for(contents_t::iterator it=m_data.begin(); it!=m_data.end(); ++it)
        {
            if(!it->second.m_T || it->second.m_T->GetType() != Value::PARAMS_T)
                continue;
            Params* pEntry = ((ParamsValue*)(Value*)it->second.m_T)->GetValue();
            if(pEntry)
                pEntry->MakeReadOnly();
        };
    };

//internal methods
    
    void CParamsImpl::MoveOrCopy(Params* psrc, bool bMove)
    {
		CParamsImpl* pSource= static_cast<CParamsImpl*>(psrc);
		for(contents_t::iterator it=pSource->m_data.begin(); it!=pSource->m_data.end(); ++it)
		{
			CAutoPtr<Value> pNewVal;
			if(bMove)
            {
                pNewVal=it->second;
            }
			else
            {
				it->second.m_T->Clone(&pNewVal);
			};
			entry_add(it->first.get(), pNewVal, true);//we force copying by using it->first.get()
		};
		if(bMove)
			psrc->Clear();
    };

	void CParamsImpl::entry_add(const string_t& name, Value * value, bool bReplace)
    {
        KLSTD_ASSERT_THROW(!m_bIsReadOnly);
		KLSTD_ASSERT_THROW(value != NULL);
		contents_t::iterator it=m_data.find(name);
		if(it!=m_data.end())
        {// if already exists
			if(!bReplace)
				KLERR_MYTHROW1(ALREADY_EXIST, name.get());
			entry_setval(it, name, value);
		}
		else
        {
			m_data.insert(contents_t::value_type(name,ValuePtr())).first->second.m_T = value;
            /*
#ifdef KLPAR_USE_HASHMAP
            if(m_data.size() > 4)
            {
            KLSTD_ASSERT(m_data.bucket_count() >= m_data.size());
            };
#endif
            */
		};
    };

	bool CParamsImpl::entry_delete(const string_t& name, bool bThrow)
    {
        KLSTD_ASSERT_THROW(!m_bIsReadOnly);
		contents_t::iterator it=m_data.find(name);
		if(it==m_data.end())
        {// not found
			if(bThrow)
				KLERR_MYTHROW1(NOT_EXIST, name.get());
			return false;
		};
		m_data.erase(it);
		return true;
    };

	void CParamsImpl::entry_setval(contents_t::iterator it, const string_t& name, Value * value)
    {
        KLSTD_ASSERT_THROW(!m_bIsReadOnly);
		KLSTD_ASSERT_THROW(value != NULL);
		it->second = value;
    };

	void CParamsImpl::entry_setval(const string_t& name, Value * value)
    {
        KLSTD_ASSERT_THROW(!m_bIsReadOnly);
		KLSTD_ASSERT_THROW(value != NULL);
		contents_t::iterator it=m_data.find(name);
		if(it == m_data.end())
			KLERR_MYTHROW1(NOT_EXIST, name.get());
		entry_setval(it, name, value);
    };

	bool CParamsImpl::entry_getval(const string_t& name, Value** ppValue, bool bThrow)
    {
		contents_t::iterator it = m_data.find(name);
		if(it == m_data.end())
        {// not found
			if(bThrow)
				KLERR_MYTHROW1(NOT_EXIST, name.get());
			return false;
		};
		it->second.m_T.CopyTo(ppValue);
		return true;
    };

	const Value* CParamsImpl::entry_getval(const string_t& name, bool bThrow) const
    {
		contents_t::const_iterator it=m_data.find(name);
		if(it == m_data.end())
        {// not found
			if(bThrow)
				KLERR_MYTHROW1(NOT_EXIST, name.get());
			return NULL;
		};
		return it->second.m_T;
    };

	void CParamsImpl::entry_getnames(names_t &names) const
    {
        klwstrarr_t result(m_data.size());        
        size_t j = 0;
		for(contents_t::const_iterator i=m_data.begin(); i!=m_data.end(); ++i, ++j)
        {
            klwstr_t wstr = i->first.get();
            result.setat(j, wstr.detach());
        };
        names = result.detach();
    };

	void CParamsImpl::entry_clear()
    {
        m_data.clear();
        m_bIsReadOnly = false;
    };



    /************************************************************************/
    /*                                                                      */
    /************************************************************************/

    const size_t c_nParamsEntryPool = 500;
    size_t g_nParamsEntryPool = c_nParamsEntryPool;

    KLCSC_DECL void SetParamsEntryPool(size_t n)
    {
        g_nParamsEntryPool = n;
        if(!g_nParamsEntryPool)
            g_nParamsEntryPool = c_nParamsEntryPool;
    };

    CParamsNodeAllocator::CParamsNodeAllocator()
#if defined(_WIN32) && !defined(_DEBUG)
    #ifdef KLPAR_USE_HASHMAP
        :   m_nMaxAllocated(g_nParamsEntryPool/2)
    #else
        :   m_nMaxAllocated(g_nParamsEntryPool)
    #endif
#endif
    {
#if defined(_WIN32) && !defined(_DEBUG)
        KLSTD_CreateCriticalSection(&m_pCS);
        m_vecAllocated_Size1.reserve(m_nMaxAllocated);
    #ifdef KLPAR_USE_HASHMAP
        m_vecAllocated_Size2.reserve(m_nMaxAllocated);
    #endif
#endif
    };

    CParamsNodeAllocator::~CParamsNodeAllocator()
    {
        ;
    };
    void* CParamsNodeAllocator::allocate(size_t n, const void *hint)        
    {
        //KLSTD_TRACE1(1, L"size=%u\n", n);
        void* pResult = NULL;
#if defined(_WIN32) && !defined(_DEBUG)
        switch(n)
        {
    #ifdef KLPAR_USE_HASHMAP
        case 212:
            {
                KLSTD::AutoCriticalSection acs(m_pCS);
                if(!m_vecAllocated_Size2.empty())
                {
                    pResult = m_vecAllocated_Size2.back();
                    m_vecAllocated_Size2.pop_back();
                };
            }
            break;
        case 16:
    #else
        case 28:
    #endif
            {
                KLSTD::AutoCriticalSection acs(m_pCS);
                if(!m_vecAllocated_Size1.empty())
                {
                    pResult = m_vecAllocated_Size1.back();
                    m_vecAllocated_Size1.pop_back();
                };
            }
            break;
        };
#endif
        if(!pResult)
            pResult = KLSTD_AllocTmp(n);
        return pResult;
    };

    void CParamsNodeAllocator::deallocate(void* p, size_t n)
    {
        void* pResult = p;
#if defined(_WIN32) && !defined(_DEBUG)
        switch(n)
        {
    #ifdef KLPAR_USE_HASHMAP
        case 212:
            {
                KLSTD::AutoCriticalSection acs(m_pCS);
                if(m_vecAllocated_Size2.size() < m_nMaxAllocated)
                {
                    m_vecAllocated_Size2.push_back(pResult);
                    pResult = NULL;
                };
            }
            break;
        case 16:
    #else
        case 28:
    #endif
            {
                KLSTD::AutoCriticalSection acs(m_pCS);
                if(m_vecAllocated_Size1.size() < m_nMaxAllocated)
                {
                    m_vecAllocated_Size1.push_back(pResult);
                    pResult = NULL;
                };
            }
            break;
            };
#endif
        if(pResult)
            KLSTD_FreeTmp(pResult);
    };

    KLSTD::CPointer<CParamsNodeAllocator> g_pParamsNodeAllocator;
};

using namespace KLPAR;

KLCSC_DECL void KLPAR_CreateValuesFactory(KLPAR::ValuesFactory** ppFact) 
{
	KLSTD_CHKOUTPTR(ppFact);
    if(!g_pFactory)KLSTD_NOINIT(KLCS_MODULENAME);
	g_pFactory.CopyTo(ppFact);
};


IMPLEMENT_MODULE_INIT_BEGIN(KLPAR)
	CALL_MODULE_INIT(KLSTD);
	KLERR_InitModuleDescriptions(
		KLCS_MODULENAME,
		KLPAR::c_errorDescriptions,
		KLSTD_COUNTOF(KLPAR::c_errorDescriptions));
    KLPAR::g_pParamsNodeAllocator = new KLPAR::CParamsNodeAllocator;
    InitValues();
IMPLEMENT_MODULE_INIT_END()

IMPLEMENT_MODULE_DEINIT_BEGIN(KLPAR)
    DeinitValues();
    KLPAR::g_pParamsNodeAllocator = NULL;
	KLERR_DeinitModuleDescriptions(KLCS_MODULENAME);
	CALL_MODULE_DEINIT(KLSTD);
IMPLEMENT_MODULE_DEINIT_END()
