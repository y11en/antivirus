#include <stdio.h>
#include "logformat.h"
#include "ned.h"
#include "ipstr.h"

inline unsigned short _ntohs(unsigned short a) { return ((a & 0xFF00) >> 8) | ((a & 0x00FF) << 8); }

inline unsigned int _ntohl(unsigned int a) 
{ 
    return ((a & 0xFF000000) >> 24) | 
           ((a & 0x00FF0000) >>  8) | 
           ((a & 0x0000FF00) <<  8) | 
           ((a & 0x000000FF) << 24); }

inline IPStr::IPv6 _ntoho(const IPStr::IPv6& i)
{
    IPStr::IPv6 o;
    o.bytes[15] = i.bytes[0];
    o.bytes[14] = i.bytes[1];
    o.bytes[13] = i.bytes[2];
    o.bytes[12] = i.bytes[3];
    o.bytes[11] = i.bytes[4];
    o.bytes[10] = i.bytes[5];
    o.bytes[9]  = i.bytes[6];
    o.bytes[8]  = i.bytes[7];
    o.bytes[7]  = i.bytes[8];
    o.bytes[6]  = i.bytes[9];
    o.bytes[5]  = i.bytes[10];
    o.bytes[4]  = i.bytes[11];
    o.bytes[3]  = i.bytes[12];
    o.bytes[2]  = i.bytes[13];
    o.bytes[1]  = i.bytes[14];
    o.bytes[0]  = i.bytes[15];
    return o;
}

void OutIP(bool incoming, char *hdr, unsigned char protocol, int fragoffs, char *sp)
{
   switch(protocol)
   {
   case IP_PROTO_TCP:   
     {
       if (fragoffs == 0)
       {
          TCP_HEADER *tcph = (TCP_HEADER*)hdr;

          short locport = incoming ? tcph->dstPort : tcph->srcPort;
          short remport = incoming ? tcph->srcPort : tcph->dstPort;

          sprintf(sp, "TCP  %5d %s %-5d %c%c%c%c%c%c%c%c", 
            _ntohs(locport), incoming ? "<-" : "->", _ntohs(remport),
            tcph->fin?'f':'-',
            tcph->syn?'s':'-',
            tcph->rst?'r':'-',
            tcph->psh?'p':'-',
            tcph->ack?'a':'-',
            tcph->urg?'u':'-',
            tcph->ece?'e':'-',
            tcph->cwr?'c':'-'
          );     
       }
       else
          sprintf(sp, "TCP                         ");     
       break;
     }
   case IP_PROTO_UDP:
     {
       if (fragoffs == 0)
       {
          UDP_HEADER *udph = (UDP_HEADER*)hdr;

          short locport = incoming ? udph->dstPort : udph->srcPort;
          short remport = incoming ? udph->srcPort : udph->dstPort;

          sprintf(sp, "UDP  %5d %s %-5d         ", 
            _ntohs(locport), incoming ? "<-" : "->", _ntohs(remport)
          );     
       }
       else
          sprintf(sp, "UDP                         ");
       break;
     }
   case IP_PROTO_ICMP:    strcpy(sp, "ICMP                        "); break;
   case IP_PROTO_ICMPV6:  strcpy(sp, "ICMP6                       "); break;
   case 2:  strcpy(sp, "IGMP                        "); break;
   default:             sprintf(sp, "%5d                       ", protocol); break;
   }
}

bool IPv6_GetNextHdrOffset( char* CurHdr, unsigned char CurHdrType, unsigned int* pOffset, unsigned char* pNextType )
{
    if ( CurHdrType == 0  ||
        CurHdrType == 43 ||
        CurHdrType == 60 ) 
    {
        *pNextType  = CurHdr[0];
        *pOffset    = ( CurHdr[1] + 1 ) * 8 ;

        return true;
    }

    if ( CurHdrType == 51 ) // ðºªõýªø¯øúð¡ø  
    {
        *pNextType  = CurHdr[0];
        *pOffset    = ( CurHdr[1] + 2 ) * 4;
    }

    if ( CurHdrType == 44 ) // ¯¨ðóüõýªð¡ø 
    {
        *pNextType  = CurHdr[0];
        *pOffset    = 8;
        return true;
    }

    return false;
}

bool IPv6_HasNextHeader( unsigned char HeaderType )
{
    switch ( HeaderType )
    {
    case 0:
    case 43:
    case 44:
    case 51:
    case 60:
        return true;
    }

    return false;
}


