// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Friday,  07 December 2007,  18:50 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Denisov
// File Name   -- as_trainsupport.cpp
// -------------------------------------------
// AVP Prague stamp end



#define PR_IMPEX_DEF

// AVP Prague stamp begin( Interface version,  )
#define AS_TrainSupport_VERSION ((tVERSION)1)
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>
#include <Prague/pr_vtbl.h>
#include <Mail/plugin/p_as_trainsupport.h>
// AVP Prague stamp end



#include <Prague/iface/i_objptr.h>
#include <Prague/iface/i_os.h>
#include <Prague/iface/i_threadpool.h>
#include <Extract/iface/i_mailmsg.h>
#include <Extract/plugin/p_mailmsg.h>
#include <Extract/plugin/p_mdb.h>
#include <Extract/plugin/p_msoe.h>
#include <Mail/plugin/p_mctask.h>
#include <Mail/structs/s_as_trainsupport.h>
#include <Mail/plugin/p_mctask.h>


#include <vector>
#include "OutLookAddrBook.h"
#include "../../../interceptors/OutlookPlugin/mapiedk/mapiedk.h"
#include "msoeapi.h"
#include "antispam_interface_imp2.h"
#include "GetUserNameByProcessID.h"


CMAPIEDK* g_pMAPIEDK = NULL;
tPROPID g_propMessageIsIncoming;
static tPROPID g_propMailerPID;

#define MICROSOFT_OUTLOOK			"Microsoft Office Outlook"
#define MICROSOFT_OUTLOOK_EXPRESS	"Microsoft Outlook Express"
#define MICROSOFT_WINDOWS_MAIL		"Microsoft Windows Mail"

#define ANTISPAM_TRAIN_BEGIN()	\
{	\
	m_bStartTraining = true;	\
	cAntispamTrainParams params; params.p_Action = cAntispamTrainParams::Train_Begin; params.p_dwPID = ::GetCurrentProcessId();	\
	error = sysSendMsg( pmc_ANTISPAM_TRAIN, NULL, NULL, &params, SER_SENDMSG_PSIZE );	\
}

#define ANTISPAM_TRAIN_END_APPLY()	\
{	\
	m_bStartTraining = false;	\
	cAntispamTrainParams params; params.p_Action = cAntispamTrainParams::Train_End_Apply;	params.p_dwPID = ::GetCurrentProcessId();	\
	error = sysSendMsg( pmc_ANTISPAM_TRAIN, NULL, NULL, &params, SER_SENDMSG_PSIZE );	\
}

#define ANTISPAM_TRAIN_END_MERGE()	\
{	\
	m_bStartTraining = false;	\
	cAntispamTrainParams params; params.p_Action = cAntispamTrainParams::MergeDatabase;	params.p_dwPID = ::GetCurrentProcessId();	\
	error = sysSendMsg( pmc_ANTISPAM_TRAIN, NULL, NULL, &params, SER_SENDMSG_PSIZE );	\
}

#define ANTISPAM_TRAIN_END_CANCEL()	\
if ( m_bStartTraining )	\
{	\
	m_bStartTraining = false;	\
	cAntispamTrainParams params; params.p_Action = cAntispamTrainParams::Train_End_Cancel;	params.p_dwPID = ::GetCurrentProcessId();	\
	error = sysSendMsg( pmc_ANTISPAM_TRAIN, NULL, NULL, &params, SER_SENDMSG_PSIZE );	\
}



struct CAS_TrainSupport;

class CTrainThread : public cThreadPoolBase, public cThreadTaskBase
{
public:

	CTrainThread();

	tERROR run(TrainSettings_t* pSettings, CAS_TrainSupport *pAS);

protected:
	void do_work();
	void TrainOnSentItems();

	TrainSettings_t       m_Settings;
	CAS_TrainSupport *    m_pAS;
};

class CTrainThreadSentItems : public cThreadPoolBase, public cThreadTaskBase
{
public:

	CTrainThreadSentItems();

	tERROR run(cAntispamSettings * pSettings, CAS_TrainSupport *pAS);

protected:
	void do_work();
	
	cAntispamSettings *	m_pSettings;
	CAS_TrainSupport *  m_pAS;
};


// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable CAS_TrainSupport : public cAS_TrainSupport {
private:
// Internal function declarations
	tERROR pr_call ObjectInit();
	tERROR pr_call ObjectInitDone();
	tERROR pr_call ObjectPreClose();
	tERROR pr_call MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len );

// Property function declarations

public:
// External function declarations
	tERROR pr_call GetFolderInfoList( FolderInfo_t* p_pFolderInfo, FolderInfoList_t* p_pFolderInfoList );
	tERROR pr_call Train( cSerializable* p_pFolderInfoList );
	tERROR pr_call GetMAPIEntryID( FolderInfo_t* p_pFolderInfo, EntryID_t* p_pEntryID );
	tERROR pr_call CreateSubFolder( FolderInfo_t* p_pFolderInfo, FolderInfo_t* p_pNewFolderInfo );
	tERROR pr_call TrainAsync( cSerializable* p_pFolderInfoList );
	tERROR pr_call TrainOnOutLookSentItems();
	tERROR pr_call TrainOnIntMailSentItems();
	tERROR pr_call TrainOnAllSentItemsAsync( cSerializable* p_Settings );

// Data declaration
	hTASKMANAGER m_TM;               // --
	tBOOL        m_bForceUISuppress; // --
	tBOOL        m_bNeedHamTrain;    // --
// AVP Prague stamp end



	friend CTrainThreadSentItems;

private:

	tERROR       sysCREATEPTR( IN FolderInfo_t* pFolderInfo );
	tBOOL        HasSubfolders(FolderInfo_t* pFolderInfo);
	tERROR       TrainOnFolder( IN FolderInfo_t* pFolder, IN bool bIsSpam, IN tUINT ulFolderNumber, IN tUINT ulFolderCount, tBOOL bForceAsSentMessage, OUT tUINT& ulMessageProcessedCount);

	tERROR TrainOnOLSentItemFolders(OUT tDWORD * pMsgCount);
	tERROR TrainOnIMSentItemFolders(OUT tDWORD * pMsgCount);
	tERROR TrainOnOLFolderByEntry(IN EntryID_t& entry, IN LPMDB pMsgStore, OUT tDWORD * pMsgCount);

	template <typename TComparator>
	tERROR TrainOnFolderByCmp(TComparator& comp, IN FolderInfo_t& folder, IN tBOOL bForceAsSentMessage, OUT tDWORD * pMsgCount);
	tERROR GetWinMailSentItemsName(cStringObj& name);
	tERROR GetFolderName(STOREFOLDERID folderId, IStoreNamespace * pStore, SPECIALFOLDER folderType, cStringObj& name);
	tERROR GetFolderInfoListDefProf( FolderInfo_t* p_pFolderInfo, FolderInfoList_t* p_pFolderInfoList );

	tERROR AntiSpamTrainBegin();
	tERROR AntiSpamTrainEndApply();
	tERROR AntiSpamTrainEndMerge();
	tERROR AntiSpamTrainEndCancel();

	tERROR TrainOnMessage(hOS os_message, cAntispamTrainParams& params);
	tERROR TrainOnAllSentItemsImpl( cSerializable* p_Settings );
	void GetDefaultMessageStore(LPMAPISESSION pSession, LPMDB * ppMsgStore);


	class COutLookCmp
	{
	public:
		COutLookCmp(LPMDB pMsgStore, const EntryID_t& entry): m_pMsgStore(pMsgStore), m_entry(entry){}
		tBOOL operator()(FolderInfo_t& folder)
		{
			ULONG ulRes = FALSE;
			m_pMsgStore->CompareEntryIDs(m_entry.cb, (LPENTRYID)m_entry.get_lpb(), folder.EntryID.cb, (LPENTRYID)folder.EntryID.get_lpb(), 0, &ulRes);
			return ulRes != FALSE ? cTRUE : cFALSE;
		}
	private:
		LPMDB m_pMsgStore;
		EntryID_t m_entry;
	};

	class COutLookExpressCmp
	{
	public:
		COutLookExpressCmp(FolderInfo_t::FolderType fldType):m_fldType(fldType){}
		tBOOL operator()(FolderInfo_t& folder)
		{
			return (m_fldType == folder.folderType) ? cTRUE : cFALSE;
		}
	private:
		const FolderInfo_t::FolderType m_fldType;
	};

	class CWindowsMailCmp
	{
	public:
		CWindowsMailCmp(const cStringObj& name):m_name(name){}
		tBOOL operator()(FolderInfo_t& folder)
		{
			return (m_name == folder.szName) ? cTRUE : cFALSE;
		}
	private:
		const cStringObj m_name;
	};


private:
	bool         m_bStartTraining;
	
	volatile bool m_bAbortTraining;
	volatile bool m_bSkipTraining;

	CTrainThread m_TrainTread;
	CTrainThreadSentItems m_TrainTreadSI;
	hOS          m_hOS_OU;
	hOS          m_hOS_OE;
	hObjPtr      m_objPtr_OU;
	hObjPtr      m_objPtr_OE;
	cStringObj   m_szProfile;
	bool		 m_bIsTaskPresent;
	hANTISPAMFILTER		m_hAntispamFilter;
	cAntispamSettings	m_settings;

	cStringObj m_WindowsIntegratedMailerName;

public:
	CAS_TrainSupport();

// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION(CAS_TrainSupport)
};
// AVP Prague stamp end



CAS_TrainSupport::CAS_TrainSupport() : 
	m_WindowsIntegratedMailerName((tBYTE)GetVersion() < 6 ? MICROSOFT_OUTLOOK_EXPRESS : MICROSOFT_WINDOWS_MAIL),
	m_hAntispamFilter(NULL), m_TM(NULL), m_bForceUISuppress(cFALSE)
{
}

// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "AS_TrainSupport". Static(shared) property table variables
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInit )
// Extended method comment
//    Kernel notifies an object about successful creating of object
// Behaviour comment
//    Initializes internal object data
// Result comment
//    Returns value differ from errOK if object cannot be initialized
// Parameters are:
tERROR CAS_TrainSupport::ObjectInit() {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "AS_TrainSupport::ObjectInit method" );

	// Place your code here
	m_WindowsIntegratedMailerName = ((tBYTE)GetVersion() < 6 ? MICROSOFT_OUTLOOK_EXPRESS : MICROSOFT_WINDOWS_MAIL);
	m_hAntispamFilter = NULL;
	m_TM = NULL;
	m_bForceUISuppress = cFALSE;
	m_bNeedHamTrain = cFALSE;

	return error;
}
// AVP Prague stamp end



tERROR CAS_TrainSupport::sysCREATEPTR( IN FolderInfo_t* pFolderInfo )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "CAS_TrainSupport::sysCREATEPTR method" );

	PR_RPT(("CAS_TrainSupport::sysCREATEPTR is deprecated and shouldn't be called"));

	return error;
}

#define sysCLOSEOBJECT(_obj) if (_obj){_obj->sysCloseObject(); _obj=NULL;}

// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
//! tERROR cAS_TrainSupport::ObjectInitDone() {
tERROR CAS_TrainSupport::ObjectInitDone() {
	tERROR error = errOK;	
	PR_TRACE_FUNC_FRAME( "AS_TrainSupport::ObjectInitDone method" );

	//////////////////////////////////////////////////////////////////////////
	//
	// Создаем hOS для обхода Аутлука
	//
	cERROR errMDB = errOK;
	if ( PR_SUCC(errMDB) )
		errMDB = sysCreateObject((hOBJECT*)&m_hOS_OU, IID_OS, PID_MDB, 0);
	if ( PR_SUCC(errMDB) )
		errMDB = m_hOS_OU->propSetDWord(pgOBJECT_OS_TYPE, MAIL_OS_TYPE_OUTLOOK);
	if ( PR_SUCC(errMDB))
		errMDB = m_hOS_OU->propSetBool(plMDB_IS_FORCE_UI_SUPPRESS, m_bForceUISuppress);
	if ( PR_SUCC(errMDB) )
		errMDB = m_hOS_OU->sysCreateObjectDone();
	if ( PR_FAIL(errMDB) )
		m_hOS_OU = NULL;
	if ( PR_SUCC(errMDB) )
		errMDB = m_hOS_OU->sysCreateObjectQuick((hOBJECT*)&m_objPtr_OU, IID_OBJPTR, PID_MDB, 0);
	if ( PR_FAIL(errMDB) )
	{
		PR_TRACE((g_root, prtERROR, "tas\tOutLook plugin object creation failed"));
		m_objPtr_OU = NULL;
	}

	//
	// Создаем hOS для обхода Аутлука
	//
	//////////////////////////////////////////////////////////////////////////

	
	//////////////////////////////////////////////////////////////////////////
	//
	// Создаем hOS для обхода OutlookExpress'а
	//
	cERROR errMSOE = errOK;
	if ( PR_SUCC(errMSOE) )
		errMSOE = sysCreateObject((hOBJECT*)&m_hOS_OE, IID_OS, PID_MSOE, 0);
	if ( PR_SUCC(errMSOE) )
		errMSOE = m_hOS_OE->propSetDWord(pgOBJECT_OS_TYPE, MAIL_OS_TYPE_SYSTEM);
	if ( PR_SUCC(errMSOE) )
		errMSOE = m_hOS_OE->sysCreateObjectDone();
	if ( PR_FAIL(errMSOE) )
		m_hOS_OE = NULL;
	if ( PR_SUCC(errMSOE) )
		errMSOE = m_hOS_OE->sysCreateObjectQuick((hOBJECT*)&m_objPtr_OE, IID_OBJPTR, PID_MSOE, 0);
	if ( PR_FAIL(errMSOE) )
		m_objPtr_OE = NULL;
	//
	// Создаем hOS для обхода OutlookExpress'а
	//
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Проверяем, запущена, ли AntiSpam Task
	
	m_bIsTaskPresent = true;

	if (m_TM)
	{
		// TaskManager установлен, можно посмотреть статус antiSpam task
		cProfileBase profileBase;
		tERROR errProfile = m_TM->GetProfileInfo(AVP_PROFILE_ANTISPAM, &profileBase);

		if (PR_SUCC(errProfile))
		{
			tTaskState taskState = profileBase.task_state();
			m_bIsTaskPresent = ((TASK_REQUEST_RUN & taskState) > 0);
		}
		else
		{
			PR_TRACE((g_root, prtERROR, "tas\tTask Manager GetProfileInfo() failed"));
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// Создаем Антиспам фильтр для тренировки и получения статистики
	m_hAntispamFilter = NULL;
	
	tERROR errFilter = sysCreateObject((hOBJECT*)&m_hAntispamFilter, IID_ANTISPAMFILTER, PID_ANTISPAMTASK);
	if (PR_SUCC(errFilter))
		errFilter = m_hAntispamFilter->sysCreateObjectDone();
	if (PR_SUCC(errFilter))
		errFilter = m_hAntispamFilter->SetSettings(&m_settings);
	if (PR_SUCC(errFilter))
		errFilter = m_hAntispamFilter->SetState((tTaskRequestState)TASK_STATE_RUNNING);
	if ( PR_FAIL(errFilter) )
	{
		PR_TRACE((g_root, prtERROR, "tas\tAntiSpam Filter sysCreateObject failed"));
		if (m_hAntispamFilter)
		{
			m_hAntispamFilter->sysCloseObject();
			m_hAntispamFilter = NULL;
		}
		return errOBJECT_CANNOT_BE_INITIALIZED;
	}

	// получим статистику
	// для определения необходимости обучения на Ham
	if (m_hAntispamFilter)
	{
		cAntispamStatistics antiSpamStatistics;
		if (PR_SUCC(m_hAntispamFilter->GetStatistics(&antiSpamStatistics)))
		{
			m_bNeedHamTrain = (0 == antiSpamStatistics.m_dwHamBaseSize);
		}
		else
		{
			PR_TRACE((g_root, prtERROR, "tas\tAntiSpam statistic extraction failed"));
		}
	}

	if (m_bIsTaskPresent)
	{
		PR_TRACE((g_root, prtNOTIFY, "tas\tAntiSpam task is found"));

		// Если Anti-Spam task запущена, то anti-spam фильтр нам больше не понадобится
		if (m_hAntispamFilter)
		{
			m_hAntispamFilter->sysCloseObject();
			m_hAntispamFilter = NULL;
		}

	}
	else
	{	
		PR_TRACE((g_root, prtNOTIFY, "tas\tAntiSpam task is not found"));
	}
	
	// Создаем Антиспам фильтр для тренировки и получения статистики
	//////////////////////////////////////////////////////////////////////////


	if (PR_FAIL(errMDB) && PR_FAIL(errMSOE))
		error = errOBJECT_CANNOT_BE_INITIALIZED;

	if(PR_SUCC(error))
		error = sysRegisterMsgHandler(pmc_AS_TRAIN_STATUS, rmhDECIDER, this, IID_ANY, PID_ANY, IID_ANY, PID_ANY);

	m_TrainTread.init(g_root);
	m_TrainTreadSI.init(g_root);

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:
//! tERROR cAS_TrainSupport::ObjectPreClose() {
tERROR CAS_TrainSupport::ObjectPreClose() {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "AS_TrainSupport::ObjectPreClose method" );

	m_bSkipTraining = true;

	m_TrainTread.wait_for_stop();
	m_TrainTread.de_init();

	m_TrainTreadSI.wait_for_stop();
	m_TrainTreadSI.de_init();

	// Place your code here
	sysCLOSEOBJECT(m_objPtr_OE);
	sysCLOSEOBJECT(m_hOS_OE);
	sysCLOSEOBJECT(m_objPtr_OU);
	sysCLOSEOBJECT(m_hOS_OU);
	sysCLOSEOBJECT(m_hAntispamFilter);

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, MsgReceive )
// Extended method comment
//    Receives message sent to the object or to the one of the object parents as broadcast
// Parameters are:
//   "msg_cls_id"    : Message class identifier
//   "msg_id"        : Message identifier
//   "send_point"    : Object where the message was send initially
//   "ctx"           : Context of the object processing
//   "receive_point" : Point of distributing message (tree top for RegisterMsgHandler call
//   "par_buf"       : Buffer of the parameters
//   "par_buf_len"   : Lenght of the buffer of the parameters
tERROR CAS_TrainSupport::MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len ) {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "AS_TrainSupport::MsgReceive method" );

	// Place your code here
	if(p_msg_cls_id == pmc_AS_TRAIN_STATUS)
	{
		switch(p_msg_id)
		{
		case pm_AS_CANCEL_TRAINING:
			m_bAbortTraining = m_bSkipTraining = true;
			m_TrainTread.wait_for_stop();
			m_TrainTreadSI.wait_for_stop();
			error = errOK_DECIDED;
			break;
		
		case pm_AS_SKIP_TRAINING:
			m_bSkipTraining = true;
			m_TrainTread.wait_for_stop();
			m_TrainTreadSI.wait_for_stop();
			error = errOK_DECIDED;
			break;
		
		case pm_AS_APPLY_RESULTS:
			AntiSpamTrainEndApply();
			error = errOK_DECIDED;
			break;
		
		case pm_AS_MERGE_RESULTS:
			AntiSpamTrainEndMerge();
			error = errOK_DECIDED;
			break;
		
		case pm_AS_CANCEL_RESULTS:
			AntiSpamTrainEndCancel();
			error = errOK_DECIDED;
			break;
		}
	}

	return error;
}
// AVP Prague stamp end



