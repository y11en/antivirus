// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Saturday,  18 November 2006,  12:08 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Guschin
// File Name   -- task.cpp
// -------------------------------------------
// AVP Prague stamp end
// AVP Prague stamp begin( Interface version,  )
#define Task_VERSION ((tVERSION)1)
// AVP Prague stamp end
#include "../../include/ver_mod.h"
#include "../comdefs.h"
// AVP Prague stamp begin( Includes for interface,  )
#include "task.h"
// AVP Prague stamp end
#define IMPEX_IMPORT_LIB
#include <Prague/iface/e_loader.h>
#include <Updater/updater.h>
// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "Task". Static(shared) property table variables
// AVP Prague stamp end
const tWCHAR * const CUpdater2005::s_szSingletonEventName = L"UPDATER_SINGLETON_EVENT_D3738069-D556-43c4-AEFE-4D0C69402939";


#ifdef _WIN32
	// ****************************************************************************************
	// ******** bug 26917 HACK against access violation in jsproxy BEGIN ********
	// Note this hack by some unknown reasons fixes TSL leak under Applicaiton Verifier on Windows XP
	// IMPORTANT: this HACK only works in case update runs in separate process
	//
	// BEFORE you remove this hack, try update with automatic proxy server address detection
	//    with WPAD script on Windows XP under Application Verifier with TLS check enabled.
	//
	// Some additional detatils here:
	//  http://www.eggheadcafe.com/software/aspnet/30425239/leaks-with-jsproxy-in-csc.aspx
	//  http://www.microsoft.com/communities/newsgroups/en-us/default.aspx?dg=microsoft.public.win32.programmer.networks&tid=cea6c0b7-3496-42bc-8f98-d9bdd5d27a5e&p=1
	// ****************************************************************************************

    // defined type of function for initialize library API
    // this type structure is needed to get function addresses from jsproxy.dll
    struct AutoProxyHelperFunctions
    {
        const struct AutoProxyHelperVtbl *lpVtbl;
    };
    // this type structure is needed to get function addresses from jsproxy.dll
    struct AutoProxyScriptBuffer
    {
        DWORD m_structSize;
        LPSTR m_buffer;
        DWORD m_bufferSize;
    };

    typedef BOOL(CALLBACK *Function_InitializeDll)(DWORD dwVersion, LPSTR lpszDownloadedTempFile,
        LPSTR lpszMime, AutoProxyHelperFunctions *lpAutoProxyCallbacks, AutoProxyScriptBuffer *lpAutoProxyScriptBuffer);
    typedef BOOL (CALLBACK *Function_DeInitializeDll)(LPSTR lpszMime, DWORD dwReserved);
#endif // _WIN32
    void hackAgainstTlsLeak()
    {
#ifdef _WIN32
        // on Windows Vista the problem is not actual and not reproducable
        if(KLUPD::isVistaOrGreaterOS())
            return;

        // initialization jsproxy.dll with just downloaded WPAD script file
        HMODULE module = ::LoadLibrary(_T("jsproxy.dll"));
        if(!module)
            return;

        char *wpad_LocalFullFileNameCopy = "nofile.nofile.nofile.782632";
        Function_InitializeDll functionInitializeDll = reinterpret_cast<Function_InitializeDll>(GetProcAddress(module, "InternetInitializeAutoProxyDll"));
        Function_DeInitializeDll functionDeInitializeDll = reinterpret_cast<Function_DeInitializeDll>(GetProcAddress(module, "InternetDeInitializeAutoProxyDll"));
        if(functionInitializeDll
            && !!functionInitializeDll(0, wpad_LocalFullFileNameCopy, 0, 0, 0)
            && functionDeInitializeDll)
        {
            functionDeInitializeDll(0, 0);
        }
        FreeLibrary(module);
#endif 
    }

// ****************************************************************************************
// ******** bug 26917 HACK against access violation in jsproxy END ********
// ****************************************************************************************

//-------------------------------------------------------------------
// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR CUpdater2005::ObjectInitDone()
{
    pLog = new PRAGUE_HELPERS::PragueLog("UPD", static_cast<tTRACE_LEVEL>(349));

    TRACE_MESSAGE("Started Updater Task instanse creation");

    m_dwState = TASK_STATE_CREATED;
    m_dwSessionID = 0;
    m_dwTaskID = 0;
    m_dwNewState = TASK_STATE_CREATED;
    m_failedToUpdateAllComponents = cFALSE;
    m_hToken = 0;
    m_interactionWithUserEnabled = cTRUE;


    const tERROR stateCriticalSectionCreateResult = sysCreateObjectQuick((hOBJECT*)&m_hCSState, IID_CRITICAL_SECTION);
    if(PR_FAIL(stateCriticalSectionCreateResult))
    {
        TRACE_MESSAGE2("Failed to create state critical section, result '%s'", PRAGUE_HELPERS::toStringPragueResult(stateCriticalSectionCreateResult).c_str());
        return stateCriticalSectionCreateResult;
    }

    const tERROR newStateCriticalSectionCreateResult = sysCreateObjectQuick((hOBJECT*)&m_hCSNewState, IID_CRITICAL_SECTION);
    if(PR_FAIL(newStateCriticalSectionCreateResult))
    {
        TRACE_MESSAGE2("Failed to create new state critical section, result '%s'", PRAGUE_HELPERS::toStringPragueResult(newStateCriticalSectionCreateResult).c_str());
        return newStateCriticalSectionCreateResult;
    }

    const tERROR statisticsCriticalSectionCreateResult = sysCreateObjectQuick((hOBJECT*)&m_hCSStatistics, IID_CRITICAL_SECTION);
    if(PR_FAIL(statisticsCriticalSectionCreateResult))
    {
        TRACE_MESSAGE2("Failed to create statistics critical section, result '%s'", PRAGUE_HELPERS::toStringPragueResult(statisticsCriticalSectionCreateResult).c_str());
        return statisticsCriticalSectionCreateResult;
    }

    const tERROR settingsCriticalSectionCreateResult = sysCreateObjectQuick((hOBJECT*)&m_hCSSettings, IID_CRITICAL_SECTION);
    if(PR_FAIL(settingsCriticalSectionCreateResult))
    {
        TRACE_MESSAGE2("Failed to create settings critical section, result '%s'", PRAGUE_HELPERS::toStringPragueResult(settingsCriticalSectionCreateResult).c_str());
        return settingsCriticalSectionCreateResult;
    }

    const tERROR pauseEventCreateResult = sysCreateObjectQuick((hOBJECT *)&m_hPauseEvent, IID_EVENT);
    if(PR_FAIL(pauseEventCreateResult))
    {
        TRACE_MESSAGE2("Failed to create pause event, result '%s'", PRAGUE_HELPERS::toStringPragueResult(pauseEventCreateResult).c_str());
        return pauseEventCreateResult;
    }

    tERROR singletonEventCreateResult = sysCreateObject((hOBJECT *)&m_SingletonEvent, IID_EVENT);
    if( PR_SUCC(singletonEventCreateResult) )
        m_SingletonEvent->set_pgOBJECT_NAME( (tPTR)s_szSingletonEventName, 0, cCP_UNICODE );
    if( PR_SUCC(singletonEventCreateResult) )
        singletonEventCreateResult = m_SingletonEvent->set_pgMANUAL_RESET(cFALSE);
    if( PR_SUCC(singletonEventCreateResult) )
        singletonEventCreateResult = m_SingletonEvent->set_pgINITIAL_STATE(cTRUE);
    if( PR_SUCC(singletonEventCreateResult) )
        singletonEventCreateResult = m_SingletonEvent->sysCreateObjectDone();
    if(PR_FAIL(singletonEventCreateResult))
    {
        TRACE_MESSAGE2("Failed to create singleton support event, result '%s'",
            PRAGUE_HELPERS::toStringPragueResult(singletonEventCreateResult).c_str());
        return singletonEventCreateResult;
    }

    // set lower thread priority, because updater opens to many files and hits performance
    const tERROR threadPoolInitResult = cThreadPoolBase::init(this, 1, 0, TP_THREADPRIORITY_BELOW_NORMAL);
    if(PR_FAIL(threadPoolInitResult))
    {
        TRACE_MESSAGE2("Failed to initialize thread pool, result '%s'", PRAGUE_HELPERS::toStringPragueResult(threadPoolInitResult).c_str());
        return threadPoolInitResult;
    }

    m_hLic = 0;
    const tERROR getLicenseServiceResult = m_hTM->GetService(TASKID_TM_ITSELF, *this, sid_TM_LIC, (cObject**)&m_hLic);
    // not fatal error, may continue event without license key, because Bussiness
     // Logic product part is responsible is update task may be started
    if(PR_FAIL(getLicenseServiceResult))
        TRACE_MESSAGE2("Warning: failed to get license service, result '%s'", PRAGUE_HELPERS::toStringPragueResult(getLicenseServiceResult).c_str());


    TRACE_MESSAGE("Updater Task instanse has been successfully initialized");
	return errOK;
}
// AVP Prague stamp end
// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:
tERROR CUpdater2005::ObjectPreClose()
{
    TRACE_MESSAGE("Started Updater Task instanse close");

    cThreadPoolBase::de_init();

    if(m_hToken)
    {
        switchToSystemContext(L"Update task closed");
		m_hToken->sysCloseObject();
        m_hToken = 0;
    }

    if(m_hPauseEvent)
    {
        m_hPauseEvent->sysCloseObject();
        m_hPauseEvent = 0;
    }

    if(m_hCSSettings)
    {
        m_hCSSettings->sysCloseObject();
        m_hCSSettings = 0;
    }

    if(m_hCSStatistics)
    {
        m_hCSStatistics->sysCloseObject();
        m_hCSStatistics = 0;
    }

    if(m_hCSNewState)
    {
        m_hCSNewState->sysCloseObject();
        m_hCSNewState = 0;
    }

    if(m_hCSState)
    {
        m_hCSState->sysCloseObject();
        m_hCSState = 0;
    }

    if(m_hLic)
    {
        m_hTM->ReleaseService(TASKID_TM_ITSELF, (hOBJECT)m_hLic);
        m_hLic = 0;
    }

    TRACE_MESSAGE("Updater Task instanse has been successfully closed");

    delete pLog;
    pLog = 0;

	return errOK;
}
// AVP Prague stamp end
// AVP Prague stamp begin( External (user called) interface method implementation, SetSettings )
// Parameters are:
tERROR CUpdater2005::SetSettings(const cSerializable *p_settings)
{
    if(!p_settings || !p_settings->isBasedOn(cUpdaterSettings::eIID))
    {
        TRACE_MESSAGE2("Failed to set Updater Task settings, new settings handle 0x%x", p_settings);
        return errPARAMETER_INVALID;
    }

    cAutoCS autoCSSettings(m_hCSSettings, true);
    m_Settings = *((cUpdaterSettings *)(p_settings));
    TRACE_MESSAGE("Updater Task settings has been set");
    return errOK;
}
// AVP Prague stamp end
// AVP Prague stamp begin( External (user called) interface method implementation, GetSettings )
// Parameters are:
tERROR CUpdater2005::GetSettings(cSerializable *p_settings)
{
    TRACE_MESSAGE("Updater Task get settings request is not implemented");
	return errNOT_IMPLEMENTED;
}
// AVP Prague stamp end
// AVP Prague stamp begin( External (user called) interface method implementation, AskAction )
// Parameters are:
tERROR CUpdater2005::AskAction(tActionId p_actionId, cSerializable *p_actionInfo)
{
	return errNOT_IMPLEMENTED;
}
// AVP Prague stamp end
// AVP Prague stamp begin( External (user called) interface method implementation, SetState )
// Parameters are:
tERROR CUpdater2005::SetState(tTaskRequestState state)
{
    tDWORD dwState = TASK_REQUEST_UNK;
    switch(state)
    {
    case TASK_REQUEST_RUN:
        TRACE_MESSAGE("Updater Task run state request received");
        dwState = TASK_STATE_RUNNING;

        if(!m_hToken)
        {
            cProxySettings proxySettingsValue;
            if(proxySettings(proxySettingsValue)
                && !!proxySettingsValue.m_bUseIEProxySettings)
            {
                hackAgainstTlsLeak();
            }

            const tERROR createImpersonationTokenResult = sysCreateObjectQuick((hOBJECT*)&m_hToken, IID_TOKEN);
            if(!m_hToken || PR_FAIL(createImpersonationTokenResult))
                TRACE_MESSAGE2("Failed to create token for impersonation, result '%s'", PRAGUE_HELPERS::toStringPragueResult(createImpersonationTokenResult).c_str());
        }
        break;
    case TASK_REQUEST_PAUSE:
        TRACE_MESSAGE("Updater Task pause state request received");
        dwState = TASK_STATE_PAUSED;
        break;
    case TASK_REQUEST_STOP:
        TRACE_MESSAGE("Updater Task stop state request received");
        dwState = TASK_STATE_STOPPED;
        break;

    default:
        break; // suppress compiler warning
    }

    {
        cAutoCS autoCSNewState(m_hCSNewState, true);
        if(m_dwNewState == (tDWORD)dwState)
            return errOK;

        m_dwNewState = dwState;
    }

    tDWORD dwOldState = TASK_STATE_UNKNOWN;
    {
        cAutoCS autoCSState(m_hCSState, false);
        dwOldState = m_dwState;
    }

    if(dwOldState == TASK_STATE_PAUSED)
        m_hPauseEvent->SetState(cTRUE);

    if(dwState == TASK_STATE_RUNNING)
    {
        if(dwOldState != TASK_STATE_RUNNING && dwOldState != TASK_STATE_PAUSED)
        {
            const tERROR errSingletonEvent = CALL_Event_Wait(m_SingletonEvent, 0);
            if (PR_FAIL(errSingletonEvent))
            {
                TRACE_MESSAGE("Updater Task is already running");
                return errTASK_ALREADY_RUNNING ;
            }

            const tERROR threadStartResult = cThreadTaskBase::start(*this);
            if(PR_FAIL(threadStartResult))
            {
                TRACE_MESSAGE2("Failed to start Updater Task, result '%s'", PRAGUE_HELPERS::toStringPragueResult(threadStartResult).c_str());
                return threadStartResult;
            }
        }
    }

	return errOK;
}
// AVP Prague stamp end
// AVP Prague stamp begin( External (user called) interface method implementation, GetStatistics )
// Parameters are:
tERROR CUpdater2005::GetStatistics(cSerializable *p_statistics)
{
    if(!p_statistics || !p_statistics->isBasedOn(cUpdaterStatistics::eIID))
        return errPARAMETER_INVALID;

   cAutoCS autoCSStatistics(m_hCSStatistics, false);
   *((cUpdaterStatistics *)p_statistics) = m_Statistics;
	return errOK;
}
// AVP Prague stamp end
CUpdater2005::CUpdater2005()
 : cThreadPoolBase("Updater")
{
}

