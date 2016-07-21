#ifndef __CKAHIPTOA_H__
#define __CKAHIPTOA_H__

#include "ckahip.h"
#include <stdio.h>
#include <ipstr.h>

#define CKAHUM_IPTOA_MAXSTRLEN 48

namespace CKAHUM
{

inline const char *IPToA(const CKAHUM::IP& ip)
{
    static char str[CKAHUM_IPTOA_MAXSTRLEN];

    if      (ip.Isv4()) IPStr::IPv4ToStr(ip.v4, str, sizeof(str));
    else if (ip.Isv6()) IPStr::IPv6ToStr(ip.v6.O.Bytes, ip.v6.Zone, str, sizeof(str));
    else                sprintf(str, "unknown IP version");

    return str;
}

inline const char *IPMaskToA(const CKAHUM::IP& mask)
{
    static char str[CKAHUM_IPTOA_MAXSTRLEN];

    if      (mask.Isv4()) IPStr::IPv4ToStr(mask.v4, str, sizeof(str));
    else if (mask.Isv6()) sprintf(str, "%d", mask.GetMaskPrefixLength());
    else                  sprintf(str, "unknown IP version");

    return str;
}

}

#define CKAHUM_IPTOA(ip) (strcpy((char*)_alloca(CKAHUM_IPTOA_MAXSTRLEN+1),CKAHUM::IPToA(ip)))
#define CKAHUM_IPMASKTOA(mask) (strcpy((char*)_alloca(CKAHUM_IPTOA_MAXSTRLEN+1),CKAHUM::IPMaskToA(mask)))

#endif __CKAHIPTOA_H__