tBOOL CAS_TrainSupport::HasSubfolders(FolderInfo_t* pFolderInfo)
{
	tERROR err = errOK;
	tBOOL bResult = cFALSE;

	if ( !pFolderInfo->m_objPtr )
		return false;
	hObjPtr& objPtr = pFolderInfo->m_objPtr;
	if ( pFolderInfo->MDB.szName == MICROSOFT_OUTLOOK )
	{
		err = objPtr->propGet( NULL, pgSUBFOLDERS_COUNT, &bResult, sizeof(tBOOL) );
		if ( PR_FAIL(err) || bResult )
			return cTRUE;
		else
			return cFALSE;
	}
	else
	{
		if ( objPtr->get_pgIS_FOLDER() )
		{
			tBOOL bNeedToStepDown = (objPtr->get_pgOBJECT_PTR_STATE()==cObjPtrState_PTR);
			if ( bNeedToStepDown )
				objPtr->StepDown();
			if ( PR_SUCC(objPtr->Next()) && objPtr->get_pgIS_FOLDER() )
				bResult = cTRUE;
			if ( bNeedToStepDown )
				objPtr->StepUp();
		}
	}
	
	return bResult;
}

#ifdef ULRELEASE
#undef ULRELEASE
#define ULRELEASE(x) \
{                    \
	g_pMAPIEDK->pUlRelease((x));  \
	(x) = NULL;      \
}
#endif

#ifdef MAPIFREEBUFFER
#undef MAPIFREEBUFFER
#define MAPIFREEBUFFER(x) \
	if (x)	\
	{                         \
		g_pMAPIEDK->pMAPIFreeBuffer((x));  \
		(x) = NULL;           \
	}
#endif

#ifdef FREEPADRLIST
#undef FREEPADRLIST
#define FREEPADRLIST(x) \
{                       \
    g_pMAPIEDK->pFreePadrlist((x));  \
	(x) = NULL;         \
}
#endif

#ifdef FREEPROWS
#undef FREEPROWS
#define FREEPROWS(x)    \
{                       \
    g_pMAPIEDK->pFreeProws((x));     \
	(x) = NULL;         \
}
#endif

HRESULT GetCurrentProfileName(IN LPMAPISESSION lpSession, OUT cStringObj* szProfileName)
{
	if (!lpSession)
		return E_POINTER;

	if (!szProfileName)
		return E_POINTER;

	HRESULT hr = S_OK;

	LPMAPITABLE     lpStatusTable = NULL;
	SRestriction    sres;
	SPropValue      spvResType;
	LPSRowSet       pRows = NULL;
	LPTSTR          lpszProfileName = NULL;
	SizedSPropTagArray(2, Columns) =
	{
		2, 
		PR_DISPLAY_NAME, 
		PR_RESOURCE_TYPE
	};

	hr = lpSession->GetStatusTable(NULL, &lpStatusTable);
	if ( SUCCEEDED(hr) ) 
		hr = lpStatusTable->SetColumns((LPSPropTagArray)&Columns, NULL);

	if ( SUCCEEDED(hr) ) 
	{
		spvResType.ulPropTag = PR_RESOURCE_TYPE;
		spvResType.Value.ul = MAPI_SUBSYSTEM;
		
		sres.rt = RES_PROPERTY;
		sres.res.resProperty.relop = RELOP_EQ;
		sres.res.resProperty.ulPropTag = PR_RESOURCE_TYPE;
		sres.res.resProperty.lpProp = &spvResType;
		
		if ( SUCCEEDED(hr) ) 
			hr = lpStatusTable->Restrict(&sres, TBL_ASYNC);
		if ( SUCCEEDED(hr) ) 
			hr = lpStatusTable->FindRow(&sres, BOOKMARK_BEGINNING, 0);
		if ( SUCCEEDED(hr) ) 
			hr = lpStatusTable->QueryRows(1,0,&pRows);
		if (SUCCEEDED(hr))
		{
			lpszProfileName = pRows->aRow[0].lpProps[0].Value.lpszA;
			*szProfileName = lpszProfileName;
		} 
		
	}
   
	ULRELEASE(lpStatusTable);
	FREEPROWS(pRows);
	return hr;
}

class cMAPIInitializer
{
	HRESULT m_hr;
public:
	cMAPIInitializer()
	{
		if(GetMAPIEDK(g_pMAPIEDK, NULL) && g_pMAPIEDK)
		{
			MAPIINIT_0 MapiInit = {MAPI_INIT_VERSION, MAPI_MULTITHREAD_NOTIFICATIONS};
			m_hr = g_pMAPIEDK->pMAPIInitialize(&MapiInit);
		}
		else
			m_hr = E_FAIL;
	}
	~cMAPIInitializer()
	{
		if(g_pMAPIEDK)
			g_pMAPIEDK->pMAPIUninitialize();
	}
	bool IsInitialized() const
	{
		return SUCCEEDED(m_hr);
	}
};

