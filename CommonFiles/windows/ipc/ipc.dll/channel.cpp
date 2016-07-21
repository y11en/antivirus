// channel.h
//
// Interprocess communication library (IPC)
//
// IPC channel implementation
//
// (C) 2003 Kaspersky Labs
//
// Author: Vyacheslav A. Batenin (Vyacheslav.Batenin@kaspersky.com)
//

#include "ipc_impl.h"

////////////////////////////////////////////////////////////////
// IPC_Port_Access

bool IPC_PortAccess::create (SECURITY_ATTRIBUTES *pSA, char *pathBuf, unsigned int prefixLen)
{
	strcpy (pathBuf+prefixLen, IPC_SUFFIX_AVAIL);
	m_hAvail = CreateEvent (pSA, TRUE, FALSE, pathBuf);
	if (! m_hAvail.isValid ()) return false;

	strcpy (pathBuf+prefixLen, IPC_SUFFIX_MUTEX);
	m_hMutex = CreateMutex (pSA, FALSE, pathBuf);
	if (! m_hMutex.isValid ()) return false;

	return true;
}

DWORD IPC_PortAccess::waitAccess (DWORD tmo)
{
	HANDLE hdls[2] = { m_hAvail, m_hMutex };
	DWORD st = WaitForMultipleObjects (2, hdls, TRUE, tmo);
	switch (st) {
	case WAIT_OBJECT_0:
	case WAIT_OBJECT_0+1:
	case WAIT_ABANDONED_0:
	case WAIT_ABANDONED_0+1:
		st = WAIT_OBJECT_0;
		break;
	}
	return st;
}

////////////////////////////////////////////////////////////////
// IPC_Control

bool IPC_Control::create (SECURITY_ATTRIBUTES *pSA, char *pathBuf, unsigned int prefixLen)
{
	strcpy (pathBuf+prefixLen, IPC_SUFFIX_CLOSE);
	m_hClose = CreateEvent (pSA, TRUE, FALSE, pathBuf);
	if (! m_hClose.isValid ()) return false;

	if (GetLastError () == ERROR_ALREADY_EXISTS) {
		m_hClose.close ();
		return false;
	}
	return true;
}

bool IPC_Control::open (char *pathBuf, unsigned int prefixLen)
{
	strcpy (pathBuf+prefixLen, IPC_SUFFIX_CLOSE);
	m_hClose = OpenEvent (SYNCHRONIZE|EVENT_MODIFY_STATE, FALSE, pathBuf);
	return m_hClose.isValid ();
}

void IPC_Control::signalClose ()
{
	if (m_hClose.isValid ()) SetEvent (m_hClose);
}

void IPC_Control::close ()
{
	m_hClose.close ();
}

////////////////////////////////////////////////////////////////
// IPC_Channel

bool IPC_Channel::create (SECURITY_ATTRIBUTES *pSA, char *pathBuf, unsigned int prefixLen)
{
	m_hMutex = CreateMutex (NULL, FALSE, NULL);
	if (! m_hMutex.isValid ()) return false;

	strcpy (pathBuf+prefixLen, IPC_SUFFIX_SENDRDY);
	m_hSendRdy = CreateEvent (pSA, FALSE, FALSE, pathBuf);
	if (! m_hSendRdy.isValid () || GetLastError () == ERROR_ALREADY_EXISTS) return false;

	strcpy (pathBuf+prefixLen, IPC_SUFFIX_SEND);
	m_hSend = CreateEvent (pSA, FALSE, FALSE, pathBuf);
	if (! m_hSend.isValid () || GetLastError () == ERROR_ALREADY_EXISTS) return false;

	strcpy (pathBuf+prefixLen, IPC_SUFFIX_RECV);
	m_hRecv = CreateEvent (pSA, FALSE, FALSE, pathBuf);
	if (! m_hRecv.isValid () || GetLastError () == ERROR_ALREADY_EXISTS) return false;

	return true;
}

