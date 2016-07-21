// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Wednesday,  13 June 2007,  18:44 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Denisov
// File Name   -- task.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define Task_VERSION ((tVERSION)1)
// AVP Prague stamp end

#define PR_IMPEX_DEF

// AVP Prague stamp begin( Header includes,  )
#include <prague.h>
#include <iface/i_root.h>
//#include <pr_vtbl.h>
#include <iface/i_reg.h>
#include <plugin/p_avzscan.h>
// AVP Prague stamp end



#include <windows.h>
#include <plugin/p_win32loader.h>
#include <plugin/p_win32_nfio.h>
#include <iface/i_csect.h>
#include <iface/i_token.h>
#include <structs/s_avzscan.h>
#include <structs/s_avs.h>
#include <structs/s_scaner.h>
#include <pr_time.h>
#include <avzkrnl_dll.h>
#include <guiloader.h>
#include "ThreadImp.h"
#include "stream.cpp"
#include <map>
#include <string>

typedef std::map<tTYPE_ID, std::wstring> pr_type_map_t;
tERROR DeserializeXML( 
	IN CAVZScanKernel* pAVZAScanKernel,    // Контекст ядра AVZ
	IN IStream* pXMLStream,                // Стрим с XML
	IN wchar_t* szTagName,                 // Имя тега первого уровня, чье содержимое будет преобразовано в структуру
	IN tDWORD dwXMLSerId,                  // Идентификатор сериализатора 
	IN pr_type_map_t& pr_type_map,         // Таблица соответствия объявленных типов XML и пражских типов
	IN OUT cXMLRecords* paXMLRecords       // Вектор с результатами сканирования
	);

class CStat
{
public:
	CStat(hOBJECT hParent): m_cs(NULL), m_hNeedStop(NEED_RUN) 
	{ cERROR error = hParent->sysCreateObjectQuick((hOBJECT*)&m_cs, IID_CRITICAL_SECTION, PID_WIN32LOADER_2LEVEL_CSECT);};
	~CStat()
	{ if (m_cs) m_cs->sysCloseObject(); m_cs = NULL;};
	tERROR Set(IN tDWORD dwProgressCur, IN tDWORD dwProgressMax, wchar_t* szMsg, tDWORD dwMsgSize, tBOOL bNeedReboot = cFALSE)
	{
		cAutoCS cs(m_cs, true);
		if ( dwProgressMax )
		{
			tDWORD nPercentCompleted = (dwProgressMax && dwProgressCur) ? (100 * dwProgressCur) / dwProgressMax : 0;
			if ( nPercentCompleted > m_stat.m_nPercentCompleted )
				m_stat.m_nPercentCompleted = nPercentCompleted;
		}
		if ( szMsg && dwMsgSize ) 
			m_stat.m_szLastEvent = szMsg;
		m_stat.m_bNeedReboot |= bNeedReboot;
		return errOK;
	};
	tERROR Get(OUT cSerializable* p_statistics)
	{
		if ( p_statistics )
		{
			cAutoCS cs(m_cs, false);
			return p_statistics->assign( m_stat, false );
		}
		else
			return errPARAMETER_INVALID;
	};
	enum enumStop {NEED_RUN, NEED_STOP, NEED_PAUSE};
	void SetNeedStop(enumStop hNeedStop)
	{
		cAutoCS cs(m_cs, true);
		m_hNeedStop = hNeedStop;
	}
	enumStop GetNeedStop()
	{
		cAutoCS cs(m_cs, false);
		return m_hNeedStop;
	}
	const hCRITICAL_SECTION& GetCS() {return m_cs;};
	cAVZScanStatistics* GetStat(){return &m_stat;};
private:
	hCRITICAL_SECTION m_cs;
	cAVZScanStatistics m_stat;
	enumStop m_hNeedStop;
};

// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable CAVZScan : public cTask {
private:
// Internal function declarations
	tERROR pr_call ObjectInitDone();
	tERROR pr_call ObjectPreClose();

// Property function declarations

public:
// External function declarations
	tERROR pr_call SetSettings( const cSerializable* p_settings );
	tERROR pr_call GetSettings( cSerializable* p_settings );
	tERROR pr_call AskAction( tActionId p_actionId, cSerializable* p_actionInfo );
	tERROR pr_call SetState( tTaskRequestState p_state );
	tERROR pr_call GetStatistics( cSerializable* p_statistics );