// AVP Prague stamp begin( External (user called) interface method implementation, GetFolderInfoList )
// Parameters are:
//! tERROR cAS_TrainSupport::GetFolderInfoList( FolderInfo_t* p_pFolderInfo, FolderInfoList_t* p_pFolderInfoList ) {
tERROR CAS_TrainSupport::GetFolderInfoList( FolderInfo_t* p_pFolderInfo, FolderInfoList_t* p_pFolderInfoList ) {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "AS_TrainSupport::GetFolderInfoList method" );

	cMAPIInitializer MAPIInitializer;

	// Place your code here
	tBOOL bNeedToStepDown = cFALSE;

	if (
		!p_pFolderInfo ||
		(
		  !p_pFolderInfo->m_objPtr &&
		  !p_pFolderInfo->MDB.m_objPtr &&
		  p_pFolderInfo->szName.empty() &&
		  p_pFolderInfo->MDB.szName.empty()
		)
		)
	{
		// Создаем два корня для деревьев
		cERROR errMDB = errNOT_OK;
		if ( m_objPtr_OU )
		{
			FolderInfo_t Outlook;
			errMDB = Outlook.SetObjPtr(m_objPtr_OU);
			if ( PR_SUCC(errMDB) )
				errMDB = Outlook.MDB.SetObjPtr(m_objPtr_OU);
			if ( PR_SUCC(errMDB) )
			{
				Outlook.MDB.szName = MICROSOFT_OUTLOOK;
				Outlook.szName = MICROSOFT_OUTLOOK;
				Outlook.bHasChildren = cTRUE;
				p_pFolderInfoList->push_back(Outlook);
			}
		}

		cERROR errMSOE = errNOT_OK;
		if ( m_objPtr_OE )
		{
			FolderInfo_t OutlookExpress;
			errMSOE = OutlookExpress.SetObjPtr(m_objPtr_OE);
			if ( PR_SUCC(errMSOE) )
				errMSOE = OutlookExpress.MDB.SetObjPtr(m_objPtr_OE);
			if ( PR_SUCC(errMSOE) )
			{
				OutlookExpress.MDB.szName = m_WindowsIntegratedMailerName;
				OutlookExpress.szName = m_WindowsIntegratedMailerName;
				OutlookExpress.bHasChildren = cTRUE;
				p_pFolderInfoList->push_back(OutlookExpress);
			}
		}

		if ( PR_FAIL(errMDB) && PR_FAIL(errMSOE) )
			error = errNOT_OK;
		return error;
	}

	if ( !p_pFolderInfo->m_objPtr )
	{
		if(!p_pFolderInfo->MDB.m_objPtr)
		{
			if ( m_objPtr_OU && p_pFolderInfo->MDB.szName == MICROSOFT_OUTLOOK )
				m_objPtr_OU->Clone(&p_pFolderInfo->MDB.m_objPtr);
			else if ( m_objPtr_OE && p_pFolderInfo->MDB.szName == m_WindowsIntegratedMailerName )
				m_objPtr_OE->Clone(&p_pFolderInfo->MDB.m_objPtr);
		}
		error = p_pFolderInfo->MDB.m_objPtr->Clone(&p_pFolderInfo->m_objPtr);
		if ( PR_SUCC(error) )
			error = p_pFolderInfo->m_objPtr->ChangeTo(cAutoString(p_pFolderInfo->szName));
	}

	if ( !p_pFolderInfo->m_objPtr )
	{
		PR_RPT(("Failed to get ObjPtr, err = 0x%08x", error));
		return error;
	}

	if (
		PR_SUCC(error) &&
		(p_pFolderInfo->szName == MICROSOFT_OUTLOOK) &&
		(p_pFolderInfo->MDB.szName == MICROSOFT_OUTLOOK) &&
		!p_pFolderInfo->EntryID.cb
		)
	{
		// пробуем получить список профилей OutLook
		MAPIAccess::COutLookAddrBook::Profiles profiles;
		try
		{
			MAPIAccess::COutLookAddrBook addrbook;
			addrbook.GetProfiles(profiles);
		}
		catch (const MAPIAccess::CMAPIException&)
		{
			PR_TRACE((g_root, prtERROR, "tas\tCAS_TrainSupport::GetFolderInfoList MAPIAccess::CMAPIException is catched"));
		}

		if (1 == profiles.size())
		{
			// если профиль один, то его и открываем
			m_szProfile = profiles[0].first.c_str();
			p_pFolderInfo->m_objPtr->ChangeTo(cAutoString (m_szProfile));
			p_pFolderInfo->MDB.m_objPtr->ChangeTo(cAutoString (m_szProfile));
		}
		else
		{
			// пользователь в диалоге сам выбирает профиль
			// Логонимся в MAPI и передаем имя МАПИшного профайла для m_szProfile
			HRESULT			hr = E_FAIL;
			LPMAPISESSION   pMAPISession = NULL;
			if ( MAPIInitializer.IsInitialized() )
				hr = g_pMAPIEDK->pMAPILogonEx(
				(ULONG)::GetActiveWindow(),
				TEXT(""),
				TEXT(""),
				MAPI_EXTENDED | MAPI_LOGON_UI,
				&pMAPISession);
			if ( SUCCEEDED(hr) )
			{
				MSG message;
				while( PeekMessage(&message, NULL, 0, 0, PM_REMOVE) )
				{
					TranslateMessage( &message );
					DispatchMessage( &message );
				}
				hr = GetCurrentProfileName(pMAPISession, &m_szProfile);
				pMAPISession->Logoff(0, 0, 0);
				ULRELEASE(pMAPISession);
				if ( SUCCEEDED(hr) )
				{
					p_pFolderInfo->m_objPtr->ChangeTo(cAutoString (m_szProfile));
					p_pFolderInfo->MDB.m_objPtr->ChangeTo(cAutoString (m_szProfile));
				}
			}
			else
				return error = errNOT_OK;
		}
		
	}

	hObjPtr objPtr = p_pFolderInfo->m_objPtr;

	bool bNeedCloseClone = false;
	if ( PR_SUCC(error) && objPtr )
	{
		bNeedToStepDown = objPtr->get_pgOBJECT_PTR_STATE() == cObjPtrState_PTR;
		if ( bNeedToStepDown )
		{
			// из-за ошибок в MDB StepDown и обход делаем клоном, не трогая p_pFolderInfo->m_objPtr
			if(objPtr->propGetPID() == PID_MDB)
			{
				error = objPtr->Clone(&objPtr);
				if ( PR_SUCC(error) )
					bNeedCloseClone = true;
				else
					objPtr = p_pFolderInfo->m_objPtr;
			}
			error = objPtr->StepDown();
		}
	}

	if ( PR_SUCC(error) && objPtr )
	{
		while ( PR_SUCC(error = objPtr->Next()) && objPtr->get_pgIS_FOLDER() )
		{
			ObjectInfo_t oiFolder;
			error = oiFolder.szName.assign(objPtr, pgOBJECT_NAME);
			if ( PR_SUCC(error) )
			{
				if(objPtr->propGetPID() == PID_MDB)
				{
					EntryID_t EntryID;
					error = objPtr->propGet((tDWORD*)&EntryID.cb, pgMAIL_OBJECT_ENTRYID, NULL, NULL);
					if(PR_SUCC(error) && EntryID.cb)
						error = objPtr->propGet(NULL, pgMAIL_OBJECT_ENTRYID, EntryID.buff.get(EntryID.cb, false), EntryID.cb);
					if(PR_SUCC(error))
					{
						oiFolder.EntryID.cb = *(tDWORD*)(EntryID.buff.ptr());
						oiFolder.EntryID.buff.set(oiFolder.EntryID.cb, ((EntryID.buff.ptr()) + sizeof(tDWORD)));
					}
				}
				else
				{
					cStringObj str; error = str.assign(objPtr, pgOBJECT_FULL_NAME);
					if ( PR_SUCC(error) )
					{
						oiFolder.EntryID.cb = str.memsize(cCP_UNICODE)+2;
						oiFolder.EntryID.buff.set( oiFolder.EntryID.cb, (tBYTE*)(tWCHAR*)str.c_str(cCP_UNICODE) );
					}
				}
			}
			if ( PR_SUCC(error) )
			{
				FolderInfo_t FolderInfo(&p_pFolderInfo->MDB, &oiFolder);
				error = FolderInfo.SetObjPtr(objPtr);
				FolderInfo.bCanCreateSubfolders = objPtr->propGetBool(pgCAN_CREATE_SUBFOLDERS);
				FolderInfo.bCanHaveMessages = objPtr->propGetBool(pgIS_MESSAGE_STORAGE);
				FolderInfo.bHasChildren =
					p_pFolderInfo->szName.empty()
					?
					cTRUE : HasSubfolders(&FolderInfo);
				FolderInfo.folderType = (FolderInfo_t::FolderType)objPtr->propGetDWord(pgFOLDER_TYPE);
				p_pFolderInfoList->push_back(FolderInfo);
			}
		}
	}

	if ( error == errEND_OF_THE_LIST )
		error = errOK;

	PR_ASSERT(PR_SUCC(error));

	if ( bNeedToStepDown && PR_SUCC(error) && objPtr && objPtr->propGetPID() != PID_MDB )
		error = objPtr->StepUp();

	if ( bNeedCloseClone && objPtr )
		objPtr->sysCloseObject();

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Train )
// Parameters are:
//! tERROR cAS_TrainSupport::Train( TrainFolderInfoList_t* p_pFolderInfoList ) {
//! tERROR CAS_TrainSupport::Train( TrainFolderInfoList_t* p_pFolderInfoList ) {
tERROR CAS_TrainSupport::Train( cSerializable* p_pFolderInfoList ) {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "AS_TrainSupport::Train method" );

	TrainSettings_t *pSett = (TrainSettings_t *)p_pFolderInfoList;
	if(!pSett->isBasedOn(TrainSettings_t::eIID))
		return errPARAMETER_INVALID;

	// Place your code here

	m_bStartTraining = m_bAbortTraining = m_bSkipTraining = false;

	hObjPtr objPtr = NULL;
	FolderInfo_t* pFolderInfo = NULL;
	tULONG ulLoop = 0;
	tULONG ulCount = pSett->m_Folders.size();
	tUINT  ulMessagesProcessedCount = 0;
	
	AntiSpamTrainBegin();
	
	for ( ulLoop = 0; ulLoop < ulCount; ulLoop++ )
	{
		error = TrainOnFolder(
			&pSett->m_Folders.at(ulLoop),
			(pSett->m_Folders.at(ulLoop).m_FolderState == TrainFolderInfo_t::FolderSPAM),
			ulLoop,
			ulCount,
			cFALSE,
			ulMessagesProcessedCount
			);
		if ( PR_FAIL(error) )
			break;
	}

	if(m_bAbortTraining)
		AntiSpamTrainEndCancel();

	TrainStatus_t TrainStatus;
	TrainStatus.ulMessagesCount = ulMessagesProcessedCount;
	TrainStatus.ulMessagesProcessed	= ulMessagesProcessedCount;
	sysSendMsg(pmc_AS_TRAIN_STATUS, pm_AS_TRAIN_STATUS, 0, &TrainStatus, SER_SENDMSG_PSIZE);
	sysSendMsg(pmc_AS_TRAIN_STATUS, pm_AS_TRAIN_STATUS, 0, 0, 0);

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetMAPIEntryID )
// Parameters are:
//! tERROR CAS_TrainSupport::GetMAPIEntryID( FolderInfo_t* p_pFolderInfo, tDWORD* p_pEntryID ) {
tERROR CAS_TrainSupport::GetMAPIEntryID( FolderInfo_t* p_pFolderInfo, EntryID_t* p_pEntryID ) {
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "AS_TrainSupport::GetMAPIEntryID method" );
	
	PR_RPT(("CAS_TrainSupport::GetMAPIEntryID is deprecated and shouldn't be called"));

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, CreateSubFolder )
// Parameters are:
tERROR CAS_TrainSupport::CreateSubFolder( FolderInfo_t* p_pFolderInfo, FolderInfo_t* p_pNewFolderInfo ) {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "AS_TrainSupport::CreateSubFolder method" );
	
	if ( !p_pFolderInfo )
		return errOBJECT_NOT_FOUND;
	if ( !p_pNewFolderInfo )
		return errOBJECT_NOT_FOUND;

	cStringObj szName;
	error = szName.assign(p_pNewFolderInfo->szName);

	if ( PR_SUCC(error) )
		if ( !p_pFolderInfo->m_objPtr )
			error = sysCREATEPTR( p_pFolderInfo );
	if ( PR_SUCC(error) )
	{
		hObjPtr& objPtr = p_pFolderInfo->m_objPtr;
		if ( PR_SUCC(error) )
			error = objPtr->propGetBool(pgCAN_CREATE_SUBFOLDERS) ? errOK : errOBJECT_INCOMPATIBLE;
		if ( PR_SUCC(error) )
			error = objPtr->IOCreate(
				NULL,
				cAutoString (szName),
				fACCESS_READ,
				fOMODE_CREATE_IF_NOT_EXIST
				);
		if ( PR_SUCC(error) )
		{
			// Записываем в p_pNewFolderInfo указатели на новую папку
			*p_pNewFolderInfo = *p_pFolderInfo;
			error = p_pNewFolderInfo->m_objPtr->ChangeTo(cAutoString (szName));
			if ( PR_SUCC(error) )
				error = p_pNewFolderInfo->szName.assign(szName);
			if ( PR_SUCC(error) )
			{
				cStringObj str; error = str.assign(p_pNewFolderInfo->m_objPtr, pgOBJECT_FULL_NAME);
				if ( PR_SUCC(error) )
				{
					p_pNewFolderInfo->EntryID.cb = str.memsize(cCP_UNICODE)+2;
					p_pNewFolderInfo->EntryID.buff.set(
						p_pNewFolderInfo->EntryID.cb,
						(tBYTE*)(tWCHAR*)str.c_str(cCP_UNICODE)
						);
					p_pNewFolderInfo->bCanCreateSubfolders = p_pNewFolderInfo->m_objPtr->propGetBool(pgCAN_CREATE_SUBFOLDERS);
					p_pNewFolderInfo->bCanHaveMessages = p_pNewFolderInfo->m_objPtr->propGetBool(pgIS_MESSAGE_STORAGE);
					p_pNewFolderInfo->bHasChildren = cFALSE;
				}
			}
		}
	}

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, TrainAsync )
// Parameters are:
//! tERROR CAS_TrainSupport::TrainAsync( TrainFolderInfoList_t* p_pFolderInfoList )
tERROR CAS_TrainSupport::TrainAsync( cSerializable* p_pFolderInfoList )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "AS_TrainSupport::TrainAsync method" );

	if(!p_pFolderInfoList->isBasedOn(TrainSettings_t::eIID))
		return errPARAMETER_INVALID;

	return m_TrainTread.run((TrainSettings_t *)p_pFolderInfoList, this);
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, TrainOnOutLookSentItems )
// Parameters are:
//! tERROR CAS_TrainSupport::TrainOnOutLookSentItems( tDWORD* p_MsgCount ) {
tERROR CAS_TrainSupport::TrainOnOutLookSentItems() {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "AS_TrainSupport::TrainOnOutLookSentItems method" );

	AntiSpamTrainBegin();

	tDWORD dwMsgCount = 0;
	error = TrainOnOLSentItemFolders(&dwMsgCount);

	if(m_bAbortTraining)
		AntiSpamTrainEndCancel();
	
	TrainStatus_t TrainStatus;
	TrainStatus.ulMessagesCount = dwMsgCount;
	TrainStatus.ulMessagesProcessed	= dwMsgCount;
	sysSendMsg(pmc_AS_TRAIN_STATUS, pm_AS_TRAIN_STATUS, 0, &TrainStatus, SER_SENDMSG_PSIZE);
	sysSendMsg(pmc_AS_TRAIN_STATUS, pm_AS_TRAIN_STATUS, 0, 0, 0);

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, TrainOnIntMailSentItems )
// Parameters are:
//! tERROR CAS_TrainSupport::TrainOnIntMailSentItems( tDWORD* p_MsgCount ) {
tERROR CAS_TrainSupport::TrainOnIntMailSentItems() {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "AS_TrainSupport::TrainOnIntMailSentItems method" );

	AntiSpamTrainBegin();

	tDWORD dwMsgCount = 0;
	error = TrainOnIMSentItemFolders(&dwMsgCount);

	if(m_bAbortTraining)
		AntiSpamTrainEndCancel();

	TrainStatus_t TrainStatus;
	TrainStatus.ulMessagesCount = dwMsgCount;
	TrainStatus.ulMessagesProcessed	= dwMsgCount;
	sysSendMsg(pmc_AS_TRAIN_STATUS, pm_AS_TRAIN_STATUS, 0, &TrainStatus, SER_SENDMSG_PSIZE);
	sysSendMsg(pmc_AS_TRAIN_STATUS, pm_AS_TRAIN_STATUS, 0, 0, 0);

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, TrainOnAllSentItemsAsync )
// Parameters are:
tERROR CAS_TrainSupport::TrainOnAllSentItemsAsync( cSerializable* p_Settings ) {
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "AS_TrainSupport::TrainOnAllSentItemsAsync method" );

	if (!p_Settings)
		return errPARAMETER_INVALID;

	if (p_Settings->isBasedOn(cAntispamSettings::eIID))
		m_settings.assign(*p_Settings, false);
	else
		return errPARAMETER_INVALID;

	if (!m_bIsTaskPresent)
	{
		error = m_hAntispamFilter->SetSettings(&m_settings);
		if (PR_SUCC(error))
			error = m_hAntispamFilter->SetState((tTaskRequestState)TASK_STATE_RUNNING);
		if (PR_FAIL(error))
			return errNOT_OK;
	}
	
	error = m_TrainTreadSI.run((cAntispamSettings*)p_Settings, this);

	return error;
}
// AVP Prague stamp end



