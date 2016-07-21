#include "environment.h"

#include <iostream>
#include <process.h>
#include <sstream>

using namespace std;

void TrafficSender::Enqueue (boost::shared_ptr<aux::processor_proxy> sh_proxy, const char* begin, const char* end, msg_direction direction, bool is_ssl, HANDLE hDoneEvent)
{
	EnterCriticalSection (TrafficSender::GetInstance().Get_pCS());
	Data dd;
	dd.sh_proxy = sh_proxy;
	dd.data.assign (begin, end-begin);
	dd.direction = direction;
	dd.is_ssl = is_ssl;
	dd.hDoneEvent = hDoneEvent;
	TrafficSender::GetInstance().GetDeque().push_back (dd);
	LeaveCriticalSection (TrafficSender::GetInstance().Get_pCS());
}

LPCRITICAL_SECTION TrafficSender::Get_pCS ()
{
	return &cs;
}

std::deque<TrafficSender::Data>& TrafficSender::GetDeque ()
{
	return tr_deque;
}

TrafficSender& TrafficSender::GetInstance()
{
	static TrafficSender ts;
	return ts;
}


TrafficSender::TrafficSender(): fl_Stop (false)
{
	InitializeCriticalSection (&cs);
	hThread = (HANDLE)_beginthread (TrafficSender::threadFunc, NULL, NULL);
}

TrafficSender::~TrafficSender()
{
	fl_Stop = true;
	WaitForSingleObject (hThread, INFINITE);
	CloseHandle (hThread);
}

void TrafficSender::threadFunc (void* param)
{
	TrafficSender& ts = TrafficSender::GetInstance();
	while (true)
	{
		EnterCriticalSection(&ts.cs);
		while (ts.tr_deque.size())
		{	
			Data& dd = ts.GetDeque().front();
			dd.sh_proxy->process(dd.data.c_str(), dd.data.c_str()+dd.data.size(), dd.direction, dd.is_ssl);
			if (dd.hDoneEvent != INVALID_HANDLE_VALUE) SetEvent (dd.hDoneEvent);
			ts.GetDeque().pop_front ();
		}
		LeaveCriticalSection(&ts.cs);

		if (ts.fl_Stop) return;
		Sleep (1000);
	}
}

/////////////////////////////////////////////////////////////////////////


CTrafficMonitor::CTrafficMonitor (): bNewPacket (true), bEachPacketSignalling (false), bClosingConnectionExpected (false),
		bInterrupted (false), bNewDetectRequestExpected (false), cTask(), bSpecificDataSignalling(false),
		_file (string("h:\\HTTP\\to_client.txt").append(aux::GetCurrentThreadId()))
{
	hPacketCompletedEvent = CreateEvent (NULL, false, false, NULL);
	hAboutToCompletePacketEvent = CreateEvent (NULL, false, false, NULL);

	hSpecificDataCompletedEvent = CreateEvent (NULL, false, false, NULL);
	hAboutToCompleteSpecificDataEvent = CreateEvent (NULL, false, false, NULL);
};

CTrafficMonitor::~CTrafficMonitor ()
{
	CloseHandle (hPacketCompletedEvent);
	CloseHandle (hAboutToCompletePacketEvent);

	CloseHandle (hSpecificDataCompletedEvent);
	CloseHandle (hAboutToCompleteSpecificDataEvent);
}

