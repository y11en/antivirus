/*!
 * (C) 2002 Kaspersky Lab 
 * 
 * \file	ss_sync.cpp
 * \author	Andrew Kazachkov
 * \date	10.11.2002 11:03:24
 * \brief	
 * 
 */

#include <build/general.h>
#include <std/base/klbase.h>
#include <std/err/error.h>
#include <std/par/paramsi.h>
#include <std/par/errors.h>
#include <std/par/par_conv.h>
#include "kca/prss/errors.h"
#include "kca/prss/ss_sync.h"
#include <common/measurer.h>

#include "kca/prss/helpersi.h"
#include "kca/prss/store.h"

#include "kca/prci/componentid.h"
#include "kca/prci/prci_const.h"

#include "transport/ev/event.h"
#include "transport/ev/eventsource.h"

#define KLCS_MODULENAME L"SSSYNC"

using namespace std;
using namespace KLSTD;
using namespace KLPAR;
using namespace KLERR;
using namespace KLPRSS;

		static void modify_path(wchar_t** path, wchar_t* &szEntry)
		{
			szEntry=NULL;
			for(int i=0; path[i]; ++i)
			{
				if(path[i] && !path[i+1]){
					szEntry=path[i];
					path[i]=NULL;
					break;
				};
			};
		};

namespace
{
    const wchar_t c_szwAllowSemiPacked[] = L"allow_semipacked"; //BOOL_T
}

namespace KLPRSS
{
    void ConvertSectionIntoSemiPacked(KLPAR::ParamsPtr pSec, bool bSemiPacked, bool& bChanged);

	void KLCSKCA_DECL getNew(Params* pCurrent, BoolValue** ppBoolValue)
	{
		if(!pCurrent->DoesExist(c_wstrNew))
		{
			CAutoPtr<BoolValue> pBoolValue;
			KLPAR::CreateValue(true, &pBoolValue);
			pCurrent->AddValue(c_wstrNew, pBoolValue);			
		};
		pCurrent->GetValue(c_wstrNew, (Value**)ppBoolValue);
		KLPAR_CHKTYPE(*ppBoolValue, BOOL_T, c_wstrNew);
	};

	void KLCSKCA_DECL getChanged(Params* pCurrent, BoolValue** ppBoolValue)
	{
		if(!pCurrent->DoesExist(c_wstrChanged))
		{
			CAutoPtr<BoolValue> pBoolValue;
			KLPAR::CreateValue(true, &pBoolValue);
			pCurrent->AddValue(c_wstrChanged, pBoolValue);			
		};
		pCurrent->GetValue(c_wstrChanged, (Value**)ppBoolValue);
		KLPAR_CHKTYPE(*ppBoolValue, BOOL_T, c_wstrChanged);		
	};

	class CStoreSync: public KLSTD::KLBaseImpl<IStoreSync>
	{
	public:
		CStoreSync(const std::wstring& wstrFileName, const ss_format_t*  pFormat):
			KLSTD::KLBaseImpl<IStoreSync>(),
			m_wstrFileName(wstrFileName)
		{
			CreateStorage(
                        m_wstrFileName,
                        CF_OPEN_ALWAYS,
                        AF_READ|AF_WRITE,
                        &m_pStorage,
                        pFormat);
		};

		virtual ~CStoreSync()
		{
			m_pStorage=NULL;
		};

        void sync_get_store(Storage** ppStore)
        {
            m_pStorage.CopyTo(ppStore);
        };

		void sync_lock(long lTimeout)
		{
			m_pStorage->trans_begin(true, lTimeout, true);
		};

		void sync_unlock(bool bCommit)
		{
			m_pStorage->trans_end(bCommit);
			m_pParams=NULL;
		};

		void sync_enum(const wchar_t** pszAbsPath, std::vector<std::wstring>& names)
		{
			m_pStorage->region_load(pszAbsPath, false);
			m_pStorage->entry_enumerate(names);
		};

		void sync_read(const wchar_t** pszAbsPath, KLPAR::Params** ppParams)
		{
			m_pStorage->region_load(pszAbsPath, false);
			m_pStorage->entry_readall(ppParams);
		};
        
		void sync_read2(const wchar_t** pszAbsPath, KLPAR::Params** ppParams, SYNC_SSF nFlags)
		{
			m_pStorage->region_load(pszAbsPath, false);
            m_pStorage->entry_readall2(ppParams, (SYNC_SSF_COMPATIBLE != nFlags));
            if(SYNC_SSF_FORCE_SEMIPACKED == nFlags)
            {
                KLSTD_TRACE0(4, L"sync_read2: forcing section into semipacked\n");
                bool bChanged = false;
                ConvertSectionIntoSemiPacked(*ppParams, true, bChanged);
            };
		};
        
        virtual void sync_new(const wchar_t** pszAbsPath, bool bNewValue)
		{
			m_pStorage->region_load(pszAbsPath, true);
			CAutoPtr<BoolValue> pBoolValue;
			CreateValue(bNewValue, &pBoolValue);
			m_pStorage->attr_put(c_wstrNew, pBoolValue);
		};

		void sync_changed(const wchar_t** pszAbsPath, bool bChangedValue)
		{
			m_pStorage->region_load(pszAbsPath, true);
			CAutoPtr<BoolValue> pBoolValue;
			CreateValue(bChangedValue, &pBoolValue);
			m_pStorage->attr_put(c_wstrChanged, pBoolValue);
		};

		void sync_write(const wchar_t** pszAbsPath, KLPAR::Params* pParams)
		{
			m_pStorage->region_load(pszAbsPath, true);
			m_pStorage->entry_writeall(pParams);
		};

		void sync_create(const wchar_t** pszAbsPath)
		{			
			int nLen;
			for(nLen=0; pszAbsPath[nLen]; ++nLen);
			++nLen;
			wchar_t** ppCopy=(wchar_t**)alloca(nLen * sizeof(const wchar_t*));
			memcpy(ppCopy, pszAbsPath, nLen * sizeof(const wchar_t*));
			wchar_t* szwEntry=NULL;
			modify_path(ppCopy, szwEntry);
			m_pStorage->region_load((const wchar_t**)ppCopy, true);
			m_pStorage->entry_create(szwEntry);
		};
		
