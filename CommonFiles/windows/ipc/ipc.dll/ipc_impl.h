// ipc_impl.h
//
// Interprocess communication library (IPC)
//
// Implementation defines
//
// (C) 2003 Kaspersky Labs
//
// Author: Vyacheslav A. Batenin (Vyacheslav.Batenin@kaspersky.com)
//

#ifndef _ipc_impl_h_INCLUDED_
#define _ipc_impl_h_INCLUDED_ 1

#ifndef STRICT
#define STRICT
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

#define	IPC_API	__declspec(dllexport)
#include <windows/ipc/ipc.h>
#include <service/sa.h>

#include <stdlib.h>
#include <string.h>

////////////////////////////////////////////////////////////////
// Forward declarations

class IPC_Runtime;
class IPC_Server;
class IPC_Connection;

////////////////////////////////////////////////////////////////
// Utilites and constants

const HIPCSERVER     HIPCSERVER_INVALID = 0;
const HIPCCONNECTION HIPCCONNECTION_INVALID = 0;

// timeout values are limited to 1/4 of max range
inline bool IsValidTimeout (DWORD tmo)
	{ return (tmo == INFINITE || tmo < 0x40000000); }

// name limits
const int IPC_MAX_PATH = MAX_PATH;
const int IPC_MAX_SERVER_NAME = IPC_MAX_PATH - 40;

// kernel object name components
const char IPC_PORT_PREFIX[]     = "KL-IPC-P-56828276151E-";  // prefix for port object names
const char IPC_CONN_PREFIX[]     = "KL-IPC-C-25ECA7CF0DC6-";  // prefix for connection control objects

const char IPC_SUFFIX_AVAIL[]   = "-A";  // 'port available' event
const char IPC_SUFFIX_MUTEX[]   = "-M";  // port access mutex
const char IPC_SUFFIX_BUF[]     = "-B";  // connection SHM buffer
const char IPC_SUFFIX_CLOSE[]   = "-X";  // connection close event

const char IPC_SUFFIX_CLIENT[]  = "-CS";  // client->server channel objects
const char IPC_SUFFIX_SERVER[]  = "-SC";  // server->client channel objects

const char IPC_SUFFIX_SENDRDY[] = "-SR"; // client SendRdy event
const char IPC_SUFFIX_SEND[]    = "-S";  // client Send event
const char IPC_SUFFIX_RECV[]    = "-R";  // client Recv event

const DWORD IPC_BUFFER_SIZE = 0x1000;
const DWORD IPC_MSG_SIZE_LIMIT = 0x20000000;

inline DWORD IPC_ERR_TO_RC (DWORD ec)
	{ return (ec == IPC_ERR_TIMEOUT) ? IPC_RC_TIMEOUT : IPC_RC_ERROR; }

inline HIPCSERVER IPC_ERR_TO_HIPCSERVER (DWORD ec)
	{ return (ec == IPC_ERR_TIMEOUT) ? ( HIPCSERVER ) IPC_RC_TIMEOUT : ( HIPCSERVER ) IPC_RC_INVALID_HANDLE; }

inline HIPCCONNECTION IPC_ERR_TO_HIPCCONNECTION (DWORD ec)
	{ return (ec == IPC_ERR_TIMEOUT) ? ( HIPCCONNECTION ) IPC_RC_TIMEOUT : ( HIPCCONNECTION ) IPC_RC_INVALID_HANDLE; }

////////////////////////////////////////////////////////////////
// IPC structures

#pragma pack(push,1)

// connection control config data
struct IPC_CONTROL_DATA
{
	HANDLE hClose;
};

// connection channel config data
struct IPC_CHANNEL_DATA
{
	HANDLE hSendRdy;  // 'ready to send' event
	HANDLE hSend;     // 'data sent' event
	HANDLE hRecv;     // 'ready to receive/data received' event
};

// port information
// handles must be duplicated to the client process
struct IPC_PORT_INFO
{
	DWORD  serverPid;
};

struct IPC_MSG_HDR
{
	DWORD msgSize;
	DWORD pktSize;
};

const DWORD IPC_MSG_INVALID       = 0xFFFFFFFF;

const DWORD IPC_MAX_PKT_SIZE = IPC_BUFFER_SIZE - sizeof (IPC_MSG_HDR);