bool IPC_Channel::open (char *pathBuf, unsigned int prefixLen)
{
	m_hMutex = CreateMutex (NULL, FALSE, NULL);
	if (! m_hMutex.isValid ()) return false;

	strcpy (pathBuf+prefixLen, IPC_SUFFIX_SENDRDY);
	m_hSendRdy = OpenEvent (SYNCHRONIZE|EVENT_MODIFY_STATE, FALSE, pathBuf);
	if (! m_hSendRdy) return false;

	strcpy (pathBuf+prefixLen, IPC_SUFFIX_SEND);
	m_hSend = OpenEvent (SYNCHRONIZE|EVENT_MODIFY_STATE, FALSE, pathBuf);
	if (! m_hSend) return false;

	strcpy (pathBuf+prefixLen, IPC_SUFFIX_RECV);
	m_hRecv = OpenEvent (SYNCHRONIZE|EVENT_MODIFY_STATE, FALSE, pathBuf);
	if (! m_hRecv) return false;

	return true;
}

void IPC_Channel::close ()
{
	m_hSendRdy.close ();
	m_hSend.close ();
	m_hRecv.close ();
	m_hMutex.close ();
}

DWORD IPC_Channel::lock (DWORD tmo)
{
	DWORD st = WaitForSingleObject (m_hMutex, tmo);
	switch (st) {
	case WAIT_OBJECT_0:
	case WAIT_ABANDONED_0:
		return 0;
	case WAIT_TIMEOUT:
		return IPC_ERR_TIMEOUT;
	default:
		return IPC_ERR_UNKNOWN;
	}
}

void  IPC_Channel::unlock ()
{
	ReleaseMutex (m_hMutex);
}

////////////////////////////////////////////////////////////////
// IPC_Server

IPC_Server::IPC_Server ()
{
}

IPC_Server::~IPC_Server ()
{
	unlisten ();
}

DWORD IPC_Server::listen (const char *epName)
{
	unsigned int nameLen = IPC_strlen (epName);
	if (nameLen == 0 || nameLen > IPC_MAX_SERVER_NAME) return IPC_ERR_INVALID_ARG;

	char pathBuf[IPC_MAX_PATH];
	unsigned int prefixLen = IPC_Runtime::instance ().formatObjectPath (pathBuf, IPC_PORT_PREFIX, epName);

	// no synchronization here, because this method
	// is called only by runtime during object creation
	SECURITY_ATTRIBUTES *pSA = IPC_Runtime::instance ().getSecurityAttributes ();
	
	// create shared semaphore
	if (! m_portAccess.create (pSA, pathBuf, prefixLen)) return IPC_ERR_UNKNOWN;

	// create shared memory buffer in exclusive mode
	strcpy (pathBuf + prefixLen, IPC_SUFFIX_BUF);
	m_hBuffer = CreateFileMapping (INVALID_HANDLE_VALUE, pSA, PAGE_READWRITE, 0, IPC_BUFFER_SIZE, pathBuf);

	DWORD ec = GetLastError ();
	if (! m_hBuffer.isValid () || ec == ERROR_ALREADY_EXISTS) return IPC_ERR_UNKNOWN;

	// map buffer data
	m_buffer = MapViewOfFile (m_hBuffer, FILE_MAP_WRITE, 0, 0, 0);
	if (! m_buffer.isValid ()) return IPC_RC_ERROR;

	// create connection state
	if (! m_control.create (pSA, pathBuf, prefixLen)) return IPC_ERR_UNKNOWN;

	// create connection channel
	
	// channel buffer points just after the IPC_PORT_INFO structure
	IPC_PORT_INFO *portInfo = (IPC_PORT_INFO *)m_buffer.data ();

	if (! m_channel.create (pSA, pathBuf, prefixLen)) return IPC_ERR_UNKNOWN;
	m_channel.setBuffer (m_buffer.data() + sizeof(IPC_PORT_INFO));

	portInfo->serverPid = GetCurrentProcessId ();

	// open the port for external access
	m_portAccess.allowAccess ();

	return 0;
}