		void sync_delete(const wchar_t** pszAbsPath, bool bThrow)
		{
			int nLen;
			for(nLen=0; pszAbsPath[nLen]; ++nLen);
			++nLen;
			wchar_t** ppCopy=(wchar_t**)alloca(nLen * sizeof(const wchar_t*));
			memcpy(ppCopy, pszAbsPath, nLen * sizeof(const wchar_t*));
			wchar_t* szwEntry=NULL;
			modify_path(ppCopy, szwEntry);
			m_pStorage->region_load((const wchar_t**)ppCopy, true);
			m_pStorage->entry_destroy(szwEntry);
		};
	protected:
		const std::wstring	m_wstrFileName;
		CAutoPtr<Params>	m_pParams;
		CAutoPtr<Storage>	m_pStorage;
	};

	void CreateStoreSync(
                    const std::wstring& wstrFileName,
                    IStoreSync**        ppSync,
                    const ss_format_t*  pFormat)
	{
		KLSTD_CHKOUTPTR(ppSync);
		*ppSync=new CStoreSync(wstrFileName, pFormat);
        KLSTD_CHKMEM(*ppSync);
	};

};

namespace KLPRSS
{
	static const wchar_t c_szwSyncType[]=L"nType";		// INT_T, 1 if headers, 2 if sections, 3 if SAVEFLAG
	static const wchar_t c_szwSyncHeaders[]=L"arrHeaders";	// ARRAY_T
	static const wchar_t c_szwSyncSections[]=L"arrSections";	// ARRAY_T
	static const wchar_t c_szwSyncSaveFlag[]=L"bSaveFlag"; // BOOL_T

	static const wchar_t c_szwSyncProduct[]=L"wstrProduct";
	static const wchar_t c_szwSyncVersion[]=L"wstrVersion";
	static const wchar_t c_szwSyncSection[]=L"wstrSection";
	static const wchar_t c_szwSyncChanged[]=L"bChanged";
    static const wchar_t c_szwSyncNew[]=L"bNew";

	static const wchar_t c_szwSyncAction[]=L"nAction";
	static const wchar_t c_szwSyncContents[]=L"parContents";

	static const wchar_t c_szwSyncEOF[]=L"bEOF";

	static void put_headers(headers_map_t& headers, Params** ppParams)
	{	
    KL_TMEASURE_BEGIN(L"put_headers", 4)
		KLPAR_CreateParams(ppParams);
		CAutoPtr<ValuesFactory> pFactory;
		KLPAR_CreateValuesFactory(&pFactory);
		CAutoPtr<IntValue> pIntValue;
		KLPAR::CreateValue(1, &pIntValue);
		CAutoPtr<ArrayValue> pArray;
		pFactory->CreateArrayValue(&pArray);
		(*ppParams)->AddValue(c_szwSyncType, pIntValue);
		(*ppParams)->AddValue(c_szwSyncHeaders, pArray);
		;
		pArray->SetSize(headers.size());
		int i=0;
		for(headers_map_t::iterator it=headers.begin(); it!=headers.end(); ++it, ++i)
		{
			section_header_t& header=it->second;
			CAutoPtr<Params> pTmpParams;
			KLPAR_CreateParams(&pTmpParams);		
			{
				CAutoPtr<ParamsValue> pParamsValue;
				KLPAR::CreateValue(pTmpParams, &pParamsValue);
				pArray->SetAt(i, pParamsValue);
			};		
			CAutoPtr<StringValue> pProduct, pVersion, pSection;
			CAutoPtr<BoolValue>   pChanged, pNew;
			KLPAR::CreateValue(header.wstrProduct.c_str(), &pProduct);
			KLPAR::CreateValue(header.wstrVersion.c_str(), &pVersion);
			KLPAR::CreateValue(header.wstrSection.c_str(), &pSection);
			KLPAR::CreateValue(header.bChanged, &pChanged);
            KLPAR::CreateValue(header.bNew, &pNew);
            
			;
			pTmpParams->AddValue(c_szwSyncProduct, pProduct);
			pTmpParams->AddValue(c_szwSyncVersion, pVersion);
			pTmpParams->AddValue(c_szwSyncSection, pSection);
			pTmpParams->AddValue(c_szwSyncChanged, pChanged);
            pTmpParams->AddValue(c_szwSyncNew, pNew);
		};
    KL_TMEASURE_END()
	};

	static void get_headers(Params* pParams, headers_map_t& headers)
	{
    KL_TMEASURE_BEGIN(L"get_headers", 4)
		headers.clear();
		CAutoPtr<IntValue> pIntValue;
		pParams->GetValue(c_szwSyncType, (Value**)&pIntValue);
		KLPAR_CHKTYPE(pIntValue, INT_T, c_szwSyncType);
		if(pIntValue->GetValue() != 1)
			KLSTD_THROW(STDE_CANCELED);
		CAutoPtr<ArrayValue> pArray;
		pParams->GetValue(c_szwSyncHeaders, (Value**)&pArray);
		KLPAR_CHKTYPE(pArray, ARRAY_T, c_szwSyncHeaders);
		int nSize=pArray->GetSize();
		for(int i=0; i < nSize; ++i)
		{
			section_header_t header;
			CAutoPtr<StringValue> pProduct, pVersion, pSection;
			CAutoPtr<BoolValue>   pChanged, pNew;
			;
			CAutoPtr<Params> pTmpParams;
			{
				CAutoPtr<ParamsValue> pParamsValue;
				pArray->GetAt(i, (Value**)&pParamsValue);
				KLPAR_CHKTYPE(pParamsValue, PARAMS_T, c_szwSyncHeaders);
				pTmpParams=pParamsValue->GetValue();
			};		
			;
			pTmpParams->GetValue(c_szwSyncProduct, (Value**)&pProduct);
			KLPAR_CHKTYPE(pProduct, STRING_T, c_szwSyncProduct);
			pTmpParams->GetValue(c_szwSyncVersion, (Value**)&pVersion);
			KLPAR_CHKTYPE(pVersion, STRING_T, c_szwSyncVersion);
			pTmpParams->GetValue(c_szwSyncSection, (Value**)&pSection);
			KLPAR_CHKTYPE(pSection, STRING_T, c_szwSyncSection);
			pTmpParams->GetValue(c_szwSyncChanged, (Value**)&pChanged);
			KLPAR_CHKTYPE(pChanged, BOOL_T, c_szwSyncChanged);
            KLSTD_ASSERT(pTmpParams->DoesExist(c_szwSyncNew));
			pTmpParams->GetValue(c_szwSyncNew, (Value**)&pNew);
			KLPAR_CHKTYPE(pNew, BOOL_T, c_szwSyncNew);
            
			;
			header.wstrProduct=pProduct->GetValue();
			header.wstrVersion=pVersion->GetValue();
			header.wstrSection=pSection->GetValue();
			header.bChanged=pChanged->GetValue();
            header.bNew=pNew->GetValue();
			header.bSuccess=true;
			;
			headers[secaddr_t(header.wstrProduct, header.wstrVersion, header.wstrSection)]=header;
		};
    KL_TMEASURE_END()
	};

