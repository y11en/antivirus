#ifndef _hips_task_structs_
#define _hips_task_structs_

#include <Prague/pr_time.h>
#include <ProductCore/structs/s_taskmanager.h>
#include "s_fw.h"
#include "../plugin/p_hips.h"
#include <ProductCore/report.h>
#include <AV/structs/s_avs.h>

#include "../../../Windows/Hook/mklif/inc/hips_tools.h"

#define warnFLT_DEV_ERR PR_MAKE_IMP_WARN(PID_HIPS, 0x1)

#define HIPS_NOT_USED_RES_ID	1	//	service resource number (indicate no parent)
#define HIPS_ROOT_GR_NUM		0x00000001	//	number of root resource
#define HIPS_ROOT_APP_GR_NUM 0x80000002
#define HIPS_USER_PREFIX		0x40000000

#define HIPS_HASH_SIZE	16
#define IS_KL_RES_ID(Id) (((tDWORD)(Id)) < 0x40000000)
#define IS_KL_RULE_ID(Id) (((tDWORD)(Id)) < 0x40000000)

#define HIPS_GR_PREFIX	0x80000000

#define HIPS_FLAG_DENY_ALL (HIPS_FLAG_DENY | (HIPS_FLAG_DENY<<4) | (HIPS_FLAG_DENY<<(4*2)) | (HIPS_FLAG_DENY<<(4*3)))

#define HIPS_MAKE_ID_EX(id, is_group) ( (is_group) ? ((id) | HIPS_GR_PREFIX) : (id) )
#define HIPS_IS_GR_ID_EX(idEx) ( !!((idEx) & HIPS_GR_PREFIX) )
#define HIPS_GET_ID_FROM_EX(idEx) ((idEx) & ~HIPS_GR_PREFIX)

#define HIPS_GET_RES_TYPE(val) ( (val) & 0x1F)		//	5 low bits
#define HIPS_GET_RES_ENABLED(val) ( (val) & 0x20)	//	6-th bit

#define HIPS_SET_RES_TYPE(val, type) ( (type) | (HIPS_GET_RES_ENABLED(val)))
#define HIPS_SET_RES_ENABLED(val, enabled) ( ( (enabled) << 5) | (HIPS_GET_RES_TYPE((val))))
#define HIPS_SET_RES_FLAG(enabled, type) (( (enabled) <<5 ) | (type) )

//	states that could came from GUI, and could be shown
#define HIPS_RULE_STATE_DENY	0	//	deny
#define HIPS_RULE_STATE_ALLOW	1	//	allow
#define HIPS_RULE_STATE_ASK		2	//	ask
#define HIPS_RULE_STATE_INH		10	//	inherit

//	states that could be shown
#define HIPS_RULE_STATE_MIX		3	//	mixed state
#define HIPS_RULE_STATE_UNK		4	//	rule not set

#define HIPS_RULE_STATE_INH_DENY	5	//	deny
#define HIPS_RULE_STATE_INH_ALLOW	6	//	allow
#define HIPS_RULE_STATE_INH_ASK		7	//	ask
#define HIPS_RULE_STATE_INH_MIX		8	//	mixed state
#define HIPS_RULE_STATE_INH_UNK		9	//	rule not set

#define HIPS_ROOT_GR_ID			2

//////////////////////////////////////////////////////
//	types of resource params
//////////////////////////////////////////////////////
enum eHIPS_ResParamType {
	ehrptStr	= 0,		//	string type
	ehrptNum	= 1			//	number type
};
//////////////////////////////////////////////////////
//	hips structures
//////////////////////////////////////////////////////
enum eHIPS_STRUCTS
{
	eFileInfo = 0,
	eRegKeyInfo,
	eHipsResItem,
	eHipsResGroup,
	//eHipsResGrouping,
	eHipsAppItem,
	eAppGroup,
	eHipsRuleItem,
	eHipsZoneRules,
	eHipsReportInfo,
	eHipsAskObjectAction,
	eDeviceInfo,
	eResource,
	ePrivileges,
	eHipsPacketRule,
	eSeverity,
	eFirewallSettings,
    eExclusion,
	eHipsStatistics,
	eHipsNetZone
};
struct cFirewallSettings  : public cSerializable
{
	enum eFwFlags
	{
		FwPacketRules        = 1,	
		FwAppRules           = 2,	
		FwLocalhostAllowAll  = 4,   //if not set, rules for localhost makes by severity;
		FwBlockInternet		 = 8,
		FwBlockAll			 = 0x10,   //block all, localhost by FwLocalhostAllowAll
	};
	cFirewallSettings():
		m_nFlags(FwPacketRules+FwAppRules),
	m_nFilteringMode(0),   //fmMaxCompatibility
	m_nWorkingMode(3),      //wmAllowNotFiltered
	m_State(TASK_REQUEST_RUN)
	{}
	DECLARE_IID( cFirewallSettings, cSerializable, PID_HIPS, eFirewallSettings );
	tDWORD	m_State;
	tDWORD	m_nFlags;
	tDWORD  m_nFilteringMode;
	tDWORD  m_nWorkingMode;
};
IMPLEMENT_SERIALIZABLE_BEGIN( cFirewallSettings, 0 )
	SER_VALUE(m_State,            tid_DWORD,     "State")
	SER_VALUE(m_nFlags,            tid_DWORD,     "Flags")
	SER_VALUE(m_nFilteringMode,    tid_DWORD,     "FilteringMode")
	SER_VALUE(m_nWorkingMode,      tid_DWORD,     "WorkingMode")
IMPLEMENT_SERIALIZABLE_END()

//////////////////////////////////////////////////////
//	FILE resource param description
//////////////////////////////////////////////////////
struct cFileInfo : public cSerializable
{
	cFileInfo() : m_bRecursive(cFALSE) {}

	DECLARE_IID( cFileInfo, cSerializable, PID_HIPS, eFileInfo );

	cStringObj	m_Path;
	tBOOL		m_bRecursive;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cFileInfo, 0 )
	SER_VALUE( m_Path,			tid_STRING_OBJ,	"Path" )
	SER_VALUE( m_bRecursive,	tid_BOOL,		"Recursive" )
IMPLEMENT_SERIALIZABLE_END()
//////////////////////////////////////////////////////////////////////////
// cRegKeyInfo
//////////////////////////////////////////////////////////////////////////
struct cRegKeyInfo : public cSerializable
{
	cRegKeyInfo() : m_bRecursive(cFALSE) {}

	DECLARE_IID( cRegKeyInfo, cSerializable, PID_HIPS, eRegKeyInfo );

	cStringObj	m_KeyPath;
	cStringObj	m_KeyValue;
	tBOOL		m_bRecursive;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cRegKeyInfo, 0 )
	SER_VALUE( m_KeyValue,		tid_STRING_OBJ,	"KeyValue" )
	SER_VALUE( m_KeyPath,		tid_STRING_OBJ,	"KeyPath" )
	SER_VALUE( m_bRecursive,	tid_BOOL,		"Recursive" )
IMPLEMENT_SERIALIZABLE_END()
//////////////////////////////////////////////////////
//	Device resource param description
//////////////////////////////////////////////////////
struct cDeviceInfo : public cSerializable
{
	DECLARE_IID( cDeviceInfo, cSerializable, PID_HIPS, eDeviceInfo );

	cStringObj	m_sClassGUID;		//	class GUID
	cStringObj	m_sTypeName;		//	subtype name
};

IMPLEMENT_SERIALIZABLE_BEGIN( cDeviceInfo, 0 )
	SER_VALUE( m_sClassGUID,	tid_STRING_OBJ,	"ClassGUID" )
	SER_VALUE( m_sTypeName,		tid_STRING_OBJ,	"TypeName" )
IMPLEMENT_SERIALIZABLE_END()
//////////////////////////////////////////////////////
//	Privileges resource param description
//////////////////////////////////////////////////////
struct cPrivilegeInfo : public cSerializable
{
	cPrivilegeInfo() : m_nType(ehptProcStart) {};
	//-----------------------------------------------------------------------
	DECLARE_IID( cPrivilegeInfo, cSerializable, PID_HIPS, ePrivileges );
	//-----------------------------------------------------------------------
	//	types of Privileges
	enum eType
	{
		ehptUnknown			= 0,		//	
		ehptProcStart		= 1,		//	
		ehptProcStop		= 2,		//	
		ehptSetHook			= 3,
		ehptCodeInject		= 4,
		ehptWindowsShutDown = 5,
		ehptHiddenRegistry	= 6,
		ehptKeyLogger		= 7,
		ehptSetHardLink     = 8,
		ehptSuspend			= 9,
		ehptSchedulerStart	= 10,
		ehptWMSend			= 11,
		ehptDrvStart		= 12,
		ehptServiceStart	= 13,
		ehptScreenShots		= 14,
		ehptDiskFormat		= 15,
		ehptLLDiskAccess	= 16,
		ehptLLFSAccess		= 17,
		ehptClipBoardAcceess= 18,
		ehptSysEnterChange	= 19,
		ehptADSAccess		= 20,
		ehptDirectMemAccess	= 21,
		ehptReadProcMem		= 22,
		ehptSetDbgPrivilege	= 23,
		ehptChangeObjPrivilege	= 24,
		ehptUseBrowserCL	= 25,
		ehptUseBrowserAPI	= 26,
		ehptUseSystemAPI	= 27
	};
	//-----------------------------------------------------------------------
	tDWORD	m_nType;		//	eType
};
IMPLEMENT_SERIALIZABLE_BEGIN( cPrivilegeInfo, 0 )
	SER_VALUE( m_nType,		tid_DWORD,	"Type" )
