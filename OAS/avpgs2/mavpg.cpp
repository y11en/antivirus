/*
 *  mavpg.cpp
 *  
 *
 *  Created by Rustam Muginov on 14.08.07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "mavpg.h"
#include "kliflib/kliflib.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <Prague/prague.h>
#include <Prague/pr_types.h>
#include <Prague/pr_oid.h>
#include <Prague/iface/i_io.h>
#include <Prague/iface/i_string.h>
#include <OAS/iface/i_avpg2.h>
//#include <Prague/iface/i_nfio.h>
#include <Prague/plugin/p_nfio.h>
//#include <ProductCore/common/UserBan.h"
#include <sys/sysctl.h>
#include <pwd.h>

static char PathPrefix[] = "/1/";
static int InitiallTimeout = 1;

#define cAVPG2_PROP_OBJECTID      ((tSTRING)("i_avpg2 macosx objectid"))
tPROPID propid_ObjectID = 0;
static bool b_stop = false;

static char securitykey[8];
static hOBJECT s_pPragueContext = NULL;
static long s_TaskCounter = 0;

static int AvgpToKlif(tDWORD Verdict)
{
	int result=KLIF_MODULE_ACTION_PASSCACHE;
	
	if(Verdict & cAVPG2_RC_DENY_ACCESS)
		result=KLIF_MODULE_ACTION_DENY;
	else if( Verdict & (cAVPG2_RC_ALLOW_ACCESS || cAVPG2_RC_CACHE) )
		result=KLIF_MODULE_ACTION_PASSCACHE;
	else if(Verdict & cAVPG2_RC_ALLOW_ACCESS )
		result = KLIF_MODULE_ACTION_PASS;
	return result;
} // AvgpToKlif

typedef struct
{
	char filename[MAX_PATH];
	long objectid;
	long uid;
	long pid;
	int action;
	void *context;
} tThreatCallbackData;

int getprocessname( pid_t inPID, char *outName, size_t inMaxLen)
{
	struct kinfo_proc info;
	size_t length = sizeof(struct kinfo_proc);
	int mib[4] = { CTL_KERN, KERN_PROC, KERN_PROC_PID, inPID };
		
	if (sysctl(mib, 4, &info, &length, NULL, 0) < 0)
		return -1 ;
	else
		strncpy(outName, info.kp_proc.p_comm, inMaxLen)	;

	return 0	;
}

tERROR pr_call TaskCallback(tThreadCallType CallType, tPTR pThreadContext, tPTR* ppTaskContext, tPTR pTaskData, tDWORD TaskDataLen)
{
//	fprintf( stderr, "TaskCallback called CallType %d pTaskData %p TaskDataLen %d\n", CallType, pTaskData, TaskDataLen );
	if( CallType != TP_CBTYPE_TASK_PROCESS)
		return errOK;


	tThreatCallbackData *pData = (tThreatCallbackData *)pTaskData;
	const char *filename = pData->filename;
	long objectid = pData->objectid;
	long pid = pData->pid;
	long uid = pData->uid;
	int action = pData->action;

	char ProcessName[1024];
	memset( ProcessName, 0, sizeof(ProcessName));
	const char *UserName = "<<n/a>>";
	const char *LoginName = "<<n/a>>";
	getprocessname(pid, ProcessName, sizeof(ProcessName)-1);
	struct passwd *pwd = getpwuid(uid);
	if( pwd != NULL )
	{
		UserName = pwd->pw_name;
		LoginName = pwd->pw_gecos;
	}
	
	const char * actstr = "";
	
	switch ( action )
	{
		case KLIF_ACTION_OPEN:
			actstr = "open";
			break;
		
		case KLIF_ACTION_CLOSE:
			actstr = "close";
			break;
		
		case KLIF_ACTION_EXEC:
			actstr = "exec";
			break;
	}

//	if( strncmp(filename, PathPrefix, strlen(PathPrefix)) != 0 )
//	{
//		klif_set_file_action( securitykey, objectid, KLIF_MODULE_ACTION_PASSCACHE );
//		return errOK;
//	}

//	sleep(2);
//	fprintf( stderr, "\n<<<<<<<<<< TaskCallback handling file \"%s\" task No %ld\n\n", filename, s_TaskCounter ++ );


	PR_TRACE(( g_root, prtNOTIFY, "file_interceptor_callback HANDLING file path \"%s\"", filename));
	fprintf( stderr, "Event: No %ld %s \"%s\", user %ld (%s/\"%s\"), pid %ld (\"%s\")\n", s_TaskCounter ++, actstr, filename, uid, UserName, LoginName, pid, ProcessName);

	cString* hCtx = NULL;
	tERROR error = s_pPragueContext->sysCreateObjectQuick((hOBJECT*) &hCtx, IID_STRING, PID_ANY, 0);
	if( PR_FAIL(error) )
	{
		fprintf( stderr, "TaskCallback - sysCreateObjectQuick error %ld\n", error );
		klif_set_file_action( securitykey, objectid, KLIF_MODULE_ACTION_PASSCACHE );
		return errUNEXPECTED;
	}

	tORIG_ID Origin = OID_GENERIC_IO;
	error = CALL_SYS_PropertySet( hCtx, 0, propid_CustomOrigin, &Origin, sizeof(Origin) );	// cAVPG2_PROP_CUSTOM_ORIGIN
	if( PR_FAIL(error) )
	{
		fprintf( stderr, "TaskCallback - CALL_SYS_PropertySet failed setting propid_CustomOrigin, error %ld\n", error );
		klif_set_file_action( securitykey, objectid, KLIF_MODULE_ACTION_PASSCACHE );
		return errUNEXPECTED;
	}
	
	error = CALL_SYS_PropertySet( hCtx, 0, propid_EventProcessID, &pid, sizeof(pid) );	// cAVPG2_PROP_PROCESS_ID
	if( PR_FAIL(error) )
	{
		fprintf( stderr, "TaskCallback - CALL_SYS_PropertySet failed setting propid_EventProcessID, error %ld\n", error );
		klif_set_file_action( securitykey, objectid, KLIF_MODULE_ACTION_PASSCACHE );
		return errUNEXPECTED;
	}

	CALL_SYS_PropertySet( hCtx, 0, propid_ObjectID, &objectid, sizeof(objectid) );	// cAVPG2_PROP_OBJECTID
	if( PR_FAIL(error) )
	{
		fprintf( stderr, "TaskCallback - CALL_SYS_PropertySet failed setting objectid, error %ld\n", error );
		klif_set_file_action( securitykey, objectid, KLIF_MODULE_ACTION_PASSCACHE );
		return errUNEXPECTED;
	}

	tBOOL bTrue = cTRUE;
	error = CALL_SYS_PropertySet( hCtx, 0, propid_Background, &bTrue, sizeof(bTrue) );	// cAVPG2_PROP_BACKGROUND_PROCESSING
	if( PR_FAIL(error) )
	{
		fprintf( stderr, "TaskCallback - CALL_SYS_PropertySet failed setting propid_Background, error %ld\n", error );
		klif_set_file_action( securitykey, objectid, KLIF_MODULE_ACTION_PASSCACHE );
		return errUNEXPECTED;
	}

// Add properties:
//#define cAVPG2_PROP_USER_ACCESSED      ((tSTRING)("i_avpg2 User info")) // описывает пользователя, вызвавшего событие
//#define cAVPG2_PROP_USER_CREATED       ((tSTRING)("i_avpg2 creator User info")) // описывает пользователя, создавшего событие

	error = hCtx->ImportFromBuff (
		NULL,
		(void *)filename,
		strlen(filename),
		cCP_ANSI,
		cSTRING_Z
		);
	if( PR_FAIL(error) )
	{
		fprintf( stderr, "TaskCallback - hCtx->ImportFromBuff failed for filename \"%s\", error %ld\n", filename, error );
		klif_set_file_action( securitykey, objectid, KLIF_MODULE_ACTION_PASSCACHE );
		return errUNEXPECTED;
	}

	tDWORD Verdict = cAVPG2_RC_ALLOW_ACCESS;
	tDWORD blen = sizeof(Verdict);
	error = s_pPragueContext->sysSendMsg (
		pmc_AVPG2,
		pm_AVPG2_PROCEED_OBJECT,
		(hOBJECT) *hCtx,
		&Verdict,
		&blen
		);
	if( PR_FAIL(error) )
	{
		fprintf( stderr, "TaskCallback - s_pPragueContext->sysSendMsg failed, error %ld\n", error );
		klif_set_file_action( securitykey, objectid, KLIF_MODULE_ACTION_PASSCACHE );
		return errUNEXPECTED;
	}

	fprintf( stderr, "scan_events_thread called sysSendMsg for file \"%s\", error %ld verdict %u\n", filename, error, Verdict );

	int klifAction = AvgpToKlif(Verdict);
	klif_set_file_action( securitykey, objectid, klifAction );

	if (hCtx)
	{
		hCtx->sysCloseObject();
		hCtx = NULL;
	}

	return errOK;
} // TaskCallback



void cMacAvpgImpl::s_file_interceptor_callback ( const char * filename, long objectid, long uid, long pid, int action, void *context )
{
	if( strncmp(filename, "/System/", strlen("/System/")) == 0 )
	{
		klif_set_file_action( securitykey, objectid, KLIF_MODULE_ACTION_PASSCACHE );
		return;
	}
	if( strncmp(filename, "/usr/", strlen("/usr/")) == 0 )
	{
		klif_set_file_action( securitykey, objectid, KLIF_MODULE_ACTION_PASSCACHE );
		return;
	}

	cMacAvpgImpl *_This = (cMacAvpgImpl *)context;
	_This->file_interceptor_callback( filename, objectid, uid, pid, action);
}

void cMacAvpgImpl::file_interceptor_callback ( const char * filename, long objectid, long uid, long pid, int action)
{
//	fprintf( stderr, "\n>>>>>>>>>>>>>cMacAvpgImpl::s_file_interceptor_callback entered for file \"%s\"\n", filename );
	tTaskId TaskId;
	tThreatCallbackData callbackData;
	memset( &callbackData, 0, sizeof(callbackData));
	strncpy(callbackData.filename, filename, sizeof(callbackData.filename)-1);
	callbackData.objectid = objectid;
	callbackData.uid     = uid;
	callbackData.pid     = pid;
	callbackData.action  = action;
	callbackData.context = this;
	
	tERROR errr;
	errr = m_ThreadPool->AddTask(&TaskId, TaskCallback, &callbackData, sizeof(callbackData), TP_THREADPRIORITY_NORMAL);
//	fprintf( stderr, "cMacAvpgImpl::file_interceptor_callback AddTask result %ld\n", errr );
	if( PR_FAIL(errr) )
	{
		klif_set_file_action( securitykey, objectid, KLIF_MODULE_ACTION_PASSCACHE );
		fprintf( stderr, ">>>>>>>>>>>>>cMacAvpgImpl::s_file_interceptor_callback AddTask failed error %ld setting PASSCACHE for file \"%s\"\n\n", errr, filename );
		return;
	}
//	fprintf( stderr, ">>>>>>>>>>>>>cMacAvpgImpl::s_file_interceptor_callback about to exit\n\n" );
	return;
/*	
// call klif_set_file_timeout with constant 1 second (???)
	klif_set_file_timeout(securitykey, objectid, InitiallTimeout);
	const char * actstr = "";
	
	switch ( action )
	{
		case KLIF_ACTION_OPEN:
			actstr = "open";
			break;
		
		case KLIF_ACTION_CLOSE:
			actstr = "close";
			break;
		
		case KLIF_ACTION_EXEC:
			actstr = "exec";
			break;
	}
	
	// return if not in watched folder
//	PR_TRACE(( g_root, prtNOTIFY, "about to strncpy file \"%s\"", filename));
//	int strRes = strncmp(filename, PathPrefix, strlen(PathPrefix));
//	PR_TRACE(( g_root, prtNOTIFY, "strncmp file \"%s\" prefix \"%s\" len %d result %d", filename, PathPrefix, strlen(PathPrefix), strRes));
	if( strncmp(filename, PathPrefix, strlen(PathPrefix)) != 0 )
	{
		klif_set_file_action( securitykey, objectid, KLIF_MODULE_ACTION_PASSCACHE );
//		PR_TRACE(( g_root, prtNOTIFY, "file_interceptor_callback passing file path \"%s\"", filename));
		return;
	}
	
	PR_TRACE(( g_root, prtNOTIFY, "file_interceptor_callback HANDLING file path \"%s\"", filename));
	fprintf( stderr, "Event: %s file %s user %ld, pid %ld\n", actstr, filename, uid, pid);

		cString* hCtx = NULL;
		tERROR error = s_pPragueContext->sysCreateObjectQuick((hOBJECT*) &hCtx, IID_STRING, PID_ANY, 0);

	tORIG_ID Origin = OID_GENERIC_IO;
	CALL_SYS_PropertySet( hCtx, 0, propid_CustomOrigin, &Origin, sizeof(Origin) );	// cAVPG2_PROP_CUSTOM_ORIGIN
	
	CALL_SYS_PropertySet( hCtx, 0, propid_EventProcessID, &pid, sizeof(pid) );	// cAVPG2_PROP_PROCESS_ID

	CALL_SYS_PropertySet( hCtx, 0, propid_ObjectID, &objectid, sizeof(objectid) );	// cAVPG2_PROP_OBJECTID

// Add properties:
//#define cAVPG2_PROP_USER_ACCESSED      ((tSTRING)("i_avpg2 User info")) // описывает пользователя, вызвавшего событие
//#define cAVPG2_PROP_USER_CREATED       ((tSTRING)("i_avpg2 creator User info")) // описывает пользователя, создавшего событие

		error = hCtx->ImportFromBuff (
			NULL,
			(void *)filename,
			strlen(filename),
			cCP_ANSI,
			cSTRING_Z
			);
	
		tDWORD Verdict = cAVPG2_RC_ALLOW_ACCESS;
		tDWORD blen = sizeof(Verdict);

		error = s_pPragueContext->sysSendMsg (
			pmc_AVPG2,
			pm_AVPG2_PROCEED_OBJECT,
			(hOBJECT) *hCtx,
			&Verdict,
			&blen
			);

		fprintf( stderr, "scan_events_thread called sysSendMsg, error %ld verdict %u\n", error, Verdict );

		int klifAction = AvgpToKlif(Verdict);
		klif_set_file_action( securitykey, objectid, klifAction );
*/
} // file_interceptor_callback