BOOL IPC_Server::unlisten ()
{
	if (! m_portAccess.isValid ()) return FALSE;

	// close control channel (sets hClose)
	m_control.signalClose ();

	waitForOperationsComplete();

	m_control.close ();
	m_channel.close ();
	m_buffer.close ();
	m_hBuffer.close ();

	// close the semaphore
	m_portAccess.blockAccess ();
	m_portAccess.close ();

	return TRUE;
}

void IPC_Server::waitForOperationsComplete(DWORD tmo /*= INFINITE*/)
{
	// wait for completting of a current channel operaton
	IPC_Channel_Lock locker;
	locker.lock(&m_channel, tmo);
}

IPC_Connection * IPC_Server::accept (DWORD tmo, DWORD& err, HANDLE hBreakEvent /*= NULL*/)
{
	if (! IsValidTimeout (tmo)) { err = IPC_ERR_INVALID_ARG; return NULL; }
	err = 0;

	DWORD t0;
	if (tmo != INFINITE && tmo != 0) t0 = GetTickCount ();

	IPC_Channel_Lock locker;
	err = locker.lock (&m_channel, tmo);
	if (err != 0) return NULL; // timeout or error

	// rendezvous handles:
	// 0 - primary handle
	// 1 - hClose
	int hcnt = 2;
	HANDLE hdls[3];
	hdls[1] = m_control.m_hClose;
	if (hBreakEvent)
		hdls[hcnt++] = hBreakEvent;

	IPC_MSG_HDR *msgHdr = (IPC_MSG_HDR *)(m_channel.m_buffer);

	// perform rendezvous
	for (;;) {

		DWORD rtmo = tmo;
		if (rtmo != INFINITE && rtmo != 0) {
			DWORD dt = GetTickCount () - t0;
			if (dt < rtmo) rtmo -= dt; else rtmo = 0;
		}

		hdls[0] = m_channel.m_hSendRdy;
		DWORD st = WaitForMultipleObjects (hcnt, hdls, FALSE, rtmo);
		switch (st) {
		case WAIT_OBJECT_0: break;
		case WAIT_OBJECT_0+1:
			err = IPC_ERR_CLOSED;
			return NULL;
		case WAIT_OBJECT_0+2:
			err = IPC_ERR_USER_EVENT_SET;
			return NULL;
		case WAIT_TIMEOUT:
			err = IPC_ERR_TIMEOUT;
			return NULL;
		default:
			err = IPC_ERR_UNKNOWN;
			return NULL;
		}

		SetEvent (m_channel.m_hRecv);

		hdls[0] = m_channel.m_hSend;

		st = WaitForMultipleObjects (hcnt, hdls, FALSE, INFINITE);
		switch (st) {
		case WAIT_OBJECT_0: break;
		default:
			ResetEvent (m_channel.m_hRecv);
			err = IPC_ERR_UNKNOWN;
			return NULL;
		}

		if (msgHdr->msgSize != IPC_MSG_INVALID) break;
	}

	// data received
	// accept connection request packet
	IPC_CONNECT_REQUEST *connReq = (IPC_CONNECT_REQUEST *)(msgHdr+1);
	// zero-terminate name, just in case...
	connReq->connName[sizeof(connReq->connName)-1] = 0;

	IPC_CONNECT_REPLY *connRep = (IPC_CONNECT_REPLY *)(msgHdr+1);

	// initialize connection
	IPC_Connection *pConn = new IPC_Connection ();
	if (pConn != NULL) {
		if (pConn->initServerSide (connReq)) {
			err = 0;
		} else {
			delete pConn;
			pConn = NULL;
			err = IPC_ERR_UNKNOWN;
		}
	} else {
		err = IPC_ERR_OUT_OF_MEMORY;
	}

	msgHdr->msgSize = sizeof (IPC_CONNECT_REPLY);
	msgHdr->pktSize = sizeof (IPC_CONNECT_REPLY);
	connRep->status = err;

	SetEvent (m_channel.m_hRecv);
	return pConn;
}

