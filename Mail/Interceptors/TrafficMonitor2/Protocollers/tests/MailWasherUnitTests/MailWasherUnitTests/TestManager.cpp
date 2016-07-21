#define PR_IMPEX_DEF
#define NOMINMAX
#include "TestManager.h"

#include <plugin/p_tm.h>
#include <plugin/p_trafficmonitor.h>
#include <plugin/p_mailwasher.h>

#include "../TaskManager/i_fake_taskmanager.h"

#include "config.h"

#include "Tracer.h"
#include <algorithm>

TestManager::TestManager():
hProtoStateChanged (CreateEvent (NULL, false, false, NULL)),
hProtoStateChangeExpected (CreateEvent (NULL, false, false, NULL)),
hMailProcessExpected (CreateEvent (NULL, false, false, NULL)),
hMailProcessReceived (CreateEvent (NULL, false, false, NULL)),
bTraceMailProcess (false),
_file ("h:\\from_washer.txt"),
_trace ("h:\\washer.txt"),
h_Proto (NULL),
h_TaskManager (NULL),
h_TrafficMonitor (NULL)

{

}


void TestManager::LoadPrague()
{
	static char const* plugins[] = {"nfio.ppl", "prutil.ppl", "tempfile.ppl", "msoe.ppl",
		"thpimpl.ppl", "pop3prtc.ppl", "maildisp.ppl", "FakeTaskManager.ppl", "FakeTrafficMonitor.ppl", 0};

	prague = boost::shared_ptr<prague::init_prague>(new prague::init_prague (plugins));
	prague::pr_remote_api().GetRoot(&g_root);
}

void TestManager::GetObjects (hTASK& m_Protocoller, hTASKMANAGER& h_TaskManager, hMAILTASK& h_TrafficMonitor)
{
	tERROR error;

	if (m_Protocoller) m_Protocoller->sysCloseObject();
	if (h_TaskManager) h_TaskManager->sysCloseObject();
	if (h_TrafficMonitor) h_TrafficMonitor->sysCloseObject();

	error = g_root->sysCreateObject(reinterpret_cast<hOBJECT*>(&h_TaskManager), IID_TASKMANAGER, PID_TM);
	error = g_root->sysCreateObject(reinterpret_cast<hOBJECT*>(&h_TrafficMonitor), IID_MAILTASK, PID_TRAFFICMONITOR);

	error = h_TaskManager->propSetObj( pgTM_TRAFFICMONITOR_PTR, (hOBJECT)h_TrafficMonitor);
	error = h_TaskManager->propSetPtr( pgTEST_MANAGER_PTR, (hOBJECT)this);
	error = h_TrafficMonitor->propSetObj( pgTASK_TM, (hOBJECT)h_TaskManager);
	error = h_TrafficMonitor->propSetPtr( pgTEST_MANAGER_PTR, (hOBJECT)this);

	error = h_TaskManager->sysCreateObjectDone();
	error = h_TrafficMonitor->sysCreateObjectDone();

	error = h_TrafficMonitor->sysCreateObject((hOBJECT*)&m_Protocoller, IID_TASK, PID_MAILWASHER);
	error = m_Protocoller->propSetObj( pgTASK_TM, (hOBJECT)h_TaskManager);
	error = m_Protocoller->sysCreateObjectDone();
}


bool SessionData::TranslateDataToItself(const char* begin, const char* end, TrafficProtocoller::data_source_t source)
{
	_process_msg.assign(_sessionCtx, false);
	_process_msg.dsDataTarget = aux::SourceToTarget(source);
	_process_msg.psStatus = psKeepAlive;
	_process_msg.hData = aux::create_io_from_array (begin, end - begin);
	tERROR error = m_testManager->h_TrafficMonitor->sysSendMsg 
		(pmcTRAFFIC_PROTOCOLLER,
		pmKEEP_ALIVE, 
		0, 
		&_process_msg, 
		SER_SENDMSG_PSIZE 
		);
	_process_msg.hData->sysCloseObject ();
	_ASSERTE (error >= 0);
	return true;
}

