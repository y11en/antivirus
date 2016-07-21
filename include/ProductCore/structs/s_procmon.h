#ifndef __S_PROCMON_H
#define __S_PROCMON_H

#include <ProductCore/plugin/p_processmonitor.h>
#include <ProductCore/structs/s_taskmanager.h>
#include <ProductCore/structs/s_trmod.h>

struct cBLTrustedApps : public cTaskSettings
{
	DECLARE_IID_SETTINGS( cBLTrustedApps, cTaskSettings, PID_PROCESSMONITOR );

	cVector<cBLTrustedApp> m_aItems;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cBLTrustedApps, 0 )
	SER_BASE( cTaskSettings, 0 )
	SER_VECTOR_M( aItems, cBLTrustedApp::eIID )
IMPLEMENT_SERIALIZABLE_END( );

//-----------------------------------------------------------------------------

#endif//  __S_PROCMON_H
