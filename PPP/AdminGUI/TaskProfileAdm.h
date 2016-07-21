// TaskProfileAdm.h: interface for the CProfileAdm class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "TaskProfile.h"

namespace KLPRSS
{
	class SettingsStorage;
};

#include "pid_gui_fix.h"

class CProfileAdm : public CProfile
{
public:
	DECLARE_IID( CProfileAdm, CProfile, PID_GUI, gst_CTaskProfileAdm );

	CProfileAdm();
	CProfileAdm(CProfileAdm &src);
	~CProfileAdm();
	
	CProfileAdm &          operator =(const CProfileAdm &src);

	virtual tDWORD         GetCreateIID() const { return eIID; }
	CProfile *             GetProfileByType(LPCSTR strType);
	static LPCSTR          Type2Name(LPCSTR strType);
	cStringObj             GetName();
	bool                   SetInterfaces(CONFIGURATOR_TYPES eCfgType, IEnumInterfaces* pIfaces);
	
	virtual bool           GetSettings(bool bUsePolicy = true);
	virtual tERROR         SetSettings(bool bUsePolicy = true);

	tERROR                 WritePolicySection(LPCWSTR sSection, KLPAR::Params *pParams);

protected:
	tERROR                 SaveGroupPolicySett(PParams pPolicyTasks = NULL);
	tERROR                 SaveLocalPolicySett(bool bUsePolicy);
	tERROR                 SaveTaskSett(bool bUsePolicy);
	
	tERROR                 LoadGroupPolicySett(bool bLoadLockedOnly = false, PParams pPolicyTasks = NULL);
	tERROR                 LoadLocalPolicySett(bool bUsePolicy);
	tERROR                 LoadTaskSett(bool bUsePolicy);

	bool                   UpgradePolicy();
	void                   ConvertPolicy();

protected:
	tERROR                 ReadSS(KLPRSS::SettingsStorage *pStorage, LPCWSTR sSection, KLPAR::Params **ppParams);
	tERROR                 SaveSS(KLPRSS::SettingsStorage *pStorage, LPCWSTR sSection, KLPAR::Params *pParams);

protected:
	void                   SetInterfaces();

protected:
	CONFIGURATOR_TYPES                       m_eCfgType;
	KLSTD::CAutoPtr<KLPRSS::SettingsStorage> m_pPolicySS;
	KLSTD::CAutoPtr<KLPRSS::SettingsStorage> m_pProductSS;
	KLSTD::CAutoPtr<KLPAR::Params>           m_pTaskParams;
};

typedef CProfileAdm CProduct;

//////////////////////////////////////////////////////////////////////