bool SessionData::TranslateData (const char* begin, const char* end, TrafficProtocoller::data_source_t source)
{
	const char* slide_begin = begin;

	do
		{
			size_t chunk_size (aux::rnd_chunk_size(Configure::GetConfiguration().MaximumChunkSize()));
			chunk_size = std::min (chunk_size, (size_t)(end-slide_begin));

			//отправляем буфер протоколёру
			process_status_t psStatus;
			_processParams.assign(_sessionCtx, false);
			_processParams.dsDataSource = source;
			_processParams.pData = (tVOID*)slide_begin;
			_processParams.ulDataSize = chunk_size;
			_processParams.phDetectorHandle = (hOBJECT*)&m_testManager->h_Proto;
			_processParams.psStatus = &psStatus;
			tERROR error = m_testManager->h_TaskManager->sysSendMsg
				(pmcTRAFFIC_PROTOCOLLER,
				pmPROCESS,
				0,
				&_processParams,
				SER_SENDMSG_PSIZE);
			LOG_CALL5("pmPROCESS ", aux::noRN(string(slide_begin,slide_begin + std::min(10, end-slide_begin))), aux::prResultToString(error), aux::processStatusToString(psStatus),"\n");
			m_testManager->_trace.Write (slide_begin, slide_begin + chunk_size);
			m_testManager->_trace.Flush();
			_ASSERTE (error == errOK_DECIDED && psStatus == psKeepAlive);
			
			slide_begin += chunk_size;
			Sleep (Configure::GetConfiguration().TimeoutBetweenChunks());
		}
	while (slide_begin != end);

	return true;
}

void SessionData::SendBytesToProcess (vector<string>* packets, vector<aux::output_packet>* out_packets,
									  bool split_packets)
{
	//чтобы порции трафика не ограничивались рамками одного пакета, но и можно было сделать закрытие соединения	
	string prev_packet_chunk;
	vector<string>::const_iterator it;
	vector<aux::output_packet>::iterator it2;
	for (it = packets->begin(), it2 = out_packets->begin(); it != packets->end(); ++it, ++it2)
	{
		Sleep (Configure::GetConfiguration().TimeoutBetweenChunks());

		string pckt = *it;
		if (prev_packet_chunk.size()) 
		{
			pckt = prev_packet_chunk;
			string pckt2 (*it);
			pckt.append (pckt2.c_str());
			prev_packet_chunk.clear();
		}

		const char* begin = pckt.c_str();
		const char* end = begin + pckt.size();
		char* start = const_cast<char*>(begin);
		while (start != end)
		{
			size_t chunk_size (aux::rnd_chunk_size(Configure::GetConfiguration().MaximumChunkSize()));
			if (chunk_size >= static_cast<size_t>(end - start))
			{
				if (split_packets ||
					it2->dsDataTarget != ((it+1 != packets->end())?(it2+1)->dsDataTarget : it2->dsDataTarget))
				{
					chunk_size = static_cast<size_t>(end - start);
					_ASSERTE (TranslateData(start, start + chunk_size, it2->dsDataTarget));
					start += chunk_size;
				}
				else
				{	
					prev_packet_chunk.assign (start, end);
					break;
				}
			}
			else
			{
				_ASSERTE(TranslateData(start, start + chunk_size, it2->dsDataTarget));
				start += chunk_size;
			}
			Sleep (Configure::GetConfiguration().TimeoutBetweenChunks());
		}

	}
	if (prev_packet_chunk.size())
	{
		_ASSERTE(TranslateData (prev_packet_chunk.c_str(), prev_packet_chunk.c_str() + prev_packet_chunk.size(),
			out_packets->rbegin()->dsDataTarget));
	}

}


