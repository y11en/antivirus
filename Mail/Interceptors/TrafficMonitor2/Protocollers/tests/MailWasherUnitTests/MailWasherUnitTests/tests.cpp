#include "TestManager.h"
#include "_aux.h"
#include "config.h"
#include "tracer.h"

#include <structs/s_mailwasher.h>

#define WAIT_FOR_SESSION_COMPLETION \
WaitForSingleObject (p_SD->hWasherSessionFinished, INFINITE);\
Sleep (3000);

#define DELETED_MARK "Subject: [!! SPAM] The message has been deleted\r\n"
 
//≈сли вдруг когда-то баги будут закрыты, необходимо доработать тесты, добавив
//1. ѕроверку переданных вошером писем (содержание, ну или хот€ бы размер) - —делано в критических местах
//2. ѕроверку нормальной обработки команд UIDL, LIST, STAT и т.д.
//3. ѕередачу данных в вошер порци€ми различных размеров - »справлено
//4. Ќормальную отработку команды TOP на письмо-квиток - »справлено
//5. "”мное" кэширование, т.е. вошер запросил RETR, клиент просит TOP - можно не лезть на сервер еще раз
//6. ƒобавить сценарии старта сессии вошера в тесты POP3
//7. ѕроверка, что все работает в случае, если перед стартом сессии че-то уже удал€лось - «аработало
//8. ѕроверка, если TOP/RETR со стороны MW обламываетс€.

/*
void TestManager::TestDD_Runner ()
{
	
	TestDD ("..\\DataManager\\test1");//USER-PASS-RETR
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
*/

/*
void TestManager::Test1 (const string& data_folder)
{
	SessionData* p_SD = SessionData::StartSession (this, data_folder);
	
	DWORD err;
	//p_SD->wGetLetterTop(0, 7, raw_packets->at(0), false);
	//p_SD->wGetLetterTop(1, 7, raw_packets->at(1), false);
	//p_SD->wGetLetterTop(2, 7, raw_packets->at(2), false);
	//p_SD->wGetLetterTop(3, 7, raw_packets->at(3), false);

	//p_SD->wGetLetterTops (0, 4, 7, *raw_packets);
	//p_SD->wGetLetterTops (0, 4, 7, *raw_packets);

	//p_SD->wGetLetter(0, -1, raw_packets->at(0), false);
	//p_SD->wGetLetter(1, -1, raw_packets->at(1), false);
	//p_SD->wGetLetter(2, -1, raw_packets->at(2), false);
	//p_SD->wGetLetter(3, -1, raw_packets->at(3), false);

	//p_SD->wGetLetters (0, 4, -1, *raw_packets);
	//p_SD->wGetLetters (0, 4, -1, *p_SD->raw_packets);

	//

	RETR_cmd tc;

	tc.no = 1;
	tc.answer = raw_packets->at(0);
	p_SD->cmds_stor.push_back(&tc);


	err = h_Proto->sysSendMsg
					(pmc_MAIL_WASHER, pm_SESSION_CLOSE, 0, &p_SD->washerSession, SER_SENDMSG_PSIZE);
	PR_ASSERT(PR_SUCC(err));
	//WaitForSingleObject (p_SD->hWasherSessionFinished, INFINITE);
	WAIT_FOR_SESSION_COMPLETION
	
	tc.no = 2;
	tc.answer = p_SD->raw_packets->at(1);
	p_SD->cmds_stor.push_back(&tc);

	p_SD->process_msg.pData = "RETR 2\x0d\x0a";
	err = h_Proto->sysSendMsg( 
		pmcTRAFFIC_PROTOCOLLER, 
		pmPROCESS, 
		0, 
		&p_SD->process_msg, 
		SER_SENDMSG_PSIZE 
		);
	//WaitForSingleObject (p_SD->hReqToServerGot, INFINITE);
	//p_SD->TranslateData
	//	(raw_packets->at(1).c_str(), raw_packets->at(1).c_str() + raw_packets->at(1).size(), dsServer);


	process_msg.pData = "RETR 2\x0d\x0a";
	err = h_Proto->sysSendMsg( 
		pmcTRAFFIC_PROTOCOLLER, 
		pmPROCESS, 
		0, 
		&process_msg, 
		SER_SENDMSG_PSIZE 
		);

	//Sleep(INFINITE);



	//finishing actions

	//h_Proto->SetState (TASK_REQUEST_STOP);

	//SetEvent (hProtoStateChangeExpected);
	//WaitForSingleObject (hProtoStateChanged, INFINITE);
	//_ASSERTE (m_ProtoState = TASK_STATE_STOPPED);

	/*cPOP3MessageReguest mr;
	mr.assign (p_SD->washerSession, false);
	err = p_SD->m_testManager->h_TrafficMonitor->sysSendMsg
		(pmc_MAIL_WASHER, pm_MESSAGE_GETLIST, 0,&mr, SER_SENDMSG_PSIZE);
	PR_ASSERT(PR_SUCC(err));




	//Sleep(INFINITE);

	//p_SD->VerifyCheckedObjects (control_files.get());
	//p_SD->VerifyConnStream (out_packets.get());


	p_SD->StopSession ();
	DeleteSession (p_SD->_sessionCtx.pdwSessionCtx[TEST_PROTO]);

	//_ASSERTE(!"Halt");

}
*/

/*
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
*/


void TestManager::TestHeadersRequest (const string& data_folder)
{
	SessionData* p_SD = SessionData::StartSession (this, data_folder);

	DWORD err;
	p_SD->wGetLetterTop(0, 7, p_SD->raw_packets->at(0), false);
	//p_SD->wGetLetterTop(1, 7, p_SD->raw_packets->at(1), false);
	//p_SD->wGetLetterTop(2, 7, p_SD->raw_packets->at(2), false);
	p_SD->wGetLetterTop(3, 7, p_SD->raw_packets->at(3), false);

	p_SD->wGetLetterTop(0, 7, p_SD->raw_packets->at(0), true);
	p_SD->wGetLetterTop(1, 7, p_SD->raw_packets->at(1), false);
	//p_SD->wGetLetterTop(2, 7, p_SD->raw_packets->at(2), true);
	p_SD->wGetLetterTop(3, 7, p_SD->raw_packets->at(3), true);

	bool arr[4] = {true, true, false, true};
	bool arr2[4] = {true, true, true, true};
	p_SD->wGetLetterTops (0, 4, 7, *p_SD->raw_packets, arr);
	p_SD->wGetLetterTops (0, 4, 7, *p_SD->raw_packets, arr2);

	RETR_cmd tc;
	tc.no = 1;
	tc.answer = p_SD->raw_packets->at(0);
	p_SD->cmds_stor.push_back(&tc);

	err = h_Proto->sysSendMsg
		(pmc_MAIL_WASHER, pm_SESSION_CLOSE, 0, &p_SD->washerSession, SER_SENDMSG_PSIZE);
	PR_ASSERT(PR_SUCC(err));

	WaitForSingleObject (p_SD->hReqToServerGot, INFINITE);
	p_SD->TranslateData
		(p_SD->raw_packets->at(1).c_str(), p_SD->raw_packets->at(1).c_str() + p_SD->raw_packets->at(1).size(), dsServer);


	//WaitForSingleObject (p_SD->hWasherSessionFinished, INFINITE);
	WAIT_FOR_SESSION_COMPLETION


	//finishing actions
	p_SD->StopSession ();
	DeleteSession (p_SD->_sessionCtx.pdwSessionCtx[TEST_PROTO]);

}

