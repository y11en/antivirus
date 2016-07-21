#if !defined(__TRAFFIC_MONITOR_H)
#define __TRAFFIC_MONITOR_H

#include <Prague/prague.h>
#include <Prague/pr_vtbl.h>
#include <Prague/iface/i_reg.h>
#include <Prague/iface/i_root.h>
#include <ProductCore/iface/i_task.h>


struct pr_novtable CTrafficMonitor : public cTask
{
public:

	CTrafficMonitor ();
	virtual ~CTrafficMonitor ();

	tERROR pr_call SetSettings( const cSerializable* p_settings ) {return errOK;};
	tERROR pr_call GetSettings( cSerializable* p_settings ) {return errOK;};
	tERROR pr_call AskAction( tActionId p_actionId, cSerializable* p_actionInfo ) {return errOK;};
	tERROR pr_call SetState( tTaskRequestState p_state ) {return errOK;};
	tERROR pr_call GetStatistics( cSerializable* p_statistics ) {return errOK;};

	virtual tERROR pr_call sysSendMsg( tDWORD msg_cls, tDWORD msg_id, tPTR ctx, tPTR pbuff, tDWORD* pblen );


};



#endif