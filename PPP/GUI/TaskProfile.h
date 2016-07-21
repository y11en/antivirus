// TaskProfile.h: interface for the CTaskProfile1 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TASKPROFILE_H__5188517C_F30E_4C3A_935D_4ABF9474C856__INCLUDED_)
#define AFX_TASKPROFILE_H__5188517C_F30E_4C3A_935D_4ABF9474C856__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_PROFILE_ODS_COUNT     4
#define MAX_PROFILE_UPD_COUNT     2

class CProfileRoot;

//////////////////////////////////////////////////////////////////////
// CProfile

#include "pid_gui_fix.h"

class CProfile : public cProfileEx
{
public:
	DECLARE_IID( CProfile, cProfileEx, PID_GUI, gst_CProfile );
	
	CProfile();
	~CProfile();
	
	virtual tDWORD GetCreateIID() const { return eIID; }

	bool           Enable(bool bEnable);
	bool           SetState(tTaskRequestState nState, bool bPrevId = false, tERROR * pErr = NULL);
	tERROR         SetResumeTime(tDWORD nResumeTime);
	virtual bool   GetInfo(cSerializable* pInfo, tGetProfileInfoSource eSrc = gpi_FROM_CURRENT_STATE/*, bool bFixRtId = false*/);
	tERROR         SetInfo(cSerializable* pInfo);
	tERROR         AskAction(tDWORD action_id, cSerializable* pInfo);

	tERROR         SendMsg(tDWORD nMsgClass, tDWORD nMsgId, cSerializable* pData, bool fAsync=true);

	bool           isSettingsDirty() const { return !!m_bSettingsDirty; }
	bool           IsEnableDirtyOff(bool bRecursive = false);
	void           SetSettingsDirty(bool bDirty = true, bool bRecursive = false);
	virtual bool   GetSettings();
	virtual tERROR StoreSettings(bool bRecursive = false);
	bool           GetSettingsByLevelTo(tDWORD lev, cSerializable * pSettings);
	bool           GetSettingsByLevel(tDWORD lev, bool bRecursive = false);
	void           ResetSettingsLevelTo(tDWORD lev, cCfg& _Cfg);
	bool           ResetSettingsLevel(tDWORD lev, bool bRecursive = false);
	bool           ApplyToAllScanSettings(CProfile* pProfile);
	tERROR         SaveSettings(bool bRecursive = false);
	void           PopSettings();
		           
	bool           Update(bool bFull = false);
	void           UpdateState(cProfileBase* pInfo);
	void           UpdateStatistics(/*bool bFixRtId = false, */bool bFresh = false);
	void           UpdateSchedulTime(bool bRecursive = true);

	tDWORD         GetUserCount(LPCSTR strType = NULL);
	bool           IsProtectionTask();
	bool           IsProfilesLimit(LPCSTR strType = NULL);
	bool           IsUser() { return m_eOrigin == poUser; }
	bool           IsAdmin() { return m_eOrigin == poRemoteAdmin || m_eOrigin == poLocalAdmin; }
	bool           IsVisibleOnGui() { return IsUser() || (IsAdmin() && !(m_nAdmFlags & PROFILE_ADMFLAG_GROUPTASK)); }
	bool           IsScan(bool& bUserProfile);
	bool           IsScanPredefined();
	bool           IsScanProfile(LPCSTR strProfile, bool& bUserProfile);
	bool           IsTaskExist(LPCSTR strTaskType);

	bool           IsSettExist();
	bool           IsStatExist();
	
	CProfile *     GetProfileByName(LPCSTR strName, LPCSTR strType);
	virtual CProfile * GetProfile(LPCSTR strProfile);
	CProfile *     GetProfileByType(LPCSTR strType);
	CProfile *     GetRootProfile();
	
	CProfile *     AddProfile(cProfileBase * pProfile = NULL, tDWORD nClassId = cSerializable::eIID);
	void           DelProfile(LPCSTR strProfile);
	void           UpdateProfile(tDWORD nEventId, cSerializable * pData, /*bool bFixRtId = false, */bool bAddDel = true);
	
	virtual DWORD  GetChildrenCount() const { return m_aChildren.size(); }
	CProfile * GetChild(LPCSTR strProfile) const;
	virtual CProfile * GetChild(int idx) const { return (CProfile *)(const cSerializable *)m_aChildren.operator[](idx); }

	void           Customize(LPCSTR strName, tDWORD nSettingsId, tDWORD nStatisticsId);
	bool           IsCustom() { return m_bCustom; }

	CProfile&  operator =(const CProfile& src);

public:
	bool           Update(const cProfileEx * pProfile);
	bool           Assign(const cProfile& _Data);
	bool           Assign(const cCfg& _Data);
	bool           AssignEx(CProfile * pProfile, bool bRecursive);
	bool           Copy(const CProfile * pProfile, bool bFull = false, bool bRecursive = true);
	bool           CopyFrom(const cProfileEx& _Data, bool bAddUserProfilesOnly = false, CProfile* pTopProfile = NULL);
	bool           CopyTo(cProfileEx &oDest, bool bRecursive = true);
	bool           ApplyPolicy();
	bool           GetData();
	void           Disconnect();

public:
	cTaskSettings*   settings(LPCSTR strProfile = NULL, tDWORD nSettingsId = cSerializable::eIID);
	cSerializable*   settings_user(LPCSTR strProfile = NULL, tDWORD nSettingsId = cSerializable::eIID);
	cTaskStatistics* statistics(LPCSTR strProfile = NULL, tDWORD nStatisticsId = cSerializable::eIID);
	void             ClearUnusedData();

	static cSerializable* GetUserSettings(cSerializable * pSettings, tDWORD nSettingsId = cSerializable::eIID);
	static bool           IsCfgEqual(cCfg& _cfg1, cCfg& _cfg2);

protected:
	virtual void   OnPrepareSettings();
	virtual void   OnPrepareStatistics();

public:
	CProfileRoot *    m_pRoot;
	CProfile *        m_pOriginal;
	CProfile *        m_pParent;

private:
	friend class CProfileRoot;

	tPTR           m_pData;
	unsigned       m_bSettingsDirty : 1;
	unsigned       m_bOriginalInited : 1;
	unsigned       m_bSettingsExist : 2;
	unsigned       m_bStatisticsExist : 2;
	unsigned       m_bCustom : 1;
};

//////////////////////////////////////////////////////////////////////
// CTaskProfile

class CTaskProfile : public CProfile
{
public:
	DECLARE_IID( CTaskProfile, CProfile, PID_GUI, gst_CTaskProfile );

	virtual tDWORD GetCreateIID() const { return eIID; }

	bool           GetInfo(cSerializable* pInfo, tGetProfileInfoSource eSrc = gpi_FROM_CURRENT_STATE/*, bool bFixRtId = false*/);
	tERROR         SetInfo(cSerializable* pInfo);

	void           KeepTempOn();
};

//////////////////////////////////////////////////////////////////////

extern cTaskManager * g_hTM;

//////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_TASKPROFILE_H__5188517C_F30E_4C3A_935D_4ABF9474C856__INCLUDED_)