void* cMacAvpgImpl::s_scan_events_thread( void *param )
{
	cMacAvpgImpl *_This = (cMacAvpgImpl *)param;
	_This->scan_events_thread();
	return 0;
}
void* cMacAvpgImpl::s_scan_events_thread_fake( void *param )
{
	cMacAvpgImpl *_This = (cMacAvpgImpl *)param;
	_This->scan_events_thread_fake();
	return 0;
}

void cMacAvpgImpl::scan_events_thread()
{
	s_pPragueContext = (hOBJECT)(this->m_pPragueContext);
	// Starting the file interceptor. Remember that klif_start_interceptor() will only return when
	// klif_stop_interceptor is called.
	fprintf( stderr, "Starting file interceptor\n");
	if ( klif_start_interceptor( s_file_interceptor_callback, this ) )
	{
		fprintf( stderr, "Could not start or maintain file interceptor, error %d\n", errno );
		exit (1);
	}

	// We only get here when the file interceptor is stopped, so no need to call klif_stop_interceptor()
	fprintf( stderr, "The file interceptor successfully stopped. Shutdown KLIF.\n");
} // scan_events_thread

void cMacAvpgImpl::scan_events_thread_fake()
{
	s_pPragueContext = (hOBJECT)(this->m_pPragueContext);
	// Starting the file interceptor. Remember that klif_start_interceptor() will only return when
	// klif_stop_interceptor is called.
	fprintf( stderr, "Starting FAKE file interceptor\n");

	while( !b_stop )
	{
		sleep( 10 );
		file_interceptor_callback( "/1/n.exe", 0, 0, 0, KLIF_ACTION_OPEN );
		
	}
	// We only get here when the file interceptor is stopped, so no need to call klif_stop_interceptor()
	fprintf( stderr, "The file interceptor successfully stopped. Shutdown KLIF.\n");
} // scan_events_thread


