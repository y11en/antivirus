#define PR_IMPEX_DEF
#define NOMINMAX
#include "TestManager.h"

#include <plugin/p_tm.h>
#include <plugin/p_trafficmonitor.h>

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
_file ("h:\\from_imap4.txt"),
_trace ("h:\\imap4.txt"),
h_Proto (NULL),
h_TaskManager (NULL),
h_TrafficMonitor (NULL)

{

}

void TestManager::LoadPrague()
{
	static char const* plugins[] = {"nfio.ppl", "prutil.ppl", "tempfile.ppl",
		"thpimpl.ppl", "nntpprtc.ppl", "FakeTaskManager.ppl", "FakeTrafficMonitor.ppl", 0};

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

	error = h_TrafficMonitor->sysCreateObject((hOBJECT*)&m_Protocoller, IID_TASK, PID_NNTPPROTOCOLLER);
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
	switch (dcDetectCode)
	{
	case dcNeedMoreInfo:
	case dcProtocolDetected:
		{
			_detectParams.assign(_sessionCtx, false);
			_detectParams.dsDataSource = source;
			_detectParams.pLastReceivedData = (tVOID*)begin;
			_detectParams.ulLastReceivedData = end - begin;
			tERROR error = m_testManager->h_TaskManager->sysSendMsg (pmcTRAFFIC_PROTOCOLLER, pmDETECT, 0, &_detectParams, SER_SENDMSG_PSIZE);
			LOG_CALL5("pmDETECT ", aux::noRN(string(begin,begin + std::min(10, end-begin))), aux::prResultToString(error), aux::detectCodeToString(dcDetectCode),"\n");
			if (error == errOK_NO_DECIDERS)
			{
				//отправим сами себе на выход
				_ASSERTE (dcDetectCode == dcNeedMoreInfo);
				TranslateDataToItself(begin, end, source);
				break;
			}
			if (error == errOK_DECIDED)
			{
				_ASSERTE (dcDetectCode == dcProtocolDetected || dcDetectCode == dcDataDetected);
				TranslateDataToItself(begin, end, source);
				break;
			}
			_ASSERTE (!"Unexpected");

			break;
		}

	case dcDataDetected:
		{
			//отправляем буфер протоколёру
			process_status_t psStatus;
			_processParams.assign(_sessionCtx, false);
			_processParams.dsDataSource = source;
			_processParams.pData = (tVOID*)begin;
			_processParams.ulDataSize = end - begin;
			_processParams.phDetectorHandle = (hOBJECT*)&m_testManager->h_Proto;
			_processParams.psStatus = &psStatus;
			tERROR error = m_testManager->h_TaskManager->sysSendMsg
				(pmcTRAFFIC_PROTOCOLLER,
				pmPROCESS,
				0,
				&_processParams,
				SER_SENDMSG_PSIZE);
			LOG_CALL5("pmPROCESS ", aux::noRN(string(begin,begin + std::min(10, end-begin))), aux::prResultToString(error), aux::processStatusToString(psStatus),"\n");
			_ASSERTE (error == errOK_DECIDED && psStatus == psKeepAlive);
			break;
			//Sleep (INFINITE);
		}

	default:
		_ASSERTE (!"Unexpected");

	}

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
				//убрать потом
				if (*start == '\x0d' && chunk_size == 1)
					chunk_size += 1;
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

SessionData* SessionData::StartSession (TestManager* m_tm)
{
	SessionData* p_SD = new SessionData (m_tm);

	memset(p_SD->dwSessionCtx, 0, po_last*sizeof(tDWORD));
	p_SD->dwSessionCtx[po_ProxySession] = (tPTR)0xBBBBBBBB;
	p_SD->_sessionCtx.pdwSessionCtx = p_SD->dwSessionCtx;
	p_SD->_sessionCtx.dwSessionCtxCount = po_last;

	tERROR error;

	error = m_tm->h_Proto->sysRegisterMsgHandler(pmcTRAFFIC_PROTOCOLLER, rmhDECIDER, m_tm->h_TrafficMonitor, IID_ANY, PID_ANY, IID_ANY, PID_ANY );
	PR_ASSERT(PR_SUCC(error));
	error = m_tm->h_TrafficMonitor->sysSendMsg (pmcTRAFFIC_PROTOCOLLER,	pmSESSION_START, 0,	&p_SD->_sessionCtx, SER_SENDMSG_PSIZE);
	PR_ASSERT(PR_SUCC(error));
	error = m_tm->h_Proto->sysUnregisterMsgHandler(pmcTRAFFIC_PROTOCOLLER, m_tm->h_TrafficMonitor);
	PR_ASSERT(PR_SUCC(error));

	p_SD->_detectParams.assign(p_SD->_sessionCtx, false);
	p_SD->_detectParams.pdcDetectCode = &p_SD->dcDetectCode;
	p_SD->_detectParams.phDetectorHandle = (hOBJECT*)&m_tm->h_Proto;

	p_SD->_sessionCtx = p_SD->_sessionCtx;
	m_tm->AddSession (p_SD);

	return p_SD;
}

void SessionData::StopSession()
{
	tERROR error;

	error = m_testManager->h_Proto->sysRegisterMsgHandler(pmcTRAFFIC_PROTOCOLLER, rmhDECIDER, m_testManager->h_TrafficMonitor, IID_ANY, PID_ANY, IID_ANY, PID_ANY );
	PR_ASSERT(PR_SUCC(error));
	error = m_testManager->h_TrafficMonitor->sysSendMsg (pmcTRAFFIC_PROTOCOLLER,	pmSESSION_STOP, 0,	&_sessionCtx, SER_SENDMSG_PSIZE);
	PR_ASSERT(PR_SUCC(error));
	error = m_testManager->h_Proto->sysUnregisterMsgHandler(pmcTRAFFIC_PROTOCOLLER, m_testManager->h_TrafficMonitor);
	PR_ASSERT(PR_SUCC(error));

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

			LOG_CALL("<<FIRST _STREAM>>:");
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
