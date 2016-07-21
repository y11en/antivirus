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

	SetEvent (hProtoStateChangeExpected);
	h_Proto->SetState (TASK_REQUEST_RUN);
	
	WaitForSingleObject (hProtoStateChanged, INFINITE);
	_ASSERTE (m_ProtoState = TASK_STATE_RUNNING);

//RFC
	//TestDD ("..\\DataManager\\test1"); // FETCH N RFC822	
	//TestDD ("..\\DataManager\\test2"); // FETCH N RFC822.text	 - не проверяется, ошибка- исправлено
	TestDD ("..\\DataManager\\test3"); // FETCH N RFC822.PEEK
	//TestDD ("..\\DataManager\\test4"); // FETCH N RFC822.HEADER
	TestDD ("..\\DataManager\\test5"); // FETCH N FLAGS
	//TestDD ("..\\DataManager\\test6"); // FETCH N:M RFC822
	//TestDD ("..\\DataManager\\test7"); // FETCH N ENVELOPE RFC822
	//TestDD ("..\\DataManager\\test8"); // FETCH N RFC822 ENVELOPE
	TestDD ("..\\DataManager\\test9"); // FETCH N INTERNALDATE RFC822 ENVELOPE
	//TestDD ("..\\DataManager\\test10"); // FETCH N INTERNALDATE RFC822 - исправлено
	//TestDD ("..\\DataManager\\test11"); // FETCH N FLAGS RFC822
	//TestDD ("..\\DataManager\\test12"); // FETCH N BODYSTRUCTURE RFC822
	//TestDD ("..\\DataManager\\test13"); // FETCH N BODY RFC822
	//TestDD ("..\\DataManager\\test14"); // FETCH N RFC822.SIZE RFC822
	//TestDD ("..\\DataManager\\test15"); // FETCH N RFC822.HEADER RFC822 
	//TestDD ("..\\DataManager\\test16"); // FETCH N UID RFC822

	TestDD ("..\\DataManager\\test17"); // FETCH N:M (ENVELOPE RFC822)

//BODY
	//TestDD ("..\\DataManager\\test30"); // FETCH N BODY[]
	//TestDD ("..\\DataManager\\test31"); // FETCH N BODY[M]
	//TestDD ("..\\DataManager\\test32"); // FETCH N (BODY[M] BODY[K])
	//TestDD ("..\\DataManager\\test33"); // FETCH N (ENVELOPE BODY[M])
	//TestDD ("..\\DataManager\\test34"); // FETCH N INTERNALDATE RFC822 - не проверяется, это все internaldate - исправлено
	//TestDD ("..\\DataManager\\test35"); // FETCH N (BODY[M] ENVELOPE BODY[K])
	//TestDD ("..\\DataManager\\test36"); // FETCH N BODY.PEEK[M]

	//TestDD ("..\\DataManager\\test37"); // FETCH N BODY[TEXT] - не проверяется - исправлено
	//TestDD ("..\\DataManager\\test38"); // FETCH N BODY[M].text - не проверяется - исправлено

//PARTIAL
	//TestDD ("..\\DataManager\\test40"); // FETCH N BODY[M]<<partial>> - не проверяется - исправлено
	//TestDD ("..\\DataManager\\test41"); // FETCH N BODY[]<<partial>> - не проверяется - исправлено

//UID
	//TestDD ("..\\DataManager\\test50"); // UID FETCH N RFC822	

//Pipelining
	//TestDD ("..\\DataManager\\test60"); // pipelining 

//Errors in dialogue
	//TestDD ("..\\DataManager\\test62");

//Protocol Detection (commands sequence)
	//TestDD ("..\\DataManager\\test70");
	//TestDD ("..\\DataManager\\test71");
	//TestDD ("..\\DataManager\\test72");
	//TestDD ("..\\DataManager\\test73");
	//TestDD ("..\\DataManager\\test74");
	//TestDD ("..\\DataManager\\test75");

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


void TestManager::TestMailProcess_Disinfect (const string& data_folder)
{
	LOG_CALL3("Test folder ",data_folder,"\n");

	GetObjects(h_Proto, h_TaskManager, h_TrafficMonitor);
	tERROR error = h_TrafficMonitor->sysRegisterMsgHandler(pmc_TASK_STATE_CHANGED, rmhLISTENER,
		h_TrafficMonitor, IID_ANY, PID_ANY, IID_ANY, PID_ANY );
	SetEvent (hProtoStateChangeExpected);
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

	//до FETCH включительно
	packets_in1.assign (packets->begin(), packets->begin() + 8);
	packets_out1.assign (out_packets->begin(), out_packets->begin() + 8);
	p_SD->SendBytesToProcess (&packets_in1, &packets_out1);

	//ответ на FETCH (подменим файл aka пролечили)
	bTraceMailProcess = true;

	SetEvent (hMailProcessExpected);
	mc_ToProto_flags = mcv_MESSAGE_AV_DISINFECTED;
	mc_ToProto_virname = "Mega.Virus";
	io_ToProto = aux::create_io_from_disk (std::string(data_folder).append("\\object.09.chk.mod"));

	//само сообщение
	packets_in1.assign (packets->begin()+8, packets->begin() + 9);
	packets_out1.assign (out_packets->begin()+8, out_packets->begin() + 9);
	p_SD->SendBytesToProcess (&packets_in1, &packets_out1);

	WaitForSingleObject (hMailProcessReceived, INFINITE);

	//и остальное
	bTraceMailProcess = false;
	packets_in1.assign (packets->begin()+9, packets->end() );
	packets_out1.assign (out_packets->begin()+9, out_packets->end() );
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
	tERROR error = h_TrafficMonitor->sysRegisterMsgHandler(pmc_TASK_STATE_CHANGED, rmhLISTENER,
					h_TrafficMonitor, IID_ANY, PID_ANY, IID_ANY, PID_ANY );
	SetEvent (hProtoStateChangeExpected);
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

	//до FETCH включительно
	packets_in1.assign (packets->begin(), packets->begin() + 8);
	packets_out1.assign (out_packets->begin(), out_packets->begin() + 8);
	p_SD->SendBytesToProcess (&packets_in1, &packets_out1);

	//ответ на FETCH (удалим файл (вирус неизлечим)
	bTraceMailProcess = true;

	SetEvent (hMailProcessExpected);
	mc_ToProto_flags = mcv_MESSAGE_DELETED;
	mc_ToProto_virname = "Mega.Virus";

	//само сообщение
	packets_in1.assign (packets->begin()+8, packets->begin() + 9);
	packets_out1.assign (out_packets->begin()+8, out_packets->begin() + 9);
	p_SD->SendBytesToProcess (&packets_in1, &packets_out1);

	WaitForSingleObject (hMailProcessReceived, INFINITE);

	//и остальное
	bTraceMailProcess = false;
	packets_in1.assign (packets->begin()+9, packets->end() );
	packets_out1.assign (out_packets->begin()+9, out_packets->end() );
	p_SD->SendBytesToProcess (&packets_in1, &packets_out1);

	p_SD->VerifyCheckedObjects (control_files.get());
	p_SD->VerifyConnStream (out_packets.get());

	p_SD->StopSession ();
	DeleteSession (p_SD->_sessionCtx.pdwSessionCtx[TEST_PROTO]);

}