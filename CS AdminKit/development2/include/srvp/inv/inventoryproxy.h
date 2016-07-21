/*!
 * (C) 2007 Kaspersky Lab
 * 
 * \file	inventoryproxy.h
 * \author	Eugene Rogozhnikov
 * \date	26.07.2007 17:31:17
 * \brief	Прокси интерфейса для доступа к inventory
 * 
 */

#ifndef __INVENTORYPROXY_H__
#define __INVENTORYPROXY_H__

#include <std/base/klbase.h>
#include <srvp/csp/klcsp.h>
#include <common/inventory.h>
#include <common/cleanercommon.h>

namespace KLINV
{
	class KLSTD_NOVTABLE InventoryProxy : public KLSTD::KLBaseQI 
	{
    public:
		virtual void GetInvProductsList( std::vector<KLINV::CInvProductInfo>& vecInvProducts ) = 0;
		virtual void GetInvPatchesList( std::vector<KLINV::CInvPatchInfo>& vecInvPatches ) = 0;
		virtual void GetHostInvProductsList( long lHostId, std::vector<KLINV::CInvProductInfo>& vecInvProducts )  = 0;
		virtual void GetHostInvPatchesList( long lHostId, std::vector<KLINV::CInvPatchInfo>& vecInvPatches )  = 0;
		virtual void GetSrvCompetitorIniFileInfoList( 
			std::vector<KLCLEANER::CIniFileInfo>& vectInfo, 
			const std::wstring& wstrType ) = 0;
	};
}; // namespace KLINV

#endif //__INVENTORYPROXY_H__