// connection establishment request
// handles are already duplicated to the server process
struct IPC_CONNECT_REQUEST
{
	DWORD  clientPid;
	char   connName [80];  // UUID
};

struct IPC_CONNECT_REPLY
{
	DWORD  status; // zero - OK, nonzero - error
};

#pragma pack(pop)

///////////////////////////////////////////////////////////////
// utility class: handle holder

class Handle
{
public:
		Handle (HANDLE h=0) : m_handle (h) {}
		~Handle () { close (); }

	Handle& operator= (HANDLE h)
		{ close (); m_handle = h; return *this; }

	HANDLE detach ()
		{ HANDLE h = m_handle; m_handle = 0; return h; }

	bool copyFrom (HANDLE hSrcProcess, HANDLE hSrcHandle)
		{ close(); return DuplicateHandle (hSrcProcess, hSrcHandle,
			GetCurrentProcess (), &m_handle, 0, FALSE, DUPLICATE_SAME_ACCESS) != 0; }

	bool copyTo (HANDLE hDstProcess, HANDLE *phDstHandle)
		{ return DuplicateHandle (GetCurrentProcess(), m_handle, 
			hDstProcess, phDstHandle, 0, FALSE, DUPLICATE_SAME_ACCESS) != 0; }

	void close ()
		{ if (isValid ()) CloseHandle (m_handle); m_handle = 0; }

	operator HANDLE () const { return m_handle; }

	bool isValid () const    { return m_handle != 0 && m_handle != INVALID_HANDLE_VALUE; }

	HANDLE * PPHandle ()     { return &m_handle; }

private:
	HANDLE m_handle;

	Handle (const Handle&);
	Handle& operator= (const Handle&);
};

///////////////////////////////////////////////////////////////
// utlilty class: mapped buffer holder

class MapView
{
public:
		MapView (void *buf=NULL) : m_buf (buf) {}
		~MapView () { close (); }

	MapView& operator= (void *buf)
		{ close (); m_buf = buf; return *this; }

	void close ()
		{ if (m_buf != NULL) UnmapViewOfFile (m_buf); m_buf = NULL; }

	unsigned char * data () const      { return (unsigned char *)m_buf; }  
	operator void * () const  { return m_buf; }

	bool isValid () const	{ return m_buf != NULL; }

private:
	void * m_buf;

	MapView (const MapView&);
	MapView& operator= (const MapView&);
};

////////////////////////////////////////////////////////////////
// Utility structures

inline unsigned int IPC_strlen (const char *s)
	{ return s==NULL ? 0 : strlen(s); }

// IPC connection control
struct IPC_Control
{
	Handle m_hClose;

	IPC_Control ()  {}
	~IPC_Control () { close (); }

	// pathBuf contains prefix of length prefixLen
	bool create (SECURITY_ATTRIBUTES *pSA, char *pathBuf, unsigned int prefixLen);
	bool open (char *pathBuf, unsigned int prefixLen);

	void signalClose (); // set hClose
	void close ();
};

// IPC channel control
struct IPC_Channel
{
	Handle m_hMutex;        // (local) access mutex
	Handle m_hSendRdy;      // 'ready to send' event
	Handle m_hSend;         // 'data sent' event
	Handle m_hRecv;         // 'ready to receive/data received' event
	unsigned char * m_buffer;
	DWORD  m_bufSize;

	IPC_Channel () : m_buffer(NULL), m_bufSize (0) {}
	~IPC_Channel ()  { close (); }

	// pathBuf contains prefix of length prefixLen
	bool create (SECURITY_ATTRIBUTES *pSA, char *pathBuf, unsigned int prefixLen);
	bool open (char *pathBuf, unsigned int prefixLen);

	void setBuffer (void *buf)
		{ m_buffer = (unsigned char *)buf; }

	void close ();

	DWORD lock (DWORD tmo);  // returns IPC_ERR_XXXX
	void  unlock ();
};

// IPC channel locker with auto-unlock
class IPC_Channel_Lock
{
public:
	IPC_Channel_Lock () : m_lock (NULL) {}
	~IPC_Channel_Lock () { unlock (); }

	DWORD lock (IPC_Channel *pChan, DWORD tmo) {
			DWORD st = pChan->lock (tmo);
			if (st == 0) m_lock = pChan;
			return st;
		}