//void* scan_events_thread( void *param )
//{
//	hOBJECT pPragueContext = (hOBJECT)param;
//	fprintf( stderr, "scan_events_thread started...\n" );
//	static int iters = 10;
//	while(iters > 0)
//	{
//		sleep( 1 );
//		fprintf( stderr, "scan_events_thread ticking, %d iterations left...\n", iters );
//		iters --;
//
//
//		cString* hCtx = NULL;
//		tERROR error = pPragueContext->sysCreateObjectQuick((hOBJECT*) &hCtx, IID_STRING, PID_ANY, 0);
//
//		error = hCtx->ImportFromBuff (
//			NULL,
//			(void *)"/1/eicars.com",
//			strlen("/1/eicars.com"),
//			cCP_ANSI,
//			cSTRING_Z
//			);
//	
//		tDWORD Verdict = cAVPG2_RC_ALLOW_ACCESS;
//		tDWORD blen = sizeof(Verdict);
//
//		error = pPragueContext->sysSendMsg (
//			pmc_AVPG2,
//			pm_AVPG2_PROCEED_OBJECT,
//			(hOBJECT) *hCtx,
//			&Verdict,
//			&blen
//			);
//
//		fprintf( stderr, "scan_events_thread called sysSendMsg, error %d\n", error );
//	}
//
//	fprintf( stderr, "scan_events_thread about to return\n" );
//	return 0;
//}