// Data declaration
	tDWORD           m_state;      // --
	hTASKMANAGER     m_tm;         // --
	tDWORD           m_session_id; // --
	cAVZScanSettings m_settings;   // --
	tDWORD           m_task_id;    // --
	cRegistry*       m_storage;    // --
// AVP Prague stamp end

private:
	tERROR FileToStr(cStringObj szFileName, cStringObj& szStr);
	AVZERROR RunCustomScript();

private:
	CAVZScanKernel* m_pAVZAScanKernel;
	hIO m_hXML;

private:
	pr_type_map_t m_pr_type_map;

public:
	CStat* m_statistics;

	class CStateManager : public CThreadImp
	{
	public:
		CStateManager(CAVZScan* pTask) : m_pTask(pTask), m_hToken(NULL)  
		{
			m_pTask->sysCreateObjectQuick((hOBJECT*)&m_hToken, IID_TOKEN);
		}
		virtual ~CStateManager()
		{
			if( m_hToken )
				m_hToken->sysCloseObject();
		};
		virtual tERROR _Run()
		{
			cERROR err = errOK;
			if( m_hToken )
				m_hToken->Impersonate();
			do 
			{
				err = m_pTask->SetStateEx((tTaskRequestState)m_p_state);
			} 
			while( 
				err == errTASK_CANNOT_GO_TO_STATE &&
				WAIT_TIMEOUT == WaitForSingleObject(m_hStopEvent, 100)
				);
			if( m_hToken )
				m_hToken->Revert();
			return errOK;
		}
		tERROR SetState(tDWORD p_state)   
		{ 
			m_p_state = p_state;   
			return Start(); 
		};
	private:
		CAVZScan* m_pTask;
		tDWORD m_p_state;
		cToken* m_hToken;
	};
	CStateManager* m_pStateManager;
public:
	tERROR SetStateEx( tTaskRequestState p_state );

// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION(CAVZScan)
};
// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "Task". Static(shared) property table variables
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR CAVZScan::ObjectInitDone() {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::ObjectInitDone method" );

	m_pr_type_map[tid_BYTE]       = L"ftByte,0; ";
	m_pr_type_map[tid_WORD]       = L"ftWord,0; ";
	m_pr_type_map[tid_DWORD]      = L"ftDWORD,0; ";
	m_pr_type_map[tid_INT]        = L"ftInteger,0; ";
	m_pr_type_map[tid_STRING_OBJ] = L"ftPWChar,; ";

	// Place your code here
	m_state = TASK_STATE_STOPPED;
	m_pStateManager = NULL;
	m_pAVZAScanKernel = NULL;
	m_statistics = NULL;
	m_hXML = NULL;
	if ( PR_SUCC(error) )
	{
		error = sysCreateObjectQuick( (hOBJECT*)&m_hXML, IID_IO, PID_TMPFILE);
		//////////////////////////////////////////////////////////////////////////
		//	код для тестирования парсинга XML
		// 	error = sysCreateObject( (hOBJECT*)&m_hXML, IID_IO, PID_NATIVE_FIO);
		// 	error = m_hXML->set_pgOBJECT_FULL_NAME("O:\\AVPTool\\out_Win32\\Debug\\LOG\\virusinfo_syscheck.xml", 0, cCP_ANSI);
		// 	error = m_hXML->sysCreateObjectDone();
		//////////////////////////////////////////////////////////////////////////
	}
	if ( !LoadAVZKrnlDll() )
		return error = errMODULE_CANNOT_BE_LOADED;
	m_statistics = new CStat((hOBJECT)this);
	if ( !m_statistics )
		return error = errOBJECT_CANNOT_BE_INITIALIZED;

	if ( PR_SUCC(error) )
	{
		m_pStateManager = new CStateManager(this);
		if ( !m_pStateManager )
			error = errOBJECT_CANNOT_BE_INITIALIZED;
	}

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:
tERROR CAVZScan::ObjectPreClose() {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::ObjectPreClose method" );

	// Place your code here
	if ( m_pStateManager )
	{
		m_pStateManager->Stop();
		delete m_pStateManager;
		m_pStateManager = NULL;
	}
	if ( m_pAVZAScanKernel )
	{
		pAVZ_Done(m_pAVZAScanKernel);
		m_pAVZAScanKernel = NULL;
	}
	if ( m_statistics ) 
	{
		delete m_statistics;
		m_statistics = NULL;
	}
	if ( m_hXML )
	{
		m_hXML->sysCloseObject();
		m_hXML = NULL;
	}

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetSettings )
// Parameters are:
tERROR CAVZScan::SetSettings( const cSerializable* p_settings ) {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::SetSettings method" );

	// Place your code here
	error = m_settings.assign(*p_settings, true);

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetSettings )
// Parameters are:
tERROR CAVZScan::GetSettings( cSerializable* p_settings ) {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::GetSettings method" );

	// Place your code here
	if ( p_settings )
		error = p_settings->assign( m_settings, false );
	else
		error = errPARAMETER_INVALID;

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, AskAction )
// Parameters are:
tERROR CAVZScan::AskAction( tActionId p_actionId, cSerializable* p_actionInfo ) {
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "Task::AskAction method" );

	// Place your code here

	return error;
}
// AVP Prague stamp end

