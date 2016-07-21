// runtime.h
//
// Interprocess communication library (IPC)
//
// IPC_Runtime implementation
//
// (C) 2003 Kaspersky Labs
//
// Author: Vyacheslav A. Batenin (Vyacheslav.Batenin@kaspersky.com)
//

#include "ipc_impl.h"
#include "version.h"

#include <rpc.h>
#include <aclapi.h>

////////////////////////////////////////////////////////////////

typedef DWORD (WINAPI *PFN_GetSecurityInfo)(HANDLE, SE_OBJECT_TYPE,
			SECURITY_INFORMATION, PSID*, PSID*, PACL*, PACL*, PSECURITY_DESCRIPTOR*);

typedef DWORD (WINAPI *PFN_SetSecurityInfo)(HANDLE, SE_OBJECT_TYPE,
			SECURITY_INFORMATION, PSID, PSID, PACL, PACL);

typedef DWORD (WINAPI *PFN_SetEntriesInAclA)(ULONG, PEXPLICIT_ACCESS_A, PACL, PACL*);

bool fixProcessDACL ()
{
	HMODULE hAdvApi = GetModuleHandle ("ADVAPI32.DLL");
	if (hAdvApi == NULL) return false;

	PFN_GetSecurityInfo pGetSecurityInfo = 
		(PFN_GetSecurityInfo)GetProcAddress (hAdvApi, "GetSecurityInfo");
	PFN_SetSecurityInfo pSetSecurityInfo = 
		(PFN_SetSecurityInfo)GetProcAddress (hAdvApi, "SetSecurityInfo");
	PFN_SetEntriesInAclA pSetEntriesInAclA = 
		(PFN_SetEntriesInAclA)GetProcAddress (hAdvApi, "SetEntriesInAclA");

	if (!pGetSecurityInfo || !pSetSecurityInfo || !pSetEntriesInAclA) return false;

	DWORD ec = 0;
	HANDLE hProcess = NULL;
	PACL pOldAcl = NULL;
	PACL pNewAcl = NULL;
	PSID sid = NULL;
	PSECURITY_DESCRIPTOR pSD = NULL;

	SID_IDENTIFIER_AUTHORITY auth = SECURITY_WORLD_SID_AUTHORITY;
	BOOL f = AllocateAndInitializeSid (&auth, 1, 0, 0, 0, 0, 0, 0, 0, 0, &sid);
	if (! f) return false;

	bool fOK = true;

	hProcess = OpenProcess (PROCESS_ALL_ACCESS, FALSE, GetCurrentProcessId ());
	if (hProcess == NULL) fOK = false;

	if (fOK) {
		ec = pGetSecurityInfo (hProcess, SE_KERNEL_OBJECT,
			DACL_SECURITY_INFORMATION, NULL, NULL, &pOldAcl, NULL, &pSD);
		if (ec != ERROR_SUCCESS) fOK = false;
	}

	// grant SYNCHRONIZE right to EVERYONE
	if (fOK) {
		EXPLICIT_ACCESS_A ace;
		memset (&ace, 0, sizeof (ace));

		ace.grfAccessPermissions = SYNCHRONIZE;
		ace.grfAccessMode = GRANT_ACCESS;
		ace.grfInheritance = NO_INHERITANCE;
		ace.Trustee.pMultipleTrustee = NULL;
		ace.Trustee.MultipleTrusteeOperation = NO_MULTIPLE_TRUSTEE;
		ace.Trustee.TrusteeForm = TRUSTEE_IS_SID;
		ace.Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
		ace.Trustee.ptstrName = (char *)sid;

		ec = pSetEntriesInAclA (1, &ace, pOldAcl, &pNewAcl);
		if (ec != ERROR_SUCCESS) fOK = false;
	}

	if (fOK) {
		ec = pSetSecurityInfo (hProcess, SE_KERNEL_OBJECT,
				DACL_SECURITY_INFORMATION, NULL, NULL, pNewAcl, NULL);
		if (ec != ERROR_SUCCESS) fOK = false;
	}

	if (pNewAcl != NULL) LocalFree (pNewAcl);
	if (pSD != NULL) LocalFree (pSD);	
	if (sid != NULL) FreeSid (sid);
	if (hProcess != NULL) CloseHandle (hProcess);

	if (! fOK) OutputDebugString ("KL_IPC: fixProcessDACL() failed\n");
	return fOK;
}

////////////////////////////////////////////////////////////////

IPC_Runtime IPC_Runtime::g_instance;

IPC_Runtime::IPC_Runtime () : m_bInitOK (false), m_hSA (0), m_pSA (NULL),
	m_bPostInitDone (false), m_bPostInitOK (false)
{
	InitializeCriticalSection (& m_postInitCSect);

	memset (&m_osVersion, 0, sizeof (m_osVersion));
	m_osVersion.dwOSVersionInfoSize = sizeof (m_osVersion);

	if (! GetVersionEx (& m_osVersion)) return;

	if (m_osVersion.dwPlatformId == VER_PLATFORM_WIN32_NT) {
		m_hSA = SA_Create (SECURITY_WORLD_RID, SA_ACCESS_MASK_ALL);
		if (m_hSA == 0) return;
		m_pSA = SA_Get (m_hSA);

//		if (! fixProcessDACL ()) return;
	}

	m_bInitOK = true;
}

IPC_Runtime::~IPC_Runtime ()
{
	if (m_hSA != 0) {
		SA_Destroy (m_hSA);
		m_hSA = 0;
		m_pSA = NULL;
	}

	DeleteCriticalSection (& m_postInitCSect);
}

