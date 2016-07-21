/*!
 * (C) 2002 Kaspersky Lab 
 * 
 * \file	ssync.h
 * \author	Andrew Kazachkov
 * \date	10.12.2002 10:39:30
 * \brief	
 * 
 */

#ifndef __SYNC_H__22fPAkZUhwKXREiRVQL4o2
#define __SYNC_H__22fPAkZUhwKXREiRVQL4o2

#include <std/base/klbase.h>
#include <std/trc/trace.h>
#include <std/par/paramsi.h>
#include <kca/prss/store.h>
#include <transport/tr/transport.h>
#include <transport/tr/transportproxy.h>
#include <transport/ev/eventsource.h>
#include <kca/prci/componentinstance.h>
#include <kca/prci/simple_params.h>
#include <kca/prcp/agentproxy.h>
#include <kca/prcp/internal.h>
#include <std/tmstg/timeoutstore.h>
//#include "./g_sync.h"

#include <list>
#include <map>

//#define SSSYNC_GROUPSYN L"CSYNC"

namespace CSYNC
{
    const wchar_t c_szwAllowSemiPacked[] = L"allow_semipacked"; //BOOL_T
    
	// Типы посылаемых при синхронизации данных
	enum sync_types_t
	{
		st_unknown,
		st_headers,
		st_sections,
		st_command
	};

	sync_types_t QueryType(KLPAR::Params* pData);
	void SetType(KLPAR::Params* pData, sync_types_t type);

	// Команды синхронизации
	typedef enum
	{
		sc_nop,
		sc_getheaders,
		sc_write_sections,
		sc_delete_sections,
		sc_eof,
		sc_finish,
		sc_failure,
		sc_last=sc_failure
	}sync_command_t;

	sync_command_t GetCommandId(KLPAR::Params* pData);
	void SetCommandId(KLPAR::Params* pData, sync_command_t id);

	//sc_getheaders
		const wchar_t c_szwSyn_GetHeadersFlags[]=L"csyn-getHeadersFlags";

	//sc_write_sections
		const wchar_t c_szwSyn_WriteSectionsFlags[]=L"csyn-writeSectionsFlags"; // INT_T
		enum{
			WSF_CLEAR_CHANGED=1
		};

	const wchar_t c_szwCommand[]=L"csyn-nCommand";// INT_T	
	const wchar_t c_szwHeaders[]=L"csyn-arrHeaders";	// ARRAY_T
	const wchar_t c_szwSections[]=L"csyn-arrSections";	// ARRAY_T
	const wchar_t c_szwType[]=L"csyn-nType";	//sync_types_t
	const wchar_t c_szwProduct[]=L"csyn-wstrProduct"; // STRING_T
	const wchar_t c_szwVersion[]=L"csyn-wstrVersion"; // STRING_T
	const wchar_t c_szwSection[]=L"csyn-wstrSection"; // STRING_T
	const wchar_t c_szwHash[]=L"csyn-wstrHash"; // STRING_T
	const wchar_t c_szwChanged[]=L"csyn-bChanged"; // BOOL_T
	const wchar_t c_szwContents[]=L"csyn-parContents"; // PARAMS_T

	void PrepareCommand(sync_command_t id, KLPAR::Params** ppData);

	struct sec_address_t
	{
		sec_address_t(){;}
		sec_address_t(const sec_address_t& x)
			:	m_wstrProduct(x.m_wstrProduct)
			,	m_wstrVersion(x.m_wstrVersion)
			,	m_wstrSection(x.m_wstrSection)
		{
			;
		};

		int compare(const sec_address_t& x) const
		{
			int result=m_wstrProduct.compare(x.m_wstrProduct);
			if(result)
				return result;
			result=m_wstrVersion.compare(x.m_wstrVersion);
			if(result)
				return result;
			result=m_wstrSection.compare(x.m_wstrSection);
			return result;
		};

		bool operator == (const sec_address_t& x) const
		{
			return	compare(x)==0;
		};

		bool operator < (const sec_address_t& x) const
		{
			return	compare(x) < 0;
		};

		std::wstring	m_wstrProduct;
		std::wstring	m_wstrVersion;
		std::wstring	m_wstrSection;
	};

	
	struct section_t
	{
		section_t(){;};
		section_t(const section_t& x)
			:	m_address(x.m_address)
			,	m_pContents(x.m_pContents)
		{
			;
		};

		sec_address_t			m_address;
		KLSTD::CAutoPtr<KLPAR::Params>	m_pContents;
	};

	typedef std::list<section_t>	sections_list_t;

	struct sec_header_t
	{
		sec_header_t()
			:	m_bChanged(false)
		{;};
		sec_header_t(const sec_header_t& x)
			:	m_address(x.m_address)
			,	m_wstrHash(x.m_wstrHash)
			,	m_bChanged(x.m_bChanged)
		{
			;
		};
		sec_address_t	m_address;
		std::wstring			m_wstrHash;
		bool					m_bChanged;
	};

	typedef std::list<sec_header_t>							headers_list_t;
	typedef std::map<sec_address_t, sec_header_t>	headers_map_t;

	typedef void (*EnumAllSectionsCallback)(void* pContext, sec_address_t& sddr);
	void EnumAllSections(KLPRSS::Storage* pStorage, void* pContext, EnumAllSectionsCallback pCallback);

	enum{
		MAHLF_HASH_REQUIRED=1,
		MAHLF_CHGD_REQUIRED=2,
		MAHLF_INCLUDE_UNCHANGED=4,
		MAHLF_INCLUDE_CHANGED=8,
		MAHLF_INCLUDE_ALL=MAHLF_INCLUDE_UNCHANGED|MAHLF_INCLUDE_CHANGED,
		MAHLF_EXCLUDE_CHANGED=MAHLF_INCLUDE_ALL&(~MAHLF_INCLUDE_CHANGED)
	};
	void MakeAllHeadersList(KLPRSS::Storage* pStorage, headers_list_t& headers, AVP_dword dwFlags);
	void PrepareHeaders(const headers_list_t& headers, KLPAR::Params** ppHeaders, AVP_dword dwFlags);
	void PrepareHeaders(const headers_map_t& headers, KLPAR::Params** ppHeaders, AVP_dword dwFlags);
	
	void PrepareSections(const sections_list_t& sections, KLPAR::Params** ppSections);
	void GetSections(KLPAR::Params* pSections, sections_list_t& sections);
	void GetHeaders(KLPAR::Params* pHeaders, headers_map_t& headers);
	void GetHeaders(KLPAR::Params* pHeaders, headers_list_t& headers);

	void ForceCreateEntry(
						KLPRSS::Storage*		pStorage,
						sec_address_t&	address);

};

#endif //__SYNC_H__22fPAkZUhwKXREiRVQL4o2

// Local Variables:
// mode: C++
// End:

