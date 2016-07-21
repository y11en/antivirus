//
//  MODULE:   Net.cpp
//
//	AUTHOR: Carlos Antollini 
//
//  mailto: cantollini@hotmail.com
//
//	Date: 11/29/2001
//
//	Version 1.00
// 

#include "stdafx.h"
#include "mwnusers.h"

DWORD CNetDomain::GetPDC(CString& strPDC)
{
	NET_API_STATUS	nStatus = 0;
	
	if(m_strPDC.IsEmpty())
		nStatus = InitNetInfo();
	strPDC = m_strPDC;
	
	return nStatus;
}

DWORD CNetDomain::GetAnyDC(CString& strDC)
{
	NET_API_STATUS	nStatus = 0;
	LPWSTR  lpszDomainName = (LPWSTR)m_strDomainName.AllocSysString();
	LPWSTR	lpszPrimaryDC = NULL;
	
	strDC.Empty();
	
	nStatus = NetGetAnyDCName( NULL,	// Local machine 
		lpszDomainName,             // Domain name 
		(LPBYTE *)&lpszPrimaryDC);  // Returned PDC 
	
    if(nStatus == NERR_Success)
		strDC = _T(lpszPrimaryDC);
	
	SetLastErrorString(nStatus);
	SysFreeString(lpszDomainName);
	
	NetApiBufferFree(lpszPrimaryDC);
	return nStatus;
}

DWORD CNetDomain::InitNetInfo()
{
	NET_API_STATUS	nStatus = 0;
	LPWSTR  lpszDomainName = (LPWSTR)m_strDomainName.AllocSysString();
	LPWSTR	lpszPrimaryDC = NULL;

	m_strPDC.Empty();

	nStatus = NetGetDCName( NULL,				// Local machine 
                   lpszDomainName,              // Domain name 
                   (LPBYTE *)&lpszPrimaryDC);  // Returned PDC 

    if(nStatus == NERR_Success)
		m_strPDC = _T(lpszPrimaryDC);
	
	SetLastErrorString(nStatus);

	SysFreeString(lpszDomainName);
	NetApiBufferFree(lpszPrimaryDC);
	return nStatus;
}

DWORD CNetUsers::Add(CString strUserName, CString strPassword)
{
	USER_INFO_1 user_info;
	LPWSTR lpszUserName = (LPWSTR)strUserName.AllocSysString();
	LPWSTR lpszPassword = (LPWSTR)strPassword.AllocSysString();
	LPWSTR lpszPrimaryDC = (LPWSTR)m_pDomain.m_strPDC.AllocSysString();
	NET_API_STATUS nStatus = 0;
	DWORD parm_err = 0;
	DWORD dwLevel = 1;

	// Set up the USER_INFO_1 structure. 
    user_info.usri1_name = lpszUserName;
    user_info.usri1_password = lpszPassword;
    user_info.usri1_priv = USER_PRIV_USER;
    user_info.usri1_home_dir = (LPWSTR)"";
    user_info.usri1_comment = NULL;
    user_info.usri1_flags = UF_SCRIPT;
    user_info.usri1_script_path = (LPWSTR)"";

    nStatus = NetUserAdd(lpszPrimaryDC,
						dwLevel,
						(LPBYTE)&user_info,
						&parm_err);

	SetLastErrorString(nStatus);

	if(nStatus == NERR_UserExists)
		nStatus = NERR_Success;

	if(nStatus == NERR_Success)
		m_strUserName = strUserName;

	SysFreeString(lpszUserName);
	SysFreeString(lpszPassword);
	SysFreeString(lpszPrimaryDC);

	return nStatus;
}

DWORD CNetUsers::Delete(CString strUserName)
{
	NET_API_STATUS nStatus = 0;
	LPWSTR lpszUserName = (LPWSTR)strUserName.AllocSysString();
	LPWSTR lpszPrimaryDC = (LPWSTR)m_pDomain.m_strPDC.AllocSysString();

	nStatus = NetUserDel(lpszPrimaryDC, 
						lpszUserName);

	SetLastErrorString(nStatus);

	if(nStatus == NERR_Success && m_strUserName.Compare(strUserName) == 0)
		m_strUserName.Empty();

	SysFreeString(lpszUserName);
	SysFreeString(lpszPrimaryDC);
	
	return nStatus;
}
DWORD CNetUsers::SetPassword(CString strPassword)
{
	ASSERT(m_strUserName.GetLength());

	return SetPassword(m_strUserName, strPassword);
}


DWORD CNetUsers::SetPassword(CString strUserName, CString strPassword)
{
	LPWSTR lpszUserName = (LPWSTR)strUserName.AllocSysString();
	LPWSTR lpszPrimaryDC = (LPWSTR)m_pDomain.m_strPDC.AllocSysString();
	USER_INFO_1003 usriSetPassword;
	DWORD dwLevel = 1003;
	NET_API_STATUS nStatus = 0;
	DWORD dwParmError = 0;
	
	usriSetPassword.usri1003_password  = (LPWSTR)strPassword.AllocSysString();

	nStatus = NetUserSetInfo(lpszPrimaryDC, 
							lpszUserName,  
							dwLevel, 
							(LPBYTE)&usriSetPassword,
							&dwParmError);

	SetLastErrorString(nStatus, dwParmError);

	SysFreeString(lpszUserName);
	SysFreeString(lpszPrimaryDC);
	
	return nStatus;
}

DWORD CNetUsers::ChangePassword(CString strOldPassword, CString strNewPassword)
{

	ASSERT(m_strUserName.GetLength());

	return ChangePassword(m_strUserName, strOldPassword, strNewPassword);
}

DWORD CNetUsers::ChangePassword(CString strUserName, CString strOldPassword, CString strNewPassword)
{

	NET_API_STATUS nStatus = 0;

	LPWSTR lpszDomainName = (LPWSTR)m_pDomain.GetDomainName().AllocSysString();
	LPWSTR lpszUserName = (LPWSTR)strUserName.AllocSysString();
	LPWSTR lpszOldPassword = (LPWSTR)strOldPassword.AllocSysString();
	LPWSTR lpszNewPassword = (LPWSTR)strNewPassword.AllocSysString();

	nStatus = NetUserChangePassword(lpszDomainName, 
									lpszUserName, 
									lpszOldPassword, 
									lpszNewPassword);

	SetLastErrorString(nStatus);

	SysFreeString(lpszUserName);
	SysFreeString(lpszDomainName);
	
	SysFreeString(lpszOldPassword);
	SysFreeString(lpszNewPassword);

	return nStatus;	
}

DWORD CNetUsers::GetUserInfo(CNetUserInfo* pUserInfo)
{
	ASSERT(m_strUserName.GetLength());

	return GetUserInfo(m_strUserName, pUserInfo);
}

