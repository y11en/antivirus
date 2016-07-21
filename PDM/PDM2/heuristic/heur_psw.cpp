#include "heur_psw.h"

// #define USERNAME                      "ADMINISTRATOR"
// 
// #define SYSTEM_DRIVE                  "C:"
// #define WINDOWS_DIRECTORY	          SYSTEM_DRIVE"\\WINDOWS"
// #define PROGRAMS_DIRECTORY	          SYSTEM_DRIVE"\\PROGRAM FILES"
// #define PROFILES_DIRECTORY            SYSTEM_DRIVE "\\DOCUMENTS AND SETTINGS"
// #define PROFILE_DIRECTORY             PROFILES_DIRECTORY "\\" USERNAME
// #define APPDATA_DIRECTORY             PROFILE_DIRECTORY "\\APPLICATION DATA"

#define WINDOWS_DIRECTORY	          "%WINDIR%"
#define PROGRAMS_DIRECTORY	          "%PROGRAMFILES%"
#define APPDATA_DIRECTORY             "%APPDATA%"
#define PROFILE_DIRECTORY             "%USERPROFILE%"
#define USERNAME                      "%USERNAME%"

#define MAX_FILES 3

#define MAKE_FLAG_BY_NO(no) (1<<(no))
#define INDEX_PROTECTED_STORAGE 31

typedef struct {
	uint32_t nFileCreateFailedKnown;
	uint32_t nFileCreateFailedUnknown;
	uint32_t nFlags;
} PSW_PROCESS_DATA;

struct folder_t {
	tcstring path;
	tcstring files[MAX_FILES];
};

static const folder_t psw_folders [] = { // UPPERCASE!
	{ _T(APPDATA_DIRECTORY) _T("\\THE BAT"), {_T("ACCOUNT.CFG"), _T("ACCOUNT.CFN")}},
	{ _T(APPDATA_DIRECTORY) _T("\\IPSWITCH\\WS_FTP\\SITES"), {_T("WS_FTP.INI")} },
	{ _T(PROFILE_DIRECTORY), {_T("WCX_FTP.INI")} },
	{ _T(APPDATA_DIRECTORY) _T("\\OPERA\\") _T(USERNAME) _T("\\MAIL"), {_T("ACCOUNTS.INI")} },
	{ _T(APPDATA_DIRECTORY) _T("\\OPERA\\") _T(USERNAME) _T("\\PROFILE"), {_T("WAND.DAT")} },
	{ _T(APPDATA_DIRECTORY) _T("\\THUNDERBIRD"), {_T("PROFILES.INI")} },
	{ _T(APPDATA_DIRECTORY) _T("\\MOZILLA\\FIREFOX"), {_T("PROFILES.INI")} },
	{ _T(APPDATA_DIRECTORY) _T("\\QUALCOMM\\EUDORA"), {_T("EUDORA.INI")} },
	{ _T(APPDATA_DIRECTORY) _T("\\SMARTFTP"), {_T("FAVORITES.DAT"), _T("HISTORY.DAT")} },
	{ _T(PROGRAMS_DIRECTORY) _T("\\CUTEFTP"), {_T("SM.DAT"), _T("TREE.DAT"), _T("SMDATA.DAT")} },
	{ _T(APPDATA_DIRECTORY) _T("\\GLOBALSCAPE\\CUTEFTP\\7.0"), {_T("SM.DAT"), _T("TREE.DAT"), _T("SMDATA.DAT")} },
	{ _T(APPDATA_DIRECTORY) _T("\\.GAIM"), {_T("ACCOUNTS.XML")} },
	{ _T(WINDOWS_DIRECTORY) , {_T("VD3USER.DAT"), _T("VD3MAIN.DAT")} },
	{ _T(PROGRAMS_DIRECTORY) _T("\\TRILLIAN\\USERS\\GLOBAL"), {_T("PROFILES.INI")} }
};	  
	  
cEventHandlerHeurPsw::cEventHandlerHeurPsw()
: psw_expanded_folders(0)
{
}

cEventHandlerHeurPsw::~cEventHandlerHeurPsw()
{
	if (psw_expanded_folders)
	{
		for (size_t i=0; i<countof(psw_folders); i++)
			tstrfree(psw_expanded_folders[i]);
		m_pEnvironmentHelper->Free(psw_expanded_folders);
	}
}

bool cEventHandlerHeurPsw::Init()
{
	psw_expanded_folders = (tstring*) m_pEnvironmentHelper->Alloc(countof(psw_folders)*sizeof(tstring));
	if (!psw_expanded_folders)
		return false;
	if (!m_pProcessList->AllocSlot(&m_slot_index))
		return false;
	for (size_t i=0; i<countof(psw_folders); i++)
	{
		psw_expanded_folders[i] = m_pEnvironmentHelper->ExpandEnvironmentStr(psw_folders[i].path);
		if (!psw_expanded_folders[i])
			psw_expanded_folders[i] = tstrdup(psw_folders[i].path);
		if (!psw_expanded_folders[i])
			return false;
		psw_expanded_folders[i] = tstrupr(psw_expanded_folders[i]);
		if (!psw_expanded_folders[i])
			return false;
	}
	return true;
}

void cEventHandlerHeurPsw::iOnPassAccess(cEvent& event, cFile& file, size_t nFolderIndex)
{
	cProcess pProcess = event.GetProcess();
	if (!pProcess)
		return;
	PSW_PROCESS_DATA* pData = (PSW_PROCESS_DATA*)pProcess->GetSlotData(m_slot_index, sizeof(PSW_PROCESS_DATA));
	if (!pData)
		return;
	
	uint32_t nFlag = MAKE_FLAG_BY_NO(nFolderIndex);

	if (pData->nFlags & nFlag) // already marked
		return;

	pData->nFlags |= nFlag;
	pData->nFileCreateFailedKnown++;
	uint32_t nKnown = pData->nFileCreateFailedKnown;
	pProcess.release();

	m_pEventMgr->OnPswAccess(event, file);

	if (nKnown >= 3)
		m_pEventMgr->OnPswAccessMultiple(event, file);

	return;
}

void cEventHandlerHeurPsw::iOnLegitAccess(cEvent& event, cFile& file, size_t nFolderIndex)
{
	return;
}


void cEventHandlerHeurPsw::OnFileCreateFail(cEvent& event, cFile& file, uint32_t desired_access, uint32_t share_mode, uint32_t creation_disposition)
{
	if (creation_disposition != OPEN_EXISTING || desired_access != GENERIC_READ)
		return;
	tcstring pFullName = file.getFull();
	tcstring pFileName = file.getFilename();
	for (size_t nFolderIndex=0; nFolderIndex<countof(psw_folders); nFolderIndex++)
	{
		tcstring pName = pFullName;
		if (!tstrcmpinc(&pName, psw_expanded_folders[nFolderIndex]))
			continue;
		if (*pName == '\\')
			pName++;
		if (pName != pFileName)
			continue;
		for (size_t j=0; j<MAX_FILES; j++)
		{
			if (psw_folders[nFolderIndex].files[j] == 0)
				continue;
			if (0 == tstrcmp(pFileName, psw_folders[nFolderIndex].files[j]))
				return iOnPassAccess(event, file, nFolderIndex);
		}
		return iOnLegitAccess(event, file, nFolderIndex);
	}

	return;
}

void cEventHandlerHeurPsw::OnProtectedStorageAccessPre(cEvent& event)
{
	cFile file(m_pEnvironmentHelper, _T("<Protected Storage>"), _T("<Protected Storage>"));
	return iOnPassAccess(event, file, INDEX_PROTECTED_STORAGE);
}