void CUpdater2005::do_work()
{
    TRACE_MESSAGE("Updater Task started");

    switchToUserContext(L"Updater Task Started");

    m_failedToUpdateAllComponents = cFALSE;
    tBOOL rollbackMode = cFALSE;
    {
        cAutoCS autoCSSettings(m_hCSSettings, true);
        rollbackMode = m_Settings.m_bRollback;
    }
    setState(TASK_STATE_RUNNING);

    // clear statistics
    {
        cAutoCS autoCSStatistics(m_hCSStatistics, true);

        m_Statistics.m_strHostPath = "";
        m_Statistics.m_strFileName = "";
        m_Statistics.m_timeStart = static_cast<tDWORD>(cDateTime::now_utc());
        m_Statistics.m_timeFinish = 0;
        m_Statistics.m_qwTotalDownloadBytesCount = 0;
        m_Statistics.m_qwDownloadedBytesCount = 0;
        m_Statistics.m_dbSpeed = 0;
        m_Statistics.m_nPercentCompleted = 0;
    }

    PragueDownloadProgress pragueDownloadProgress(*this, pLog);
    PragueCallbacks callbacks(*this, *pLog);
    PragueJournal journal(*this, pragueDownloadProgress, callbacks);

    KLUPD::CoreError result = KLUPD::CORE_NO_ERROR;
    
    if(callbacks.loadUpdaterConfiguration())
    {
        if(rollbackMode)
        {
            result = KLUPD::Updater(pragueDownloadProgress, callbacks, journal, pLog).doRollback();
            // rollback is available no more
            if(result != KLUPD::CORE_CANCELLED)
                callbacks.informProductAboutRollbackAvailability(false);
        }
        else
		{
            result = KLUPD::Updater(pragueDownloadProgress, callbacks, journal, pLog).doUpdate();
		}

        // in case update black list only mode, then report not all components were updated
        if(!rollbackMode && callbacks.m_blackListOnly)
            m_failedToUpdateAllComponents = true;
    }
    else
    {
        TRACE_MESSAGE("Failed to load updater configuration");
        result = KLUPD::CORE_ADMKIT_FAILURE;
    }

    updateInformationAboutLockedFiles(callbacks);


    // set task completion result
    if(KLUPD::isSuccess(result))
    {
        if ( m_failedToUpdateAllComponents )
        {
            setCompletionState(TASK_STATE_COMPLETED, KLUPD::CORE_NotAllComponentsAreUpdated);
            TRACE_MESSAGE2("Update task completion result: '%S'",
                KLUPD::toString(KLUPD::CORE_NotAllComponentsAreUpdated).toWideChar());
        }
        else
        {
            setCompletionState(TASK_STATE_COMPLETED, result);
            TRACE_MESSAGE2("Successful update task completion result: '%S'",
                KLUPD::toString(result).toWideChar());
        }
    }
    else
    {
		setCompletionState(result == KLUPD::CORE_CANCELLED ? TASK_STATE_STOPPED : TASK_STATE_FAILED, result);
        TRACE_MESSAGE2("Failure update task completion result: '%S'",
            KLUPD::toString(result).toWideChar());
    }

    CALL_Event_SetState(m_SingletonEvent, cTRUE);
}