DWORD CNetUsers::GetUserInfo(CString strUserName, CNetUserInfo* pUserInfo)
{
	DWORD dwLevel = 3;
	LPUSER_INFO_3 pBuf = NULL;
	NET_API_STATUS nStatus;
	LPWSTR lpszUserName = (LPWSTR)strUserName.AllocSysString();
	LPWSTR lpszServerName = (LPWSTR)m_pDomain.m_strPDC.AllocSysString();
	m_strError.Empty();

	nStatus = NetUserGetInfo(lpszServerName,
							lpszUserName,
                            dwLevel,
                            (LPBYTE*)&pBuf);
   
	if (nStatus == NERR_Success)
	{
		if (pBuf != NULL)
		{
			memset(pUserInfo, 0, sizeof(CNetUserInfo));

			if(pBuf->usri3_name)
			{
				pUserInfo->name = (LPSTR)malloc(UNLEN * sizeof(char));
				memset(pUserInfo->name, 0, UNLEN * sizeof(char));
				wcstombs(pUserInfo->name, pBuf->usri3_name, 256);
				//pUserInfo->name = szBuff;
			}
			
			if(pBuf->usri3_password)
			{
				pUserInfo->password = (LPSTR)malloc(UNLEN * sizeof(char));
				memset(pUserInfo->password, 0, UNLEN * sizeof(char));
				wcstombs(pUserInfo->password, pBuf->usri3_password, 256);
				//pUserInfo->password = szBuff;
			}

			pUserInfo->password_age = pBuf->usri3_password_age;

			pUserInfo->priv = pBuf->usri3_priv;
			
			if(pBuf->usri3_home_dir)
			{
				pUserInfo->home_dir = (LPSTR)malloc(UNLEN * sizeof(char));
				memset(pUserInfo->home_dir, 0, UNLEN * sizeof(char));

				wcstombs(pUserInfo->home_dir, pBuf->usri3_home_dir, 256);
				//pUserInfo->home_dir = szBuff;
			}

			if(pBuf->usri3_comment)
			{
				pUserInfo->comment = (LPSTR)malloc(UNLEN * sizeof(char));
				memset(pUserInfo->comment, 0, UNLEN * sizeof(char));

				wcstombs(pUserInfo->comment, pBuf->usri3_comment, 256);
				//pUserInfo->comment = szBuff;
			}

			pUserInfo->flags = pBuf->usri3_flags;

			if(pBuf->usri3_script_path)
			{
				pUserInfo->script_path = (LPSTR)malloc(UNLEN * sizeof(char));
				memset(pUserInfo->script_path, 0, UNLEN * sizeof(char));
				wcstombs(pUserInfo->script_path, pBuf->usri3_script_path, 256);
			}

			if(pBuf->usri3_full_name)
			{
				pUserInfo->full_name = (LPSTR)malloc(UNLEN * sizeof(char));
				memset(pUserInfo->full_name, 0, UNLEN * sizeof(char));
				wcstombs(pUserInfo->full_name, pBuf->usri3_full_name, 256);
				
			}

			if(pBuf->usri3_usr_comment)
			{
				pUserInfo->usr_comment = (LPSTR)malloc(UNLEN * sizeof(char));
				memset(pUserInfo->usr_comment, 0, UNLEN * sizeof(char));
				wcstombs(pUserInfo->usr_comment, pBuf->usri3_usr_comment, 256);
			}

			if(pBuf->usri3_parms)
			{
				pUserInfo->parms = (LPSTR)malloc(UNLEN * sizeof(char));
				memset(pUserInfo->parms, 0, UNLEN * sizeof(char));
				wcstombs(pUserInfo->parms, pBuf->usri3_parms, 256);
			}

			if(pBuf->usri3_workstations)
			{
				pUserInfo->workstations = (LPSTR)malloc(UNLEN * sizeof(char));
				memset(pUserInfo->workstations, 0, UNLEN * sizeof(char));
				wcstombs(pUserInfo->workstations, pBuf->usri3_workstations, 256);
			}

			pUserInfo->max_storage = pBuf->usri3_max_storage;

			pUserInfo->units_per_week = pBuf->usri3_units_per_week;

			pUserInfo->logon_hours = pBuf->usri3_logon_hours;

			pUserInfo->bad_pw_count = pBuf->usri3_bad_pw_count;

			pUserInfo->num_logons = pBuf->usri3_num_logons;

			if(pBuf->usri3_logon_server)
			{
				pUserInfo->logon_server = (LPSTR)malloc(UNLEN * sizeof(char));
				memset(pUserInfo->logon_server, 0, UNLEN * sizeof(char));
				wcstombs(pUserInfo->logon_server, pBuf->usri3_logon_server, 256);
			}

			pUserInfo->country_code = pBuf->usri3_country_code;
			pUserInfo->code_page = pBuf->usri3_code_page;
			pUserInfo->user_id= pBuf->usri3_user_id;
			pUserInfo->primary_group_id = pBuf->usri3_primary_group_id;

			if(pBuf->usri3_profile)
			{
				pUserInfo->profile = (LPSTR)malloc(UNLEN * sizeof(char));
				memset(pUserInfo->profile, 0, UNLEN * sizeof(char));
				wcstombs(pUserInfo->profile, pBuf->usri3_profile, 256);
			}
	
			if(pBuf->usri3_home_dir_drive)
			{
				pUserInfo->script_path = (LPSTR)malloc(UNLEN * sizeof(char));
				memset(pUserInfo->script_path, 0, UNLEN * sizeof(char));
				wcstombs(pUserInfo->home_dir_drive, pBuf->usri3_home_dir_drive, 256);
			}
			pUserInfo->password_expired = pBuf->usri3_password_expired;;

			NetApiBufferFree(pBuf);
		}
	}
	
	SetLastErrorString(nStatus);
	
	SysFreeString(lpszUserName);
	SysFreeString(lpszServerName);

	return nStatus;
}

DWORD CNetUsers::IsForcedChangePassword(BOOL& bChangePass)
{
	ASSERT(m_strUserName.GetLength());

	return IsForcedChangePassword(m_strUserName, bChangePass);
}

DWORD CNetUsers::IsForcedChangePassword(CString strUserName, BOOL& bChangePass)
{
	ASSERT(strUserName.GetLength());

	DWORD dwLevel = 3;
	PUSER_INFO_3 pUser = NULL;
    NET_API_STATUS nStatus = 0;
	LPWSTR lpszUserName = (LPWSTR)strUserName.AllocSysString();
	LPWSTR lpszServerName = (LPWSTR)m_pDomain.m_strPDC.AllocSysString();
	
	nStatus = NetUserGetInfo(lpszServerName, 
							lpszUserName, 
							dwLevel, 
							(LPBYTE *)&pUser);

	bChangePass = pUser->usri3_password_expired;
	
	SetLastErrorString(nStatus);
		
	SysFreeString(lpszUserName);
	SysFreeString(lpszServerName);
	
	NetApiBufferFree(pUser);

	return nStatus;
}
 
DWORD CNetUsers::ForceChangePassword(BOOL bChangePass)
{
	ASSERT(m_strUserName.GetLength());

	return ForceChangePassword(m_strUserName, bChangePass);
}

DWORD CNetUsers::ForceChangePassword(CString strUserName, BOOL bChangePass)
{

	ASSERT(strUserName.GetLength());

	DWORD dwLevel = 3;
	PUSER_INFO_3 pUser = NULL;
    NET_API_STATUS nStatus = 0;
	LPWSTR lpszUserName = (LPWSTR)strUserName.AllocSysString();
	LPWSTR lpszServerName = (LPWSTR)m_pDomain.m_strPDC.AllocSysString();
    DWORD dwParmError = 0;
	
	nStatus = NetUserGetInfo(lpszServerName, 
							lpszUserName, 
							dwLevel, 
							(LPBYTE *)&pUser);

	if( nStatus == NERR_Success )
	{
		pUser->usri3_password_expired = bChangePass;
		nStatus = NetUserSetInfo(lpszServerName, 
								lpszUserName, 
								dwLevel, 
								(LPBYTE)pUser, 
								&dwParmError);
		
		NetApiBufferFree(pUser);
	}

	SysFreeString(lpszUserName);
	SysFreeString(lpszServerName);
	
	SetLastErrorString(nStatus);

	return nStatus;
}

DWORD CNetUsers::IsUserDisable(BOOL& bDisable)
{
	ASSERT(m_strUserName.GetLength());
	
	return IsUserDisable(m_strUserName, bDisable);
}

DWORD CNetUsers::IsUserDisable(CString strUserName, BOOL& bDisable)
{
	
	ASSERT(strUserName.GetLength());

	DWORD dwLevel = 20;
	LPUSER_INFO_20 uinfo20;

	NET_API_STATUS nStatus;
	LPWSTR lpszUserName = (LPWSTR)strUserName.AllocSysString();
	LPWSTR lpszServerName = (LPWSTR)m_pDomain.m_strPDC.AllocSysString();
	DWORD dwParmError = 0;

	bDisable = FALSE;
	
	nStatus = NetUserGetInfo(lpszServerName,
							lpszUserName,
							20,
							(LPBYTE*)&uinfo20);
	
	if(nStatus == NERR_Success)
		bDisable = (BOOL)uinfo20->usri20_flags & UF_ACCOUNTDISABLE;
	

	SetLastErrorString(nStatus, dwParmError);

	SysFreeString(lpszUserName);
	SysFreeString(lpszServerName);
	
	NetApiBufferFree(uinfo20);

	return nStatus;
}