void TestManager::TestMessagesRequest (const string& data_folder)
{
	SessionData* p_SD = SessionData::StartSession (this, data_folder);

	DWORD err;
	p_SD->wGetLetter(0, p_SD->raw_packets->at(0), false);
	//p_SD->wGetLetter(1, p_SD->raw_packets->at(1), false);
	//p_SD->wGetLetter(2, p_SD->raw_packets->at(2), false);
	p_SD->wGetLetter(3, p_SD->raw_packets->at(3), false);

	p_SD->wGetLetter(0, p_SD->raw_packets->at(0), true);
	p_SD->wGetLetter(1, p_SD->raw_packets->at(1), false);
	//p_SD->wGetLetter(2, p_SD->raw_packets->at(2), true);
	p_SD->wGetLetter(3, p_SD->raw_packets->at(3), true);

	bool arr[4] = {true, true, false, true};
	bool arr2[4] = {true, true, true, true};
	p_SD->wGetLetters (0, 4, *p_SD->raw_packets, arr);
	p_SD->wGetLetters (0, 4, *p_SD->raw_packets, arr2);

	err = h_Proto->sysSendMsg
		(pmc_MAIL_WASHER, pm_SESSION_CLOSE, 0, &p_SD->washerSession, SER_SENDMSG_PSIZE);
	PR_ASSERT(PR_SUCC(err));

	//WaitForSingleObject (p_SD->hWasherSessionFinished, INFINITE);
	WAIT_FOR_SESSION_COMPLETION


	//finishing actions
	p_SD->StopSession ();
	DeleteSession (p_SD->_sessionCtx.pdwSessionCtx[TEST_PROTO]);

}


void TestManager::TestRetrMessage_MW_Messages (const string& data_folder)
{//клиент запрашивает сообщени€ целиком (вошер также выкачивал их целиком)
	SessionData* p_SD = SessionData::StartSession (this, data_folder);

	string retr1 ("RETR 1\r\n");
	string retr2 ("RETR 2\r\n");
	string retr3 ("RETR 3\r\n");
	string retr4 ("RETR 4\r\n");

	DWORD err;
	RETR_cmd tc;

	//4-й (мейлвошер)
	p_SD->wGetLetter(0, p_SD->raw_packets->at(3), false);
	//p_SD->wGetLetter(1, p_SD->raw_packets->at(2), false);
	//p_SD->wGetLetter(2, p_SD->raw_packets->at(1), false);
	p_SD->wGetLetter(3, p_SD->raw_packets->at(0), false);

	//1-й (клиент просил)
//	tc.no = 2;
//	tc.answer = p_SD->raw_packets->at(0);
//	p_SD->cmds_stor.push_back(&tc);
	err = h_Proto->sysSendMsg
		(pmc_MAIL_WASHER, pm_SESSION_CLOSE, 0, &p_SD->washerSession, SER_SENDMSG_PSIZE);
	PR_ASSERT(PR_SUCC(err));

//	WaitForSingleObject (p_SD->hReqToServerGot, INFINITE);
//	p_SD->TranslateData
//		(p_SD->raw_packets->at(0).c_str(), p_SD->raw_packets->at(0).c_str() + p_SD->raw_packets->at(0).size(), dsServer);


	//WaitForSingleObject (p_SD->hWasherSessionFinished, INFINITE);
	WAIT_FOR_SESSION_COMPLETION

	string sf ("SESSION FINISHED\r\n-------\r\n--------\r\n");
	_file.Write((char*)sf.c_str(), (char*)sf.c_str()+sf.size());
	_file.Flush ();
	_trace.Write((char*)sf.c_str(), (char*)sf.c_str()+sf.size());
	_trace.Flush ();


	//retr 1
	//cached
	p_SD->TranslateData (retr1.c_str(), retr1.c_str()+retr1.size(), dsClient);

	//retr 2
	tc.no = 2;
	tc.answer = p_SD->raw_packets->at(0);
	p_SD->cmds_stor.push_back(&tc);
	p_SD->TranslateData (retr2.c_str(), retr2.c_str()+retr2.size(), dsClient);
	WaitForSingleObject (p_SD->hReqToServerGot, INFINITE);
	p_SD->TranslateData
		(p_SD->raw_packets->at(1).c_str(), p_SD->raw_packets->at(1).c_str() + p_SD->raw_packets->at(1).size(), dsServer);
	
	//retr 3
	tc.no = 3;
	tc.answer = p_SD->raw_packets->at(0);
	p_SD->cmds_stor.push_back(&tc);
	p_SD->TranslateData (retr3.c_str(), retr3.c_str()+retr3.size(), dsClient);
	WaitForSingleObject (p_SD->hReqToServerGot, INFINITE);
	p_SD->TranslateData
		(p_SD->raw_packets->at(2).c_str(), p_SD->raw_packets->at(2).c_str() + p_SD->raw_packets->at(2).size(), dsServer);

	//retr 4
	//cached
	p_SD->TranslateData (retr4.c_str(), retr4.c_str()+retr4.size(), dsClient);

	//retr 3
	//not cached - Suggestion ???
	//p_SD->TranslateData (retr3.c_str(), retr3.c_str()+retr3.size(), dsClient);

	//finishing actions
	p_SD->StopSession ();
	DeleteSession (p_SD->_sessionCtx.pdwSessionCtx[TEST_PROTO]);

}

void TestManager::TestRetrMessage_MW_Headers (const string& data_folder)
{//клиент запрашивает сообщени€ целиком (вошер также выкачивал их целиком)
	SessionData* p_SD = SessionData::StartSession (this, data_folder);

	string retr1 ("RETR 1\r\n");
	string retr2 ("RETR 2\r\n");
	string retr3 ("RETR 3\r\n");
	string retr4 ("RETR 4\r\n");

	DWORD err;
	RETR_cmd tc;

	//4-й (мейлвошер)
	p_SD->wGetLetterTop(0, 0, p_SD->raw_packets->at(3), false);
	//p_SD->wGetLetter(1, 0, p_SD->raw_packets->at(2), false);
	//p_SD->wGetLetter(2, 0, p_SD->raw_packets->at(1), false);
	p_SD->wGetLetterTop(3, 0, p_SD->raw_packets->at(0), false);

	//1-й (клиент просил)
	tc.no = 2;
	tc.answer = p_SD->raw_packets->at(0);
	p_SD->cmds_stor.push_back(&tc);
	err = h_Proto->sysSendMsg
		(pmc_MAIL_WASHER, pm_SESSION_CLOSE, 0, &p_SD->washerSession, SER_SENDMSG_PSIZE);
	PR_ASSERT(PR_SUCC(err));

	WaitForSingleObject (p_SD->hReqToServerGot, INFINITE);
	p_SD->TranslateData
		(p_SD->raw_packets->at(0).c_str(), p_SD->raw_packets->at(0).c_str() + p_SD->raw_packets->at(0).size(), dsServer);

	//WaitForSingleObject (p_SD->hWasherSessionFinished, INFINITE);
	WAIT_FOR_SESSION_COMPLETION

	string sf ("SESSION FINISHED\r\n-------\r\n--------\r\n");
	_file.Write((char*)sf.c_str(), (char*)sf.c_str()+sf.size());
	_file.Flush ();
	_trace.Write((char*)sf.c_str(), (char*)sf.c_str()+sf.size());
	_trace.Flush ();



	//retr 1
	tc.no = 2;
	p_SD->cmds_stor.push_back(&tc);
	p_SD->TranslateData (retr1.c_str(), retr1.c_str()+retr1.size(), dsClient);
	WaitForSingleObject (p_SD->hReqToServerGot, INFINITE);
	p_SD->TranslateData
		(p_SD->raw_packets->at(0).c_str(), p_SD->raw_packets->at(0).c_str() + p_SD->raw_packets->at(0).size(), dsServer);


	//retr 2
	tc.no = 2;
	p_SD->cmds_stor.push_back(&tc);
	p_SD->TranslateData (retr2.c_str(), retr2.c_str()+retr2.size(), dsClient);
	WaitForSingleObject (p_SD->hReqToServerGot, INFINITE);
	p_SD->TranslateData
		(p_SD->raw_packets->at(1).c_str(), p_SD->raw_packets->at(1).c_str() + p_SD->raw_packets->at(1).size(), dsServer);

	//retr 3
	tc.no = 2;
	p_SD->cmds_stor.push_back(&tc);
	p_SD->TranslateData (retr3.c_str(), retr3.c_str()+retr3.size(), dsClient);
	WaitForSingleObject (p_SD->hReqToServerGot, INFINITE);
	p_SD->TranslateData
		(p_SD->raw_packets->at(2).c_str(), p_SD->raw_packets->at(2).c_str() + p_SD->raw_packets->at(2).size(), dsServer);

	//retr 4
	tc.no = 2;
	tc.answer = p_SD->raw_packets->at(0);
	p_SD->cmds_stor.push_back(&tc);
	p_SD->TranslateData (retr4.c_str(), retr4.c_str()+retr4.size(), dsClient);
	WaitForSingleObject (p_SD->hReqToServerGot, INFINITE);
	p_SD->TranslateData
		(p_SD->raw_packets->at(3).c_str(), p_SD->raw_packets->at(3).c_str() + p_SD->raw_packets->at(3).size(), dsServer);

	//retr 4
	tc.no = 2;
	tc.answer = p_SD->raw_packets->at(0);
	p_SD->cmds_stor.push_back(&tc);
	p_SD->TranslateData (retr4.c_str(), retr4.c_str()+retr4.size(), dsClient);
	WaitForSingleObject (p_SD->hReqToServerGot, INFINITE);
	p_SD->TranslateData
		(p_SD->raw_packets->at(3).c_str(), p_SD->raw_packets->at(3).c_str() + p_SD->raw_packets->at(3).size(), dsServer);

	//finishing actions
	p_SD->StopSession ();
	DeleteSession (p_SD->_sessionCtx.pdwSessionCtx[TEST_PROTO]);

}