AVZERROR WINAPI GetLocalizedStr( IN void* pTaskContext, IN char* szID, OUT wchar_t** pszResult )
{
	if ( !szID || !pszResult || *pszResult )
		return AVZ_Error;

	CRootItem* pGuiRoot = CGuiLoader::Instance().CreateAndInitRoot(g_root);
	if ( !pGuiRoot )
		return AVZ_Error;

	if ( strchr(szID, '%') )
	{
		if ( !g_root )
			return AVZ_Error;
		cStringObj l_szID = szID;
		g_root->sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hOBJECT)cAutoString(l_szID), 0, 0);
		return AVZ_OK;
	}
	else
	{
#if _BASEGUI_VER >= 0x0200
		LPCWSTR lszResult = NULL;
		tString utf8;  
		cStrObj utf16; 
		if ( pGuiRoot->GetString(utf8, PROFILE_LOCALIZE, "Global", szID) )
		{
			remove_quotes(utf8);
			lszResult = pGuiRoot->LocalizeStr(utf16, utf8.c_str(), utf8.size());
		}
#else
		LPCWSTR lszResult = pGuiRoot->GetStringBufW(PROFILE_LOCALIZE, "Global", szID);
#endif //_BASEGUI_VER >= 0x0200
		if ( !lszResult || !wcslen(lszResult) )
			return AVZ_Error;

		*pszResult = (wchar_t*)malloc(wcslen(lszResult)*2+4);
		if ( wcscpy(*pszResult, lszResult) )
			return AVZ_OK;
	}

	return AVZ_Error;
}

AVZERROR WINAPI FreeLocalizedStr( IN void* pTaskContext, IN wchar_t* szResult )
{
	if ( !szResult )
		return AVZ_Error;
	free(szResult);
	return AVZ_OK;
}

AVZERROR WINAPI ScriptCallback ( 
	IN void* pScriptContext,
	IN tDWORD dwProgressCur,   // Номер текущего события
	IN tDWORD dwProgressMax,   // Общее количество события (м.б. приблизительным – я его отмасштабирую для прогресс-бара)
	IN wchar_t* szMsg,         // Описание события (надо придумать, как его локализовывать)
	IN tDWORD dwMsgSize,       // Размер строки описания события
	IN AVZMSG dwMsgType        // Тип события
	)
{
	CAVZScan* _this = (CAVZScan*)pScriptContext;
	if ( !_this ) 
		return AVZ_Error;
	_this->m_statistics->Set(dwProgressCur, dwProgressMax, szMsg, dwMsgSize);
	tDWORD proc = (dwProgressMax && dwProgressCur) ? (100 * dwProgressCur) / dwProgressMax : 0;
	PR_TRACE((_this, prtNOTIFY, "CAVZScan::ScriptCallback => (%d%%) %S", proc, szMsg));
	if ( szMsg && dwMsgSize ) 
	{
		cDetectObjectInfo report;
		report.m_tmTimeStamp = cDateTime::now_utc();
		report.m_strObjectName.assign(szMsg, cCP_UNICODE, dwMsgSize);
		report.m_nObjectStatus = (enObjectStatus)dwMsgType;
		_this->sysSendMsg( pmc_PROTECTION_EVENTS_NOTIFY, report.m_nObjectStatus, NULL, &report, SER_SENDMSG_PSIZE );
	}
	while (_this->m_statistics->GetNeedStop() == CStat::NEED_PAUSE)
		Sleep(100);
	if (_this->m_statistics->GetNeedStop() == CStat::NEED_STOP)
		return AVZ_Cancel; 
	else
		return AVZ_OK;
}


