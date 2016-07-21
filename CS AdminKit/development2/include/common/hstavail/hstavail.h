/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	hstavail.h
 * \author	Andrew Kazachkov
 * \date	04.10.2004 18:55:24
 * \brief	
 * 
 */

#ifndef __KLHSTAVAIL_H__
#define __KLHSTAVAIL_H__

#include <string>
#include <vector>

namespace KLHSTAVAIL
{
    void InitializeIcmp();

    void DeinitializeIcmp();
    
    void GetMacAddresses(std::vector<std::wstring>& vecAddresses);
    
    KLSTD_NOTHROW bool GetMacByIp(unsigned long ulIp, std::wstring& wstrMac) throw();
    
    int  SendIcmpPing(unsigned long ulIp, size_t nPingTimeout, size_t nPackets, size_t nSleep);    

    const size_t c_nMaxIcmpPings = 64;
    /*!
      \brief	Send pings to several nIps ip addresses.

      \param	pulIp           IN  array of ip addresses
      \param	nIps            IN  size of 
      \param	nPingTimeout    IN  timeout to wait
      \param	nPackets        IN  number of packetes to send
      \param	nSleep          IN  wait between sends
      \param	pResults        OUT array of size nIps
    */
    void  SendIcmpPings(
                unsigned long*  pulIp,
                size_t          nIps,
                size_t          nPingTimeout, 
                size_t          nPackets, 
                size_t          nSleep,
                int*            pResults);
}

#endif //__KLHSTAVAIL_H__