void TestManager::TestRetrHeaders_MW_Messages (const string& data_folder)
{//клиент запрашивает заголовок сообщени€ (вошер выкачивал их целиком)
	SessionData* p_SD = SessionData::StartSession (this, data_folder);

	string top1 ("TOP 1 0\r\n");
	string top2 ("TOP 2 0\r\n");
	string top3 ("TOP 3 0\r\n");
	string top4 ("TOP 4 0\r\n");

	DWORD err;
	TOP_cmd tc;

	//4-й (мейлвошер)
	p_SD->wGetLetter(0, p_SD->raw_packets->at(3), false);
	//p_SD->wGetLetter(1, 0, p_SD->raw_packets->at(2), false);
	//p_SD->wGetLetter(2, 0, p_SD->raw_packets->at(1), false);
	p_SD->wGetLetter(3, p_SD->raw_packets->at(0), false);

	err = h_Proto->sysSendMsg
		(pmc_MAIL_WASHER, pm_SESSION_CLOSE, 0, &p_SD->washerSession, SER_SENDMSG_PSIZE);
	PR_ASSERT(PR_SUCC(err));

	//WaitForSingleObject (p_SD->hWasherSessionFinished, INFINITE);
	WAIT_FOR_SESSION_COMPLETION

	string sf ("SESSION FINISHED\r\n-------\r\n--------\r\n");
	_file.Write((char*)sf.c_str(), (char*)sf.c_str()+sf.size());
	_file.Flush ();
	_trace.Write((char*)sf.c_str(), (char*)sf.c_str()+sf.size());
	_trace.Flush ();


	//retr 1
	tc.no = 2;
	p_SD->cmds_stor.push_back(&tc);
	p_SD->TranslateData (top1.c_str(), top1.c_str()+top1.size(), dsClient);
	WaitForSingleObject (p_SD->hReqToServerGot, INFINITE);
	p_SD->TranslateData
		(p_SD->raw_packets->at(0).c_str(), p_SD->raw_packets->at(0).c_str() + p_SD->raw_packets->at(0).size(), dsServer);


	//retr 2
	tc.no = 2;
	p_SD->cmds_stor.push_back(&tc);
	p_SD->TranslateData (top2.c_str(), top2.c_str()+top2.size(), dsClient);
	WaitForSingleObject (p_SD->hReqToServerGot, INFINITE);
	p_SD->TranslateData
		(p_SD->raw_packets->at(1).c_str(), p_SD->raw_packets->at(1).c_str() + p_SD->raw_packets->at(1).size(), dsServer);

	//retr 3
	tc.no = 2;
	p_SD->cmds_stor.push_back(&tc);
	p_SD->TranslateData (top3.c_str(), top3.c_str()+top3.size(), dsClient);
	WaitForSingleObject (p_SD->hReqToServerGot, INFINITE);
	p_SD->TranslateData
		(p_SD->raw_packets->at(2).c_str(), p_SD->raw_packets->at(2).c_str() + p_SD->raw_packets->at(2).size(), dsServer);

	//retr 4
	tc.no = 2;
	tc.answer = p_SD->raw_packets->at(0);
	p_SD->cmds_stor.push_back(&tc);
	p_SD->TranslateData (top4.c_str(), top4.c_str()+top4.size(), dsClient);
	WaitForSingleObject (p_SD->hReqToServerGot, INFINITE);
	p_SD->TranslateData
		(p_SD->raw_packets->at(3).c_str(), p_SD->raw_packets->at(3).c_str() + p_SD->raw_packets->at(3).size(), dsServer);

	//retr 4
	tc.no = 2;
	tc.answer = p_SD->raw_packets->at(0);
	p_SD->cmds_stor.push_back(&tc);
	p_SD->TranslateData (top4.c_str(), top4.c_str()+top4.size(), dsClient);
	WaitForSingleObject (p_SD->hReqToServerGot, INFINITE);
	p_SD->TranslateData
		(p_SD->raw_packets->at(3).c_str(), p_SD->raw_packets->at(3).c_str() + p_SD->raw_packets->at(3).size(), dsServer);

	//finishing actions
	p_SD->StopSession ();
	DeleteSession (p_SD->_sessionCtx.pdwSessionCtx[TEST_PROTO]);

}


void TestManager::TestRetrHeaders_MW_Headers (const string& data_folder)
{//клиент запрашивает заголовки сообщений (вошер тоже выкачивал тока заголовки)
	SessionData* p_SD = SessionData::StartSession (this, data_folder);

	string top1 ("TOP 1 0\r\n");
	string top2 ("TOP 2 0\r\n");
	string top3 ("TOP 3 0\r\n");
	string top4 ("TOP 4 0\r\n");

	DWORD err;
	TOP_cmd tc;
	RETR_cmd rc;

	//4-й (мейлвошер)
	p_SD->wGetLetterTop(0, 0, p_SD->raw_packets->at(3), false);
	//p_SD->wGetLetter(1, 0, p_SD->raw_packets->at(2), false);
	//p_SD->wGetLetter(2, 0, p_SD->raw_packets->at(1), false);
	p_SD->wGetLetterTop(3, 0, p_SD->raw_packets->at(0), false);

	//1-й (клиент просил)
	rc.no = 2;
	rc.answer = p_SD->raw_packets->at(0);
	p_SD->cmds_stor.push_back(&rc);
	err = h_Proto->sysSendMsg
		(pmc_MAIL_WASHER, pm_SESSION_CLOSE, 0, &p_SD->washerSession, SER_SENDMSG_PSIZE);
	PR_ASSERT(PR_SUCC(err));

	WaitForSingleObject (p_SD->hReqToServerGot, INFINITE);
	p_SD->TranslateData
		(p_SD->raw_packets->at(0).c_str(), p_SD->raw_packets->at(0).c_str() + p_SD->raw_packets->at(0).size(), dsServer);

	//WaitForSingleObject (p_SD->hWasherSessionFinished, INFINITE);
	WAIT_FOR_SESSION_COMPLETION

	string sf ("SESSION FINISHED\r\n-------\r\n--------\r\n");
	_file.Write((char*)sf.c_str(), (char*)sf.c_str()+sf.size());
	_file.Flush ();
	_trace.Write((char*)sf.c_str(), (char*)sf.c_str()+sf.size());
	_trace.Flush ();


	//retr 1
	tc.no = 2;
	p_SD->cmds_stor.push_back(&tc);
	p_SD->TranslateData (top1.c_str(), top1.c_str()+top1.size(), dsClient);
	WaitForSingleObject (p_SD->hReqToServerGot, INFINITE);
	p_SD->TranslateData
		(p_SD->raw_packets->at(0).c_str(), p_SD->raw_packets->at(0).c_str() + p_SD->raw_packets->at(0).size(), dsServer);


	//retr 2
	tc.no = 2;
	p_SD->cmds_stor.push_back(&tc);
	p_SD->TranslateData (top2.c_str(), top2.c_str()+top2.size(), dsClient);
	WaitForSingleObject (p_SD->hReqToServerGot, INFINITE);
	p_SD->TranslateData
		(p_SD->raw_packets->at(1).c_str(), p_SD->raw_packets->at(1).c_str() + p_SD->raw_packets->at(1).size(), dsServer);

	//retr 3
	tc.no = 2;
	p_SD->cmds_stor.push_back(&tc);
	p_SD->TranslateData (top3.c_str(), top3.c_str()+top3.size(), dsClient);
	WaitForSingleObject (p_SD->hReqToServerGot, INFINITE);
	p_SD->TranslateData
		(p_SD->raw_packets->at(2).c_str(), p_SD->raw_packets->at(2).c_str() + p_SD->raw_packets->at(2).size(), dsServer);

	//retr 4
	tc.no = 2;
	tc.answer = p_SD->raw_packets->at(0);
	p_SD->cmds_stor.push_back(&tc);
	p_SD->TranslateData (top4.c_str(), top4.c_str()+top4.size(), dsClient);
	WaitForSingleObject (p_SD->hReqToServerGot, INFINITE);
	p_SD->TranslateData
		(p_SD->raw_packets->at(3).c_str(), p_SD->raw_packets->at(3).c_str() + p_SD->raw_packets->at(3).size(), dsServer);

	//retr 4
	tc.no = 2;
	tc.answer = p_SD->raw_packets->at(0);
	p_SD->cmds_stor.push_back(&tc);
	p_SD->TranslateData (top4.c_str(), top4.c_str()+top4.size(), dsClient);
	WaitForSingleObject (p_SD->hReqToServerGot, INFINITE);
	p_SD->TranslateData
		(p_SD->raw_packets->at(3).c_str(), p_SD->raw_packets->at(3).c_str() + p_SD->raw_packets->at(3).size(), dsServer);

	//finishing actions
	p_SD->StopSession ();
	DeleteSession (p_SD->_sessionCtx.pdwSessionCtx[TEST_PROTO]);

}


