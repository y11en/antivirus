#include "TestManager.h"
#include "_aux.h"
#include "config.h"
#include "tracer.h"

#define CONFIGURE(SIZE,TIMEOUT)\
	Configure::GetConfiguration().MaximumChunkSize(SIZE);\
	Configure::GetConfiguration().TimeoutBetweenChunks(TIMEOUT);

void TestManager::TestDD_Runner ()
{
	GetObjects(h_Proto, h_TaskManager, h_TrafficMonitor);

	SetEvent (hProtoStateChangeExpected);
	tERROR error = h_TrafficMonitor->sysRegisterMsgHandler(pmc_TASK_STATE_CHANGED, rmhLISTENER,
		h_TrafficMonitor, IID_ANY, PID_ANY, IID_ANY, PID_ANY );

	h_Proto->SetState (TASK_REQUEST_RUN);
	
	WaitForSingleObject (hProtoStateChanged, INFINITE);
	_ASSERTE (m_ProtoState = TASK_STATE_RUNNING);

	//retrieving
	TestDD ("..\\DataManager\\test1"); // article N - 220
	TestDD ("..\\DataManager\\test2"); // article <id> - 220
	TestDD ("..\\DataManager\\test3"); // head N - 221
	TestDD ("..\\DataManager\\test4"); // head <id> - 221
	TestDD ("..\\DataManager\\test5"); // body N - 222//        | не работает
	TestDD ("..\\DataManager\\test6"); // body <id> - 222		|

	//posting
	TestDD ("..\\DataManager\\test10"); // post - ok
	//TestDD ("..\\DataManager\\test11"); // ihave - ok - не работает

	//pipelining
	//TestDD ("..\\DataManager\\test30"); // pipelining - не работает

	//command sequence (protocol detecting)
	TestDD ("..\\DataManager\\test40");
	TestDD ("..\\DataManager\\test41");
	TestDD ("..\\DataManager\\test42");
	TestDD ("..\\DataManager\\test43");


	//finishing actions

	SetEvent (hProtoStateChangeExpected);
	h_Proto->SetState (TASK_REQUEST_STOP);
	
	WaitForSingleObject (hProtoStateChanged, INFINITE);
	_ASSERTE (m_ProtoState = TASK_STATE_STOPPED);

}

void TestManager::TestDD (const string& data_folder)
{
	LOG_CALL3("Test folder ",data_folder,"\n");

	SessionData* p_SD = SessionData::StartSession (this);

	auto_ptr<vector<string> > packets = aux::get_raw_packets (data_folder.c_str());
	auto_ptr<vector<aux::output_packet> > out_packets = aux::get_output_packets ((hOBJECT)g_root, data_folder.c_str());
	auto_ptr<vector<hIO> > control_files = aux::get_control_files ((hOBJECT)g_root, data_folder.c_str());

	CONFIGURE(100000,10)	//осторожнее, чтобы не попасть посередине 0D0A (баг)

	p_SD->SendBytesToProcess (packets.get(), out_packets.get(), true);

	p_SD->VerifyCheckedObjects (control_files.get());
	p_SD->VerifyConnStream (out_packets.get());

	p_SD->StopSession ();
	DeleteSession (p_SD->_sessionCtx.pdwSessionCtx[TEST_PROTO]);

}