void CUpdater2005::setState(const tDWORD state)
{
    {
        cAutoCS autoCSState(m_hCSState, true);
        cAutoCS autoCSNewState(m_hCSNewState, true);

        m_dwState = state;
    }

    tDWORD size = sizeof(state);
    const tERROR setStateResult = sysSendMsg(pmc_TASK_STATE_CHANGED, state, 0, reinterpret_cast<void *>(const_cast<tDWORD *>(&state)), &size);
    if(PR_FAIL(setStateResult))
        TRACE_MESSAGE2("Failed to set update task state: '%s'", PRAGUE_HELPERS::toStringPragueResult(setStateResult).c_str());
}

void CUpdater2005::setCompletionState(const tDWORD &state, const KLUPD::CoreError &resultIn)
{
    // translate result to Prague understandable code
    const tERROR result = PRAGUE_HELPERS::translateUpdaterResultCodeToPrague(resultIn);

    cUpdaterEvent completionEvent;
    completionEvent.m_qwEvent = PragueJournal::createMessageIdentifier(pmc_UPDATER_CRITICAL_REPORT, pm_REP_UPDATE_END);
    completionEvent.m_errError = result;
    completionEvent.m_tmTimeStamp = cDateTime::now_utc();
    completionEvent.m_blRollback = m_Settings.m_bRollback ;

    cAutoCS autoCSState(m_hCSState, true);
    cAutoCS autoCSNewState(m_hCSNewState, true);

    {
        cAutoCS autoCSStatistics(m_hCSStatistics, true);
        m_Statistics.m_errUpdateError = result;
        // task completion time will be corrected by Prague Task Manager
        m_Statistics.m_timeFinish = 0;
    }

    const tERROR writeTaskCompletionToJournalResult = sysSendMsg(pmc_UPDATER_CRITICAL_REPORT, pm_REP_UPDATE_END, 0, &completionEvent, SER_SENDMSG_PSIZE);
    if(PR_FAIL(writeTaskCompletionToJournalResult))
        TRACE_MESSAGE2("Failed to write task completion result to journal: '%s'", PRAGUE_HELPERS::toStringPragueResult(writeTaskCompletionToJournalResult).c_str());
    

    m_dwState = state;

    autoCSNewState.unlock();
    autoCSState.unlock();

    tDWORD size = sizeof(state);
    const tERROR sendCompletionMessageResult = sysSendMsg(pmc_TASK_STATE_CHANGED, state, 0, reinterpret_cast<void *>(const_cast<tERROR *>(&result)), &size);
    if(PR_FAIL(sendCompletionMessageResult))
        TRACE_MESSAGE2("Failed to send updater task completion message: '%s'", PRAGUE_HELPERS::toStringPragueResult(sendCompletionMessageResult).c_str());
}