DWORD CNetUsers::DisableUser(BOOL bDisable)
{
	ASSERT(m_strUserName.GetLength());

	return DisableUser(m_strUserName, bDisable);
}


DWORD CNetUsers::DisableUser(CString strUserName, BOOL bDisable)
{

	ASSERT(strUserName.GetLength());

	DWORD dwLevel = 1008;
	USER_INFO_1008 uinfo1008;
	LPUSER_INFO_20 uinfo20;

	NET_API_STATUS nStatus;
	LPWSTR lpszUserName = (LPWSTR)strUserName.AllocSysString();
	LPWSTR lpszServerName = (LPWSTR)m_pDomain.m_strPDC.AllocSysString();
	DWORD dwParmError = 0;

	// UF_SCRIPT: required for LAN Manager 2.0 and Windows NT/Windows 2000.

	//=> Check this, the Set Info change the previous set of the user
	nStatus = NetUserGetInfo(lpszServerName,
							lpszUserName,
							20,
							(LPBYTE*)&uinfo20);
	
	if(nStatus == NERR_Success)
	{
		uinfo1008.usri1008_flags = uinfo20->usri20_flags;

		uinfo1008.usri1008_flags |= UF_SCRIPT;	
		
		if(bDisable)
			uinfo1008.usri1008_flags |= UF_ACCOUNTDISABLE;
		else
			if((uinfo1008.usri1008_flags & UF_ACCOUNTDISABLE) == UF_ACCOUNTDISABLE)
				uinfo1008.usri1008_flags ^= UF_ACCOUNTDISABLE;

	
		nStatus = NetUserSetInfo(lpszServerName,
		                        lpszUserName,
			                    dwLevel,
				                (LPBYTE)&uinfo1008,
								&dwParmError);
	}
	
	SetLastErrorString(nStatus, dwParmError);

	SysFreeString(lpszUserName);
	SysFreeString(lpszServerName);
	
	NetApiBufferFree(uinfo20);

	return nStatus;
}


DWORD CNetUsers::GetFullName(CString& strFullName)
{
	ASSERT(m_strUserName.GetLength());

	return GetFullName(m_strUserName, strFullName);
}

DWORD CNetUsers::GetFullName(CString strUserName, CString& strFullName)
{
	ASSERT(strUserName.GetLength());

	LPWSTR lpszUserName = (LPWSTR)strUserName.AllocSysString();
	LPWSTR lpszServerName = (LPWSTR)m_pDomain.m_strPDC.AllocSysString();
	LPUSER_INFO_2 uinfo2;
	DWORD dwLevel = 2;
	NET_API_STATUS nStatus;
	DWORD dwParmError = 0;

	nStatus = NetUserGetInfo(lpszServerName, 
							lpszUserName,  
							dwLevel, 
							(LPBYTE*)&uinfo2);

	if(nStatus == NERR_Success)
		strFullName = _T(uinfo2->usri2_full_name);

	SetLastErrorString(nStatus);
	
	SysFreeString(lpszUserName);
	SysFreeString(lpszServerName);
	
	NetApiBufferFree(uinfo2);

	return nStatus;
}

DWORD CNetUsers::SetFullName(CString strFullName)
{
	ASSERT(m_strUserName.GetLength());

	return SetFullName(m_strUserName, strFullName);
}

DWORD CNetUsers::SetFullName(CString strUserName, CString strFullName)
{
	LPWSTR lpszUserName = (LPWSTR)strUserName.AllocSysString();
	LPWSTR lpszServerName = (LPWSTR)m_pDomain.m_strPDC.AllocSysString();
	USER_INFO_1011 usriFullName;
	DWORD dwLevel = 1011;
	NET_API_STATUS nStatus;
	DWORD dwParmError = 0;

	usriFullName.usri1011_full_name  = (LPWSTR)strFullName.AllocSysString();

	nStatus = NetUserSetInfo(lpszServerName, 
							lpszUserName,  
							dwLevel, 
							(LPBYTE)&usriFullName,
							&dwParmError);

	SetLastErrorString(nStatus, dwParmError);
	SysFreeString(lpszUserName);
	SysFreeString(lpszServerName);

	return nStatus;
}

DWORD CNetUsers::SetComment(CString strComment)
{
	ASSERT(m_strUserName.GetLength());

	return SetComment(m_strUserName, strComment);
}

DWORD CNetUsers::SetComment(CString strUserName, CString strComment)
{
	LPWSTR lpszUserName = (LPWSTR)strUserName.AllocSysString();
	LPWSTR lpszServerName = (LPWSTR)m_pDomain.m_strPDC.AllocSysString();
	USER_INFO_1007 usriComment;
	DWORD dwLevel = 1007;
	NET_API_STATUS nStatus;
	DWORD dwParmError = 0;

	usriComment.usri1007_comment = (LPWSTR)strComment.AllocSysString();

	nStatus = NetUserSetInfo(lpszServerName, 
							lpszUserName,  
							dwLevel, 
							(LPBYTE)&usriComment,
							&dwParmError);

	SetLastErrorString(nStatus, dwParmError);
	SysFreeString(lpszUserName);
	SysFreeString(lpszServerName);

	return nStatus;
}

DWORD CNetUsers::GetHomeDirectory(CString& strHomeDirectory)
{
	ASSERT(m_strUserName.GetLength());

	return GetHomeDirectory(m_strUserName, strHomeDirectory);
}

DWORD CNetUsers::GetHomeDirectory(CString strUserName, CString& strHomeDirectory)
{
	ASSERT(strUserName.GetLength());

	LPWSTR lpszUserName = (LPWSTR)strUserName.AllocSysString();
	LPWSTR lpszServerName = (LPWSTR)m_pDomain.m_strPDC.AllocSysString();
	LPUSER_INFO_1 uinfo1;
	DWORD dwLevel = 1;
	NET_API_STATUS nStatus;
	DWORD dwParmError = 0;

	nStatus = NetUserGetInfo(lpszServerName, 
							lpszUserName,  
							dwLevel, 
							(LPBYTE*)&uinfo1);

	if(nStatus == NERR_Success)
		strHomeDirectory = _T(uinfo1->usri1_home_dir);

	SetLastErrorString(nStatus);
	
	SysFreeString(lpszUserName);
	SysFreeString(lpszServerName);
	
	NetApiBufferFree(uinfo1);

	return nStatus;
}


DWORD CNetUsers::SetHomeDirectory(CString strHomeDirectory)
{
	ASSERT(m_strUserName.GetLength());

	return SetHomeDirectory(m_strUserName, strHomeDirectory);
}

DWORD CNetUsers::SetHomeDirectory(CString strUserName, CString strHomeDirectory)
{
	LPWSTR lpszUserName = (LPWSTR)strUserName.AllocSysString();
	LPWSTR lpszServerName = (LPWSTR)m_pDomain.m_strPDC.AllocSysString();
	USER_INFO_1006 usriHomeDir;
	DWORD dwLevel = 1006;
	NET_API_STATUS nStatus;
	DWORD dwParmError = 0;

	usriHomeDir.usri1006_home_dir = (LPWSTR)strHomeDirectory.AllocSysString();

	nStatus = NetUserSetInfo(lpszServerName, 
							lpszUserName,  
							dwLevel, 
							(LPBYTE)&usriHomeDir,
							&dwParmError);

	SetLastErrorString(nStatus, dwParmError);
	SysFreeString(lpszUserName);
	SysFreeString(lpszServerName);
	
	return nStatus;
}

DWORD CNetUsers::GetUserPrivilege(int& nPrivilege)
{
	ASSERT(m_strUserName.GetLength());

	return GetUserPrivilege(m_strUserName, nPrivilege);
}

DWORD CNetUsers::GetUserPrivilege(CString strUserName, int& nPrivilege)
{
	LPWSTR lpszUserName = (LPWSTR)strUserName.AllocSysString();
	LPWSTR lpszServerName = (LPWSTR)m_pDomain.m_strPDC.AllocSysString();
	LPUSER_INFO_2 usriPrivilege;
	DWORD dwLevel = 2;
	NET_API_STATUS nStatus;
	nPrivilege = 0;
	
	nStatus = NetUserGetInfo(lpszServerName, 
							lpszUserName,  
							dwLevel, 
							(LPBYTE*)&usriPrivilege);

	if(nStatus == NERR_Success)
		nPrivilege = usriPrivilege->usri2_priv;

	SetLastErrorString(nStatus);
	SysFreeString(lpszUserName);
	SysFreeString(lpszServerName);
	
	return nStatus;
}


