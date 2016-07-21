#ifndef _UDP_
#define _UDP_
#include "..\StdAfx.h"


typedef struct udphdr {
	__u16	source;
	__u16	dest;
	__u16	len;
	__u16	check;
}udphdr;

DEF(UDP)

#endif	/* _UDP_ */
