// -----------------------------------------------------------------------------------------

#include <windows.h>
#include <stdio.h>
#include <prague.h>
#include <pr_loadr.h>
#include <structs\s_avs.h>
#include <structs\s_scaner.h>

hROOT g_root = NULL;

// -----------------------------------------------------------------------------------------

class CODSScaner : public CPrague
{
public:
	CODSScaner() : m_hDoneEvent(NULL), m_hAVS(NULL), m_hODS(NULL), m_hReciever(NULL){}
	~CODSScaner(){ Done(); };

	bool Init();
	bool Process(const char* sPath);
	bool Done();

private:
	bool CreateAVS();
	bool CreateODS();
	void OutStatistics();

	void OnEvent(tDWORD nMsgClass, cDetectObjectInfo* pData);
	void OnStateChanged(tTaskState nState);

	static tERROR MsgReceive(hOBJECT _this, tDWORD msg_cls,
		tDWORD msg_id, hOBJECT obj, hOBJECT ctx, hOBJECT receive_point, tPTR pbuff, tDWORD* blen)
	{
		CODSScaner *pThis = (CODSScaner*)_this->propGetPtr(pgRECEIVE_CLIENT_ID);
		if( msg_cls == pmc_TASK_STATE_CHANGED )
			pThis->OnStateChanged((tTaskState)msg_id);
		else if( pbuff )
			pThis->OnEvent(msg_cls, (cDetectObjectInfo*)pbuff);
		return errOK;
	}

private:
	HANDLE          m_hDoneEvent;
	cAVS*           m_hAVS;
	cTask*          m_hODS;
	cObject*        m_hReciever;
};

// -----------------------------------------------------------------------------------------

bool CODSScaner::Init()
{
	m_hDoneEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	if( PR_FAIL(CPrague::Init(GetModuleHandle(NULL), PR_LOADER_FLAG_FORCE_PLUGIN_SEARCH)) )
	{
		puts("Cannot init prague");
		return false;
	}

	if( !CreateAVS() )
	{
		puts("Cannot create AVS");
		return false;
	}

	if( !CreateODS() )
	{
		puts("Cannot create ODS");
		return false;
	}

	return true;
}

bool CODSScaner::Done()
{
	if( m_hDoneEvent )
		CloseHandle(m_hDoneEvent);

	if( m_hODS )
		m_hODS->sysCloseObject(), m_hAVS = NULL;

	if( m_hAVS )
		m_hAVS->sysCloseObject(), m_hAVS = NULL;

	CPrague::Deinit();
	return true;
}

bool CODSScaner::CreateAVS()
{
	tERROR error = g_root->sysCreateObjectQuick((hOBJECT*)&m_hAVS, IID_AVS, PID_AVS);
	if( PR_SUCC(error) )
	{
		tCHAR strPath[MAX_PATH];

		::GetModuleFileNameA(NULL, strPath, sizeof(strPath));
		tCHAR* pName = strrchr(strPath, '\\');
		if( pName )
			strcpy(++pName, "bases");

		cAVSSettings pSettings;
		pSettings.m_sBaseFolder = strPath;
		pSettings.m_sDataFolder = strPath; // folder for sfdb
		pSettings.m_sSetName = "avp.set";
		pSettings.m_nStackSizeToCheck = 10;
		error = m_hAVS->SetSettings(&pSettings);
	}

	return PR_SUCC(error);
}

bool CODSScaner::CreateODS()
{
	tERROR error = g_root->sysCreateObject((hOBJECT*)&m_hODS, IID_TASK, PID_ODS);

	if( PR_SUCC(error) )
		error = m_hODS->propSetObj(plAVS, (cObject*)m_hAVS);

	if( PR_SUCC(error) )
		error = m_hODS->sysCreateObjectDone();

	return PR_SUCC(error);
}

