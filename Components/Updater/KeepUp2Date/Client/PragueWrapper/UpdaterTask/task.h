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
// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __task_cpp__716f72cb_22e6_4af3_be2f_e54788d22244 )
#define __task_cpp__716f72cb_22e6_4af3_be2f_e54788d22244
// AVP Prague stamp end
// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_reg.h>
// AVP Prague stamp end
#include <Prague/iface/e_loader.h>
#include <Prague/iface/i_threadpool.h>
#include <Prague/iface/i_csect.h>
#include <Prague/iface/i_io.h>
#include <Prague/iface/i_event.h>
#include <Prague/iface/i_token.h>
#include <ProductCore/iface/i_licensing.h>
#include <Updater/updater.h>
#include <Prague/pr_cpp.h>
#include <Prague/pr_vector.h>

#define PROFILE_RETRANSLATION   "RetranslationCfg"

// forward declaration
namespace PRAGUE_HELPERS {
    class PragueLog;
}
class PragueCallbacks;

// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable CUpdater2005 : public cTask, public cThreadPoolBase, public cThreadTaskBase
{
private:
	CUpdater2005();
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
    tDWORD       m_dwState;     // --
    tDWORD       m_dwSessionID; // --
    tDWORD       m_dwTaskID;    // --
    hTASKMANAGER m_hTM;         // --
    hREGISTRY    m_hRegistre;   // --
// AVP Prague stamp end
private:
    virtual void do_work();
    // state [in] - current state
     // Note: use updater specific code as source for Updater Specific states, or product specific states
    void setState(const tDWORD state);
    void setCompletionState(const tDWORD &state, const KLUPD::CoreError &);

    // helper function: switch to system context when read data from registry is needed on Vista platforms
    void updateInformationAboutLockedFilesInSystemContext(PragueCallbacks &);
    // write information about locked files into Windows Registry
    void updateInformationAboutLockedFiles(PragueCallbacks &);

    // messages to product are sent under impersonation impersonated
    tERROR sendImpersonatedMessage(const std::string &traceText,
        const tDWORD msg_cls, const tDWORD msg_id, tPTR ctx, tPTR buffer, tDWORD *size);
    void switchToSystemContext(const KLUPD::NoCaseString &traceText);
    void switchToUserContext(const KLUPD::NoCaseString &traceText);
    bool proxySettings(cProxySettings &);



    tDWORD m_dwNewState;
    hCRITICAL_SECTION m_hCSNewState;
    hCRITICAL_SECTION m_hCSState;
    hCRITICAL_SECTION m_hCSStatistics;
    hCRITICAL_SECTION m_hCSSettings;
    hSYNC_EVENT m_hPauseEvent;
    cUpdaterStatistics m_Statistics;
    cUpdaterSettings m_Settings;
    tBOOL m_failedToUpdateAllComponents;
    hLICENSING m_hLic;
	cToken *m_hToken;
    PRAGUE_HELPERS::PragueLog *pLog;
    tBOOL m_interactionWithUserEnabled;

    hSYNC_EVENT                 m_SingletonEvent;           ///< ensures the only updater task is running at a time
    static const tWCHAR * const  s_szSingletonEventName;     ///< the name of singleton support event

    friend class PragueDownloadProgress;
    friend class PragueCallbacks;
    friend class PragueJournal;

// AVP Prague stamp begin( C++ class declaration end,  )
public:
    mDECLARE_INITIALIZATION(CUpdater2005)
};
// AVP Prague stamp end
// AVP Prague stamp begin( Header endif,  )
#endif // task_cpp
// AVP Prague stamp end