IMPLEMENT_SERIALIZABLE_END()
//////////////////////////////////////////////////////
//	Severity resource param description
//////////////////////////////////////////////////////
struct cSeverity : public cSerializable
{
	cSeverity() : m_nType(ehsUnknown) {};
	//-----------------------------------------------------------------------
	DECLARE_IID( cSeverity, cSerializable, PID_HIPS, eSeverity );
	//-----------------------------------------------------------------------
	//	types of Privileges
	enum eType
	{
		ehsUnknown	= 0,
		ehsLocal	= 1,
		ehsTrusted	= 2,
		ehsPublic	= 3,
	};
	//-----------------------------------------------------------------------
	tDWORD	m_nType;		//	eType
};
IMPLEMENT_SERIALIZABLE_BEGIN( cSeverity, 0 )
	SER_VALUE( m_nType,		tid_DWORD,	"Type" )
IMPLEMENT_SERIALIZABLE_END()
//////////////////////////////////////////////////////
//	NetZone resource param description
//////////////////////////////////////////////////////
struct cHipsNetZone : public cSerializable
{
	cHipsNetZone() : m_nID(0) {};
	//-----------------------------------------------------------------------
	DECLARE_IID( cHipsNetZone, cSerializable, PID_HIPS, eHipsNetZone );
	//-----------------------------------------------------------------------
	tDWORD	m_nID;
};
IMPLEMENT_SERIALIZABLE_BEGIN( cHipsNetZone, 0 )
	SER_VALUE( m_nID,		tid_DWORD,	"ID" )
IMPLEMENT_SERIALIZABLE_END()
//////////////////////////////////////////////////////
//	Exclusions resource param description
//////////////////////////////////////////////////////
struct cExclusion : public cSerializable
{
	cExclusion(tDWORD nType = ehexOAS) : m_nType(nType) {};
	//-----------------------------------------------------------------------
	DECLARE_IID( cExclusion, cSerializable, PID_HIPS, eExclusion );
	//-----------------------------------------------------------------------
	//	types of Privileges
	enum eType
	{
		ehexOAS		= 0,
		ehexPDM		= 1,
		ehexNetwork	= 2,
	};
	//-----------------------------------------------------------------------
	tDWORD	m_nType;		//	eType
};
IMPLEMENT_SERIALIZABLE_BEGIN( cExclusion, 0 )
	SER_VALUE( m_nType,		tid_DWORD,	"Type" )
IMPLEMENT_SERIALIZABLE_END()
//////////////////////////////////////////////////////
//	uniq (res\gr) description
//////////////////////////////////////////////////////
struct cResource : public cSerializable
{
	cResource() : cSerializable(), m_nID(0), m_nParentID(0), m_nFlags(0), m_sName() {}
	//-----------------------------------------------------------------------
	DECLARE_IID(cResource, cSerializable, PID_HIPS, eResource);
	//-----------------------------------------------------------------------
	enum eFlags
	{
		ehrtUnknown          = 1,
		ehrtFile             = 2, // 1 param
		ehrtReg              = 3, // 2 params
		ehrtDevice           = 4, // 1 param
		ehrtPrivileges       = 5,
		ehrtWebService       = 6,
		ehrtIPRange          = 7,
		ehrtSeverity		 = 8,
		ehrtExclusion		 = 9,
		ehrtNetZone			 = 10,
		ehrtMask             = 0x1f,
		
		fEnabled             = 0x0020,
		fResourceView        = 0x0040,	//	reverce (set if need to disable)
		fFilesRegistryView   = 0x0080,	//	reverce (set if need to disable)
		fRulesView           = 0x0100,	//	direct  (set if need to enable)
	};
	//-----------------------------------------------------------------------
	tDWORD Type()
	{
		return m_nFlags & ehrtMask;
	}
	//-----------------------------------------------------------------------
	cResource * Find(tDWORD nID)
	{
		if( nID == 0 )
			return NULL;

		if( m_nID == nID )
			return this;

		for(tSIZE_T i = 0, n = m_vChilds.count(); i < n; i++)
			if( cResource *res = m_vChilds[i].Find(nID) )
				return res;

		return NULL;
	}
	//-----------------------------------------------------------------------
	void GetNextID(tDWORD &nID) const
	{
		if( m_nID >= nID )
			nID = m_nID + 1;
		
		for(tSIZE_T i = 0, n = m_vChilds.count(); i < n; i++)
			m_vChilds[i].GetNextID(nID);
	}
	//-----------------------------------------------------------------------
	cResource * FindResByContent(cSerializable * l_pData)
	{
		if (l_pData == 0)
			return 0;
		cResource * pRes = 0;
		//	if l_pRes == 0 this is root (or find from current pos)
		for (tDWORD i = 0; i < m_vChilds.count(); i++)
		{
			if ((m_vChilds[i]).m_pData &&
				(l_pData->getIID() == (m_vChilds[i]).m_pData->getIID()))
			{
				switch (l_pData->getIID())
				{
					case cDeviceInfo::eIID:
					{
						cDeviceInfo * pDev = *(cDeviceInfo**)&m_vChilds[i].m_pData;
						if ((((cDeviceInfo*)l_pData)->m_sClassGUID == pDev->m_sClassGUID) &&
							(((cDeviceInfo*)l_pData)->m_sTypeName == pDev->m_sTypeName))
						{
							pRes = &m_vChilds[i];
						}
						break;
					}
					case cPrivilegeInfo::eIID:
					{
						cPrivilegeInfo * pPriv = *(cPrivilegeInfo**)&m_vChilds[i].m_pData;
						if (((cPrivilegeInfo*)l_pData)->m_nType == pPriv->m_nType)
						{
							pRes = &m_vChilds[i];
						}
						break;
					}
					case cExclusion::eIID:
					{
						cExclusion * pExcl = *(cExclusion**)&m_vChilds[i].m_pData;
						if (((cExclusion*)l_pData)->m_nType == pExcl->m_nType)
						{
							pRes = &m_vChilds[i];
						}
						break;
					}
					case cHipsNetZone::eIID:
					{
						cHipsNetZone * pZone = *(cHipsNetZone**)&m_vChilds[i].m_pData;
						if (((cHipsNetZone*)l_pData)->m_nID == pZone->m_nID)
						{
							pRes = &m_vChilds[i];
						}
						break;
					}
				};
			}
			if (pRes)
				return pRes;
			pRes = m_vChilds[i].FindResByContent(l_pData);
			if (pRes)
				return pRes;
		}
		return 0;
	}
	//-----------------------------------------------------------------------
	tERROR AddResource(cResource & Res)
	{
		if (Res.m_nID == Res.m_nParentID &&
			Res.m_nID == HIPS_ROOT_GR_NUM)
		{
			//	there is no parent, it is root
			m_nID = Res.m_nID;
			m_nParentID = Res.m_nParentID;
			m_sName = Res.m_sName;
			m_nFlags = Res.m_nFlags;
			return errOK;
		}
		if (m_nID == Res.m_nParentID)
		{
			//	need add to this resource
			m_vChilds.push_back(Res);
			return errOK;
		}
		cResource * pParentRes = Find(Res.m_nParentID);
		if (pParentRes)
		{
			pParentRes->m_vChilds.push_back(Res);
			pParentRes->m_nFlags |= (Res.m_nFlags & 0x3f);//HIPS_SET_RES_FLAG(1, HIPS_GET_RES_TYPE(Res.m_nFlags));
			return errOK;
		}
		else
			return errNOT_FOUND;
	}
	//-----------------------------------------------------------------------
	tBYTE GetBlockToCompare()
	{
		tDWORD type = Type();
		return type == ehrtDevice || type == ehrtPrivileges ? 1 : 4;
	}
	//-----------------------------------------------------------------------
	tDWORD					m_nID;			//	unique ID
	tDWORD					m_nParentID;	//	parent ID
	tDWORD					m_nFlags;		//	eFlags
	cStringObj				m_sName;		//	group name \ res description
	cSerObj<cSerializable>	m_pData;		//	resource param (item depend on m_ResType)
	cVector<cResource>		m_vChilds;		//	child groups
};
//-----------------------------------------------------------------------
IMPLEMENT_SERIALIZABLE_BEGIN(cResource, "cResource")
	SER_VALUE(m_nID,		tid_DWORD,       "Id")
	SER_VALUE(m_nParentID,	tid_DWORD,		 "ParentID")
	SER_VALUE(m_nFlags,		tid_DWORD,       "Flags")
	SER_VALUE(m_sName,		tid_STRING_OBJ,  "Name")
	SER_VALUE_PTR(m_pData,	cSer::eIID,      "Data" )
	SER_VECTOR(m_vChilds,   cResource::eIID, "Childs")