////////////////////////////////////////////////////////////////
// IPC_Connection

IPC_Connection::IPC_Connection () : m_hUserEvent (0)
{
	clearLastError ();
}

IPC_Connection::~IPC_Connection ()
{
	close ();
}

DWORD IPC_Connection::connect (const char *epName, DWORD tmo)
{
	// no synchronization here, sorry...
	clearLastError ();

	unsigned int nameLen = IPC_strlen (epName);
	if (nameLen == 0 || nameLen > IPC_MAX_SERVER_NAME) return setLastError (IPC_ERR_INVALID_ARG);

	if (! IsValidTimeout (tmo)) return setLastError (IPC_ERR_INVALID_ARG);

	DWORD t0;
	if (tmo != INFINITE && tmo != 0) t0 = GetTickCount ();

	char pathBuf[IPC_MAX_PATH];
	unsigned int prefixLen = IPC_Runtime::instance ().formatObjectPath (pathBuf, IPC_PORT_PREFIX, epName);

	SECURITY_ATTRIBUTES *pSA = IPC_Runtime::instance ().getSecurityAttributes ();

	IPC_PortAccess portAccess;
	if (! portAccess.create (pSA, pathBuf, prefixLen)) return false;

	DWORD st = portAccess.waitAccess (tmo);
	if (st == WAIT_TIMEOUT) return setLastError (IPC_ERR_TIMEOUT);
	if (st != WAIT_OBJECT_0) return setLastError (IPC_ERR_UNKNOWN);

	IPC_PortAccessHolder accessHolder (portAccess);

	// mutex locked, get access to the shared memory buffer
	strcpy (pathBuf + prefixLen, IPC_SUFFIX_BUF);
	Handle hBuffer = OpenFileMapping (FILE_MAP_WRITE, FALSE, pathBuf);
	if (! hBuffer.isValid ()) return setLastError (IPC_ERR_UNKNOWN);

	MapView buffer = MapViewOfFile (hBuffer, FILE_MAP_WRITE, 0, 0, 0);
	if (! buffer.isValid ()) return setLastError (IPC_ERR_UNKNOWN);

	IPC_PORT_INFO *portInfo = (IPC_PORT_INFO *)buffer.data ();

	m_hProcess = OpenProcess (SYNCHRONIZE, FALSE, portInfo->serverPid);
	if (! m_hProcess.isValid ()) return setLastError (IPC_ERR_UNKNOWN);

	IPC_Control control;
	IPC_Channel channel;

	if (! control.open (pathBuf, prefixLen)) return setLastError (IPC_ERR_UNKNOWN);

	if (! channel.open (pathBuf, prefixLen)) return setLastError (IPC_ERR_UNKNOWN);
	channel.setBuffer (buffer.data() + sizeof (IPC_PORT_INFO));

	// perform rendezvous

	// rendezvous handles:
	// 0 - primary handle
	// 1 - hClose
	// 2 - hServerProcess
	int hcnt = 3;
	HANDLE hdls[3];
	hdls[0] = channel.m_hRecv;
	hdls[1] = control.m_hClose;
	hdls[2] = m_hProcess;

	DWORD rtmo = tmo;
	if (rtmo != INFINITE && rtmo != 0) {
		DWORD dt = GetTickCount () - t0;
		if (dt < rtmo) rtmo -= dt; else rtmo = 0;
	}

	SetEvent (channel.m_hSendRdy);

	DWORD err = 0;
	st = WaitForMultipleObjects (hcnt, hdls, FALSE, rtmo);
	switch (st) {
	case WAIT_OBJECT_0: break;
	case WAIT_OBJECT_0+1: // m_hClose
	case WAIT_OBJECT_0+2: // m_hProcess
		return setLastError (IPC_ERR_UNKNOWN);
	case WAIT_TIMEOUT:
		err = IPC_ERR_TIMEOUT;
		break;
	default:
		err = IPC_ERR_UNKNOWN;
		break;
	}

	IPC_MSG_HDR *msgHdr = (IPC_MSG_HDR *)(channel.m_buffer);

	if (st != WAIT_OBJECT_0) {
		// timeout/error handling
		// try to 'atomically' drop SendRdy event
		st = WaitForSingleObject (channel.m_hSendRdy, 0);
		if (st == WAIT_TIMEOUT) {
			// receiver already synchronized on the object
			// discard message
			msgHdr->msgSize = IPC_MSG_INVALID;
			msgHdr->pktSize = 0;
			SetEvent (channel.m_hSend);
			// synchronize on hRecv+hClose (to clear hRecv), ignore result
			WaitForMultipleObjects (hcnt, hdls, FALSE, INFINITE);
		}
		return setLastError (err);
	}

	// create connection
	IPC_CONNECT_REQUEST *request = (IPC_CONNECT_REQUEST *)(msgHdr+1);

	if (! initClientSide (request)) {
		// error creating/duplicating connection handles
		msgHdr->msgSize = IPC_MSG_INVALID;
		msgHdr->pktSize = 0;
		SetEvent (channel.m_hSend);

		return setLastError (IPC_ERR_UNKNOWN);
	}

	msgHdr->msgSize = sizeof(IPC_CONNECT_REQUEST);
	msgHdr->pktSize = sizeof(IPC_CONNECT_REQUEST);
	SetEvent (channel.m_hSend);

	// synchronize on hRecv+hClose
	st = WaitForMultipleObjects (hcnt, hdls, FALSE, INFINITE);
	switch (st) {
	case WAIT_OBJECT_0: break;
	default:
		return setLastError (IPC_ERR_UNKNOWN);
	}

	// check reply
	IPC_CONNECT_REPLY *reply = (IPC_CONNECT_REPLY *)(msgHdr+1);
	if (reply->status != 0) {
		return setLastError (reply->status);
	}

	return 0;
}

