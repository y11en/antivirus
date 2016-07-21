// TaskView.h: interface for the CTaskView class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TASKVIEW_H__C7D32A70_AE04_4467_88A7_23A2912BE79C__INCLUDED_)
#define AFX_TASKVIEW_H__C7D32A70_AE04_4467_88A7_23A2912BE79C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TaskProfile.h"
#include "ListsView.h"
#include <plugin/p_reportdb.h>

#define EVENTID_SETTINGS_UPDATE        0x51648943
#define EVENTID_SETTINGS_LOADED        0x0a222235

#define IS_TASK_IN_PROGRESS( _state ) ((_state) & STATE_FLAG_ACTIVE )

class CProfile;

//////////////////////////////////////////////////////////////////////
// CSettingsDlg

class CSettingsDlg : public CDialogBindingT<cSerializable>
{
protected:
	typedef CDialogBindingT<cSerializable> TBase;
	using TBase::m_nActionsMask;
	using TBase::m_ser;
	using TBase::m_pStruct;

public:
	CSettingsDlg(CSinkCreateCtx * pCtx);
	~CSettingsDlg();

	void SetDirty() { m_bModified = 2; }

protected:
	bool  OnOk();
	bool  OnClose(tDWORD& nResult);

	CItemBase * m_pContextItem;
	unsigned    m_bContextIsList : 1;
	unsigned    m_bModified : 2;
};

//////////////////////////////////////////////////////////////////////
// CTaskProfileView

class CTaskProfileView : public CDialogSink
{
public:
	enum eFlags {fStat = 0x01, fEdit = 0x02};
	
	CTaskProfileView(tDWORD nFlags, CSinkCreateCtx * pCtx);
	CTaskProfileView(CProfile * pParentProfiles, tDWORD nFlags = 0, LPCSTR strDefProfile = NULL);
	
	tERROR SaveSettings();
	void RefreshStatistics(bool bFresh = true);
	
	static LPCSTR GetProfileName(tString &strProfileName, CSinkCreateCtx * pCtx, size_t nParamNum);
	static bool   AddProfileSource(bool bStat, CFieldsBinder * pBinding, CProfile * pProfile, LPCTSTR strSect = NULL);
	
	virtual void OnCustomizeData(cSerializable * pData, bool bIn);

protected:
	void OnCreate();
	void OnInit();
	void OnInitProps(CItemProps& pProps);
	void OnTimerRefresh(tDWORD nTimerSpin);
	void OnEvent(tDWORD nEventId, cSerializable* pData);
	void OnChangedData(CItemBase * pItem);
	bool OnAddDataSection(LPCTSTR strSect);
	bool OnApply();

protected:
	SINK_MAP_BEGIN()
		SINK_DYNAMIC("taskreport", CReportSink(pItem, m_pProfile))
	SINK_MAP_END(CDialogSink)

	cNode * CreateOperator(const char *name, cNode **args, cAlloc *al);

	class _Data : public CProfile
	{
	public:
		CProfile * GetProfile(LPCSTR strProfile);
		tERROR SaveSettings();
	}; friend class _Data;

	struct CReportSink : public CListItemSink
	{
		CReportSink(CItemBase* pList, CProfile * pProfile);

		bool GetInfo(cDataInfo& info);
		cSerializable* GetRowData(tIdx nItem);

	private:
		cAutoObj<cReport> m_pReport;
		cSerializableObj  m_Data;
	};

	CProfile *        m_pProfile;
	CProfile *        m_pParentProfiles;
	tDWORD            m_nFlags;
	CItemBase *       m_pContextItem;
	_Data             m_data;
	
	MEMBER_NODE(cNodeSaveSettings) m_nodeSaveSettings;
	unsigned                       m_saveSettingsUsed : 1;
};

//////////////////////////////////////////////////////////////////////
// CReportDbView

#define CACHE_EVENTS_SIZE 1024

class CReportDbView : public CListSinkMaster
{
public:
	CReportDbView(tDWORD nSerId, tDWORD nDataSize);
	~CReportDbView();
	
	tDWORD OnBindingFlags() { return ITEMBIND_F_VALUE; }
	tTYPE_ID OnGetType() { return tTYPE_ID(m_Data ? m_Data->getIID() : cSer::eIID); }
	void OnInitProps(CItemProps& pProps);
	void OnChangedData(CItemBase * pItem) { CListItemSink::OnChangedData(pItem); }
	
	bool GetInfo(cDataInfo& info);
	cSerializable* GetRowData(tIdx nItem);
	void PrepareRows(tIdx nFrom, tDWORD nCount);

	virtual void OnCustomizeData(cSer * dst, tBYTE * src) = 0;

protected:
	cReportDB*       m_pReport;
	tDWORD           m_eDbType;
	cSerializableObj m_Data;
	tDWORD           m_nDataSize;
	cVector<tBYTE>   m_Cache;
	tIdx             m_Idx[CACHE_EVENTS_SIZE];
	tDWORD           m_nCurPos;
};

//////////////////////////////////////////////////////////////////////
// CThreatsList

class CThreatsList : public CListItemSink
{
public:
	CThreatsList();
	~CThreatsList();

	void DiscardItems(bool bAll);
	void TreatItems(bool bAll, bool bDelete);
	void CollectSelected(cDwordVector &aScanRecords);
	void RestoreItems(bool bAll);
	void AddToQuarantine();

protected:
	void OnInitProps(CItemProps& pProps);
	cNode * CreateOperator(const char *name, cNode **args, cAlloc *al);

	bool GetInfo(cDataInfo& info);
	cSerializable* GetRowData(tIdx nItem);

private:
	cAVSTreats *        m_pTreats;
	cInfectedObjectInfo m_cache;
};

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_TASKVIEW_H__C7D32A70_AE04_4467_88A7_23A2912BE79C__INCLUDED_)