tERROR CTrafficMonitor::sysSendMsg( tDWORD msg_cls, tDWORD msg_id, tPTR ctx, tPTR pbuff, tDWORD* pblen )
{
	_ASSERTE (!bInterrupted);
	if (msg_cls == pmcTRAFFIC_PROTOCOLLER)
	{
		tp::process_msg_t* pin_msg = reinterpret_cast<tp::process_msg_t*>(pbuff);
		tp::process_msg_t process_msg;
		process_msg.assign(*static_cast<tp::session_t*>(pin_msg), false);
		process_msg.dsDataTarget = pin_msg->dsDataTarget;
		process_msg.psStatus = pin_msg->psStatus;
		//IO
		prague::IO<char> obj_ptr(prague::create_temp_io());
		tQWORD size;
		((hIO)pin_msg->hData)->GetSize ( &size, IO_SIZE_TYPE_EXPLICIT);
		cIOObj::copy (obj_ptr.get(), pin_msg->hData, size);
		process_msg.hData = obj_ptr.release();

		{//запись в трейс
			std::vector<byte> vec;
			aux::pump_io_to_array (process_msg.hData, vec);
			if (vec.size())
			{//иногда приходит пустые данные - зачем, пока не пойму
				_file.Write((char*)&vec[0], (char*)(&vec[0])+vec.size());
				_file.Flush ();
			}
		}

		//если сменилось направление передачи, то по-любому, пакет закончен
		if (commands.size() && commands.back().dsDataTarget != process_msg.dsDataTarget) bNewPacket = true;
		//запишем порцию данных
		commands.push_back (process_msg);

		if (!bNewPacket)
		{//кусок старого сообщения
			aux::pump_io_to_array (process_msg.hData, conn_stream.back());
		}
		else
		{//кусок нового сообщения
			conn_stream.push_back (vector<byte>());
			aux::pump_io_to_array (process_msg.hData, conn_stream.back());
			bNewPacket = false;
		}

		if (isPacketCompleted(conn_stream.back()))
			bNewPacket = true;

		//процессор пока не должен отказываться от обработки, скидывая нас обратно в режим детектирования
		//но может потребовать закрыть соединение, ежели возвращает свой 499-й ответ
		//или замещает баннер
		_ASSERTE (pin_msg->psStatus == TrafficProtocoller::psKeepAlive ||
			(strncmp ((char*)(&(conn_stream.back()[0])), "HTTP/1.1 499", 12) == 0 &&
			pin_msg->psStatus == TrafficProtocoller::psCloseConnection ) ||
			(string (conn_stream.back().begin(), conn_stream.back().end()).find ("HTTP/1.1 200") == 0 &&
			string (conn_stream.back().begin(), conn_stream.back().end()).find ("Server: kav/7.0\015\012") != string::npos &&
			pin_msg->psStatus == TrafficProtocoller::psCloseConnection )
			||/*можно разорвать соединение*/bClosingConnectionExpected
			||/*возвращаемся в режим детекта*/bNewDetectRequestExpected);

		if (pin_msg->psStatus == TrafficProtocoller::psUnknown)
		{//велено начать детектировать трафик заново, текущий пакет заканчиваем
			bNewDetectRequestExpected = false;
			bNewPacket = true;
			m_lastPacket.Reset();
		};

		if (pin_msg->psStatus == TrafficProtocoller::psCloseConnection)
		{//соединение велено закрыть, текущий пакет заканчиваем
			bClosingConnectionExpected = false;
			bNewPacket = true;
			//ставим для контроля флаг того, что соединение разорвано, который нужно сбросить вручную.
			bInterrupted = true;
			//пересоберем chunked-пакет, запишем только данные
			string packet (conn_stream.back().begin(), conn_stream.back().end());
			if (packet.find ("Transfer-Encoding: chunked\015\012") != string::npos)
			{
				string packet1 = RebuildPacket( packet, false );
				conn_stream.back().assign (packet1.c_str(), packet1.c_str() + packet1.size());
			}
			m_lastPacket.Reset();
		}
		if (bNewPacket && bEachPacketSignalling)
		{
			WaitForSingleObject(hAboutToCompletePacketEvent, INFINITE);
			SetEvent(hPacketCompletedEvent);
		}
		if (bSpecificDataSignalling &&
			strSpecificData == string(conn_stream.back().begin(), conn_stream.back().end()))
		{
			WaitForSingleObject(hAboutToCompleteSpecificDataEvent, INFINITE);
			SetEvent(hSpecificDataCompletedEvent);
		}

		process_msg.hData->sysCloseObject();

		return errOK_DECIDED;
	}

	if (msg_cls == pmc_HTTPSCAN_PROCESS)
	{

		return m_skipThatQuery.Process(static_cast<tp::anybody_here_t*>(pbuff));

	}

	return errOK_NO_DECIDERS;
}

void CTrafficMonitor::setEachPacketSignalling (bool mode)
{
	bEachPacketSignalling = mode;
}