tERROR CAS_TrainSupport::TrainOnAllSentItemsImpl( cSerializable* p_Settings )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "AS_TrainSupport::TrainOnAllSentItemsImpl method" );

	m_bAbortTraining = m_bSkipTraining = false;
	
	AntiSpamTrainBegin();

	tDWORD dwMsgCount = 0;
	error = TrainOnOLSentItemFolders(&dwMsgCount);
	if (PR_FAIL(error))
	{
		PR_TRACE((g_root, prtERROR, "tas\tAS_TrainSupport::TrainOnOLSentItemFolders failed"));
	}
		
	if (!m_bAbortTraining && !m_bSkipTraining && (dwMsgCount < m_settings.m_dMinHamLettersCount))
	{
		error = TrainOnIMSentItemFolders(&dwMsgCount);
		if (PR_FAIL(error))
		{
			PR_TRACE((g_root, prtERROR, "tas\tAS_TrainSupport::TrainOnIMSentItemFolders failed"));
		}
	}

	if(m_bAbortTraining)
		AntiSpamTrainEndCancel();
	else
		AntiSpamTrainEndMerge();


	TrainStatus_t TrainStatus;
	TrainStatus.ulMessagesCount = dwMsgCount;
	TrainStatus.ulMessagesProcessed	= dwMsgCount;
	sysSendMsg(pmc_AS_TRAIN_STATUS, pm_AS_TRAIN_STATUS, 0, &TrainStatus, SER_SENDMSG_PSIZE);
	sysSendMsg(pmc_AS_TRAIN_STATUS, pm_AS_TRAIN_STATUS, 0, 0, 0);

	if (!m_bIsTaskPresent)
		p_Settings->assign(m_settings, false);

	return error;
}


void CAS_TrainSupport::GetDefaultMessageStore(LPMAPISESSION pSession, LPMDB * ppMsgStore)
{
	if (!pSession || !ppMsgStore)
		return;
	
	*ppMsgStore = NULL;

	LPMAPITABLE pMapiTable = NULL;
	if (S_OK == pSession->GetMsgStoresTable(0, &pMapiTable))
	{
		ULONG ulRowCount = 0;
		pMapiTable->GetRowCount(0, &ulRowCount);

		LPSRowSet pRowSet = NULL;
	
		if (SUCCEEDED(pMapiTable->QueryRows(ulRowCount, 0, &pRowSet)))
		{
			for (ULONG i = 0; i < pRowSet->cRows; ++i)
			{
				if (LPSPropValue pPVN = g_pMAPIEDK->pPpropFindProp(pRowSet->aRow[i].lpProps, pRowSet->aRow[i].cValues, PR_DEFAULT_STORE))
				{
					if (pPVN->Value.b)
					{
						// нашли дефолтное хранилище
						if (LPSPropValue pPVN = g_pMAPIEDK->pPpropFindProp(pRowSet->aRow[i].lpProps, pRowSet->aRow[i].cValues, PR_ENTRYID))
						{
							SBinary entry = pPVN->Value.bin;
							ULONG ulObjType = 0;
							LPUNKNOWN pUnk = NULL;
							pSession->OpenEntry(entry.cb, (LPENTRYID)entry.lpb, NULL, MAPI_BEST_ACCESS, &ulObjType, &pUnk);

							if (MAPI_STORE == ulObjType && pUnk)
							{
								*ppMsgStore = (LPMDB)pUnk;								
								(*ppMsgStore)->AddRef();
							}
							
							if (pUnk)
								pUnk->Release();							
						}

						break;
					}
				}
			}

			g_pMAPIEDK->pFreeProws(pRowSet);
		}
	}
	else
	{
		PR_TRACE((g_root, prtERROR, "tas\tAS_TrainSupport::GetDefaultMessageStore GetMsgStoresTable() failed"));
	}

	if (pMapiTable)
		pMapiTable->Release();
}


tERROR CAS_TrainSupport::TrainOnFolder(
		IN FolderInfo_t* pFolder, 
		IN bool bIsSpam, 
		IN tUINT ulFolderNumber,
		IN tUINT ulFolderCount,
		IN tBOOL bForceAsSentMessage,
		OUT tUINT& ulMessageProcessedCount		
		)
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "AS_TrainSupport::TrainOnFolder method" );

	const tDWORD dwMaxTrainMessages = pFolder->dwMaxTrainMessages;
	tDWORD dwTrainMessages = 0;
	hObjPtr& objPtr = pFolder->m_objPtr;
	
	PR_ASSERT(objPtr);

	if ( !objPtr )
		return errNOT_OK;

	tBOOL bNeedToStepDown = (objPtr->get_pgOBJECT_PTR_STATE()==cObjPtrState_PTR);
	if ( bNeedToStepDown )
		error = objPtr->StepDown();
	
	if ( PR_SUCC(error) && !m_bSkipTraining)
	{
		tULONG ulLoop = 0;
		tULONG ulCount = 0;
		// подсчитаем письма
		while ( PR_SUCC(error = objPtr->Next()) && (/*!bIsSpam || */!dwTrainMessages || dwTrainMessages < dwMaxTrainMessages) )
		{
			if ( objPtr->get_pgIS_FOLDER() )
				continue;
			ulCount++;
			dwTrainMessages++;

			// Сообщаем GUI, что мы не висим, а работаем
			TrainStatus_t TrainStatus;
			TrainStatus.ulMessagesCount = ulCount;
			TrainStatus.ulMessagesProcessed	= 0;
			TrainStatus.szCurrentFolder	= pFolder->szName;
			TrainStatus.szCurrentMessage = "...preparing...";
			sysSendMsg(pmc_AS_TRAIN_STATUS, pm_AS_TRAIN_STATUS, 0, &TrainStatus, SER_SENDMSG_PSIZE);
			if(m_bSkipTraining)
			{
				error = errNOT_OK;
				break;
			}
		}
		
		if ( error == errEND_OF_THE_LIST )
			error = errOK;
		if ( PR_SUCC(error) )
			error = objPtr->Reset(cFALSE);
		dwTrainMessages = 0;
		
		// теперь переберем письма
		if ( PR_SUCC(error) && !m_bSkipTraining)
		{
			tULONG ulTotal = 0; // ограничим перебор ulCount, так как при тренировке на IMAP папке 
								// могут появляться новые сообщения с модифицированной темой, а оригинальные
								// будут перечеркнуты
			while ( PR_SUCC(error = objPtr->Next()) && ulTotal < ulCount && ( /*!bIsSpam ||*/ (dwTrainMessages < dwMaxTrainMessages) ))
			{
				if ( objPtr->get_pgIS_FOLDER() )
					continue;
				dwTrainMessages++;
				ulTotal++;
				
				hIO io_temp = NULL;
				error = objPtr->IOCreate(&io_temp, 0, fACCESS_READ, fOMODE_OPEN_IF_EXIST);
				if ( PR_SUCC(error) )
				{
					hOS os_message;
					tPID pid_message = (pFolder->MDB.m_objPtr->propGetPID() == PID_MDB) ? PID_MAILMSG : PID_MSOE;
					error = io_temp->sysCreateObjectQuick( (hOBJECT*)&os_message, IID_OS, pid_message );
					if ( PR_SUCC(error) )
					{
						cAntispamStatisticsEx statistics;
						cAntispamTrainParams params;
						params.p_pStat = &statistics;
						params.p_dwPID = ::GetCurrentProcessId();
						params.p_Action = bIsSpam ? 
							cAntispamTrainParams::TrainAsSpam 
							: 
							cAntispamTrainParams::TrainAsHam;
						cStringObj szMessageSubject;
						szMessageSubject.assign(os_message, pgOBJECT_NAME);

						if (bForceAsSentMessage)
							os_message->propSetBool(g_propMessageIsIncoming, cFALSE);
						else
							os_message->propSetBool(g_propMessageIsIncoming, !bIsSpam && (dwTrainMessages <= dwMaxTrainMessages) );
						
						
						error = TrainOnMessage(os_message, params);

						if ( PR_FAIL(error) )
							break;
						if ( params.p_pStat->m_dwLastMessageCategory == cAntispamStatisticsEx::IsUnknown )
						{
							error = errNOT_OK;
							break;
						}

						TrainStatus_t TrainStatus;
						TrainStatus.ulMessagesCount = ulCount;
						TrainStatus.ulMessagesProcessed	= dwTrainMessages;
						TrainStatus.szCurrentFolder	= pFolder->szName;
						TrainStatus.szCurrentMessage = szMessageSubject;
						sysSendMsg(pmc_AS_TRAIN_STATUS, pm_AS_TRAIN_STATUS, 0, &TrainStatus, SER_SENDMSG_PSIZE);
						if(m_bSkipTraining)
						{
							error = errNOT_OK;
							break;
						}
						if ( params.p_pStat->m_dwLastReason != asdr_Training )
						{
							dwTrainMessages--;
							ulLoop--;
						}
					}
					sysCLOSEOBJECT(os_message);
					ulLoop++;
				}
				sysCLOSEOBJECT(io_temp);
			}
		}
		ulMessageProcessedCount += dwTrainMessages;
	}
		
	if ( error == errEND_OF_THE_LIST )
		error = errOK;
	if ( objPtr && bNeedToStepDown )
		objPtr->StepUp();
	return error;
}