void OutPacket(bool incoming, unsigned long size, char *packet)
{
   char locaddr[48] = "";
   char remaddr[48] = "";

   char ethproto[5] = "";
   char sp[256] = "";
   char fr[256] = "";

   ETH_HEADER *ethh = (ETH_HEADER *)packet;

   switch(_ntohs(ethh->Type))
   {
   case ETH_P_IP:
       {
           IP_HEADER *iph = (IP_HEADER *)(packet + ETH_HEADER_LENGTH);

           bool morefrag = !!(iph->Flg_FragOffs & 0x0020 );
           bool dontfrag = !!(iph->Flg_FragOffs & 0x0020 );
           int fragoffs = _ntohs ( iph->Flg_FragOffs & 0xFF1F ) << 3;

           IPStr::IPv4ToStr(_ntohl(incoming ? iph->dstIP : iph->srcIP), locaddr, sizeof(locaddr));
           IPStr::IPv4ToStr(_ntohl(incoming ? iph->srcIP : iph->dstIP), remaddr, sizeof(remaddr));

           if (morefrag || fragoffs)
           {
              sprintf(fr, " %04x %04x", _ntohs(iph->Identification), fragoffs);
           }

           char *nexthdr = (packet + ETH_HEADER_LENGTH + iph->Ihl*sizeof(DWORD));
           unsigned char protocol = iph->Protocol;

           OutIP(incoming, nexthdr, protocol, fragoffs, sp); 
       }
       break;
   case ETH_P_IPV6:
       {
           IPV6_HEADER *ipv6h = (IPV6_HEADER *)(packet + ETH_HEADER_LENGTH);

           IPStr::IPv6 locipv6, remipv6;

           memcpy(locipv6.bytes, incoming ? ipv6h->DstIP : ipv6h->SrcIP, sizeof(locipv6));
           memcpy(remipv6.bytes, incoming ? ipv6h->SrcIP : ipv6h->DstIP, sizeof(remipv6));

           locipv6 = _ntoho(locipv6);
           remipv6 = _ntoho(remipv6);

           IPStr::IPv6ToStr(locipv6.bytes, 0, locaddr, sizeof(locaddr));
           IPStr::IPv6ToStr(remipv6.bytes, 0, remaddr, sizeof(remaddr));

           unsigned char proto = ipv6h->NextHeader;
           unsigned int  curoffs = sizeof (IPV6_HEADER);
           unsigned long totaloffs = curoffs;

           while ( IPv6_HasNextHeader(proto) )
           {        
               if ( !IPv6_GetNextHdrOffset( packet + ETH_HEADER_LENGTH + totaloffs, proto, &curoffs, &proto ) )
                    break;

               totaloffs += curoffs;
           }

           OutIP(incoming, packet + ETH_HEADER_LENGTH + totaloffs, proto, 0, sp);
       }
       break;
   case ETH_P_ARP:
       {
           strcpy(sp, "ARP                         ");

           sprintf(incoming ? locaddr : remaddr, "%02X%02X%02X%02X%02X%02X", ethh->dstMac[0],ethh->dstMac[1],ethh->dstMac[2],ethh->dstMac[3],ethh->dstMac[4],ethh->dstMac[5]);
           sprintf(incoming ? remaddr : locaddr, "%02X%02X%02X%02X%02X%02X", ethh->srcMac[0],ethh->srcMac[1],ethh->srcMac[2],ethh->srcMac[3],ethh->srcMac[4],ethh->srcMac[5]);
       }
       break;
   }
   printf("%5d %-15s %s %-15s %s%s\n", size,
          locaddr, incoming ? "<-" : "->", remaddr,
          sp, fr);
}

void main(int argc, char *argv[])
{
   if (argc < 2)
   {
     printf("LogBarzer for kldump logs  Version 1.1.006\n"
            "Copyright (c)2006-2007, Kaspersky Lab. All rights reserved.\n\n"

            "Usage: logbarzer <logfile>\n");
     return;
   }

   FILE * fi = fopen(argv[1],"rb");
   if (!fi) 
   {
     printf("can't open %s\n", argv[1]);
     return;
   }

   KLDUMP_LOG_HEADER h;
   if (fread(&h, 1, sizeof(h), fi) != sizeof(h) || h.sig != KLDUMP_LOG_SIG)
   {
     fclose(fi);
     return;     
   }

   printf(
"size  local addr         remote addr     prot  locp    remp  flags    id   offs\n"
"-------------------------------------------------------------------------------\n"
);

   while(!feof(fi))
   {
     KLDUMP_LOG_PKT_HEADER ph;
     if (fread(&ph, 1, sizeof(ph), fi) != sizeof(ph)) break;

     if (ph.PacketSize > 32768) break;

     char p[32768];     
     if (fread(&p, 1, ph.PacketSize, fi) != ph.PacketSize) break;

     OutPacket(ph.isIncoming ? true : false, ph.PacketSize, p);
   }

   fclose(fi);
}