bool IPC_Connection::initClientSide (IPC_CONNECT_REQUEST *connData)
{
	// create IPC buffer size twice of IPC_BUFFER_SIZE:
	// first half:  client->server channel
	// second half: server->client channel
	SECURITY_ATTRIBUTES *pSA = IPC_Runtime::instance ().getSecurityAttributes ();

	connData->clientPid = GetCurrentProcessId ();
	if (! IPC_Runtime::instance ().generateConnectionName (connData->connName)) return false;

	char pathBuf[IPC_MAX_PATH];
	unsigned int prefixLen = IPC_Runtime::instance ().formatObjectPath (pathBuf, IPC_CONN_PREFIX, connData->connName);

	strcpy (pathBuf + prefixLen, IPC_SUFFIX_BUF);	
	m_hBuffer = CreateFileMapping (INVALID_HANDLE_VALUE, pSA, PAGE_READWRITE, 0, IPC_BUFFER_SIZE*2, pathBuf);
	if (! m_hBuffer.isValid () || GetLastError () == ERROR_ALREADY_EXISTS) return false;

	m_buffer = MapViewOfFile (m_hBuffer, FILE_MAP_WRITE, 0, 0, 0);
	if (! m_buffer.isValid ()) return false;

	if (! m_control.create (pSA, pathBuf, prefixLen)) return false;

	strcpy (pathBuf + prefixLen, IPC_SUFFIX_CLIENT);
	unsigned int suffixLen = strlen (pathBuf + prefixLen);
	if (! m_sendChannel.create (pSA, pathBuf, prefixLen + suffixLen)) return false;
	m_sendChannel.setBuffer (m_buffer.data ());

	strcpy (pathBuf + prefixLen, IPC_SUFFIX_SERVER);
	suffixLen = strlen (pathBuf + prefixLen);
	if (! m_recvChannel.create (pSA, pathBuf, prefixLen + suffixLen)) return false;
	m_recvChannel.setBuffer (m_buffer.data () + IPC_BUFFER_SIZE);

	return true;
}

