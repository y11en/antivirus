#ifndef _structs_s_ip_
#define _structs_s_ip_

#include <ipstr.h>
#include <Prague/pr_serializable.h>
#include <Prague/pr_vector.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

enum _eIP_IID
{
    _eiid_ip = 0xa0, // the numbers 0-0x9f are already used in s_settings.h and s_trmod.h
    _eiid_ipv6,
	_eiid_ipv6_mask,
};

//-----------------------------------------------------------------------------
//! IPv4 address

typedef tDWORD tIPv4;
#define tid_IPv4        tid_DWORD

inline tIPv4 tIPv4Mask(tDWORD prefixlength) { return 0xffffffff << (32 - prefixlength); }

inline tDWORD tIPv4GetMaskPrefixLength(tIPv4 mask)
{
    unsigned int width = 0;
    for(; !(mask & 1) && width < 32; mask >>= 1, width++);
    return 32 - width; 
}

inline tSIZE_T tIPv4ToStr(tIPv4 ipv4, tCHAR *str, tSIZE_T size)
{
    return IPStr::IPv4ToStr(ipv4, str, size);
}

inline bool tIPv4FromStr(tIPv4& ipv4, const tCHAR *str)
{
    return IPStr::IPv4FromStr(str, &ipv4);
}

//-----------------------------------------------------------------------------
//! IPv6 address

enum _eIPv6Scope
{
    _ipv6Scope_Interface    = 0x01,
    _ipv6Scope_Link         = 0x02,
    _ipv6Scope_Admin        = 0x04,
    _ipv6Scope_Site         = 0x05,
    _ipv6Scope_Organization = 0x08,
    _ipv6Scope_Global       = 0x0e,

    _ipv6Scope_Max          = 0x10,
};

struct cIPv6 : public cSerializable
{
    cIPv6() : cSerializable(),
        m_Lo(0),
        m_Hi(0),
        m_Zone(0)
    {}

    /*
    cIPv6(tDWORD Dword0, tDWORD Dword1, tDWORD Dword2, tDWORD Dword3) : cSerializable()
    {
        m_Dwords[0] = Dword0;
        m_Dwords[1] = Dword1;
        m_Dwords[2] = Dword2;
        m_Dwords[3] = Dword3;
    }
    */

    DECLARE_IID(cIPv6, cSerializable, PID_ANY, _eiid_ipv6);

    union
    {
        struct {
            tQWORD m_Lo;
            tQWORD m_Hi;
        };
		tQWORD  m_Qwords[2];
        tDWORD  m_Dwords[4];
        tWORD   m_Words[8];
        tBYTE   m_Bytes[16];
    };
    tDWORD  m_Zone;

    bool IsValid() const { return m_Hi != 0 || m_Lo != 0; }
    void SetZero() { m_Hi = m_Lo = 0; m_Zone = 0; }

    bool operator == (const cIPv6& that) const { return m_Zone != that.m_Zone ? false : m_Hi != that.m_Hi ? false : m_Lo == that.m_Lo; };
    bool operator != (const cIPv6& that) const { return m_Zone != that.m_Zone ? true  : m_Hi != that.m_Hi ? true  : m_Lo != that.m_Lo; };

    bool operator <  (const cIPv6& that) const { return m_Zone != that.m_Zone ? false : m_Hi >  that.m_Hi ? false : m_Hi != that.m_Hi ? false : m_Lo < that.m_Lo;  };
    bool operator >  (const cIPv6& that) const { return m_Zone != that.m_Zone ? false : m_Hi <  that.m_Hi ? false : m_Hi != that.m_Hi ? false : m_Lo > that.m_Lo;  };
    bool operator <= (const cIPv6& that) const { return m_Zone != that.m_Zone ? false : m_Hi >  that.m_Hi ? false : m_Hi != that.m_Hi ? false : m_Lo <= that.m_Lo; };
    bool operator >= (const cIPv6& that) const { return m_Zone != that.m_Zone ? false : m_Hi <  that.m_Hi ? false : m_Hi != that.m_Hi ? false : m_Lo >= that.m_Lo; };

