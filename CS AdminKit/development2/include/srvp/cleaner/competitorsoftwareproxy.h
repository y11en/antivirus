/*!
 * (C) 2006 Kaspersky Lab
 * 
 * \file	competitorsoftwareproxy.h
 * \author	Eugene Rogozhnikov
 * \date	23.05.2006 17:49:52
 * \brief	Прокси интерфейса для доступа к cleaner
 * 
 */

#ifndef __COMPETITORSOFTWAREPROXY_H__
#define __COMPETITORSOFTWAREPROXY_H__

#include <std/base/klbase.h>
#include <srvp/csp/klcsp.h>
//#include <common/cleanercommon.h>

namespace KLCLEANER
{
	class KLSTD_NOVTABLE CompetitorSoftwareProxy : public KLSTD::KLBaseQI 
	{
    public:
        virtual void GetCompetitorSoftwareList( std::vector<std::wstring>& vectSoftwares )  = 0;
		virtual void GetHostCompetitorSoftwareList( long lHostId, std::vector<std::wstring>& vectSoftwares )  = 0;
		//virtual void GetSrvIniFileInfoList( std::vector<CIniFileInfo>& vectInfo, const std::wstring& wstrType ) = 0;
	};
}

#endif //__COMPETITORSOFTWAREPROXY_H__