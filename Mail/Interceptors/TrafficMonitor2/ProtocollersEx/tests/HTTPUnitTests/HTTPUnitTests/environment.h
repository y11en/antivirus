#if !defined(__ENVIRONMENT_H)
#define __ENVIRONMENT_H

#include <boost/shared_ptr.hpp>
#include "_aux.h"
#include <Mail/structs/s_httpscan.h>
#include <ParentalControl/structs/s_urlflt.h>

#include "query.hpp"

#include "Tracer.h"

class TrafficSender
{
public:
	struct Data
	{
		boost::shared_ptr<aux::processor_proxy> sh_proxy;
		std::string data;
		msg_direction direction;
		bool is_ssl;
		HANDLE hDoneEvent;
	};

	static void Enqueue (boost::shared_ptr<aux::processor_proxy> sh_proxy, const char* begin, const char* end, msg_direction direction, bool is_ssl, HANDLE hDoneEvent = INVALID_HANDLE_VALUE);
	LPCRITICAL_SECTION Get_pCS ();
	std::deque<Data>& GetDeque ();
	static TrafficSender& GetInstance();


private:

	TrafficSender();
	~TrafficSender();
	static void threadFunc (void* param);

	HANDLE hThread;
	bool fl_Stop;
	CRITICAL_SECTION cs;
	std::deque<Data> tr_deque;

};


struct pr_novtable CTrafficMonitor : public cTask
{
public:

	CTrafficMonitor ();
	~CTrafficMonitor ();

	tERROR pr_call SetSettings( const cSerializable* p_settings ) {return errOK;};
	tERROR pr_call GetSettings( cSerializable* p_settings ) {return errOK;};
	tERROR pr_call AskAction( tActionId p_actionId, cSerializable* p_actionInfo ) {return errOK;};
	tERROR pr_call SetState( tTaskRequestState p_state ) {return errOK;};
	tERROR pr_call GetStatistics( cSerializable* p_statistics ) {return errOK;};

	virtual tERROR pr_call sysSendMsg( tDWORD msg_cls, tDWORD msg_id, tPTR ctx, tPTR pbuff, tDWORD* pblen );
	void setEachPacketSignalling (bool mode);
	void setSpecificDataSignalling (bool mode);
	bool isPacketCompleted (vector<byte>& packet_buf);
	bool __isPacketCompleted (const string& packet);
	bool VerifySentPackets (const vector<aux::output_packet>* e_packets);
	bool VerifyConnStream (const vector<aux::output_packet>* e_packets);

	static std::string RebuildPacket (const std::string& packet, bool service_data = true);	

private:

	aux::TraceFile _file;

	enum packet_type
	{
		CMD_NONE,
		CMD_GET,
		CMD_OPTIONS,
		CMD_HEAD,
		CMD_ANSWER,
		CMD_POST,
		CMD_PUT,
		CMD_DELETE,
		CMD_TRACE,
		CMD_EXT,
		CMD_CONTINUE
	};

	struct PacketInfo
	{
		PacketInfo()
		{
			Reset();
		}

		void Reset ()
		{
			m_packetType = CMD_NONE;
			m_lengthDefined = false;
		}

		packet_type m_packetType;
		bool m_lengthDefined;
		int m_length;

		deque<packet_type> requests;

	};

	PacketInfo m_lastPacket;
	bool bEachPacketSignalling;
	
	vector<tp::process_msg_t> commands;
	vector<vector<byte> > conn_stream;

	bool bNewPacket;

public:
	bool bSpecificDataSignalling;
	string strSpecificData;
	HANDLE hAboutToCompleteSpecificDataEvent;
	HANDLE hSpecificDataCompletedEvent;


public:

	class SkipThatQuery: public CommonQuery<errOK_DECIDED, std::string, std::string>
	{
	public:

		tERROR Process (tp::anybody_here_t* skip_msg);

	private:

	};

	SkipThatQuery m_skipThatQuery;

public:

	SkipThatQuery& GetSkipThatQuery();


public:

	HANDLE hPacketCompletedEvent;
	HANDLE hAboutToCompletePacketEvent;

	bool bInterrupted;
	bool bClosingConnectionExpected;
	bool bNewDetectRequestExpected;

	void ExpectClosingConnection ()
	{
		bClosingConnectionExpected = true;
	}

	void ExpectNewDetectRequest ()
	{
		bNewDetectRequestExpected = true;
	}

	const vector<vector<byte> >& GetHttpData ()
	{
		return conn_stream;
	}

};



#endif