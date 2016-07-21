#ifndef _KL_IPADDR_H_
#define _KL_IPADDR_H_

#pragma pack(push, 1)
typedef struct _ip_addr_t {
    unsigned char   IpVer;
    union { 

        unsigned long   Ipv4;
        unsigned char   Ipv4_b[4];        

        unsigned long   Ipv6_i32[4];
        unsigned short  Ipv6_ushort[8];
        unsigned char   Ipv6_uchar[16];

        struct {
                __int64 Ipv6_Hi;
                __int64 Ipv6_Lo;
        };        
    };
    unsigned long       Zone;   // ipv6 field only
} ip_addr_t, *pip_addr_t;
#pragma pack(pop)

#endif //_KL_IPADDR_H_