//
void TestManager::TestMailProcess_Disinfect (const string& data_folder)
{
	LOG_CALL3("Test folder ",data_folder,"\n");

	GetObjects(h_Proto, h_TaskManager, h_TrafficMonitor);

	SetEvent (hProtoStateChangeExpected);
	tERROR error = h_TrafficMonitor->sysRegisterMsgHandler(pmc_TASK_STATE_CHANGED, rmhLISTENER,
		h_TrafficMonitor, IID_ANY, PID_ANY, IID_ANY, PID_ANY );
	
	h_Proto->SetState (TASK_REQUEST_RUN);
	
	WaitForSingleObject (hProtoStateChanged, INFINITE);
	_ASSERTE (m_ProtoState = TASK_STATE_RUNNING);


	SessionData* p_SD = SessionData::StartSession (this);

	auto_ptr<vector<string> > packets = aux::get_raw_packets (data_folder.c_str());
	auto_ptr<vector<aux::output_packet> > out_packets = aux::get_output_packets ((hOBJECT)g_root, data_folder.c_str());
	auto_ptr<vector<hIO> > control_files = aux::get_control_files ((hOBJECT)g_root, data_folder.c_str());

	CONFIGURE(10000,100)	

	vector<string> packets_in1;
	vector<aux::output_packet> packets_out1;

	//до ARTICLE включительно
	packets_in1.assign (packets->begin(), packets->begin() + 6);
	packets_out1.assign (out_packets->begin(), out_packets->begin() + 6);
	p_SD->SendBytesToProcess (&packets_in1, &packets_out1);

	//ответ на ARTICLE (подменим файл aka пролечили)
	bTraceMailProcess = true;

	SetEvent (hMailProcessExpected);
	mc_ToProto_flags = mcv_MESSAGE_AV_DISINFECTED;
	mc_ToProto_virname = "Mega.Virus";
	io_ToProto = aux::create_io_from_disk (std::string(data_folder).append("\\object.07.chk.mod"));

	//само сообщение
	packets_in1.assign (packets->begin()+6, packets->begin() + 7);
	packets_out1.assign (out_packets->begin()+6, out_packets->begin() + 7);
	p_SD->SendBytesToProcess (&packets_in1, &packets_out1);

	WaitForSingleObject (hMailProcessReceived, INFINITE);

	//и остальное
	bTraceMailProcess = false;
	packets_in1.assign (packets->begin()+7, packets->end() );
	packets_out1.assign (out_packets->begin()+7, out_packets->end() );
	p_SD->SendBytesToProcess (&packets_in1, &packets_out1);

	p_SD->VerifyCheckedObjects (control_files.get());
	p_SD->VerifyConnStream (out_packets.get());

	p_SD->StopSession ();
	DeleteSession (p_SD->_sessionCtx.pdwSessionCtx[TEST_PROTO]);

	//_ASSERTE(!"Halt");
}


void TestManager::TestMailProcess_Delete (const string& data_folder)
{
	LOG_CALL3("Test folder ",data_folder,"\n");

	GetObjects(h_Proto, h_TaskManager, h_TrafficMonitor);
	SetEvent (hProtoStateChangeExpected);
	tERROR error = h_TrafficMonitor->sysRegisterMsgHandler(pmc_TASK_STATE_CHANGED, rmhLISTENER,
		h_TrafficMonitor, IID_ANY, PID_ANY, IID_ANY, PID_ANY );
	h_Proto->SetState (TASK_REQUEST_RUN);
	
	WaitForSingleObject (hProtoStateChanged, INFINITE);
	_ASSERTE (m_ProtoState = TASK_STATE_RUNNING);

	SessionData* p_SD = SessionData::StartSession (this);

	auto_ptr<vector<string> > packets = aux::get_raw_packets (data_folder.c_str());
	auto_ptr<vector<aux::output_packet> > out_packets = aux::get_output_packets ((hOBJECT)g_root, data_folder.c_str());
	auto_ptr<vector<hIO> > control_files = aux::get_control_files ((hOBJECT)g_root, data_folder.c_str());

	CONFIGURE(10000,100)	

	vector<string> packets_in1;
	vector<aux::output_packet> packets_out1;

	//до ARTICLE/BODY включительно
	packets_in1.assign (packets->begin(), packets->begin() + 6);
	packets_out1.assign (out_packets->begin(), out_packets->begin() + 6);
	p_SD->SendBytesToProcess (&packets_in1, &packets_out1);

	//ответ на ARTICLE/BODY (удалим файл (вирус неизлечим)
	bTraceMailProcess = true;

	SetEvent (hMailProcessExpected);
	mc_ToProto_flags = mcv_MESSAGE_DELETED;
	mc_ToProto_virname = "Mega.Virus";

	//само сообщение
	packets_in1.assign (packets->begin()+6, packets->begin() + 7);
	packets_out1.assign (out_packets->begin()+6, out_packets->begin() + 7);
	p_SD->SendBytesToProcess (&packets_in1, &packets_out1);

	WaitForSingleObject (hMailProcessReceived, INFINITE);

	//и остальное
	bTraceMailProcess = false;
	packets_in1.assign (packets->begin()+7, packets->end() );
	packets_out1.assign (out_packets->begin()+7, out_packets->end() );
	p_SD->SendBytesToProcess (&packets_in1, &packets_out1);

	p_SD->VerifyCheckedObjects (control_files.get());
	p_SD->VerifyConnStream (out_packets.get());

	p_SD->StopSession ();
	DeleteSession (p_SD->_sessionCtx.pdwSessionCtx[TEST_PROTO]);

}