#define CHECK_ERRAVZ(ret, e1, e2)	\
	if ( AVZ_FAIL(e2) )	\
	{	\
		PR_TRACE((this, prtNOTIFY, "CAVZScan::SetState(TASK_REQUEST_RUN) => errAVZ = 0x%X", e2));	\
		sysSendMsg(pmc_TASK_STATE_CHANGED, TASK_STATE_RUNNING_MALFUNCTION, 0, 0, 0);	\
		e1 = errNOT_OK;	\
		if (ret) return e1;	\
	}

#define SET_CALLBACK(_name_) pAVZ_SetCallbackProc(m_pAVZAScanKernel, avzcb_##_name_, (void*)_name_);


// AVP Prague stamp begin( External (user called) interface method implementation, SetState )
// Parameters are:
tERROR CAVZScan::SetState( tTaskRequestState p_state ) {
	if ( p_state == TASK_REQUEST_RUN )
	{
		if ( m_state != TASK_STATE_PAUSED )
			return m_pStateManager->SetState(p_state);
		else
		{
			// RESUME
			m_statistics->SetNeedStop(CStat::NEED_RUN);
			sysSendMsg(pmc_TASK_STATE_CHANGED, m_state = TASK_STATE_RUNNING, 0, 0, 0);
		}
		return errOK;
	}
	else
		return SetStateEx(p_state);
}

tERROR CAVZScan::FileToStr(cStringObj szFileName, cStringObj& szStr)
{
	hIO hFile = NULL; 
	sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hOBJECT)cAutoString(szFileName), 0, 0);
	cERROR 	err = sysCreateObject( (hOBJECT*)&hFile, IID_IO, PID_NATIVE_FIO);
	if ( PR_SUCC(err) )
		err = hFile->set_pgOBJECT_FULL_NAME((wchar_t*)szFileName.c_str(cCP_UNICODE), 0, cCP_UNICODE);
	if ( PR_SUCC(err) )
		err = hFile->sysCreateObjectDone();
	if ( PR_SUCC(err) )
	{
		szStr.clear();
		tQWORD qwMaxFileSize = 20*1024;
		tDWORD dwRead = 0;
		tQWORD pos = 0;
		byte c[1024];
		tDWORD dwCodePage = cCP_ANSI;
		const int utf16_flag_size = 2, utf8_flag_size = 3;
		err = hFile->SeekRead(&dwRead, pos, c, 3);
		if ( PR_SUCC(err) )
		{
			if ( 
				dwRead >= utf16_flag_size &&
				c[0] == 0xFF &&
				c[1] == 0xFE
				)
			{
				dwCodePage = cCP_UNICODE;
				pos += utf16_flag_size;
			}
			if ( 
				dwRead == utf8_flag_size &&
				c[0] == 0xEF &&
				c[1] == 0xBB &&
				c[2] == 0xBF
				)
			{
				dwCodePage = cCP_UTF8;
				pos += utf8_flag_size;
			}
			do 
			{
				if(PR_SUCC(err = hFile->SeekRead(&dwRead, pos, c, 1024)) && dwRead)
				{
					pos += dwRead;
					szStr.append(c, dwCodePage, dwRead);
				}
			} 
			while(PR_SUCC(err) && dwRead && (pos < qwMaxFileSize));
		}
		hFile->sysCloseObject();
		hFile = NULL;
	}
	return err;
}
AVZERROR CAVZScan::RunCustomScript()
{
	AVZERROR errAVZ = AVZ_OK;

	void* pScriptBase = NULL;
	cStringObj szScriptBaseShortName; 
	cStringObj szScriptBaseFullName = m_settings.m_szScriptBase; 
	cStringObj szScriptBackupBaseFullName = m_settings.m_szScriptBackupBase; 
	cStringObj szLoadBackupBase; 
	cStringObj szSaveBackupBase; 
	tDWORD i=0, count=m_settings.m_aScriptID.count();
	for ( i=0; i<count; i++ )
	{
		if ( szScriptBaseShortName != m_settings.m_aScriptID.at(i).szScriptBase )
		{
			if ( pScriptBase )
			{
				if ( !szSaveBackupBase.empty() )
					pAVZ_RunScript(m_pAVZAScanKernel, (wchar_t*)szSaveBackupBase.c_str(cCP_UNICODE));
				pAVZ_FreeCustomScriptDB(m_pAVZAScanKernel, pScriptBase);
			}
			szScriptBaseShortName = m_settings.m_aScriptID.at(i).szScriptBase;
			szScriptBaseFullName = m_settings.m_szScriptBase;
			szScriptBaseFullName += szScriptBaseShortName;
			sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hOBJECT)cAutoString(szScriptBaseFullName), 0, 0);
			errAVZ = pAVZ_LoadCustomScriptDB(m_pAVZAScanKernel, (wchar_t*)szScriptBaseFullName.c_str(cCP_UNICODE), &pScriptBase);
			szScriptBackupBaseFullName = m_settings.m_szScriptBackupBase; 
			szScriptBackupBaseFullName += szScriptBaseShortName;
			sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hOBJECT)cAutoString(szScriptBackupBaseFullName), 0, 0);
			szLoadBackupBase = L"##BACKUP.LOAD="; szLoadBackupBase += szScriptBackupBaseFullName; 
			szSaveBackupBase = L"##BACKUP.SAVE="; szSaveBackupBase += szScriptBackupBaseFullName; 
			pAVZ_RunScript(m_pAVZAScanKernel, (wchar_t*)szLoadBackupBase.c_str(cCP_UNICODE));
		}

		tDWORD csrAVZResult = csres_Check_Unknown;
		tDWORD dwScriptID = m_settings.m_aScriptID.at(i).dwScriptID;
		errAVZ = pAVZ_RunCustomScript(m_pAVZAScanKernel, pScriptBase, dwScriptID, m_settings.m_dwScriptMode, &csrAVZResult);
		cSerializable* pResult = NULL;
		if ( PR_FAIL(g_root->CreateSerializable(cAVZScanResult::eIID, &pResult)))
			break;
		if ( errAVZ == AVZ_InvalidScriptID )
		{
			((cAVZScanResult*)pResult)->dwResult = csres_Check_Unknown;
			errAVZ = AVZ_OK;
		}
		else if ( AVZ_SUCC(errAVZ) ) 
		{
			((cAVZScanResult*)pResult)->dwResult = csrAVZResult;
			if ( m_settings.m_dwScriptMode != csmode_CheckUndo )
				if ( AVZ_SUCC(errAVZ = pAVZ_RunCustomScript(m_pAVZAScanKernel, pScriptBase, dwScriptID, csmode_CheckUndo, &csrAVZResult)) )
					((cAVZScanResult*)pResult)->bHasBackup = (csrAVZResult==csres_CheckUndo_OK);
		}
		else if ( AVZ_FAIL(errAVZ) )
			break;
		cAutoCS cs(m_statistics->GetCS(),true);
		m_statistics->GetStat()->m_aXMLRecords.push_back().ptr_ref() = pResult;
		m_statistics->Set(i, count, 0, 0, (m_settings.m_dwScriptMode != csmode_Check) ? csrAVZResult==csres_Fix_NeedReboot: cFALSE);
	}
	pAVZ_RunScript(m_pAVZAScanKernel, (wchar_t*)szSaveBackupBase.c_str(cCP_UNICODE));
	pAVZ_FreeCustomScriptDB(m_pAVZAScanKernel, pScriptBase);

	return errAVZ;
}