void TestManager::TestRetr_Deleted (const string& data_folder)
{
	
	{//case 1

		SessionData* p_SD = SessionData::StartSession (this, data_folder);

		string retr1 ("RETR 1\r\n");
		string retr2 ("RETR 2\r\n");
		string retr3 ("RETR 3\r\n");
		string retr4 ("RETR 4\r\n");

		DWORD err;
		RETR_cmd tc;

		p_SD->wGetLetterTop(0, 0, p_SD->raw_packets->at(0), false);
		p_SD->wGetLetterTop(1, 0, p_SD->raw_packets->at(1), false);
		p_SD->wGetLetterTop(2, 0, p_SD->raw_packets->at(2), false);
		p_SD->wGetLetterTop(3, 0, p_SD->raw_packets->at(3), false);

		p_SD->DeleteLetter (3);
		p_SD->DeleteLetter (4);

		//1-й (клиент просил)
		tc.no = 2;
		tc.answer = p_SD->raw_packets->at(0);
		p_SD->cmds_stor.push_back(&tc);
		err = h_Proto->sysSendMsg
			(pmc_MAIL_WASHER, pm_SESSION_CLOSE, 0, &p_SD->washerSession, SER_SENDMSG_PSIZE);
		PR_ASSERT(PR_SUCC(err));


		WaitForSingleObject (p_SD->hReqToServerGot, INFINITE);
		p_SD->TranslateData
			(p_SD->raw_packets->at(0).c_str(), p_SD->raw_packets->at(0).c_str() + p_SD->raw_packets->at(0).size(), dsServer);

		//WaitForSingleObject (p_SD->hWasherSessionFinished, INFINITE);
		WAIT_FOR_SESSION_COMPLETION

		p_SD->conn_stream.clear();
		p_SD->bNewPacket = true;

		//retr 1 - not deleted
		tc.no = 2;
		tc.answer = p_SD->raw_packets->at(0);
		p_SD->cmds_stor.push_back(&tc);
		p_SD->TranslateData (retr1.c_str(), retr1.c_str()+retr1.size(), dsClient);
		WaitForSingleObject (p_SD->hReqToServerGot, INFINITE);
		p_SD->TranslateData
			(p_SD->raw_packets->at(0).c_str(), p_SD->raw_packets->at(0).c_str() + p_SD->raw_packets->at(0).size(), dsServer);

		//retr 2 - not deleted
		tc.no = 2;
		tc.answer = p_SD->raw_packets->at(0);
		p_SD->cmds_stor.push_back(&tc);
		p_SD->TranslateData (retr2.c_str(), retr2.c_str()+retr2.size(), dsClient);
		WaitForSingleObject (p_SD->hReqToServerGot, INFINITE);
		p_SD->TranslateData
			(p_SD->raw_packets->at(1).c_str(), p_SD->raw_packets->at(1).c_str() + p_SD->raw_packets->at(1).size(), dsServer);

		//retr 3 - deleted
		//tc.no = 2;
		//tc.answer = p_SD->raw_packets->at(0);
		//p_SD->cmds_stor.push_back(&tc);
		p_SD->TranslateData (retr3.c_str(), retr3.c_str()+retr3.size(), dsClient);
		//WaitForSingleObject (p_SD->hReqToServerGot, INFINITE);
		//p_SD->TranslateData
		//	(p_SD->raw_packets->at(3).c_str(), p_SD->raw_packets->at(3).c_str() + p_SD->raw_packets->at(3).size(), dsServer);

		//retr 4 - deleted
		//tc.no = 2;
		//tc.answer = p_SD->raw_packets->at(0);
		//p_SD->cmds_stor.push_back(&tc);
		p_SD->TranslateData (retr4.c_str(), retr4.c_str()+retr4.size(), dsClient);
		//WaitForSingleObject (p_SD->hReqToServerGot, INFINITE);
		//p_SD->TranslateData
		//	(p_SD->raw_packets->at(3).c_str(), p_SD->raw_packets->at(3).c_str() + p_SD->raw_packets->at(3).size(), dsServer);

		_ASSERTE (p_SD->conn_stream.size() == 6);
		_ASSERTE (string(p_SD->conn_stream.at(4).begin(),p_SD->conn_stream.at(4).end()).find(DELETED_MARK) != string::npos);
		_ASSERTE (string(p_SD->conn_stream.at(5).begin(),p_SD->conn_stream.at(5).end()).find(DELETED_MARK) != string::npos);


		//finishing actions
		p_SD->StopSession ();
		DeleteSession (p_SD->_sessionCtx.pdwSessionCtx[TEST_PROTO]);
	}

	{//case 2

		SessionData* p_SD = SessionData::StartSession (this, data_folder);

		string retr1 ("RETR 1\r\n");
		string retr2 ("RETR 2\r\n");
		string retr3 ("RETR 3\r\n");
		string retr4 ("RETR 4\r\n");

		DWORD err;
		RETR_cmd tc;

		p_SD->wGetLetterTop(0, 0, p_SD->raw_packets->at(0), false);
		p_SD->wGetLetterTop(1, 0, p_SD->raw_packets->at(1), false);
		p_SD->wGetLetterTop(2, 0, p_SD->raw_packets->at(2), false);
		p_SD->wGetLetterTop(3, 0, p_SD->raw_packets->at(3), false);

		p_SD->DeleteLetter (1);
		p_SD->DeleteLetter (2);

		err = h_Proto->sysSendMsg
			(pmc_MAIL_WASHER, pm_SESSION_CLOSE, 0, &p_SD->washerSession, SER_SENDMSG_PSIZE);
		PR_ASSERT(PR_SUCC(err));

		//WaitForSingleObject (p_SD->hWasherSessionFinished, INFINITE);
		WAIT_FOR_SESSION_COMPLETION
		p_SD->conn_stream.clear();
		p_SD->bNewPacket = true;

		//retr 1 - deleted
		//tc.no = 2;
		//tc.answer = p_SD->raw_packets->at(0);
		//p_SD->cmds_stor.push_back(&tc);
		p_SD->TranslateData (retr1.c_str(), retr1.c_str()+retr1.size(), dsClient);
		//WaitForSingleObject (p_SD->hReqToServerGot, INFINITE);
		//p_SD->TranslateData
		//	(p_SD->raw_packets->at(3).c_str(), p_SD->raw_packets->at(3).c_str() + p_SD->raw_packets->at(3).size(), dsServer);

		//retr 2 - deleted
		//tc.no = 2;
		//tc.answer = p_SD->raw_packets->at(0);
		//p_SD->cmds_stor.push_back(&tc);
		p_SD->TranslateData (retr2.c_str(), retr2.c_str()+retr2.size(), dsClient);
		//WaitForSingleObject (p_SD->hReqToServerGot, INFINITE);
		//p_SD->TranslateData
		//	(p_SD->raw_packets->at(3).c_str(), p_SD->raw_packets->at(3).c_str() + p_SD->raw_packets->at(3).size(), dsServer);


		//retr 3 - not deleted
		tc.no = 2; 
		tc.answer = p_SD->raw_packets->at(0);
		p_SD->cmds_stor.push_back(&tc);
		p_SD->TranslateData (retr3.c_str(), retr3.c_str()+retr3.size(), dsClient);
		WaitForSingleObject (p_SD->hReqToServerGot, INFINITE);
		p_SD->TranslateData
			(p_SD->raw_packets->at(2).c_str(), p_SD->raw_packets->at(2).c_str() + p_SD->raw_packets->at(2).size(), dsServer);

		//retr 4 - not deleted
		tc.no = 2;
		tc.answer = p_SD->raw_packets->at(0);
		p_SD->cmds_stor.push_back(&tc);
		p_SD->TranslateData (retr4.c_str(), retr4.c_str()+retr4.size(), dsClient);
		WaitForSingleObject (p_SD->hReqToServerGot, INFINITE);
		p_SD->TranslateData
			(p_SD->raw_packets->at(3).c_str(), p_SD->raw_packets->at(3).c_str() + p_SD->raw_packets->at(3).size(), dsServer);

		_ASSERTE (p_SD->conn_stream.size() == 6);
		_ASSERTE (string(p_SD->conn_stream.at(0).begin(),p_SD->conn_stream.at(0).end()).find(DELETED_MARK) != string::npos);
		_ASSERTE (string(p_SD->conn_stream.at(1).begin(),p_SD->conn_stream.at(1).end()).find(DELETED_MARK) != string::npos);


		//finishing actions
		p_SD->StopSession ();
		DeleteSession (p_SD->_sessionCtx.pdwSessionCtx[TEST_PROTO]);
	}

}




