/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	hostdomain.cpp
 * \author	Andrew Kazachkov
 * \date	31.03.2003 13:27:20
 * \brief	
 * 
 */

#ifndef KLSTD_HOSTDOMAIN_H
#define KLSTD_HOSTDOMAIN_H

#include <string>

#include "std/base/kldefs.h"

namespace KLSTD
{
	enum KlDomainType
	{
		KLDT_WIN_DOMAIN,	//< Windows NT домен
		KLDT_WIN_WORKGROUP  //< Рабочая группа Windows
	};
};

KLCSC_DECL void KLSTD_GetHostAndDomainName(
					std::wstring&			wstrHost,
					std::wstring*			pwstrDomain,
					KLSTD::KlDomainType*	pnDomainType);

#endif // KLSTD_HOSTDOMAIN_H