void CUpdater2005::updateInformationAboutLockedFilesInSystemContext(PragueCallbacks &callbacks)
{
    char filesToRemoveRegistryKey[] = {"HKLM\\" VER_PRODUCT_REGISTRY_PATH "\\FilesToRemove"};

    cAutoRegistry registry(*this, filesToRemoveRegistryKey, PID_WIN32_REG, cFALSE);
    cAutoRegKey registryKey(registry, cRegRoot);

    FilesToRemove *filesToRemove = 0;
    const tERROR readFromRegistryResult = g_root->RegDeserialize(reinterpret_cast<cSerializable **>(&filesToRemove), registryKey, "", FilesToRemove::eIID);
    if(PR_FAIL(readFromRegistryResult) || !filesToRemove)
    {
        TRACE_MESSAGE2("Failed to deserialize files to remove structure, result '%s'", PRAGUE_HELPERS::toStringPragueResult(readFromRegistryResult).c_str());
        filesToRemove = new FilesToRemove;
    }

    // try remove files read from registry
    for(size_t fileIndex = 0; fileIndex < filesToRemove->m_files.size(); )
    {
        // file removed
        const KLUPD::CoreError removeFileResult = KLUPD::LocalFile(filesToRemove->m_files[fileIndex].data(), pLog).unlink();
        if(isSuccess(removeFileResult))
            filesToRemove->m_files.remove(fileIndex, fileIndex);
        else
            ++fileIndex;
    }

    // update registry with new locked files, which needs to be removed
        // these files will be removed on next update start or by installer
    for(std::vector<KLUPD::Path>::const_iterator fileIter = callbacks.m_lockedFilesToRemove.begin(); fileIter != callbacks.m_lockedFilesToRemove.end(); ++fileIter)
    {
        filesToRemove->m_files.push_back(fileIter->toWideChar());
        TRACE_MESSAGE2("File scheduled for remove on next update '%S'", fileIter->toWideChar());
    }

    const tERROR writeToRegistryResult = g_root->RegSerialize(filesToRemove, FilesToRemove::eIID, registryKey, "");
    if(PR_FAIL(writeToRegistryResult))
    {
        TRACE_MESSAGE2("Failed to serialize files to remove structure, result '%s'",
            PRAGUE_HELPERS::toStringPragueResult(readFromRegistryResult).c_str());
    }
}

