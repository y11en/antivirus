#include "POP3Protocoller\POP3SessionAnalyzer.h"

#include "_aux.h"

#include <utils/prague/prague.hpp>
#include <plugin/p_tempfile.h>
#include <plugin/p_tm.h>

#include "../TaskManager/i_fake_taskmanager.h"

#include "TestManager.h"
#include <string>
using namespace std;


/*tPROPID  g_propVirtualName = 0;
tPROPID  g_propMailerPID = 0;
tPROPID  g_propMessageIsIncoming = 0;
tPROPID  g_propMsgDirection = 0;
tPROPID  g_propStartScan = 0;
tPROPID g_propMailMessageOS_PID = 0;
*/

#include <iface/i_taskex.h>
#include <plugin/p_pop3protocoller.h>

hROOT g_root;
/*
int main()
{
srand(time(0));

TestManager tm;

tm.LoadPrague();

tm.TestDD_Runner();
tm.TestMailProcess("..\\DataManager\\test50");

return 0;
}
*/
/*
int main()
{
srand(time(0));

static char const* plugins[] = {"nfio.ppl", "prutil.ppl", "tempfile.ppl",
"thpimpl.ppl", "pop3prtc.ppl", "FakeTaskManager.ppl", "FakeTrafficMonitor.ppl", 0};
prague::init_prague init(plugins);
prague::pr_remote_api().GetRoot(&g_root);

tERROR error;
hTASK m_Protocoller;


hTASKMANAGER h_TaskManager;
error = g_root->sysCreateObject(reinterpret_cast<hOBJECT*>(&h_TaskManager), IID_TASKMANAGER, PID_TM);

hOBJECT h_TrafficMonitor;
error = g_root->sysCreateObject(reinterpret_cast<hOBJECT*>(&h_TrafficMonitor), IID_MAILTASK, PID_TRAFFICMONITOR);

error = h_TaskManager->propSetObj( pgTM_TRAFFICMONITOR_PTR, (hOBJECT)h_TrafficMonitor);
error = h_TrafficMonitor->propSetObj( pgTASK_TM, (hOBJECT)h_TaskManager);

error = h_TaskManager->sysCreateObjectDone();
error = h_TrafficMonitor->sysCreateObjectDone();

error = h_TrafficMonitor->sysCreateObject((hOBJECT*)&m_Protocoller, IID_TASK, PID_POP3PROTOCOLLER);
error = m_Protocoller->propSetObj( pgTASK_TM, (hOBJECT)h_TaskManager);
error = m_Protocoller->sysCreateObjectDone();


error = m_Protocoller->SetState (TASK_REQUEST_RUN);
Sleep(2000);


session_t m_hSessionCtx;
tDWORD dwSessionCtx[po_last];
memset(dwSessionCtx, 0, po_last*sizeof(tDWORD));
dwSessionCtx[po_ProxySession] = (tDWORD)0xBBBBBBBB;//???
m_hSessionCtx.pdwSessionCtx = dwSessionCtx;
m_hSessionCtx.dwSessionCtxCount = po_last;

error = m_Protocoller->sysRegisterMsgHandler(pmcTRAFFIC_PROTOCOLLER, rmhDECIDER, h_TrafficMonitor, IID_ANY, PID_ANY, IID_ANY, PID_ANY );
error = h_TrafficMonitor->sysSendMsg (pmcTRAFFIC_PROTOCOLLER,	pmSESSION_START, 0,	&m_hSessionCtx, SER_SENDMSG_PSIZE);
error = m_Protocoller->sysUnregisterMsgHandler(pmcTRAFFIC_PROTOCOLLER, h_TrafficMonitor);

auto_ptr<string> image;

//detect
detect_t hDetectParams;
detect_code_t dcDetectCode;
//data_source_t dsDataSource = dsServer;

hOBJECT m_hProtocoller;

hDetectParams.assign(m_hSessionCtx, false);
hDetectParams.pdcDetectCode = &dcDetectCode;
hDetectParams.phDetectorHandle = &m_hProtocoller;


image = aux::getFileImage ("h:\\pop3test\\01.txt");
hDetectParams.dsDataSource = dsServer;
hDetectParams.pLastReceivedData = (tVOID*)image->c_str();
hDetectParams.ulLastReceivedData = image->size();
error = h_TaskManager->sysSendMsg (pmcTRAFFIC_PROTOCOLLER, pmDETECT, 0, &hDetectParams, SER_SENDMSG_PSIZE);
_ASSERTE (error == errOK_NO_DECIDERS && dcDetectCode == dcNeedMoreInfo);

image = aux::getFileImage ("h:\\pop3test\\02.txt");
hDetectParams.dsDataSource = dsClient;
hDetectParams.pLastReceivedData = (tVOID*)image->c_str();
hDetectParams.ulLastReceivedData = image->size();
error = h_TaskManager->sysSendMsg (pmcTRAFFIC_PROTOCOLLER, pmDETECT, 0, &hDetectParams, SER_SENDMSG_PSIZE);
_ASSERTE (error == errOK_NO_DECIDERS && dcDetectCode == dcNeedMoreInfo);

image = aux::getFileImage ("h:\\pop3test\\03.txt");
hDetectParams.dsDataSource = dsServer;
hDetectParams.pLastReceivedData = (tVOID*)image->c_str();
hDetectParams.ulLastReceivedData = image->size();
error = h_TaskManager->sysSendMsg (pmcTRAFFIC_PROTOCOLLER, pmDETECT, 0, &hDetectParams, SER_SENDMSG_PSIZE);
_ASSERTE (error == errOK_NO_DECIDERS && dcDetectCode == dcNeedMoreInfo);

image = aux::getFileImage ("h:\\pop3test\\04.txt");
hDetectParams.dsDataSource = dsClient;
hDetectParams.pLastReceivedData = (tVOID*)image->c_str();
hDetectParams.ulLastReceivedData = image->size();
error = h_TaskManager->sysSendMsg (pmcTRAFFIC_PROTOCOLLER, pmDETECT, 0, &hDetectParams, SER_SENDMSG_PSIZE);
_ASSERTE (error == errOK_NO_DECIDERS && dcDetectCode == dcNeedMoreInfo);

image = aux::getFileImage ("h:\\pop3test\\05.txt");
hDetectParams.dsDataSource = dsServer;
hDetectParams.pLastReceivedData = (tVOID*)image->c_str();
hDetectParams.ulLastReceivedData = image->size();
error = h_TaskManager->sysSendMsg (pmcTRAFFIC_PROTOCOLLER, pmDETECT, 0, &hDetectParams, SER_SENDMSG_PSIZE);
_ASSERTE (error == errOK_NO_DECIDERS && dcDetectCode == dcNeedMoreInfo);

image = aux::getFileImage ("h:\\pop3test\\06.txt");
hDetectParams.dsDataSource = dsClient;
hDetectParams.pLastReceivedData = (tVOID*)image->c_str();
hDetectParams.ulLastReceivedData = image->size();
error = h_TaskManager->sysSendMsg (pmcTRAFFIC_PROTOCOLLER, pmDETECT, 0, &hDetectParams, SER_SENDMSG_PSIZE);
_ASSERTE (error == errOK_DECIDED && dcDetectCode == dcDataDetected);


//process
image = aux::getFileImage ("h:\\pop3test\\07.txt");
process_status_t psStatus;
process_t hProcessParams;
hProcessParams.assign(m_hSessionCtx, false);
hProcessParams.dsDataSource = dsServer;
hProcessParams.pData = (tVOID*)image->c_str();
hProcessParams.ulDataSize = image->size();
hProcessParams.phDetectorHandle = &m_hProtocoller;
hProcessParams.psStatus = &psStatus;
error = h_TaskManager->sysSendMsg (pmcTRAFFIC_PROTOCOLLER,	pmPROCESS, 0, &hProcessParams, SER_SENDMSG_PSIZE);
_ASSERTE (error == errOK_DECIDED && psStatus == psKeepAlive);

//process
image = aux::getFileImage ("h:\\pop3test\\08.txt");
hProcessParams.dsDataSource = dsClient;
hProcessParams.pData = (tVOID*)image->c_str();
hProcessParams.ulDataSize = image->size();
error = h_TaskManager->sysSendMsg (pmcTRAFFIC_PROTOCOLLER,	pmPROCESS, 0, &hProcessParams, SER_SENDMSG_PSIZE);
_ASSERTE (error == errOK_DECIDED && psStatus == psKeepAlive);

//process
image = aux::getFileImage ("h:\\pop3test\\09.txt");
hProcessParams.dsDataSource = dsServer;
hProcessParams.pData = (tVOID*)image->c_str();
hProcessParams.ulDataSize = image->size();
error = h_TaskManager->sysSendMsg (pmcTRAFFIC_PROTOCOLLER,	pmPROCESS, 0, &hProcessParams, SER_SENDMSG_PSIZE);
_ASSERTE (error == errOK_DECIDED && psStatus == psKeepAlive);

//process
image = aux::getFileImage ("h:\\pop3test\\10.txt");
hProcessParams.dsDataSource = dsClient;
hProcessParams.pData = (tVOID*)image->c_str();
hProcessParams.ulDataSize = image->size();
error = h_TaskManager->sysSendMsg (pmcTRAFFIC_PROTOCOLLER,	pmPROCESS, 0, &hProcessParams, SER_SENDMSG_PSIZE);
_ASSERTE (error == errOK_DECIDED && psStatus == psKeepAlive);


_ASSERTE(!"Halt");


return 0;
}
*/