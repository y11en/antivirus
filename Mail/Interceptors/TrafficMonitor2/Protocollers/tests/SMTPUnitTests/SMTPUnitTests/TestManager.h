#if !defined(__TESTMANAGER_H)
#define __TESTMANAGER_H

#include <boost/shared_ptr.hpp>
#include <utils/prague/prague.hpp>

#include <iface/i_taskex.h>
#include <iface/i_mailtask.h>
#include <plugin/p_smtpprotocoller.h>

#include "../../../../trafficprotocoller.h"

#include <vector>

#include "_aux.h"

#define pgTEST_MANAGER_PTR	mPROPERTY_MAKE_GLOBAL( pTYPE_PTR  , 0x00001033 )

#define TEST_PROTO po_SMTP

using namespace std;
using namespace TrafficProtocoller;

extern hROOT g_root;

class TestManager;
struct SessionData
{
	SessionData (TestManager* m_tm):
dcDetectCode (dcNeedMoreInfo), m_testManager(m_tm), bOrderedToCloseConnection(false) {};

session_t _sessionCtx;

tPTR dwSessionCtx[po_last];
detect_t _detectParams;
process_t _processParams;
process_msg_t _process_msg;

detect_code_t dcDetectCode;

vector<process_msg_t> commands;
vector<vector<byte> > conn_stream;
//vector<hIO> gotten_objects; - вне контекста сессии

bool bNewPacket;
bool bOrderedToCloseConnection;

TestManager* m_testManager;

static SessionData* StartSession (TestManager* m_tm);
void StopSession ();
bool VerifyConnStream (const vector<aux::output_packet>* e_packets);
bool VerifyCheckedObjects (vector<hIO>* etalon);

bool TranslateData (const char* begin, const char* end, TrafficProtocoller::data_source_t source);
bool TranslateDataToItself(const char* begin, const char* end, TrafficProtocoller::data_source_t source);
void SendBytesToProcess (vector<string>* packets, vector<aux::output_packet>* out_packets,
						 bool split_packets = false);
};

typedef vector<SessionData*> sessions_arr;

class TestManager
{

public:
	__declspec(dllexport) TestManager();

	__declspec(dllexport) void TestDD_Runner ();
	__declspec(dllexport) void TestMailProcess_WithVirus (const string& data_folder, int i_no);

	void TestDD (const string& data_folder);
	__declspec(dllexport) void LoadPrague ();

private:


	void GetObjects (hTASK& m_Protocoller, hTASKMANAGER& h_TaskManager, hMAILTASK& h_TrafficMonitor);

public:




private:
	boost::shared_ptr<prague::init_prague> prague;


	vector<SessionData*> session_data;

public:

	SessionData* GetSessionData(tPTR _ctx)
	{
		for (sessions_arr::iterator it = session_data.begin(); it != session_data.end(); ++it)
			if (((*it)->_sessionCtx.pdwSessionCtx[TEST_PROTO]) == _ctx)
			{
				return *it;
			}
			_ASSERTE ("Session not found");
	}

	void DeleteSession (tPTR _ctx)
	{
		for (sessions_arr::iterator it = session_data.begin(); it != session_data.end(); ++it)
			if ((*it)->_sessionCtx.pdwSessionCtx[TEST_PROTO] == _ctx)
			{
				session_data.erase(it);
				break;
			}
			_ASSERTE ("Session not found");
	}

	void AddSession (SessionData* psd)
	{
		session_data.push_back(psd);
	}

	hTASKMANAGER h_TaskManager;
	hMAILTASK h_TrafficMonitor;
	hTASK h_Proto;
	tDWORD m_ProtoState;

	//~~

	vector<hIO> gotten_objects;// - вне контекста сессии

	//HANDLE hPacketCompletedEvent;
	//HANDLE hAboutToCompletePacketEvent;

	//bool bInterrupted;
	//bool bClosingConnectionExpected;
	//bool bNewDetectRequestExpected;

	bool bEachPacketSignalling;
	//~~

	//events
	HANDLE hProtoStateChanged;
	HANDLE hProtoStateChangeExpected;

	bool bTraceMailProcess;
	HANDLE hMailProcessExpected;
	HANDLE hMailProcessReceived;
	hIO io_ToProto;
	tDWORD mc_Verdict;

	//traces
	aux::TraceFile _file;
	aux::TraceFile _trace;

};














#endif