void CTrafficMonitor::setSpecificDataSignalling (bool mode)
{
	bSpecificDataSignalling = mode;
}

bool CTrafficMonitor::isPacketCompleted (vector<byte>& packet_buf)
{
	string packet (packet_buf.begin(), packet_buf.end());

	bool result = __isPacketCompleted (packet);
	if (result)
	{
		if (packet.find ("Transfer-Encoding: chunked\015\012") != string::npos)
		{
			string packet1 = RebuildPacket( packet );
			packet_buf.assign (packet1.c_str(), packet1.c_str() + packet1.size());
		}

		//if (bEachPacketSignalling)
		//{
		//	WaitForSingleObject(hAboutToCompletePacketEvent, INFINITE);
		//	SetEvent(hPacketCompletedEvent);
		//}
		switch (m_lastPacket.m_packetType)
		{
		case CMD_GET:
		case CMD_OPTIONS:
		case CMD_HEAD:
		case CMD_POST:
		case CMD_PUT:
		case CMD_DELETE:
		case CMD_TRACE:
		case CMD_EXT:
		case CMD_CONTINUE:
			m_lastPacket.requests.push_back (m_lastPacket.m_packetType);
			break;
		default://response
			_ASSERTE (m_lastPacket.requests.size() || //there is a corresponding request
				packet.find ("HTTP/1.1 499") == 0 || //499 response by kav
				packet.find ("HTTP/1.1 200") == 0 &&
				packet.find ("Server: kav/7.0\015\012") != string::npos); //200 response by kav
			if (m_lastPacket.requests.size()) m_lastPacket.requests.pop_front();
		}

		m_lastPacket.Reset();
	}
	return result;
}