SessionData* SessionData::StartSession (TestManager* m_tm, const std::string& data_folder)
{
	SessionData* p_SD = new SessionData (m_tm);
	tERROR error;

	LOG_CALL3("Test folder ",data_folder,"\n");

	m_tm->GetObjects(m_tm->h_Proto, m_tm->h_TaskManager, m_tm->h_TrafficMonitor);

	error = m_tm->h_TrafficMonitor->sysRegisterMsgHandler(pmc_TASK_STATE_CHANGED, rmhLISTENER,
		m_tm->h_TrafficMonitor, IID_ANY, PID_ANY, IID_ANY, PID_ANY );

	SetEvent (m_tm->hProtoStateChangeExpected);
	m_tm->h_Proto->SetState (TASK_REQUEST_RUN);
	
	WaitForSingleObject (m_tm->hProtoStateChanged, INFINITE);
	_ASSERTE (m_tm->m_ProtoState = TASK_STATE_RUNNING);

//
	memset(p_SD->dwSessionCtx, 0, po_last*sizeof(tDWORD));
	p_SD->dwSessionCtx[po_ProxySession] = (tPTR)0xBBBBBBBB;
	p_SD->_sessionCtx.pdwSessionCtx = p_SD->dwSessionCtx;
	p_SD->_sessionCtx.dwSessionCtxCount = po_last;

	error = m_tm->h_Proto->sysRegisterMsgHandler(pmcTRAFFIC_PROTOCOLLER, rmhDECIDER, m_tm->h_TrafficMonitor, IID_ANY, PID_ANY, IID_ANY, PID_ANY );
	PR_ASSERT(PR_SUCC(error));
	error = m_tm->h_Proto->sysRegisterMsgHandler(pmc_MAIL_WASHER, rmhDECIDER, m_tm->h_TrafficMonitor, IID_ANY, PID_ANY, IID_ANY, PID_ANY );
	PR_ASSERT(PR_SUCC(error));
	
	
	//пинаем Вошер, что мы как будто задетектили протокол из pop3_protocoller-а
	p_SD->_detectParams.assign(p_SD->_sessionCtx, false);
	p_SD->_detectParams.pdcDetectCode = &p_SD->dcDetectCode;
	p_SD->_detectParams.phDetectorHandle = (hOBJECT*)&m_tm->h_Proto;
	p_SD->_detectParams.dwSessionCtxCount = po_last;
	p_SD->_detectParams.dwInterfaceVersion = TRAFFIC_PROTOCOLLER_INTERFACE_VERSION;

	error = m_tm->h_TrafficMonitor->sysSendMsg (pmc_MAIL_WASHER, pmDETECT, 0,&p_SD->_detectParams, SER_SENDMSG_PSIZE);
	PR_ASSERT(PR_SUCC(error));
	Sleep (1000);

	//error = m_tm->h_Proto->sysUnregisterMsgHandler(pmc_MAIL_WASHER, m_tm->h_TrafficMonitor);
	//PR_ASSERT(PR_SUCC(error));

	p_SD->_sessionCtx = p_SD->_sessionCtx;
	m_tm->AddSession (p_SD);
//

	p_SD->raw_packets = aux::get_raw_packets (data_folder.c_str());
	p_SD->out_packets = aux::get_output_packets ((hOBJECT)g_root, data_folder.c_str());
	p_SD->control_files = aux::get_control_files ((hOBJECT)g_root, data_folder.c_str());

	CONFIGURE(10000,1)	

	p_SD->process_msg.assign(p_SD->_sessionCtx, false);
	p_SD->process_msg.phDetectorHandle = (hOBJECT*)&m_tm->h_Proto;
	p_SD->process_msg.psStatus = &p_SD->p_s_t;
	p_SD->process_msg.dwInterfaceVersion = TRAFFIC_PROTOCOLLER_INTERFACE_VERSION;
	p_SD->process_msg.dwSessionCtxCount = po_MailWasher;
	p_SD->process_msg.dsDataSource = dsClient;
	p_SD->process_msg.pData = "RETR 1\x0d\x0a";
	p_SD->process_msg.ulDataSize = 8;

	//ждем LIST
	{
		string lst_path (data_folder);
		lst_path.append ("\\list.lst");
		FILE* f = fopen (lst_path.c_str(), "rb");
		_ASSERTE (f);
		char buf[1024];
		LIST_cmd lcmd;
		int b_read;
		do 
		{
			b_read = fread (buf, 1, 1024, f);
			if (b_read) lcmd.to_send.append (buf, b_read);
		} while (b_read);
		fclose (f);

		size_t pos = 4;
		size_t pos2 = lcmd.to_send.find (" ", 4);

		lcmd.no = atoi (lcmd.to_send.substr (4, pos2 - 4).c_str());
		//p_SD->messagesReceived.resize (lcmd.no, cPOP3Message());
		//p_SD->messagesAvailable.resize (lcmd.no, false);

		pos = lcmd.to_send.find ("\r\n", pos);
		while (pos != string::npos)
		{
			pos += 2;
			size_t pos2 = lcmd.to_send.find (" ", pos);

			int order_no = atoi (lcmd.to_send.substr (pos, pos2 - pos).c_str());
			if (order_no)
			{
				p_SD->messagesReceived.resize (order_no, cPOP3Message());
				p_SD->messagesAvailable.resize (order_no, 0);
				p_SD->messagesAvailable.at(order_no - 1) = order_no;
			}
			
			pos = lcmd.to_send.find ("\r\n", pos);
		}

		p_SD->cmds_stor.push_back (&lcmd);

		DWORD err = m_tm->h_Proto->sysSendMsg( 
						pmcTRAFFIC_PROTOCOLLER, 
						pmPROCESS, 
						0, 
						&p_SD->process_msg, 
						SER_SENDMSG_PSIZE 
						);

		p_SD->dcDetectCode = dcDataDetected;


		WaitForSingleObject (p_SD->hWasherSessionStarted, INFINITE);
		_ASSERTE (p_SD->washerSession.m_dwMessageCount = lcmd.no);
	}
	
/*	{
		LIST_cmd lcmd;
		lcmd.no = p_SD->raw_packets->size();
		for (vector<string>::iterator it = p_SD->raw_packets->begin(); it != p_SD->raw_packets->end(); ++it)
			lcmd.sizes.push_back (it->length());
		p_SD->cmds_stor.push_back (&lcmd);
		WaitForSingleObject (p_SD->hWasherSessionStarted, INFINITE);
		_ASSERTE (p_SD->washerSession.m_dwMessageCount = lcmd.no);
	}*/

	return p_SD;
}

