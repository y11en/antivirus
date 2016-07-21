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

	tERROR error = h_TrafficMonitor->sysRegisterMsgHandler(pmc_TASK_STATE_CHANGED, rmhLISTENER,
		h_TrafficMonitor, IID_ANY, PID_ANY, IID_ANY, PID_ANY );

	h_Proto->SetState (TASK_REQUEST_RUN);

	SetEvent (hProtoStateChangeExpected);
	WaitForSingleObject (hProtoStateChanged, INFINITE);
	_ASSERTE (m_ProtoState = TASK_STATE_RUNNING);

	TestDD ("..\\DataManager\\test0");//USER-PASS-RETR

	//TestDD ("..\\DataManager\\test1");//USER-PASS-RETR
	//TestDD ("..\\DataManager\\test2");//USER-PASS-RETR(FAIL)-NOOP-RETR(OK)
	//TestDD ("..\\DataManager\\test3");//APOP-RETR

	//TestDD ("..\\DataManager\\test4");//USER-PASS-STAT-RETR
	//TestDD ("..\\DataManager\\test5");//USER-PASS-STAT NNN-RETR
	//TestDD ("..\\DataManager\\test6");//USER-PASS-LIST-RETR
	//TestDD ("..\\DataManager\\test7");//USER-PASS-LIST NNN-RETR
	//TestDD ("..\\DataManager\\test8");//USER-PASS-UIDL-RETR
	//TestDD ("..\\DataManager\\test9");//USER-PASS-UIDL NNN-RETR

	//TestDD ("..\\DataManager\\test10");//USER-PASS-RETR-RETR
	//TestDD ("..\\DataManager\\test11");//USER-PASS-RSET-RETR
	//TestDD ("..\\DataManager\\test12");//APOP-TOP-RETR

	//auth
	//TestDD ("..\\DataManager\\test20");//AUTH-RETR
	//TestDD ("..\\DataManager\\test21");//APOP(failed)-USER-PASS-RETR
	//TestDD ("..\\DataManager\\test22");//USER-PASS(failed)-USER(OK)-RETR
	//TestDD ("..\\DataManager\\test23");//USER-PASS-DELE-RETR
	//TestDD ("..\\DataManager\\test24");//APOP-NOOP-RETR

	//TestDD ("..\\DataManager\\test25");//USER-PASS-UNKNOWN_COMMAND-RETR-UNKNOWN_COMMAND-RETR
	//TestDD ("..\\DataManager\\test26");//USER-PASS-DELE-QUIT(ERR)

	//TestDD ("..\\DataManager\\test27");//USER-PASS-UNKNOWN_COMMAND-RETR-UNKNOWN_COMMAND-RETR (long commands/arguments)

	//pipelinening
	//TestDD ("..\\DataManager\\test30");//USER-PASS-RETRx2


	//finishing actions

	h_Proto->SetState (TASK_REQUEST_STOP);

	SetEvent (hProtoStateChangeExpected);
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

	CONFIGURE(10000,100)	

	p_SD->SendBytesToProcess (packets.get(), out_packets.get(), true);

	p_SD->VerifyCheckedObjects (control_files.get());
	p_SD->VerifyConnStream (out_packets.get());

	p_SD->StopSession ();
	DeleteSession (p_SD->_sessionCtx.pdwSessionCtx[TEST_PROTO]);
	
	//_ASSERTE(!"Halt");
}

void TestManager::TestMailProcess (const string& data_folder)
{
	LOG_CALL3("Test folder ",data_folder,"\n");

	GetObjects(h_Proto, h_TaskManager, h_TrafficMonitor);
	tERROR error = h_TrafficMonitor->sysRegisterMsgHandler(pmc_TASK_STATE_CHANGED, rmhLISTENER,
		h_TrafficMonitor, IID_ANY, PID_ANY, IID_ANY, PID_ANY );
	h_Proto->SetState (TASK_REQUEST_RUN);
	SetEvent (hProtoStateChangeExpected);
	WaitForSingleObject (hProtoStateChanged, INFINITE);
	_ASSERTE (m_ProtoState = TASK_STATE_RUNNING);


	SessionData* p_SD = SessionData::StartSession (this);

	auto_ptr<vector<string> > packets = aux::get_raw_packets (data_folder.c_str());
	auto_ptr<vector<aux::output_packet> > out_packets = aux::get_output_packets ((hOBJECT)g_root, data_folder.c_str());
	auto_ptr<vector<hIO> > control_files = aux::get_control_files ((hOBJECT)g_root, data_folder.c_str());

	CONFIGURE(10000,100)	

	vector<string> packets_in1;
	vector<aux::output_packet> packets_out1;

	//до RETR включительно
	packets_in1.assign (packets->begin(), packets->begin() + 4);
	packets_out1.assign (out_packets->begin(), out_packets->begin() + 4);
	p_SD->SendBytesToProcess (&packets_in1, &packets_out1);

	//ответ на RETR (подменим файл aka пролечили)
	bTraceMailProcess = true;

	SetEvent (hMailProcessExpected);
	io_ToProto = aux::create_io_from_disk (std::string(data_folder).append("\\object.05.chk.mod"));

		//само сообщение
		packets_in1.assign (packets->begin()+4, packets->begin() + 5);
		packets_out1.assign (out_packets->begin()+4, out_packets->begin() + 5);
		p_SD->SendBytesToProcess (&packets_in1, &packets_out1);

	WaitForSingleObject (hMailProcessReceived, INFINITE);

	//и остальное
	bTraceMailProcess = false;
	packets_in1.assign (packets->begin()+5, packets->end() );
	packets_out1.assign (out_packets->begin()+5, out_packets->end() );
	p_SD->SendBytesToProcess (&packets_in1, &packets_out1);

	p_SD->VerifyCheckedObjects (control_files.get());
	p_SD->VerifyConnStream (out_packets.get());

	p_SD->StopSession ();
	DeleteSession (p_SD->_sessionCtx.pdwSessionCtx[TEST_PROTO]);

	//_ASSERTE(!"Halt");
}