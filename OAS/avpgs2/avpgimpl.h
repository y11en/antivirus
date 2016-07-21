#ifndef avpgimpl_h
#define avpgimpl_h

#include <Prague/prague.h>
#include <OAS/iface/i_avpg2.h>
//#include <Prague/pr_serializable.h>
//#include <Prague/pr_vector.h>
#include <ProductCore/common/userban.h>

extern tPROPID propid_CustomOrigin;
extern tPROPID propid_DosDeviceName;
extern tPROPID propid_EventProcessID;
extern tPROPID propid_Background;
extern tPROPID propid_UserAccessed;
extern tPROPID propid_UserCreated;

class cAvpgImpl
{
public:
	cAvpgImpl( hOBJECT pPragueContext, tAVPG2_WORKING_MODE WorkingMode, tDWORD ThreadMaxCount )
	{
		m_pPragueContext = pPragueContext;
		m_WorkingMode = WorkingMode;
		m_ThreadMaxCount = ThreadMaxCount;
		m_ActivityMode = cAVPG2_CLIENT_ACTIVITY_MODE_SLEEP;
	};
	virtual ~cAvpgImpl(){};

	virtual tERROR SetActivityMode ( tAVPG2_CLIENT_ACTIVITY_MODE ActivityMode )
	{
		m_ActivityMode = ActivityMode;
		return errOK;
	};

	virtual tERROR ChangeWorkingMode( tAVPG2_WORKING_MODE WorkingMode ) {
		m_WorkingMode = WorkingMode;
		return errOK;
	};

	virtual tERROR YieldEvent( hOBJECT EventObject, tDWORD YieldTimeout )
	{
		return errNOT_IMPLEMENTED;
	};

	virtual tERROR ResetDriverCache()
	{
		return errNOT_IMPLEMENTED;
	};

	virtual tERROR CreateIO( hOBJECT* phObject, hSTRING hObjectName )
	{
		return errNOT_IMPLEMENTED;
	};

	virtual tERROR SetPreverdict( hOBJECT EventObject, tAVPG2_PROCESS_OBJECT_RESULT_FLAGS ResultFlags )
	{
		return errNOT_IMPLEMENTED;
	};

	virtual tERROR ResetBanList()
	{
		return errNOT_IMPLEMENTED;
	};

	virtual tERROR AddBan( cUserInfo* pUserInfo )
	{
		return errNOT_IMPLEMENTED;
	};

	virtual tERROR ClearTrustedProcessesCache()
	{
		return errNOT_IMPLEMENTED;
	};

public:
	hOBJECT						m_pPragueContext;
	tAVPG2_WORKING_MODE			m_WorkingMode;
	tDWORD						m_ThreadMaxCount;
	tAVPG2_CLIENT_ACTIVITY_MODE	m_ActivityMode;
};


#endif // avpgimpl_h