bool IPC_Connection::initServerSide (const IPC_CONNECT_REQUEST *connData)
{
	m_hProcess = OpenProcess (SYNCHRONIZE, FALSE, connData->clientPid);
	if (! m_hProcess.isValid ()) {
		OutputDebugString ("KL_IPC: OpenProcess() failed\n");
		return false;
	}

	char pathBuf[IPC_MAX_PATH];
	unsigned int prefixLen = IPC_Runtime::instance ().formatObjectPath (pathBuf, IPC_CONN_PREFIX, connData->connName);

	strcpy (pathBuf+prefixLen, IPC_SUFFIX_BUF);
	m_hBuffer = OpenFileMapping (FILE_MAP_WRITE, FALSE, pathBuf);
	if (! m_hBuffer.isValid ()) return false;

	m_buffer = MapViewOfFile (m_hBuffer, FILE_MAP_WRITE, 0, 0, 0);
	if (! m_buffer.isValid ()) return false;

	if (! m_control.open (pathBuf, prefixLen)) return false;

	strcpy (pathBuf+prefixLen, IPC_SUFFIX_CLIENT);
	unsigned int suffixLen = strlen (pathBuf+prefixLen);
	if (! m_recvChannel.open (pathBuf, prefixLen + suffixLen)) return false;
	m_recvChannel.setBuffer (m_buffer.data ());

	strcpy (pathBuf+prefixLen, IPC_SUFFIX_SERVER);
	suffixLen = strlen (pathBuf+prefixLen);
	if (! m_sendChannel.open (pathBuf, prefixLen + suffixLen)) return false;
	m_sendChannel.setBuffer (m_buffer.data () + IPC_BUFFER_SIZE);

	return true;
}

BOOL IPC_Connection::close ()
{
	if (! m_hBuffer.isValid ()) return FALSE;

	m_control.signalClose ();

	waitForOperationsComplete();

	m_control.close ();
	m_sendChannel.close ();
	m_recvChannel.close ();
	m_buffer.close ();
	m_hBuffer.close ();
	m_hProcess.close ();
	m_hUserEvent = 0;

	return TRUE;
}

void IPC_Connection::waitForOperationsComplete(DWORD tmo /*= INFINITE*/)
{
	IPC_Channel_Lock send_locker, recv_locker;
	send_locker.lock(&m_sendChannel, tmo);
	recv_locker.lock(&m_recvChannel, tmo);
}