	static void put_sections(sections_list_t& sections, Params** ppParams, bool bEOF)
	{
    KL_TMEASURE_BEGIN(L"put_sections", 4)
		KLPAR_CreateParams(ppParams);
		CAutoPtr<ValuesFactory> pFactory;
		KLPAR_CreateValuesFactory(&pFactory);
		CAutoPtr<IntValue> pIntValue;
		KLPAR::CreateValue(2, &pIntValue);
		CAutoPtr<ArrayValue> pArray;
		pFactory->CreateArrayValue(&pArray);
		(*ppParams)->AddValue(c_szwSyncType, pIntValue);
		(*ppParams)->AddValue(c_szwSyncSections, pArray);
		{
			CAutoPtr<BoolValue> pEOF;
			KLPAR::CreateValue(bEOF, &pEOF);
			(*ppParams)->AddValue(c_szwSyncEOF, pEOF);
		};
		pArray->SetSize(sections.size());
		int i=0;
		for(sections_list_t::iterator it=sections.begin(); it!=sections.end(); ++it, ++i)
		{
			section_data_t& sec=*it;
			CAutoPtr<Params> pTmpParams;
			KLPAR_CreateParams(&pTmpParams);
			{
				CAutoPtr<ParamsValue> pParamsValue;
				KLPAR::CreateValue(pTmpParams, &pParamsValue);
				pArray->SetAt(i, pParamsValue);
			};		
			CAutoPtr<StringValue>	pProduct, pVersion, pSection;
			CAutoPtr<IntValue>		pAction;
			CAutoPtr<ParamsValue>	pContents;
			CAutoPtr<Params>		pContentsParams;
			
			if(sec.pContents)
				sec.pContents->Clone(&pContentsParams);//@todo really need clone ?
			KLPAR::CreateValue(sec.wstrProduct.c_str(), &pProduct);
			KLPAR::CreateValue(sec.wstrVersion.c_str(), &pVersion);
			KLPAR::CreateValue(sec.wstrSection.c_str(), &pSection);
			KLPAR::CreateValue((long)sec.nAction, &pAction);
			KLPAR::CreateValue(pContentsParams, &pContents);
			;
			pTmpParams->AddValue(c_szwSyncProduct, pProduct);
			pTmpParams->AddValue(c_szwSyncVersion, pVersion);
			pTmpParams->AddValue(c_szwSyncSection, pSection);
			pTmpParams->AddValue(c_szwSyncAction, pAction);
			pTmpParams->AddValue(c_szwSyncContents, pContents);
		};
    KL_TMEASURE_END()
	};

	static void get_sections(Params* pParams, sections_list_t& sections, bool& bEOF)
	{
    KL_TMEASURE_BEGIN(L"get_sections", 4)
		sections.clear();
		CAutoPtr<IntValue> pIntValue;
		pParams->GetValue(c_szwSyncType, (Value**)&pIntValue);
		KLPAR_CHKTYPE(pIntValue, INT_T, c_szwSyncType);
		if(pIntValue->GetValue() != 2)
			KLSTD_THROW(STDE_CANCELED);
		{
			CAutoPtr<BoolValue> pEOF;
			pParams->GetValue(c_szwSyncEOF, (Value**)&pEOF);
			KLPAR_CHKTYPE(pEOF, BOOL_T, c_szwSyncEOF);
			bEOF=pEOF->GetValue();
		};
		CAutoPtr<ArrayValue> pArray;
		pParams->GetValue(c_szwSyncSections, (Value**)&pArray);
		KLPAR_CHKTYPE(pArray, ARRAY_T, c_szwSyncSections);
		int nSize=pArray->GetSize();
		for(int i=0; i < nSize; ++i)
		{
			section_data_t sec;
			CAutoPtr<StringValue>	pProduct, pVersion, pSection;
			CAutoPtr<IntValue>		pAction;
			CAutoPtr<ParamsValue>	pContents;
			;
			CAutoPtr<Params> pTmpParams;
			{
				CAutoPtr<ParamsValue> pParamsValue;
				pArray->GetAt(i, (Value**)&pParamsValue);
				KLPAR_CHKTYPE(pParamsValue, PARAMS_T, c_szwSyncSections);
				pTmpParams=pParamsValue->GetValue();
			};		
			;
			pTmpParams->GetValue(c_szwSyncProduct, (Value**)&pProduct);
			KLPAR_CHKTYPE(pProduct, STRING_T, c_szwSyncProduct);
			pTmpParams->GetValue(c_szwSyncVersion, (Value**)&pVersion);
			KLPAR_CHKTYPE(pVersion, STRING_T, c_szwSyncVersion);
			pTmpParams->GetValue(c_szwSyncSection, (Value**)&pSection);
			KLPAR_CHKTYPE(pSection, STRING_T, c_szwSyncSection);
			pTmpParams->GetValue(c_szwSyncAction, (Value**)&pAction);
			KLPAR_CHKTYPE(pAction, INT_T, c_szwSyncAction);
			pTmpParams->GetValue(c_szwSyncContents, (Value**)&pContents);
			KLPAR_CHKTYPE(pContents, PARAMS_T, c_szwSyncContents);

			;
			sec.wstrProduct=pProduct->GetValue();
			sec.wstrVersion=pVersion->GetValue();
			sec.wstrSection=pSection->GetValue();
			sec.nAction=(SEC_ACTION)pAction->GetValue();
			sec.pContents=pContents->GetValue();
			sec.bResult=true;
			;
			sections.push_back(sec);
		};	
    KL_TMEASURE_END();
	};

