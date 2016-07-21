#ifndef _ckah_ipconv_h_
#define _ckah_ipconv_h_

#include <CKAH/ckahip.h>
#include <ProductCore/structs/s_ip.h>

inline CKAHUM::IPv6 cIPv6_CKAHUMIPv6(const cIPv6& cipv6)
{
    CKAHUM::IPv6 ckahumipv6;
    ckahumipv6.Set(*(CKAHUM::OWord*)cipv6.m_Bytes, cipv6.m_Zone);
    return ckahumipv6;
}

inline CKAHUM::IP cIP_CKAHUMIP(const cIP& cip) 
{
    CKAHUM::IP ckahumip; 

    if (cip.IsV4())             ckahumip.Setv4(cip.m_V4); 
    else if (cip.IsV6())        ckahumip.Setv6(cIPv6_CKAHUMIPv6(cip.m_V6));

    return ckahumip;
}

inline cIPv6 CKAHUMIPv6_cIPv6(const CKAHUM::IPv6& ckahumipv6)
{
    cIPv6 cipv6;
    cipv6.m_Lo   = ckahumipv6.O.Lo;
    cipv6.m_Hi   = ckahumipv6.O.Hi;
    cipv6.m_Zone = ckahumipv6.Zone;
    return cipv6;
}

inline cIP CKAHUMIP_cIP(const CKAHUM::IP& ckahumip) 
{
    cIP cip;

    if (ckahumip.Isv4())        cip.SetV4(ckahumip.v4);
    else if (ckahumip.Isv6())   cip.SetV6(CKAHUMIPv6_cIPv6(ckahumip.v6));

    return cip;
}

#endif //_ckah_ipconv_h_