/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	nag_hostinfo.h
 * \author	Andrew Kazachkov
 * \date	09.11.2004 13:18:50
 * \brief	
 * 
 */

#ifndef __KLNAG_HOSTINFO_H__
#define __KLNAG_HOSTINFO_H__

#include <std/hstd/hostdomain.h>

namespace KLNAG
{
    void GetHostInfo_HostId(
            KLPAR::Params*  pSecServerData,
            std::wstring&   wstrHostId);

    void GetHostInfo_NagentId(
            KLPAR::Params*  pSettings,
            std::wstring&   wstrNagentId);
    
    void GetHostInfo_HostName(
            std::wstring&           wstrWinHostName,
            std::wstring&           wstrWinDomainName,
            KLSTD::KlDomainType&    nDomainType);
    
    KLSTD_NOTHROW std::wstring GetDnsName() throw();

    KLSTD_NOTHROW std::wstring GetDnsDomain() throw();
};

#endif //__KLNAG_HOSTINFO_H__