	KLCSKCA_DECL void put_saveflag(bool bSaveFlag, Params** ppParams)
	{
		KLPAR_CreateParams(ppParams);
		CAutoPtr<ValuesFactory> pFactory;
		KLPAR_CreateValuesFactory(&pFactory);
		CAutoPtr<IntValue> pIntValue;
		KLPAR::CreateValue(3, &pIntValue);
		CAutoPtr<BoolValue> pSaveFlag;
		KLPAR::CreateValue(bSaveFlag, &pSaveFlag);
		(*ppParams)->AddValue(c_szwSyncType, pIntValue);
		(*ppParams)->AddValue(c_szwSyncSaveFlag, pSaveFlag);
	};

	void get_saveflag(Params* pParams, bool& bSaveFlag)
	{
		CAutoPtr<IntValue> pIntValue;
		pParams->GetValue(c_szwSyncType, (Value**)&pIntValue);
		KLPAR_CHKTYPE(pIntValue, INT_T, c_szwSyncType);
		if(pIntValue->GetValue() != 3)
			KLSTD_THROW(STDE_CANCELED);
		CAutoPtr<BoolValue> pSaveFlag;
		pParams->GetValue(c_szwSyncSaveFlag, (Value**)&pSaveFlag);
		KLPAR_CHKTYPE(pSaveFlag, BOOL_T, c_szwSyncSaveFlag);
		bSaveFlag=pSaveFlag->GetValue();
	};


	static void FillHeaders(IStoreSync* pStore, headers_map_t& headers)
	{
    KL_TMEASURE_BEGIN(L"FillHeaders", 4)
		KLSTD_ASSERT(pStore && headers.size()==0);

		std::vector<std::wstring> vecProducts;
		const wchar_t* path[]={NULL};
		pStore->sync_enum(path, vecProducts);
		for(int iProduct=0; iProduct < (int)vecProducts.size(); ++iProduct)
		{
			const std::wstring& wstrProduct=vecProducts[iProduct];
			std::vector<std::wstring> vecVersions;
			const wchar_t* path[]={wstrProduct.c_str(), NULL};
			pStore->sync_enum(path, vecVersions);
			for(int iVersion=0; iVersion < (int)vecVersions.size(); ++iVersion)
			{
				const std::wstring& wstrVersion=vecVersions[iVersion];
				std::vector<std::wstring> vecSections;
				const wchar_t* path[]={wstrProduct.c_str(), wstrVersion.c_str(), NULL};
				pStore->sync_enum(path, vecSections);
				for(int iSection=0; iSection < (int)vecSections.size(); ++iSection)
				{
					const std::wstring& wstrSection=vecSections[iSection];
					
					const wchar_t* path[]={	wstrProduct.c_str(),
											wstrVersion.c_str(),
											wstrSection.c_str(),
											NULL};
					CAutoPtr<Params>	pContents;
					pStore->sync_read2(path, &pContents, SYNC_SSF_ALLOW_SEMIPACKED);
					CAutoPtr<BoolValue>	pChanged, pNew;
					getChanged(pContents, &pChanged);
                    getNew(pContents, &pNew);
					
					section_header_t header;
					header.wstrProduct=wstrProduct;
					header.wstrVersion=wstrVersion;
					header.wstrSection=wstrSection;
					header.bChanged=pChanged->GetValue();
                    header.bNew=pNew->GetValue();
					header.bSuccess=true;
					secaddr_t sa(header.wstrProduct, header.wstrVersion, header.wstrSection);
					KLSTD_ASSERT(headers.find(sa)==headers.end());
					headers[sa]=header;
				};
			};
		};
    KL_TMEASURE_END();
	};

	static void makepath(IStoreSync* pStore, const wchar_t** path)
	{
		int nSize;
		for(nSize=0; path[nSize]; ++nSize);
		if(!nSize)return;
		std::vector<const wchar_t*> p(nSize+1, (const wchar_t*)NULL);

		//const wchar_t** p=(const wchar_t**)alloca((nSize+1)*sizeof(wchar_t*));
		//memset(p, 0, (nSize+1)*sizeof(wchar_t*));
		for(int i=0; i < nSize; ++i)
		{
			p[i]=path[i];
			KLERR_BEGIN
				pStore->sync_create(&p[0]);
			KLERR_END
		};
	};

	void ClearChanged(Params* pParams)
	{
		CAutoPtr<BoolValue> pBoolValue;
		getChanged(pParams, &pBoolValue);
		pBoolValue->SetValue(false);
	};

	void ClearNew(Params* pParams)
	{
		CAutoPtr<BoolValue> pBoolValue;
		getNew(pParams, &pBoolValue);
		pBoolValue->SetValue(false);
	};

    struct section_changed_t
    {
        DataChangeType  m_nChangeType;
        secaddr_t       m_SecAddr;
    };

    typedef std::list<section_changed_t> changed_t;


