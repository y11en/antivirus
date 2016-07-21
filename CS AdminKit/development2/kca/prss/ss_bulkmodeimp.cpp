/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	ss_bulkmodeimp.cpp
 * \author	Andrew Kazachkov
 * \date	27.10.2003 14:38:17
 * \brief	
 * 
 */


#include "std/base/klbase.h"
#include "kca/prss/settingsstorage.h"
#include "kca/prss/errors.h"
#include "kca/prss/store.h"
#include "std/par/paramsi.h"
#include "kca/prss/ss_bulkmode.h"
#include <set>
#include <list>
#include <map>

#include "kca/prss/helpersi.h"

#include "nameconvertor.h"
#include "std/base/klbaseqi_imp.h"
#include "./ss_bulkmodeimp.h"

using namespace std;
using namespace KLSTD;
using namespace KLPAR;
using namespace KLERR;
using namespace KLPRSS;

#define KLCS_MODULENAME L"KLPRSS"

namespace KLPRSS
{
    //SsBulkMode
    typedef std::map<std::wstring, int> wstr2int_t;

    class CWstring2Int
    {        
        wstr2int_t  m_mapData;
        int         m_lId;
    public:
        CWstring2Int()
            :   m_lId(0)
        {;};
        int Put(const std::wstring& wstrName);            
        int Size(){return m_lId;};
        wstr2int_t::iterator Begin(){return m_mapData.begin();};
        wstr2int_t::iterator End(){return m_mapData.end();};
    };

    int CWstring2Int::Put(const std::wstring& wstrName)
    {
        int nResult=0;
        wstr2int_t::iterator it=m_mapData.find(wstrName);
        if(it == m_mapData.end())
        {
            m_mapData.insert(wstr2int_t::value_type(wstrName, m_lId));
            nResult=m_lId;
            ++m_lId;
        }
        else
            nResult=it->second;
        return nResult;
    };

    CBulkMode::CBulkMode()
        :   m_plTimeout(NULL)
        ,   m_pOwner(NULL)
    {;};

    void CBulkMode::Initialize(
                KLSTD::KLBaseQI*    pOwner,
                CriticalSection*    pCS,	
		        Storage*			pStorage,
                long*				plTimeout)
    {
        m_pOwner=pOwner;
        m_pCS=pCS;	
        m_pStorage=pStorage;
        m_plTimeout=plTimeout;
    };

    void CBulkMode::EnumAllWSections(sections_t& sections)
    {
        EnumAllWSectionsI(sections, true);
    }

    void CBulkMode::EnumAllWSectionsI( sections_t& sections, bool bOwnTransaction)    
    {
        CWstring2Int objVersions, objSections;
        std::list<section_name_t> lstSections;
		{
			AutoCriticalSection cs(m_pCS);
            if(!m_pStorage)
                KLSTD_THROW(STDE_NOTPERM);
            
			KLERR_TRY
                if(bOwnTransaction)
				    m_pStorage->trans_begin(false, (*m_plTimeout));

                const wchar_t* pathProducts[]={NULL};
				m_pStorage->region_load(pathProducts, false);
				m_pStorage->entry_enumerate(sections.m_vecProducts);                    

                for(unsigned int nProduct=0; nProduct < sections.m_vecProducts.size(); ++nProduct)
                {
                    const wchar_t* pathVer[]=
                    {
                        sections.m_vecProducts[nProduct].c_str(),
                        NULL
                    };
                    
                    m_pStorage->region_load(pathVer, false);
                    std::vector<std::wstring> vecVersions;
                    m_pStorage->entry_enumerate(vecVersions);

                    for(unsigned int nVersion=0; nVersion < vecVersions.size(); ++nVersion)
                    {
                        const wchar_t* pathSec[]=
                        {
                            sections.m_vecProducts[nProduct].c_str(),
                            vecVersions[nVersion].c_str(),
                            NULL
                        };
                        int nVersionId=objVersions.Put(vecVersions[nVersion]);

                        m_pStorage->region_load(pathSec, false);
                        std::vector<std::wstring> vecSections;
                        m_pStorage->entry_enumerate(vecSections);

                        for(unsigned int nSection=0; nSection < vecSections.size(); ++nSection)
                        {
                            int nSectionId=objSections.Put(vecSections[nSection]);                                
                            std::list<section_name_t>::iterator it=lstSections.insert(lstSections.end());
                            (*it).m_dwFlags=0;
                            (*it).m_nProduct=nProduct;
                            (*it).m_nVersion=nVersionId;
                            (*it).m_nSection=nSectionId;
                        };
                    };
                };
                if(bOwnTransaction)
				    m_pStorage->trans_end(true);
			KLERR_CATCH(pError)                
                if(bOwnTransaction)
				    m_pStorage->trans_end(false);
                KLERR_SAY_FAILURE(3, pError);
                sections.m_vecNames.clear();
                sections.m_vecProducts.clear();
                sections.m_vecVersions.clear();
                sections.m_vecSections.clear();
				KLERR_RETHROW();
			KLERR_ENDTRY
		};
        sections.m_vecNames.clear();
        sections.m_vecNames.reserve(lstSections.size());            
        for(std::list<section_name_t>::iterator itNames=lstSections.begin(); itNames != lstSections.end(); ++itNames)
            sections.m_vecNames.push_back(*itNames);
        
        sections.m_vecVersions.clear();
        sections.m_vecVersions.resize(objVersions.Size());
        for(wstr2int_t::iterator itVersion=objVersions.Begin(); itVersion != objVersions.End(); ++itVersion)
            sections.m_vecVersions[itVersion->second]=itVersion->first;

        sections.m_vecSections.clear();
        sections.m_vecSections.resize(objSections.Size());
        for(wstr2int_t::iterator itSection=objSections.Begin(); itSection != objSections.End(); ++itSection)
            sections.m_vecSections[itSection->second]=itSection->first;
    };