// тренировка на папке отправленных "большого" OutLook
tERROR CAS_TrainSupport::TrainOnOLSentItemFolders(OUT tDWORD * pMsgCount)
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "AS_TrainSupport::TrainOnOLSentItemFolders method" );

	error = errNOT_FOUND;

	if (!pMsgCount)
		return errPARAMETER_INVALID;

	if (!m_objPtr_OU)
		return error;

	MAPIAccess::COutLookAddrBook::Profiles profiles;
	try
	{
		MAPIAccess::COutLookAddrBook addrbook;
		addrbook.GetProfiles(profiles);
	}
	catch (const MAPIAccess::CMAPIException&)
	{
		PR_TRACE((g_root, prtERROR, "tas\tCAS_TrainSupport::TrainOnOLSentItemFolders MAPIAccess::CMAPIException is catched"));
	}

	// не найдено ни одного профля
	if (profiles.empty())
		return errNOT_FOUND;

	// находим профиль поумолчанию
	MAPIAccess::COutLookAddrBook::Profiles::const_iterator it = profiles.begin();
	MAPIAccess::COutLookAddrBook::Profiles::const_iterator it_end = profiles.end();

	for (; it != it_end; ++it)
	{
		if (it->second)
		{
			m_szProfile = it->first.c_str();
			break;
		}
	}

	cMAPIInitializer initializer;
	if (!initializer.IsInitialized())
		return errNOT_FOUND;

	LPMAPISESSION pSession = NULL;
	
	PR_TRACE((g_root, prtIMPORTANT, "tas\tTrainOnOLSentItemFolders use profile %S", m_szProfile.data() ? m_szProfile.data() : L""));

	// используем профиль по умолчанию
	if (S_OK == g_pMAPIEDK->pMAPILogonEx(0, m_szProfile.c_str(cCP_ANSI), TEXT(""), MAPI_EXTENDED | /*MAPI_LOGON_UI |*/ MAPI_ALLOW_OTHERS | MAPI_NEW_SESSION, &pSession))
	{
		LPMDB pMsgStore = NULL;
		GetDefaultMessageStore(pSession, &pMsgStore);

		if (pMsgStore)
		{
			SizedSPropTagArray(1, sptSlots) = {1, PR_IPM_SENTMAIL_ENTRYID};

			ULONG ulValues = 0;
			LPSPropValue pPropValue = NULL;
			// Находим entry папки отправленных сообщений
			if (S_OK == pMsgStore->GetProps((LPSPropTagArray)&sptSlots, 0, &ulValues, &pPropValue))
			{
				EntryID_t entry;
				entry = pPropValue[0].Value.bin;
				error = TrainOnOLFolderByEntry(entry, pMsgStore, pMsgCount);
			}

			if (pPropValue)
				g_pMAPIEDK->pMAPIFreeBuffer(pPropValue);
			
			pMsgStore->Release();
		}
		else
		{
			PR_TRACE((g_root, prtERROR, "tas\tCAS_TrainSupport::TrainOnOLSentItemFolders GetDefaultMessageStore() failed"));
		}

		pSession->Logoff(0, 0, 0);
	}
	else
	{
		PR_TRACE((g_root, prtERROR, "tas\tCAS_TrainSupport::TrainOnOLSentItemFolders MapiLogOn failed"));
	}

	if (pSession)
		pSession->Release();

	return error;
}

// тренировка на папке отправленных Integrated Mailer (OutLook Express, Windows Mail)
tERROR CAS_TrainSupport::TrainOnIMSentItemFolders(OUT tDWORD * pMsgCount)
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "AS_TrainSupport::TrainOnIMSentItemFolders method" );

	if (!pMsgCount)
		return errPARAMETER_INVALID;

	error = errNOT_FOUND;

	
	if ( m_objPtr_OE )
	{
		FolderInfo_t OutlookExpress;
		tERROR errMSOE = OutlookExpress.SetObjPtr(m_objPtr_OE);
		if ( PR_SUCC(errMSOE) )
			errMSOE = OutlookExpress.MDB.SetObjPtr(m_objPtr_OE);
		if ( PR_SUCC(errMSOE) )
		{
			OutlookExpress.MDB.szName = m_WindowsIntegratedMailerName;
			OutlookExpress.szName = m_WindowsIntegratedMailerName;
			OutlookExpress.bHasChildren = cTRUE;
			
			if (m_WindowsIntegratedMailerName == MICROSOFT_WINDOWS_MAIL)
			{
				cStringObj name;
				if (PR_SUCC(GetWinMailSentItemsName(name)))
				{
					error = TrainOnFolderByCmp(CWindowsMailCmp(name), OutlookExpress, cTRUE, pMsgCount);
				}				
			}
			else
			{
				error = TrainOnFolderByCmp(COutLookExpressCmp(FolderInfo_t::ftSentItems), OutlookExpress, cTRUE, pMsgCount);
			}
			
		}
	}

	return error;
}

// тренировка на папке по заданному entry "большого" OutLook
tERROR CAS_TrainSupport::TrainOnOLFolderByEntry(EntryID_t& entry, LPMDB pMsgStore, OUT tDWORD * pMsgCount)
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "AS_TrainSupport::TrainOnOLFolderByEntry method" );

	if (!pMsgCount)
		return errPARAMETER_INVALID;

	error = errNOT_FOUND;

	if ( m_objPtr_OU )
	{
		FolderInfo_t Outlook;
		tERROR errMDB = Outlook.SetObjPtr(m_objPtr_OU);
		if ( PR_SUCC(errMDB) )
			errMDB = Outlook.MDB.SetObjPtr(m_objPtr_OU);
		if ( PR_SUCC(errMDB) )
		{
			Outlook.MDB.szName = MICROSOFT_OUTLOOK;
			Outlook.szName = MICROSOFT_OUTLOOK;
			Outlook.bHasChildren = cTRUE;
		}

		return TrainOnFolderByCmp(COutLookCmp(pMsgStore, entry), Outlook, cTRUE, pMsgCount);
	}

	return error;
}


// тренировка на папке по заданному компаратору для папки
template <typename TComparator>
tERROR CAS_TrainSupport::TrainOnFolderByCmp(TComparator& comp, IN FolderInfo_t& folder, IN tBOOL bForceAsSentMessage, OUT tDWORD * pMsgCount)
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "AS_TrainSupport::TrainOnFolderByCmp method" );

	if (!pMsgCount)
		return errPARAMETER_INVALID;

	error = errNOT_FOUND;

	// проверяем текущий каталог

	if (comp(folder))
	{
		// сравнение прошло!
		// тренируемся на самой папке
		FolderInfo_t fld = folder;

		if (*pMsgCount < m_settings.m_dMinHamLettersCount)
		{
			fld.dwMaxTrainMessages = m_settings.m_dMinHamLettersCount - *pMsgCount;
			tUINT totalMsgs = 0;
			tERROR err = TrainOnFolder(&folder, false, 0, 1, bForceAsSentMessage, totalMsgs);
			*pMsgCount += totalMsgs;
		}		
		
		if (*pMsgCount < m_settings.m_dMinHamLettersCount)
		{
			// тренируемся так же и на дочерних
			FolderInfoList_t childFolders;
			GetFolderInfoListDefProf(&fld, &childFolders);
			{
				tDWORD dwSize = childFolders.size();
				
				for (tDWORD i = 0; ((i < dwSize) && !m_bSkipTraining && (*pMsgCount < m_settings.m_dMinHamLettersCount)); ++i)
				{
					// выставляем ограничение, чтобы не было лишней работы
					childFolders[i].dwMaxTrainMessages = m_settings.m_dMinHamLettersCount - *pMsgCount;
					tUINT totalMsgs = 0;
					tERROR err = TrainOnFolder(&childFolders[i], false, i, dwSize, bForceAsSentMessage, totalMsgs);
					*pMsgCount += totalMsgs;
				}
			}
		}
		
		return  errOK;
	}
	else
	{
		// сравнение по entry не прошло... пробуем искать папку среди дочерних
		FolderInfoList_t childFolders;
		GetFolderInfoListDefProf(&folder, &childFolders);
		{
			tDWORD dwSize = childFolders.size();
			for (tDWORD i = 0; ((i < dwSize) && !m_bSkipTraining && (*pMsgCount < m_settings.m_dMinHamLettersCount)); ++i)
			{
				tERROR err = TrainOnFolderByCmp(comp, childFolders[i], bForceAsSentMessage, pMsgCount);
				if (errOK != error)
					error = err;
				//if (PR_SUCC(error))
				//	break;
			}
		}
	}

	return error;
}