void TestManager::TestTop_Deleted (const string& data_folder)
{
	{//case 1

		SessionData* p_SD = SessionData::StartSession (this, data_folder);

		string top1 ("TOP 1 0\r\n");
		string top2 ("TOP 2 0\r\n");
		string top3 ("TOP 3 0\r\n");
		string top4 ("TOP 4 0\r\n");

		DWORD err;
		RETR_cmd rc;
		TOP_cmd tc;

		p_SD->wGetLetterTop(0, 0, p_SD->raw_packets->at(0), false);
		p_SD->wGetLetterTop(1, 0, p_SD->raw_packets->at(1), false);
		p_SD->wGetLetterTop(2, 0, p_SD->raw_packets->at(2), false);
		p_SD->wGetLetterTop(3, 0, p_SD->raw_packets->at(3), false);

		p_SD->DeleteLetter (3);
		p_SD->DeleteLetter (4);

		//1-й (клиент просил)
		rc.no = 2;
		rc.answer = p_SD->raw_packets->at(0);
		p_SD->cmds_stor.push_back(&rc);
		err = h_Proto->sysSendMsg
			(pmc_MAIL_WASHER, pm_SESSION_CLOSE, 0, &p_SD->washerSession, SER_SENDMSG_PSIZE);
		PR_ASSERT(PR_SUCC(err));

		WaitForSingleObject (p_SD->hReqToServerGot, INFINITE);
		p_SD->TranslateData
			(p_SD->raw_packets->at(0).c_str(), p_SD->raw_packets->at(0).c_str() + p_SD->raw_packets->at(0).size(), dsServer);

		//WaitForSingleObject (p_SD->hWasherSessionFinished, INFINITE);
		WAIT_FOR_SESSION_COMPLETION
		p_SD->conn_stream.clear();
		p_SD->bNewPacket = true;

		//top 1 - not deleted
		tc.no = 2;
		tc.answer = p_SD->raw_packets->at(0);
		p_SD->cmds_stor.push_back(&tc);
		p_SD->TranslateData (top1.c_str(), top1.c_str()+top1.size(), dsClient);
		WaitForSingleObject (p_SD->hReqToServerGot, INFINITE);
		p_SD->TranslateData
			(p_SD->raw_packets->at(0).c_str(), p_SD->raw_packets->at(0).c_str() + p_SD->raw_packets->at(0).size(), dsServer);

		//top 2 - not deleted
		tc.no = 2;
		tc.answer = p_SD->raw_packets->at(0);
		p_SD->cmds_stor.push_back(&tc);
		p_SD->TranslateData (top2.c_str(), top2.c_str()+top2.size(), dsClient);
		WaitForSingleObject (p_SD->hReqToServerGot, INFINITE);
		p_SD->TranslateData
			(p_SD->raw_packets->at(1).c_str(), p_SD->raw_packets->at(1).c_str() + p_SD->raw_packets->at(1).size(), dsServer);

		//top 3 - deleted
		//tc.no = 2;
		//tc.answer = p_SD->raw_packets->at(0);
		//p_SD->cmds_stor.push_back(&tc);
		p_SD->TranslateData (top3.c_str(), top3.c_str()+top3.size(), dsClient);
		//WaitForSingleObject (p_SD->hReqToServerGot, INFINITE);
		//p_SD->TranslateData
		//	(p_SD->raw_packets->at(3).c_str(), p_SD->raw_packets->at(3).c_str() + p_SD->raw_packets->at(3).size(), dsServer);

		//top 4 - deleted
		//tc.no = 2;
		//tc.answer = p_SD->raw_packets->at(0);
		//p_SD->cmds_stor.push_back(&tc);
		p_SD->TranslateData (top4.c_str(), top4.c_str()+top4.size(), dsClient);
		//WaitForSingleObject (p_SD->hReqToServerGot, INFINITE);
		//p_SD->TranslateData
		//	(p_SD->raw_packets->at(3).c_str(), p_SD->raw_packets->at(3).c_str() + p_SD->raw_packets->at(3).size(), dsServer);

		_ASSERTE (p_SD->conn_stream.size() == 6);
		_ASSERTE (string(p_SD->conn_stream.at(4).begin(),p_SD->conn_stream.at(4).end()).find(DELETED_MARK) != string::npos);
		_ASSERTE (string(p_SD->conn_stream.at(5).begin(),p_SD->conn_stream.at(5).end()).find(DELETED_MARK) != string::npos);

		//finishing actions
		p_SD->StopSession ();
		DeleteSession (p_SD->_sessionCtx.pdwSessionCtx[TEST_PROTO]);
	}

	{//case 2

		SessionData* p_SD = SessionData::StartSession (this, data_folder);

		string top1 ("TOP 1 0\r\n");
		string top2 ("TOP 2 0\r\n");
		string top3 ("TOP 3 0\r\n");
		string top4 ("TOP 4 0\r\n");

		DWORD err;
		RETR_cmd rc;
		TOP_cmd tc;

		p_SD->wGetLetter(0, p_SD->raw_packets->at(0), false);
		p_SD->wGetLetter(1, p_SD->raw_packets->at(1), false);
		p_SD->wGetLetter(2, p_SD->raw_packets->at(2), false);
		p_SD->wGetLetter(3, p_SD->raw_packets->at(3), false);

		p_SD->DeleteLetter (3);
		p_SD->DeleteLetter (4);

		err = h_Proto->sysSendMsg
			(pmc_MAIL_WASHER, pm_SESSION_CLOSE, 0, &p_SD->washerSession, SER_SENDMSG_PSIZE);
		PR_ASSERT(PR_SUCC(err));
		//WaitForSingleObject (p_SD->hWasherSessionFinished, INFINITE);
		WAIT_FOR_SESSION_COMPLETION
		p_SD->conn_stream.clear();
		p_SD->bNewPacket = true;

		//top 1 - not deleted
		tc.no = 2;
		tc.answer = p_SD->raw_packets->at(0);
		p_SD->cmds_stor.push_back(&tc);
		p_SD->TranslateData (top1.c_str(), top1.c_str()+top1.size(), dsClient);
		WaitForSingleObject (p_SD->hReqToServerGot, INFINITE);
		p_SD->TranslateData
			(p_SD->raw_packets->at(0).c_str(), p_SD->raw_packets->at(0).c_str() + p_SD->raw_packets->at(0).size(), dsServer);

		//top 2 - not deleted
		tc.no = 2;
		tc.answer = p_SD->raw_packets->at(0);
		p_SD->cmds_stor.push_back(&tc);
		p_SD->TranslateData (top2.c_str(), top2.c_str()+top2.size(), dsClient);
		WaitForSingleObject (p_SD->hReqToServerGot, INFINITE);
		p_SD->TranslateData
			(p_SD->raw_packets->at(1).c_str(), p_SD->raw_packets->at(1).c_str() + p_SD->raw_packets->at(1).size(), dsServer);

		//top 3 - deleted
		//tc.no = 2;
		//tc.answer = p_SD->raw_packets->at(0);
		//p_SD->cmds_stor.push_back(&tc);
		p_SD->TranslateData (top3.c_str(), top3.c_str()+top3.size(), dsClient);
		//WaitForSingleObject (p_SD->hReqToServerGot, INFINITE);
		//p_SD->TranslateData
		//	(p_SD->raw_packets->at(3).c_str(), p_SD->raw_packets->at(3).c_str() + p_SD->raw_packets->at(3).size(), dsServer);

		//top 4 - deleted
		//tc.no = 2;
		//tc.answer = p_SD->raw_packets->at(0);
		//p_SD->cmds_stor.push_back(&tc);
		p_SD->TranslateData (top4.c_str(), top4.c_str()+top4.size(), dsClient);
		//WaitForSingleObject (p_SD->hReqToServerGot, INFINITE);
		//p_SD->TranslateData
		//	(p_SD->raw_packets->at(3).c_str(), p_SD->raw_packets->at(3).c_str() + p_SD->raw_packets->at(3).size(), dsServer);

		_ASSERTE (p_SD->conn_stream.size() == 6);
		_ASSERTE (string(p_SD->conn_stream.at(4).begin(),p_SD->conn_stream.at(4).end()).find(DELETED_MARK) != string::npos);
		_ASSERTE (string(p_SD->conn_stream.at(5).begin(),p_SD->conn_stream.at(5).end()).find(DELETED_MARK) != string::npos);

		//finishing actions
		p_SD->StopSession ();
		DeleteSession (p_SD->_sessionCtx.pdwSessionCtx[TEST_PROTO]);
	}

}

