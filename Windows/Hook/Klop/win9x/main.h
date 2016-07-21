#if !defined(AFX_WIN9XPRECOMP_H__00268CE3_6ECB_441F_A0A7_58ECEF2C9591__INCLUDED_)
#define AFX_WIN9XPRECOMP_H__00268CE3_6ECB_441F_A0A7_58ECEF2C9591__INCLUDED_

#define USE_NDIS

extern "C" {
    #include <vtoolsc.h>       
}

#define KLOP_Major			1
#define KLOP_Minor			0
#define KLOP_DeviceID		UNDEFINED_DEVICE_ID
#define KLOP_Init_Order		VXDLDR_INIT_ORDER + 4

#include "..\g_dispatcher.h"



#endif // !defined(AFX_WIN9XPRECOMP_H__00268CE3_6ECB_441F_A0A7_58ECEF2C9591__INCLUDED_)