void SessionData::StopSession()
{

//	VerifyCheckedObjects (control_files.get());
//	VerifyConnStream (out_packets.get());

	tERROR error;

	//error = m_testManager->h_Proto->sysRegisterMsgHandler(pmcTRAFFIC_PROTOCOLLER, rmhDECIDER, m_testManager->h_TrafficMonitor, IID_ANY, PID_ANY, IID_ANY, PID_ANY );
	//PR_ASSERT(PR_SUCC(error));
	error = m_testManager->h_TrafficMonitor->sysSendMsg (pmcTRAFFIC_PROTOCOLLER, pmSESSION_STOP, 0,	&_sessionCtx, SER_SENDMSG_PSIZE);
	PR_ASSERT(PR_SUCC(error));
	//error = m_testManager->h_Proto->sysUnregisterMsgHandler(pmcTRAFFIC_PROTOCOLLER, m_testManager->h_TrafficMonitor);
	//PR_ASSERT(PR_SUCC(error));

	SetEvent (m_testManager->hProtoStateChangeExpected);

	m_testManager->h_Proto->SetState (TASK_REQUEST_STOP);

	
	WaitForSingleObject (m_testManager->hProtoStateChanged, INFINITE);
	_ASSERTE (m_testManager->m_ProtoState = TASK_STATE_STOPPED);

	m_testManager->gotten_objects.clear();
}

bool SessionData::VerifyConnStream (const vector<aux::output_packet>* e_packets)
{
	LOG_CALL("Verifying http stream ...\n");

	bool bResult = true;

	vector<vector<byte> >::iterator it = conn_stream.begin();
	vector<aux::output_packet>::const_iterator it2 = e_packets->begin();

	while (it != conn_stream.end() && it2 != e_packets->end())
	{
		vector<byte> e_stream;
		it2->GetData (e_stream);

		if (*it != e_stream)
		{
			char buf[32], buf2[32];
			LOG_CALL5("\tIncorrect http-packet",itoa(e_stream.size(), buf, 10),"-",itoa(it->size(), buf, 10),"\n");

			LOG_CALL("<<FIRST_STREAM>>:");
			{
				sync::local_mutex::scoped_lock lock(Tracer::GetTracer().GetMutex());
				copy (e_stream.begin(), e_stream.end (), ostream_iterator<byte> (ostream_iterator<byte> (cout)));
			}
			LOG_CALL("\n");

			LOG_CALL("<<SECOND_STREAM>>:");
			{
				sync::local_mutex::scoped_lock lock(Tracer::GetTracer().GetMutex());
				copy (it->begin(), it->end (), ostream_iterator<byte> (ostream_iterator<byte> (cout)));
			}
			LOG_CALL("\n");

			bResult = false;
		}
		else
		{
			char buf[32], buf2[32];
			LOG_CALL5("\tCorrect http-packet",itoa(e_stream.size(), buf, 10),"-",itoa(it->size(), buf2, 10),"\n");
			/*
			LOG_CALL("<<FIRST_STREAM>>:");
			{
			sync::local_mutex::scoped_lock lock(Tracer::GetTracer().GetMutex());
			copy (e_stream.begin(), e_stream.end (), ostream_iterator<byte> (cout));
			}
			LOG_CALL("\n");

			LOG_CALL5("<<SECOND_STREAM>>:");
			{
			sync::local_mutex::scoped_lock lock(Tracer::GetTracer().GetMutex());
			copy (it->begin(), it->end (), ostream_iterator<byte> (cout));
			}
			LOG_CALL("\n");
			*/

		};

		++it;
		++it2;
	}

	if ( it != conn_stream.end() || it2 != e_packets->end() )
	{
		LOG_CALL("\tIncorrect http-packets vector size\n");
		bResult = false;
	}

	return bResult;
}


bool SessionData::VerifyCheckedObjects (vector<hIO>* etalon)
{
	LOG_CALL("Verifying checked objects ...\n");

	size_t min_count = std::min (etalon->size(), m_testManager->gotten_objects.size());
	bool bResult = true;
	for (int i = 0; i < min_count; ++i)
	{
		char buf[32];
		LOG_CALL3("Verifying object ",itoa(i+1, buf, 10),"\n");
		if (!aux::isIoEquals (etalon->at(i), m_testManager->gotten_objects.at(i))) bResult = false;
	}

	if (etalon->size() != m_testManager->gotten_objects.size())
	{
		char buf[32], buf2[32];
		LOG_CALL5("\tIncorrect number of objects...",itoa(etalon->size(), buf, 10),"-",itoa(m_testManager->gotten_objects.size(), buf, 10),"\n");
		return false;
	}

	return bResult;
}