DWORD IPC_Connection::send (const void *buf, DWORD bufSize, DWORD tmo)
{
	clearLastError ();
	if (bufSize >= IPC_MSG_SIZE_LIMIT || ! IsValidTimeout (tmo)) return setLastError (IPC_ERR_INVALID_ARG);

	const unsigned char *udata = (const unsigned char *)buf;

	// lock connection object
	DWORD t0;
	if (tmo != INFINITE && tmo != 0) t0 = GetTickCount ();

	IPC_Channel_Lock locker;
	DWORD err = locker.lock (&m_sendChannel, tmo);
	if (err != 0) return setLastError (err); // timeout or error

	// perform rendezvous

	// rendezvous handles:
	// 0 - primary handle
	// 1 - hClose
	// 2 - server process
	// 3 - user handle (optional)
	int hcnt = 3;
	HANDLE hdls[4];
	hdls[0] = m_sendChannel.m_hRecv;
	hdls[1] = m_control.m_hClose;
	hdls[2] = m_hProcess;
	if (m_hUserEvent != 0) hdls[hcnt++] = m_hUserEvent;

	DWORD rtmo = tmo;
	if (rtmo != INFINITE && rtmo != 0) {
		DWORD dt = GetTickCount () - t0;
		if (dt < rtmo) rtmo -= dt; else rtmo = 0;
	}

	SetEvent (m_sendChannel.m_hSendRdy);

	err = 0;
	DWORD st = WaitForMultipleObjects (hcnt, hdls, FALSE, rtmo);
	switch (st) {
	case WAIT_OBJECT_0: break;
	case WAIT_OBJECT_0+1: // hClose
		return setLastError (IPC_ERR_CLOSED);
	case WAIT_OBJECT_0+2: // hProcess
		return setLastError (IPC_ERR_BROKEN);
	case WAIT_OBJECT_0+3: // hUserEvent
		err = IPC_ERR_USER_EVENT_SET;
		break;
	case WAIT_TIMEOUT:
		err = IPC_ERR_TIMEOUT;
		break;
	default:
		err = IPC_ERR_UNKNOWN;
		break;
	}

	IPC_MSG_HDR *msgHdr = (IPC_MSG_HDR *)(m_sendChannel.m_buffer);

	if (st != WAIT_OBJECT_0) {
		// timeout/error handling
		// try to 'atomically' drop SendRdy event
		st = WaitForSingleObject (m_sendChannel.m_hSendRdy, 0);
		if (st == WAIT_TIMEOUT) {
			// receiver already synchronized on the object
			// discard message
			msgHdr->msgSize = IPC_MSG_INVALID;
			msgHdr->pktSize = 0;
			SetEvent (m_sendChannel.m_hSend);
			// synchronize on hRecv+hClose+hProcess (to clear hRecv), ignore result
			WaitForMultipleObjects (3, hdls, FALSE, INFINITE);
		}
		return setLastError (err);
	}

	// send packets
	void *databuf = msgHdr + 1;
	msgHdr->msgSize = bufSize;

	do {
		DWORD portion = IPC_BUFFER_SIZE - sizeof (IPC_MSG_HDR);
		if (portion > bufSize) portion = bufSize;

		msgHdr->pktSize = portion;
		memcpy (databuf, udata, portion);

		udata += portion;
		bufSize -= portion;

		SetEvent (m_sendChannel.m_hSend);

		// sync on hRecv+hClose+hProcess
		st = WaitForMultipleObjects (3, hdls, FALSE, INFINITE);
		switch (st) {
		case WAIT_OBJECT_0: break;
		case WAIT_OBJECT_0+1: // hClose
			return setLastError (IPC_ERR_CLOSED);
		case WAIT_OBJECT_0+2: // hProcess
			return setLastError (IPC_ERR_BROKEN);
		default:
			return setLastError (IPC_ERR_UNKNOWN);
		}

	} while (bufSize != 0);

	return 0;
}

