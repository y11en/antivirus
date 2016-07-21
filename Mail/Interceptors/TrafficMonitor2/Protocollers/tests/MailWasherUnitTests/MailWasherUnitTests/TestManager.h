#if !defined(__TESTMANAGER_H)
#define __TESTMANAGER_H

#define CONFIGURE(SIZE,TIMEOUT)\
	Configure::GetConfiguration().MaximumChunkSize(SIZE);\
	Configure::GetConfiguration().TimeoutBetweenChunks(TIMEOUT);


#include <boost/shared_ptr.hpp>
#include <utils/prague/prague.hpp>

#include <iface/i_taskex.h>
#include <iface/i_mailtask.h>



#include <plugin/p_pop3protocoller.h>
#include <plugin/p_mailwasher.h>


#include "../../../../trafficprotocoller.h"

#include <vector>
#include <deque>

#include "_aux.h"

#include <structs/s_mailwasher.h>

#define pgTEST_MANAGER_PTR	mPROPERTY_MAKE_GLOBAL( pTYPE_PTR  , 0x00001033 )

#define TEST_PROTO po_MailWasher

using namespace std;
using namespace TrafficProtocoller;

extern hROOT g_root;

struct _cmd
{
	enum cmd_type
	{
		cmd_list,
		cmd_top,
		cmd_retr,
		cmd_dele
	};

	virtual cmd_type GetCmdType () = 0;
};

struct LIST_cmd: public _cmd
{
	int no;
	//vector<int> sizes;
	string to_send;

	virtual cmd_type GetCmdType ()
	{
		return cmd_list;
	}
};

struct TOP_cmd: public _cmd
{
	int no;
	int lines;
	string answer;

	virtual cmd_type GetCmdType ()
	{
		return cmd_top;
	}
};

struct RETR_cmd: public _cmd
{
	int no;
	string answer;

	virtual cmd_type GetCmdType ()
	{
		return cmd_retr;
	}
};

struct DELE_cmd: public _cmd
{
	int no;

	virtual cmd_type GetCmdType ()
	{
		return cmd_dele;
	}
};

class cmds_storage: public deque<_cmd*>
{
public:
	_cmd* GetCommand ()
	{
		_ASSERTE (size());
		_cmd* q = front();
		pop_front();
		return q;
	}

};


class TestManager;
struct SessionData
{
	SessionData (TestManager* m_tm):
			dcDetectCode (dcNeedMoreInfo), m_testManager(m_tm),
			hWasherSessionStarted (CreateEvent (NULL, false, false, NULL)),
			hWasherSessionFinished (CreateEvent (NULL, false, false, NULL)),
			hMessageReceived (CreateEvent (NULL, false, false, NULL)),
			hMessageToReceive (CreateEvent (NULL, false, false, NULL)),
			hMessageDeleted (CreateEvent (NULL, false, false, NULL)),
			hMessageToDelete (CreateEvent (NULL, false, false, NULL)),
			hReqToServerGot (CreateEvent (NULL, false, false, NULL))

{};