void TestManager::TestDele_Deleted (const string& data_folder)
{

	SessionData* p_SD = SessionData::StartSession (this, data_folder);

	string dele1 ("DELE 1\r\n");
	string dele2 ("DELE 2\r\n");
	string dele3 ("DELE 3\r\n");
	string dele4 ("DELE 4\r\n");

	string ok ("+OK\r\n");
	string er ("-ERR\r\n");

	DWORD err;
	RETR_cmd rc;
	DELE_cmd dc;

	p_SD->wGetLetterTop(0, 0, p_SD->raw_packets->at(0), false);
	p_SD->wGetLetterTop(1, 0, p_SD->raw_packets->at(1), false);
	p_SD->wGetLetterTop(2, 0, p_SD->raw_packets->at(2), false);
	p_SD->wGetLetterTop(3, 0, p_SD->raw_packets->at(3), false);

	p_SD->DeleteLetter (3);
	p_SD->DeleteLetter (4);

	//1-й (клиент просил)
	rc.no = 2;
	rc.answer = p_SD->raw_packets->at(0);
	p_SD->cmds_stor.push_back(&rc);
	err = h_Proto->sysSendMsg
	(pmc_MAIL_WASHER, pm_SESSION_CLOSE, 0, &p_SD->washerSession, SER_SENDMSG_PSIZE);
	PR_ASSERT(PR_SUCC(err));

	WaitForSingleObject (p_SD->hReqToServerGot, INFINITE);
	p_SD->TranslateData
	(p_SD->raw_packets->at(0).c_str(), p_SD->raw_packets->at(0).c_str() + p_SD->raw_packets->at(0).size(), dsServer);

	//WaitForSingleObject (p_SD->hWasherSessionFinished, INFINITE);
	WAIT_FOR_SESSION_COMPLETION
	p_SD->conn_stream.clear();
	p_SD->bNewPacket = true;

//first time
	//dele 1 - not deleted
	dc.no = 2;
	p_SD->cmds_stor.push_back(&dc);
	p_SD->TranslateData (dele1.c_str(), dele1.c_str()+dele1.size(), dsClient);
	WaitForSingleObject (p_SD->hReqToServerGot, INFINITE);
	p_SD->TranslateData (ok.c_str(), ok.c_str() + ok.size(), dsServer);

	//top 2 - not deleted
	dc.no = 2;
	p_SD->cmds_stor.push_back(&dc);
	p_SD->TranslateData (dele2.c_str(), dele2.c_str()+dele2.size(), dsClient);
	WaitForSingleObject (p_SD->hReqToServerGot, INFINITE);
	p_SD->TranslateData (ok.c_str(), ok.c_str() + ok.size(), dsServer);

	//top 3 - deleted
	p_SD->TranslateData (dele3.c_str(), dele3.c_str()+dele3.size(), dsClient);

	//top 4 - deleted
	p_SD->TranslateData (dele4.c_str(), dele4.c_str()+dele4.size(), dsClient);

//second time

	//dele 1 - not deleted
	dc.no = 2;
	p_SD->cmds_stor.push_back(&dc);
	p_SD->TranslateData (dele1.c_str(), dele1.c_str()+dele1.size(), dsClient);
	WaitForSingleObject (p_SD->hReqToServerGot, INFINITE);
	p_SD->TranslateData (er.c_str(), er.c_str() + er.size(), dsServer);

	//top 2 - not deleted
	dc.no = 2;
	p_SD->cmds_stor.push_back(&dc);
	p_SD->TranslateData (dele2.c_str(), dele2.c_str()+dele2.size(), dsClient);
	WaitForSingleObject (p_SD->hReqToServerGot, INFINITE);
	p_SD->TranslateData (er.c_str(), er.c_str() + er.size(), dsServer);

	//top 3 - deleted
	p_SD->TranslateData (dele3.c_str(), dele3.c_str()+dele3.size(), dsClient);

	//top 4 - deleted
	p_SD->TranslateData (dele4.c_str(), dele4.c_str()+dele4.size(), dsClient);

	//finishing actions
	p_SD->StopSession ();
	DeleteSession (p_SD->_sessionCtx.pdwSessionCtx[TEST_PROTO]);
	
}