void CUpdater2005::updateInformationAboutLockedFiles(PragueCallbacks &callbacks)
{
    // switch is needed to access registry
    const std::string traceText = "Update information about locked files";
    callbacks.switchToSystemContext(traceText);
    updateInformationAboutLockedFilesInSystemContext(callbacks);
    callbacks.switchToUserContext(traceText);
}

tERROR CUpdater2005::sendImpersonatedMessage(const std::string &traceText,
    const tDWORD msg_cls, const tDWORD msg_id, tPTR ctx, tPTR buffer, tDWORD *size)
{
    switchToSystemContext(KLUPD::asciiToWideChar(traceText));
    const tERROR sysSendMessageResult = sysSendMsg(msg_cls, msg_id, ctx, buffer, size);
    switchToUserContext(KLUPD::asciiToWideChar(traceText));
    return sysSendMessageResult;
}

void CUpdater2005::switchToSystemContext(const KLUPD::NoCaseString &traceText)
{
    if(!m_hToken)
    {
        TRACE_MESSAGE2("Failed to switch to system context, token is not present, operation: %S",
            traceText.toWideChar());
        return;
    }

    const tERROR switchedToSystemContextResult = m_hToken->Revert();
    if(PR_SUCC(switchedToSystemContextResult))
    {
        TRACE_MESSAGE2("Switched to system context, operation: %S", traceText.toWideChar());
    }
    else
    {
        TRACE_MESSAGE3("Failed to switch to system context, result '%s', operation: %S",
            PRAGUE_HELPERS::toStringPragueResult(switchedToSystemContextResult).c_str(), traceText.toWideChar());
    }
}
void CUpdater2005::switchToUserContext(const KLUPD::NoCaseString &traceText)
{
    if(!m_hToken)
    {
        TRACE_MESSAGE2("Failed to switch to user context, token is not present, operation: %S",
            traceText.toWideChar());
        return;
    }

    const tERROR switchedToUserContextResult = m_hToken->Impersonate();
    if(PR_SUCC(switchedToUserContextResult))
    {
        TRACE_MESSAGE2("Switched to user context, operation: %S", traceText.toWideChar());
    }
    else
    {
        TRACE_MESSAGE3("Failed to switch to user context, result '%s', operation: %S",
            PRAGUE_HELPERS::toStringPragueResult(switchedToUserContextResult).c_str(), traceText.toWideChar());
    }
}