IMPLEMENT_SERIALIZABLE_END()
//////////////////////////////////////////////////////
//	
//////////////////////////////////////////////////////
class cResourceIterator
{
private:
	struct tState
	{
		cResource * first;
		size_t second;
		tState() { first = 0; second = 0; }
		tState(cResource * l_first, size_t l_second) { first = l_first; second = l_second; }
	};
	typedef cVector<tState> tStates;
public:
	tState m_state;
	tStates m_states;
	cResourceIterator() {m_state.first = 0; m_state.second = 0;};
	cResourceIterator(cResource *root)  { ReInit(root); }
	void ReInit(cResource *root)  { m_state.first = root; m_state.second = 0; }

	cResource *GetNext(bool skipChildren = false)
	{
		if (m_states.size() == 0)
			m_states.push_back(m_state);
		if (!skipChildren)
		{
			cVector<cResource> &childs = m_state.first->m_vChilds;
			size_t childsNum = childs.size();
			if( m_state.second < childsNum )
			{
				cResource *child = &childs[m_state.second++];
				if( child->m_vChilds.size() )
				{
					m_state.first = child; m_state.second = 0;
					m_states.push_back(m_state);
				}
				return child;
			}
		}
		if( m_states.empty() )
			return NULL;
		m_states.remove(m_states.size()-1);//  pop_back();
		if( m_states.empty() )
			return NULL;
		m_state = m_states[m_states.size()-1];//m_states.back();
		m_states.remove(m_states.size()-1);//m_states.pop_back();
		m_state.second++;
		m_states.push_back(m_state);
		return GetNext();
	}
};
//////////////////////////////////////////////////////
//	application desciption
//////////////////////////////////////////////////////
struct CHipsAppItem: public cSerializable
{
	CHipsAppItem() : cSerializable(),
		m_AppId(0),
		m_AppGrId(0),
		m_AppName()
	{
		m_AppName.setCP(cCP_UNICODE);
		memset(m_AppHash, 0, sizeof(m_AppHash));
		m_AppFlags.Zero();
	}
	DECLARE_IID(CHipsAppItem, cSerializable, PID_HIPS, eHipsAppItem);

	tDWORD			m_AppId;					//	uniq application ID
	tDWORD			m_AppGrId;					//	uniq application ID
	cStringObj		m_AppName;					//	application name || mask
	tBYTE			m_AppHash[HIPS_HASH_SIZE];	//	application hash
	cBitMask		m_AppFlags;					//	for service purpose (trasted class, is verinfo, ...)
};

typedef cVector<CHipsAppItem> cApplications;

//-----------------------------------------------------------------------
IMPLEMENT_SERIALIZABLE_BEGIN(CHipsAppItem, "CHipsAppItem")
	SER_VALUE_M(AppId,		tid_DWORD)
	SER_VALUE_M(AppGrId,	tid_DWORD)
	SER_VALUE_M(AppName,	tid_STRING_OBJ)
	SER_VALUE(m_AppHash,	tid_BINARY, "AppHash")
	SER_VALUE(m_AppFlags,	tid_BINARY, "AppFlags")
IMPLEMENT_SERIALIZABLE_END()

//////////////////////////////////////////////////////
//	RULE desciption
//	this rule store in settings (rules for driver generated from this rule list)
//////////////////////////////////////////////////////
struct CHipsRuleItem: public cSerializable
{
	CHipsRuleItem() : cSerializable(),
		m_RuleId(0),
		m_RuleState(0),
		m_RuleTypeId(0), 
		m_AppIdEx(0),
		m_ResIdEx(0),
		m_AccessFlag(0)
	{
	}
	DECLARE_IID(CHipsRuleItem, cSerializable, PID_HIPS, eHipsRuleItem);

	enum {fDisabled = 0x0, fEnabled = 0x0001 };

	enum eType
	{
		ehrltGlobDeny		= HIPS_RULE_TYPE_GLB_DNY,	//	1
		ehrltLocAllow		= HIPS_RULE_TYPE_LCL_ALW,	//	2
		ehrltLocDeny		= HIPS_RULE_TYPE_LCL_DNY,	//	3
		ehrltPacket			= 4,	//	packet rules
		ehrltAppSeverity	= 5,	//	common severity rules for app
		ehrltAppWeb			= 6		//	extended rules for app
	};


	bool   IsInherit(tBYTE BlockToCompare) const  {
		bool bRes = true;
		if (BlockToCompare > HIPS_POS_WRITE)
			bRes = bRes && HIPS_GET_INH(m_AccessFlag, HIPS_POS_WRITE);
		if (BlockToCompare > HIPS_POS_READ)
			bRes = bRes && HIPS_GET_INH(m_AccessFlag, HIPS_POS_READ);
		if (BlockToCompare > HIPS_POS_ENUM)
			bRes = bRes && HIPS_GET_INH(m_AccessFlag, HIPS_POS_ENUM);
		if (BlockToCompare > HIPS_POS_DELETE)
			bRes = bRes && HIPS_GET_INH(m_AccessFlag, HIPS_POS_DELETE);
		return bRes;
	}

	bool   IsSomeInherit(tBYTE BlockToCompare) const  {
		bool bRes = false;
		if (BlockToCompare > HIPS_POS_WRITE)
			bRes = bRes || HIPS_GET_INH(m_AccessFlag, HIPS_POS_WRITE);
		if (BlockToCompare > HIPS_POS_READ)
			bRes = bRes || HIPS_GET_INH(m_AccessFlag, HIPS_POS_READ);
		if (BlockToCompare > HIPS_POS_ENUM)
			bRes = bRes || HIPS_GET_INH(m_AccessFlag, HIPS_POS_ENUM);
		if (BlockToCompare > HIPS_POS_DELETE)
			bRes = bRes || HIPS_GET_INH(m_AccessFlag, HIPS_POS_DELETE);
		return bRes;
	}

	tDWORD MakeAMState(tBYTE BlockToCompare)
	{
		tDWORD temp1 = HIPS_GET_AM(m_AccessFlag, HIPS_POS_WRITE);
		tDWORD temp2 = HIPS_GET_AM(m_AccessFlag, HIPS_POS_READ);
		tDWORD temp3 = HIPS_GET_AM(m_AccessFlag, HIPS_POS_ENUM);
		tDWORD temp4 = HIPS_GET_AM(m_AccessFlag, HIPS_POS_DELETE);

		bool IsTheSame = true;
		if (BlockToCompare > HIPS_POS_WRITE)
			IsTheSame = true;
		if (BlockToCompare > HIPS_POS_READ)
			IsTheSame = temp1 == temp2;
		if (BlockToCompare > HIPS_POS_ENUM)
			IsTheSame = temp2 == temp3;
		if (BlockToCompare > HIPS_POS_DELETE)
			IsTheSame = temp3 == temp4;

		if (IsTheSame)
		{
			//	equal am per operation
			if (temp1 == HIPS_FLAG_ALLOW)
				return HIPS_RULE_STATE_ALLOW;
			else if (temp1 == HIPS_FLAG_ASK)
				return HIPS_RULE_STATE_ASK;
			else
				return HIPS_RULE_STATE_DENY;
		}
		return HIPS_RULE_STATE_MIX;
	}


	tDWORD AppId() const      { return HIPS_GET_ID_FROM_EX(m_AppIdEx); }
	bool   IsAppGroup() const { return HIPS_IS_GR_ID_EX(m_AppIdEx); }

	tDWORD		m_RuleId;			//	internal rule ID
	tDWORD		m_RuleState;		//	enable/disable (may be excess)
	tDWORD		m_RuleTypeId;		//	eType
	tDWORD		m_AppIdEx;			//	application item/group id
	tDWORD		m_ResIdEx;			//	resource item/group id
	tDWORD		m_ResIdEx2;
	tDWORD		m_AccessFlag;		//	bitmask of rule
};
//-----------------------------------------------------------------------
IMPLEMENT_SERIALIZABLE_BEGIN(CHipsRuleItem, "CHipsRuleItem")
	SER_VALUE_M(RuleId,		tid_DWORD)
	SER_VALUE_M(RuleState,	tid_DWORD)
	SER_VALUE_M(RuleTypeId,	tid_DWORD)
	SER_VALUE_M(AppIdEx,	tid_DWORD)
	SER_VALUE_M(ResIdEx,	tid_DWORD)
	SER_VALUE_M(ResIdEx2,	tid_DWORD)
	SER_VALUE_M(AccessFlag,	tid_DWORD)
IMPLEMENT_SERIALIZABLE_END()

//////////////////////////////////////////////////////////////////////////

struct CHipsPacketRule : public CHipsRuleItem
{
	CHipsPacketRule() : m_Severity(0) {}

	DECLARE_IID(CHipsPacketRule, CHipsRuleItem, PID_HIPS, eHipsPacketRule);
	
	tDWORD m_Severity;
	//tDWORD m_ResIdEx2;	//	moved to CHipsRuleItem
};

IMPLEMENT_SERIALIZABLE_BEGIN(CHipsPacketRule, "CHipsPacketRule")
	SER_BASE(CHipsRuleItem, 0)
	SER_VALUE_M(Severity, tid_DWORD)
	//SER_VALUE_M(ResIdEx2, tid_DWORD)	//	moved to CHipsRuleItem
