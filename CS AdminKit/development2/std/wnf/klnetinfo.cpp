/*!
*		(C) 2002 Kaspersky Lab 
*		
*		\file	klnetinfo.cpp
*		\brief	
*		
*		\author Андрей Брыксин
*		\date	16.10.2002 14:06:24
*		
*		Example:	
*/		


#include <std/win32/klos_win32v50.h>
#include <std/err/klerrors.h>
#include "klnetinfo.h"

namespace KLWNF {

#define	NO_MORE_DATA	0xFFFFFFFF

/////////////////////////////////////////////////////////////////////////////
// CNetDomain

bool CNetDomain::IsDomain()
{
	bool rc = false;

	try
	{
		if (!GetPDC().empty()) rc = true;
	}
	catch(KLERR::Error* err)
	{
		if (err->GetId() != KLSTD::STDE_SYSTEM || err->GetErrorSubcode() != NERR_DCNotFound) throw;
		err->Release();
	}

	return rc;
}

std::wstring CNetDomain::GetPDC()
{
	if (m_sPDC.empty())
	{
		LPWSTR lpszPrimaryDC = NULL;

		NET_API_STATUS nStatus = NetGetDCName(NULL, m_sDomainName.c_str(), (LPBYTE*)&lpszPrimaryDC);  // Returned PDC 
		if (nStatus == NERR_Success) 
		{
			m_sPDC = lpszPrimaryDC;
			NetApiBufferFree(lpszPrimaryDC);
		}
		else
			KLSTD_THROW_LASTERROR_CODE2(nStatus);
	}

	return m_sPDC;
}

std::wstring CNetDomain::GetAnyDC()
{
	std::wstring rc;

	LPWSTR lpszPrimaryDC = NULL;
	NET_API_STATUS nStatus = NetGetAnyDCName(NULL,	m_sDomainName.c_str(), (LPBYTE*)&lpszPrimaryDC);  // Returned PDC 
	if (nStatus == NERR_Success)
	{
		rc = lpszPrimaryDC;
		NetApiBufferFree(lpszPrimaryDC);
	}
	else
		KLSTD_THROW_LASTERROR_CODE2(nStatus);

	return rc;
}

/////////////////////////////////////////////////////////////////////////////
// CNetUsers

void CNetUsers::Add(const wchar_t* pszUserName, const wchar_t* pszPassword)
{
	// Set up the USER_INFO_1 structure. 
	USER_INFO_1 user_info;
    user_info.usri1_name = const_cast<wchar_t*>(pszUserName);
    user_info.usri1_password = const_cast<wchar_t*>(pszPassword);
    user_info.usri1_priv = USER_PRIV_USER;
    user_info.usri1_home_dir = L"";
    user_info.usri1_comment = NULL;
    user_info.usri1_flags = UF_SCRIPT;
    user_info.usri1_script_path = L"";

	DWORD parm_err = 0;
	DWORD dwLevel = 1;

    NET_API_STATUS nStatus = NetUserAdd(m_Domain.GetPDC().c_str(), dwLevel, (LPBYTE)&user_info, &parm_err);
	if (nStatus != NERR_UserExists && nStatus != NERR_Success) KLSTD_THROW_LASTERROR_CODE2(nStatus);
}

void CNetUsers::Delete(const wchar_t* pszUserName)
{
	NET_API_STATUS nStatus = NetUserDel(m_Domain.GetPDC().c_str(), pszUserName);
	if (nStatus != NERR_Success) KLSTD_THROW_LASTERROR_CODE2(nStatus);
}

void CNetUsers::SetPassword(const wchar_t* pszUserName, const wchar_t* pszPassword)
{
	USER_INFO_1003 usriSetPassword;
	usriSetPassword.usri1003_password = (wchar_t*)pszPassword;

	DWORD dwLevel = 1003;
	DWORD dwParmError = 0;
	NET_API_STATUS nStatus = NetUserSetInfo(m_Domain.GetPDC().c_str(), pszUserName, dwLevel, (LPBYTE)&usriSetPassword, &dwParmError);
	if (nStatus != NERR_Success) KLSTD_THROW_LASTERROR_CODE2(nStatus);
}

void CNetUsers::ChangePassword(const wchar_t* pszUserName, const wchar_t* pszOldPassword, const wchar_t* pszNewPassword)
{
	NET_API_STATUS nStatus = NetUserChangePassword(m_Domain.GetPDC().c_str(), pszUserName, pszOldPassword, pszNewPassword);
	if (nStatus != NERR_Success) KLSTD_THROW_LASTERROR_CODE2(nStatus);
}

void CNetUsers::GetUserInfo(const wchar_t* pszUserName, CNetUserInfo* pUserInfo)
{
	DWORD dwLevel = 3;
	LPUSER_INFO_3 pBuf = NULL;

	NET_API_STATUS nStatus = NetUserGetInfo(m_Domain.GetPDC().c_str(), pszUserName, dwLevel, (LPBYTE*)&pBuf);
	if (nStatus == NERR_Success)
	{
		if (pBuf)
		{
			if(pBuf->usri3_name) pUserInfo->name = pBuf->usri3_name;
			if(pBuf->usri3_password) pUserInfo->password = pBuf->usri3_password;
			pUserInfo->password_age = pBuf->usri3_password_age;
			pUserInfo->priv = pBuf->usri3_priv;
			if(pBuf->usri3_home_dir) pUserInfo->home_dir = pBuf->usri3_home_dir;
			if(pBuf->usri3_comment) pUserInfo->comment = pBuf->usri3_comment;
			pUserInfo->flags = pBuf->usri3_flags;
			if(pBuf->usri3_script_path) pUserInfo->script_path = pBuf->usri3_script_path;
			if(pBuf->usri3_full_name) pUserInfo->full_name = pBuf->usri3_full_name;
			if(pBuf->usri3_usr_comment) pUserInfo->usr_comment = pBuf->usri3_usr_comment;
			if(pBuf->usri3_parms) pUserInfo->parms = pBuf->usri3_parms;
			if(pBuf->usri3_workstations) pUserInfo->workstations = pBuf->usri3_workstations;
			pUserInfo->max_storage = pBuf->usri3_max_storage;
			pUserInfo->units_per_week = pBuf->usri3_units_per_week;
			pUserInfo->logon_hours = pBuf->usri3_logon_hours;
			pUserInfo->bad_pw_count = pBuf->usri3_bad_pw_count;
			pUserInfo->num_logons = pBuf->usri3_num_logons;
			if(pBuf->usri3_logon_server) pUserInfo->logon_server = pBuf->usri3_logon_server;
			pUserInfo->country_code = pBuf->usri3_country_code;
			pUserInfo->code_page = pBuf->usri3_code_page;
			pUserInfo->user_id= pBuf->usri3_user_id;
			pUserInfo->primary_group_id = pBuf->usri3_primary_group_id;
			if(pBuf->usri3_profile) pUserInfo->profile = pBuf->usri3_profile;
			if(pBuf->usri3_home_dir_drive) pUserInfo->home_dir_drive = pBuf->usri3_home_dir_drive;
			pUserInfo->password_expired = pBuf->usri3_password_expired;

			NetApiBufferFree(pBuf);
		}
	}
	else
		KLSTD_THROW_LASTERROR_CODE2(nStatus);
}

bool CNetUsers::IsForcedChangePassword(const wchar_t* pszUserName)
{
	bool rc = false;

	DWORD dwLevel = 3;
	PUSER_INFO_3 pUser = NULL;
	
	NET_API_STATUS nStatus = NetUserGetInfo(m_Domain.GetPDC().c_str(), pszUserName, dwLevel, (LPBYTE*)&pUser);
	if (nStatus == NERR_Success)
	{
		rc = pUser->usri3_password_expired?true:false;
		NetApiBufferFree(pUser);
	}
	else
		KLSTD_THROW_LASTERROR_CODE2(nStatus);

	return rc;
}
 
void CNetUsers::ForceChangePassword(const wchar_t* pszUserName, bool bChangePass)
{
	PUSER_INFO_3 pUser = NULL;

	DWORD dwLevel = 3;
    DWORD dwParmError = 0;
	
	NET_API_STATUS nStatus = NetUserGetInfo(m_Domain.GetPDC().c_str(), pszUserName, dwLevel, (LPBYTE*)&pUser);
	if( nStatus == NERR_Success )
	{
		pUser->usri3_password_expired = bChangePass;
		nStatus = NetUserSetInfo(m_Domain.GetPDC().c_str(), pszUserName, dwLevel, (LPBYTE)pUser, &dwParmError);
		
		NetApiBufferFree(pUser);
	}

	if (nStatus != NERR_Success) KLSTD_THROW_LASTERROR_CODE2(nStatus);
}

bool CNetUsers::IsUserDisable(const wchar_t* pszUserName)
{
	bool rc = false;

	LPUSER_INFO_20 uinfo20;

	DWORD dwLevel = 20;
	DWORD dwParmError = 0;

	NET_API_STATUS nStatus = NetUserGetInfo(m_Domain.GetPDC().c_str(), pszUserName, 20, (LPBYTE*)&uinfo20);
	
	if (nStatus == NERR_Success)
	{
		rc = (uinfo20->usri20_flags & UF_ACCOUNTDISABLE)?true:false;
		NetApiBufferFree(uinfo20);
	}
	else
		KLSTD_THROW_LASTERROR_CODE2(nStatus);

	return rc;
}

void CNetUsers::DisableUser(const wchar_t* pszUserName, bool bDisable)
{
	USER_INFO_1008 uinfo1008;
	LPUSER_INFO_20 uinfo20;

	DWORD dwLevel = 1008;
	DWORD dwParmError = 0;

	// UF_SCRIPT: required for LAN Manager 2.0 and Windows NT/Windows 2000.
	//=> Check this, the Set Info change the previous set of the user
	NET_API_STATUS nStatus = NetUserGetInfo(m_Domain.GetPDC().c_str(), pszUserName, 20, (LPBYTE*)&uinfo20);
	if (nStatus == NERR_Success)
	{
		uinfo1008.usri1008_flags = uinfo20->usri20_flags;
		uinfo1008.usri1008_flags |= UF_SCRIPT;	
		
		if(bDisable)
			uinfo1008.usri1008_flags |= UF_ACCOUNTDISABLE;
		else
			if((uinfo1008.usri1008_flags & UF_ACCOUNTDISABLE) == UF_ACCOUNTDISABLE)
				uinfo1008.usri1008_flags ^= UF_ACCOUNTDISABLE;

		NetApiBufferFree(uinfo20);

		nStatus = NetUserSetInfo(m_Domain.GetPDC().c_str(), pszUserName, dwLevel, (LPBYTE)&uinfo1008, &dwParmError);
	}

	if (nStatus != NERR_Success) KLSTD_THROW_LASTERROR_CODE2(nStatus);
}

std::wstring CNetUsers::GetFullName(const wchar_t* pszUserName)
{
	std::wstring rc;

	LPUSER_INFO_2 uinfo2;

	DWORD dwLevel = 2;
	DWORD dwParmError = 0;

	NET_API_STATUS nStatus = NetUserGetInfo(m_Domain.GetPDC().c_str(), pszUserName, dwLevel, (LPBYTE*)&uinfo2);
	if(nStatus == NERR_Success)
	{
		rc = uinfo2->usri2_full_name;
		NetApiBufferFree(uinfo2);
	}
	else
		KLSTD_THROW_LASTERROR_CODE2(nStatus);

	return rc;
}

void CNetUsers::SetFullName(const wchar_t* pszUserName, const wchar_t* pszFullName)
{
	USER_INFO_1011 usriFullName;
	usriFullName.usri1011_full_name  = (wchar_t*)pszFullName;
	
	DWORD dwLevel = 1011;
	DWORD dwParmError = 0;

	NET_API_STATUS nStatus = NetUserSetInfo(m_Domain.GetPDC().c_str(), pszUserName, dwLevel, (LPBYTE)&usriFullName, &dwParmError);
	if (nStatus != NERR_Success) KLSTD_THROW_LASTERROR_CODE2(nStatus);
}

void CNetUsers::SetComment(const wchar_t* pszUserName, const wchar_t* pszComment)
{
	USER_INFO_1007 usriComment;
	usriComment.usri1007_comment = (wchar_t*)pszComment;
	
	DWORD dwLevel = 1007;
	DWORD dwParmError = 0;

	NET_API_STATUS nStatus = NetUserSetInfo(m_Domain.GetPDC().c_str(), pszUserName, dwLevel, (LPBYTE)&usriComment, &dwParmError);
	if (nStatus != NERR_Success) KLSTD_THROW_LASTERROR_CODE2(nStatus);
}

std::wstring CNetUsers::GetHomeDirectory(const wchar_t* pszUserName)
{
	std::wstring rc;

	LPUSER_INFO_1 uinfo1;

	DWORD dwLevel = 1;
	DWORD dwParmError = 0;

	NET_API_STATUS nStatus = NetUserGetInfo(m_Domain.GetPDC().c_str(), pszUserName, dwLevel, (LPBYTE*)&uinfo1);
	if (nStatus == NERR_Success)
	{
		rc = uinfo1->usri1_home_dir;
		NetApiBufferFree(uinfo1);
	}
	else
		KLSTD_THROW_LASTERROR_CODE2(nStatus);

	return rc;
}

void CNetUsers::SetHomeDirectory(const wchar_t* pszUserName, const wchar_t* pszHomeDirectory)
{
	USER_INFO_1006 usriHomeDir;
	usriHomeDir.usri1006_home_dir = (wchar_t*)pszHomeDirectory;

	DWORD dwLevel = 1006;
	DWORD dwParmError = 0;

	NET_API_STATUS nStatus = NetUserSetInfo(m_Domain.GetPDC().c_str(), pszUserName, dwLevel, (LPBYTE)&usriHomeDir, &dwParmError);
	if (nStatus != NERR_Success) KLSTD_THROW_LASTERROR_CODE2(nStatus);
}

int CNetUsers::GetUserPrivilege(const wchar_t* pszUserName)
{
	int rc = 0;

	LPUSER_INFO_2 usriPrivilege;
	DWORD dwLevel = 2;
	
	NET_API_STATUS nStatus = NetUserGetInfo(m_Domain.GetPDC().c_str(), pszUserName, dwLevel, (LPBYTE*)&usriPrivilege);
	if (nStatus == NERR_Success)
		rc = usriPrivilege->usri2_priv;
	else
		KLSTD_THROW_LASTERROR_CODE2(nStatus);

	return rc;
}

void CNetUsers::SetUserPrivilege(const wchar_t* pszUserName, int nPrivilege)
{
	USER_INFO_1005 usriPrivilege;
	usriPrivilege.usri1005_priv = nPrivilege;

	DWORD dwLevel = 1005;
	DWORD dwParmError = 0;

	NET_API_STATUS nStatus = NetUserSetInfo(m_Domain.GetPDC().c_str(), pszUserName, dwLevel, (LPBYTE)&usriPrivilege, &dwParmError);
	if (nStatus != NERR_Success) KLSTD_THROW_LASTERROR_CODE2(nStatus);
}

void CNetUsers::SetWorkStations(const wchar_t* pszUserName, const wchar_t* pszWorkStations)
{
	USER_INFO_1014 uinfo1014;
	uinfo1014.usri1014_workstations = (wchar_t*)pszWorkStations;

	DWORD dwLevel = 1014;
	DWORD dwParmError = 0;
		
	NET_API_STATUS nStatus = NetUserSetInfo(m_Domain.GetPDC().c_str(), pszUserName, dwLevel, (LPBYTE)&uinfo1014, &dwParmError);
	if (nStatus != NERR_Success) KLSTD_THROW_LASTERROR_CODE2(nStatus);
}

std::wstring CNetUsers::GetWorkStations(const wchar_t* pszUserName)
{
	std::wstring rc;
	
	LPUSER_INFO_2 uinfo2;
	DWORD dwLevel = 2;
		
	NET_API_STATUS nStatus = NetUserGetInfo(m_Domain.GetPDC().c_str(), pszUserName, dwLevel, (LPBYTE*)&uinfo2);
	if (nStatus == NERR_Success)
	{
		rc = uinfo2->usri2_workstations;
		NetApiBufferFree(uinfo2);
	}
	else
		KLSTD_THROW_LASTERROR_CODE2(nStatus);
	
	return rc;
}

std::wstring CNetUsers::GetComment(const wchar_t* pszUserName)
{
	std::wstring rc;

	LPUSER_INFO_1 uinfo1;
	DWORD dwLevel = 1;

	NET_API_STATUS nStatus = NetUserGetInfo(m_Domain.GetPDC().c_str(), pszUserName, dwLevel, (LPBYTE*)&uinfo1);
	if (nStatus == NERR_Success)
	{
		rc = uinfo1->usri1_comment;
		NetApiBufferFree(uinfo1);
	}
	else
		KLSTD_THROW_LASTERROR_CODE2(nStatus);

	return rc;
}

long CNetUsers::GetBadPasswordCount(const wchar_t* pszUserName)
{
	long rc = 0;

	LPUSER_INFO_11 uinfo11;
	DWORD dwLevel = 11;

	NET_API_STATUS nStatus = NetUserGetInfo(m_Domain.GetPDC().c_str(), pszUserName, dwLevel, (LPBYTE*)&uinfo11);
	if (nStatus == NERR_Success)
	{
		rc = uinfo11->usri11_bad_pw_count;
		NetApiBufferFree(uinfo11);
	}
	else
		KLSTD_THROW_LASTERROR_CODE2(nStatus);

	return rc;
}

long CNetUsers::GetLogonCount(const wchar_t* pszUserName)
{
	long rc = 0;

	LPUSER_INFO_11 uinfo11;
	DWORD dwLevel = 11;

	NET_API_STATUS nStatus = NetUserGetInfo(m_Domain.GetPDC().c_str(), pszUserName, dwLevel, (LPBYTE*)&uinfo11);
	if(nStatus == NERR_Success)
	{
		rc = uinfo11->usri11_num_logons;
		NetApiBufferFree(uinfo11);
	}
	else
		KLSTD_THROW_LASTERROR_CODE2(nStatus);

	return rc;
}

DWORD CNetUsers::GetLastLogonTime(const wchar_t* pszUserName)
{
	DWORD rc;
	
	LPUSER_INFO_2 uinfo2;
	DWORD dwLevel = 2;
		
	NET_API_STATUS nStatus = NetUserGetInfo(m_Domain.GetPDC().c_str(), pszUserName, dwLevel, (LPBYTE*)&uinfo2);
	if (nStatus == NERR_Success)
	{
		rc = uinfo2->usri2_last_logon;
		NetApiBufferFree(uinfo2);
	}
	else
		KLSTD_THROW_LASTERROR_CODE2(nStatus);
	
	return rc;
}

DWORD CNetUsers::GetLastLogoffTime(const wchar_t* pszUserName)
{
	DWORD rc;
	
	LPUSER_INFO_2 uinfo2;
	DWORD dwLevel = 2;
		
	NET_API_STATUS nStatus = NetUserGetInfo(m_Domain.GetPDC().c_str(), pszUserName, dwLevel, (LPBYTE*)&uinfo2);
	if (nStatus == NERR_Success)
	{
		rc = uinfo2->usri2_last_logoff;
		NetApiBufferFree(uinfo2);
	}
	else
		KLSTD_THROW_LASTERROR_CODE2(nStatus);
	
	return rc;
}

DWORD CNetUsers::GetPasswordAge(const wchar_t* pszUserName)
{
	DWORD rc = 0;

	LPUSER_INFO_11 uinfo11;
	DWORD dwLevel = 11;

	NET_API_STATUS nStatus = NetUserGetInfo(m_Domain.GetPDC().c_str(), pszUserName, dwLevel, (LPBYTE*)&uinfo11);
	if (nStatus == NERR_Success)
	{
		rc = uinfo11->usri11_password_age;
		NetApiBufferFree(uinfo11);
	}
	else
		KLSTD_THROW_LASTERROR_CODE2(nStatus);

	return rc;
}

bool CNetUsers::GetLogonScript(const wchar_t* pszUserName, std::wstring& sLogonScript)
{
	bool rc = false;
	
	LPUSER_INFO_1 pusriScript;

	DWORD dwLevel = 1;
	DWORD dwParmError = 0;
	
	NET_API_STATUS nStatus = NetUserGetInfo(m_Domain.GetPDC().c_str(), pszUserName, dwLevel, (LPBYTE*)&pusriScript);
	if (nStatus == NERR_Success)
	{
        if( pusriScript->usri1_script_path )
        {
            sLogonScript = pusriScript->usri1_script_path;
        } else
        {
            sLogonScript = L"";
        }
		
		NetApiBufferFree(pusriScript);
		rc = (!sLogonScript.empty());
	}
	else if( nStatus == NERR_UserNotFound )
	{
		KLSTD_THROW(KLSTD::STDE_NOTFOUND);
	}
	else
		KLSTD_THROW_LASTERROR_CODE2(nStatus);
	
	return rc;
}

void CNetUsers::SetLogonScript(const wchar_t* pszUserName, const wchar_t* pszLoginScriptPath)
{
	USER_INFO_1009 usriScript;
	usriScript.usri1009_script_path = (wchar_t*)pszLoginScriptPath;

	DWORD dwLevel = 1009;
	DWORD dwParmError = 0;

	NET_API_STATUS nStatus = NetUserSetInfo(m_Domain.GetPDC().c_str(), pszUserName, dwLevel, (LPBYTE)&usriScript, &dwParmError);
	if (nStatus != NERR_Success) KLSTD_THROW_LASTERROR_CODE2(nStatus);
}

void CNetUsers::RemoveLogonScript(const wchar_t* pszUserName)
{
	USER_INFO_1009 usriScript;
	usriScript.usri1009_script_path = L"";

	DWORD dwLevel = 1009;
	DWORD dwParmError = 0;

	NET_API_STATUS nStatus = NetUserSetInfo(m_Domain.GetPDC().c_str(), pszUserName, dwLevel, (LPBYTE)&usriScript, &dwParmError);
	if (nStatus != NERR_Success) KLSTD_THROW_LASTERROR_CODE2(nStatus);
}

void CNetUsers::StartQueryNextUser()
{
	m_nextUsrIndex = 0;
}

bool CNetUsers::QueryNextUser(std::wstring& sUserName, bool& bDisabledAccount)
{
	if (m_nextUsrIndex == NO_MORE_DATA) return false;

	bool rc = false;

	DWORD dwLevel = 1;
	DWORD dwEntryCount = 0;
	NET_DISPLAY_USER* lpBuffer = NULL;
	
	NET_API_STATUS nStatus = NetQueryDisplayInformation(m_Domain.GetPDC().c_str(), dwLevel, m_nextUsrIndex, 1, 0xFFFFFFFF, &dwEntryCount, (PVOID*)&lpBuffer);
	if (nStatus == NERR_Success || nStatus == ERROR_MORE_DATA)
	{
		sUserName = lpBuffer->usri1_name;
		bDisabledAccount = (lpBuffer->usri1_flags & UF_ACCOUNTDISABLE)?true:false;
		if(nStatus == ERROR_MORE_DATA) 
			m_nextUsrIndex = lpBuffer->usri1_next_index;
		else
			m_nextUsrIndex = NO_MORE_DATA;

		NetApiBufferFree(lpBuffer);
		rc = true;
	}
	else
		KLSTD_THROW_LASTERROR_CODE2(nStatus);

	return rc;
}

void CNetUsers::GetUserGroups(const wchar_t* pszUserName, std::list<std::wstring>& groups)
{
	LPGROUP_USERS_INFO_0 pBuf = NULL;
	DWORD dwLevel = 0;
	DWORD dwPrefMaxLen = MAX_PREFERRED_LENGTH;
	DWORD dwEntriesRead = 0;
	DWORD dwTotalEntries = 0;

	NET_API_STATUS nStatus = NetUserGetGroups(m_Domain.GetPDC().c_str(), pszUserName, dwLevel, (LPBYTE*)&pBuf, dwPrefMaxLen, &dwEntriesRead, &dwTotalEntries);
	if (nStatus == NERR_Success)
	{
		LPGROUP_USERS_INFO_0 pTmpBuf;

		pTmpBuf = pBuf;
		for (DWORD i = 0; i < dwEntriesRead; i++)
		{
			groups.push_back(pTmpBuf->grui0_name);
			pTmpBuf++;
		}

		NetApiBufferFree(pBuf);
	}
	else
		KLSTD_THROW_LASTERROR_CODE2(nStatus);
}

/////////////////////////////////////////////////////////////////////////////////////
// CNetShare
/////////////////////////////////////////////////////////////////////////////////////

void CNetShare::Add(const wchar_t*pszShareName, const wchar_t* pszLocalPath, const wchar_t* pszComment, PSECURITY_DESCRIPTOR psd)
{
	DWORD nStatus = ERROR_SUCCESS;

	SHARE_INFO_502 si502;

	// setup share info structure
	si502.shi502_current_uses = 0;
	si502.shi502_max_uses = SHI_USES_UNLIMITED;
	si502.shi502_netname = (wchar_t*)pszShareName;
	si502.shi502_passwd = NULL;
	si502.shi502_permissions = 0;
	si502.shi502_remark = (wchar_t*)pszComment;
	si502.shi502_type = STYPE_DISKTREE;
	si502.shi502_path = (wchar_t*)pszLocalPath;
	si502.shi502_reserved = 0;
	si502.shi502_security_descriptor = psd;
	
	nStatus = NetShareAdd((wchar_t*)m_sServerName.c_str(), 502, (LPBYTE)&si502, NULL);	// don't bother with par
	if (nStatus == NERR_DuplicateShare) nStatus = ERROR_SUCCESS;
}

#define SID_SIZE 96

void CNetShare::Add(const wchar_t*pszShareName, const wchar_t* pszLocalPath, const wchar_t* pszComment, const wchar_t* pszUserName, DWORD dwPermissions)
{
	DWORD nStatus = ERROR_SUCCESS;

    SECURITY_DESCRIPTOR sd;
    PACL pDacl = NULL;
    DWORD dwAclSize;

// initial allocation attempt for Sid
	DWORD cbSid = SID_SIZE;
    PSID pSid = (PSID)HeapAlloc(GetProcessHeap(), 0, cbSid);
    if (pSid)
	{
		wchar_t RefDomain[DNLEN + 1];
		DWORD cchDomain = DNLEN + 1;
		SID_NAME_USE peUse;

		// get the Sid associated with the supplied user/group name
		// force Unicode API since we always pass Unicode string
		if (!LookupAccountName(m_sServerName.c_str(), pszUserName, pSid, &cbSid, RefDomain, &cchDomain, &peUse)) 
		{
			// if the buffer wasn't large enough, try again
			nStatus = GetLastError();
			if (nStatus == ERROR_INSUFFICIENT_BUFFER)
			{
				pSid = (PSID)HeapReAlloc(GetProcessHeap(), 0, pSid, cbSid);
				if (pSid) 
				{
					cchDomain = DNLEN + 1;

					if (!LookupAccountName(m_sServerName.c_str(), pszUserName, pSid, &cbSid, RefDomain, &cchDomain, &peUse)) 
					{
						nStatus = GetLastError();
						goto cleanup;
					}
				
					nStatus = ERROR_SUCCESS;
				}
				else
				{
					nStatus = ERROR_OUTOFMEMORY;
					goto cleanup;
				}
			}
			else 
				goto cleanup;
		}
		
		// compute size of new acl
		dwAclSize = sizeof(ACL) +
			1 * ( sizeof(ACCESS_ALLOWED_ACE) - sizeof(DWORD) ) +
			GetLengthSid(pSid);
	
		// allocate storage for Acl
		pDacl = (PACL)HeapAlloc(GetProcessHeap(), 0, dwAclSize);
		if (!pDacl) 
		{
			nStatus = ERROR_OUTOFMEMORY;
			goto cleanup;
		}
	
		if (!InitializeAcl(pDacl, dwAclSize, ACL_REVISION))
		{
			nStatus = GetLastError();
			goto cleanup;
		}
	
		// grant GENERIC_ALL (Full Control) access
		if (!AddAccessAllowedAce(pDacl, ACL_REVISION, dwPermissions, pSid)) 
		{
			nStatus = GetLastError();
			goto cleanup;
		}
		
		if (!InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION))
		{
			nStatus = GetLastError();
			goto cleanup;
		}
		
		if (!SetSecurityDescriptorDacl(&sd, TRUE, pDacl, FALSE))
		{
			nStatus = GetLastError();
			goto cleanup;
		}

		Add(pszShareName, pszLocalPath, pszComment, &sd);
	}
	else
		nStatus = ERROR_NOT_ENOUGH_MEMORY;
		
cleanup:
    // free allocated resources
    if (pDacl) HeapFree(GetProcessHeap(), 0, pDacl);
    if(pSid) HeapFree(GetProcessHeap(), 0, pSid);
	if (nStatus != ERROR_SUCCESS) KLSTD_THROW_LASTERROR_CODE2(nStatus);
}