DWORD CNetUsers::SetUserPrivilege(int nPrivilege)
{
	ASSERT(m_strUserName.GetLength());

	return SetUserPrivilege(m_strUserName, nPrivilege);
}

DWORD CNetUsers::SetUserPrivilege(CString strUserName, int nPrivilege)
{
	LPWSTR lpszUserName = (LPWSTR)strUserName.AllocSysString();
	LPWSTR lpszServerName = (LPWSTR)m_pDomain.m_strPDC.AllocSysString();
	USER_INFO_1005 usriPrivilege;
	DWORD dwLevel = 1005;
	NET_API_STATUS nStatus;
	DWORD dwParmError = 0;

	usriPrivilege.usri1005_priv = nPrivilege;

	nStatus = NetUserSetInfo(lpszServerName, 
							lpszUserName,  
							dwLevel, 
							(LPBYTE)&usriPrivilege,
							&dwParmError);

	SetLastErrorString(nStatus, dwParmError);
	SysFreeString(lpszUserName);
	SysFreeString(lpszServerName);
	
	return nStatus;
}

DWORD CNetUsers::SetOperatorPrivilege(int nPrivileges)
{
	ASSERT(m_strUserName.GetLength());

	return SetOperatorPrivilege(m_strUserName, nPrivileges);
}

DWORD CNetUsers::SetOperatorPrivilege(CString strUserName, int nPrivileges)
{
	LPWSTR lpszUserName = (LPWSTR)strUserName.AllocSysString();
	LPWSTR lpszServerName = (LPWSTR)m_pDomain.m_strPDC.AllocSysString();
	USER_INFO_1010 usriAuthFlags;
	DWORD dwLevel = 1010;
	NET_API_STATUS nStatus;
	DWORD dwParmError = 0;

	usriAuthFlags.usri1010_auth_flags = nPrivileges;

	nStatus = NetUserSetInfo(lpszServerName, 
							lpszUserName,  
							dwLevel, 
							(LPBYTE)&usriAuthFlags,
							&dwParmError);

	SetLastErrorString(nStatus, dwParmError);
	SysFreeString(lpszUserName);
	SysFreeString(lpszServerName);
	
	return nStatus;
}

DWORD CNetUsers::GetNotChangePass(BOOL& bNotPermit)
{
	ASSERT(m_strUserName.GetLength());

	return GetNotChangePass(m_strUserName, bNotPermit);
}


DWORD CNetUsers::GetNotChangePass(CString strUserName, BOOL& bNotPermit)
{

	ASSERT(strUserName.GetLength());

	DWORD dwLevel = 20;
	LPUSER_INFO_20 uinfo20;

	NET_API_STATUS nStatus;
	LPWSTR lpszUserName = (LPWSTR)strUserName.AllocSysString();
	LPWSTR lpszServerName = (LPWSTR)m_pDomain.m_strPDC.AllocSysString();

	nStatus = NetUserGetInfo(lpszServerName,
							lpszUserName,
							dwLevel,
							(LPBYTE*)&uinfo20);
	
	if(nStatus == NERR_Success)
		bNotPermit = (BOOL)uinfo20->usri20_flags & UF_PASSWD_CANT_CHANGE;
	
	SetLastErrorString(nStatus);

	NetApiBufferFree(uinfo20);
	SysFreeString(lpszUserName);
	SysFreeString(lpszServerName);
	
	return nStatus;
}


DWORD CNetUsers::SetNotChangePass(BOOL bPermit)
{
	ASSERT(m_strUserName.GetLength());

	return SetNotChangePass(m_strUserName, bPermit);
}


DWORD CNetUsers::SetNotChangePass(CString strUserName, BOOL bPermit)
{

	ASSERT(strUserName.GetLength());

	DWORD dwLevel = 1008;
	USER_INFO_1008 uinfo1008;
	LPUSER_INFO_20 uinfo20;

	NET_API_STATUS nStatus;
	LPWSTR lpszUserName = (LPWSTR)strUserName.AllocSysString();
	LPWSTR lpszServerName = (LPWSTR)m_pDomain.m_strPDC.AllocSysString();
	DWORD dwParmError = 0;

	// UF_SCRIPT: required for LAN Manager 2.0 and Windows NT/Windows 2000.

	//=> Check this, the Set Info change the previous set of the user
	nStatus = NetUserGetInfo(lpszServerName,
							lpszUserName,
							20,
							(LPBYTE*)&uinfo20);
	
	if(nStatus == NERR_Success)
	{
		uinfo1008.usri1008_flags = uinfo20->usri20_flags;

		uinfo1008.usri1008_flags |= UF_SCRIPT;

		if(bPermit)
			uinfo1008.usri1008_flags ^= UF_PASSWD_CANT_CHANGE;	
		else
			uinfo1008.usri1008_flags |= UF_PASSWD_CANT_CHANGE;
		
		
		nStatus = NetUserSetInfo(lpszServerName,
		                        lpszUserName,
			                    dwLevel,
				                (LPBYTE)&uinfo1008,
								&dwParmError);
	}
	
	SetLastErrorString(nStatus, dwParmError);

	NetApiBufferFree(uinfo20);
	SysFreeString(lpszUserName);
	SysFreeString(lpszServerName);
	
	return nStatus;
}

DWORD CNetUsers::SetWorkStations(CString strWorkStations)
{
	ASSERT(m_strUserName.GetLength());

	return SetWorkStations(m_strUserName, strWorkStations);
}


DWORD CNetUsers::SetWorkStations(CString strUserName, CString strWorkStations)
{

	ASSERT(strUserName.GetLength());

	DWORD dwLevel = 1014;
	USER_INFO_1014 uinfo1014;

	NET_API_STATUS nStatus;
	LPWSTR lpszUserName = (LPWSTR)strUserName.AllocSysString();
	LPWSTR lpszServerName = (LPWSTR)m_pDomain.m_strPDC.AllocSysString();
	DWORD dwParmError = 0;

	uinfo1014.usri1014_workstations = (LPWSTR)strWorkStations.AllocSysString();
		
	nStatus = NetUserSetInfo(lpszServerName,
	                        lpszUserName,
		                    dwLevel,
			                (LPBYTE)&uinfo1014,
							&dwParmError);
	
	
	SetLastErrorString(nStatus, dwParmError);
	SysFreeString(lpszUserName);
	SysFreeString(lpszServerName);
	
	return nStatus;
}


DWORD CNetUsers::GetWorkStations(CString& strWorkStations)
{
	ASSERT(m_strUserName.GetLength());

	return GetWorkStations(m_strUserName, strWorkStations);
}


DWORD CNetUsers::GetWorkStations(CString strUserName, CString& strWorkStations)
{

	ASSERT(strUserName.GetLength());

	DWORD dwLevel = 2;
	LPUSER_INFO_2 uinfo2;
	NET_API_STATUS nStatus;
	LPWSTR lpszUserName = (LPWSTR)strUserName.AllocSysString();
	LPWSTR lpszServerName = (LPWSTR)m_pDomain.m_strPDC.AllocSysString();
		
	strWorkStations.Empty();

	nStatus = NetUserGetInfo(lpszServerName,
	                        lpszUserName,
		                    dwLevel,
			                (LPBYTE*)&uinfo2);
	
	if(nStatus == NERR_Success)
		strWorkStations = _T(uinfo2->usri2_workstations);

	SetLastErrorString(nStatus);
	
	NetApiBufferFree(uinfo2);
	SysFreeString(lpszUserName);
	SysFreeString(lpszServerName);
	
	return nStatus;
}

DWORD CNetUsers::GetComment(CString& strComment)
{
	ASSERT(m_strUserName.GetLength());

	return GetComment(m_strUserName, strComment);
}