void TestManager::TestPipelining_NotCached (const string& data_folder)
{
	{//nicht cached

		SessionData* p_SD = SessionData::StartSession (this, data_folder);

		string doubled_retr ("RETR 2\r\nRETR 3\r\n");

		string retr1 ("retr 1\r\n");
		string retr2 ("retr 2\r\n");
		string retr3 ("retr 3\r\n");
		string retr4 ("retr 4\r\n");

		DWORD err;
		RETR_cmd tc;

		p_SD->wGetLetterTop(0, 0, p_SD->raw_packets->at(0), false);
		p_SD->wGetLetterTop(1, 0, p_SD->raw_packets->at(1), false);
		p_SD->wGetLetterTop(2, 0, p_SD->raw_packets->at(2), false);
		p_SD->wGetLetterTop(3, 0, p_SD->raw_packets->at(3), false);

		//p_SD->DeleteLetter (3);
		p_SD->DeleteLetter (4);

		//1-й (клиент просил)
		tc.no = 2;
		tc.answer = p_SD->raw_packets->at(0);
		p_SD->cmds_stor.push_back(&tc);
		err = h_Proto->sysSendMsg
			(pmc_MAIL_WASHER, pm_SESSION_CLOSE, 0, &p_SD->washerSession, SER_SENDMSG_PSIZE);
		PR_ASSERT(PR_SUCC(err));

		WaitForSingleObject (p_SD->hReqToServerGot, INFINITE);
		p_SD->TranslateData
			(p_SD->raw_packets->at(0).c_str(), p_SD->raw_packets->at(0).c_str() + p_SD->raw_packets->at(0).size(), dsServer);

		//WaitForSingleObject (p_SD->hWasherSessionFinished, INFINITE);
		WAIT_FOR_SESSION_COMPLETION

		//p_SD->TranslateData (retr1.c_str(), retr1.c_str()+retr1.size(), dsClient);
		//p_SD->TranslateData (retr2.c_str(), retr2.c_str()+retr2.size(), dsClient);

		//retr 2
		RETR_cmd tc1;
		tc1.no = 2;
		tc1.answer = p_SD->raw_packets->at(0);
		p_SD->cmds_stor.push_back(&tc1);
		//p_SD->TranslateData (retr2.c_str(), retr2.c_str()+retr2.size(), dsClient);
		
		//retr 3
		RETR_cmd tc2;
		tc2.no = 3;
		tc2.answer = p_SD->raw_packets->at(0);
		p_SD->cmds_stor.push_back(&tc2);
		//p_SD->TranslateData (retr3.c_str(), retr3.c_str()+retr3.size(), dsClient);

		p_SD->TranslateData (doubled_retr.c_str(), doubled_retr.c_str()+doubled_retr.size(), dsClient);
		
		WaitForSingleObject (p_SD->hReqToServerGot, INFINITE);
		//WaitForSingleObject (p_SD->hReqToServerGot, INFINITE);
		
		p_SD->TranslateData
			(p_SD->raw_packets->at(2).c_str(), p_SD->raw_packets->at(2).c_str() + p_SD->raw_packets->at(2).size(), dsServer);

		p_SD->TranslateData
			(p_SD->raw_packets->at(1).c_str(), p_SD->raw_packets->at(1).c_str() + p_SD->raw_packets->at(1).size(), dsServer);


		//finishing actions
		p_SD->StopSession ();
		DeleteSession (p_SD->_sessionCtx.pdwSessionCtx[TEST_PROTO]);
	}
}

void TestManager::TestPipelining_Cached (const string& data_folder)
{
	{//both letters cached

		SessionData* p_SD = SessionData::StartSession (this, data_folder);

		string doubled_retr ("RETR 2\r\nRETR 3\r\n");

		string retr1 ("retr 1\r\n");
		string retr2 ("retr 2\r\n");
		string retr3 ("retr 3\r\n");
		string retr4 ("retr 4\r\n");

		DWORD err;
		RETR_cmd tc;

		p_SD->wGetLetterTop(0, 0, p_SD->raw_packets->at(0), false);
		p_SD->wGetLetterTop(1, 0, p_SD->raw_packets->at(1), false);
		p_SD->wGetLetterTop(2, 0, p_SD->raw_packets->at(2), false);
		p_SD->wGetLetterTop(3, 0, p_SD->raw_packets->at(3), false);

		p_SD->DeleteLetter (2);
		p_SD->DeleteLetter (3);

		//1-й (клиент просил)
		tc.no = 2;
		tc.answer = p_SD->raw_packets->at(0);
		p_SD->cmds_stor.push_back(&tc);
		err = h_Proto->sysSendMsg
			(pmc_MAIL_WASHER, pm_SESSION_CLOSE, 0, &p_SD->washerSession, SER_SENDMSG_PSIZE);
		PR_ASSERT(PR_SUCC(err));

		WaitForSingleObject (p_SD->hReqToServerGot, INFINITE);
		p_SD->TranslateData
			(p_SD->raw_packets->at(0).c_str(), p_SD->raw_packets->at(0).c_str() + p_SD->raw_packets->at(0).size(), dsServer);

		//WaitForSingleObject (p_SD->hWasherSessionFinished, INFINITE);
		WAIT_FOR_SESSION_COMPLETION
	
		p_SD->TranslateData (doubled_retr.c_str(), doubled_retr.c_str()+doubled_retr.size(), dsClient);

Sleep(INFINITE);
		//finishing actions
		p_SD->StopSession ();
		DeleteSession (p_SD->_sessionCtx.pdwSessionCtx[TEST_PROTO]);
	}
}

void TestManager::TestPipelining_Mixed (const string& data_folder)
{
	{//first cached, second not

		SessionData* p_SD = SessionData::StartSession (this, data_folder);

		string doubled_retr ("RETR 4\r\nRETR 3\r\n");

		string retr1 ("retr 1\r\n");
		string retr2 ("retr 2\r\n");
		string retr3 ("retr 3\r\n");
		string retr4 ("retr 4\r\n");

		DWORD err;
		RETR_cmd tc;

		p_SD->wGetLetterTop(0, 0, p_SD->raw_packets->at(0), false);
		p_SD->wGetLetterTop(1, 0, p_SD->raw_packets->at(1), false);
		p_SD->wGetLetterTop(2, 0, p_SD->raw_packets->at(2), false);
		p_SD->wGetLetterTop(3, 0, p_SD->raw_packets->at(3), false);

		p_SD->DeleteLetter (3);

		//1-й (клиент просил)
		tc.no = 2;
		tc.answer = p_SD->raw_packets->at(0);
		p_SD->cmds_stor.push_back(&tc);
		err = h_Proto->sysSendMsg
			(pmc_MAIL_WASHER, pm_SESSION_CLOSE, 0, &p_SD->washerSession, SER_SENDMSG_PSIZE);
		PR_ASSERT(PR_SUCC(err));

		WaitForSingleObject (p_SD->hReqToServerGot, INFINITE);
		p_SD->TranslateData
			(p_SD->raw_packets->at(0).c_str(), p_SD->raw_packets->at(0).c_str() + p_SD->raw_packets->at(0).size(), dsServer);

		//WaitForSingleObject (p_SD->hWasherSessionFinished, INFINITE);
		WAIT_FOR_SESSION_COMPLETION

		RETR_cmd tc2;
		tc2.no = 3;
		tc2.answer = p_SD->raw_packets->at(0);
		p_SD->cmds_stor.push_back(&tc2);

		p_SD->TranslateData (doubled_retr.c_str(), doubled_retr.c_str()+doubled_retr.size(), dsClient);

		WaitForSingleObject (p_SD->hReqToServerGot, INFINITE);

		p_SD->TranslateData
			(p_SD->raw_packets->at(3).c_str(), p_SD->raw_packets->at(3).c_str() + p_SD->raw_packets->at(3).size(), dsServer);

	Sleep(INFINITE);
		//finishing actions
		p_SD->StopSession ();
		DeleteSession (p_SD->_sessionCtx.pdwSessionCtx[TEST_PROTO]);
	}
}

void TestManager::TestSessionWithDeleted (const string& data_folder)
{
	SessionData* p_SD = SessionData::StartSession (this, data_folder);

	DWORD err;

	int no = 0;
	for (vector<short>::reverse_iterator it = p_SD->messagesAvailable.rbegin();
		it != p_SD->messagesAvailable.rend(); ++it)
	{
		if (*it)
		{
			//if (no % 2)
				p_SD->wGetLetterTop(no, 7, p_SD->raw_packets->at(no), false);
			//else
			//	p_SD->wGetLetter(no, p_SD->raw_packets->at(no), false);
			no++;
		}
		
	}

	//1-й
	RETR_cmd tc;
	tc.no = 1;
	tc.answer = p_SD->raw_packets->at(0);
	p_SD->cmds_stor.push_back(&tc);
	err = h_Proto->sysSendMsg
		(pmc_MAIL_WASHER, pm_SESSION_CLOSE, 0, &p_SD->washerSession, SER_SENDMSG_PSIZE);
	PR_ASSERT(PR_SUCC(err));

	WaitForSingleObject (p_SD->hReqToServerGot, INFINITE);
	p_SD->TranslateData
		(p_SD->raw_packets->at(0).c_str(), p_SD->raw_packets->at(0).c_str() + p_SD->raw_packets->at(0).size(), dsServer);


	WAIT_FOR_SESSION_COMPLETION

	//finishing actions
	p_SD->StopSession ();
	DeleteSession (p_SD->_sessionCtx.pdwSessionCtx[TEST_PROTO]);

}