void SessionData::SendNoopReply()
{
	static string ok ("+OK\r\n");
	TranslateData (ok.c_str(), ok.c_str() + ok.size(), dsServer);
}


void SessionData::wGetLetterTops (int first, int count, int lines, vector<string> letters, bool* cached)
{
	cPOP3MessageReguest mr;
	mr.assign (washerSession, false);
	mr.m_dwBegin = first;
	mr.m_dwCount = count;
	mr.m_dwSizeOf = lines;

	vector<TOP_cmd*> topcmds;

	vector<string>::iterator it = letters.begin();
	bool* cur_cached = cached;
	for (int i = first+count; i > first; --i)
	{

		if (cur_cached && (!*cur_cached))
		{

			TOP_cmd* p_tc = new TOP_cmd();

			p_tc->no = i;
			p_tc->lines = lines;
			p_tc->answer = *it;

			cmds_stor.push_back(p_tc);
			topcmds.push_back(p_tc);
			
		}
		if (cur_cached) cur_cached++;
		++it;
	}

	tDWORD err = m_testManager->h_TrafficMonitor->sysSendMsg
		(pmc_MAIL_WASHER, pm_MESSAGE_GETLIST, 0,&mr, SER_SENDMSG_PSIZE);
	PR_ASSERT(PR_SUCC(err));

	it = letters.begin();
	cur_cached = cached;
	for (int i = first+count; i > first; --i)
	{
		if (cur_cached && (!*cur_cached))
		{
			WaitForSingleObject (hReqToServerGot, INFINITE);
			TranslateData(it->c_str(), it->c_str() + it->size(), dsServer);
		}
		

		SetEvent (hMessageToReceive);
		WaitForSingleObject (hMessageReceived, INFINITE);

		++it;
		if (cur_cached) cur_cached++;
	}

	SetEvent (hMessageToReceive);
	WaitForSingleObject (hMessageReceived, INFINITE);
	
	for (int i = 0; i < topcmds.size(); ++i) delete topcmds[i];
}

void SessionData::wGetLetterTop (int no, int lines, string letter, bool cached)
{
	cPOP3MessageReguest mr;
	mr.assign (washerSession, false);
	mr.m_dwBegin = no;
	mr.m_dwCount = 1;
	mr.m_dwSizeOf = lines;

	TOP_cmd tc;

	if (!cached)
	{
		vector<short>::reverse_iterator it = messagesAvailable.rbegin();
		for (int i = 0; i < no; ++i)
		{
			do
			{
				++it;
			}
			while (!*it);
		}
		tc.no = *it;
		tc.lines = lines;
		tc.answer = letter;

		cmds_stor.push_back(&tc);
	}

	tDWORD err = m_testManager->h_TrafficMonitor->sysSendMsg
		(pmc_MAIL_WASHER, pm_MESSAGE_GETLIST, 0,&mr, SER_SENDMSG_PSIZE);
	PR_ASSERT(PR_SUCC(err));

	if (!cached)
	{
		WaitForSingleObject (hReqToServerGot, INFINITE);
		TranslateData (letter.c_str(), letter.c_str() + letter.size(), dsServer);
	}

	SetEvent (hMessageToReceive);
	WaitForSingleObject (hMessageReceived, INFINITE);
	
	SetEvent (hMessageToReceive);
	WaitForSingleObject (hMessageReceived, INFINITE);

}