    cIPv6 operator & (const cIPv6& that) const 
    { 
        cIPv6 ipv6; 
        ipv6.m_Lo = m_Lo & that.m_Lo; 
        ipv6.m_Hi = m_Hi & that.m_Hi; 
        ipv6.m_Zone = m_Zone;
        return ipv6; 
    }

    tDWORD GetMaskPrefixLength() const
    {
        tDWORD nWidth = 0;
        for(; !(m_Dwords[nWidth/32] & (1 << nWidth%32)) && nWidth < 128; nWidth++);
        return 128 - nWidth;
    }

    void SetMask(tDWORD prefixlength, tDWORD zone)
    {
        if (prefixlength > 64)
            m_Hi = 0xffffffffffffffffULL,
            m_Lo = 0xffffffffffffffffULL << (128 - prefixlength);
        else
            m_Hi = 0xffffffffffffffffULL << (64 - prefixlength),
            m_Lo = 0x0;
        m_Zone = zone;
    }

    _eIPv6Scope GetScope() const 
    {
        if      ((m_Dwords[3] & 0xff800000) == 0xfe800000) // link-local unicast
            return _ipv6Scope_Link;
        else if ((m_Dwords[3] & 0xff000000) == 0xff000000) //multicast
            return (_eIPv6Scope)((m_Dwords[3] & 0x000f0000) >> 16); 
        else
            return _ipv6Scope_Global;
    }

    bool SetScopePrefix(_eIPv6Scope scope, tBOOL multicast, tDWORD zone)
    {
        if (multicast)
        {
            m_Hi   = 0xff00000000000000ULL | ((tQWORD)scope << 48);
            m_Lo   = 0;
            m_Zone = zone;
            return true;
        }
        else if (scope == _ipv6Scope_Link)
        {
            m_Hi   = 0xfe80000000000000ULL;
            m_Lo   = 0;
            m_Zone = zone;
            return true;
        }
        else
            return false;
    }
    void ntoh6()
	{
		tBYTE n;
		for(tDWORD x=0;x<8;x++)
		{
			n=m_Bytes[x];
			m_Bytes[x]=m_Bytes[15-x];
			m_Bytes[15-x]=n;
		}
    }
    tSIZE_T ToStr(tCHAR *str, tSIZE_T size) const 
    { 
        return IPStr::IPv6ToStr(m_Bytes, m_Zone, str, size); 
    }
    bool FromStr(const tCHAR *str) 
    { 
        return IPStr::IPv6FromStr(str, m_Bytes, &m_Zone); 
    }
};

IMPLEMENT_SERIALIZABLE_BEGIN(cIPv6, "IPv6")
    SER_VALUE_M(Hi,       tid_QWORD)
    SER_VALUE_M(Lo,       tid_QWORD)
    SER_VALUE_M(Zone,     tid_DWORD)
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------
//! IP address (IPv4/IPv6)

struct cIP : public cSerializable
{
    cIP() : cSerializable(),
        m_Version(0),
        m_V4(0)
    {}

    explicit cIP(tDWORD ipv4) : cSerializable(),
        m_Version(4),
        m_V4(ipv4)
    {}

    cIP(const cIPv6& ipv6) : cSerializable(),
        m_Version(6),
        m_V6(ipv6)
    {}

    /*
    cIP(tDWORD Dword0, tDWORD Dword1, tDWORD Dword2, tDWORD Dword3) : cSerializable(),
        m_Version(6),
        m_V6(Dword0, Dword1, Dword2, Dword3)
    {}
    */

    DECLARE_IID(cIP, cSerializable, PID_ANY, _eiid_ip);

    tBYTE			m_Version;		// версия IP протокола (4, 6)
    tIPv4			m_V4;			// IPv4-адрес
    cIPv6			m_V6;		    // IPv6-адрес

    bool IsV4() const { return m_Version == 4; }
    bool IsV6() const { return m_Version == 6; }

	void SetV4(tIPv4 ipv4)          { m_Version = 4; m_V4 = ipv4; m_V6.m_Hi=0;m_V6.m_Lo=0;m_V6.m_Zone=0;}
    void SetV6(const cIPv6& ipv6)   { m_Version = 6; m_V6 = ipv6; m_V4=0;}