DWORD CNetUsers::GetComment(CString strUserName, CString& strComment)
{
	ASSERT(strUserName.GetLength());

	LPWSTR lpszUserName = (LPWSTR)strUserName.AllocSysString();
	LPWSTR lpszServerName = (LPWSTR)m_pDomain.m_strPDC.AllocSysString();
	LPUSER_INFO_1 uinfo1;
	DWORD dwLevel = 1;
	NET_API_STATUS nStatus;

	nStatus = NetUserGetInfo(lpszServerName, 
							lpszUserName,  
							dwLevel, 
							(LPBYTE*)&uinfo1);

	if(nStatus == NERR_Success)
		strComment = _T(uinfo1->usri1_comment);

	SetLastErrorString(nStatus);
	
	NetApiBufferFree(uinfo1);
	SysFreeString(lpszUserName);
	SysFreeString(lpszServerName);
	
	return nStatus;
}

DWORD CNetUsers::GetBadPasswordCount(long& nCount)
{
	ASSERT(m_strUserName.GetLength());

	return GetBadPasswordCount(m_strUserName, nCount);
}
 
DWORD CNetUsers::GetBadPasswordCount(CString strUserName, long& nCount)
{
	ASSERT(strUserName.GetLength());

	LPWSTR lpszUserName = (LPWSTR)strUserName.AllocSysString();
	LPWSTR lpszServerName = (LPWSTR)m_pDomain.m_strPDC.AllocSysString();
	LPUSER_INFO_11 uinfo11;
	DWORD dwLevel = 11;
	NET_API_STATUS nStatus;

	nCount = 0;

	nStatus = NetUserGetInfo(lpszServerName, 
							lpszUserName,  
							dwLevel, 
							(LPBYTE*)&uinfo11);

	if(nStatus == NERR_Success)
		nCount = uinfo11->usri11_bad_pw_count;

	SetLastErrorString(nStatus);
	
	NetApiBufferFree(uinfo11);
	SysFreeString(lpszUserName);
	SysFreeString(lpszServerName);
	
	return nStatus;
}

DWORD CNetUsers::GetLogonCount(long& nCount)
{
	ASSERT(m_strUserName.GetLength());

	return GetLogonCount(m_strUserName, nCount);
}
 
DWORD CNetUsers::GetLogonCount(CString strUserName, long& nCount)
{
	ASSERT(strUserName.GetLength());

	LPWSTR lpszUserName = (LPWSTR)strUserName.AllocSysString();
	LPWSTR lpszServerName = (LPWSTR)m_pDomain.m_strPDC.AllocSysString();
	LPUSER_INFO_11 uinfo11;
	DWORD dwLevel = 11;
	NET_API_STATUS nStatus;

	nCount = 0;

	nStatus = NetUserGetInfo(lpszServerName, 
							lpszUserName,  
							dwLevel, 
							(LPBYTE*)&uinfo11);

	if(nStatus == NERR_Success)
		nCount = uinfo11->usri11_num_logons;

	SetLastErrorString(nStatus);
	
	NetApiBufferFree(uinfo11);
	SysFreeString(lpszUserName);
	SysFreeString(lpszServerName);
	
	return nStatus;
}

DWORD CNetUsers::GetPasswordAge(DWORD& nSeconds)
{
	ASSERT(m_strUserName.GetLength());

	return GetPasswordAge(m_strUserName, nSeconds);
}
 
DWORD CNetUsers::GetPasswordAge(CString strUserName, DWORD& nSeconds)
{
	ASSERT(strUserName.GetLength());

	LPWSTR lpszUserName = (LPWSTR)strUserName.AllocSysString();
	LPWSTR lpszServerName = (LPWSTR)m_pDomain.m_strPDC.AllocSysString();
	LPUSER_INFO_11 uinfo11;
	DWORD dwLevel = 11;
	NET_API_STATUS nStatus;

	nSeconds = 0;

	nStatus = NetUserGetInfo(lpszServerName, 
							lpszUserName,  
							dwLevel, 
							(LPBYTE*)&uinfo11);

	if(nStatus == NERR_Success)
		nSeconds = uinfo11->usri11_password_age;

	SetLastErrorString(nStatus);
	
	NetApiBufferFree(uinfo11);
	SysFreeString(lpszUserName);
	SysFreeString(lpszServerName);
	
	return nStatus;
}

DWORD CNetUsers::StartQueryNextUser()
{
	m_nextUsrIndex = 0;

	return NERR_Success;
}

DWORD CNetUsers::QueryNextUser(CString& strUserName, BOOL& bMoreData, BOOL& bDisabledAccount)
{

	LPWSTR lpszServerName = (LPWSTR)m_pDomain.m_strPDC.AllocSysString();
	DWORD dwLevel = 1;
	NET_API_STATUS nStatus; 
	DWORD dwEntryCount = 0;
	NET_DISPLAY_USER* lpBuffer = NULL;
	
	nStatus = NetQueryDisplayInformation(
						lpszServerName,
						dwLevel,
						m_nextUsrIndex,
						1,
						0xFFFFFFFF,
						&dwEntryCount,
						(PVOID*)&lpBuffer);

	bMoreData = (nStatus == ERROR_MORE_DATA);

	if(nStatus == NERR_Success || nStatus == ERROR_MORE_DATA)
	{
		strUserName = _T(lpBuffer->usri1_name);
		m_strUserName = strUserName;
		bDisabledAccount = (lpBuffer->usri1_flags & UF_ACCOUNTDISABLE)?TRUE:FALSE;
		
		if(nStatus == ERROR_MORE_DATA)
		{
			m_nextUsrIndex = lpBuffer->usri1_next_index;
			nStatus = NERR_Success;
		}
	}
		
	SetLastErrorString(nStatus);

	if(lpBuffer) NetApiBufferFree(lpBuffer);
	SysFreeString(lpszServerName);

	return nStatus;
}

DWORD CNetUsers::FindFirstUser(BOOL& bMoreData)
{
	CString strUserName = _T("");

	return FindFirstUser(strUserName, bMoreData);
}

DWORD CNetUsers::FindFirstUser(CString& strUserName, BOOL& bMoreData)
{
	LPWSTR lpszServerName = (LPWSTR)m_pDomain.m_strPDC.AllocSysString();
	LPUSER_INFO_0 usrinf0 = NULL;
	DWORD dwLevel = 0;
	DWORD dwPrefMaxLen = -1;
    DWORD dwEntriesRead = 0;
    DWORD dwTotalEntries = 0;
    DWORD dwResumeHandle = 0;
	NET_API_STATUS nStatus;

	m_lstUsers.RemoveAll();
	bMoreData = FALSE;
	strUserName.Empty();
	
	do 
	{
		nStatus = NetUserEnum(lpszServerName,
							dwLevel,
                            FILTER_NORMAL_ACCOUNT,
                            (LPBYTE*)&usrinf0,
                            dwPrefMaxLen,
                            &dwEntriesRead,
                            &dwTotalEntries,
                            &dwResumeHandle);
      
		if(nStatus == NERR_Success || nStatus == ERROR_MORE_DATA)
		{
			if(usrinf0 != NULL)
			{

				for (register DWORD i = 0; i < dwEntriesRead; i++)
				{
        
					CString strUser = _T(usrinf0->usri0_name);
					m_lstUsers.AddTail(strUser);
					usrinf0++;
				}
			}
		}
	}
	while (nStatus == ERROR_MORE_DATA); // end do
   
	if(m_lstUsers.GetCount())
	{
		m_position = m_lstUsers.GetHeadPosition();
//AA		strUserName = m_lstUsers.GetNext(m_position);
//AA		m_strUserName = strUserName;
		bMoreData = m_position != NULL;
	}
	
	SysFreeString(lpszServerName);
	SetLastErrorString(nStatus);
   
	return nStatus;
}

DWORD CNetUsers::FindNextUser(BOOL& bMoreData)
{

	ASSERT(m_strUserName.GetLength());

	CString strUserName = _T("");
	
	return FindNextUser(strUserName, bMoreData);
}

DWORD CNetUsers::FindNextUser(CString& strUserName, BOOL& bMoreData)
{
	strUserName = m_lstUsers.GetNext(m_position);
	
	m_strUserName = strUserName;

	bMoreData = m_position != NULL;
	
	return NERR_Success;
}
 
