#ifndef _licmgr_task_structs_
#define _licmgr_task_structs_

#include <ProductCore/structs/s_taskmanager.h>
#include <PPP/plugin/p_licmgr.h>

struct cLicMgrSettings : public cTaskSettings
{
	cLicMgrSettings() :
		m_bSecondary(cFALSE)
	{}

	DECLARE_IID_SETTINGS(cLicMgrSettings, cTaskSettings, PID_LICMGR);

	cStringObj              m_sFileName;
	tBOOL                   m_bSecondary;
	cVector<tBYTE>          m_aLicBody;
};

IMPLEMENT_SERIALIZABLE_BEGIN(cLicMgrSettings, 0)
	SER_BASE(cTaskSettings, 0)
	SER_VALUE(  m_sFileName,        tid_STRING_OBJ,         "FileName" )
	SER_VALUE(  m_bSecondary,       tid_BOOL,               "Secondary" )
	SER_VECTOR_STREAM(m_aLicBody,   tid_BYTE,               "LicBody" )
IMPLEMENT_SERIALIZABLE_END()

#endif // _ahfw_licmgr_structs_