bool CTrafficMonitor::__isPacketCompleted (const string& packet)
{
	//никакого формального признака окончания пакета нет, только анализ поступивших данных
	//это важно не для всех тестов, к тому же данные мы готовим сами и смысла заморачиваться
	//со сложным парсером нет, достаточно, если он будет работать на наших примерах
	//переделать под Boost::regex ?
	{
		if (m_lastPacket.m_packetType == CMD_NONE)
		{
			//определяем тип
			if (packet.find ("GET") == 0) m_lastPacket.m_packetType = CMD_GET;
			if (packet.find ("DELETE") == 0) m_lastPacket.m_packetType = CMD_DELETE;
			if (packet.find ("OPTIONS") == 0) m_lastPacket.m_packetType = CMD_OPTIONS;
			if (packet.find ("POST") == 0) m_lastPacket.m_packetType = CMD_POST;
			if (packet.find ("PUT") == 0) m_lastPacket.m_packetType = CMD_PUT;
			if (packet.find ("HEAD") == 0) m_lastPacket.m_packetType = CMD_HEAD;
			if (packet.find ("TRACE") == 0) m_lastPacket.m_packetType = CMD_TRACE;
			if (packet.find ("HTTP/") == 0) m_lastPacket.m_packetType = CMD_ANSWER;
			if (packet.find ("PROPFIND") == 0 || packet.find ("PROPPATCH") == 0 || packet.find ("MKCOL") == 0 ||
				packet.find ("COPY") == 0 || packet.find ("MOVE") == 0 || packet.find ("LOCK") == 0 ||
				packet.find ("UNLOCK") == 0 || packet.find ("BCOPY") == 0 || packet.find ("BDELETE") == 0 ||
				packet.find ("BMOVE") == 0 || packet.find ("BPROPFIND") == 0 || packet.find ("BPROPPATCH") == 0 ||
				packet.find ("NOTIFY") == 0 || packet.find ("POLL") == 0 || packet.find ("SEARCH") == 0 ||
				packet.find ("SUBSCRIBE") == 0 || packet.find ("UNSUBSCRIBE") == 0 || packet.find ("X-MS-ENUMATTS") == 0 ||
				packet.find ("CHECKOUT") == 0 || packet.find ("CHECKIN") == 0 || packet.find ("UNCHECKOUT") == 0 ||
				packet.find ("VERSION-CONTROL") == 0 || packet.find ("MKWORKSPACE") == 0 || packet.find ("UPDATE") == 0 ||
				packet.find ("LABEL") == 0 || packet.find ("MERGE") == 0 || packet.find ("BASELINE-CONTROL") == 0 ||
				packet.find ("MKACTIVITY") == 0 || packet.find ("REPORT") == 0 || packet.find ("TEST_METHOD") == 0)
				m_lastPacket.m_packetType = CMD_EXT;

		}
		if (m_lastPacket.m_packetType == CMD_ANSWER || m_lastPacket.m_packetType == CMD_OPTIONS ||
			m_lastPacket.m_packetType == CMD_POST ||
			m_lastPacket.m_packetType == CMD_PUT || m_lastPacket.m_packetType == CMD_EXT)
		{
			if (packet.find ("HTTP/1.1 100 Continue\015\012\015\012") != string::npos)
			{
				m_lastPacket.m_packetType = CMD_CONTINUE;
				return true;
			};
			if (packet.find ("\015\012\015\012") != string::npos && 
				(
				packet.find ("HTTP/1.1 304") != string::npos ||
				packet.find ("HTTP/1.1 204") != string::npos
				)) return true;
			if (m_lastPacket.m_lengthDefined && packet.size()>=/*may be bug*/m_lastPacket.m_length) return true;

			//ответ на команду HEAD не содержит тела
			if (m_lastPacket.requests.size() && m_lastPacket.requests.front() == CMD_HEAD &&
				packet.find("\x0d\x0a\x0d\x0a") != string::npos) return true;

			if (!m_lastPacket.m_lengthDefined)
			{
				size_t pos = packet.find ("\x0d\x0a\x0d\x0a");
				if (pos == string::npos) return false;

				size_t pos2;

				pos2 = packet.find ("Content-Length: ");
				if (pos2 != string::npos)
				{
					//есть определенная длина
					size_t pos3 = packet.find ("\x0d\x0a", pos2);
					if (pos3 != string::npos)
					{
						char buf[20];
						strncpy (buf, packet.c_str()+pos2+16, pos3-pos2-16 );
						buf [pos3-pos2-16] = 0;
						return ( packet.size() >= atoi (buf) + 4 + pos);
					}
					return false;
				}

				pos2 = packet.find ("Transfer-Encoding: chunked\x0d\x0a");
				if (pos2 != string::npos)
				{
					//chunked
					size_t pos3 = packet.find ("\x0d\x0a\x0d\x0a", pos2);
					if (pos3 != string::npos)
					{
						size_t pos4 = packet.find ("\x0d\x0a\x0d\x0a", pos3+4);
						if (pos4 != string::npos) return true;
					}

					return false;
				}

			}
		}

		if (m_lastPacket.m_packetType == CMD_GET || m_lastPacket.m_packetType == CMD_OPTIONS ||
			m_lastPacket.m_packetType == CMD_HEAD || m_lastPacket.m_packetType == CMD_DELETE ||
			m_lastPacket.m_packetType == CMD_PUT || m_lastPacket.m_packetType == CMD_POST ||
			m_lastPacket.m_packetType == CMD_TRACE || m_lastPacket.m_packetType == CMD_EXT)
		{
			return (packet.find("\x0d\x0a\x0d\x0a") != string::npos && 
				packet.find ("Transfer-Encoding: chunked\x0d\x0a") == string::npos &&
				( packet.find ("Content-Length: ") == string::npos || packet.find ("Content-Length: 0") != string::npos)
				);
		}

	}
	return false;
}

bool CTrafficMonitor::VerifySentPackets (const vector<aux::output_packet>* e_packets)
{
	LOG_CALL("Verifying sent packets ...\n");
	size_t min_count = std::min (e_packets->size(), commands.size());
	bool bResults = true;
	for (int i = 0; i < min_count; ++i)
	{
		char buf[32];
		LOG_CALL3("Verifying packet ",itoa(i+1, buf, 10),"\n");
		if (!aux::isIoEquals (e_packets->at(i).hData, commands.at(i).hData) ||
			/*e_packets->at(i).dsDataTarget != commands.at(i).dsDataTarget ||*/
			//пока отключено, хрен разберет, источник это или получатель
			e_packets->at(i).psStatus != commands.at(i).psStatus ) 
		{
			bResults = false;
			char buf[32];
			LOG_CALL3("\tIncorrect packet ",itoa(i+1, buf, 10),"...\n");
		}
	}

	if (e_packets->size() != commands.size())
	{
		LOG_CALL("\tIncorrect number of packets...\n");
		return false;
	}

	return bResults;

}