DWORD CNetGroups::Add(CString strGroupName, CString strComments)
{
	LPWSTR lpszGroupName = (LPWSTR)strGroupName.AllocSysString();
	LPWSTR lpszPrimaryDC = (LPWSTR)m_pDomain.m_strPDC.AllocSysString();
	GROUP_INFO_1 group_info;
	NET_API_STATUS nStatus;
	DWORD dwLevel = 1;
	DWORD dwParmError = 0;
	
	group_info.grpi1_name = lpszGroupName;
	group_info.grpi1_comment = (LPWSTR)strComments.AllocSysString();

    nStatus = NetGroupAdd(lpszPrimaryDC,
                  dwLevel,                    
                  (LPBYTE)&group_info,   
                  &dwParmError);   

	if(nStatus == NERR_GroupExists || nStatus == ERROR_ALIAS_EXISTS)
		nStatus = NERR_Success;
	
	if(nStatus == NERR_Success)
		m_strGroupName = strGroupName;

	SetLastErrorString(nStatus, dwParmError);
	SysFreeString(lpszGroupName);
	SysFreeString(lpszPrimaryDC);

	return nStatus;
	
}

DWORD CNetGroups::AddUser(CString strUserName)
{
	ASSERT(m_strGroupName.GetLength());

	return AddUser(m_strGroupName, strUserName);
}

DWORD CNetGroups::AddUser(CString strGroupName, CString strUserName)
{

	LPWSTR  lpszPrimaryDC = (LPWSTR)m_pDomain.m_strPDC.AllocSysString();
	LPWSTR lpszGroupName = (LPWSTR)strGroupName.AllocSysString();
	LPWSTR lpszUserName = (LPWSTR)strUserName.AllocSysString();
	NET_API_STATUS nStatus;	

    nStatus = NetGroupAddUser(lpszPrimaryDC,
                           lpszGroupName,
                           lpszUserName);  
	
	SetLastErrorString(nStatus);
	
	SysFreeString(lpszGroupName);
	SysFreeString(lpszPrimaryDC);
	SysFreeString(lpszUserName);
	
	return nStatus;
}

DWORD CNetGroups::DeleteUser(CString strUserName)
{
	ASSERT(m_strGroupName.GetLength());

	return DeleteUser(m_strGroupName, strUserName);
}

DWORD CNetGroups::DeleteUser(CString strGroupName, CString strUserName)
{

	LPWSTR  lpszPrimaryDC = (LPWSTR)m_pDomain.m_strPDC.AllocSysString();
	LPWSTR lpszGroupName = (LPWSTR)strGroupName.AllocSysString();
	LPWSTR lpszUserName = (LPWSTR)strUserName.AllocSysString();
	
	NET_API_STATUS nStatus;

	nStatus = NetGroupDelUser(
				  lpszPrimaryDC, 
				  lpszGroupName,  
				  lpszUserName);

	SetLastErrorString(nStatus);
	SysFreeString(lpszGroupName);
	SysFreeString(lpszPrimaryDC);
	SysFreeString(lpszUserName);
	
	return nStatus;
}

DWORD CNetGroups::Delete()
{
	ASSERT(m_strGroupName.GetLength());

	return Delete(m_strGroupName);
}


DWORD CNetGroups::Delete(CString strGroupName)
{
		
	LPWSTR  lpszPrimaryDC = (LPWSTR)m_pDomain.m_strPDC.AllocSysString();
	LPWSTR lpszGroupName = (LPWSTR)strGroupName.AllocSysString();
	NET_API_STATUS nStatus;

	nStatus = NetGroupDel(lpszPrimaryDC,
                           lpszGroupName);
    
	SetLastErrorString(nStatus);

	return nStatus;
}

DWORD CNetGroups::FindFirstGroup(BOOL& bMoreData)
{
	CString strGroupName = _T("");

	return FindFirstGroup(strGroupName, bMoreData);
}

DWORD CNetGroups::FindFirstGroup(CString& strGroupName, BOOL& bMoreData)
{
	LPWSTR lpszServerName = (LPWSTR)m_pDomain.m_strPDC.AllocSysString();
	LPGROUP_INFO_0 grpinf0 = NULL;
	DWORD dwLevel = 0;
	DWORD dwPrefMaxLen = -1;
    DWORD dwEntriesRead = 0;
    DWORD dwTotalEntries = 0;
    DWORD dwResumeHandle = 0;
	NET_API_STATUS nStatus;

	m_lstGroups.RemoveAll();
	bMoreData = FALSE;
	strGroupName.Empty();

	do 
	{
		nStatus = NetGroupEnum(lpszServerName,
							dwLevel,
                            (LPBYTE*)&grpinf0,
                            dwPrefMaxLen,
                            &dwEntriesRead,
                            &dwTotalEntries,
                            &dwResumeHandle);
      
		if(nStatus == NERR_Success || nStatus == ERROR_MORE_DATA)
		{
			if(grpinf0 != NULL)
			{

				for (register DWORD i = 0; i < dwEntriesRead; i++)
				{
        
					CString strUser = _T(grpinf0->grpi0_name);
					m_lstGroups.AddTail(strUser);
					grpinf0++;
				}
			}
		}
	}
	while (nStatus == ERROR_MORE_DATA); // end do
   
	if(m_lstGroups.GetCount())
	{
		m_position = m_lstGroups.GetHeadPosition();
		strGroupName = m_lstGroups.GetNext(m_position);
		m_strGroupName = strGroupName;
		bMoreData = m_position != NULL;
	}
	
	
	SetLastErrorString(nStatus);
   
	return nStatus;
}

DWORD CNetGroups::FindNextGroup(BOOL& bMoreData)
{
	ASSERT(m_strGroupName.GetLength());

	CString strGroupName = _T("");

	return FindNextGroup(strGroupName, bMoreData);
}

DWORD CNetGroups::FindNextGroup(CString& strGroupName, BOOL& bMoreData)
{
	strGroupName = m_lstGroups.GetNext(m_position);
	m_strGroupName = strGroupName;
	bMoreData = m_position != NULL;
	
	return NERR_Success;
}

DWORD CNetLocalGroups::Add(CString strLocalGroupName, CString strComments)
{
	LPWSTR lpszLocalGroupName = (LPWSTR)strLocalGroupName.AllocSysString();
	LPWSTR lpszPrimaryDC = (LPWSTR)m_pDomain.m_strPDC.AllocSysString();
	LOCALGROUP_INFO_1 localgroup_info;
	NET_API_STATUS nStatus;
	DWORD dwLevel = 1;
	DWORD dwParmError = 0;
	
	localgroup_info.lgrpi1_name = lpszLocalGroupName;
	localgroup_info.lgrpi1_comment = (LPWSTR)strComments.AllocSysString();

    nStatus = NetLocalGroupAdd(lpszPrimaryDC,
                  dwLevel,                    
                  (LPBYTE)&localgroup_info,   
                  &dwParmError);   

	if(nStatus == NERR_GroupExists || nStatus == ERROR_ALIAS_EXISTS)
		nStatus = NERR_Success;
	
	if(nStatus == NERR_Success)
		m_strLocalGroupName = strLocalGroupName;

	SetLastErrorString(nStatus, dwParmError);
	SysFreeString(lpszLocalGroupName);
	SysFreeString(lpszPrimaryDC);
	
	return nStatus;
	
}

DWORD CNetLocalGroups::AddUser(CString strUserName)
{
	ASSERT(m_strLocalGroupName.GetLength());

	return AddUser(m_strLocalGroupName, strUserName);
}


DWORD CNetLocalGroups::AddUser(CString strLocalGroupName, CString strUserName)
{

	LOCALGROUP_MEMBERS_INFO_3 localgroup_members;
	LPWSTR  lpszPrimaryDC = (LPWSTR)m_pDomain.m_strPDC.AllocSysString();
	LPWSTR lpszLocalGroupName = (LPWSTR)strLocalGroupName.AllocSysString();
	DWORD dwLevel = 3;
	NET_API_STATUS nStatus;

	localgroup_members.lgrmi3_domainandname = (LPWSTR)strUserName.AllocSysString();
	

    nStatus = NetLocalGroupAddMembers(lpszPrimaryDC,
                           lpszLocalGroupName,
                           dwLevel,
                           (LPBYTE)&localgroup_members,
                           1);  
	
	SetLastErrorString(nStatus);
	SysFreeString(lpszLocalGroupName);
	SysFreeString(lpszPrimaryDC);
	
	return nStatus;
}


