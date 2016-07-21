#ifndef _KL_IPV6_HELPER_H__
#define _KL_IPV6_HELPER_H__

#include "kldef.h"
#include "kl_ipaddr.h"
#include "tdi.h"

struct CIPUNI : public ip_addr_t {    

    CIPUNI();
    CIPUNI( const CIPUNI& ip );
    CIPUNI( unsigned long ip );
    CIPUNI( unsigned short* ip, unsigned long zone );
    CIPUNI( const ip_addr_t& ip );

    CIPUNI& Assign( PTRANSPORT_ADDRESS Addr );
    CIPUNI& Assign( unsigned short* ip, unsigned long zone );

    bool operator == ( const CIPUNI& ip ) const;
    bool operator == ( const unsigned long ip ) const;
    bool operator == ( const unsigned short* ip ) const;

    bool operator > ( const CIPUNI& ip ) const;
    bool operator < ( const CIPUNI& ip ) const;

    bool operator >= ( const CIPUNI& ip ) const;
    bool operator <= ( const CIPUNI& ip ) const;

    CIPUNI operator & ( const CIPUNI& ip ) const;

    CIPUNI& UNI_Hton();
    CIPUNI& UNI_Ntoh();

    bool    isLocalhost();
    bool    Zero();

    void    SetLocalhost()    ;
    void    SetLocalhost_v6();
};

#endif