	static void DoCommand(IStoreSync* pStore, section_data_t& sec, bool bImClient, changed_t& lstChanged)
	{
    KL_TMEASURE_BEGIN(L"DoCommand", 4)
		const wchar_t* path[]=
		{
			sec.wstrProduct.empty() ? NULL : sec.wstrProduct.c_str(),
			sec.wstrVersion.empty() ? NULL : sec.wstrVersion.c_str(),
			sec.wstrSection.empty() ? NULL : sec.wstrSection.c_str(),
			NULL
		};
		;
        if( 
            ((sec.nAction == SA_CREATE_C || sec.nAction == SA_MERGE_S2C) && bImClient) ||
            ((sec.nAction == SA_CREATE_S || sec.nAction == SA_MERGE_C2S) && !bImClient))
        {
			makepath(pStore, path);
			if(!sec.pContents)
			{
				KLPAR_CreateParams(&sec.pContents);
			};
			ClearChanged(sec.pContents);
            ClearNew(sec.pContents);
			pStore->sync_write(path, sec.pContents);
            ;
            section_changed_t& changed=*lstChanged.insert(lstChanged.end());
            changed.m_SecAddr.m_wstrProduct=sec.wstrProduct;
            changed.m_SecAddr.m_wstrVersion=sec.wstrVersion;
            changed.m_SecAddr.m_wstrSection=sec.wstrSection;
            if(sec.nAction == SA_MERGE_S2C || sec.nAction == SA_MERGE_C2S)
                changed.m_nChangeType=DCT_CHANGED;
            else
                changed.m_nChangeType=DCT_ADDED;
		}
        else
        if(
            (sec.nAction == SA_REMOVE_C && bImClient) ||
            (sec.nAction == SA_REMOVE_S && !bImClient))
        {
			pStore->sync_delete(path, false);
            ;
            section_changed_t& changed=*lstChanged.insert(lstChanged.end());
            changed.m_SecAddr.m_wstrProduct=sec.wstrProduct;
            changed.m_SecAddr.m_wstrVersion=sec.wstrVersion;
            changed.m_SecAddr.m_wstrSection=sec.wstrSection;
            changed.m_nChangeType=DCT_REMOVED;
        };
    KL_TMEASURE_END()
	};


	static void SendHeaders(Synchronizer* pSync, headers_map_t& headers)
	{
    KL_TMEASURE_BEGIN(L"SendHeaders", 4)
		CAutoPtr<Params> pParams;
		put_headers(headers, &pParams);

        KLPAR::BoolValuePtr pAllowSemiPacked;
        KLPAR::CreateValue(true, &pAllowSemiPacked);
        pParams->ReplaceValue(c_szwAllowSemiPacked, pAllowSemiPacked);

		pSync->SndParams(pParams);
    KL_TMEASURE_END()
	};

	static void RcvHeaders(Synchronizer* pSync, headers_map_t& headers, bool& bAllowSemiPacked)
	{
    KL_TMEASURE_BEGIN(L"RcvHeaders", 4)
		CAutoPtr<Params> pParams;
		pSync->RcvParams(&pParams);
        bAllowSemiPacked = KLPAR::ParVal(GetVal(pParams, c_szwAllowSemiPacked), false);
        KLSTD_TRACE1(4, L"bAllowSemiPacked=%u\n", bAllowSemiPacked);
		get_headers(pParams, headers);		
    KL_TMEASURE_END()
	};

	static int RcvSections(Synchronizer* pSync, sections_list_t& rcvData, bool& bEOF, bool& bAllowSemiPacked)
	{
    KL_TMEASURE_BEGIN(L"RcvSections", 4)
		//KLSTD_TRACE0(3, L"Receiving sections...\n");
		CAutoPtr<Params> pParams;
		pSync->RcvParams(&pParams);
        bAllowSemiPacked = KLPAR::ParVal(GetVal(pParams, c_szwAllowSemiPacked), false);
        KLSTD_TRACE1(4, L"bAllowSemiPacked=%u\n", bAllowSemiPacked);
		get_sections(pParams, rcvData, bEOF);
        /*
        if(KLSTD::IsTraceStarted())
        {
            if(rand() % 7 == 1)
                KLPAR_SerializeToFileName2(KLSTD_CreateGUIDString(), KLPAR_SF_BINARY, pParams);
        };
        */
		//KLSTD_TRACE0(3, L"... OK Receiving sections\n");
    KL_TMEASURE_END()
		return rcvData.size();
	};

	static int SndSections(Synchronizer* pSync, sections_list_t& sndData, bool bEOF)
	{
    KL_TMEASURE_BEGIN(L"SndSections", 4)
		KLSTD_TRACE1(3, L"Sending sections (bEOF=%d)...\n", bEOF);
		CAutoPtr<Params> pParams;
		put_sections(sndData, &pParams, bEOF);
        
        KLPAR::BoolValuePtr pAllowSemiPacked;
        KLPAR::CreateValue(true, &pAllowSemiPacked);
        pParams->ReplaceValue(c_szwAllowSemiPacked, pAllowSemiPacked);

		pSync->SndParams(pParams);
		KLSTD_TRACE1(3, L"...OK Sending sections (bEOF=%d)\n", bEOF);
    KL_TMEASURE_END()
		return sndData.size();
	};

	static void SndSaveFlag(Synchronizer* pSync, bool bSaveFlag)
	{
    KL_TMEASURE_BEGIN(L"SndSaveFlag", 4)
		KLSTD_TRACE1(3, L"Sending save flag (bSaveFlag=%d)...\n", bSaveFlag);
		CAutoPtr<Params> pParams;
		put_saveflag(bSaveFlag, &pParams);
		pSync->SndParams(pParams);
		KLSTD_TRACE1(3, L"...OK sending save flag (bSaveFlag=%d)\n", bSaveFlag);
    KL_TMEASURE_END()
	};

	static bool RcvSaveFlag(Synchronizer* pSync)
	{
		KLSTD_TRACE0(3, L"Receiving save flag...\n");
		bool bSaveFlag;
		CAutoPtr<Params> pParams;
		pSync->RcvParams(&pParams);
		get_saveflag(pParams, bSaveFlag);
		KLSTD_TRACE0(3, L"...OK Receiving save flag\n");
		return bSaveFlag;
	};