DWORD CNetLocalGroups::DeleteUser(CString strUserName)
{
	ASSERT(m_strLocalGroupName.GetLength());

	return DeleteUser(m_strLocalGroupName, strUserName);
}

DWORD CNetLocalGroups::DeleteUser(CString strLocalGroupName, CString strUserName)
{

	LOCALGROUP_MEMBERS_INFO_3 localgroup_members;
	LPWSTR  lpszPrimaryDC = (LPWSTR)m_pDomain.m_strPDC.AllocSysString();
	LPWSTR lpszLocalGroupName = (LPWSTR)strLocalGroupName.AllocSysString();
	DWORD dwLevel = 3;
	NET_API_STATUS nStatus;
	
	localgroup_members.lgrmi3_domainandname = (LPWSTR)strUserName.AllocSysString();

	nStatus = NetLocalGroupDelMembers(
				  lpszPrimaryDC, 
				  lpszLocalGroupName,  
				  dwLevel,        
				  (LPBYTE)&localgroup_members,         
				  1);

	SetLastErrorString(nStatus);
	SysFreeString(lpszLocalGroupName);
	SysFreeString(lpszPrimaryDC);
	
	return nStatus;
}

DWORD CNetLocalGroups::Delete()
{
	ASSERT(m_strLocalGroupName.GetLength());

	return Delete(m_strLocalGroupName);
}

DWORD CNetLocalGroups::Delete(CString strLocalGroupName)
{
		
	LPWSTR  lpszPrimaryDC = (LPWSTR)m_pDomain.m_strPDC.AllocSysString();
	LPWSTR lpszLocalGroupName = (LPWSTR)strLocalGroupName.AllocSysString();
	NET_API_STATUS nStatus;

	nStatus = NetLocalGroupDel(lpszPrimaryDC,
                           lpszLocalGroupName);
    
	SetLastErrorString(nStatus);
	SysFreeString(lpszLocalGroupName);
	SysFreeString(lpszPrimaryDC);
	
	return nStatus;
}

DWORD CNetLocalGroups::FindFirstGroup(BOOL& bMoreData)
{
	CString strLocalGroupName = _T("");

	return FindFirstGroup(strLocalGroupName, bMoreData);
}

DWORD CNetLocalGroups::FindFirstGroup(CString& strLocalGroupName, BOOL& bMoreData)
{
	LPWSTR lpszServerName = (LPWSTR)m_pDomain.m_strPDC.AllocSysString();
	LPLOCALGROUP_INFO_0 grpinf0 = NULL;
	DWORD dwLevel = 0;
	DWORD dwPrefMaxLen = -1;
    DWORD dwEntriesRead = 0;
    DWORD dwTotalEntries = 0;
    DWORD dwResumeHandle = 0;
	NET_API_STATUS nStatus;

	m_lstLocalGroups.RemoveAll();
	bMoreData = FALSE;
	strLocalGroupName.Empty();
	
	do 
	{
		nStatus = NetLocalGroupEnum(lpszServerName,
							dwLevel,
                            (LPBYTE*)&grpinf0,
                            dwPrefMaxLen,
                            &dwEntriesRead,
                            &dwTotalEntries,
                            &dwResumeHandle);
      
		if(nStatus == NERR_Success || nStatus == ERROR_MORE_DATA)
		{
			if(grpinf0 != NULL)
			{

				for (register DWORD i = 0; i < dwEntriesRead; i++)
				{
        
					CString strLocalGroup = _T(grpinf0->lgrpi0_name);
					m_lstLocalGroups.AddTail(strLocalGroup);
					grpinf0++;
				}
			}
		}
	}
	while (nStatus == ERROR_MORE_DATA); // end do
   
	if(m_lstLocalGroups.GetCount())
	{
		m_position = m_lstLocalGroups.GetHeadPosition();
		strLocalGroupName = m_lstLocalGroups.GetNext(m_position);
		m_strLocalGroupName = strLocalGroupName;
		bMoreData = m_position != NULL;
	}
	
	SetLastErrorString(nStatus);
	SysFreeString(lpszServerName);
	
	return nStatus;
}

DWORD CNetLocalGroups::FindNextGroup(BOOL& bMoreData)
{
	ASSERT(m_strLocalGroupName.GetLength());
	
	CString strLocalGroupName = _T("");

	return FindNextGroup(strLocalGroupName, bMoreData);
}

DWORD CNetLocalGroups::FindNextGroup(CString& strLocalGroupName, BOOL& bMoreData)
{
	strLocalGroupName = m_lstLocalGroups.GetNext(m_position);
	m_strLocalGroupName = strLocalGroupName;
	bMoreData = m_position != NULL;
	
	return NERR_Success;
}

CString GetLastErrorString(DWORD nError, DWORD parm_err)
{
	CString strError = _T("");
/*
	switch(nError)
	{
	case NERR_Success:
		strError.Empty();
		break;
	case NERR_DCNotFound:
		strError.Format("Could not find the domain controller for the domain."); 
		break;
	case ERROR_INVALID_NAME:
		strError.Format("The name could not be found."); 
		break;
	case NERR_UserExists:
		strError.Format("user already exists.");
		break;
	case ERROR_INVALID_PARAMETER:
		strError.Format("Invalid Parameter Error adding user: Parameter Index = %d", parm_err);
		break;        		
	case ERROR_ACCESS_DENIED:
		strError.Format("The user does not have access to the requested information.");
		break;
	case NERR_InvalidComputer:
		strError.Format("The computer name is invalid.");
		break;
	case NERR_NotPrimary: 
		strError.Format("The operation is allowed only on the primary domain controller of the domain.");
		break;
	case NERR_UserNotFound:
		strError.Format("The user name could not be found.");
		break;
	case NERR_UserNotInGroup:
		strError.Format("The user does not belong to this global group.");
		break;
	case NERR_SpeGroupOp:
		strError.Format("The operation is not allowed on specified special groups, which are user groups, admin groups, local groups, or guest groups."); 
		break;
	case NERR_LastAdmin:
		strError.Format("The operation is not allowed on the last administrative account.");
		break;
	case NERR_BadPassword:
		strError.Format("The share name or password is invalid.");
		break;
	case NERR_PasswordTooShort:
		strError.Format("The password is shorter than required.");
		break;
	case ERROR_INVALID_PASSWORD:
		strError.Format("The user has entered an invalid password.");
		break;
	case NERR_GroupExists: 
	case ERROR_ALIAS_EXISTS:
		strError.Format("The local group already exists."); 
		break;
	case NERR_GroupNotFound:  
		strError.Format("The Global/local group specified by the groupname parameter does not exist.");
		break;
	case ERROR_NO_SUCH_ALIAS:
		strError.Format("The specified local group does not exist.");
		break;
	case ERROR_NO_SUCH_MEMBER:
		strError.Format("One or more of the members specified do not exist. Therefore, no new members were added.");
		break;
	case ERROR_MEMBER_IN_ALIAS:
		strError.Format("One or more of the members specified were already members of the local group. No new members were added.");
		break;
	case ERROR_INVALID_MEMBER:  
		strError.Format("One or more of the members cannot be added because their account type is invalid. No new members were added.");
		break;
	case ERROR_INVALID_LEVEL:
		strError.Format("The Level parameter specifies an invalid value.");
		break;
	}	*/
	return strError;
}

/////////////////////////////////////////////////////////////////////
// Additions

DWORD CNetUsers::GetLogonScript(CString &sLoginScriptPath)
{
	ASSERT(m_strUserName.GetLength());
	
	return GetLogonScript(m_strUserName, sLoginScriptPath);
}

