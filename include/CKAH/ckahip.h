#ifndef __CKAHIP_H__
#define __CKAHIP_H__

#include "ckahdefs.h"
#include "ckahumdefs.h"

#pragma warning(disable : 4200) // non-standard MS extension

#define CKAHUMMETHOD_ CKAHUMEXPORT
#define CKAHUMMETHOD CKAHUMMETHOD_ CKAHUM::OpResult 

// общие функции работы с CKAH
namespace CKAHUM
{
    inline ULONG IPv4Mask(unsigned int prefixlength) { return 0xffffffff << (32 - prefixlength); }

    inline unsigned int IPv4GetMaskPrefixLength(ULONG mask)
    {
        unsigned int width = 0;
        for(; !(mask & 1) && width < 32; mask >>= 1, width++); 
        return 32 - width; 
    }

#pragma pack( push, 1)
    struct OWord
    {
        union
        {
            struct {
                __int64 Lo;
                __int64 Hi;
            };
            unsigned long  Dwords[4];
            unsigned short Words[8];
            unsigned char  Bytes[16];
        };

        bool IsValid() const { return Lo != 0 || Hi != 0; }
        void SetZero() { Lo = Hi = 0; }

        bool operator == (const OWord& that) const { return Hi != that.Hi ? false : Lo == that.Lo; };
        bool operator != (const OWord& that) const { return Hi == that.Hi ? false : Lo != that.Lo; };

        bool operator <  (const OWord& that) const { return Hi >  that.Hi ? false : Hi != that.Hi ? false : Lo <  that.Lo; };
        bool operator >  (const OWord& that) const { return Hi <  that.Hi ? false : Hi != that.Hi ? false : Lo >  that.Lo; };
        bool operator <= (const OWord& that) const { return Hi >  that.Hi ? false : Hi != that.Hi ? false : Lo <= that.Lo; };
        bool operator >= (const OWord& that) const { return Hi <  that.Hi ? false : Hi != that.Hi ? false : Lo >= that.Lo; };

        OWord operator & (const OWord& that) const 
        { 
            OWord oword; 
            oword.Lo = Lo & that.Lo; 
            oword.Hi = Hi & that.Hi; 
            return oword; 
        }

        OWord ntoh() const
        {
            OWord n;
            unsigned char Byte7 = Bytes[7];
            unsigned char Byte6 = Bytes[6];
            unsigned char Byte5 = Bytes[5];
            unsigned char Byte4 = Bytes[4];
            unsigned char Byte3 = Bytes[3];
            unsigned char Byte2 = Bytes[2];
            unsigned char Byte1 = Bytes[1];
            unsigned char Byte0 = Bytes[0];
            n.Bytes[0]  = Bytes[15];
            n.Bytes[1]  = Bytes[14];
            n.Bytes[2]  = Bytes[13];
            n.Bytes[3]  = Bytes[12];
            n.Bytes[4]  = Bytes[11];
            n.Bytes[5]  = Bytes[10];
            n.Bytes[6]  = Bytes[9];
            n.Bytes[7]  = Bytes[8];
            n.Bytes[8]  = Byte7;
            n.Bytes[9]  = Byte6;
            n.Bytes[10] = Byte5;
            n.Bytes[11] = Byte4;
            n.Bytes[12] = Byte3;
            n.Bytes[13] = Byte2;
            n.Bytes[14] = Byte1;
            n.Bytes[15] = Byte0;
            return n;
        }
        OWord hton() const { return ntoh(); }

        unsigned int GetMaskPrefixLength() const
        {
            unsigned int width = 0;
            for(; !(Dwords[width/32] & (1 << width%32)) && width < 128; width++);
            return 128 - width;
        }

        void SetMask(unsigned int prefixlength)
        {
            if (prefixlength > 64)
                Hi = 0xffffffffffffffff,
                Lo = 0xffffffffffffffff << (128 - prefixlength);
            else
                Hi = 0xffffffffffffffff << (64 - prefixlength),
                Lo = 0x0;
        }
    };

    struct IPv6
    {
        enum Scope
        {
            ScopeInterface    = 0x01,
            ScopeLink         = 0x02,
            ScopeAdmin        = 0x04,
            ScopeSite         = 0x05,
            ScopeOrganization = 0x08,
            ScopeGlobal       = 0x0e,

