#ifndef _G_KLOP_DISPATCHER_H_
#define _G_KLOP_DISPATCHER_H_

#include "kldef.h"
#include "klstatus.h"
#include "..\hook\klop_api.h"
#include "..\hook\klim_api.h"
#include "g_custom_packet.h"
#include "g_cp_proto.h"
#include "g_key.h"

#define CHECK_IN_BUFFER_SIZE(_size_)                        \
if ( (pInBuffer == NULL) || ( InBufferSize < (_size_) ) )   \
    return KL_BUFFER_TOO_SMALL

#define CHECK_OUT_BUFFER_SIZE(_size_)                        \
if ( (pOutBuffer == NULL) || ( OutBufferSize < (_size_) ) )  \
    return KL_BUFFER_TOO_SMALL

KLSTATUS
KlopDispatch(
			 ULONG dwIOCTL, 
			 PVOID pInBuffer, 
			 ULONG InBufferSize, 
			 PVOID pOutBuffer, 
			 ULONG OutBufferSize, 
			 ULONG* nReturned );

extern UCHAR	isWin98;
extern HANDLE	ClientPID;

extern PKLIM_TABLE ext_KlimTable;

#endif