DWORD IPC_Connection::recv (void *buf, DWORD bufSize, DWORD tmo, DWORD& rsz)
{
	clearLastError ();
	if (! IsValidTimeout (tmo)) return setLastError (IPC_ERR_INVALID_ARG);

	unsigned char *udata = (unsigned char *)buf;

	// lock connection object
	DWORD t0;
	if (tmo != INFINITE && tmo != 0) t0 = GetTickCount ();

	IPC_Channel_Lock locker;
	DWORD err = locker.lock (&m_recvChannel, tmo);
	if (err != 0) return setLastError (err); // timeout or error

	// perform rendezvous

	// rendezvous handles:
	// 0 - primary handle (m_hSendRdy or m_hSend)
	// 1 - hClose
	// 2 - server process
	// 3 - user handle (optional)
	int hcnt = 3;
	HANDLE hdls[4];
	hdls[1] = m_control.m_hClose;
	hdls[2] = m_hProcess;
	if (m_hUserEvent != 0) hdls[hcnt++] = m_hUserEvent;

	const IPC_MSG_HDR *msgHdr = (const IPC_MSG_HDR *)(m_recvChannel.m_buffer);

	// perform rendezvous
	for (;;) {
		DWORD rtmo = tmo;
		if (rtmo != INFINITE && rtmo != 0) {
			DWORD dt = GetTickCount () - t0;
			if (dt < rtmo) rtmo -= dt; else rtmo = 0;
		}

		hdls[0] = m_recvChannel.m_hSendRdy;
		DWORD st = WaitForMultipleObjects (hcnt, hdls, FALSE, rtmo);
		switch (st) {
		case WAIT_OBJECT_0: break;
		case WAIT_OBJECT_0+1: // hClose
			return setLastError (IPC_ERR_CLOSED);
		case WAIT_OBJECT_0+2: // hProcess
			return setLastError (IPC_ERR_BROKEN);
		case WAIT_OBJECT_0+3: // hUserEvent
			return setLastError (IPC_ERR_USER_EVENT_SET);
		case WAIT_TIMEOUT:
			return setLastError (IPC_ERR_TIMEOUT);
		default:
			return setLastError (IPC_ERR_UNKNOWN);
		}

		SetEvent (m_recvChannel.m_hRecv);

		hdls[0] = m_recvChannel.m_hSend;

		// sync on hSend, hClose, hProcess
		st = WaitForMultipleObjects (3, hdls, FALSE, INFINITE);
		switch (st) {
		case WAIT_OBJECT_0: break;
		default:
			ResetEvent (m_recvChannel.m_hRecv);
			return setLastError (IPC_ERR_UNKNOWN);
		}

		if (msgHdr->msgSize != IPC_MSG_INVALID) break;
	}

	// normal data packet received
	DWORD orgMsgSize = msgHdr->msgSize;
	DWORD msgSize = orgMsgSize;
	rsz = 0;

	const unsigned char *msgBuf = (const unsigned char *)(msgHdr + 1);

	for (;;) {
		DWORD pktSize = msgHdr->pktSize;
		if (pktSize > IPC_MAX_PKT_SIZE) pktSize = IPC_MAX_PKT_SIZE; // sender error !!!
		if (pktSize > msgSize) pktSize = msgSize;

		DWORD portion = pktSize;
		if (portion > bufSize) portion = bufSize;
		memcpy (udata, msgBuf, portion);

		bufSize -= portion;
		udata += portion;
		rsz += portion;

		SetEvent (m_recvChannel.m_hRecv);

		msgSize -= pktSize;
		if (msgSize == 0) break;

		// sync on hSend, hClose, hProcess
		DWORD st = WaitForMultipleObjects (3, hdls, FALSE, INFINITE);
		switch (st) {
		case WAIT_OBJECT_0: break;
		case WAIT_OBJECT_0+1: // hClose
			return setLastError (IPC_ERR_CLOSED);
		case WAIT_OBJECT_0+2: // hProcess
			return setLastError (IPC_ERR_BROKEN);
		default:
			return setLastError (IPC_ERR_UNKNOWN);
		}
	}

	if (rsz < orgMsgSize) return setLastError (IPC_ERR_UNKNOWN);
	return 0;
}

BOOL IPC_Connection::setUserEvent (HANDLE hEvent)
{
	clearLastError ();
	if (hEvent == 0) { setLastError (IPC_ERR_INVALID_ARG); return FALSE; }

	m_hUserEvent = hEvent;
	return TRUE;
}

BOOL IPC_Connection::getUserEvent (HANDLE *phEvent)
{
	clearLastError ();
	if (phEvent == NULL) { setLastError (IPC_ERR_INVALID_ARG); return FALSE; }

	*phEvent = m_hUserEvent;
	return TRUE;
}

BOOL IPC_Connection::resetUserEvent ()
{
	clearLastError ();
	if (m_hUserEvent == 0) { setLastError (IPC_ERR_UNKNOWN); return FALSE; }

	ResetEvent (m_hUserEvent);
	return TRUE;
}