	void unlock () {
			if (m_lock) { m_lock->unlock (); m_lock = NULL; }
		}

private:
	IPC_Channel *m_lock;

	IPC_Channel_Lock (const IPC_Channel_Lock&);
	IPC_Channel_Lock& operator= (const IPC_Channel_Lock&);
};

////////////////////////////////////////////////////////////////
// Port access lock (event+mutex pair)

class IPC_PortAccess
{
public:
	IPC_PortAccess () {}
	~IPC_PortAccess () { close (); }

	bool create (SECURITY_ATTRIBUTES *pSA, char *pathBuf, unsigned int prefixLen);
	void close ()
		{ m_hAvail.close (); m_hMutex.close (); }

	void allowAccess ()  { SetEvent (m_hAvail); }
	void blockAccess ()  { ResetEvent (m_hAvail); }

	// returns WAIT_OBJECT_0 (success)
	// WAIT_TIMEOUT or other error instead
	DWORD waitAccess (DWORD tmo);

	bool isValid () const
		{ return m_hAvail.isValid () && m_hMutex.isValid (); }

private:
	Handle m_hAvail;  // 'port available' event
	Handle m_hMutex;  // access mutex

	friend class IPC_PortAccessHolder;
};

class IPC_PortAccessHolder
{
public:
	IPC_PortAccessHolder (IPC_PortAccess& acc) : m_acc (acc) {}
	~IPC_PortAccessHolder () { ReleaseMutex (m_acc.m_hMutex); }

private:
	IPC_PortAccess& m_acc;
};

////////////////////////////////////////////////////////////////
// Server object

class IPC_Server
{
public:
	IPC_Server ();
	~IPC_Server ();

	DWORD listen (const char *epName);
	BOOL  unlisten ();

	IPC_Connection * accept (DWORD tmo, DWORD& err, HANDLE hBreakEvent = NULL);

private:
	IPC_PortAccess m_portAccess; // port access
	Handle      m_hBuffer;     // IPC buffer for port info and connection channel
	MapView     m_buffer;
	IPC_Control m_control;     // connection control
	IPC_Channel m_channel;     // send/receive channel for connection requests

	IPC_Server (const IPC_Connection&);
	IPC_Server& operator= (const IPC_Connection&);

	void waitForOperationsComplete(DWORD tmo = INFINITE);
};

////////////////////////////////////////////////////////////////
// Connection object

class IPC_Connection
{
public:
	IPC_Connection ();
	~IPC_Connection ();

	// returns IPC_ERR_XXX
	DWORD connect (const char *epName, DWORD tmo);
	BOOL close ();

	// returns IPC_ERR_XXX
	DWORD send (const void *buf, DWORD bufSize, DWORD tmo);
	DWORD recv (void *buf, DWORD bufSize, DWORD tmo, DWORD& rsz);

	BOOL setUserEvent (HANDLE hEvent);
	BOOL getUserEvent (HANDLE *phEvent);
	BOOL resetUserEvent ();

	DWORD getLastError () const { return m_lastError; }

	bool initClientSide (IPC_CONNECT_REQUEST *connData);
	bool initServerSide (const IPC_CONNECT_REQUEST *connData);

private:
	Handle      m_hProcess;    // handle to the peer process
	Handle      m_hBuffer;     // handle to the shared memory object
	MapView     m_buffer;
	IPC_Control m_control;     // connection control
	IPC_Channel m_sendChannel; // send channel
	IPC_Channel m_recvChannel; // receive channel
	HANDLE      m_hUserEvent;  // user event object

	DWORD m_lastError;

	void clearLastError ()
		{ m_lastError = 0; }

	DWORD setLastError (DWORD ec)
		{ m_lastError = ec; return ec; }

	void waitForOperationsComplete(DWORD tmo = INFINITE);

	IPC_Connection (const IPC_Connection&);
	IPC_Connection& operator= (const IPC_Connection&);
};

////////////////////////////////////////////////////////////////
// Global IPC runtime object

class IPC_Runtime
{
public:
	~IPC_Runtime ();

	bool isValid () const
		{ return m_bInitOK; }

	DWORD getVersion ();

	HIPCSERVER serverStart (const char *epName);
	BOOL serverStop (HIPCSERVER hServer);