tERROR CAVZScan::SetStateEx( tTaskRequestState p_state ) {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::SetState method" );

	// Place your code here
	tDWORD nOldState= m_state;

	switch( p_state )
	{
	case TASK_REQUEST_RUN:
		if ( !m_pAVZAScanKernel )
		{
			AVZERROR errAVZ = AVZ_OK;
			//sysSendMsg(pmc_TASK_STATE_CHANGED, m_state = TASK_STATE_STARTING, 0, 0, 0);
			cStringObj sBaseFolder = "%Bases%\\";      sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hOBJECT)cAutoString(sBaseFolder), 0, 0);
			cStringObj sProductType = "%ProductType%"; sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hOBJECT)cAutoString(sProductType), 0, 0);

			PR_TRACE((this, prtNOTIFY, "CAVZScan::SetState(TASK_REQUEST_RUN) => AVZ_InitializeEx(%S)", (wchar_t*)sBaseFolder.c_str(cCP_UNICODE)));
			errAVZ = pAVZ_InitializeExW(&m_pAVZAScanKernel, (wchar_t*)sBaseFolder.c_str(cCP_UNICODE), (wchar_t*)sProductType.c_str(cCP_UNICODE));
			CHECK_ERRAVZ( m_pAVZAScanKernel?cTRUE:cFALSE,  error, errAVZ);
			pAVZ_SetCallbackCtx(m_pAVZAScanKernel, this);
			if ( AVZ_FAIL(errAVZ) && m_pAVZAScanKernel )
			{
				pAVZ_Done(m_pAVZAScanKernel);
				break;
			}
			SET_CALLBACK(ScriptCallback);
			SET_CALLBACK(GetLocalizedStr);
			SET_CALLBACK(FreeLocalizedStr);
			sysSendMsg(pmc_TASK_STATE_CHANGED, m_state = TASK_STATE_RUNNING, 0, 0, 0);
			PR_TRACE((this, prtNOTIFY, "CAVZScan::SetState(TASK_REQUEST_RUN) => AVZ_GetSomeInfo(%S)", (wchar_t*)m_settings.m_szScript.c_str(cCP_UNICODE)));
			IStream* pXML = NULL;

			if ( m_settings.m_szScript.size() )
			{
				if ( m_settings.m_dwXMLSerId ) pXML = CreateIStreamIO(m_hXML, false);
				errAVZ = pAVZ_GetSomeInfo(m_pAVZAScanKernel, (wchar_t*)m_settings.m_szScript.c_str(cCP_UNICODE), pXML);
			}
			else if ( m_settings.m_szScriptFile.size() )
			{
				cStringObj szScript;
				error = FileToStr(m_settings.m_szScriptFile, szScript);
				if ( PR_SUCC(error) )
				{
					if ( m_settings.m_dwXMLSerId ) pXML = CreateIStreamIO(m_hXML, false);
					errAVZ = pAVZ_GetSomeInfo(m_pAVZAScanKernel, (wchar_t*)szScript.c_str(cCP_UNICODE), pXML);
				}
			}
			else if ( m_settings.m_szScriptBase.size() )
			{
				errAVZ = RunCustomScript();
			}
			if ( pXML )
			{
				AVZERROR errAVZ = AVZ_OK;
				{
					cAutoCS cs(m_statistics->GetCS(),true);
					error = DeserializeXML(
						m_pAVZAScanKernel, 
						pXML, 
						(wchar_t*)m_settings.m_szXMLTag.c_str(cCP_UNICODE),
						m_settings.m_dwXMLSerId,
						m_pr_type_map,
						&(m_statistics->GetStat()->m_aXMLRecords)
						);
				}
				if (pXML) pXML->Release(); pXML = NULL;
				CHECK_ERRAVZ( cFALSE, error, errAVZ = pAVZ_Done(m_pAVZAScanKernel));
			}
			if ( errAVZ == AVZ_Cancel )
			{
				PR_TRACE((this, prtNOTIFY, "CAVZScan::SetState(TASK_REQUEST_RUN) => AVZ_Cancel"));
				sysSendMsg(pmc_TASK_STATE_CHANGED, m_state = TASK_STATE_STOPPED,   0, 0, 0);
			}
			else if ( errAVZ == AVZ_OK )
				sysSendMsg(pmc_TASK_STATE_CHANGED, m_state = TASK_STATE_COMPLETED, 0, 0, 0);
			else
			{
				PR_TRACE((this, prtNOTIFY, "CAVZScan::SetState(TASK_REQUEST_RUN) => AVZ_FAIL(0x%X)", errAVZ));
				tERROR err = errNOT_OK;
				tDWORD len = sizeof(tERROR);
				sysSendMsg(pmc_TASK_STATE_CHANGED, m_state = TASK_STATE_FAILED,    0, &err, &len);
			}
			m_pAVZAScanKernel = NULL;
		}
		break;

	case TASK_REQUEST_PAUSE:
		if( m_state != TASK_STATE_COMPLETED )
			m_state = TASK_STATE_PAUSED;
		m_statistics->SetNeedStop(CStat::NEED_PAUSE);
		PR_TRACE((this, prtNOTIFY, "CAVZScan::SetState(TASK_REQUEST_PAUSE) => I don't know what to do in this case..."));
		sysSendMsg(pmc_TASK_STATE_CHANGED, TASK_STATE_PAUSED, 0, 0, 0);
		break;

	case TASK_REQUEST_STOP:
		if( m_state == TASK_STATE_COMPLETED )
			sysSendMsg(pmc_TASK_STATE_CHANGED, m_state, 0, 0, 0);
		else
		{
			PR_TRACE((this, prtNOTIFY, "CAVZScan::SetState(TASK_REQUEST_STOP) => SetNeedStop"));
			m_statistics->SetNeedStop(CStat::NEED_STOP);
			m_state = TASK_STATE_STOPPED;
		}
		break;

	default:
		return error = errTASK_STATE_UNKNOWN;
	}

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetStatistics )
// Parameters are:
tERROR CAVZScan::GetStatistics( cSerializable* p_statistics ) {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::GetStatistics method" );

	// Place your code here
	error = m_statistics->Get(p_statistics);

	return error;
}
// AVP Prague stamp end

