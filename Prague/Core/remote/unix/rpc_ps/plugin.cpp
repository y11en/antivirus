
#include <rpc.h>
#include <prague.h>
#include "../pr_manager.h"
#include <iface/i_root.h>

extern PRRemoteManager * g_manager = NULL;

// --------------------------------------------------------------------------------
extern "C" void PRRegisterProxyIID_BUFFER ();
extern "C" void PRRegisterStubIID_BUFFER ();
extern "C" void PRUnregisterProxyIID_BUFFER ();
extern "C" void PRUnregisterStubIID_BUFFER ();

void RPCRegister(hROOT root)
{
  tPROPID prop;
  if( PR_FAIL(root->RegisterCustomPropId(&prop, PR_REMOTE_MANAGER_PROP, pTYPE_PTR)) )
    return;
  
  g_manager = (PRRemoteManager*)root->propGetPtr(prop);
  if( !g_manager )
    return;
  
  PR_REGISTER_PROXY(IID_BUFFER);
  PR_REGISTER_STUB(IID_BUFFER);	
}

void PRCUnregister ()
{
  if ( !g_manager )
    return;
  PR_UNREGISTER_STUB(IID_BUFFER);	  
}

extern "C" tBOOL DllMain ( tVOID* hModule, 
                           tDWORD ul_reason_for_call, 
                           tVOID* lpReserved
					 )
{
  switch (ul_reason_for_call)	{
  case PRAGUE_PLUGIN_LOAD :
    RPCRegister((hROOT)hModule);
    break;
  case PRAGUE_PLUGIN_UNLOAD :
    PRCUnregister ();
    break;
  default:
    break;
  }
  return cTRUE;
}
