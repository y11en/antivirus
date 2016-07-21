/*!
 * (C) 2002 Kaspersky Lab 
 * 
 * \file	ss_sync.h
 * \author	Andrew Kazachkov
 * \date	06.11.2002 13:56:17
 * \brief	INTERNAL
 * 
 */

#ifndef __SYNC_H__DC7F7113_B2AC_4738_8366_6221F93E0621
#define __SYNC_H__DC7F7113_B2AC_4738_8366_6221F93E0621


#include <map>
#include <list>
#include "kca/prss/settingsstorage.h"
#include "kca/prss/store.h"

namespace KLPRSS {

//	const wchar_t c_szwSS_SETTINGS[]=L"SS_SETTINGS";
//	const wchar_t c_szwSS_PRODINFO[]=L"SS_PRODINFO";


	class secaddr_t
	{
	public:
        secaddr_t(){;};
		secaddr_t(	const std::wstring& wstrProduct,
					const std::wstring& wstrVersion,
					const std::wstring& wstrSection) :
			m_wstrProduct(wstrProduct),
			m_wstrVersion(wstrVersion),
			m_wstrSection(wstrSection)
		{
			;
		};
		secaddr_t(const secaddr_t& x) :
			m_wstrProduct(x.m_wstrProduct),
			m_wstrVersion(x.m_wstrVersion),
			m_wstrSection(x.m_wstrSection)
		{
			;
		};

		int compare(const secaddr_t& x) const 
		{
			int result;
			result=wcscmp(m_wstrProduct.c_str(), x.m_wstrProduct.c_str());
			if(result!=0)
				return result;
			result=wcscmp(m_wstrVersion.c_str(), x.m_wstrVersion.c_str());
			if(result!=0)
				return result;
			return wcscmp(m_wstrSection.c_str(), x.m_wstrSection.c_str());
		};

		bool operator == (const secaddr_t& x) const
		{
			return compare(x)==0;
		};

		bool operator < (const secaddr_t& x) const
		{
			return compare(x) < 0;
		};

		bool operator > (const secaddr_t& x) const
		{
			return compare(x) > 0;
		};

		std::wstring	m_wstrProduct;
		std::wstring	m_wstrVersion;
		std::wstring	m_wstrSection;
	};

	struct section_header_t
	{
		std::wstring	wstrProduct;
		std::wstring	wstrVersion;
		std::wstring	wstrSection;
		bool			bChanged;
        bool            bNew;
		;
		bool			bSuccess;
	};

	typedef std::map<secaddr_t, section_header_t> headers_map_t;
	typedef std::list<section_header_t> headers_list_t;

	enum SEC_ACTION
	{
		SA_NULL,		
		SA_CREATE_S,
        SA_CREATE_C,
		SA_REMOVE_S,
        SA_REMOVE_C,
		SA_MERGE_S2C,
        SA_MERGE_C2S,
	};

	struct section_data_t
	{
		std::wstring					wstrProduct;
		std::wstring					wstrVersion;
		std::wstring					wstrSection;
		KLSTD::CAutoPtr<KLPAR::Params>	pContents;
		SEC_ACTION						nAction;
		;
		bool							bResult;
	};

//	typedef std::map<secaddr_t, section_data_t> sections_t;
	typedef std::list<section_data_t> sections_list_t;

	enum SYNC_TYPE
	{
		ST_RO,	// product registration
		ST_RW	// settings
	};

	class Synchronizer : public KLSTD::KLBase
	{
	public:
		virtual void SndParams(KLPAR::Params* pParams)=0;
		virtual void RcvParams(KLPAR::Params** ppParams)=0;
		KLSTD_NOTHROW virtual void Shutdown() throw() =0;
		KLSTD_NOTHROW virtual void Fail()=0;
        KLSTD_NOTHROW virtual void Close() = 0;
	};

	// On server side
	void KLCSKCA_DECL StartFullSync(
		const std::wstring&	wstrLocation,
		SYNC_TYPE			nType,
		Synchronizer*		pSync,
		int					nGroupSize,
		long				lTimeout,
        const std::wstring&	wstrType);

	//	On agent side
	void KLCSKCA_DECL HandleFullSync(
		const std::wstring&	wstrLocation,
		SYNC_TYPE			nType,
		Synchronizer*		pSync,		
		int					nGroupSize,
		long				lTimeout,
        const std::wstring&	wstrType,
        const std::wstring&	wstrServerId,
        const std::wstring&	wstrClientId);
};
#endif //__SYNC_H__DC7F7113_B2AC_4738_8366_6221F93E0621
