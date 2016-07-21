// TaskProfile.h: interface for the CTaskProfile1 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TASKPROFILEPRODUCT_H__5188517C_F30E_4C3A_935D_4ABF9474C856__INCLUDED_)
#define AFX_TASKPROFILEPRODUCT_H__5188517C_F30E_4C3A_935D_4ABF9474C856__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TaskProfile.h"
#include <iface/i_threadpool.h>
#include <iface/i_csect.h>

//////////////////////////////////////////////////////////////////////

#define UPADTE_STAT_AUTO     0x01
#define UPADTE_STAT_RECURSE  0x02
#define UPADTE_STAT_FIXRTID  0x04

//////////////////////////////////////////////////////////////////////
// CProduct

class CProduct : public CProfile
{
public:
	DECLARE_IID( CProduct, CProfile, PID_GUI, gst_CProduct );
	
	CProfile * GetProfile(LPCTSTR strProfile);

	DWORD      GetChildrenCount() const;
	CProfile * GetChild(int idx) const;

	CProduct& operator =(const CProduct& src);

public:
	cSerObj<CProfile> m_local;
};

//////////////////////////////////////////////////////////////////////
// CGuiSubProfile

class CGuiSubProfile : public CProfile
{
public:
	CGuiSubProfile() { m_eOrigin = cProfileBase::poPrivate; }

	DECLARE_IID( CGuiSubProfile, CProfile, PID_GUI, gst_CGuiSubProfile );
	
	bool   GetSettings()                          { return true; }
	tERROR StoreSettings(bool bRecursive = false) { return errOK; }

	void   OnPrepareSettings();
	void   OnPrepareStatistics();
};

//////////////////////////////////////////////////////////////////////
// CGuiProfile

class CGuiProfile : public CProfile
{
public:
	CGuiProfile() { m_eOrigin = cProfileBase::poPrivate; }

	DECLARE_IID( CGuiProfile, CProfile, PID_GUI, gst_CGuiProfile );
	
	bool   GetSettings();
	tERROR StoreSettings(bool bRecursive = false);
	tDWORD GetCreateIID() const { return CGuiSubProfile::eIID; }

	void   OnPrepareSettings() { GetSettings(); }
};

//////////////////////////////////////////////////////////////////////
// CProfileRoot

class CProfileRoot :
	public cSerObj<CProduct>,
	public cThreadPoolBase
{
public:
	CProfileRoot(): cThreadPoolBase("GUISettStatSync") {}
	struct CProfileData
	{
		CProfileData() : m_pProfile(NULL), m_stamp(0), m_pos(-1){}

		CProfile *              m_pProfile;
		cSerObj<cSerializable>  m_data;
		cSerObj<cSerializable>  m_dataOrig;
		tLONG                   m_stamp;
		tDWORD                  m_pos;
	};

	void   Init();
	void   Start();
	void   Stop();
	void   Done();

	void   ReinitStatistics(CProfile* pProfile);
	void   NeedStatistics(CProfile* pProfile);
	void   DeleteData(CProfile* pProfile);
	void   SyncData(bool bIn);

	tERROR on_idle();

	CProfileRoot* operator &(){ return this; }

private:
	bool                         m_dirty;
	tLONG                        m_stamp;
	std::vector<CProfileData*>   m_Data;
	cSerObj<cTaskStatisticsList> m_Stat;
	cCriticalSection*            m_lock;
};

//////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_TASKPROFILEPRODUCT_H__5188517C_F30E_4C3A_935D_4ABF9474C856__INCLUDED_)
