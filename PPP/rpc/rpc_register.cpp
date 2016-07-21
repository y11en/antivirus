// --------------------------------------------------------------------------------

#define __PROXY_DECLARATION

#include "stdafx.h"
#include <iface/i_root.h>
#include <iface/i_taskmanager.h>
#include <iface/i_tmclient.h>
#include <iface/i_tmhost.h>
#include <iface/i_avs.h>
#include <iface/i_cryptohelper.h>
#include <iface/i_licensing.h>
#include <iface/i_report.h>
#include <iface/i_task.h>
#include <iface/i_mailtask.h>
#include <iface/i_processmonitor.h>
#include <iface/i_os.h>
#include <iface/i_objptr.h>
#include <iface/i_io.h>

extern "C" void __RPC_FAR * __RPC_USER MIDL_user_allocate( size_t len )       { return malloc(len); }
extern "C" void             __RPC_USER MIDL_user_free( void __RPC_FAR * ptr ) { free(ptr);} 

PRRemoteManager * g_manager = NULL;

// --------------------------------------------------------------------------------

extern "C" void RegisterRPC(hROOT root)
{
	tPROPID prop;
	if( PR_FAIL(root->RegisterCustomPropId(&prop, PR_REMOTE_MANAGER_PROP, pTYPE_PTR)) )
		return;

	g_manager = (PRRemoteManager*)root->propGetPtr(prop);
	if( !g_manager )
	{
		OutputDebugString("RegisterRPC error\n");
		return;
	}

/*	PR_REGISTER_PROXY(IID_TASK)
	PR_REGISTER_STUB(IID_TASK)

	PR_REGISTER_PROXY(IID_AVS)
	PR_REGISTER_STUB(IID_AVS)

	PR_REGISTER_PROXY(IID_AVSTREATS)
	PR_REGISTER_STUB(IID_AVSTREATS)

	PR_REGISTER_PROXY(IID_REPORT)
	PR_REGISTER_STUB(IID_REPORT)

	PR_REGISTER_PROXY(IID_IO)
	PR_REGISTER_STUB(IID_IO)

	PR_REGISTER_PROXY(IID_OS)
	PR_REGISTER_STUB(IID_OS)

	PR_REGISTER_PROXY(IID_OBJPTR)
	PR_REGISTER_STUB(IID_OBJPTR)

	PR_REGISTER_PROXY(IID_MAILTASK)
	PR_REGISTER_STUB(IID_MAILTASK)

	PR_REGISTER_PROXY(IID_TASKMANAGER)
	PR_REGISTER_STUB(IID_TASKMANAGER)

	PR_REGISTER_PROXY(IID_TM_CLIENT)
	PR_REGISTER_STUB(IID_TM_CLIENT)

	PR_REGISTER_PROXY(IID_TMHOST)
	PR_REGISTER_STUB(IID_TMHOST)

	PR_REGISTER_PROXY(IID_LICENSING)
	PR_REGISTER_STUB(IID_LICENSING)

	PR_REGISTER_PROXY(IID_CRYPTOHELPER)
	PR_REGISTER_STUB(IID_CRYPTOHELPER)

	PR_REGISTER_PROXY(IID_STRING)
	PR_REGISTER_STUB(IID_STRING)
	
	PR_REGISTER_PROXY(IID_PROCESSMONITOR)
	PR_REGISTER_STUB(IID_PROCESSMONITOR)
*/
}

extern "C" void UnregisterRPC()
{
	if( !g_manager )
		return;

/*	PR_UNREGISTER_STUB(IID_TASK)
	PR_UNREGISTER_STUB(IID_AVS)
	PR_UNREGISTER_STUB(IID_AVSTREATS)
	PR_UNREGISTER_STUB(IID_REPORT)
	PR_UNREGISTER_STUB(IID_IO)
	PR_UNREGISTER_STUB(IID_MAILTASK)
	PR_UNREGISTER_STUB(IID_OS)
	PR_UNREGISTER_STUB(IID_OBJPTR)
	PR_UNREGISTER_STUB(IID_TASKMANAGER)
	PR_UNREGISTER_STUB(IID_TM_CLIENT)
	PR_UNREGISTER_STUB(IID_TMHOST)
	PR_UNREGISTER_STUB(IID_LICENSING)
	PR_UNREGISTER_STUB(IID_CRYPTOHELPER)
	PR_UNREGISTER_STUB(IID_STRING)
	PR_UNREGISTER_STUB(IID_PROCESSMONITOR)
*/
}