IMPLEMENT_SERIALIZABLE_END()

//////////////////////////////////////////////////////////////////////////
// cAppGroup

struct cAppGroup : public cSerializable
{
	cAppGroup() : m_nAppGrpID(0), m_nServStrType(fNotUsed) {}

	DECLARE_IID( cAppGroup, cSerializable, PID_HIPS, eAppGroup );

	tDWORD             m_nAppGrpID;			//	uniq application group ID
	cStringObj	       m_sGroupName;		//	application group name
	cVector<cAppGroup> m_aChilds;			//	list of child groups

	enum {fNotUsed = 0, fDgtSign = 1, fProdName = 2};
	//	for service purpose (may by additional group property)
	tDWORD             m_nServStrType;		//	type of service string
	cStringObj	       m_sServStr;			//	service string (digital signature/product name/...)

	//-----------------------------------------------------------------------
	cAppGroup * Find(tDWORD nID)
	{
		if( nID == 0 )
			return NULL;

		if( m_nAppGrpID == nID )
			return this;

		for(tSIZE_T i = 0, n = m_aChilds.count(); i < n; i++)
			if( cAppGroup * app = m_aChilds[i].Find(nID) )
				return app;

		return NULL;
	}
	//-----------------------------------------------------------------------
	cAppGroup * FindByName(cStringObj GrName)
	{
		if( GrName == "" )
			return NULL;

		if( m_sGroupName == GrName )
			return this;

		for(tSIZE_T i = 0, n = m_aChilds.count(); i < n; i++)
			if( cAppGroup * app = m_aChilds[i].FindByName(GrName) )
				return app;

		return NULL;
	}
	//-----------------------------------------------------------------------
	void GetNextID(tDWORD &nID) const
	{
		if( m_nAppGrpID >= nID )
			nID = m_nAppGrpID + 1;
		
		for(tSIZE_T i = 0, n = m_aChilds.count(); i < n; i++)
			m_aChilds[i].GetNextID(nID);
	}
	//-----------------------------------------------------------------------
	cAppGroup * Add(cStringObj GrName, tDWORD GrID)
	{
		if (Find(GrID))
			return 0;
		if (FindByName(GrName))
			return 0;

		cAppGroup App;
		App.m_nAppGrpID = GrID;
		App.m_sGroupName = GrName;
		m_aChilds.push_back(App);
		return &m_aChilds[m_aChilds.size()-1];
	}
};

IMPLEMENT_SERIALIZABLE_BEGIN( cAppGroup, 0 )
	SER_VALUE( m_nAppGrpID,		tid_DWORD,		"AppGrpID" )
	SER_VALUE( m_sGroupName,	tid_STRING_OBJ,	"GroupName" )
	SER_VECTOR( m_aChilds,		cAppGroup::eIID,"Childs" )
	SER_VALUE( m_nServStrType,	tid_DWORD,		"ServStrType" )
	SER_VALUE( m_sServStr,		tid_STRING_OBJ,	"ServStr" )
IMPLEMENT_SERIALIZABLE_END()

typedef void (*ProcessAppGrItemFunc) (cAppGroup & pItem, cAppGroup & pParentItem, void * pContext);


//////////////////////////////////////////////////////
//	CHipsSettings
//////////////////////////////////////////////////////
struct CHipsSettings: public cSerializable
{
	CHipsSettings() : cSerializable(),
		//m_vResList(),
		m_vAppList(),
		m_vRuleList(),
		m_FileAndRegState(ehsssApp),
		m_DevicesState(ehsssApp),
		m_NetworkState(ehsssApp),
		m_PermissionsState(ehsssApp)
	{
	}

	DECLARE_IID_SETTINGS(CHipsSettings, cSerializable, PID_HIPS);

	enum eSubSystemState{
		ehsssAllow	= 0,
		ehsssDeny	= 1,
		ehsssApp	= 2
	};

	tDWORD GetNextResID() const;
	tDWORD GetUniqueResID() const;
	tDWORD GetUniqueResGroupID() const;
	//static tDWORD GetUniqueResID(const cResources &resources);
	//!!!
	//static void   GetUniqueGroupID(const CHipsResourceGroupsItem &grp, tDWORD &uniqueID, tDWORD RecursiveLevel = 0);
	static tDWORD GetResTypeBySerID(tDWORD SetID);

	tDWORD GetUniqueAppID();
	tDWORD GetUniqueAppID(const cVector<CHipsAppItem> &apps);

	tDWORD GetUniqueAppGrID();
	//tDWORD GetUniqueAppGrID(const cVector<cAppGroup> &appgrs);

	tDWORD GetUniqueRuleID();
	tDWORD GetUniqueRuleID(const cVector<CHipsRuleItem> &rules);


	tDWORD GetState(tDWORD AppId, bool IsAppGroup, tDWORD ResId, bool IsResGroup);
	tDWORD SetState(tDWORD AppId, bool IsAppGroup, tDWORD ResId, bool IsResGroup, tDWORD State);

	tERROR GetRule(tDWORD AppId, bool IsAppGroup, tDWORD ResId, bool IsResGroup, CHipsRuleItem ** ppRealRule, CHipsRuleItem & VirtRule, bool IsNested = false);
	tERROR GetRule(tDWORD AppIdEx, tDWORD ResIdEx, CHipsRuleItem ** ppRealRule, CHipsRuleItem & VirtRule, bool IsNested = false, tDWORD * pOrigApp = 0);
	tERROR SetRule(CHipsRuleItem & Rule);
	tERROR CreateRule(tDWORD RuleTypeId, tDWORD AppId, bool IsAppGroup, tDWORD ResId, bool IsResGroup, tDWORD AccessMask);
	tERROR CreateRule(CHipsRuleItem * l_pRule);
	tERROR DeleteRule(tDWORD RuleId);

	tDWORD MergeWithAF(tDWORD l_SourceAccessFlag, tDWORD l_AccessFlag);

	//!!!
	//tERROR GetParentRes(tDWORD ResIdEx, tDWORD * pOutResIsEx);
	tERROR GetParentApp(tDWORD AppIdEx, tDWORD * pOutAppIsEx);
	
	//!!!
	//tERROR GetResGrById(CHipsResourceGroupsItem & RootGr, tDWORD GrId, CHipsResourceGroupsItem ** ppOutGr, CHipsResourceGroupsItem ** ppOutParentGr);
	tERROR GetAppGrById(cAppGroup & RootGr, tDWORD GrId, cAppGroup ** ppOutGr, cAppGroup ** ppOutParentGr);
	tERROR DeleteAppGrById(tDWORD GrId);

	tERROR GetAppById(tDWORD AppId, CHipsAppItem ** ppApp);


	//!!!
	//typedef void (*ProcessResGrItemFunc) (CHipsResourceGroupsItem & pItem, void * pContext);
	//!!!
	//static void CollectAllResGrID(CHipsResourceGroupsItem & Item, void * pContext);
	//!!!
	//void EnumResGroups(CHipsResourceGroupsItem & RootGr, ProcessResGrItemFunc pFunc, void * pContext);

//	typedef void (*ProcessAppGrItemFunc) (cAppGroup & pItem, void * pContext);
	static void CollectAllKLAppGrID(cAppGroup & Item, cAppGroup & ParentGr, void * pContext);
	void EnumAppGroups(cAppGroup & RootGr, cAppGroup & ParentGr, ProcessAppGrItemFunc pFunc, void * pContext);

	void MakeConsistent();

	cResource                           m_Resource;				//	Resource tree
	cApplications						m_vAppList;				//	application list
	cAppGroup                           m_AppGroups;			//	application groups
	cVector<CHipsRuleItem>				m_vRuleList;			//	rule list
	cVector<CHipsPacketRule>            m_PacketRules;

	cFirewallSettings					m_FirewallSettings;

	tDWORD								m_FileAndRegState;		//	global HIPS state to file and registry
	tDWORD								m_DevicesState;			//	global HIPS state to Devices
	tDWORD								m_NetworkState;			//	global HIPS state to Network
	tDWORD								m_PermissionsState;		//	global HIPS state to Permissions
};

//-----------------------------------------------------------------------

