#ifndef __IPTOA_H__
#define __IPTOA_H__

#include <ProductCore/structs/s_ip.h>
#include <stdio.h>

#define CIP_IPTOA_MAXSTRLEN 48

inline const char *cIPToA(const cIP& ip)
{
    static char str[CIP_IPTOA_MAXSTRLEN];
    int size = sizeof(str);
    ip.ToStr(str, sizeof(str));
    return str;
}

inline const char *cIPMaskToA(const cIP& mask)
{
    static char str[CIP_IPTOA_MAXSTRLEN];

    if (mask.IsV4())
        mask.ToStr(str, sizeof(str));
    else if (mask.IsV6())
        sprintf(str, "%d", mask.GetMaskPrefixLength());
    else
        *str = '\0';

    return str;
}

#define CIP_IPTOA(ip) (strcpy((char*)_alloca(CIP_IPTOA_MAXSTRLEN+1),cIPToA(ip)))
#define CIP_IPMASKTOA(ip) (strcpy((char*)_alloca(CIP_IPTOA_MAXSTRLEN+1),cIPMaskToA(mask)))

#endif __IPTOA_H__

