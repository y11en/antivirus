// --------------------------------------------------------------------------------

#include <basegui.h>
#include <binding.h>
#include <GuiLoader.h>

struct BlImpl;

// --------------------------------------------------------------------------------

class cBlLoc : public CRootSink, public CGuiLoader
{
public:
	bool init(bool bPreInit = false);
	bool done();
	
	void clear_caches(bool bAll = false);
	bool send_mail(CSendMailInfo& pInfo);
	bool send_net(cDetectNotifyInfo& pInfo);
	bool localize_info(cSerializable* pInfo);
	void init_event_log(bool bSystem);
	bool report_to_system_log(enNotifySeverity nSeverity, cStrObj& strText, bool bSystem);

	bool get_mail_subject(cStrObj& strText);
	bool get_mail_header(cStrObj& strText);
	bool get_event_text(cBLNotification& pNotification, cStrObj& strText, cStrObj* strType);
	bool get_profile_name(cSerString& strText);

	tERROR svrmgr_request(cSvrMgrRequest* pRequest);

protected:
	cSerializable* GetGlobals();
	void GetLocalizationId(tString& strLocId);

	BlImpl*       m_bl;
	cCriticalSection* m_lock;

	cStrObj       m_strPath;
	tBOOL         m_bFailed;

	cStrObj       m_strMailSubject;
#if defined (_WIN32)
	HANDLE        m_hEventLog;
#elif defined (__unix__)
	void*         m_hEventLog;
#endif
	bool          m_bSystemLog;

	typedef std::pair<enNotifyIds, enNotifySeverity> tEventID;
	typedef std::map<tEventID, std::wstring> tEventNames;
	typedef std::map<std::wstring, std::wstring> tProfileNames;
	typedef std::pair<cNode *, CFieldsBinder *> tEventBinding;
	typedef std::map<tDWORD, tEventBinding> tEventBindings;
	
	tEventNames    m_mapEventNames;
	tProfileNames  m_mapProfileNames;
	tEventBindings m_mapEventBindings;
	tDWORD         m_nLastEventTime;
};

// --------------------------------------------------------------------------------