bool IPC_Runtime::checkPostInit ()
{
	EnterCriticalSection (& m_postInitCSect);

	if (! m_bPostInitDone) {
		if (m_osVersion.dwPlatformId == VER_PLATFORM_WIN32_NT) {
			// The function fixProcessDACL fails mysteriously under some circumstances
			// when called from the global constructor.
			m_bPostInitOK = fixProcessDACL ();
		} else {
			m_bPostInitOK = true;
		}
		if (m_bPostInitOK) m_bPostInitDone = true;
	}

	const bool st = m_bPostInitOK;

	LeaveCriticalSection (& m_postInitCSect);
	return st;
}

DWORD IPC_Runtime::getVersion ()
{
	return (MODULE_VERSION_A << 24)
	     | (MODULE_VERSION_B << 16)
		 | (MODULE_VERSION_C << 8)
		 | MODULE_VERSION_D;
}

HIPCSERVER IPC_Runtime::serverStart (const char *epName)
{
	checkPostInit ();

	IPC_Server *pServer = new IPC_Server ();
	if (! pServer) return HIPCSERVER_INVALID;

	DWORD ec = pServer->listen (epName);
	if (ec != 0) {
		delete pServer;
		return IPC_ERR_TO_HIPCSERVER(ec);
	}
	
	HIPCSERVER h = registerServer (pServer);
	if (h == HIPCSERVER_INVALID) {
		// very unlikely
		delete pServer;
		return IPC_ERR_TO_HIPCSERVER(IPC_ERR_UNKNOWN);
	}

	return h;
}

BOOL IPC_Runtime::serverStop (HIPCSERVER hServer)
{
	IPC_Server *pServer = unregisterServer (hServer);
	if (! pServer) return FALSE;

	BOOL f = pServer->unlisten ();
	delete pServer;
	
	return f;
}

HIPCCONNECTION IPC_Runtime::serverWaitForConnection (HIPCSERVER hServer, DWORD tmo, HANDLE hBreakEvent /*= NULL*/)
{
	checkPostInit (); // TMP

	IPC_Server *pServer = getServer (hServer);
	if (! pServer) return IPC_ERR_TO_HIPCCONNECTION (IPC_ERR_INVALID_ARG);

	DWORD err;
	IPC_Connection *pConn = pServer->accept (tmo, err, hBreakEvent);
	if (! pConn) return IPC_ERR_TO_HIPCCONNECTION (err);

	HIPCCONNECTION h = registerConnection (pConn);
	if (h == HIPCCONNECTION_INVALID) {
		// very unlikely
		pConn->close ();
		delete pConn;
		return HIPCCONNECTION_INVALID;
	}

	return h;
}

HIPCCONNECTION IPC_Runtime::connect (const char *epName, DWORD tmo)
{
	checkPostInit ();

	IPC_Connection *pConn = new IPC_Connection ();
	if (! pConn) return IPC_ERR_TO_HIPCCONNECTION (IPC_ERR_INVALID_ARG);

	DWORD err = pConn->connect (epName, tmo);
	if (err != 0) {
		delete pConn;
		return IPC_ERR_TO_HIPCCONNECTION (err);
	}

	HIPCCONNECTION h = registerConnection (pConn);
	if (h == HIPCCONNECTION_INVALID) {
		// very unlikely
		pConn->close ();
		delete pConn;
		return HIPCCONNECTION_INVALID;
	}

	return h;
}

BOOL IPC_Runtime::closeConnection (HIPCCONNECTION hConn)
{
	IPC_Connection *pConn = unregisterConnection (hConn);
	if (! pConn) return FALSE;
	
	BOOL f = pConn->close ();
	delete pConn;

	return f;
}

BOOL IPC_Runtime::setUserEvent (HIPCCONNECTION hConn, HANDLE hUserEvent)
{
	IPC_Connection *pConn = getConnection (hConn);
	if (! pConn) return FALSE;
	
	return pConn->setUserEvent (hUserEvent);
}

BOOL IPC_Runtime::getUserEvent (HIPCCONNECTION hConn, HANDLE *phUserEvent)
{
	IPC_Connection *pConn = getConnection (hConn);
	if (! pConn) return FALSE;
	
	return pConn->getUserEvent (phUserEvent);
}

BOOL IPC_Runtime::resetUserEvent (HIPCCONNECTION hConn)
{
	IPC_Connection *pConn = getConnection (hConn);
	if (! pConn) return FALSE;
	
	return pConn->resetUserEvent ();
}

DWORD IPC_Runtime::getConnectionLastErr (HIPCCONNECTION hConn)
{
	IPC_Connection *pConn = unregisterConnection (hConn);
	if (! pConn) return IPC_ERR_INVALID_ARG;

	return pConn->getLastError ();
}

////////////////////////////////////////////////////////////////
// utilites

unsigned int IPC_Runtime::formatObjectPath (char *pathBuf, const char *prefix, const char *name)
{
	char *p = pathBuf;

	// on W2K and higher, prepend "Global\" prefix
	if (m_osVersion.dwPlatformId == VER_PLATFORM_WIN32_NT
	 && m_osVersion.dwMajorVersion >= 5) {
		strcpy (p, "Global\\");
		p += 7; // strlen ("Global\\");
	}

	if (prefix != NULL) {
		strcpy (p, prefix);
		p += strlen (p);
	}

	if (name != NULL) {
		strcpy (p, name);
		p += strlen (p);
	}

	return p - pathBuf;
}

bool IPC_Runtime::generateConnectionName (char *nameBuf)
{
	UUID uuid;
	UuidCreate (& uuid);

	unsigned char *strUuid = NULL;
	UuidToString (& uuid, &strUuid);

	if (! strUuid) return false;

	strcpy (nameBuf, (const char *)strUuid);
	RpcStringFree (& strUuid);

	return true;
}

////////////////////////////////////////////////////////////////
// security-related stuff

#include <service/sa.cpp>


