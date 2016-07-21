#ifndef __KLIF_PCH_H__
#define __KLIF_PCH_H__

#if WINVER<0x0501
#define WIN2K
#else
#define RTL_USE_AVL_TABLES 0
#endif //WINVER>=0x0500
//
//  Enabled warnings
//

#pragma warning(error:4100)     //  Enable-Unreferenced formal parameter
#pragma warning(error:4101)     //  Enable-Unreferenced local variable
#pragma warning(error:4061)     //  Eenable-missing enumeration in switch statement
#pragma warning(error:4505)     //  Enable-identify dead functions

//
//  Includes
//


#include <fltKernel.h>

#ifndef WIN2K
#include <ntstrsafe.h>   // This is for using RtlStringcbPrintfXXX
#endif

// global system data
NTSYSAPI USHORT NtBuildNumber;
extern PULONG InitSafeBootMode;

#include "eresource.h"
#include "tags.h"

#include "mtypes.h"

#include "hook.h"

#include "Struct.h"
#include "inc/commdata.h"
#include "structures.h"

#include "comm.h"
#include "Proc.h"
#include "debug.h"
#include "osspec.h"
#include "client.h"
#include "InvThread.h"
#include "filter.h"
#include "runproc.h"
#include "prepfltd.h"
#include "SysIO.h"
#include "lists.h"
#include "extreq.h"
#include "lldio.h"
#include "cdwork.h"
#include "klsecur.h"
#include "regops.h"
#include "regflt.h"
#include "specfunc.h"
#include "ichdrv.h"
#include "fidbox.h"
#include "resolve.h"
#include "md5.h"
#include "apprul.h"
#include "klfltdev.h"
#include "virt.h"
#include "gencache.h"
#include "HkCheckLock.h"

#include "..\hook\HookSpec.h"

#endif __KLIF_PCH_H__