inline tDWORD CHipsSettings::GetNextResID() const
{
	tDWORD uniqueID = 0x40000000;
	m_Resource.GetNextID(uniqueID);
	uniqueID |= 0x40000000;
	return uniqueID;
}
//!!!
//inline tDWORD CHipsSettings::GetUniqueResID() const
//{
//	return CHipsSettings::GetUniqueResID(m_vResList);
//}
//-----------------------------------------------------------------------
//inline tDWORD CHipsSettings::GetUniqueResID(const cResources &resources)
//{
//	tDWORD uniqueID = 0x40000000;		//	users resource ID separates from KL resources
//	for(size_t  i = 0, n = resources.size(); i < n; i++)
//		if( uniqueID <= resources[i].m_ResID )
//			uniqueID = resources[i].m_ResID + 1;
//	return uniqueID;
//}
//-----------------------------------------------------------------------
inline tDWORD CHipsSettings::GetUniqueRuleID()
{
	return GetUniqueRuleID(m_vRuleList);
}
//-----------------------------------------------------------------------
inline tDWORD CHipsSettings::GetUniqueRuleID(const cVector<CHipsRuleItem> &rules)
{
	tDWORD uniqueID = 0x40000000;		//	users resource ID separates from KL resources
	for(size_t  i = 0, n = rules.size(); i < n; i++)
		if( uniqueID <= rules[i].m_RuleId )
			uniqueID = rules[i].m_RuleId + 1;
	return uniqueID;
}
//-----------------------------------------------------------------------
inline tDWORD CHipsSettings::GetUniqueAppID()
{
	return GetUniqueAppID(m_vAppList);
}
//-----------------------------------------------------------------------
inline tDWORD CHipsSettings::GetUniqueAppID(const cVector<CHipsAppItem> &apps)
{
	tDWORD uniqueID = 0x40000000;		//	users resource ID separates from KL resources
	for(size_t  i = 0, n = apps.size(); i < n; i++)
		if( uniqueID <= apps[i].m_AppId )
			uniqueID = apps[i].m_AppId + 1;
	return uniqueID;
}
//-----------------------------------------------------------------------
inline tDWORD CHipsSettings::GetUniqueAppGrID()
{
	tDWORD uniqueID = 0x40000000;
	m_AppGroups.GetNextID(uniqueID);
	uniqueID |= 0x40000000;
	return uniqueID;
//	return GetUniqueAppGrID(m_AppGroups);
}
//-----------------------------------------------------------------------
//inline tDWORD CHipsSettings::GetUniqueAppGrID(const cAppGroup &appgrs)
//{
//	tDWORD uniqueID = 0x40000000;		//	users resource ID separates from KL resources
//	for(size_t  i = 0, n = appgrs.size(); i < n; i++)
//		if( uniqueID <= appgrs[i].m_nAppGrpID )
//			uniqueID = appgrs[i].m_nAppGrpID + 1;
//	return uniqueID;
//}
//!!!
//-----------------------------------------------------------------------
//inline tDWORD CHipsSettings::GetUniqueResGroupID() const
//{
//	tDWORD uniqueID = 0x40000000;	//	users groups ID separates from KL groups
//	GetUniqueGroupID(m_ResGroups, uniqueID);
//	return uniqueID;
//}
//-----------------------------------------------------------------------
//	this function have been called directly (not throw GetUniqueResGroupID()),
//	that is why RecursiveLevel needed
//!!!
//inline void CHipsSettings::GetUniqueGroupID(const CHipsResourceGroupsItem &grp, tDWORD &uniqueID, tDWORD RecursiveLevel)
//{
//	if (RecursiveLevel == 0)
//		uniqueID = 0x40000000;
//	if( uniqueID <= grp.m_ResGroupID )
//		uniqueID = grp.m_ResGroupID + 1;
//
//	for(size_t  i = 0, n = grp.m_vChildResGroupList.size(); i < n; i++)
//		GetUniqueGroupID(grp.m_vChildResGroupList[i], uniqueID, ++RecursiveLevel);
//}
//-----------------------------------------------------------------------
inline tDWORD CHipsSettings::GetResTypeBySerID(tDWORD SerID)
{
	switch (SerID)
	{
		case (cFileInfo::eIID) :
			return (tDWORD)cResource::ehrtFile;
		case (cRegKeyInfo::eIID) :
			return (tDWORD)cResource::ehrtReg;
		case (cWebService::eIID) :
			return (tDWORD)cResource::ehrtWebService;
		default:
			return (tDWORD)cResource::ehrtUnknown;
	}
}
//-----------------------------------------------------------------------
inline tDWORD CHipsSettings::SetState(tDWORD AppId, bool IsAppGroup, tDWORD ResId, bool IsResGroup, tDWORD State)
{
	if ((State == HIPS_RULE_STATE_DENY) ||
		(State == HIPS_RULE_STATE_ALLOW) ||
		(State == HIPS_RULE_STATE_ASK) ||
		(State == HIPS_RULE_STATE_INH))
	{
		tDWORD AccessMask = 0;
		tDWORD flag = HIPS_FLAG_ALLOW;
		cResource * pRes = 0;
		switch (State)
		{
			case HIPS_RULE_STATE_DENY:
				flag = HIPS_FLAG_DENY;
				break;
			case HIPS_RULE_STATE_ALLOW:
				flag = HIPS_FLAG_ALLOW;
				break;
			case HIPS_RULE_STATE_ASK:
				flag = HIPS_FLAG_ASK;
				break;
			case HIPS_RULE_STATE_INH:
				//flag = HIPS_FLAG_ALLOW;
				if (!IsAppGroup)
					break;
				pRes = m_Resource.Find(ResId);
				if (pRes && pRes->m_nParentID == HIPS_ROOT_GR_NUM)
					return errNOT_OK;
				break;
		}
		tDWORD LogFlag = 0;

		LogFlag = (flag == HIPS_FLAG_DENY) ? HIPS_LOG_ON : HIPS_LOG_OFF;
		AccessMask = HIPS_SET_BLOCK_POS(flag,	LogFlag, 0,	HIPS_POS_WRITE,		AccessMask);
		LogFlag = HIPS_LOG_OFF;
		AccessMask = HIPS_SET_BLOCK_POS(flag,	LogFlag, 0,	HIPS_POS_READ,		AccessMask);
		LogFlag = HIPS_LOG_OFF;
		AccessMask = HIPS_SET_BLOCK_POS(flag,	LogFlag, 0,	HIPS_POS_ENUM,		AccessMask);
		LogFlag = (flag == HIPS_FLAG_DENY) ? HIPS_LOG_ON : HIPS_LOG_OFF;
		AccessMask = HIPS_SET_BLOCK_POS(flag,	LogFlag, 0,	HIPS_POS_DELETE,	AccessMask);

		CHipsRuleItem * pRealRule = 0;
		CHipsRuleItem VirtRule;
		if (PR_SUCC(GetRule(AppId, IsAppGroup, ResId, IsResGroup, &pRealRule, VirtRule)))
		{
			if (pRealRule)
			{
				//	it was a real rule
				if (State == HIPS_RULE_STATE_INH)
				{
					//	need delete rule
					DeleteRule(pRealRule->m_RuleId);
				}
				else 
				{
					//	need change rule
					pRealRule->m_AccessFlag = AccessMask;
				}
			}
			else
			{
				//	this is virtual rule
				if (State != HIPS_RULE_STATE_INH)
				{
					//	create if it is not inherit
					CreateRule(VirtRule.m_RuleTypeId, AppId, IsAppGroup, ResId, IsResGroup, AccessMask);
				}
			}
		}
		return errOK;
	}
	else
	{
		return errNOT_OK;
	}
}
//-----------------------------------------------------------------------
inline tDWORD CHipsSettings::GetState(tDWORD AppId, bool IsAppGroup, tDWORD ResId, bool IsResGroup)
{
	CHipsRuleItem * pRealRule = 0;
	CHipsRuleItem VirtRule;
	tERROR err = GetRule(AppId, IsAppGroup, ResId, IsResGroup, &pRealRule, VirtRule);
	if (PR_FAIL(err))
		return HIPS_RULE_STATE_UNK;
	tBOOL IsInherit = cFALSE;
	if (pRealRule == 0)
	{
		pRealRule = &VirtRule;
		IsInherit = cTRUE;
	}

	tBYTE BlockToCompare = 4;
	cResource * pRes = m_Resource.Find(ResId);
	if (pRes)
		BlockToCompare = pRes->GetBlockToCompare();
	tDWORD StateByRule = pRealRule->MakeAMState(BlockToCompare);// MakeAMState(pRealRule->m_AccessFlag);

	if (IsResGroup)
	{
		tDWORD StateByChilds = 0;
		cResource * pTempRes = 0;
		if (pRes && (pRes->m_nParentID == HIPS_ROOT_GR_NUM))
		{
			cResourceIterator it (pRes);
			bool NeedSkip = false;
			do
			{
				pTempRes = it.GetNext(NeedSkip);
				if (pTempRes == 0)
				{
					break;
				}
				NeedSkip = !pTempRes->m_pData;
				err = GetRule(AppId, IsAppGroup, pTempRes->m_nID, 1, &pRealRule, VirtRule);
				if (PR_FAIL(err))
					continue;
				if (pRealRule == 0)
					pRealRule = &VirtRule;

				StateByChilds = pRealRule->MakeAMState(BlockToCompare);//MakeAMState(pRealRule->m_AccessFlag);
				if (StateByRule != StateByChilds)
				{
					StateByRule = HIPS_RULE_STATE_MIX;
					break;
				}
			} while (pTempRes);
		}


	}
	return (IsInherit) ? StateByRule + 5 : StateByRule;
}
//!!!
//-----------------------------------------------------------------------
//inline tERROR CHipsSettings::GetResGrById(CHipsResourceGroupsItem & RootGr, tDWORD GrId, CHipsResourceGroupsItem ** ppOutGr, CHipsResourceGroupsItem ** ppOutParentGr)
//{
//	if (RootGr.m_ResGroupID == GrId)
//	{
//		*ppOutGr = &RootGr;
//		return errOK;
//	}
//	for (tDWORD i = 0; i < RootGr.m_vChildResGroupList.size(); i++)
//	{
//		if (PR_SUCC(GetResGrById(RootGr.m_vChildResGroupList[i], GrId, ppOutGr, ppOutParentGr)))
//		{
//			if (ppOutParentGr && (*ppOutParentGr == 0))
//				*ppOutParentGr = &RootGr;
//			return errOK;
//		}
//	}
//	return errNOT_FOUND;
//}
//-----------------------------------------------------------------------
inline tERROR CHipsSettings::GetAppGrById(cAppGroup & RootGr, tDWORD GrId, cAppGroup ** ppOutGr, cAppGroup ** ppOutParentGr)
{
	if (RootGr.m_nAppGrpID == GrId)
	{
		if (ppOutGr)
			*ppOutGr = &RootGr;
		return errOK;
	}
	for (tDWORD i = 0; i < RootGr.m_aChilds.size(); i++)
	{
		if (PR_SUCC(GetAppGrById(RootGr.m_aChilds[i], GrId, ppOutGr, ppOutParentGr)))
		{
			if (ppOutParentGr && (*ppOutParentGr == 0))
				*ppOutParentGr = &RootGr;
			return errOK;
		}
	}
	return errNOT_FOUND;
}
//-----------------------------------------------------------------------
inline tERROR CHipsSettings::GetAppById(tDWORD AppId, CHipsAppItem ** ppApp)
{
	if ((AppId <= 1) || (ppApp == 0))
		return errNOT_OK;
	for (tDWORD i = 0; i < m_vAppList.size(); i++)
	{
		if (m_vAppList[i].m_AppId == AppId)
		{
			*ppApp = &m_vAppList[i];
			return errOK;
		}
	}
	return errNOT_FOUND;
}
//!!!
//-----------------------------------------------------------------------
//inline tERROR CHipsSettings::GetParentRes(tDWORD ResIdEx, tDWORD * pOutResIsEx)
//{
//	if (pOutResIsEx == 0)
//		return errNOT_OK;
//	bool IsGr = HIPS_IS_GR_ID_EX(ResIdEx);
//	tDWORD ResId = HIPS_GET_ID_FROM_EX(ResIdEx);
//
//	if (IsGr)
//	{
//		CHipsResourceGroupsItem * pGr = 0;
//		CHipsResourceGroupsItem * pParentGr = 0;
//		if (PR_SUCC(GetResGrById(m_ResGroups, ResId, &pGr, &pParentGr)) && (pParentGr))
//		{
//			*pOutResIsEx = HIPS_MAKE_ID_EX(pParentGr->m_ResGroupID, 1);
//			return errOK;
//		}
//	}
//	else
//	{
//		cResource * pRes = m_Resource.FindResByID(&m_Resource, ResId);
//		if (pRes)
//		{
//			*pOutResIsEx = pRes->m_nParentID;
//			return errOK;
//		}
//		else
//		{
//			//	temp while two resource storage
//			for (tDWORD i = 0; i < m_vResList.count(); i++)
//			{
//				if (m_vResList[i].m_ResID == ResId)
//				{
//					*pOutResIsEx = HIPS_MAKE_ID_EX(m_vResList[i].m_ResGrID, 1);
//					return errOK;
//				}
//			}
//		}
//	}
//
//	*pOutResIsEx = 0;
//	return errNOT_FOUND;
//}
//-----------------------------------------------------------------------
inline tERROR CHipsSettings::GetParentApp(tDWORD AppIdEx, tDWORD * pOutAppIsEx)
{
	if (pOutAppIsEx == 0)
		return errNOT_OK;
	bool IsGr = HIPS_IS_GR_ID_EX(AppIdEx);
	tDWORD AppId = HIPS_GET_ID_FROM_EX(AppIdEx);

	if (IsGr)
	{
		//	try to find parent group fo this group
		//m_AppGroups
		cAppGroup * pGr = 0;
		cAppGroup * pParentGr = 0;
		if (PR_SUCC(GetAppGrById(m_AppGroups, AppId, &pGr, &pParentGr)) && (pParentGr))
		{
			*pOutAppIsEx = HIPS_MAKE_ID_EX(pParentGr->m_nAppGrpID, 1);
			return errOK;
		}
	}
	else
	{
		//	try to find parent group fo this app
		CHipsAppItem * pApp = 0;
		if (PR_SUCC(GetAppById(AppId, &pApp)) && pApp)
		{
			*pOutAppIsEx = HIPS_MAKE_ID_EX(pApp->m_AppGrId, 1);
			return errOK;
		}
	}

	*pOutAppIsEx = 0;
	return errNOT_FOUND;
}
//-----------------------------------------------------------------------
inline tERROR CHipsSettings::CreateRule(CHipsRuleItem * l_pRule)
{
	if (l_pRule == 0)
		return errNOT_OK;
	return CreateRule(
		l_pRule->m_RuleTypeId,
		HIPS_GET_ID_FROM_EX(l_pRule->m_AppIdEx),
		HIPS_IS_GR_ID_EX(l_pRule->m_AppIdEx),
		//!!!
		l_pRule->m_ResIdEx,//HIPS_GET_ID_FROM_EX(l_pRule->m_ResIdEx),
		0,//HIPS_IS_GR_ID_EX(l_pRule->m_ResIdEx),
		l_pRule->m_AccessFlag
		);
}
//-----------------------------------------------------------------------
inline tERROR CHipsSettings::CreateRule(tDWORD RuleTypeId, tDWORD AppId, bool IsAppGroup, tDWORD ResId, bool IsResGroup, tDWORD AccessMask)
{
	tDWORD AppIdEx = HIPS_MAKE_ID_EX(AppId, IsAppGroup);
	tDWORD ResIdEx;
	//!!!
	ResIdEx = ResId;

	for (tDWORD i = 0; i < m_vRuleList.size(); i++)
	{
		if (!m_vRuleList[i].m_RuleState)
			continue;
		if ((m_vRuleList[i].m_AppIdEx == AppIdEx) &&
			(m_vRuleList[i].m_ResIdEx == ResIdEx))
		{
			//	direct rule founded
			if (m_vRuleList[i].m_AccessFlag == AccessMask)
			{
				//	no need changes
				return errOK;
			}
			else
			{
				if (IS_KL_RULE_ID(m_vRuleList[i].m_RuleId))
				{
					//	if KL rule -> disable rule
					m_vRuleList[i].m_RuleState = 0;
				}
				else
				{
					//	if user rule -> change it
					m_vRuleList[i].m_AccessFlag = AccessMask;
					return errOK;
				}
			}
		}
	}

	CHipsRuleItem Rule;
	Rule.m_RuleId = GetUniqueRuleID();
	Rule.m_AppIdEx = AppIdEx;
	Rule.m_ResIdEx = ResIdEx;
	Rule.m_RuleState = CHipsRuleItem::fEnabled;
	Rule.m_RuleTypeId = RuleTypeId;//HIPS_RULE_TYPE_LCL_DNY;
	Rule.m_AccessFlag = AccessMask;
	m_vRuleList.push_back(Rule);
	return errOK;
}
//-----------------------------------------------------------------------
inline tDWORD CHipsSettings::MergeWithAF(tDWORD l_SourceAccessFlag, tDWORD l_AccessFlag) 
{
	tDWORD temp = 0;
	tDWORD temp2 = 0;

	tDWORD res = HIPS_GET_INH(l_SourceAccessFlag, HIPS_POS_WRITE);
	if (res == 1)
		temp = HIPS_SET_BLOCK_POS_EX((HIPS_GET_BLOCK(l_AccessFlag, HIPS_POS_WRITE) | 8), HIPS_POS_WRITE, temp);
	else
		temp = HIPS_SET_BLOCK_POS_EX(HIPS_GET_BLOCK(l_SourceAccessFlag, HIPS_POS_WRITE), HIPS_POS_WRITE, temp);

	res = HIPS_GET_INH(l_SourceAccessFlag, HIPS_POS_READ);
	if (res == 1)
		temp = HIPS_SET_BLOCK_POS_EX((HIPS_GET_BLOCK(l_AccessFlag, HIPS_POS_READ) | 8), HIPS_POS_READ, temp);
	else
		temp = HIPS_SET_BLOCK_POS_EX(HIPS_GET_BLOCK(l_SourceAccessFlag, HIPS_POS_READ), HIPS_POS_READ, temp);

	res = HIPS_GET_INH(l_SourceAccessFlag, HIPS_POS_ENUM);
	if (res == 1)
		temp = HIPS_SET_BLOCK_POS_EX((HIPS_GET_BLOCK(l_AccessFlag, HIPS_POS_ENUM) | 8), HIPS_POS_ENUM, temp);
	else
		temp = HIPS_SET_BLOCK_POS_EX(HIPS_GET_BLOCK(l_SourceAccessFlag, HIPS_POS_ENUM), HIPS_POS_ENUM, temp);

	res = HIPS_GET_INH(l_SourceAccessFlag, HIPS_POS_DELETE);
	if (res == 1)
	{
		//temp2 = HIPS_GET_BLOCK(l_AccessFlag, HIPS_POS_DELETE) | 8;
		temp = HIPS_SET_BLOCK_POS_EX((HIPS_GET_BLOCK(l_AccessFlag, HIPS_POS_DELETE) | 8), HIPS_POS_DELETE, temp);
	}
	else
		temp = HIPS_SET_BLOCK_POS_EX(HIPS_GET_BLOCK(l_SourceAccessFlag, HIPS_POS_DELETE), HIPS_POS_DELETE, temp);
	return temp;
}
//-----------------------------------------------------------------------
inline tERROR CHipsSettings::GetRule(tDWORD AppId, bool IsAppGroup, tDWORD ResId, bool IsResGroup, CHipsRuleItem ** ppRealRule, CHipsRuleItem & VirtRule, bool IsNested)
{
	tDWORD AppIdEx = HIPS_MAKE_ID_EX(AppId, IsAppGroup);
	//!!!
	tDWORD ResIdEx = ResId;//HIPS_MAKE_ID_EX(ResId, IsResGroup);
	return GetRule(AppIdEx, ResIdEx, ppRealRule, VirtRule, IsNested);
}
inline tERROR CHipsSettings::GetRule(tDWORD AppIdEx, tDWORD ResIdEx, CHipsRuleItem ** ppRealRule, CHipsRuleItem & VirtRule, bool IsNested, tDWORD * pOrigApp)
{
	tERROR err = errOBJECT_NOT_FOUND;
	if (m_vRuleList.size() == 0)
		return err;

	VirtRule.m_AccessFlag = 0;
	bool WasVirtRule = false;
	CHipsRuleItem tempRule;
	tDWORD ParentAppIdEx = 0;
	tDWORD ParentResIdEx = 0;
	cResource * pParentRes = 0;
	cResource * pRes = 0;
	tDWORD OrigApp = 0;
	if (!pOrigApp)
		pOrigApp = &OrigApp;

do
{
	if (IsNested)
		ppRealRule = 0;

	//	try to find direct rule
	for (tDWORD i = 0; i < m_vRuleList.size(); i++)
	{
		if (!m_vRuleList[i].m_RuleState)
			continue;
		if ((m_vRuleList[i].m_AppIdEx == AppIdEx) &&
			(m_vRuleList[i].m_ResIdEx == ResIdEx))
		{
			//	direct rule founded
			WasVirtRule = true;
			if (ppRealRule)
				*ppRealRule = &m_vRuleList[i];
			else
			{
				VirtRule = m_vRuleList[i];
			}
			//cResource * pRes = m_Resource.Find(m_vRuleList[i].m_ResIdEx);
			//tBYTE BlockToCompare = 4;
			//if (pRes)
			//	BlockToCompare = pRes->GetBlockToCompare();
			//if (!m_vRuleList[i].IsSomeInherit(BlockToCompare))
			//	return errOK;
			//break;
			return errOK;
		}
	}


	//!!!
	if ((pRes = m_Resource.Find(ResIdEx)) &&
		(pParentRes = m_Resource.Find(pRes->m_nParentID)))
	{
		if (pParentRes->m_nID != HIPS_ROOT_GR_NUM) // not root
		{
			if (PR_SUCC(GetRule(AppIdEx, pParentRes->m_nID, 0, tempRule, true, pOrigApp)))
			{
				if (WasVirtRule)
				{
					if (ppRealRule && *ppRealRule)
						(*ppRealRule)->m_AccessFlag = MergeWithAF((*ppRealRule)->m_AccessFlag, tempRule.m_AccessFlag);
					else
						VirtRule.m_AccessFlag = MergeWithAF(VirtRule.m_AccessFlag, tempRule.m_AccessFlag);
				}
				else
				{
					VirtRule = tempRule;
					VirtRule.m_AppIdEx = (pOrigApp && *pOrigApp!=0) ? *pOrigApp : AppIdEx;
					VirtRule.m_ResIdEx = ResIdEx;
					if (AppIdEx != 0)
						VirtRule.m_AccessFlag |= 0x8888;
					else
						VirtRule.m_AccessFlag |= 0x8;
				}
				return errOK;
			}
		}
	}
	if (IsNested)
		break;

	//	try file rules for parent applications
	if (PR_SUCC(GetParentApp(AppIdEx, &ParentAppIdEx)))
	{
		if (ParentAppIdEx != HIPS_ROOT_APP_GR_NUM)	//not root
		{
			if (PR_SUCC(GetRule(ParentAppIdEx, ResIdEx, 0, tempRule, true, pOrigApp)))
			{
				if (WasVirtRule)
					if (ppRealRule && *ppRealRule)
					{
						(*ppRealRule)->m_AccessFlag = MergeWithAF((*ppRealRule)->m_AccessFlag, tempRule.m_AccessFlag);
					}
					else
					{
						VirtRule.m_AccessFlag = MergeWithAF(VirtRule.m_AccessFlag, tempRule.m_AccessFlag);
					}
				else
				{
					VirtRule = tempRule;
					VirtRule.m_AppIdEx = (pOrigApp && *pOrigApp!=0) ? *pOrigApp : AppIdEx;
					VirtRule.m_ResIdEx = ResIdEx;
					if (AppIdEx != 0)
						VirtRule.m_AccessFlag |= 0x8888;
					else
						VirtRule.m_AccessFlag |= 0x8;
				}
				return errOK;
			}
		}
	}
	ppRealRule = 0;
	if (ParentAppIdEx == HIPS_ROOT_APP_GR_NUM)
		break;
	if (pOrigApp && *pOrigApp==0)
		*pOrigApp = AppIdEx;
} while (!IsNested && (PR_SUCC(GetParentApp(ParentAppIdEx, &AppIdEx))));

	//	was not found anything (not normal)
	//VirtRule.m_AccessFlag = HIPS_RULE_STATE_UNK;
	return errNOT_OK;
}
//-----------------------------------------------------------------------
inline tERROR CHipsSettings::SetRule(CHipsRuleItem & Rule)
{
	CHipsRuleItem * pRealRule = 0;
	CHipsRuleItem VirtRule;

	tERROR err = GetRule(Rule.AppId(), Rule.IsAppGroup(), Rule.m_ResIdEx, 0, &pRealRule, VirtRule);
	if( PR_FAIL(err) )
		return err;
		
	if( pRealRule )
	{
		cResource * pRes = m_Resource.Find(Rule.m_ResIdEx);
		tBYTE BlockToCompare = 4;
		tDWORD ParentAppIdEx = 0;
		if (pRes)
		{
			BlockToCompare = pRes->GetBlockToCompare();
			if ((pRes->m_nParentID == HIPS_ROOT_GR_NUM) &&
				Rule.IsSomeInherit(BlockToCompare) &&
				(PR_SUCC(GetParentApp(Rule.m_AppIdEx, &ParentAppIdEx))) &&
				ParentAppIdEx == HIPS_ROOT_APP_GR_NUM)
				return errNOT_OK;
		}
		if( Rule.IsInherit(BlockToCompare) )
			return DeleteRule(pRealRule->m_RuleId);
		
		pRealRule->m_AccessFlag = Rule.m_AccessFlag;
		return errOK;
	}
	
	//return CreateRule(Rule.AppId(), Rule.IsAppGroup(), (Rule.m_AppIdEx == 0) ? Rule.m_ResIdEx : Rule.ResId(), Rule.IsResGroup(), Rule.m_AccessFlag);
	return CreateRule(Rule.m_RuleTypeId, Rule.AppId(), Rule.IsAppGroup(), Rule.m_ResIdEx, 0, Rule.m_AccessFlag);
}