bool
CreateThreadPool (
	hOBJECT pPragueContext,
	const char *pcName,
	cThreadPool** ppThPool,
	tDWORD MaxLen,
	tThreadPriority Priority,
	tDWORD MinThreads,
	tDWORD MaxThreads,
	tDWORD IdleCount,
	tPTR pContext,
	tPTR pCallback
	)
{
	tERROR error = pPragueContext->sysCreateObject( (hOBJECT*)ppThPool, IID_THREADPOOL, PID_THPOOLIMP );

	if (PR_FAIL( error ))
	{
		*ppThPool = 0;
		return false;
	}

	(*ppThPool)->set_pgTP_TASK_QUEUE_MAX_LEN( MaxLen );

	(*ppThPool)->set_pgTP_DEFAULT_THREAD_PRIORITY( Priority );
	(*ppThPool)->set_pgTP_MIN_THREADS( MinThreads );
	(*ppThPool)->set_pgTP_MAX_THREADS( MaxThreads + 1 );	// +one for main thread

	(*ppThPool)->set_pgTP_ALLOW_TERMINATING( cTRUE );
	(*ppThPool)->set_pgTP_TERMINATE_TIMEOUT( 30000 );

	(*ppThPool)->set_pgTP_QUICK_EXECUTE( cTRUE );
	(*ppThPool)->set_pgTP_QUICK_DONE( cFALSE );

	(*ppThPool)->set_pgTP_MAX_IDLE_COUNT( IdleCount );
	(*ppThPool)->set_pgTP_INVISIBLE_FLAG( cTRUE );
	(*ppThPool)->set_pgTP_YIELD_TIMEOUT( 5000 );

	(*ppThPool)->set_pgTP_THREAD_CALLBACK( (tThreadCallback) pCallback );
	(*ppThPool)->set_pgTP_THREAD_CALLBACK_CONTEXT( pContext );
	
	(*ppThPool)->set_pgTP_NAME( (tPTR)pcName, strlen( pcName ) + 1 );

	error = (*ppThPool)->sysCreateObjectDone();
	if (PR_FAIL(error))
	{
		(*ppThPool)->sysCloseObject();
		*ppThPool = 0;
		
		return false;
	}

	return true;
}