void CODSScaner::OnEvent(tDWORD nMsgClass, cDetectObjectInfo* pData)
{
	const char* strFormat;
	switch(pData->m_nObjectStatus)
	{
	case OBJSTATUS_OK:        strFormat = "object <%S> ok"; break;
	case OBJSTATUS_INFECTED:  strFormat = "object <%S> infected with virus %S"; break;
	default: return;
	}
	cStringObj strMessage;
	strMessage.format(cCP_ANSI, strFormat, pData->m_strObjectName.data(), pData->m_strDetectName.data());
	puts((char*)strMessage.c_str(cCP_ANSI));
}

void CODSScaner::OnStateChanged(tTaskState nState)
{
	switch(nState)
	{
	case TASK_STATE_STOPPED:
	case TASK_STATE_COMPLETED:
	case TASK_STATE_FAILED: break;
	default: return;
	}
	SetEvent(m_hDoneEvent);
}

void CODSScaner::OutStatistics()
{
	cODSStatistics pStatistics;
	m_hODS->GetStatistics(&pStatistics);

	cStringObj strMessage;
	strMessage.format(cCP_ANSI, "Number of scaned objects %d", pStatistics.m_nNumScaned);
	puts((char*)strMessage.c_str(cCP_ANSI));

	strMessage.format(cCP_ANSI, "Number of detected threats %d", pStatistics.m_nNumThreats);
	puts((char*)strMessage.c_str(cCP_ANSI));
}

inline void AddScanObject(cODSSettings& pSettings, enScanObjType nType, const tCHAR* strName = NULL, bool bRecursive = true)
{
	cScanObject pScanObject;
	pScanObject.m_nObjType = nType;
	pScanObject.m_strObjName = strName;
	pScanObject.m_bRecursive = bRecursive;

	pSettings.m_aScanObjects.push_back(pScanObject);
}

bool CODSScaner::Process(const char* sPath)
{
	cODSSettings pSettings;
	pSettings.m_nScanFilter = SCAN_FILTER_ALL_OBJECTS;
	pSettings.m_bCountProgress = false;

	//AddScanObject(pSettings, OBJECT_TYPE_MEMORY);
	// AddScanObject(pSettings, OBJECT_TYPE_STARTUP);
	AddScanObject(pSettings, OBJECT_TYPE_FOLDER, sPath, false); // for test

	tERROR error = m_hODS->SetSettings(&pSettings);

	if( PR_SUCC(error) )
		error = m_hODS->sysCreateObject((hOBJECT*)&m_hReciever, IID_MSG_RECEIVER);
	if( PR_SUCC(error) )
		error = m_hReciever->propSetPtr(pgRECEIVE_PROCEDURE, MsgReceive);
	if( PR_SUCC(error) )
		error = m_hReciever->propSetPtr(pgRECEIVE_CLIENT_ID, this);
	if( PR_SUCC(error) )
		error = m_hReciever->sysCreateObjectDone();

	tMsgHandlerDescr hdl[] = {
		{ m_hReciever, rmhLISTENER, pmc_TASK_STATE_CHANGED, IID_ANY, PID_ANY, IID_ANY, PID_ANY },
		{ m_hReciever, rmhLISTENER, pmc_EVENTS_NOTIFY, IID_ANY, PID_ANY, IID_ANY, PID_ANY },
		{ m_hReciever, rmhLISTENER, pmc_EVENTS_IMPORTANT, IID_ANY, PID_ANY, IID_ANY, PID_ANY },
		{ m_hReciever, rmhLISTENER, pmc_EVENTS_CRITICAL, IID_ANY, PID_ANY, IID_ANY, PID_ANY },
	};
	if( PR_SUCC(error) )
		error = m_hODS->sysRegisterMsgHandlerList(hdl, countof(hdl));

	if( PR_SUCC(error) )
	{
		error = m_hODS->SetState(TASK_REQUEST_RUN);
		if( PR_FAIL(error) )
			puts("Cannot start ods");
	}
	
	if( PR_SUCC(error) )
	{
		WaitForSingleObject(m_hDoneEvent, INFINITE);
		OutStatistics();
	}
	return true;
}

// -----------------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
	if( argc < 2 )
	{
		puts("No dir");
		return 0;
	}

	CODSScaner pScanner;
	if( pScanner.Init() )
		pScanner.Process(argv[1]);
 	return 0;
}

// -----------------------------------------------------------------------------------------