#pragma pack(1)
struct cXMLRecord;
struct cXMLRecord 
{
	cXMLRecord(){ZeroMemory(this, sizeof(cXMLRecord));};
	cXMLRecord* m_pNextItem;	//	Указатель на следующий элемент списка
};
#pragma pack()

tERROR DeserializeXML( 
		IN CAVZScanKernel* pAVZAScanKernel,    // Контекст ядра AVZ
		IN IStream* pXMLStream,                // Стрим с XML
		IN wchar_t* szTagName,                 // Имя тега первого уровня, чье содержимое будет преобразовано в структуру
		IN tDWORD dwXMLSerId,                  // Идентификатор сериализатора 
		IN pr_type_map_t& pr_type_map,         // Таблица соответствия объявленных типов XML и пражских типов
		IN OUT cXMLRecords* paXMLRecords       // Вектор с результатами сканирования
		)
{
	//////////////////////////////////////////////////////////////////////////
	// 1. Создать cSerializable по идентификатору dwSerId
	// 2. Проходим по всем полям pRecord, создавая строку форматирования XML
	// 3. pAVZ_DeserializeXML, используя созданную строку szXMLFormat
	// 4. Перебираем связаный список pXMLRecords
	// 5. Создаем очередной экземпляр cSerializable по идентификатору dwSerId
	// 6. копируем соответствующие данные в поле pRecord
	// 7. Сохраняем запись в вектор
	// 8. Переходим к следующему элементу связаного списка pXMLRecords
	//////////////////////////////////////////////////////////////////////////

	//	1. Создать cSerializable по идентификатору dwSerId
	cSerializable* pRecord = NULL;
	cERROR err = g_root->CreateSerializable(dwXMLSerId, &pRecord);
	if ( PR_SUCC(err) )
	{
		// 2. Проходим по всем полям pRecord, создавая строку форматирования XML
		cStringObj szXMLFormat;
		const cSerDescriptorField * field = NULL;
		for(
			field = pRecord->getDsc()->m_fields; 
			field->m_flags != SDT_FLAG_ENDPOINT; 
			++field
			)
		{
			if ( field->m_name && field->m_size )
			{
				szXMLFormat += field->m_name;
				szXMLFormat += L"=";
				szXMLFormat += pr_type_map[(tTYPE_ID)field->m_id].c_str();
			}
		}
		g_root->DestroySerializable(pRecord);
		// 3. pAVZ_DeserializeXML, используя созданную строку szXMLFormat
		cXMLRecord* pXMLRecords = NULL;
		tDWORD dwSize = NULL;
		AVZERROR errAVZ = pAVZ_DeserializeXML(
			pAVZAScanKernel, 
			pXMLStream, 
			szTagName, 
			(wchar_t*)szXMLFormat.c_str(cCP_UNICODE), 
			(void**)&pXMLRecords, 
			&dwSize
			);
		if ( AVZ_SUCC(errAVZ) )
		{
			// 4. Перебираем связаный список pXMLRecords
			cXMLRecord* pXMLRecord = pXMLRecords;
			while ( pXMLRecord )
			{
				// 5. Создаем очередной экземпляр cSerializable по идентификатору dwSerId
				pRecord = NULL;
				err = g_root->CreateSerializable(dwXMLSerId, &pRecord);
				if ( PR_FAIL(err) )
					break;
				// 6. копируем соответствующие данные в поле pRecord
				byte* from = (byte*)pXMLRecord+sizeof(cXMLRecord*); //сдвигаем на размер первой полянки m_pNextItem
				for(
					field = pRecord->getDsc()->m_fields; 
					field->m_flags != SDT_FLAG_ENDPOINT; 
					++field
					)
				{
					if ( field->m_name && field->m_size )
					{
						switch (field->m_id)
						{
						// Размеры соответствуют объявленным в \PPP\AVZScan\AVZ\Projects\SysInfo\avz-avp\str_serialize.pas, GetStaticSizeByType
						case tid_BYTE:  memcpy((byte*)pRecord+field->m_offset, from, field->m_size); from += 1;  break;
						case tid_WORD:  memcpy((byte*)pRecord+field->m_offset, from, field->m_size); from += 2;  break;
						case tid_DWORD: memcpy((byte*)pRecord+field->m_offset, from, field->m_size); from += 4;  break;
						case tid_INT:   memcpy((byte*)pRecord+field->m_offset, from, field->m_size); from += 4;  break;
						case tid_STRING_OBJ:
							{
								cStrObj* str = (cStrObj*)((byte*)pRecord+field->m_offset);
								wchar_t* str_from = *(wchar_t**)from;
								*str = str_from;
								from += 4;
							}
							break;
						}
					}
				}	
				// 7. Сохраняем запись в вектор
				paXMLRecords->push_back().ptr_ref() = pRecord;
				// 8. Переходим к следующему элементу связаного списка pXMLRecords
				pXMLRecord = pXMLRecord->m_pNextItem;
			}
			errAVZ = pAVZ_Free(pAVZAScanKernel, pXMLRecords);
			return err;
		}
	}
	return errNOT_OK;
}