    static void PublishNotifications(
                    changed_t&          lstChanged, 
                    bool                bImClient, 
                    const std::wstring& wstrType,
                    ParamsPtr           pType)
    {
    KL_TMEASURE_BEGIN(L"PublishNotifications", 4)
        if(lstChanged.empty())return;

        KLPRCI::ComponentId idComponent;
        KLSTD_GetDefaultProductAndVersion(idComponent.productName, idComponent.version);
        if(bImClient)
        {
            idComponent.componentName=KLCS_COMPONENT_NAGENT;
            idComponent.instanceId=L""/*KLCS_WELLKNOWN_NAGENT*/;
        }
        else
        {
            idComponent.componentName=KLCS_COMPONENT_SERVER;
            idComponent.instanceId=KLCS_WELLKNOWN_SERVER;
        };
        bool bIsSS_Settings=false;
        KLERR_BEGIN
            //CAutoPtr<Params> pType;
            //KLPRSS_ExpandType(wstrType, &pType);        
            bIsSS_Settings=(GetStringValue(pType, KLPRSS::c_szwSSP_Type) == c_szwSST_HostSS);
        KLERR_END
        for(changed_t::iterator it=lstChanged.begin(); it != lstChanged.end(); ++it)
        {
        KLERR_BEGIN
            section_changed_t& sec=*it;
            CAutoPtr<Params> pEventBody;            
            KLSTD_TRACE4(
                        3,
                        L"Notify about section change:'%ls'-'%ls'-'%ls' for ss '%ls'\n",
                        sec.m_SecAddr.m_wstrProduct.c_str(),
                        sec.m_SecAddr.m_wstrVersion.c_str(),
                        sec.m_SecAddr.m_wstrSection.c_str(),
                        wstrType.c_str());
            param_entry_t pars[]=
            {
                param_entry_t(KLPRCI::c_evpProductName, sec.m_SecAddr.m_wstrProduct.c_str()),
                param_entry_t(KLPRCI::c_evpVersion, sec.m_SecAddr.m_wstrVersion.c_str()),
                param_entry_t(KLPRCI::c_evpSection, sec.m_SecAddr.m_wstrSection.c_str()),
                param_entry_t(KLPRCI::c_evpSsType, wstrType.c_str()),
                param_entry_t(KLPRCI::c_evpIs_SETTINGS, bIsSS_Settings),
                param_entry_t(KLPRCI::c_evpChangeType, (long)sec.m_nChangeType),
                param_entry_t(KLPRCI::c_evpUserName, L"")
            };
            KLPAR::CreateParamsBody(pars, KLSTD_COUNTOF(pars), &pEventBody);
            CAutoPtr<KLEV::Event> pEvent;
            KLEV_CreateEvent(&pEvent, idComponent, KLPRCI::c_EventSettingsChange, pEventBody, time(NULL), 0);
            KLEV_GetEventSource()->PublishEvent(pEvent);
        KLERR_ENDT(3)
        };
    KL_TMEASURE_END()
    };

	void KLCSKCA_DECL StartFullSync(                    
					const std::wstring&     wstrLocation,
					SYNC_TYPE			    nType,
					Synchronizer*		    pSync,
					int					    nGroupSize,
					long				    lTimeout,
                    const std::wstring&	    wstrType)
	{
    KL_TMEASURE_BEGIN(L"StartFullSync", 3);
		KLSTD_TRACE4(
			3,
			L"Initiating full synchronization: wstrLocation=\"%ls\", nType=%d, nGroupSize=%d, lTimeout=%d\n",
			wstrLocation.c_str(),
			nType,
			nGroupSize,
			lTimeout);
        ;
        changed_t lstChanged;
        ;
		CAutoPtr<IStoreSync> pStore;
        ss_format_t fid;
        fid.id = SSF_ID_SEMIPACKED;
        CreateStoreSync(wstrLocation, &pStore, &fid);
		pStore->sync_lock(lTimeout);
		headers_map_t headers;		
		FillHeaders(pStore, headers);
		;KLSTD_TRACE1(3, L"Headers collected: %d\n", headers.size());
		KLERR_TRY
			SendHeaders(pSync, headers);
			;KLSTD_TRACE0(3, L"Headers were sent\n");
			for(bool bEOF=false; !bEOF; )
			{
				sections_list_t rcvData, sndData;
                bool bAllowSemiPacked = false;
				int iResult = RcvSections(pSync, rcvData, bEOF, bAllowSemiPacked);
				;KLSTD_TRACE1(3, L"Sections were received: %d\n", iResult);
				KLSTD_ASSERT(iResult >= 0 && iResult <= nGroupSize);
				for(sections_list_t::iterator it=rcvData.begin(); it!=rcvData.end(); ++it)
				{
					section_data_t& rcvSec=*it;
					headers_map_t::iterator rcvIt=headers.find(secaddr_t(
                                                        rcvSec.wstrProduct,
                                                        rcvSec.wstrVersion,
                                                        rcvSec.wstrSection));
                    //если секцию нужно создать на клиенте
					if( rcvSec.nAction==SA_CREATE_C || rcvSec.nAction==SA_MERGE_S2C)
					{
						section_data_t sndSec;
						sndSec.wstrProduct=rcvSec.wstrProduct;
						sndSec.wstrVersion=rcvSec.wstrVersion;
						sndSec.wstrSection=rcvSec.wstrSection;
						sndSec.nAction=rcvSec.nAction;
						const wchar_t* path[]=
						{
							sndSec.wstrProduct.empty() ? NULL : sndSec.wstrProduct.c_str(),
							sndSec.wstrVersion.empty() ? NULL : sndSec.wstrVersion.c_str(),
							sndSec.wstrSection.empty() ? NULL : sndSec.wstrSection.c_str(),
							NULL
						};
						pStore->sync_new(path, false);
                        pStore->sync_changed(path, false);
                        pStore->sync_read2(
                                        path, 
                                        &sndSec.pContents, 
                                        bAllowSemiPacked
                                            ?   SYNC_SSF_ALLOW_SEMIPACKED
                                            :   SYNC_SSF_COMPATIBLE);
						;
						sndSec.bResult=true;
						;KLSTD_TRACE3(
							3,
							L"Preparing section (\"%ls\"-\"%ls\"-\"%ls\") for send\n",
							sndSec.wstrProduct.c_str(),
							sndSec.wstrVersion.c_str(),
							sndSec.wstrSection.c_str());
						sndData.push_back(sndSec);
					}
					else{
						;KLSTD_TRACE4(
							3,
							L"Updating section (\"%ls\"-\"%ls\"-\"%ls\") with action %d\n",
							rcvSec.wstrProduct.c_str(),
							rcvSec.wstrVersion.c_str(),
							rcvSec.wstrSection.c_str(),
							rcvSec.nAction);
						DoCommand(pStore, rcvSec, false, lstChanged);
					};
				};				
				SndSections(pSync, sndData, bEOF);
				;KLSTD_TRACE1(3, L"Sections were sent: %d\n", sndData.size());
			};			
            CAutoPtr<KLPRSS::Storage> pStorage;
            pStore->sync_get_store(&pStorage);
            bool bIsStoreNew=GetStoreNew(pStorage);
            bool bIsStoreChanged=KLPRSS::GetStoreChanged(pStorage);
            if(bIsStoreNew || bIsStoreChanged)
            {
                const wchar_t* path[]=
                {
                    KLPRSS_PRODUCT_CORE,
                    KLPRSS_VERSION_INDEPENDENT,
                    KLPRSS_VERSION_INFO,
                    NULL
                };
                KLERR_IGNORE(KLPRSS::SetStoreChanged(pStorage, false));
                KLERR_IGNORE(SetStoreNew(pStorage, false));
                pStore->sync_changed(path, false);
                pStore->sync_new(path, false);                
            };
            /*
            {
                CAutoPtr<Params> pType;
                KLPRSS_ExpandType(wstrType, &pType);
                PublishNotifications(lstChanged, false, wstrType, pType);
            };
            */
		KLERR_CATCH(pError)
			;KLSTD_TRACE1(3, L"StartFullSync - failure: \"%ls\", shutting down\n", pError->GetMsg());
			pSync->Shutdown();
			KLERR_RETHROW();
		KLERR_ENDTRY
        pStore->sync_unlock();// at first we save changes and only then allow nagent to save changes there
		SndSaveFlag(pSync, true);
		;KLSTD_TRACE0(3, L"StartFullSync - succeeded\n");
		KL_TMEASURE_END();
	};