bool CUpdater2005::proxySettings(cProxySettings &proxySettings)
{
    const tERROR requestProxySettingsResult = sendImpersonatedMessage("Proxy server request message",
        pmc_TRANSPORT, pm_REQEST_PROXY_SETTINGS, 0, &proxySettings, SER_SENDMSG_PSIZE);
    if(PR_SUCC(requestProxySettingsResult))
        return true;

    TRACE_MESSAGE2("Failed to request proxy server settings, result '%s'",
        PRAGUE_HELPERS::toStringPragueResult(requestProxySettingsResult).c_str());
    return false;
}
// AVP Prague stamp begin( C++ class regitration,  )
// Interface "Task". Register function
tERROR CUpdater2005::Register( hROOT root ) 
{
    tERROR error;
// AVP Prague stamp end
// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(Task_PropTable)
    mSHARED_PROPERTY( pgINTERFACE_VERSION, Task_VERSION )
    mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "Updater new implementation", 27 )
    mpLOCAL_PROPERTY_BUF( pgTASK_TM, CUpdater2005, m_hTM, cPROP_BUFFER_READ_WRITE )
    mpLOCAL_PROPERTY_BUF( pgTASK_STATE, CUpdater2005, m_dwState, cPROP_BUFFER_READ_WRITE )
    mpLOCAL_PROPERTY_BUF( pgTASK_SESSION_ID, CUpdater2005, m_dwSessionID, cPROP_BUFFER_READ_WRITE )
    mpLOCAL_PROPERTY_BUF( pgTASK_ID, CUpdater2005, m_dwTaskID, cPROP_BUFFER_READ_WRITE )
    mpLOCAL_PROPERTY_BUF( pgTASK_PERSISTENT_STORAGE, CUpdater2005, m_hRegistre, cPROP_BUFFER_READ_WRITE )
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

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_TASK,                               // interface identifier
		PID_UPDATER,                        // plugin identifier
		0x00000000,                             // subtype identifier
		Task_VERSION,                           // interface version
		VID_GUSCHIN,                            // interface developer
		&i_Task_vtbl,                           // internal(kernel called) function table
		(sizeof(i_Task_vtbl)/sizeof(tPTR)),     // internal function table size
		&e_Task_vtbl,                           // external function table
		(sizeof(e_Task_vtbl)/sizeof(tPTR)),     // external function table size
		Task_PropTable,                         // property table
		mPROPERTY_TABLE_SIZE(Task_PropTable),   // property table size
		sizeof(CUpdater2005)-sizeof(cObjImpl),  // memory size
		0                                       // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"Task(IID_TASK) registered [%terr]",error) );
	#endif // _PRAGUE_TRACE_
// AVP Prague stamp end
// AVP Prague stamp begin( C++ class regitration end,  )
    return error;
}

tERROR pr_call Task_Register( hROOT root ) { return CUpdater2005::Register(root); }
// AVP Prague stamp end
// AVP Prague stamp begin( Caution !!!,  )
// You have to implement propetry: pgTASK_PARENT_ID
// AVP Prague stamp end