DWORD CNetUsers::GetLogonScript(CString strUserName, CString &sLoginScriptPath)
{
	ASSERT(strUserName.GetLength());
	
	LPWSTR lpszUserName = (LPWSTR)strUserName.AllocSysString();
	LPWSTR lpszServerName = (LPWSTR)m_pDomain.m_strPDC.AllocSysString();
	LPUSER_INFO_1 pusriScript;
	DWORD dwLevel = 1;
	NET_API_STATUS nStatus;
	DWORD dwParmError = 0;
	
	nStatus = NetUserGetInfo(lpszServerName, 
		lpszUserName,  
		dwLevel, 
		(LPBYTE*)&pusriScript);
	
	if(nStatus == NERR_Success)
		sLoginScriptPath = _T(pusriScript->usri1_script_path);
	
	SetLastErrorString(nStatus);
	
	NetApiBufferFree(pusriScript);
	
	SysFreeString(lpszUserName);
	SysFreeString(lpszServerName);
	
	return nStatus;
}

DWORD CNetUsers::SetLogonScript(CString sLoginScriptPath)
{
	ASSERT(m_strUserName.GetLength());
	
	return SetLogonScript(m_strUserName, sLoginScriptPath);
}

DWORD CNetUsers::SetLogonScript(CString strUserName, CString sLoginScriptPath)
{
	LPWSTR lpszUserName = (LPWSTR)strUserName.AllocSysString();
	LPWSTR lpszServerName = (LPWSTR)m_pDomain.m_strPDC.AllocSysString();
	USER_INFO_1009 usriScript;
	DWORD dwLevel = 1009;
	NET_API_STATUS nStatus;
	DWORD dwParmError = 0;
	
	usriScript.usri1009_script_path = (LPWSTR)sLoginScriptPath.AllocSysString();
	
	nStatus = NetUserSetInfo(lpszServerName, 
		lpszUserName,  
		dwLevel, 
		(LPBYTE)&usriScript,
		&dwParmError);
	
	SetLastErrorString(nStatus, dwParmError);
	
	SysFreeString(lpszUserName);
	SysFreeString(lpszServerName);
	
	return nStatus;
}

/////////////////////////////////////////////////////////////////////

#define SID_SIZE 96

DWORD CNetShare::Add(CString sShareName, CString sLocalPath, CString sComment, CString sUserName, DWORD dwPermissions)
{
	NET_API_STATUS nStatus;

    SECURITY_DESCRIPTOR sd;
    PACL pDacl = NULL;
    DWORD dwAclSize;
	LPWSTR pszUserName = sUserName.AllocSysString();
	LPWSTR lpszServerName = m_sServerName.AllocSysString();
	
// initial allocation attempt for Sid
	DWORD cbSid = SID_SIZE;
    PSID pSid = (PSID)HeapAlloc(GetProcessHeap(), 0, cbSid);
    if(pSid)
	{
		WCHAR RefDomain[DNLEN + 1];
		DWORD cchDomain = DNLEN + 1;
		SID_NAME_USE peUse;
		// get the Sid associated with the supplied user/group name
		// force Unicode API since we always pass Unicode string
		if(!LookupAccountNameW(lpszServerName,	// default lookup logic
			pszUserName,				// user/group of interest from commandline
			pSid,						// Sid buffer
			&cbSid,						// size of Sid
			RefDomain,					// Domain account found on (unused)
			&cchDomain,					// size of domain in chars
			&peUse)) 
		{
			// if the buffer wasn't large enough, try again
			if(GetLastError() == ERROR_INSUFFICIENT_BUFFER) 
			{
				pSid = (PSID)HeapReAlloc(GetProcessHeap(), 0, pSid, cbSid);
				if (pSid) 
				{
					cchDomain = DNLEN + 1;

					if(!LookupAccountNameW(lpszServerName,	// default lookup logic
						pszUserName,				// user/group of interest from commandline
						pSid,						// Sid buffer
						&cbSid,						// size of Sid
						RefDomain,					// Domain account found on (unused)
						&cchDomain,					// size of domain in chars
						&peUse)) 
					{
						goto cleanup;
					}
				}
			} 
			else 
				goto cleanup;
		}
		
		// compute size of new acl
		dwAclSize = sizeof(ACL) +
			1 * ( sizeof(ACCESS_ALLOWED_ACE) - sizeof(DWORD) ) +
			GetLengthSid(pSid) ;
	
		// allocate storage for Acl
		pDacl = (PACL)HeapAlloc(GetProcessHeap(), 0, dwAclSize);
		if(pDacl == NULL) goto cleanup;
	
		if(!InitializeAcl(pDacl, dwAclSize, ACL_REVISION))
			goto cleanup;
	
		// grant GENERIC_ALL (Full Control) access
		if(!AddAccessAllowedAce(
			pDacl,
			ACL_REVISION,
			dwPermissions,
			pSid)) goto cleanup;
		
		if(!InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION))
			goto cleanup;
		
		if(!SetSecurityDescriptorDacl(&sd, TRUE, pDacl, FALSE)) goto cleanup;

		SHARE_INFO_502 si502;

		// setup share info structure
		si502.shi502_current_uses = 0;
		si502.shi502_max_uses = SHI_USES_UNLIMITED;
		si502.shi502_netname = (LPTSTR)sShareName.AllocSysString();
		si502.shi502_passwd = NULL;
		si502.shi502_permissions = 0;
		si502.shi502_remark = (LPTSTR)sComment.AllocSysString();
		si502.shi502_type = STYPE_DISKTREE;
		si502.shi502_path = (LPTSTR)sLocalPath.AllocSysString();
		si502.shi502_reserved = 0;
		si502.shi502_security_descriptor = &sd;
		
		nStatus = NetShareAdd((LPTSTR)lpszServerName, 
			502,            // info-level
			(LPBYTE)&si502, // info-buffer
			NULL);			// don't bother with par

		if (nStatus == NERR_DuplicateShare) nStatus = NERR_Success;
		SysFreeString((BSTR)si502.shi502_netname);
		SysFreeString((BSTR)si502.shi502_remark);
		SysFreeString((BSTR)si502.shi502_path);
	}	
cleanup:
	SysFreeString(pszUserName);
	SysFreeString(lpszServerName);
	
    // free allocated resources
    if(pDacl != NULL)
        HeapFree(GetProcessHeap(), 0, pDacl);
	
    if(pSid != NULL)
        HeapFree(GetProcessHeap(), 0, pSid);
	
	return nStatus;
}

DWORD CNetShare::Delete(CString sShareName)
{
	NET_API_STATUS nStatus;
	LPWSTR pszShareName = sShareName.AllocSysString();
	LPWSTR lpszServerName = m_sServerName.AllocSysString();
	
	nStatus = NetShareDel((LPTSTR)lpszServerName, (LPTSTR)pszShareName, 0);
	
	SysFreeString(pszShareName);
	SysFreeString(lpszServerName);
	
	return nStatus;
}

DWORD CNetShare::GetLocalPath(CString sShareName, CString& sLocalPath)
{
	NET_API_STATUS nStatus;
	LPWSTR pszShareName = sShareName.AllocSysString();
	LPWSTR lpszServerName = m_sServerName.AllocSysString();
	
	SHARE_INFO_2* psi = NULL;
	nStatus = NetShareGetInfo((LPTSTR)lpszServerName, (LPTSTR)pszShareName, 2, (LPBYTE*)&psi);
	if (nStatus == NERR_Success)
	{
		sLocalPath = (LPWSTR)psi->shi2_path;
		NetApiBufferFree(psi);
	}
	
	SysFreeString(pszShareName);
	SysFreeString(lpszServerName);
	
	return nStatus;
}

DWORD CNetShare::NetAddConnection(LPCTSTR pszShareName, LPCTSTR pszUser, LPCTSTR pszPassword)
{
	DWORD nStatus = ERROR_INVALID_PARAMETER;
	
	if (pszShareName && *pszShareName)
	{
		NETRESOURCE nr;
		
		nr.dwScope = RESOURCE_GLOBALNET;
		nr.dwType = RESOURCETYPE_DISK;
		nr.dwDisplayType = RESOURCEDISPLAYTYPE_SHARE;
		nr.dwUsage = RESOURCEUSAGE_CONTAINER;
		nr.lpLocalName = TEXT("");
		nr.lpComment = TEXT("");
		nr.lpProvider = TEXT("");
		nr.lpRemoteName = (LPSTR)pszShareName;

		// Make a connection to the network resource.
		nStatus = WNetAddConnection3(NULL, &nr, pszPassword, pszUser, 0);
	}
		
	return nStatus;
}