// AVP Prague stamp begin( C++ class regitration,  )
// Interface "Task". Register function
tERROR CAVZScan::Register( hROOT root ) {
	tERROR error;
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(Task_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, Task_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "AVZ Scan", 9 )
	mpLOCAL_PROPERTY_BUF( pgTASK_TM, CAVZScan, m_tm, cPROP_BUFFER_READ_WRITE )
	mpLOCAL_PROPERTY_BUF( pgTASK_STATE, CAVZScan, m_state, cPROP_BUFFER_READ )
	mpLOCAL_PROPERTY_BUF( pgTASK_SESSION_ID, CAVZScan, m_session_id, cPROP_BUFFER_READ )
	mpLOCAL_PROPERTY_BUF( pgTASK_ID, CAVZScan, m_task_id, cPROP_BUFFER_READ )
	mpLOCAL_PROPERTY_BUF( pgTASK_PERSISTENT_STORAGE, CAVZScan, m_storage, cPROP_BUFFER_READ_WRITE )
mpPROPERTY_TABLE_END(Task_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(Task)
	mINTERNAL_METHOD(ObjectInitDone)
	mINTERNAL_METHOD(ObjectPreClose)
mINTERNAL_TABLE_END(Task)
// AVP Prague stamp end



// AVP Prague stamp begin( External method table,  )
mEXTERNAL_TABLE_BEGIN(Task)
	mEXTERNAL_METHOD(Task, SetSettings)
	mEXTERNAL_METHOD(Task, GetSettings)
	mEXTERNAL_METHOD(Task, AskAction)
	mEXTERNAL_METHOD(Task, SetState)
	mEXTERNAL_METHOD(Task, GetStatistics)
mEXTERNAL_TABLE_END(Task)
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )

	PR_TRACE_FUNC_FRAME_( *root, "Task::Register method", &error );

//! 	error = CALL_Root_RegisterIFace(
//! 		root,                                   // root object
//! 		IID_TASK,                               // interface identifier
//! 		PID_AVZSCAN,                            // plugin identifier
//! 		0x00000000,                             // subtype identifier
//! 		Task_VERSION,                           // interface version
//! 		VID_DENISOV,                            // interface developer
//! 		NULL,                                   // internal(kernel called) function table
//! 		0,                                      // internal function table size
//! 		&e_Task_vtbl,                           // external function table
//! 		(sizeof(e_Task_vtbl)/sizeof(tPTR)),     // external function table size
//! 		Task_PropTable,                         // property table
//! 		mPROPERTY_TABLE_SIZE(Task_PropTable),   // property table size
//! 		sizeof(CAVZScan)-sizeof(cObjImpl),      // memory size
//! 		0                                       // interface flags
//! 	);
	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_TASK,                               // interface identifier
		PID_AVZSCAN,                            // plugin identifier
		0x00000000,                             // subtype identifier
		Task_VERSION,                           // interface version
		VID_DENISOV,                            // interface developer
		&i_Task_vtbl,                           // internal(kernel called) function table
		(sizeof(i_Task_vtbl)/sizeof(tPTR)),     // internal function table size
		&e_Task_vtbl,                           // external function table
		(sizeof(e_Task_vtbl)/sizeof(tPTR)),     // external function table size
		Task_PropTable,                         // property table
		mPROPERTY_TABLE_SIZE(Task_PropTable),   // property table size
		sizeof(CAVZScan)-sizeof(cObjImpl),      // memory size
		0                                       // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"Task(IID_TASK) registered [%terr]",error) );
	#endif // _PRAGUE_TRACE_
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration end,  )
	return error;
}

tERROR pr_call Task_Register( hROOT root ) { return CAVZScan::Register(root); }
// AVP Prague stamp end



// AVP Prague stamp begin( Caution !!!,  )
// You have to implement propetry: pgTASK_PARENT_ID
// AVP Prague stamp end



