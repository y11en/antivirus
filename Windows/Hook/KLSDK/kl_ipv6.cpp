#include "kl_ipv6.h"

CIPUNI::CIPUNI()
{    
    RtlZeroMemory( this, sizeof ( ip_addr_t ) );
}

CIPUNI::CIPUNI( const CIPUNI& ip )
{
    Zone = ip.Zone;
    IpVer = ip.IpVer;
    if ( IpVer == 4 )
        Ipv4 = ip.Ipv4;
    else if ( IpVer == 6 )
        RtlCopyMemory( Ipv6_ushort, ip.Ipv6_ushort, sizeof ( unsigned short ) * 8 );
    else
        RtlZeroMemory( this, sizeof ( ip_addr_t ) );

}

CIPUNI::CIPUNI( const ip_addr_t& ip )
{
    Zone = ip.Zone;
    IpVer = ip.IpVer;
    if ( IpVer == 4 )
        Ipv4 = ip.Ipv4;
    else if ( IpVer == 6 )
        RtlCopyMemory( Ipv6_ushort, ip.Ipv6_ushort, sizeof ( unsigned short ) * 8 );
    else 
        RtlZeroMemory( this, sizeof ( ip_addr_t ) );
}

CIPUNI::CIPUNI(unsigned long ip)
{
    IpVer = 4;
    Ipv4 = ip;
    Zone = 0;
}

CIPUNI::CIPUNI(unsigned short* ip, unsigned long zone)
{
    Zone = zone;
    IpVer = 6;
    RtlCopyMemory( Ipv6_ushort, ip, sizeof ( unsigned short ) * 8 );
}

CIPUNI&
CIPUNI::Assign(PTRANSPORT_ADDRESS Addr )
{
    if ( Addr->Address[0].AddressType == TDI_ADDRESS_TYPE_IP6 )
    {
        TA_IP6_ADDRESS_XP* ipv6 = (TA_IP6_ADDRESS_XP*)Addr;
        IpVer = 6;
        Zone = ipv6->Address[0].Address[0].sin6_scope_id;
        RtlCopyMemory( Ipv6_ushort, ipv6->Address[0].Address[0].sin6_addr, sizeof ( unsigned short ) * 8 );
    }
    else
    {
        TA_IP_ADDRESS* ipv4 = (TA_IP_ADDRESS*)Addr;
        IpVer = 4;
        Ipv4 = ipv4->Address[0].Address[0].in_addr;
        Zone = 0;
    }

    return *this;
}

CIPUNI&
CIPUNI::Assign(unsigned short* ip, unsigned long zone )
{
    Zone = zone;
    IpVer = 6;
    RtlCopyMemory( Ipv6_ushort, ip, sizeof ( unsigned short ) * 8 );

    return *this;
}

bool
CIPUNI::operator == (const CIPUNI& ip) const
{
    if ( IpVer == ip.IpVer )
    {
        if ( IpVer == 4 )
            return Ipv4 == ip.Ipv4;

        if ( Zone == ip.Zone )
            return KlCompareMemory( (PVOID)Ipv6_ushort, (PVOID)ip.Ipv6_ushort, sizeof ( USHORT ) * 8 );
    }
    return false;
}

bool
CIPUNI::operator == (const unsigned long ip) const
{
    if ( IpVer == 4 )
        return Ipv4 == ip;

    return false;
}

bool
CIPUNI::operator == (const unsigned short* ip) const
{
    if ( IpVer == 6 )
        return KlCompareMemory( (PVOID)Ipv6_ushort, (PVOID)ip, sizeof ( USHORT ) * 8 );

    return false;
}


bool
CIPUNI::operator > (const CIPUNI& ip) const
{
    if ( IpVer == ip.IpVer )
    {
        if ( IpVer == 4 )
            return Ipv4 > ip.Ipv4;

        if ( Ipv6_Hi == ip.Ipv6_Hi )
            return Ipv6_Lo > ip.Ipv6_Lo;

        return Ipv6_Hi > ip.Ipv6_Hi;
    }
    return false;
}

bool
CIPUNI::operator <= (const CIPUNI& ip) const
{
    return !( *this > ip );
}

bool
CIPUNI::operator < (const CIPUNI& ip) const
{
    if ( IpVer == ip.IpVer )
    {
        if ( IpVer == 4 )
            return Ipv4 < ip.Ipv4;

        if ( Ipv6_Hi == ip.Ipv6_Hi )
            return Ipv6_Lo < ip.Ipv6_Lo;

        return Ipv6_Hi < ip.Ipv6_Hi;
    }
    return false;
}

bool
CIPUNI::operator >= (const CIPUNI& ip) const
{
    return !( *this < ip );
}

CIPUNI
CIPUNI::operator & ( const CIPUNI& ip ) const
{
    CIPUNI ResIP;

    if ( IpVer == ip.IpVer )
    {
        ResIP.IpVer = IpVer;

        if ( IpVer == 4 )        
            ResIP.Ipv4 = Ipv4 & ip.Ipv4;

        else if ( IpVer == 6 )
        {
            ResIP.Ipv6_Hi = Ipv6_Hi & ip.Ipv6_Hi;
            ResIP.Ipv6_Lo = Ipv6_Lo & ip.Ipv6_Lo;
            ResIP.Zone = ip.Zone;
        }
    }

    return ResIP;
}

CIPUNI&
CIPUNI::UNI_Hton()
{
    if ( IpVer == 4 )
        Ipv4 = htonl( Ipv4 );
    else
    if ( IpVer == 6 )
    {
        ULONG i32_0 = htonl(Ipv6_i32[3]);
        ULONG i32_1 = htonl(Ipv6_i32[2]);
        Ipv6_i32[2] = htonl(Ipv6_i32[1]);
        Ipv6_i32[3] = htonl(Ipv6_i32[0]);

        Ipv6_i32[0] = i32_0;
        Ipv6_i32[1] = i32_1;
    }

    return *this;
}

CIPUNI&
CIPUNI::UNI_Ntoh()
{
    return UNI_Hton();
}

UCHAR localhost_v6[] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1 };


bool
CIPUNI::isLocalhost()
{
    if ( IpVer == 4 )    
        return Ipv4 == localhost;

    if ( IpVer == 6 )    
        return KlCompareMemory( Ipv6_uchar, localhost_v6, sizeof ( UCHAR ) * 16 );

    return false;
}

bool
CIPUNI::Zero()
{
    return IpVer == 0;
}

void
CIPUNI::SetLocalhost()
{
    IpVer = 4;
    Ipv4 = localhost;
}

void
CIPUNI::SetLocalhost_v6()
{
    IpVer = 6;
    RtlZeroMemory( Ipv6_ushort, sizeof( USHORT ) * 8 );
    Ipv6_uchar[15] = 1;
}