bool CTrafficMonitor::VerifyConnStream (const vector<aux::output_packet>* e_packets)
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

std::string CTrafficMonitor::RebuildPacket (const std::string& packet, bool service_data)
{//return packet;
	class ChunkException: public std::exception
	{
	};

	class NoNullChunk_ChunkException: public ChunkException
	{
	};

	class NoChunkLength_ChunkException: public ChunkException
	{
	};

	class Chunk
	{
	public:
		Chunk (const string& data): m_data (data)
		{
		}

		bool NextChunk (size_t& raw_offset, string& data)
		{
			if (raw_offset == m_data.size())
			{
				throw (NoNullChunk_ChunkException());
			}
			const string null_chunk ("0\015\012\015\012");
			size_t nc_offset = m_data.find (null_chunk, raw_offset);
			if (nc_offset == raw_offset) return false;
			if (nc_offset == string::npos) nc_offset = m_data.size();
			size_t dc_offset = m_data.find ("\015\012", raw_offset);
			if (dc_offset == string::npos)
			{
				//нет информации о длине чанка, абзац
				throw (NoChunkLength_ChunkException());
			}
			size_t sc_offset = m_data.find (";", raw_offset);
			if (sc_offset == string::npos || sc_offset > dc_offset) sc_offset = dc_offset;
			char buf[10];
			_ASSERTE (sc_offset - raw_offset > 0 && sc_offset - raw_offset < 10);
			strncpy (buf, m_data.c_str() + raw_offset, sc_offset - raw_offset);
			buf[sc_offset - raw_offset] = 0;
			std::stringstream strstr;
			strstr<<hex<<buf;
			int chunk_size;
			strstr>>chunk_size;
			_ASSERTE (chunk_size);
			dc_offset+=2;
			int data_size = std::min ((long)chunk_size, nc_offset - raw_offset);

			data.append (m_data.c_str() + dc_offset, data_size);
			raw_offset = std::min (dc_offset + data_size + 2, m_data.size());
			return true;
		}

		private:

			const string& m_data;

	};

	if (packet.find ("Transfer-Encoding: chunked\015\012") == string::npos) return packet;
	string result;
	size_t header_end = packet.find ("\015\012\015\012");
	_ASSERTE (header_end != string::npos);
	header_end += 4;
	if (service_data)
		result.append (packet.c_str(), header_end);
	string chunk_data;
	size_t chunk_offset = header_end;
	Chunk ch (packet);

	try
	{
		while (ch.NextChunk(chunk_offset, chunk_data));
	}
	catch (NoNullChunk_ChunkException&)
	{
		//в режиме оборванного коннекта никаких завершающих чанков!
		if (!service_data)
		{
			return chunk_data;
		}
		else throw;
	}

	char buf[10];
	result.append (itoa (chunk_data.size(), buf, 16));
	result.append ("\015\012");
	result.append (chunk_data);
	result.append ("\015\012");
	result.append ("0\015\012\015\012");

	return result;
}	


tERROR CTrafficMonitor::SkipThatQuery::Process (tp::anybody_here_t* skip_msg)
{
	if (bActive)
	{
		m_arg1 = skip_msg->szObjectName.c_str(cCP_ANSI);
		m_arg2 = skip_msg->szObjectType.c_str(cCP_ANSI);

		SetEvent (hRequestReceived);
		
		DWORD waitResult = WaitForMultipleObjects(2, resEvents, false, INFINITE);
		_ASSERTE (waitResult>=WAIT_OBJECT_0 && waitResult<=(WAIT_OBJECT_0 + 1));
		return (waitResult == WAIT_OBJECT_0) ? errOK_DECIDED : errOK_NO_DECIDERS;
	}
	else return errOK;
}


CTrafficMonitor::SkipThatQuery& CTrafficMonitor::GetSkipThatQuery()
{
	return m_skipThatQuery;
};