	void KLCSKCA_DECL HandleFullSync(
		const std::wstring&	    wstrLocation,
		SYNC_TYPE			    nType,
		Synchronizer*		    pSync,		
		int					    nGroupSize,
		long				    lTimeout,
        const std::wstring&	    wstrType,
        const std::wstring&	    wstrServerId,
        const std::wstring&     wstrClientId)
	{
		;KLSTD_TRACE4(
			3,
			L"Handling full synchronization, wstrLocation=\"%ls\", nType=%d, nGroupSize=%d, lTimeout=%d\n",
			wstrLocation.c_str(),
			nType,
			nGroupSize,
			lTimeout);
		KLSTD_CHKINPTR(pSync);
        ;
        CAutoPtr<Params> pType;
        KLPRSS_ExpandType(wstrType, &pType);
        const bool bIsTask = (GetStringValue(pType, KLPRSS::c_szwSSP_Type) == c_szwSST_TasksSS);
        ;
        changed_t lstChanged;
        ;
		CAutoPtr<IStoreSync> pStore;
		KLERR_TRY
			headers_map_t rcvHeaders;
            bool bAllowSemiPacked = false;
			RcvHeaders(pSync, rcvHeaders, bAllowSemiPacked);
			;KLSTD_TRACE1(3, L"Headers received: %d\n", rcvHeaders.size());
			;
            ss_format_t fid;
            fid.id = SSF_ID_BINPAR;
			CreateStoreSync(wstrLocation, &pStore, &fid);
			
			pStore->sync_lock(lTimeout);

            //<--
            CAutoPtr<KLPRSS::Storage> pStorage;
            pStore->sync_get_store(&pStorage);

            std::wstring wstrLocalServerId, wstrLocalClientId;
            
            KLPRSS::GetStoreServerId(pStorage, wstrLocalServerId);
            const bool bIsNewServer=(wstrLocalServerId != wstrServerId);
            
            KLPRSS::GetStoreClientId(pStorage, wstrLocalClientId);
            const bool bIsNewClient=(wstrLocalClientId != wstrClientId);

            KLSTD_TRACE2(3, L"bIsNewServer =%u, bIsNewClient=%u\n", bIsNewServer, bIsNewClient);

			headers_map_t myHeaders;
			FillHeaders(pStore, myHeaders);
			KLSTD_TRACE1(3, L"Local headers read: %d\n", myHeaders.size());

			// form list of sections without contents
			sections_list_t sndData;
			// find sections to create on server
			;KLSTD_TRACE0(3, L"Making Sendlist\n");
			for(headers_map_t::iterator it1=myHeaders.begin(); it1!=myHeaders.end(); ++it1)
			{
				section_header_t& header=it1->second;
				secaddr_t sa(header.wstrProduct, header.wstrVersion, header.wstrSection);
				;
				section_data_t sndSec;
				sndSec.wstrProduct=header.wstrProduct;
				sndSec.wstrVersion=header.wstrVersion;
				sndSec.wstrSection=header.wstrSection;
				sndSec.nAction=SA_NULL;
				sndSec.bResult=true;

				if(rcvHeaders.find(sa)==rcvHeaders.end())//Секция имеется локально, но её нету удалённо.
                {                    
                    if(header.bNew || bIsNewServer || bIsNewClient || bIsTask)
                        sndSec.nAction=SA_CREATE_S;//Секция - новая. Её нужно создать на сервере
                    else
                    {
                        //KLSTD_ASSERT(false);
                        sndSec.nAction=SA_REMOVE_C;//Секция - старая. Её нужно удалить на клиенте
                    };
                };
				;
				if(sndSec.nAction!=SA_NULL)
                {
                    KLSTD_TRACE4(
					    3,
					    L"Handling section (\"%ls\"-\"%ls\"-\"%ls\") with action %d to Sendlist\n",
					    sndSec.wstrProduct.c_str(),
					    sndSec.wstrVersion.c_str(),
					    sndSec.wstrSection.c_str(),
					    sndSec.nAction);
                    sndData.push_back(sndSec);
                };
			};
			// find sections to destroy/modify on server
			for(headers_map_t::iterator it2=rcvHeaders.begin(); it2!=rcvHeaders.end(); ++it2)
			{
				section_header_t& header=it2->second;
				secaddr_t sa(header.wstrProduct, header.wstrVersion, header.wstrSection);

				section_data_t sndSec;
				sndSec.wstrProduct=header.wstrProduct;
				sndSec.wstrVersion=header.wstrVersion;
				sndSec.wstrSection=header.wstrSection;
				sndSec.nAction=SA_NULL;
				sndSec.bResult=true;

                headers_map_t::iterator it3=myHeaders.find(sa);				
                if(it3==myHeaders.end())// Если секция имеется удалённо, но её нету локально
                {
                    if(it2->second.bNew /* || bIsNewServer || bIsNewClient || bIsTask */)
                    {
                        sndSec.nAction=SA_CREATE_C;// Секция - новая, поэтому её локально и нет
                    }
                    else
                    {
                        //KLSTD_ASSERT(false);
                        sndSec.nAction=SA_REMOVE_S;
                    };
                }
				else
                {
					const bool bChangedThere = it2->second.bChanged;
					const bool bChangedHere = it3->second.bChanged;
					const bool bCollision = bChangedThere && bChangedHere;
                    if(KLPRSS_VERSION_INFO == sndSec.wstrSection)
                    {
                        sndSec.nAction=SA_NULL;
                    }
                    else if( bIsNewServer || bIsNewClient )
                    {
                        sndSec.nAction = SA_MERGE_C2S;
                    }
                    else if(bChangedThere || bChangedHere)
                    {
                        sndSec.nAction = (bCollision || bChangedThere) ? SA_MERGE_S2C : SA_MERGE_C2S;
                    };
				};
                if(sndSec.nAction!=SA_NULL)
                {
				    KLSTD_TRACE4(
					    3,
					    L"Adding section (\"%ls\"-\"%ls\"-\"%ls\") with action %d to Sendlist\n",
					    sndSec.wstrProduct.c_str(),
					    sndSec.wstrVersion.c_str(),
					    sndSec.wstrSection.c_str(),
					    sndSec.nAction);
				    sndData.push_back(sndSec);
                };
			};

			KLSTD_TRACE1(3, L"Sendlist contains %d entries\n", sndData.size());

            //if(!sndData.size())
                //SndSections(pSync, sections_list_t(), true);
			do
			{			
				sections_list_t dataToSend;
				for(;;)
				{
					sections_list_t::iterator it=sndData.begin();
					if(it==sndData.end() || (int)dataToSend.size() >= nGroupSize)
						break;
					section_data_t& sndSec=*it;
					switch(sndSec.nAction)
					{
					case SA_CREATE_S:
					case SA_MERGE_C2S:
						{
							const wchar_t* path[]=
							{
								sndSec.wstrProduct.empty() ? NULL : sndSec.wstrProduct.c_str(),
								sndSec.wstrVersion.empty() ? NULL : sndSec.wstrVersion.c_str(),
								sndSec.wstrSection.empty() ? NULL : sndSec.wstrSection.c_str(),
								NULL
							};
							pStore->sync_changed(path, false);
                            pStore->sync_new(path, false);
							pStore->sync_read2(
                                        path, 
                                        &sndSec.pContents, 
                                        bAllowSemiPacked
                                            ?   SYNC_SSF_FORCE_SEMIPACKED
                                            :   SYNC_SSF_COMPATIBLE);

						};
						break;
					case SA_REMOVE_S: // in fact section sndSec doesn't exist
					case SA_NULL:
                    case SA_CREATE_C:
                    case SA_MERGE_S2C:
                        break;
                    case SA_REMOVE_C:
                        DoCommand(pStore, sndSec, true, lstChanged);
                        break;
					default:
						KLSTD_ASSERT(false);
					};
                    if(sndSec.nAction != SA_REMOVE_C)
					    dataToSend.push_back(sndSec);
					sndData.erase(it);
				};
				KLSTD_TRACE2(3, L"Sending %d sections from Sendlist; EOF=%d\n", dataToSend.size(), sndData.size()==0);
				SndSections(pSync, dataToSend, sndData.size()==0);

				sections_list_t rcvData;
				bool bEOF=false;
                bool bAllowSemiPacked = false;//not used
				RcvSections(pSync, rcvData, bEOF, bAllowSemiPacked);
				;KLSTD_TRACE2(3, L"Recieved sections: %d; EOF=%d\n", rcvData.size(), bEOF);
				KLSTD_ASSERT(  bEOF==(sndData.size()==0)  );			
				for(sections_list_t::iterator it=rcvData.begin(); it!=rcvData.end(); ++it)
				{
					section_data_t& rcvSec=*it;
					;KLSTD_TRACE4(
						3,
						L"Updating section (\"%ls\"-\"%ls\"-\"%ls\") with action %d\n",
						rcvSec.wstrProduct.c_str(),
						rcvSec.wstrVersion.c_str(),
						rcvSec.wstrSection.c_str(),
						rcvSec.nAction);
					DoCommand(pStore, rcvSec, true, lstChanged);
				};
			}while(sndData.size());
            //-->
            bool bIsSsChanged=GetStoreChanged(pStorage);
            bool bIsStoreNew=GetStoreNew(pStorage);
            if(bIsStoreNew)
            {
                KLERR_IGNORE(SetStoreNew(pStorage, false));
            };
            if(bIsNewServer)
            {
                KLERR_IGNORE(SetStoreServerId(pStorage, wstrServerId));
            };
            if(bIsNewClient)
            {
                KLERR_IGNORE(SetStoreClientId(pStorage, wstrClientId));
            };
            if(bIsSsChanged)
            {
                KLERR_IGNORE(SetStoreChanged(pStorage, false));
            };
            
            if(bIsStoreNew || bIsNewServer || bIsNewClient || bIsSsChanged)
            {
                const wchar_t* path[]=
                {
                    KLPRSS_PRODUCT_CORE,
                    KLPRSS_VERSION_INDEPENDENT,
                    KLPRSS_VERSION_INFO,
                    NULL
                };                
                pStore->sync_changed(path, false);
                pStore->sync_new(path, false);
            };
            //-->
            PublishNotifications(lstChanged, true, wstrType, pType);
		KLERR_CATCH(pError)
			KLSTD_TRACE1(3, L"Handling full synchronization - failure: \"%ls\", shutting down\n", pError->GetMsg());
			pSync->Shutdown();			
			KLERR_RETHROW();
		KLERR_ENDTRY
		if(RcvSaveFlag(pSync) && pStore)
			pStore->sync_unlock();
		KLSTD_TRACE0(3, L"Handling full synchronization succeeded\n");
	};
};
