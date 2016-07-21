#ifdef _WIN32

#include <shlobj.h>

#include "vistavirthlp.h"

#include <Prague/iface/e_loader.h> 

// ---
class CLocker
{
public:
	CLocker(){ InitializeCriticalSection(&m_cs); }
	~CLocker(){ DeleteCriticalSection(&m_cs); }

	void Enter(){ EnterCriticalSection(&m_cs); }
	void Leave(){ LeaveCriticalSection(&m_cs); }

private:
	CRITICAL_SECTION m_cs;
};

// ---
class CAutoLock
{
public:
	CAutoLock(CLocker &locker) : m_locker(locker){ m_locker.Enter(); };
	~CAutoLock() { m_locker.Leave(); };

private:
	CLocker &m_locker;
};

// ---
static CLocker g_locker;

static cVistaVirtHlp* g_pVistaVirtHlp = NULL;

// ---
cVistaVirtHlp *GetVistaVirtHlp()
{
	CAutoLock _lock(g_locker);
	if (!g_pVistaVirtHlp)
		g_pVistaVirtHlp = new cVistaVirtHlp();
	return g_pVistaVirtHlp;
}

void ReleaseVistaVirtHlp()
{
	CAutoLock _lock(g_locker);
	delete g_pVistaVirtHlp;
	g_pVistaVirtHlp = NULL;
}


// ---
cVistaVirtHlp::cVistaVirtHlp()
{
	m_bIsVista = PrGetOsVersion() >= OSVERSION_VISTA;
	if(m_bIsVista)
	{
		cStrObj s;
		tCHAR szBuffer[_MAX_PATH+2];
		if(GetWindowsDirectory(szBuffer, _countof(szBuffer)))
		{
			s = szBuffer; s.check_last_slash(false);
			m_VistaVirtualFoders.push_back(s);
			PR_TRACE((0, prtNOTIFY, "ods\tVista Folder added: %S", s.data()));
		}

		HKEY hKey = NULL;
		if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion",
											0, KEY_READ | KEY_WOW64_64KEY, &hKey) == ERROR_SUCCESS)
		{
			DWORD dwType, dwSize = sizeof(szBuffer);
			if(RegQueryValueEx(hKey, "ProgramFilesDir", NULL, &dwType, (LPBYTE)szBuffer, &dwSize) == ERROR_SUCCESS)
			{
				s = szBuffer; s.check_last_slash(false);
				m_VistaVirtualFoders.push_back(s);
				PR_TRACE((0, prtNOTIFY, "ods\tVista Folder added: %S", s.data()));
			}
			dwSize = sizeof(szBuffer);
			if(RegQueryValueEx(hKey, "ProgramFilesDir (x86)", NULL, &dwType, (LPBYTE)szBuffer, &dwSize) == ERROR_SUCCESS)
			{
				s = szBuffer; s.check_last_slash(false);
				m_VistaVirtualFoders.push_back(s);
				PR_TRACE((0, prtNOTIFY, "ods\tVista Folder added: %S", s.data()));
			}
			RegCloseKey(hKey);

			HMODULE hShFolder = LoadLibrary("shfolder.dll");
			if(hShFolder)
			{
				typedef HRESULT (WINAPI *tSHGetFolderPathA)(HWND hwnd, int csidl, HANDLE hToken, DWORD dwFlags, LPSTR pszPath);
				tSHGetFolderPathA fnSHGetFolderPath = (tSHGetFolderPathA)GetProcAddress(hShFolder, "SHGetFolderPathA");
				if(fnSHGetFolderPath)
				{
					if(SUCCEEDED(fnSHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA, NULL, SHGFP_TYPE_CURRENT, szBuffer)))
					{
						s = szBuffer; s.check_last_slash(false);
						s += "\\VirtualStore";
						m_VistaVirtualStore = s;
						PR_TRACE((0, prtNOTIFY, "ods\tVista Virtual Store: %S", m_VistaVirtualStore.data()));
					}
				}
				FreeLibrary(hShFolder);
			}
		}
	}
}

// ---
static bool ObjCmpFunc(const cStrObj& el, tCPTR params)
{
	cStrObj *pObject = (cStrObj *)params;
	return pObject ? pObject->compare(0, el.size(), el, 0, cStrObj::whole,
								fSTRING_COMPARE_CASE_INSENSITIVE) == cSTRING_COMP_EQ : false;
}

// ---
cStrObj cVistaVirtHlp::GetVirtualObject(const cStrObj &Object) const
{
	CAutoLock _lock(g_locker);
	cStrObj res_object;
	if(m_bIsVista)
	{
		tINT nVFPos = m_VistaVirtualFoders.find(ObjCmpFunc, &Object);
		if(nVFPos != cVector<cStrObj>::npos)
		{
			cStrObj VirtualStoreObject = m_VistaVirtualStore;
			tDWORD nLastSlashPos = Object.find_last_of("\\/", 0, m_VistaVirtualFoders[nVFPos].size());
			if(nLastSlashPos != cStrObj::npos)
				VirtualStoreObject += Object.substr(nLastSlashPos);
			else
				VirtualStoreObject += Object.substr(m_VistaVirtualFoders[nVFPos].size());

			nLastSlashPos = VirtualStoreObject.find_last_of("\\/");
			if(nLastSlashPos != cStrObj::npos &&
				GetFileAttributesW(VirtualStoreObject.substr(0, nLastSlashPos).data()) != INVALID_FILE_ATTRIBUTES)
			{
				res_object = VirtualStoreObject;
			}
		}
	}
	return res_object;
}

#endif // _WIN32