void SessionData::wGetLetters (int first, int count, vector<string> letters, bool* cached)
{
	cPOP3MessageReguest mr;
	mr.assign (washerSession, false);
	mr.m_dwBegin = first;
	mr.m_dwCount = count;
	mr.m_dwSizeOf = -1;

	vector<RETR_cmd*> topcmds;

	vector<string>::iterator it = letters.begin();
	bool* cur_cached = cached;
	for (int i = first+count; i > first; --i)
	{

		if (cur_cached && (!*cur_cached))
		{

			RETR_cmd* p_tc = new RETR_cmd();

			p_tc->no = i;
			p_tc->answer = *it;

			cmds_stor.push_back(p_tc);
			topcmds.push_back(p_tc);

		}
		if (cur_cached) cur_cached++;
		++it;
	}

	tDWORD err = m_testManager->h_TrafficMonitor->sysSendMsg
		(pmc_MAIL_WASHER, pm_MESSAGE_GETLIST, 0,&mr, SER_SENDMSG_PSIZE);
	PR_ASSERT(PR_SUCC(err));

	it = letters.begin();
	cur_cached = cached;
	for (int i = first+count; i > first; --i)
	{
		if (cur_cached && (!*cur_cached))
		{
			WaitForSingleObject (hReqToServerGot, INFINITE);
			TranslateData(it->c_str(), it->c_str() + it->size(), dsServer);
		}


		SetEvent (hMessageToReceive);
		WaitForSingleObject (hMessageReceived, INFINITE);

		++it;
		if (cur_cached) cur_cached++;
	}

	SetEvent (hMessageToReceive);
	WaitForSingleObject (hMessageReceived, INFINITE);

	for (int i = 0; i < topcmds.size(); ++i) delete topcmds[i];
}

void SessionData::wGetLetter (int no, string letter, bool cached)
{
	cPOP3MessageReguest mr;
	mr.assign (washerSession, false);
	mr.m_dwBegin = no;
	mr.m_dwCount = 1;
	mr.m_dwSizeOf = -1;

	RETR_cmd tc;

	if (!cached)
	{
		vector<short>::reverse_iterator it = messagesAvailable.rbegin();
		for (int i = 0; i < no; ++i)
		{
			do
			{
				++it;
			}
			while (!*it);
		}
		tc.no = *it;
		tc.answer = letter;

		cmds_stor.push_back(&tc);
	}

	tDWORD err = m_testManager->h_TrafficMonitor->sysSendMsg
		(pmc_MAIL_WASHER, pm_MESSAGE_GETLIST, 0,&mr, SER_SENDMSG_PSIZE);
	PR_ASSERT(PR_SUCC(err));

	if (!cached)
	{
		WaitForSingleObject (hReqToServerGot, INFINITE);
		TranslateData (letter.c_str(), letter.c_str() + letter.size(), dsServer);
	}

	SetEvent (hMessageToReceive);
	WaitForSingleObject (hMessageReceived, INFINITE);

	SetEvent (hMessageToReceive);
	WaitForSingleObject (hMessageReceived, INFINITE);

}

void SessionData::DeleteLetter (int no)
{
	//теперь сразу не удаляется
	//DELE_cmd dc;
	//dc.no = no;
	//cmds_stor.push_back(&dc);

	string ok ("+OK\r\n");

	cPOP3MessageList ml;
	ml.assign (washerSession, false);
	ml.m_dwPOP3Session = washerSession.m_dwPOP3Session;

	/*cPOP3Message ms;
	ms.assign (washerSession, false);
	ms.m_dwNumber = no;
	ms.m_bMarkToDelete = true;*/
	messagesReceived.at(no-1).m_bMarkToDelete = true;

	ml.m_POP3MessageList.push_back(messagesReceived.at(no-1));

	tDWORD err = m_testManager->h_TrafficMonitor->sysSendMsg
		(pmc_MAIL_WASHER, pm_MESSAGE_REMOVE, 0,&ml, SER_SENDMSG_PSIZE);
	PR_ASSERT(PR_SUCC(err));

	//теперь сразу не удаляется
	//WaitForSingleObject (hReqToServerGot, INFINITE);
	//TranslateData (ok.c_str(), ok.c_str() + ok.size(), dsServer);

	SetEvent (hMessageToDelete);
	WaitForSingleObject (hMessageDeleted, INFINITE);

	SetEvent (hMessageToDelete);
	WaitForSingleObject (hMessageDeleted, INFINITE);
}

void SessionData::DeleteLetterErr (int no)
{
	DELE_cmd dc;
	dc.no = no;
	cmds_stor.push_back(&dc);

	string errs ("-ERR fehler\r\n");

	cPOP3MessageList ml;
	ml.assign (washerSession, false);
	ml.m_dwPOP3Session = washerSession.m_dwPOP3Session;

	messagesReceived.at(no-1).m_bMarkToDelete = true;

	ml.m_POP3MessageList.push_back(messagesReceived.at(no-1));

	tDWORD err = m_testManager->h_TrafficMonitor->sysSendMsg
		(pmc_MAIL_WASHER, pm_MESSAGE_REMOVE, 0,&ml, SER_SENDMSG_PSIZE);
	PR_ASSERT(PR_SUCC(err));

	WaitForSingleObject (hReqToServerGot, INFINITE);
	TranslateData (errs.c_str(), errs.c_str() + errs.size(), dsServer);

	//не присылается
	//SetEvent (hMessageToDelete);
	//WaitForSingleObject (hMessageDeleted, INFINITE);

	SetEvent (hMessageToDelete);
	WaitForSingleObject (hMessageDeleted, INFINITE);
}