// для Windows Mail определяем имя папки SentItems
tERROR CAS_TrainSupport::GetWinMailSentItemsName(cStringObj& name)
{
	tERROR err = errNOT_FOUND;

	if (SUCCEEDED(CoInitialize(NULL)))
	{
		IStoreNamespace * pStoreNamespace = NULL;
		HRESULT hr = CoCreateInstance(CLSID_StoreNamespace, NULL, CLSCTX_SERVER, 
			IID_IStoreNamespace, (LPVOID*) &pStoreNamespace);

		if (SUCCEEDED(hr))
		{
			hr = pStoreNamespace->Initialize(NULL, NULL);
			if (SUCCEEDED(hr))
			{
				err = GetFolderName(FOLDERID_ROOT, pStoreNamespace, FOLDER_SENT, name);
			}
		}

		if (pStoreNamespace)
			pStoreNamespace->Release();

		CoUninitialize();
	}

	return err;
}

// рекурсивный обход по структуре папок
tERROR CAS_TrainSupport::GetFolderName(STOREFOLDERID folderId, IStoreNamespace * pStore, SPECIALFOLDER folderType, cStringObj& name)
{
	tERROR err = errNOT_FOUND;

	if (!pStore)
		return errNOT_FOUND;

	FOLDERPROPS props;
	props.cbSize = sizeof(FOLDERPROPS);
	HENUMSTORE hEnum = NULL;

	HRESULT hr = pStore->GetFirstSubFolder(folderId, &props, &hEnum);

	while (SUCCEEDED(hr) && hr != S_FALSE && hEnum != NULL)
	{
		if (folderType == props.sfType)
		{
			err = errOK;
			name = props.szName;
			break;
		}
		
		if (props.cSubFolders > 0)
		{
			err = GetFolderName(props.dwFolderId, pStore, folderType, name);
			if (errOK == err)
				break;
		}
	
		hr = pStore->GetNextSubFolder(hEnum, &props);
	}

	// close the enum
	if(hEnum)
	{
		pStore->GetSubFolderClose(hEnum);
	}

	return err;
}

// получаем список дочерних папок с использованием профиля по умолчанию в случае OutLook
tERROR CAS_TrainSupport::GetFolderInfoListDefProf( FolderInfo_t* p_pFolderInfo, FolderInfoList_t* p_pFolderInfoList )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "AS_TrainSupport::GetFolderInfoListDefProf method" );

	cMAPIInitializer MAPIInitializer;

	if (!p_pFolderInfo)
		return errNOT_FOUND;
	
	if ( !p_pFolderInfo->m_objPtr )
	{
		if(!p_pFolderInfo->MDB.m_objPtr)
		{
			if ( m_objPtr_OU && p_pFolderInfo->MDB.szName == MICROSOFT_OUTLOOK )
				m_objPtr_OU->Clone(&p_pFolderInfo->MDB.m_objPtr);
			else if ( m_objPtr_OE && p_pFolderInfo->MDB.szName == m_WindowsIntegratedMailerName )
				m_objPtr_OE->Clone(&p_pFolderInfo->MDB.m_objPtr);
		}
		error = p_pFolderInfo->MDB.m_objPtr->Clone(&p_pFolderInfo->m_objPtr);
		if ( PR_SUCC(error) )
			error = p_pFolderInfo->m_objPtr->ChangeTo(cAutoString(p_pFolderInfo->szName));
	}

	if ( !p_pFolderInfo->m_objPtr )
	{
		PR_RPT(("Failed to get ObjPtr, err = 0x%08x", error));
		return errNOT_OK;
	}

	if (
		PR_SUCC(error) &&
		(p_pFolderInfo->szName == MICROSOFT_OUTLOOK) &&
		(p_pFolderInfo->MDB.szName == MICROSOFT_OUTLOOK) &&
		!p_pFolderInfo->EntryID.cb
		)
	{
		// профиль по умолчанию уже должен быть установлен
		p_pFolderInfo->m_objPtr->ChangeTo(cAutoString (m_szProfile));
		p_pFolderInfo->MDB.m_objPtr->ChangeTo(cAutoString (m_szProfile));
	}

	hObjPtr objPtr = p_pFolderInfo->m_objPtr;
	tBOOL bNeedToStepDown = cFALSE;

	bool bNeedCloseClone = false;
	if ( PR_SUCC(error) && objPtr )
	{
		bNeedToStepDown = objPtr->get_pgOBJECT_PTR_STATE() == cObjPtrState_PTR;
		if ( bNeedToStepDown )
		{
			// из-за ошибок в MDB StepDown и обход делаем клоном, не трогая p_pFolderInfo->m_objPtr
			if(objPtr->propGetPID() == PID_MDB)
			{
				error = objPtr->Clone(&objPtr);
				if ( PR_SUCC(error) )
					bNeedCloseClone = true;
				else
					objPtr = p_pFolderInfo->m_objPtr;
			}
			error = objPtr->StepDown();
		}
	}

	if ( PR_SUCC(error) && objPtr )
	{
		while ( objPtr->get_pgIS_FOLDER() && PR_SUCC(error = objPtr->Next()) && !m_bSkipTraining)
		{
			ObjectInfo_t oiFolder;
			error = oiFolder.szName.assign(objPtr, pgOBJECT_NAME);
			if ( PR_SUCC(error) )
			{
				if(objPtr->propGetPID() == PID_MDB)
				{
					EntryID_t EntryID;
					error = objPtr->propGet((tDWORD*)&EntryID.cb, pgMAIL_OBJECT_ENTRYID, NULL, NULL);
					if(PR_SUCC(error) && EntryID.cb)
						error = objPtr->propGet(NULL, pgMAIL_OBJECT_ENTRYID, EntryID.buff.get(EntryID.cb, false), EntryID.cb);
					if(PR_SUCC(error))
					{
						oiFolder.EntryID.cb = *(tDWORD*)(EntryID.buff.ptr());
						oiFolder.EntryID.buff.set(oiFolder.EntryID.cb, ((EntryID.buff.ptr()) + sizeof(tDWORD)));
					}
				}
				else
				{
					cStringObj str; error = str.assign(objPtr, pgOBJECT_FULL_NAME);
					if ( PR_SUCC(error) )
					{
						oiFolder.EntryID.cb = str.memsize(cCP_UNICODE)+2;
						oiFolder.EntryID.buff.set( oiFolder.EntryID.cb, (tBYTE*)(tWCHAR*)str.c_str(cCP_UNICODE) );
					}
				}
			}
			if ( PR_SUCC(error) )
			{
				FolderInfo_t FolderInfo(&p_pFolderInfo->MDB, &oiFolder);
				error = FolderInfo.SetObjPtr(objPtr);
				FolderInfo.bCanCreateSubfolders = objPtr->propGetBool(pgCAN_CREATE_SUBFOLDERS);
				FolderInfo.bCanHaveMessages = objPtr->propGetBool(pgIS_MESSAGE_STORAGE);
				FolderInfo.bHasChildren = p_pFolderInfo->szName.empty() ? cTRUE : HasSubfolders(&FolderInfo);
				FolderInfo.folderType = (FolderInfo_t::FolderType)objPtr->propGetDWord(pgFOLDER_TYPE);
				p_pFolderInfoList->push_back(FolderInfo);
			}
		}
	}

	if ( error == errEND_OF_THE_LIST )
		error = errOK;

	PR_ASSERT(PR_SUCC(error));

	if ( bNeedToStepDown && PR_SUCC(error) && objPtr && objPtr->propGetPID() != PID_MDB )
		error = objPtr->StepUp();

	if ( bNeedCloseClone && objPtr )
		objPtr->sysCloseObject();

	return error;
}

tERROR CAS_TrainSupport::AntiSpamTrainBegin()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "AS_TrainSupport::AntiSpamTrainBegin method" );

	error = errNOT_OK;

	if (m_bIsTaskPresent)
	{
		ANTISPAM_TRAIN_BEGIN();
	}
	else
	{
		m_bStartTraining = true;

		CAntispamImplementer::CKLAntispamFilter::TrainParams_t trainParams;
		trainParams.Action = cAntispamTrainParams::Train_Begin;
		tDWORD dwTimeOut = (tDWORD)(-1);

		error = m_hAntispamFilter->Train(&trainParams, NULL, dwTimeOut, NULL);
	}

	return error;
}

