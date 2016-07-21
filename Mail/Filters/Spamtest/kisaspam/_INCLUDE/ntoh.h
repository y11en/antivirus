#ifndef __NTOH_H__
#define __NTOH_H__

#ifdef WIN32

#ifdef _WINSOCK2API_
  #error ntoh.h conflicts with winsock2.h
#endif
#ifdef _WINSOCKAPI_
  #error ntoh.h conflicts with winsock.h
#endif
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */

typedef unsigned char   u_char;
typedef unsigned short  u_short;
typedef unsigned int    u_int;
typedef unsigned long   u_long;

#define __byte_swap_long(x) (((x) << 24) | (((x) & 0xff00) << 8) | (((x) & 0xff0000) >> 8) | ((x) >> 24))
#define __byte_swap_short(x) ((u_short)(((u_short)(x) << 8) | ((u_short)(x) >> 8)))


#ifdef __cplusplus
  inline u_long htonl (u_long hostlong)     { return __byte_swap_long(hostlong); }
  inline u_long ntohl (u_long netlong)      { return __byte_swap_long(netlong); }
  inline u_short htons (u_short hostshort)  { return __byte_swap_short(hostshort); }
  inline u_short ntohs (u_short netshort)   { return __byte_swap_short(netshort); }
#else
  #define ntohl(x) __byte_swap_long(x)
  #define htonl(x) __byte_swap_long(x)
  #define ntohs(x) __byte_swap_short(x)
  #define htons(x) __byte_swap_short(x)
#endif

#endif /* WIN32 */

#endif /* __NTOH_H__ */