//-----------------------------------------------------------------------
inline tERROR CHipsSettings::DeleteRule(tDWORD RuleId)
{
	for (tDWORD i = 0; i < m_vRuleList.size(); i++)
	{
		if (m_vRuleList[i].m_RuleId == RuleId)
		{
			if (IS_KL_RULE_ID(m_vRuleList[i].m_RuleId))
			{
				m_vRuleList[i].m_RuleState = CHipsRuleItem::fDisabled;
			}
			else
			{
				m_vRuleList.remove(i, i);
			}
			return errOK;
		}
	}
	return errNOT_FOUND;
}
//-----------------------------------------------------------------------
inline void CHipsSettings::CollectAllKLAppGrID(cAppGroup & Item, cAppGroup & ParentGr, void * pContext)
{
	cVector<tDWORD> * pGrList = (cVector<tDWORD>*)pContext;
	if (IS_KL_RES_ID(Item.m_nAppGrpID))
	{
		pGrList->push_back(Item.m_nAppGrpID);
	}
	return;
}
//-----------------------------------------------------------------------
inline void CHipsSettings::EnumAppGroups(cAppGroup & RootGr, cAppGroup & ParentGr, ProcessAppGrItemFunc pFunc, void * pContext)
{
	pFunc(RootGr, ParentGr, pContext);
	for (tDWORD i = 0; i < RootGr.m_aChilds.size(); i++)
	{
		EnumAppGroups(RootGr.m_aChilds[i], RootGr, pFunc, pContext);
	}
	return;
}
//!!!
//-----------------------------------------------------------------------
//inline void CHipsSettings::CollectAllResGrID(CHipsResourceGroupsItem & Item, void * pContext)
//{
//	cVector<tDWORD> * pGrList = (cVector<tDWORD>*)pContext;
//	pGrList->push_back(Item.m_ResGroupID);
//	return;
//}
//!!!
//-----------------------------------------------------------------------
//inline void CHipsSettings::EnumResGroups(CHipsResourceGroupsItem & RootGr, ProcessResGrItemFunc pFunc, void * pContext)
//{
//	pFunc(RootGr, pContext);
//	for (tDWORD i = 0; i < RootGr.m_vChildResGroupList.size(); i++)
//	{
//		EnumResGroups(RootGr.m_vChildResGroupList[i], pFunc, pContext);
//	}
//	return;
//}
//-----------------------------------------------------------------------
inline tERROR CHipsSettings::DeleteAppGrById(tDWORD GrId)
{
	cAppGroup * pOutParentGr = 0;
	if (PR_SUCC(GetAppGrById(m_AppGroups, GrId, 0, &pOutParentGr)) && (pOutParentGr))
	{
		for (tDWORD i = 0; i < pOutParentGr->m_aChilds.size(); i++)
		{
			if (pOutParentGr->m_aChilds[i].m_nAppGrpID == GrId)
			{
				pOutParentGr->m_aChilds.remove(i, i);
				return errOK;
			}
		}
	}
	return errNOT_FOUND;
}
//-----------------------------------------------------------------------
inline void CHipsSettings::MakeConsistent()
{
	//return;
	//cVector<tDWORD> GrList;
	//!!!
	//EnumResGroups(m_ResGroups, &CHipsSettings::CollectAllResGrID, (void*)&GrList);

	//	removing resorces without real parent group
	bool IsConsistent = false;
	tDWORD i = 0;
	//!!!
	//while (i < m_vResList.size())
	//{
	//	IsConsistent = false;
	//	for (tDWORD j = 0; j < GrList.size(); j++)
	//	{
	//		if (m_vResList[i].m_ResGrID == GrList[j]) {IsConsistent = true; break;}
	//	}
	//	if (!IsConsistent) //	may be therer is no need to delete KL resources ???
	//	{
	//		//	need exclude this resource
	//		m_vResList.remove(i, i);
	//	}
	//	else i++;
	//}

	//	removing rules which connecting not consistent resources
	i = 0;
	bool IsGr = false;
	tDWORD Id = 0;
	//!!!
	cResource * pRes = 0;
	while (i < m_vRuleList.size())
	{
		pRes = m_Resource.Find(m_vRuleList[i].m_ResIdEx);
		if (!pRes)
		{
			m_vRuleList.remove(i, i);
		}
		else
		{
			i++;
		}
		//IsGr = HIPS_IS_GR_ID_EX(m_vRuleList[i].m_ResIdEx);
		//Id = HIPS_GET_ID_FROM_EX(m_vRuleList[i].m_ResIdEx);

		//if (IsGr)
		//{
		//	IsConsistent = false;
		//	for (tDWORD j = 0; j < GrList.size(); j++)
		//	{
		//		if (Id == GrList[j]) {IsConsistent = true; break;}
		//	}
		//	if (!IsConsistent)	//	may be therer is no need to delete KL rules ???
		//	{
		//		//	need exclude this rule
		//		m_vRuleList.remove(i, i);
		//	}
		//	else i++;
		//}
		//else
		//{
		//	//
		//	//	not gr
		//	//IsConsistent = false;
		//	//for (tDWORD j = 0; j < m_vResList.size(); j++)
		//	//{
		//	//	if (Id == m_vResList[j].m_ResID) {IsConsistent = true; break;}
		//	//}
		//	//if (!IsConsistent)	//	may be therer is no need to delete KL rules ???
		//	//{
		//	//	//	need exclude this rule
		//	//	m_vRuleList.remove(i, i);
		//	//}
		//	//else i++;
		//}
	}
	return;
}
//-----------------------------------------------------------------------
IMPLEMENT_SERIALIZABLE_BEGIN(CHipsSettings, "CHipsSettings")
	SER_VALUE(m_Resource,			cResource::eIID,			        "Resource")
	SER_VALUE(m_AppGroups,          cAppGroup::eIID,                    "AppGroups")
	SER_VECTOR(m_vAppList,			CHipsAppItem::eIID,					"vAppList")
	SER_VECTOR(m_vRuleList,			CHipsRuleItem::eIID,				"vRuleList")

	SER_VECTOR(m_PacketRules,	    CHipsPacketRule::eIID,              "PacketRules")

	SER_VALUE(m_FirewallSettings,   cFirewallSettings::eIID,			"FirewallSettings")

	SER_VALUE(m_FileAndRegState,	tid_DWORD,							"FileAndRegState")
	SER_VALUE(m_DevicesState,		tid_DWORD,							"DevicesState")
	SER_VALUE(m_NetworkState,		tid_DWORD,							"NetworkState")
	SER_VALUE(m_PermissionsState,	tid_DWORD,							"PermissionsState")