    bool IsValid () const { return IsV4() ? m_V4 != 0 &&  m_V4 !=0xffffffff: 
                                   IsV6() ? m_V6.IsValid() : false; }

    bool operator == (const cIP& that) const { return IsV4() && that.IsV4() ? m_V4 == that.m_V4 : 
                                                      IsV6() && that.IsV6() ? m_V6 == that.m_V6 : m_Version == that.m_Version; }
    bool operator != (const cIP& that) const { return IsV4() && that.IsV4() ? m_V4 != that.m_V4 : 
                                                      IsV6() && that.IsV6() ? m_V6 != that.m_V6 : m_Version != that.m_Version; }

    bool operator <  (const cIP& that) const { return IsV4() && that.IsV4() ? m_V4 <  that.m_V4 : 
                                                      IsV6() && that.IsV6() ? m_V6 <  that.m_V6 : false; }
    bool operator >  (const cIP& that) const { return IsV4() && that.IsV4() ? m_V4 >  that.m_V4 : 
                                                      IsV6() && that.IsV6() ? m_V6 >  that.m_V6 : false; }
    bool operator <= (const cIP& that) const { return IsV4() && that.IsV4() ? m_V4 <= that.m_V4 : 
                                                      IsV6() && that.IsV6() ? m_V6 <= that.m_V6 : false; }
    bool operator >= (const cIP& that) const { return IsV4() && that.IsV4() ? m_V4 >= that.m_V4 : 
                                                      IsV6() && that.IsV6() ? m_V6 >= that.m_V6 : false; }

    cIP operator & (const cIP& that) const 
    { 
        cIP ip;
        if      (IsV4 () && that.IsV4()) ip.SetV4 (m_V4 & that.m_V4);
        else if (IsV6 () && that.IsV6()) ip.SetV6 (m_V6 & that.m_V6);
        return ip; 
    }

    tDWORD GetMaskPrefixLength() const
    { 
        if      (IsV4()) return tIPv4GetMaskPrefixLength(m_V4);
        else if (IsV6()) return m_V6.GetMaskPrefixLength();
        else             return 0;
    }
	void SetV6Bytes(tBYTE *in_v6)
	{
		m_Version = 6;
		memcpy(m_V6.m_Bytes,in_v6,16);
	}
    void SetV4Mask(tDWORD prefixlength)              { m_Version = 4; m_V4 = tIPv4Mask(prefixlength); }
    void SetV6Mask(tDWORD prefixlength, tDWORD zone) { m_Version = 6; m_V6.SetMask(prefixlength, zone); }

    void SetV6ScopePrefix(_eIPv6Scope scope, bool multicast, tDWORD zone) 
    { 
        m_Version = 6; 
        m_V6.SetScopePrefix(scope, multicast, zone);
    }

    tSIZE_T ToStr(tCHAR *str, tSIZE_T size) const
    {
        if      (IsV4()) return tIPv4ToStr(m_V4, str, size);
        else if (IsV6()) return m_V6.ToStr(str, size);
        else             return 0;
    }

    bool FromStr(const tCHAR *str)
    {
        if (tIPv4FromStr(m_V4, str)) { m_Version = 4; return true; }
        if (m_V6.FromStr(str))       { m_Version = 6; return true; }

        return false;
    }

    bool FromStrV4(const tCHAR *str)
    {
        if (tIPv4FromStr(m_V4, str)) { m_Version = 4; return true; }
        return false;
    }

    bool FromStrV6(const tCHAR *str)
    {
        if (m_V6.FromStr(str))       { m_Version = 6; return true; }
        return false;
    }

};

IMPLEMENT_SERIALIZABLE_BEGIN(cIP, "IP")
    SER_VALUE_M(Version,		tid_BYTE)
    SER_VALUE_M(V4,             tid_IPv4)
    SER_VALUE_M(V6,             cIPv6::eIID)
IMPLEMENT_SERIALIZABLE_END()

typedef cVector<cIP> cIPs;


//-----------------------------------------------------------------------------
// cIpMask

struct cIpMask : public cIP
{
	cIpMask() : m_Mask(0) {}
	
