// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Thursday,  19 February 2004,  15:38 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Mezhuev
// File Name   -- task.cpp
// -------------------------------------------
// AVP Prague stamp end

// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __task_cpp__3c750057_913d_4803_9b74_5192deb5944c )
#define __task_cpp__3c750057_913d_4803_9b74_5192deb5944c
// AVP Prague stamp end

// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/pr_time.h>
#include <Prague/iface/i_reg.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_threadpool.h>
#include <Prague/iface/i_token.h>
#include <Prague/iface/i_csect.h>

#include <AV/iface/i_engine.h>

#include <Extract/plugin/p_windiskio.h>

#include <OAS/iface/i_avpg2.h>
#include <OAS/plugin/p_oas.h>
#include <OAS/structs/s_oas.h>

#include <ProductCore/iface/i_processmonitor.h>
#include <ProductCore/iface/i_taskmanager.h>
// AVP Prague stamp end


//#include "../windows/hook/hook/avpgcom.h"
//#include "../windows/hook/hook/fssync.h"

struct OASImpl;

// -------------------------------------------

class CInterceptedObject : public cScanProcessInfo
{
public:
	CInterceptedObject(OASImpl *pTask) :
	  m_pTask(pTask), m_SettingsId(0), m_pSettings(&m_TreatSettings), m_hObject(NULL){}

	tERROR Prepare(hSTRING hObject);
	tERROR PreProcess();
	bool   IsIOToProcess(hIO pIO);
	bool   IsNeedPostpone();
	void   BanUserIfNeeded();

public:
	OASImpl *       m_pTask;
	hSTRING         m_hObject;

	cDetectNotifyInfo m_NotifyInfo;

	cOASSettings    m_DetectSettings;
	cOASSettings    m_TreatSettings;
	cOASSettings *  m_pSettings;
	tDWORD          m_SettingsId;

	cStringObj      m_sObjectName;
	bool            m_bIsVolume;
	bool            m_bIsDelayed;
	tQWORD          m_qwFileSize;
};

// -------------------------------------------

class CDisinfectTask : public CInterceptedObject, public cThreadTaskBase
{
public:
	CDisinfectTask(CInterceptedObject &o);
	~CDisinfectTask();

public:
	virtual void do_work();
	virtual void on_exit() { delete this; }
	virtual bool is_equal(cThreadTaskBase* pTask);

	bool   StartTask();
	tERROR ProcessFile();
	tERROR ProcessDrive();
	tERROR ProcessPhysicalDisks();
	tERROR ProcessBTDisks();

private:
	cToken *        m_hToken;
	cUserInfo       m_UserCtx;
	cUserInfo       m_OwnerCtx;
};

// -------------------------------------------

class CLockedTask : public CDisinfectTask
{
public:
	CLockedTask(CInterceptedObject &o) :
		CDisinfectTask(o), m_pNext(NULL), m_pPrev(NULL), m_nLockedCount(0){}

public:
	CLockedTask *   m_pNext;
	CLockedTask *   m_pPrev;

	tDWORD			m_nLockedCount;
};

// -------------------------------------------

struct CDriveCache
{
	cStringObj      m_sDriveName;
	tDWORD			m_nDriveType;
	tDWORD			m_nScopeId;
	CDriveCache *	m_pNext;
};

#include <ProductCore/common/userban.h>
struct cPostpone
{
	tBOOL			m_bProcessing;
	tDWORD			m_TimeCount;

	hSTRING			m_hCtx;
	cToken*			m_hToken;

	tSTRING			m_pwchDeviceName;
	cUserInfo		m_User;
	cUserInfo		m_UserCr;

	cPostpone*		m_pNext;
};

class cPostponeThread : public cThreadPoolBase
{
public:
	cPostponeThread(const char* name)
		: cThreadPoolBase( name ), m_pTask( NULL )
	{
	};

	virtual tERROR  on_idle();

public:
	OASImpl* m_pTask;
};

// -------------------------------------------

// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable OASImpl : public cTask, public cThreadPoolBase
{
private:
// Internal function declarations
	tERROR pr_call ObjectInitDone();
	tERROR pr_call ObjectPreClose();
	tERROR pr_call MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len );

// Property function declarations

public:
// External function declarations
	tERROR pr_call SetSettings( const cSerializable* p_settings );
	tERROR pr_call GetSettings( cSerializable* p_settings );
	tERROR pr_call AskAction( tActionId p_actionId, cSerializable* p_actionInfo );
	tERROR pr_call SetState( tTaskRequestState p_state );
	tERROR pr_call GetStatistics( cSerializable* p_statistics );

// Data declaration
	hREGISTRY m_hConfig;    // --
	cTaskManager* m_hTM;        // --
	tDWORD    m_nState;     // --
	tDWORD    m_nSessionId; // --
	tDWORD    m_nTaskId;    // --
// AVP Prague stamp end

	cAVPG2*				m_hAvpg;
	cAVS*				m_hAvs;
	cAVSSession*		m_avsSession;
	cProcessMonitor*	m_hProcMon;

	cOASSettings		m_Settings;
	tDWORD				m_SettingsId;

	CDriveCache*		m_DriveCache;

	CLockedTask*		m_pLockedHead;
	CLockedTask*		m_pLockedTail;		

	bool				m_bMBRProcessed;
	tDWORD				m_nNetScopeId;

	cCriticalSection*   m_lock;

	tBOOL				m_bScanRemovable;
	tBOOL				m_bScanFixed;
	tBOOL				m_bScanNetwork;

    cUserBanList		m_BanList;
    cCriticalSection*   m_BanListLock;

	cPostpone*			m_pPostpone;
	tBOOL				m_bPostponeDisabled;
	cCriticalSection*   m_PostponeLock;
	cPostponeThread		m_PostponeThread;

private:
	friend class CInterceptedObject;
	friend class CDisinfectTask;
	friend class cPostponeThread;

	OASImpl();
	void   LoadPlugins();

	tDWORD GetDriveTypeG(cStrObj& sName, hOBJECT hObject=NULL, tDWORD* pScope=NULL);
	void   ClearDriveCache(bool bReset = false);

	void   PutObjectInQueue(CInterceptedObject &o);
	void   AddLockedTask(CLockedTask *pTask);
	void   ClearLockedTasks();

	void   Lock()   { m_lock->Enter(SHARE_LEVEL_WRITE); }
	void   Unlock() { m_lock->Leave(0); }

    tERROR Process( CInterceptedObject& Object, hOBJECT hIOObject, tDWORD *pdwRetVal);

	void   SendNotification(CInterceptedObject &o);

	void   GetBanList(cUserBanList* pBanList);
	void   SetBanList(cUserBanList* pBanList);
	void   AddUserBan(cUserBanItem* pBanItem, bool bReSend);
	bool   DeleteUserBan(cUserBanItem* pBanItem, bool bReSend);

    void   ReloadBanList();

	virtual tERROR  on_idle();

	static tPROPID	m_propProceedObjectOrigin;
	static tPROPID	m_propDosDeviceName;
	static tPROPID	m_propProcessID;
	static tPROPID	m_propDelayedProp;
	static tPROPID  m_propAccessedUser;
	static tPROPID  m_propModifiedUser;

	void AddPostpone( hSTRING hObject );
	void ClearPostpone();
	void FreePostponeItem( cPostpone* pPostpone );

// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION(OASImpl)
};
// AVP Prague stamp end

// AVP Prague stamp begin( Header endif,  )
#endif // task_cpp
// AVP Prague stamp end