cMacAvpgImpl::cMacAvpgImpl (
	hOBJECT pPragueContext,
	tAVPG2_WORKING_MODE WorkingMode,
	tDWORD ThreadMaxCount
	) : cAvpgImpl( pPragueContext, WorkingMode, ThreadMaxCount )
	, m_RunningAsRoot(false)
	, m_Inited(false)
{
	PR_TRACE(( g_root, prtNOTIFY, "trace cMacAvpgImpl::ctor called" ));
	m_RunningAsRoot = (getuid() == 0);

	tERROR error = errOK;
	
	bool bCreatePool = CreateThreadPool( 
		m_pPragueContext,
		"ThPoolWork",
		&m_ThreadPool,
		0,
		TP_THREADPRIORITY_ABOVE_NORMAL,
		3,
		m_ThreadMaxCount,
		2,
		this,
		NULL
		);
	
	fprintf( stderr, "CreatePool result %d\n", bCreatePool );
	
	// register custom property, making it inheritable
	if (PR_SUCC(error) )
		error = CALL_Root_RegisterCustomPropId( g_root, &propid_ObjectID, cAVPG2_PROP_OBJECTID, pTYPE_DWORD | pCUSTOM_HERITABLE );

	
	if( !m_RunningAsRoot )
	{
		fprintf( stderr, "Faking the file intercept driver\n" );
		if ( pthread_create( &m_ThreadID, 0, s_scan_events_thread_fake, this ) != 0)
		{
			fprintf( stderr, "Could not create scan thread, error %s\n", strerror(errno) );
			return;
		}
		return;
	}
	
	
	if ( geteuid() != 0 )
	{
		fprintf( stderr, "The test should be run under root\n" );
		exit( 1 );
	}

	// Initialize the driver.
	fprintf( stderr, "Initializing the KLIF driver ... ");
	
	// We use automatically generated key
	if ( klif_init( 0 ) )
	{
		fprintf( stderr, "Could not initialize driver, error %d\n",errno);
		exit(1);
	}
	
	fprintf( stderr, "succceed\n" );
	
	// Obtain the security key. Because we did not provide it during initialization, it was
	// generated automatically. This means we need to get it.
	memcpy( securitykey, klif_getsecuritykey(), 8 );

// for gdb to debug
//	klif_register_pid(securitykey, 4451 );
	
	
    // Start scan thread
    if ( pthread_create( &m_ThreadID, 0, s_scan_events_thread, this ) != 0)
    {
        fprintf( stderr, "Could not create scan thread, error %s\n", strerror(errno) );
		return;
    }

	ResetDriverCache();
	m_Inited = true;
} // ctor