IMPLEMENT_SERIALIZABLE_END()

//////////////////////////////////////////////////////
//	temporary structure cHipsReportInfo - for testing 
//////////////////////////////////////////////////////
//struct cHipsReportInfo: public cSerializable
//{
//	cHipsReportInfo() : cSerializable(),
//		m_nEvent(UNKNOWN),
//		m_nStatus(STATUS_CRITICAL),
//		m_nError(errNOT_OK)
//	{
//	}
//
//	DECLARE_IID(cHipsReportInfo, cSerializable, PID_HIPS, eHipsReportInfo);
//
//	enum enStatus {
//		STATUS_UNKNOWN = 0,
//		STATUS_OK,
//		STATUS_WARNING,
//		STATUS_CRITICAL,
//	};
//
//	enum enEvent { 
//		UNKNOWN = 0,
//		ALLOW_ACTION,
//		DENY_ACTION
//	};
//
//	enEvent   m_nEvent;							//	event 
//	enStatus  m_nStatus;						//	event status
//	tERROR    m_nError;							//	error code
//};
//
//IMPLEMENT_SERIALIZABLE_BEGIN(cHipsReportInfo, "HipsReportInfo")
//	SER_VALUE_M(nEvent,			tid_DWORD)
//	SER_VALUE_M(nStatus,		tid_DWORD)
//	SER_VALUE_M(nError,			tid_ERROR)
//IMPLEMENT_SERIALIZABLE_END()

