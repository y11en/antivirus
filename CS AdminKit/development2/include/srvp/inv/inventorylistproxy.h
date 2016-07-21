/*!
 * (C) 2007 Kaspersky Lab
 * 
 * \file	inventorylistproxy.h
 * \author	Eugene Rogozhnikov
 * \date	09.09.2007 15:58:16
 * \brief	
 * 
 */

#ifndef __INVENTORYLISTPROXY_H__
#define __INVENTORYLISTPROXY_H__

#include <std/base/klbase.h>
#include <srvp/csp/klcsp.h>

namespace KLINV
{
	//\brief Список параметров возвращаемый функциями работы с итератором элементов списка
	const wchar_t c_szwDb_Id[]						= L"nId";
	const wchar_t c_szwDb_StrID[]					= L"strID";
	const wchar_t c_szwDb_IsMsi[]					= L"bIsMsi";
	const wchar_t c_szwDb_DisplayName[]				= L"wstrDisplayName";
	const wchar_t c_szwDb_DisplayVersion[]			= L"wstrDisplayVersion";
	const wchar_t c_szwDb_Publisher[]				= L"wstrPublisher";
	const wchar_t c_szwDb_Comments[]				= L"wstrComments";
	const wchar_t c_szwDb_HelpLink[]				= L"wstrHelpLink";
	const wchar_t c_szwDb_HelpTelephone[]			= L"wstrHelpTelephone";
	const wchar_t c_szwDb_HostCount[]				= L"nHostCount";
	const wchar_t c_szwDb_InstallDate[]				= L"tmInstallDate";
	const wchar_t c_szwDb_InstallDir[]				= L"wstrInstallDir";

	//!\brief Массив элементов списка
	const wchar_t ITEMS_ARRAY[]				= L"KLINV_ITEMS_RANGE_ARRAY";
	
	
	class KLSTD_NOVTABLE InventoryListProxy : public KLSTD::KLBaseQI 
	{
    public:
		
		virtual void ResetIterator (
			const std::wstring& wstrFilter,
			const std::vector<std::wstring>& vect_fields,
			const std::vector<KLCSP::field_order_t>& vect_fields_to_order,
			long  lifetime,
			std::wstring& wstrIteratorId ) = 0;

        virtual long GetRecordCount( const std::wstring& wstrIteratorId )  = 0;

        virtual void GetRecordRange(
            const std::wstring& wstrIteratorId,
            long nStart,
            long nEnd,
            KLPAR::Params** pParamsEvents ) = 0;

		virtual void ReleaseIterator( const std::wstring& wstrIteratorId ) = 0;

	};
}


#endif //__INVENTORYLISTPROXY_H__