cMacAvpgImpl::~cMacAvpgImpl()
{
	fprintf( stderr, "cMacAvpgImpl::dtor called\n" );
	b_stop = true;


	if( !m_RunningAsRoot )
	{
	fprintf( stderr, "cMacAvpgImpl::dtor waiting for scan thread termination\n" );
	pthread_join( m_ThreadID, 0 );
	fprintf( stderr, "cMacAvpgImpl::dtor scan thread stopped\n" );
	return;
	}

	// Signal the scan thread to stop
	klif_stop_interceptor();

	// wait for scan thread to exit
	fprintf( stderr, "cMacAvpgImpl::dtor waiting for scan thread termination\n" );
	pthread_join( m_ThreadID, 0 );
	fprintf( stderr, "cMacAvpgImpl::dtor scan thread stopped\n" );

	// We're done. Shut down the interface.
	fprintf( stderr, "cMacAvpgImpl::dtor calling klif_shutdown\n" );
	klif_shutdown();


	fprintf( stderr, "cMacAvpgImpl::dtor finished\n" );
} // dtor


tERROR cMacAvpgImpl::SetActivityMode (
	tAVPG2_CLIENT_ACTIVITY_MODE ActivityMode
	)
{
//	HRESULT hResult = E_FAIL;
//
//	switch (ActivityMode)
//	{
//	case cAVPG2_CLIENT_ACTIVITY_MODE_SLEEP:
//		hResult = DRV_ChangeActiveStatus( m_pDrvContext, FALSE );
//		break;
//
//	case cAVPG2_CLIENT_ACTIVITY_MODE_ACTIVE:
//		hResult = DRV_ResetCache( m_pDrvContext );
//		if (SUCCEEDED( hResult ))
//			hResult = DRV_ChangeActiveStatus( m_pDrvContext, TRUE );
//		break;
//	}
//
//	PR_TRACE(( g_root, prtERROR, TR "SetActivityMode result 0x%x", hResult ));
//
//	if (SUCCEEDED( hResult ))
//		return m_pInstance->SetActivityMode( ActivityMode );
//
	return errUNEXPECTED;
}

