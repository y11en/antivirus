// -----------------------------------------------------------------------------------------

#define PR_IMPEX_DEF

#include <windows.h>
#include <stdio.h>
#include <prague.h>
#include <pr_loadr.h>
#include <structs\s_avs.h>
#include <structs\s_scaner.h>

hROOT g_root = NULL;

#define IMPEX_TABLE_CONTENT
IMPORT_TABLE(import_table)
	#include <iface/e_loader.h> 
IMPORT_TABLE_END

// -----------------------------------------------------------------------------------------

class CAVSScanner : public CPrague
{
public:
	CAVSScanner() : m_hAVS(NULL), m_hSession(NULL), m_hReciever(NULL){}
	~CAVSScanner(){ Done(); }

	bool Init();
	bool Process(const char* m_sFileToCheck);
	bool Done();

private:

	tERROR OnEvent(tDWORD nMsgClass, cDetectObjectInfo* pData);

	static tERROR MsgReceive(hOBJECT _this, tDWORD msg_cls,
		tDWORD msg_id, hOBJECT obj, hOBJECT ctx, hOBJECT receive_point, tPTR pbuff, tDWORD* blen)
	{
		CAVSScanner *pThis = (CAVSScanner*)_this->propGetPtr(pgRECEIVE_CLIENT_ID);
		return pbuff ? pThis->OnEvent(msg_cls, (cDetectObjectInfo*)pbuff) : errOK;
	}

private:
	cAVS*           m_hAVS;
	cAVSSession*    m_hSession;
	cObject*        m_hReciever;
};

// -----------------------------------------------------------------------------------------

bool CAVSScanner::Init()
{
	if( PR_FAIL(CPrague::Init(GetModuleHandle(NULL), PR_LOADER_FLAG_FORCE_PLUGIN_SEARCH)) )
	{
		puts("Cannot init prague");
		return false;
	}

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
		if( PR_FAIL(error) )
			puts("AVS SetSettings error");
	}

	if( PR_SUCC(error) )
		error = m_hAVS->sysCreateObject((hOBJECT*)&m_hReciever, IID_MSG_RECEIVER);
	if( PR_SUCC(error) )
		error = m_hReciever->propSetPtr(pgRECEIVE_PROCEDURE, MsgReceive);
	if( PR_SUCC(error) )
		error = m_hReciever->propSetPtr(pgRECEIVE_CLIENT_ID, this);
	if( PR_SUCC(error) )
		error = m_hReciever->sysCreateObjectDone();

	if( PR_SUCC(error) )
		error = m_hAVS->CreateSession(&m_hSession, (cTask*)m_hReciever, cFALSE, OBJECT_ORIGIN_GENERIC);
	if( PR_SUCC(error) )
	{
		error = m_hSession->SetState(TASK_REQUEST_RUN);
		if( PR_FAIL(error) )
			puts("AVSSession SetState error");
	}

	if( PR_SUCC(error) )
	{
		tMsgHandlerDescr hdl[] = {
			{ m_hReciever, rmhDECIDER, pmc_EVENTS_NOTIFY, IID_ANY, PID_ANY, IID_ANY, PID_ANY },
			{ m_hReciever, rmhDECIDER, pmc_EVENTS_IMPORTANT, IID_ANY, PID_ANY, IID_ANY, PID_ANY },
			{ m_hReciever, rmhDECIDER, pmc_EVENTS_CRITICAL, IID_ANY, PID_ANY, IID_ANY, PID_ANY },
		};
		error = m_hAVS->sysRegisterMsgHandlerList(hdl, countof(hdl));
	}

	return PR_SUCC(error);
}

bool CAVSScanner::Done()
{
	if( m_hSession )
		m_hSession->sysCloseObject(), m_hSession = NULL;

	if( m_hAVS )
		m_hAVS->sysCloseObject(), m_hAVS = NULL;

	CPrague::Deinit();
	return true;
}

tERROR CAVSScanner::OnEvent(tDWORD nMsgClass, cDetectObjectInfo* pData)
{
	const char* strFormat;
	switch(pData->m_nObjectStatus)
	{
	case OBJSTATUS_OK:        strFormat = "object <%S> ok"; break;
	case OBJSTATUS_INFECTED:  strFormat = "object <%S> infected with virus %tstr"; break;
	default: return errOK;
	}

	cStringObj strMessage;
	strMessage.format(cCP_ANSI, strFormat, pData->m_strObjectName.data(), pData->m_strDetectName.data());
	puts((char*)strMessage.c_str(cCP_ANSI));

	if( 0 ) // if need to cancel processing
		return errOPERATION_CANCELED;

	return errOK;
}

bool CAVSScanner::Process(const char* m_sFileToCheck)
{
	cIOObj hIO((cObj*)m_hAVS, cAutoString(cStrObj(m_sFileToCheck)), fACCESS_READ, fOMODE_OPEN_IF_EXIST);
	if( PR_FAIL(hIO.last_error()) )
	{
		puts("Cannot open file");
		return errOK;
	}

	cScanProcessInfo  pScanInfo;
	cDetectObjectInfo pDetectInfo;
	cProtectionSettings pSettings;
	pSettings.m_nScanAction = SCAN_ACTION_DETECT;
	tERROR error = m_hSession->ProcessObject(hIO, &pScanInfo, &pSettings, &pDetectInfo);

	if( pScanInfo.m_nProcessStatusMask & cScanProcessInfo::DETECTED ) // was detection
	{
		cStringObj strMessage;
		strMessage.format(cCP_ANSI, "File is infected with virus %S", pDetectInfo.m_strDetectName.data());
		puts((char*)strMessage.c_str(cCP_ANSI));
	}
	return true;
}

// -----------------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
	if( argc < 2 )
	{
		puts("No file");
		return 0;
	}

	CAVSScanner pScanner;
	if( pScanner.Init() )
		pScanner.Process(argv[1]);
 	return 0;
}

// -----------------------------------------------------------------------------------------
