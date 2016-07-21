/* $Id: nix_types.h,v 1.3 2005/01/05 12:51:34 alexiv Exp $
 * */

#ifndef NIX_TYPES_H
#define NIX_TYPES_H

#ifdef _WIN32
#include <stddef.h>

typedef unsigned short uint16_t;
typedef unsigned int u_int32_t;
typedef int int32_t;
typedef unsigned short u_int16_t;
typedef unsigned char u_int8_t;
typedef unsigned int u_int;
typedef unsigned long u_long;
typedef unsigned char u_char;
typedef unsigned long in_addr_t;
typedef long off_t;
typedef char* caddr_t;
typedef int pid_t;
typedef int mode_t;
typedef volatile int spinlock_t;

// function redefinitions for win32
#define strncasecmp _strnicmp
#define	bzero(a, b)		memset(a, 0, b)
#define random	rand

#ifndef EAFNOSUPPORT
#define	EAFNOSUPPORT	97	/* not present in errno.h provided with VC */
#endif


#define NI_NOFQDN       0x00000001
#define NI_NUMERICHOST  0x00000002
#define NI_NAMEREQD     0x00000004
#define NI_NUMERICSERV  0x00000008
#define NI_DGRAM        0x00000010
#define NI_WITHSCOPEID  0x00000020      

 
#ifdef __cplusplus
extern "C" {
#endif

const char* inet_ntop(int af, const void *src, char *dst, size_t size);
int inet_pton(int af, const char *src, void *dst);
int inet_aton(const char *cp, struct in_addr *addr);

#ifdef __cplusplus
}
#endif


#endif // WIN32


#endif // NIX_TYPES_H