	DECLARE_IID(cIpMask, cIP, PID_ANY, _eiid_ipv6_mask);

	bool IsMatch(const cIP &IP) const
	{
		if( IsV4() && IP.IsV4() )
		{
			tIPv4 mask = 0xffffffff << m_Mask;
			return (m_V4 & mask) == (IP.m_V4 & mask);
		}
		
		if( IsV6() && IP.IsV6() )
		{
			tQWORD mask[2];
			if( m_Mask > 64 )
				mask[0] = 0, mask[1] = 0xffffffffffffffffULL << (m_Mask - 64);
			else
				mask[1] = 0, mask[0] = 0xffffffffffffffffULL << m_Mask;

			return ((*(tQWORD *)&m_V6.m_Bytes[0] & mask[0]) == (*(tQWORD *)&IP.m_V6.m_Bytes[0] & mask[0])) &&
				((*(tQWORD *)&m_V6.m_Bytes[8] & mask[1]) == (*(tQWORD *)&IP.m_V6.m_Bytes[8] & mask[1]));
		}
		
		return false;
	}

	tSIZE_T ToStr(tCHAR *str, tSIZE_T size) const
	{
		tSIZE_T res = cIP::ToStr(str, size);
		if( m_Mask )
			if( size -= res )
				res += sprintf_s(str + res - 1, size, "/%u", m_Mask);
		return res;
	}

	bool FromStr(const tCHAR *str)
	{
		if( const tCHAR *slash = strrchr(str, '/') )
		{
			tDWORD mask;
			if( !IPStr::sscan_num(slash + 1, &mask) || mask >= 128 )
				return false;

			tSIZE_T ipLen = slash - str;
			tCHAR *ip = (tCHAR *)alloca(ipLen + 1);
			strncpy_s(ip, ipLen + 1, str, ipLen);

			cIP tmp;
			if( !tmp.FromStr(ip) )
				return false;

			if( tmp.m_Version == 4 && mask >= 32 )
				return false;
			
			*(cIP *)this = tmp;
			m_Mask = (tBYTE)mask;
			return true;
		}
		return cIP::FromStr(str);
	}
	tIPv4 GetV4Mask()
	{
		return 0xffffffff << m_Mask;
	}
	void GetMask(cIP *mask)
	{
		if(m_Version==4)
			GetV4Mask(mask);
		else
			GetV6Mask(mask);
	}
	void GetNet(cIP *net)
	{
		if(m_Version==4)
			GetV4Net(net);
		else
			GetV6Net(net);
	}
	tIPv4 GetV4Mask(cIP *mask) { mask->SetV4(GetV4Mask()); return mask->m_V4; }
	tIPv4 GetV4Net()           { return GetV4Mask() & m_V4; }
	tIPv4 GetV4Net(cIP *net)
	{
		net->SetV4(GetV4Mask() & m_V4);
		return net->m_V4;
	}
	void GetV6Mask(cIP * mask)
	{
		mask->clear();
		tDWORD index=1;
		for(tDWORD x=0;x<128;x++)
		{
			mask->m_V6.m_Qwords[index]<<=1;
			if(m_Mask>x)
				mask->m_V6.m_Qwords[index]++;
			if(x==63)
				index--;
		}
		mask->m_Version=6;
	}
	void GetV6Net(cIP * net)
	{
		GetV6Mask(net);
		net->m_V6.m_Hi&=m_V6.m_Hi;
		net->m_V6.m_Lo&=m_V6.m_Lo;
		net->m_Version=6;
	}
	void SetV4MaskByIp( tIPv4 ip)
	{
		m_Mask=tIPv4GetMaskPrefixLength(ip);
	}
	tBOOL CmpNet(cIpMask *ip)
	{
		if(ip->IsV4() && IsV4())
			return GetV4Net()==ip->GetV4Net();
		return false;
	}
	tBYTE m_Mask;
};

IMPLEMENT_SERIALIZABLE_BEGIN(cIpMask, 0)
	SER_BASE( cIP, 0 )
	SER_VALUE_M(Mask,           tid_BYTE)
IMPLEMENT_SERIALIZABLE_END()

typedef cVector<cIpMask> cIpMasks;


#endif //_structs_s_ip_