tERROR
cMacAvpgImpl::ResetDriverCache()
{
	int error = klif_clear_cache();
	if(PR_FAIL( error ))
	{
		PR_TRACE(( g_root, prtERROR, TR "klif_clear_cache error 0x%x", error ));
		return errUNEXPECTED;
	}
	return errOK;
}

tERROR
cMacAvpgImpl::CreateIO( hOBJECT* p_phObject, hSTRING p_hObjectName )
{
	hIO hIo = NULL;
	tERROR error = p_hObjectName->sysCreateObject( (hOBJECT*) &hIo, IID_IO, PID_NFIO, SUBTYPE_ANY );
	PR_TRACE(( g_root, prtNOTIFY, "cMacAvpgImpl::CreateIO sysCreateObject result %ld (0x%lx)", error, error));
	
	if(PR_FAIL(error))
	{
		PR_TRACE(( g_root, prtERROR, "cMacAvpgImpl::CreateIO sysCreateObject error %ld (0x%lx)", error, error));
		return error;
	}

//	char buf[MAX_PATH + 1];
//	char utfbuf[2*MAX_PATH + 1];
//	memset(buf, 0, sizeof(buf));
//	memset(utfbuf, 0, sizeof(utfbuf));
//	if(PR_SUCC(error))
////		error = p_hObjectName->ExportToBuff(0, cSTRING_WHOLE, utfbuf, sizeof(utfbuf), cCP_UTF8, cSTRING_Z);
//		error = p_hObjectName->ExportToBuff(0, cSTRING_WHOLE, utfbuf, sizeof(utfbuf), cCP_UNICODE, cSTRING_Z);
//	if(PR_FAIL(error))
//		PR_TRACE(( g_root, prtERROR, "cMacAvpgImpl::CreateIO sysCreateObject failed to exported UTF8 string, error %ld", error));
//	else
//		PR_TRACE(( g_root, prtNOTIFY, "cMacAvpgImpl::CreateIO sysCreateObject exported UTF8 string \"%s\"", utfbuf));
//	
//
////	if(PR_SUCC(error))
//		error = p_hObjectName->ExportToBuff(0, cSTRING_WHOLE, buf, sizeof(buf), cCP_ANSI, cSTRING_Z);
//	PR_TRACE(( g_root, prtNOTIFY, "cMacAvpgImpl::CreateIO sysCreateObject exported string \"%s\"", buf));
//	if(PR_FAIL(error))
//	{
//		PR_TRACE(( g_root, prtERROR, "cMacAvpgImpl::CreateIO failed to ExportToBuff error %ld", error));
//	}
//
//	if(PR_SUCC(error))
//		error = hIo->propSetStr(0, pgOBJECT_FULL_NAME, buf, strlen(buf));
//	if(PR_FAIL(error))
//	{
//		PR_TRACE(( g_root, prtERROR, "cMacAvpgImpl::CreateIO failed to propSetStr error %ld", error));
//	}
	
	tDWORD bytesExported=0;
	if(PR_SUCC(error))
		p_hObjectName->ExportToProp(&bytesExported, cSTRING_WHOLE, (hOBJECT)hIo, pgOBJECT_FULL_NAME);
	if(PR_FAIL(error))
	{
		PR_TRACE(( g_root, prtERROR, "cMacAvpgImpl::CreateIO failed to ExportToProp error %ld (0x%lx)", error, error));
	}
	else
		PR_TRACE(( g_root, prtNOTIFY, "cMacAvpgImpl::CreateIO exported %lu (0x%lx) bytes", bytesExported, bytesExported));
		
	char utfbuf[2*MAX_PATH + 1];
	tDWORD Count = 100;
//	memset(buf, 0, sizeof(buf));
	memset(utfbuf, 0, sizeof(utfbuf));
	PR_TRACE(( g_root, prtNOTIFY, "!!!!!!!!Before call"));

	error = hIo->propGetStr(&Count, pgOBJECT_FULL_NAME, utfbuf, sizeof(utfbuf), cCP_ANSI);
	PR_TRACE(( g_root, prtNOTIFY, "!!!!!!!!cMacAvpgImpl::CreateIO object path \"%s\" err %ld (0x%lx)", utfbuf, error, error));


	
	if(PR_SUCC(error))
		error = hIo->sysCreateObjectDone ();
	if(PR_FAIL(error))
	{
		PR_TRACE(( g_root, prtERROR, "cMacAvpgImpl::CreateIO sysCreateObjectDone failed error %ld (0x%lx)", error, error));
	}

	if (PR_FAIL( error ) && hIo)
	{
		CALL_SYS_ObjectClose(hIo);
		hIo = NULL;
	}
	
	*p_phObject = (hOBJECT) hIo;

	return error;
}