tERROR CAS_TrainSupport::AntiSpamTrainEndApply()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "AS_TrainSupport::AntiSpamTrainEndApply method" );

	error = errNOT_OK;

	if (m_bIsTaskPresent)
	{
		ANTISPAM_TRAIN_END_APPLY();
	}
	else
	{
		m_bStartTraining = false;

		CAntispamImplementer::CKLAntispamFilter::TrainParams_t trainParams;
		trainParams.Action = cAntispamTrainParams::Train_End_Apply;
		tDWORD dwTimeOut = (tDWORD)(-1);

		error = m_hAntispamFilter->Train(&trainParams, NULL, dwTimeOut, NULL);

		if (PR_SUCC(error))
		{
			error = m_hAntispamFilter->GetSettings(&m_settings);
		}
	}

	return error;
}

tERROR CAS_TrainSupport::AntiSpamTrainEndMerge()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "AS_TrainSupport::AntiSpamTrainEndMerge method" );

	error = errNOT_OK;

	if (m_bIsTaskPresent)
	{
		ANTISPAM_TRAIN_END_MERGE();
	}
	else
	{
		m_bStartTraining = false;

		CAntispamImplementer::CKLAntispamFilter::TrainParams_t trainParams;
		trainParams.Action = cAntispamTrainParams::MergeDatabase;
		tDWORD dwTimeOut = (tDWORD)(-1);
		
		error = m_hAntispamFilter->Train(&trainParams, NULL, dwTimeOut, NULL);
		
		if (PR_SUCC(error))
		{
			m_hAntispamFilter->sysSendMsg(pmc_ANTISPAM_HAS_BEEN_TRAINED, 0,0,0,0 );
			error = m_hAntispamFilter->GetSettings(&m_settings);
		}
	}

	return error;
}

tERROR CAS_TrainSupport::AntiSpamTrainEndCancel()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "AS_TrainSupport::AntiSpamTrainEndCancel method" );

	error = errNOT_OK;

	if (m_bIsTaskPresent)
	{
		ANTISPAM_TRAIN_END_CANCEL();
	}
	else
	{
		m_bStartTraining = false;

		CAntispamImplementer::CKLAntispamFilter::TrainParams_t trainParams;
		trainParams.Action = cAntispamTrainParams::Train_End_Cancel;
		tDWORD dwTimeOut = (tDWORD)(-1);

		error = m_hAntispamFilter->Train(&trainParams, NULL, dwTimeOut, NULL);
	}

	return error;
}

// тренировка антиспам-фильтра на сообщении
tERROR CAS_TrainSupport::TrainOnMessage(hOS os_message, cAntispamTrainParams& params)
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "AS_TrainSupport::TrainOnMessage method" );

	if (!os_message)
		return errPARAMETER_INVALID;

	error = errNOT_OK;

	if (m_bIsTaskPresent)
	{
		error = sysSendMsg(pmc_ANTISPAM_TRAIN, NULL, os_message, &params, SER_SENDMSG_PSIZE);
	}
	else
	{
		CAntispamImplementer::CKLAntispamFilter::TrainParams_t trainParams;
		CAntispamImplementer::CKLAntispamFilter::ProcessStatistics_t hStatistics;
		CAntispamImplementer::CKLAntispamFilter::ProcessStatistics_t* statistics = &hStatistics;

		trainParams.Action = (cAntispamTrainParams::Action_t)params.p_Action;

		if (params.p_pStat)
			statistics = params.p_pStat;

		if (os_message && !os_message->propGetDWord(g_propMailerPID))
			os_message->propSetDWord(g_propMailerPID, params.p_dwPID);

		error = m_hAntispamFilter->Train(&trainParams, (hOBJECT)os_message, params.p_dwTimeOut, statistics);

		if (PR_SUCC(error))
		{
			m_hAntispamFilter->sysSendMsg(pmc_ANTISPAM_HAS_BEEN_TRAINED, 0,0,0,0 );

			// Посылаем отчет
			cAntispamReport& rep = *statistics;
			switch (trainParams.Action)
			{
			case cAntispamTrainParams::TrainAsHam:
			case cAntispamTrainParams::RemoveFromSpam:
				rep.m_dwLastMessageCategory = cAntispamReport::IsHam;
				break;
			case cAntispamTrainParams::TrainAsSpam:
			case cAntispamTrainParams::RemoveFromHam:
				rep.m_dwLastMessageCategory = cAntispamReport::IsSpam;
				break;
			}
			sysSendMsg(pmc_ANTISPAM_REPORT, NULL, NULL, &rep, SER_SENDMSG_PSIZE);
		}
		else
			if (errOPERATION_CANCELED == error)
			{
				error = errOK;
			}
	}

	return error;	
}


// AVP Prague stamp begin( C++ class regitration,  )
// Interface "AS_TrainSupport". Register function
tERROR CAS_TrainSupport::Register( hROOT root ) {
	tERROR error;
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(AS_TrainSupport_PropTable)
	mpLOCAL_PROPERTY_BUF( pgTrainSupp_TM, CAS_TrainSupport, m_TM, cPROP_BUFFER_WRITE | cPROP_WRITABLE_ON_INIT | cPROP_REQUIRED )
	mpLOCAL_PROPERTY_BUF( pgForceUISuppress, CAS_TrainSupport, m_bForceUISuppress, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mpLOCAL_PROPERTY_BUF( pgNeedHamTrain, CAS_TrainSupport, m_bNeedHamTrain, cPROP_BUFFER_READ )
mpPROPERTY_TABLE_END(AS_TrainSupport_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(AS_TrainSupport)
	mINTERNAL_METHOD(ObjectInit)
	mINTERNAL_METHOD(ObjectInitDone)
	mINTERNAL_METHOD(ObjectPreClose)
	mINTERNAL_METHOD(MsgReceive)
mINTERNAL_TABLE_END(AS_TrainSupport)
// AVP Prague stamp end



// AVP Prague stamp begin( External method table,  )
mEXTERNAL_TABLE_BEGIN(AS_TrainSupport)
	mEXTERNAL_METHOD(AS_TrainSupport, GetFolderInfoList)
	mEXTERNAL_METHOD(AS_TrainSupport, Train)
	mEXTERNAL_METHOD(AS_TrainSupport, GetMAPIEntryID)
	mEXTERNAL_METHOD(AS_TrainSupport, CreateSubFolder)
	mEXTERNAL_METHOD(AS_TrainSupport, TrainAsync)
	mEXTERNAL_METHOD(AS_TrainSupport, TrainOnOutLookSentItems)
	mEXTERNAL_METHOD(AS_TrainSupport, TrainOnIntMailSentItems)
	mEXTERNAL_METHOD(AS_TrainSupport, TrainOnAllSentItemsAsync)
mEXTERNAL_TABLE_END(AS_TrainSupport)
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )

	PR_TRACE_FUNC_FRAME_( *root, "AS_TrainSupport::Register method", &error );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_AS_TRAINSUPPORT,                    // interface identifier
		PID_AS_TRAINSUPPORT,                    // plugin identifier
		0x00000000,                             // subtype identifier
		AS_TrainSupport_VERSION,                // interface version
		VID_DENISOV,                            // interface developer
		&i_AS_TrainSupport_vtbl,                // internal(kernel called) function table
		(sizeof(i_AS_TrainSupport_vtbl)/sizeof(tPTR)),// internal function table size
		&e_AS_TrainSupport_vtbl,                // external function table
		(sizeof(e_AS_TrainSupport_vtbl)/sizeof(tPTR)),// external function table size
		AS_TrainSupport_PropTable,              // property table
		mPROPERTY_TABLE_SIZE(AS_TrainSupport_PropTable),// property table size
		sizeof(CAS_TrainSupport)-sizeof(cObjImpl),// memory size
		0                                       // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"AS_TrainSupport(IID_AS_TRAINSUPPORT) registered [%terr]",error) );
	#endif // _PRAGUE_TRACE_
// AVP Prague stamp end



	if( PR_SUCC(error) )
		error = root->RegisterCustomPropId(
			&g_propMessageIsIncoming, 
			npMESSAGE_IS_INCOMING, 
			pTYPE_BOOL | pCUSTOM_HERITABLE
			);

	if( PR_SUCC(error) )
		error = root->RegisterCustomPropId(
			&g_propMailerPID, 
			npMAILER_PID, 
			pTYPE_DWORD | pCUSTOM_HERITABLE
			);
	

// AVP Prague stamp begin( C++ class regitration end,  )
	return error;
}

tERROR pr_call AS_TrainSupport_Register( hROOT root ) { return CAS_TrainSupport::Register(root); }
// AVP Prague stamp end



CTrainThread::CTrainThread() : cThreadPoolBase("ASpamTrain"), m_pAS(NULL) {}

tERROR CTrainThread::run(TrainSettings_t* pSettings, CAS_TrainSupport *pAS)
{
	if(m_pAS)
		return errNOT_OK;

	m_pAS = pAS;
	m_Settings = *pSettings;
	
	cThreadTaskBase::start(*this);
	
	return errOK;
}

void CTrainThread::do_work()
{
	cMAPIInitializer MAPIInitializer;

	m_pAS->Train(&m_Settings);
	m_pAS = NULL;
	m_Settings.m_Folders.clear();
}

CTrainThreadSentItems::CTrainThreadSentItems() :
cThreadPoolBase("ASpamTrainSentItems"), m_pAS(NULL), m_pSettings(NULL)
{}

tERROR CTrainThreadSentItems::run(cAntispamSettings* pSettings, CAS_TrainSupport *pAS)
{
	if(m_pAS)
		return errNOT_OK;

	m_pAS = pAS;
	m_pSettings = pSettings;

	cThreadTaskBase::start(*this);

	return errOK;
}

void CTrainThreadSentItems::do_work()
{
	cMAPIInitializer MAPIInitializer;
	m_pAS->TrainOnAllSentItemsImpl(m_pSettings);
	m_pAS = NULL;
	m_pSettings = NULL;
}

// AVP Prague stamp begin( Caution !!!,  )
// You have to implement propetry: pgINTERFACE_VERSION
// You have to implement propetry: pgINTERFACE_COMMENT
// AVP Prague stamp end