    void CBulkMode::ReadSections(
            const sections_t&   sections,
            KLPAR::ArrayValue** ppData)
    {
        KLSTD_CHKOUTPTR(ppData);
        const size_t nSize = sections.m_vecNames.size();
        ;
        KLSTD::CAutoPtr<KLPAR::ValuesFactory> pFactory;
        KLPAR_CreateValuesFactory(&pFactory);
        ;
        KLSTD::CAutoPtr<KLPAR::ArrayValue> pData;
        pFactory->CreateArrayValue(&pData);
        pData->SetSize(nSize);
        if(nSize)
        {
            m_pStorage->trans_begin(false, (*m_plTimeout));
            KLERR_TRY
                for(size_t i=0; i < nSize; ++i)
                {
                KLERR_BEGIN
                    //Creating path to section
                    /*
		            const wchar_t* path0[]={NULL};
		            KLPRSS_MAKE_PATH1(
			            path1, 
			            sections.m_vecProducts[sections.m_vecNames[i].m_nProduct].c_str());
		            KLPRSS_MAKE_PATH2(
			            path2, 
			            sections.m_vecProducts[sections.m_vecNames[i].m_nProduct].c_str(),
			            sections.m_vecVersions[sections.m_vecNames[i].m_nVersion].c_str());
                    */
		            KLPRSS_MAKE_PATH3(
			            path3, 
			            sections.m_vecProducts[sections.m_vecNames[i].m_nProduct].c_str(),
			            sections.m_vecVersions[sections.m_vecNames[i].m_nVersion].c_str(),
                        sections.m_vecSections[sections.m_vecNames[i].m_nSection].c_str());

		            //m_pStorage->region_load(path0, false);
		            //m_pStorage->region_load(path1, false);
		            //m_pStorage->region_load(path2, false);
                    m_pStorage->region_load(path3, false);
                    //reading data
                    KLSTD::CAutoPtr<KLPAR::Params> pSection;
                    m_pStorage->entry_read(&pSection);
                    KLSTD::CAutoPtr<KLPAR::ParamsValue> pItem;
                    CreateValue(pSection, &pItem);
                    pData->SetAt(i, pItem);
                KLERR_ENDT(3)
                };
                m_pStorage->trans_end(true);
            KLERR_CATCH(pError)
                m_pStorage->trans_end(false);
            KLERR_ENDTRY
        };
        pData.CopyTo(ppData);
    };

    void CBulkMode::WriteSections(
            const sections_t&   sections,
            KLPAR::ArrayValue*  pData)
    {
        KLSTD_CHKINPTR(pData);
        const int nSize=pData->GetSize();
        if(!nSize || !sections.m_vecNames.size())
            return;
        KLSTD_ASSERT_THROW((unsigned)nSize == sections.m_vecNames.size());
        m_pStorage->trans_begin(true, (*m_plTimeout));
        KLERR_TRY
            for(int i=0; i < nSize; ++i)
            {
                //Creating section
		        const wchar_t* path0[]={NULL};
		        KLPRSS_MAKE_PATH1(
			        path1, 
			        sections.m_vecProducts[sections.m_vecNames[i].m_nProduct].c_str());
		        KLPRSS_MAKE_PATH2(
			        path2, 
			        sections.m_vecProducts[sections.m_vecNames[i].m_nProduct].c_str(),
			        sections.m_vecVersions[sections.m_vecNames[i].m_nVersion].c_str());

		        KLPRSS_MAKE_PATH3(
			        path3, 
			        sections.m_vecProducts[sections.m_vecNames[i].m_nProduct].c_str(),
			        sections.m_vecVersions[sections.m_vecNames[i].m_nVersion].c_str(),
                    sections.m_vecSections[sections.m_vecNames[i].m_nSection].c_str());

		        m_pStorage->region_load(path0, true);
		        m_pStorage->entry_create(path1[0], false);
		        
		        m_pStorage->region_load(path1, true);
		        m_pStorage->entry_create(path2[1], false);
		        
		        m_pStorage->region_load(path2, true);
		        m_pStorage->entry_create(path3[2], false);
                m_pStorage->region_load(path3, true);
                //writing data
                CAutoPtr<ParamsValue> pValue;
                pData->GetAt(i, (Value**)&pValue);
                if(!pValue)
                    continue;
                KLPAR_CHKTYPE(pValue, PARAMS_T, L"");
                KLSTD::CAutoPtr<KLPAR::Params> pSection=pValue->GetValue();
                if(!pSection)
                    continue;
                m_pStorage->entry_write(sections.m_vecNames[i].m_dwFlags, pSection);
            };
            //@todo use method OnSaving instead
            if(m_pStorage->get_changed())
            {
                WriteStoreID(m_pStorage, KLSTD_CreateLocallyUniqueString());
                SetStoreChanged(m_pStorage, true);
            };
            m_pStorage->trans_end(true);
        KLERR_CATCH(pError)
            m_pStorage->trans_end(false);
        KLERR_ENDTRY
    };
    
};