            MaxScopes         = 0x10 // maximal number of scopes
        };

        OWord        O;
        unsigned int Zone;

        void Set(const OWord& oword, unsigned int zone) { memcpy(&O, &oword, sizeof(O)); Zone = zone; }

        bool IsValid() const { return O.IsValid(); }
        void SetZero()       { O.SetZero(); Zone = 0; }

        bool operator == (const IPv6& that) const { return Zone != that.Zone ? false : O == that.O; };
        bool operator != (const IPv6& that) const { return Zone == that.Zone ? false : O != that.O; };

        bool operator <  (const IPv6& that) const { return Zone != that.Zone ? false : O <  that.O; };
        bool operator >  (const IPv6& that) const { return Zone != that.Zone ? false : O >  that.O; };
        bool operator <= (const IPv6& that) const { return Zone != that.Zone ? false : O <= that.O; };
        bool operator >= (const IPv6& that) const { return Zone != that.Zone ? false : O >= that.O; };

        IPv6 operator & (const IPv6& that) const 
        {
            IPv6 ipv6;
            ipv6.O = O & that.O;
            ipv6.Zone = Zone;
            return ipv6;
        }

        Scope GetScope() const 
        {
            if      ((O.Dwords[3] & 0xff800000) == 0xfe800000) // link-local unicast
                return ScopeLink;
            else if ((O.Dwords[3] & 0xff000000) == 0xff000000) //multicast
                return (Scope)((O.Dwords[3] & 0x000f0000) >> 16);
            else
                return ScopeGlobal;
        }

        unsigned int GetMaskPrefixLength() const
        {
            return O.GetMaskPrefixLength();
        }

        void SetMask(unsigned int prefixlength, unsigned int zone)
        {
            O.SetMask(prefixlength);
            Zone = zone;
        }
    };

    struct IP
    {
        int Version;

        union {
            ULONG   v4;
            IPv6    v6;
        };

		IP() : Version(0) {}
        IP(const IP& that) { memcpy(this, &that, sizeof(that)); }

        IP(ULONG ipv4) : Version(4), v4(ipv4) {}
        IP(const IPv6& ipv6) : Version(6), v6(ipv6) {}

        IP& operator = (const IP& that) { memcpy(this, &that, sizeof(that)); return *this; }

        void Setv4 (ULONG ipv4)                            { Version = 4; v4 = ipv4; }
        void Setv6 (const IPv6& ipv6)                      { Version = 6; v6 = ipv6; }
        void Setv6 (const OWord& oword, unsigned int zone) { Version = 6; v6.Set(oword, zone); } 

        bool Isv4() const { return Version == 4; }
        bool Isv6() const { return Version == 6; }

        bool IsValid() const { return Isv4 () ? v4 != 0 :
                                      Isv6 () ? v6.IsValid() : false; }

        bool operator == (const IP& that) const { return Isv4() ? v4 == that.v4 : 
                                                         Isv6() ? v6 == that.v6 : false; }
        bool operator != (const IP& that) const { return Isv4() ? v4 != that.v4 : 
                                                         Isv6() ? v6 != that.v6 : false; }

        bool operator <  (const IP& that) const { return Isv4() ? v4 <  that.v4 : 
                                                         Isv6() ? v6 <  that.v6 : false; }
        bool operator >  (const IP& that) const { return Isv4() ? v4 >  that.v4 : 
                                                         Isv6() ? v6 >  that.v6 : false; }
        bool operator <= (const IP& that) const { return Isv4() ? v4 <= that.v4 : 
                                                         Isv6() ? v6 <= that.v6 : false; }
        bool operator >= (const IP& that) const { return Isv4() ? v4 >= that.v4 : 
                                                         Isv6() ? v6 >= that.v6 : false; }

        unsigned int GetMaskPrefixLength() const
        { 
            if      (Isv4()) return IPv4GetMaskPrefixLength(v4);
            else if (Isv6()) return v6.GetMaskPrefixLength();
            else             return 0;
        }

        void Setv4Mask(unsigned int prefixlength)                    { Version = 4; v4 = IPv4Mask(prefixlength); }
        void Setv6Mask(unsigned int prefixlength, unsigned int zone) { Version = 6; v6.SetMask(prefixlength, zone); }

    };
#pragma pack( pop )

}; //namespace CKAHUM

#endif