void CNetShare::Delete(const wchar_t* pszShareName)
{
	NET_API_STATUS nStatus = NetShareDel((wchar_t*)m_sServerName.c_str(), (wchar_t*)pszShareName, 0);
	if (nStatus != NERR_Success) KLSTD_THROW_LASTERROR_CODE2(nStatus);
}

std::wstring CNetShare::GetLocalPath(const wchar_t* pszShareName)
{
	std::wstring rc;

	SHARE_INFO_2* psi = NULL;

	NET_API_STATUS nStatus = NetShareGetInfo((wchar_t*)m_sServerName.c_str(), (wchar_t*)pszShareName, 2, (LPBYTE*)&psi);
	if (nStatus == NERR_Success)
	{
		rc = (LPWSTR)psi->shi2_path;
		NetApiBufferFree(psi);
	}
	else
		KLSTD_THROW_LASTERROR_CODE2(nStatus);
	
	return rc;
}

void CNetShare::NetAddConnection(const wchar_t* pszShareName, const wchar_t* pszUser, const wchar_t* pszPassword)
{
	KLSTD_CHKINPTR(pszShareName);

	NETRESOURCE nr;
		
	nr.dwScope = RESOURCE_GLOBALNET;
	nr.dwType = RESOURCETYPE_DISK;
	nr.dwDisplayType = RESOURCEDISPLAYTYPE_SHARE;
	nr.dwUsage = RESOURCEUSAGE_CONTAINER;
	nr.lpLocalName = L"";
	nr.lpComment = L"";
	nr.lpProvider = L"";
	nr.lpRemoteName = (wchar_t*)pszShareName;

	// Make a connection to the network resource.
	DWORD nStatus = WNetAddConnection3(NULL, &nr, pszPassword, pszUser, 0);
	if (nStatus != NO_ERROR) KLSTD_THROW_LASTERROR_CODE2(nStatus);
}

}