tERROR cMacAvpgImpl::YieldEvent( hOBJECT EventObject, tDWORD YieldTimeout )
{
	long objectid;
	tERROR error = CALL_SYS_PropertyGet( EventObject, 0, propid_ObjectID, &objectid, sizeof(objectid) );	// cAVPG2_PROP_OBJECTID
	if(PR_FAIL(error))
	{
		PR_TRACE(( g_root, prtERROR, "cannot find custom property cAVPG2_PROP_OBJECTID"));
		return error;
	}
	
	error = klif_set_file_timeout(securitykey, objectid, YieldTimeout);
	if(PR_FAIL(error))
	{
		PR_TRACE(( g_root, prtERROR, "klif_set_file_timeout failed error %d", error));
		return error;
	}

	return errOK;
} // YieldEvent


tERROR cMacAvpgImpl::SetPreverdict( hOBJECT EventObject, tAVPG2_PROCESS_OBJECT_RESULT_FLAGS ResultFlags )
{
	long objectid;
	tERROR error = CALL_SYS_PropertyGet( EventObject, 0, propid_ObjectID, &objectid, sizeof(objectid) );	// cAVPG2_PROP_OBJECTID
	if(PR_FAIL(error))
	{
		PR_TRACE(( g_root, prtERROR, "cannot find custom property cAVPG2_PROP_OBJECTID"));
	}
	
	int klifAction = AvgpToKlif(ResultFlags);
	if(PR_SUCC(error))
		error = klif_set_file_default_action(securitykey, objectid, klifAction);
	if(PR_FAIL(error))
	{
		PR_TRACE(( g_root, prtERROR, "klif_set_file_default_action failed error %d", error));
	}

	return error;
} // SetPreverdict


// ban, reset ban