void TestManager::TestRepeatRequests (const string& data_folder)
{
	SessionData* p_SD = SessionData::StartSession (this, data_folder);

	DWORD err;
	//p_SD->wGetLetterTop(0, 7, p_SD->raw_packets->at(0), false);
	//p_SD->wGetLetterTop(1, 7, p_SD->raw_packets->at(1), false);
	//p_SD->wGetLetterTop(2, 7, p_SD->raw_packets->at(2), false);
	//p_SD->wGetLetterTop(3, 7, p_SD->raw_packets->at(2), false);

	//p_SD->wGetLetter(0, p_SD->raw_packets->at(0), false);
	//p_SD->wGetLetter(1, p_SD->raw_packets->at(1), false);
	//p_SD->wGetLetter(2, p_SD->raw_packets->at(2), false);
	//p_SD->wGetLetter(3, p_SD->raw_packets->at(2), false);

	//тут надо мен€ть параметры запроса (““ 30591)
	p_SD->wGetLetterTop(0, 7, p_SD->raw_packets->at(1), false);
	p_SD->wGetLetter(0, p_SD->raw_packets->at(1), false);

	p_SD->wGetLetter(1, p_SD->raw_packets->at(1), false);
	p_SD->wGetLetterTop(1, 7, p_SD->raw_packets->at(1), false);
		
	//1-й (клиент просил)
	RETR_cmd tc;
	tc.no = 2;
	tc.answer = p_SD->raw_packets->at(0);
	p_SD->cmds_stor.push_back(&tc);
	err = h_Proto->sysSendMsg
		(pmc_MAIL_WASHER, pm_SESSION_CLOSE, 0, &p_SD->washerSession, SER_SENDMSG_PSIZE);
	PR_ASSERT(PR_SUCC(err));

	WaitForSingleObject (p_SD->hReqToServerGot, INFINITE);
	p_SD->TranslateData
		(p_SD->raw_packets->at(0).c_str(), p_SD->raw_packets->at(0).c_str() + p_SD->raw_packets->at(0).size(), dsServer);

	//WaitForSingleObject (p_SD->hWasherSessionFinished, INFINITE);
	WAIT_FOR_SESSION_COMPLETION

	//finishing actions
	p_SD->StopSession ();
	DeleteSession (p_SD->_sessionCtx.pdwSessionCtx[TEST_PROTO]);

}

void TestManager::TestCancelRequest (const string& data_folder)
{
	SessionData* p_SD = SessionData::StartSession (this, data_folder);

	DWORD err;
	bool arr[4] = {false, false, false, false};

	p_SD->CancelOnTransfer (0, 4, 7, *p_SD->raw_packets, arr, 3);

	//1-й (клиент просил)
	RETR_cmd tc;
	tc.no = 2;
	tc.answer = p_SD->raw_packets->at(0);
	p_SD->cmds_stor.push_back(&tc);
	err = h_Proto->sysSendMsg
		(pmc_MAIL_WASHER, pm_SESSION_CLOSE, 0, &p_SD->washerSession, SER_SENDMSG_PSIZE);
	PR_ASSERT(PR_SUCC(err));

	WaitForSingleObject (p_SD->hReqToServerGot, INFINITE);
	p_SD->TranslateData
		(p_SD->raw_packets->at(0).c_str(), p_SD->raw_packets->at(0).c_str() + p_SD->raw_packets->at(0).size(), dsServer);

	//WaitForSingleObject (p_SD->hWasherSessionFinished, INFINITE);
	WAIT_FOR_SESSION_COMPLETION

	//finishing actions
	p_SD->StopSession ();
	DeleteSession (p_SD->_sessionCtx.pdwSessionCtx[TEST_PROTO]);

}

void TestManager::TestCancelDelete (const string& data_folder)
{
	SessionData* p_SD = SessionData::StartSession (this, data_folder);

	bool arr[4] = {false, false, false, false};
	p_SD->wGetLetterTops (0, 4, 5, *p_SD->raw_packets, arr);

	DWORD err;
	int arr2[4] = {4, 3, 2, 1};

	p_SD->CancelOnDelete (arr2, 4, 2);
	p_SD->DeleteLetter (2);

	//1-й (клиент просил)
	RETR_cmd tc;
	tc.no = 2;
	tc.answer = p_SD->raw_packets->at(0);
	p_SD->cmds_stor.push_back(&tc);
	err = h_Proto->sysSendMsg
		(pmc_MAIL_WASHER, pm_SESSION_CLOSE, 0, &p_SD->washerSession, SER_SENDMSG_PSIZE);
	PR_ASSERT(PR_SUCC(err));

	WaitForSingleObject (p_SD->hReqToServerGot, INFINITE);
	p_SD->TranslateData
		(p_SD->raw_packets->at(0).c_str(), p_SD->raw_packets->at(0).c_str() + p_SD->raw_packets->at(0).size(), dsServer);

	//WaitForSingleObject (p_SD->hWasherSessionFinished, INFINITE);
	WAIT_FOR_SESSION_COMPLETION

	//finishing actions
	p_SD->StopSession ();
	DeleteSession (p_SD->_sessionCtx.pdwSessionCtx[TEST_PROTO]);

}



void TestManager::TestErrRequest (const string& data_folder)
{//похоже на ошибку, т.к. приходит сообщение с €кобы полученным письмом
	SessionData* p_SD = SessionData::StartSession (this, data_folder);

	DWORD err;
	bool arr[4] = {false, false, false, false};

	p_SD->ErrorOnTransfer (0, 4, 7, *p_SD->raw_packets, arr, 3);

	RETR_cmd tc;
	tc.no = 1;
	tc.answer = p_SD->raw_packets->at(0);
	p_SD->cmds_stor.push_back(&tc);

	err = h_Proto->sysSendMsg
		(pmc_MAIL_WASHER, pm_SESSION_CLOSE, 0, &p_SD->washerSession, SER_SENDMSG_PSIZE);
	PR_ASSERT(PR_SUCC(err));

	WaitForSingleObject (p_SD->hReqToServerGot, INFINITE);
	p_SD->TranslateData
		(p_SD->raw_packets->at(1).c_str(), p_SD->raw_packets->at(1).c_str() + p_SD->raw_packets->at(1).size(), dsServer);

	//WaitForSingleObject (p_SD->hWasherSessionFinished, INFINITE);
	WAIT_FOR_SESSION_COMPLETION

	//finishing actions
	p_SD->StopSession ();
	DeleteSession (p_SD->_sessionCtx.pdwSessionCtx[TEST_PROTO]);

}

void TestManager::TestErrDelete (const string& data_folder)
{

	SessionData* p_SD = SessionData::StartSession (this, data_folder);

	DWORD err;
	RETR_cmd tc;

	bool arr[4] = {false, false, false, false};
	p_SD->wGetLetterTops (0, 4, 5, *p_SD->raw_packets, arr);

	p_SD->DeleteLetterErr (3);
	p_SD->DeleteLetter (3);

	//1-й (клиент просил)
	tc.no = 2;
	tc.answer = p_SD->raw_packets->at(0);
	p_SD->cmds_stor.push_back(&tc);
	err = h_Proto->sysSendMsg
		(pmc_MAIL_WASHER, pm_SESSION_CLOSE, 0, &p_SD->washerSession, SER_SENDMSG_PSIZE);
	PR_ASSERT(PR_SUCC(err));

	WaitForSingleObject (p_SD->hReqToServerGot, INFINITE);
	p_SD->TranslateData
		(p_SD->raw_packets->at(0).c_str(), p_SD->raw_packets->at(0).c_str() + p_SD->raw_packets->at(0).size(), dsServer);

	//WaitForSingleObject (p_SD->hWasherSessionFinished, INFINITE);
	WAIT_FOR_SESSION_COMPLETION

	//finishing actions
	p_SD->StopSession ();
	DeleteSession (p_SD->_sessionCtx.pdwSessionCtx[TEST_PROTO]);

}