void SessionData::CancelOnTransfer (int first, int count, int lines, vector<string> letters, bool* cached, int int_no)
{
	cPOP3MessageReguest mr;
	mr.assign (washerSession, false);
	mr.m_dwBegin = first;
	mr.m_dwCount = count;
	mr.m_dwSizeOf = lines;

	vector<TOP_cmd*> topcmds;

	vector<string>::iterator it = letters.begin();
	bool* cur_cached = cached;
	int int_no_curr = 0;
	for (int i = first+count; i > first; --i)
	{
		if (int_no_curr == int_no) break;
		if (cur_cached && (!*cur_cached))
		{

			TOP_cmd* p_tc = new TOP_cmd();

			p_tc->no = i;
			p_tc->lines = lines;
			p_tc->answer = *it;

			cmds_stor.push_back(p_tc);
			topcmds.push_back(p_tc);

		}
		if (cur_cached) cur_cached++;
		++it;

		int_no_curr++;
	}

	tDWORD err = m_testManager->h_TrafficMonitor->sysSendMsg
		(pmc_MAIL_WASHER, pm_MESSAGE_GETLIST, 0,&mr, SER_SENDMSG_PSIZE);
	PR_ASSERT(PR_SUCC(err));

	it = letters.begin();
	cur_cached = cached;
	int_no_curr = 0;
	for (int i = first+count; i > first; --i)
	{
		if (int_no_curr == int_no) break;

		if (cur_cached && (!*cur_cached))
		{
			WaitForSingleObject (hReqToServerGot, INFINITE);
			int_no_curr++;
			if (int_no_curr == int_no)
			{
				//говорим отбой
				err = m_testManager->h_Proto->sysSendMsg
					(pmc_MAIL_WASHER, pm_CANCEL_REQUEST, 0, &washerSession, SER_SENDMSG_PSIZE);
				PR_ASSERT(PR_SUCC(err));
			}
			TranslateData(it->c_str(), it->c_str() + it->size(), dsServer);
		}


		SetEvent (hMessageToReceive);
		WaitForSingleObject (hMessageReceived, INFINITE);

		++it;
		if (cur_cached) cur_cached++;
	}

	SetEvent (hMessageToReceive);
	WaitForSingleObject (hMessageReceived, INFINITE);

	for (int i = 0; i < topcmds.size(); ++i) delete topcmds[i];
	
}

void SessionData::CancelOnDelete (int* start, int count, int int_no)
{
	string ok ("+OK\r\n");
	cPOP3MessageList ml;
	ml.assign (washerSession, false);
	ml.m_dwPOP3Session = washerSession.m_dwPOP3Session;

	vector<DELE_cmd*> delecmds;

	for (int no = 1;  no <= count; ++no)
	{
		if (no <= int_no)
		{
			DELE_cmd* dc = new DELE_cmd();
			delecmds.push_back (dc);
			dc->no = *(start+no-1);
			cmds_stor.push_back(dc);
		};

		messagesReceived.at(*(start+no-1)-1).m_bMarkToDelete = true;
		ml.m_POP3MessageList.push_back(messagesReceived.at(*(start+no-1)-1));
	}

	tDWORD err = m_testManager->h_TrafficMonitor->sysSendMsg
		(pmc_MAIL_WASHER, pm_MESSAGE_REMOVE, 0,&ml, SER_SENDMSG_PSIZE);
	PR_ASSERT(PR_SUCC(err));

	for (int no = 1;  no <= count; ++no)
	{
		WaitForSingleObject (hReqToServerGot, INFINITE);
		
		if (no == int_no) 
		{
			//говорим отбой
			err = m_testManager->h_Proto->sysSendMsg
				(pmc_MAIL_WASHER, pm_CANCEL_REQUEST, 0, &washerSession, SER_SENDMSG_PSIZE);
			PR_ASSERT(PR_SUCC(err));
		}

		TranslateData (ok.c_str(), ok.c_str() + ok.size(), dsServer);

		SetEvent (hMessageToDelete);
		WaitForSingleObject (hMessageDeleted, INFINITE);
		
		if (no == int_no) break;
	}

	SetEvent (hMessageToDelete);
	WaitForSingleObject (hMessageDeleted, INFINITE);

	for (int i = 0; i < delecmds.size(); ++i) delete delecmds[i];
}

