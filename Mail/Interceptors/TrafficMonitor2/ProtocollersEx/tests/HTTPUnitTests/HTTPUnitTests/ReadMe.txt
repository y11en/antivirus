========================================================================
    CONSOLE APPLICATION : HTTPUnitTests Project Overview
========================================================================

AppWizard has created this HTTPUnitTests application for you.  

This file contains a summary of what you will find in each of the files that
make up your HTTPUnitTests application.


HTTPUnitTests.vcproj
    This is the main project file for VC++ projects generated using an Application Wizard. 
    It contains information about the version of Visual C++ that generated the file, and 
    information about the platforms, configurations, and project features selected with the
    Application Wizard.

HTTPUnitTests.cpp
    This is the main application source file.

/////////////////////////////////////////////////////////////////////////////
Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named HTTPUnitTests.pch and a precompiled types file named StdAfx.obj.

/////////////////////////////////////////////////////////////////////////////
Other notes:

AppWizard uses "TODO:" comments to indicate parts of the source code you
should add to or customize.

/////////////////////////////////////////////////////////////////////////////

/*
#include <iface/i_taskex.h>

tERROR pr_call fMsgReceive( 
						   hOBJECT _this, 
						   tDWORD p_msg_cls_id, 
						   tDWORD p_msg_id, 
						   hOBJECT p_send_point, 
						   hOBJECT p_ctx, 
						   hOBJECT p_receive_point, 
						   tVOID* p_par_buf, 
						   tDWORD* p_par_buf_len 
						   )
{
	return errOK;
}


#include <plugin/p_imapprotocoller.h>

int _tmain(int argc, _TCHAR* argv[])
{
	using namespace TrafficProtocoller;

	srand(time(0));

	static char const* plugins[] = {"nfio.ppl", "prutil.ppl", "tempfile.ppl", "httpanlz.ppl", "thpimpl.ppl", "imapprtc.ppl", 0};
	prague::init_prague init(plugins);
	prague::pr_remote_api().GetRoot(&g_root);
	
	cObj* m_httpProtocoller;
	CTrafficMonitor mc;
	//cTestTaskManager* tm = new cTestTaskManager();

	tERROR error;

	//error = g_root->sysCreateObject((hOBJECT*)&m_httpProtocoller, IID_TASKEX, PID_HTTPPROTOCOLLER);
	error = g_root->sysCreateObject((hOBJECT*)&m_httpProtocoller, IID_TASKEX, PID_IMAPPROTOCOLLER);
	error = m_httpProtocoller->propSetObj( pgTASK_TM, (hOBJECT)g_root);
	//error = m_httpProtocoller->SetService
	//error = m_httpProtocoller->propSetDWord( pgTASK_ID, 1);
	error = m_httpProtocoller->sysCreateObjectDone();

	cMsgReceiver* m_hMsgReceiver;
	error = g_root->sysCreateObject ((hOBJECT*)&m_hMsgReceiver, IID_MSG_RECEIVER);
	//error = m_hMsgReceiver->set_pgRECEIVE_PROCEDURE((tFUNC_PTR)fMsgReceive);
	error = m_hMsgReceiver->sysCreateObjectDone();
//	error = m_hMsgReceiver->sysRegisterMsgHandler (pmcTRAFFIC_PROTOCOLLER, rmhDECIDER, g_root, IID_ANY, PID_ANY, IID_ANY, PID_ANY);

cAutoObj<cObj> task;
err = m_host->sysCreateObject((cObj**)&task, m_iid, m_pid);
task->propSetDWord( pgTASK_ID, id() );
task->propSetDWord( pgTASK_PARENT_ID, nActivityId );
task->propSetDWord( m_tm->m_prop_taskid, nActivityId );
task->sysCreateObjectDone();


	error = ((cTaskEx*)m_httpProtocoller)->SetState (TASK_REQUEST_RUN);

	session_t m_hSessionCtx;
	tDWORD dwSessionCtx[po_last];
	memset(dwSessionCtx, 0, po_last*sizeof(tDWORD));
	dwSessionCtx[po_ProxySession] = (tDWORD)&mc;
	m_hSessionCtx.pdwSessionCtx = dwSessionCtx;
	m_hSessionCtx.dwSessionCtxCount = po_last;

	Sleep (5000);
	g_root->sysSendMsg (pmcTRAFFIC_PROTOCOLLER,	pmSESSION_START, 0,	&m_hSessionCtx, SER_SENDMSG_PSIZE);
	
}
*/