#ifdef _WIN32

#include <windows.h>
#include <shlobj.h>

#include <Prague/pr_cpp.h>
#include <Prague/pr_vector.h>
#include <Prague/pr_wrappers.h>
#include <Prague/iface/e_loader.h> 
#include <Prague/iface/i_objptr.h> 

#include <hashutil/hashutil.h>
extern "C"
{
#include "../../Extract/Disk/NtfsRead/ntfs.h"
}
#include "win32hlpr.h"

// -------------------------------------------------------

static PRLocker g_locker;

static bool             bInited = false;
static cVector<cStrObj> m_VistaVirtualFoders;
static cStrObj          m_VistaVirtualStore;

// ---
static bool ObjCmpFunc(const cStrObj& el, tCPTR params)
{
	cStrObj *pObject = (cStrObj *)params;
	return pObject ? pObject->compare(0, el.size(), el, 0, cStrObj::whole,
								fSTRING_COMPARE_CASE_INSENSITIVE) == cSTRING_COMP_EQ : false;
}

// ---
bool GetVirtualObject(const cStrObj &Object, cStrObj &VirtObj)
{
	if( PrGetOsVersion() < OSVERSION_VISTA )
		return false;

	PRAutoLocker locker(g_locker);

	if( !bInited )
	{
		bInited = true;

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
			VirtObj = VirtualStoreObject;
			return true;
		}
	}
	return false;
}

void ReleaseHlpData()
{
	m_VistaVirtualFoders.clear();
	m_VistaVirtualStore.deallocate();
}

// -------------------------------------------------------

CNtfsChecker::~CNtfsChecker()
{
	if( m_hntfs )
		NtfsClose((HNTFS)m_hntfs);
}

bool CNtfsChecker::AddObject(cStrObj& sPath, cStrObj* sStream)
{
	tDWORD pos = sPath.find_last_of("\\/");
	if( pos == cStrObj::npos )
		return false;

	cStrObj sName;
	sName.assign(sPath, pos+1);
	if( sStream )
	{
		sName += ':';
		sName += *sStream;
	}

	m_crcs.push_back(iCountCRC32(sName.size()*sizeof(tWCHAR), (const tBYTE*)sName.data(), 0));
	return true;
}

bool CNtfsChecker::CheckHidden(const tWCHAR* name, tDWORD size, cStringObjVector& hidden)
{
	size = size*sizeof(tWCHAR);
	tDWORD crc = iCountCRC32(size, (const tBYTE*)name, 0);
	if( m_crcs.find(crc) != -1 )
		return false;

	cStrObj sHidden;
	sHidden.assign(name, cCP_UNICODE, size);

	bool bAdd = true;
	if( m_steps )
		bAdd = (hidden.find(sHidden) != -1);

	if( bAdd )
		m_hidden.push_back(sHidden);
	return true;
}

void CNtfsChecker::CheckHiddenStreams(NTFS_FILE* file, cVector<cStrObj>& hidden, tWCHAR* name, tDWORD size)
{
	NTFS_ATTR* pAttr = NULL;
	while( pAttr = NtfsEnumAttributes(file, pAttr) )
	{
		NTFSResAttrib* ra = pAttr->ra;
		if( !ra->NameLength || ra->AttrType != AttribData )
			continue;

		cStrObj sName;
		if( name )
			sName.assign(name, cCP_UNICODE, size*sizeof(tWCHAR));

		sName += L':';
		sName.append(pAttr->pName, cCP_UNICODE, ra->NameLength*sizeof(tWCHAR));

		CheckHidden(sName.data(), sName.size(), hidden);
	}
}

bool CNtfsChecker::CheckHiddenObjects(cObjPtr* pPtr, bool bCheckStream, bool bCheckFileStream)
{
	cStrObj sPath;
	sPath.assign(pPtr, pgOBJECT_PATH);

	cStringObjVector hidden = m_hidden;
	m_hidden.clear();

	if( m_drive[0] != sPath.at(0) && m_hntfs )
		NtfsClose((HNTFS)m_hntfs), m_hntfs = NULL;

	if( !m_hntfs )
	{
		if( m_drive[0] == sPath.at(0) )
			return false;

		m_drive[0] = sPath.at(0);
		m_hntfs = NtfsOpen(m_drive);

		if( !m_hntfs )
			return false;
	}

	if( m_steps )
		NtfsFlushVolume((CHAR)m_drive[0]);

	NTFS_FILE* file = NtfsOpenFile((HNTFS)m_hntfs, NtfsFindFileW((HNTFS)m_hntfs, sPath.data()));
	if( file )
	{
		if( bCheckStream )
			CheckHiddenStreams(file, hidden, NULL, 0);

		NTFS_INDEX_ENTRY* ie = NULL;
		HANDLE enumhnd = NtfsEnumDirFirst(file, &ie);
		if( enumhnd )
		{
			do 
			{
				if( MFT_REC_NUMBER(ie->Header.MftRef) < 16 )
					continue;

				if( ie->FileNameAttrib == 2 )
					continue;

				if( ie->Header.MftRef == ie->DirWhereEntryAppears )
					continue;

				CheckHidden(ie->FileName, ie->NameLen, hidden);

				if( bCheckFileStream )
				{
					NTFS_FILE* subfile = NtfsOpenFile((HNTFS)m_hntfs, ie->Header.MftRef);
					if( subfile )
					{
						CheckHiddenStreams(subfile, hidden, ie->FileName, ie->NameLen);
						NtfsCloseFile(subfile);
					}
				}

			} while(NtfsEnumDirNext(enumhnd, &ie));
			NtfsEnumDirClose(enumhnd);
		}
		NtfsCloseFile(file);
	}

	m_crcs.clear();
	return !m_hidden.empty();
}

bool CNtfsChecker::GetVerdict(cVector<cStrObj>& vecHidden)
{
	if( m_steps++ < 1 )
		return false;

	vecHidden = m_hidden;
	m_hidden.clear();
	m_steps = 0;
	return true;
}

// -------------------------------------------------------

#endif // _WIN32