/*void SessionData::CloseOnTransfer (int first, int count, int lines, vector<string> letters, bool* cached, int int_no)
{
	cPOP3MessageReguest mr;
	mr.assign (washerSession, false);
	mr.m_dwBegin = first;
	mr.m_dwCount = count;
	mr.m_dwSizeOf = lines;

	vector<TOP_cmd*> topcmds;

	vector<string>::iterator it = letters.begin();
	bool* cur_cached = cached;
	int int_no_curr = 0;
	for (int i = first+count; i > first; --i)
	{
		if (int_no_curr == int_no) break;
		if (cur_cached && (!*cur_cached))
		{

			TOP_cmd* p_tc = new TOP_cmd();

			p_tc->no = i;
			p_tc->lines = lines;
			p_tc->answer = *it;

			cmds_stor.push_back(p_tc);
			topcmds.push_back(p_tc);

		}
		if (cur_cached) cur_cached++;
		++it;

		int_no_curr++;
	}

	tDWORD err = m_testManager->h_TrafficMonitor->sysSendMsg
		(pmc_MAIL_WASHER, pm_MESSAGE_GETLIST, 0,&mr, SER_SENDMSG_PSIZE);
	PR_ASSERT(PR_SUCC(err));

	it = letters.begin();
	cur_cached = cached;
	int_no_curr = 0;
	for (int i = first+count; i > first; --i)
	{
		if (int_no_curr == int_no) break;

		if (cur_cached && (!*cur_cached))
		{
			WaitForSingleObject (hReqToServerGot, INFINITE);
			int_no_curr++;
			if (int_no_curr == int_no)
			{
				//SetEvent (hMessageToReceive);
				//говорим отбой
				SetEvent (hMessageToReceive);
				err = m_testManager->h_Proto->sysSendMsg
					(pmc_MAIL_WASHER, pm_SESSION_CLOSE, 0, &washerSession, SER_SENDMSG_PSIZE);
				PR_ASSERT(PR_SUCC(err));
				WaitForSingleObject (hMessageReceived, INFINITE);
				//WaitForSingleObject (hMessageReceived, INFINITE);

				Sleep (INFINITE);
			}
			TranslateData(it->c_str(), it->c_str() + it->size(), dsServer);
		}


		SetEvent (hMessageToReceive);
		WaitForSingleObject (hMessageReceived, INFINITE);

		++it;
		if (cur_cached) cur_cached++;
	}

	SetEvent (hMessageToReceive);
	WaitForSingleObject (hMessageReceived, INFINITE);

	for (int i = 0; i < topcmds.size(); ++i) delete topcmds[i];

}*/

void SessionData::ErrorOnTransfer (int first, int count, int lines, vector<string> letters, bool* cached, int int_no)
{
	string errs ("-ERR fehler\r\n");
	cPOP3MessageReguest mr;
	mr.assign (washerSession, false);
	mr.m_dwBegin = first;
	mr.m_dwCount = count;
	mr.m_dwSizeOf = lines;

	vector<TOP_cmd*> topcmds;

	vector<string>::iterator it = letters.begin();
	bool* cur_cached = cached;
	int int_no_curr = 0;
	for (int i = first+count; i > first; --i)
	{
		if (int_no_curr == int_no) break;
		if (cur_cached && (!*cur_cached))
		{

			TOP_cmd* p_tc = new TOP_cmd();

			p_tc->no = i;
			p_tc->lines = lines;
			p_tc->answer = *it;

			cmds_stor.push_back(p_tc);
			topcmds.push_back(p_tc);

		}
		if (cur_cached) cur_cached++;
		++it;

		int_no_curr++;
	}

	tDWORD err = m_testManager->h_TrafficMonitor->sysSendMsg
		(pmc_MAIL_WASHER, pm_MESSAGE_GETLIST, 0,&mr, SER_SENDMSG_PSIZE);
	PR_ASSERT(PR_SUCC(err));

	it = letters.begin();
	cur_cached = cached;
	int_no_curr = 0;
	for (int i = first+count; i > first; --i)
	{
		if (int_no_curr == int_no) break;

		if (cur_cached && (!*cur_cached))
		{
			WaitForSingleObject (hReqToServerGot, INFINITE);
			int_no_curr++;
			if (int_no_curr == int_no)
			{
				//передаем ошибку
				TranslateData(errs.c_str(), errs.c_str() + errs.size(), dsServer);
			} 
			else TranslateData(it->c_str(), it->c_str() + it->size(), dsServer);
		}


		SetEvent (hMessageToReceive);
		WaitForSingleObject (hMessageReceived, INFINITE);

		++it;
		if (cur_cached) cur_cached++;
	}

	SetEvent (hMessageToReceive);
	WaitForSingleObject (hMessageReceived, INFINITE);

	for (int i = 0; i < topcmds.size(); ++i) delete topcmds[i];

}