	~SessionData()
	{
		CloseHandle (hWasherSessionStarted);
		CloseHandle (hWasherSessionFinished);
		CloseHandle (hMessageReceived);
		CloseHandle (hMessageToReceive);
		CloseHandle (hMessageDeleted);
		CloseHandle (hMessageToDelete);
		CloseHandle (hReqToServerGot);
	}

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



TestManager* m_testManager;

//
cmds_storage cmds_stor;


HANDLE hWasherSessionStarted;
HANDLE hWasherSessionFinished;
HANDLE hMessageToReceive;
HANDLE hMessageReceived;
HANDLE hMessageToDelete;
HANDLE hMessageDeleted;
HANDLE hReqToServerGot;
cPOP3Session washerSession;
vector<cPOP3Message> messagesReceived;
vector<short> messagesAvailable;

auto_ptr<vector<string> > raw_packets;
auto_ptr<vector<aux::output_packet> > out_packets;
auto_ptr<vector<hIO> > control_files;

process_status_t p_s_t;
process_t process_msg;

static SessionData* StartSession (TestManager* m_tm, const std::string& data_folder);
void StopSession ();
bool VerifyConnStream (const vector<aux::output_packet>* e_packets);
bool VerifyCheckedObjects (vector<hIO>* etalon);

__declspec(dllexport) bool TranslateData (const char* begin, const char* end, TrafficProtocoller::data_source_t source);
bool TranslateDataToItself(const char* begin, const char* end, TrafficProtocoller::data_source_t source);
void SendBytesToProcess (vector<string>* packets, vector<aux::output_packet>* out_packets,
						 bool split_packets = false);

__declspec(dllexport) void SendNoopReply ();

void wGetLetterTops (int first, int count, int lines, vector<string> letters, bool* cached = NULL);
void wGetLetterTop (int no, int lines, string letter, bool cached);

void wGetLetters (int first, int count, vector<string> letters, bool* cached = NULL);
void wGetLetter (int no, string letter, bool cached);

void DeleteLetter (int no);
void DeleteLetterErr (int no);


//special
void CancelOnTransfer (int first, int count, int lines, vector<string> letters, bool* cached, int int_no);
void CancelOnDelete (int* start, int count, int int_no);

void ErrorOnTransfer (int first, int count, int lines, vector<string> letters, bool* cached, int int_no);
};

typedef vector<SessionData*> sessions_arr;

class TestManager
{

public:
	__declspec(dllexport) TestManager();

	//__declspec(dllexport) void TestDD_Runner ();
	//void TestDD (const string& data_folder);
	//__declspec(dllexport) void TestMailProcess (const string& data_folder);
	__declspec(dllexport) void LoadPrague ();

//private:


	__declspec(dllexport) void GetObjects (hTASK& m_Protocoller, hTASKMANAGER& h_TaskManager, hMAILTASK& h_TrafficMonitor);


	//tests
	__declspec(dllexport) void TestHeadersRequest (const string& data_folder);
	__declspec(dllexport) void TestMessagesRequest (const string& data_folder);
	
	__declspec(dllexport) void TestRetrMessage_MW_Messages (const string& data_folder);
	__declspec(dllexport) void TestRetrMessage_MW_Headers (const string& data_folder);
	__declspec(dllexport) void TestRetrHeaders_MW_Messages (const string& data_folder);
	__declspec(dllexport) void TestRetrHeaders_MW_Headers (const string& data_folder);
	
	__declspec(dllexport) void TestRetr_Deleted (const string& data_folder);
	__declspec(dllexport) void TestTop_Deleted (const string& data_folder);
	__declspec(dllexport) void TestDele_Deleted (const string& data_folder);

	__declspec(dllexport) void TestPipelining_Cached (const string& data_folder);
	__declspec(dllexport) void TestPipelining_NotCached (const string& data_folder);
	__declspec(dllexport) void TestPipelining_Mixed (const string& data_folder);

	__declspec(dllexport) void TestSessionWithDeleted (const string& data_folder);
	__declspec(dllexport) void TestRepeatRequests (const string& data_folder);
	
	__declspec(dllexport) void TestCancelRequest (const string& data_folder);
	__declspec(dllexport) void TestCancelDelete (const string& data_folder);

	//__declspec(dllexport) void TestCloseRequest (const string& data_folder);//не использовать
	//__declspec(dllexport) void TestCloseDelete (const string& data_folder);//не использовать

	__declspec(dllexport) void TestErrRequest (const string& data_folder);
	__declspec(dllexport) void TestErrDelete (const string& data_folder);

	//~tests
	
	//__declspec(dllexport) void Test1 (const string& data_folder);
	//__declspec(dllexport) void TestTopsRequest (const string& data_folder);


public:




private:
	boost::shared_ptr<prague::init_prague> prague;


	vector<SessionData*> session_data;

public:

	SessionData* GetSessionData(tPTR _ctx)
	{
		for (sessions_arr::iterator it = session_data.begin(); it != session_data.end(); ++it)
			if (((*it)->_sessionCtx.pdwSessionCtx[TEST_PROTO]) == (tPTR)_ctx)
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

	//traces
	aux::TraceFile _file;
	aux::TraceFile _trace;

};














#endif