	HIPCCONNECTION serverWaitForConnection (HIPCSERVER hServer, DWORD tmo, HANDLE hBreakEvent = NULL);
	HIPCCONNECTION connect (const char *epName, DWORD tmo);
	BOOL closeConnection (HIPCCONNECTION hConn);

	DWORD send (HIPCCONNECTION hConn, const void *buf, DWORD bufSize, DWORD tmo);
	DWORD recv (HIPCCONNECTION hConn, void *buf, DWORD bufSize, DWORD tmo);

	BOOL setUserEvent (HIPCCONNECTION hConn, HANDLE hUserEvent);
	BOOL getUserEvent (HIPCCONNECTION hConn, HANDLE *phUserEvent);
	BOOL resetUserEvent (HIPCCONNECTION hConnection);

	DWORD getConnectionLastErr (HIPCCONNECTION hConn);

	static inline IPC_Runtime& instance () { return g_instance; }

	// generate unique connection name (buffer size must be at least IPC_MAX_SERVER_NAME)
	// returns name length (0 on error)
	bool generateConnectionName (char *nameBuf);

	// pathBuf size must be at least IPC_MAX_PATH
	// return path length
	unsigned int formatObjectPath (char *pathBuf, const char *prefix, const char *name);

	SECURITY_ATTRIBUTES * getSecurityAttributes ()	{ return m_pSA; }

private:
	IPC_Runtime ();
	IPC_Runtime (const IPC_Runtime&);
	IPC_Runtime& operator= (const IPC_Runtime&);

	bool          m_bInitOK;
	OSVERSIONINFO m_osVersion;

	HSA           m_hSA;
	SECURITY_ATTRIBUTES *m_pSA;

	CRITICAL_SECTION m_postInitCSect;
	bool          m_bPostInitDone;
	bool          m_bPostInitOK;

	bool  checkPostInit ();

	// server handle management
	static IPC_Server * getServer (HIPCSERVER h)
		{ return (( unsigned long )h==IPC_RC_ERROR || ( unsigned long )h==IPC_RC_TIMEOUT) ? NULL : (IPC_Server *)h; }

	static HIPCSERVER registerServer (IPC_Server *pServer)
		{ return (HIPCSERVER)pServer; }

	static IPC_Server *unregisterServer (HIPCSERVER h)
		{ return (( unsigned long )h==IPC_RC_ERROR || ( unsigned long )h==IPC_RC_TIMEOUT) ? NULL : (IPC_Server *)h; }

	// connection handle management
	static IPC_Connection * getConnection (HIPCCONNECTION h)
		{ return (( unsigned long )h==IPC_RC_ERROR || ( unsigned long )h==IPC_RC_TIMEOUT) ? NULL : (IPC_Connection *)h; }

	static HIPCCONNECTION registerConnection (IPC_Connection *pConn)
		{ return (HIPCCONNECTION)pConn; }

	static IPC_Connection *unregisterConnection (HIPCCONNECTION h)
		{ return (( unsigned long )h==IPC_RC_ERROR || ( unsigned long )h==IPC_RC_TIMEOUT) ? NULL : (IPC_Connection *)h; }

 // single instance of the runtime
 static IPC_Runtime g_instance;
};

////////////////////////////////////////////////////////////////
// inline methods

inline DWORD IPC_Runtime::send (HIPCCONNECTION hConn, const void *buf, DWORD bufSize, DWORD tmo)
{
	IPC_Connection *conn = getConnection (hConn);
	if (conn == NULL) return IPC_ERR_TO_RC (IPC_ERR_INVALID_ARG);

	DWORD err = conn->send (buf, bufSize, tmo);
	return (err == 0) ? bufSize : IPC_ERR_TO_RC (err);
}

inline DWORD IPC_Runtime::recv (HIPCCONNECTION hConn, void *buf, DWORD bufSize, DWORD tmo)
{
	IPC_Connection *conn = getConnection (hConn);
	if (conn == NULL) return IPC_ERR_TO_RC (IPC_ERR_INVALID_ARG);

	DWORD rsz = 0;
	DWORD err = conn->recv (buf, bufSize, tmo, rsz);
	return (err == 0) ? rsz : IPC_ERR_TO_RC (err);
}

#endif // _ipc_impl_h_INCLUDED_