//////////////////////////////////////////////////////
//	temporary structure cHipsAskObjectAction - for testing 
//////////////////////////////////////////////////////
struct cHipsAskObjectAction : public cAskObjectAction
{
	cHipsAskObjectAction() : cAskObjectAction(),
		m_Action(evtUnk),
		m_strUserDescription(),
		m_strResGrName()
	{
		m_strUserDescription.setCP(cCP_UNICODE);
		m_strResGrName.setCP(cCP_UNICODE);
	}

	DECLARE_IID(cHipsAskObjectAction, cAskObjectAction, PID_HIPS, eHipsAskObjectAction);

	enum Actions { ASKACTION = eIID};

	enEventAction			m_Action;
	cStringObj				m_strUserDescription;
	cStringObj				m_strResGrName;
};

IMPLEMENT_SERIALIZABLE_BEGIN(cHipsAskObjectAction, "HipsAskObjectAction")
	SER_BASE(cAskObjectAction, 0)
	SER_VALUE_M(Action,			tid_DWORD)
	SER_VALUE_M(strUserDescription, tid_STRING_OBJ)
	SER_VALUE_M(strResGrName, tid_STRING_OBJ)
IMPLEMENT_SERIALIZABLE_END()

struct cHIPSStatistics: public cSerializable
{
	cHIPSStatistics(): cSerializable(),
		m_nLocResBlocked(0),
		m_nLocResAllowed(0),
		m_nPermisBlocked(0),
		m_nPermisAllowed(0),
		m_nDeviceBlocked(0),
		m_nDeviceAllowed(0)
	{}

	DECLARE_IID_STATISTICS(cHIPSStatistics, cSerializable, PID_HIPS );//, eHipsStatistics
	
	tDWORD m_nLocResBlocked;
	tDWORD m_nLocResAllowed;

	tDWORD m_nPermisBlocked;
	tDWORD m_nPermisAllowed;

	tDWORD m_nDeviceBlocked;
	tDWORD m_nDeviceAllowed;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cHIPSStatistics, 0 )
	SER_VALUE( m_nLocResBlocked, tid_DWORD, "m_nLocResBlocked")
	SER_VALUE( m_nLocResAllowed, tid_DWORD, "m_nLocResAllowed")

	SER_VALUE( m_nPermisBlocked, tid_DWORD, "m_nPermisBlocked")
	SER_VALUE( m_nPermisAllowed, tid_DWORD, "m_nPermisAllowed")

	SER_VALUE( m_nDeviceBlocked, tid_DWORD, "m_nDeviceBlocked")
	SER_VALUE( m_nDeviceAllowed, tid_DWORD, "m_nDeviceAllowed")
IMPLEMENT_SERIALIZABLE_END()


#endif // _hips_task_structs_

