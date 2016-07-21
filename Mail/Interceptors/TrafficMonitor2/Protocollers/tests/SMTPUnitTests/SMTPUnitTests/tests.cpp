#include "TestManager.h"
#include "_aux.h"
#include "config.h"
#include "tracer.h"

#include <structs/s_mc.h>

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

	TestDD ("..\\DataManager\\test1"); //simple test
	//TestDD ("..\\DataManager\\test2");//<rcpt to> failed
	//TestDD ("..\\DataManager\\test3");//ehlo
	//TestDD ("..\\DataManager\\test4");//duplicated <mail from>
	//TestDD ("..\\DataManager\\test5");//rset
	//TestDD ("..\\DataManager\\test6");// data, data(failed), data

	//TestDD ("..\\DataManager\\test7"); //unknown commands after dcDataDetected
	//TestDD ("..\\DataManager\\test8"); //unknown commands before dcDataDetected
	
	//TestDD ("..\\DataManager\\test10"); //multiline responses

	//TestDD ("..\\DataManager\\test20"); //pipelining (rcpt to)
	//TestDD ("..\\DataManager\\test21"); //pipelining (rcpt to + data) - бага, после фикса развить тему

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

	CONFIGURE(100000,100)	

	p_SD->SendBytesToProcess (packets.get(), out_packets.get(), true);

	p_SD->VerifyCheckedObjects (control_files.get());
	p_SD->VerifyConnStream (out_packets.get());

	p_SD->StopSession ();
	DeleteSession (p_SD->_sessionCtx.pdwSessionCtx[TEST_PROTO]);

	//_ASSERTE(!"Halt");
}

void TestManager::TestMailProcess_WithVirus (const string& data_folder, int i_no)
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

	//до DATA включительно
	packets_in1.assign (packets->begin(), packets->begin() + i_no);
	packets_out1.assign (out_packets->begin(), out_packets->begin() + i_no);
	p_SD->SendBytesToProcess (&packets_in1, &packets_out1);

	//ответ на DATA (подменим файл aka пролечили)
	bTraceMailProcess = true;

	SetEvent (hMailProcessExpected);
	io_ToProto = aux::create_io_from_disk (std::string(data_folder).append("\\object.10.chk.mod"));
	mc_Verdict = mcv_MESSAGE_AV_DISINFECTED;

	//само сообщение (должно идти насквозь на сервер)
	packets_in1.assign (packets->begin()+i_no, packets->begin() + 1 + i_no);
	packets_out1.assign (out_packets->begin()+i_no, out_packets->begin() + 1 + i_no);
	p_SD->SendBytesToProcess (&packets_in1, &packets_out1);

	//теперь дошлем \r\n.\r\n
	char eol [] = "\x0d\x0a.\x0d\x0a";
	_ASSERTE (p_SD->TranslateData(eol, eol + 5, dsClient));

	WaitForSingleObject (hMailProcessReceived, INFINITE);

	//и остальное (хотя его и нет)
	bTraceMailProcess = false;
	packets_in1.assign (packets->begin()+1+i_no, packets->end() );
	packets_out1.assign (out_packets->begin()+1+i_no, out_packets->end() );
	p_SD->SendBytesToProcess (&packets_in1, &packets_out1);

	p_SD->VerifyCheckedObjects (control_files.get());

	//добавим сообщение, которое клиенту подсунул мейлчекер
	out_packets->push_back( aux::output_packet (
		string(data_folder).append("\\dat.finish"), dsServer, psCloseConnection));
	p_SD->VerifyConnStream (out_packets.get());
	//проверим, что было велено закрыть соединение
	_ASSERTE (p_SD->bOrderedToCloseConnection);

	p_SD->StopSession ();
	DeleteSession (p_SD->_sessionCtx.pdwSessionCtx[TEST_PROTO]);

}