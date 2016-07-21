#if !defined(AFX_WIN9XPRECOMP_H__00268CE3_6ECB_441F_A0A7_58ECEF2C9591__INCLUDED_)
#define AFX_WIN9XPRECOMP_H__00268CE3_6ECB_441F_A0A7_58ECEF2C9591__INCLUDED_

#define USE_NDIS

extern "C" {
    #include <vtoolsc.h>       
}

#define KL1_Major			1
#define KL1_Minor			0
#define KL1_DeviceID		UNDEFINED_DEVICE_ID
#define KL1_Init_Order		VXDLDR_INIT_ORDER + 4

#include "kldef.h"
#include "klstatus.h"
#include "g_thread.h"
#include "..\loader.h"
#include "HOOK\kl1_api.h"



#endif // !defined(AFX_WIN9XPRECOMP_H__00268CE3_6ECB_441F_A0A7_58ECEF2C9591__INCLUDED_)
