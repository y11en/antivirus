//
//  MODULE:   Net.h
//
//	AUTHOR: Carlos Antollini 
//
//  mailto: cantollini@hotmail.com
//
//	Date: 11/29/2001
//
//	Version 1.00
// 

//#include <afx.h>
#include "lm.h"
//#include <io.h>
#include <afxtempl.h>

CString GetLastErrorString(DWORD nError, DWORD parm_err = 0);

struct CNetUserInfo
{
	LPSTR	name;
	LPSTR	password;
	DWORD	password_age;
	DWORD	priv;
	LPSTR	home_dir;
	LPSTR	comment;
	DWORD	flags;
	LPSTR	script_path;
	DWORD	auth_flags;
	LPSTR	full_name;
	LPSTR	usr_comment;
	LPSTR	parms;
	LPSTR	workstations;
	DWORD	last_logon;
	DWORD	last_logoff;
	DWORD	acct_expires;
	DWORD	max_storage;
	DWORD	units_per_week;
	PBYTE	logon_hours;
	DWORD	bad_pw_count;
	DWORD	num_logons;
	LPSTR	logon_server;
	DWORD	country_code;
	DWORD	code_page;
	DWORD	user_id;
	DWORD	primary_group_id;
	LPSTR	profile;
	LPSTR	home_dir_drive;
	BOOL	password_expired;
};


class CNetDomain
{

public:

	CNetDomain(CString strDomainName) {
		SetDomainName(strDomainName);
	}
	
	/*
	CNetDomain(BOOL bWorkGroup)
	{
		m_strDomainName = _T("");
		if(!bWorkGroup)
			InitNetInfo();
	}
	*/
	
	CNetDomain()
	{
		m_strDomainName = _T("");
	}

	~CNetDomain()
	{
	
	}

	void SetDomainName(CString strDomainName) {
		m_strDomainName = strDomainName;
		InitNetInfo();
	};
	CString GetDomainName()	{
		return m_strDomainName;
	};
	DWORD GetPDC(CString& strPDC);
	DWORD GetAnyDC(CString& strDC);
	
protected:
	DWORD InitNetInfo();
	void SetLastErrorString(DWORD nError, DWORD parm_err = 0)
		{m_strError = GetLastErrorString(nError, parm_err);};
	
public:
	CString m_strPDC;
	CString m_strError;

protected:
	CString m_strDomainName;
};

class CNetUsers
{
public:
	enum userPrivileges
	{
		usrPrivGuest = USER_PRIV_GUEST,		//Guest 
		usrPrivUser = USER_PRIV_USER,		//User
		usrPrivAdmin = USER_PRIV_ADMIN		//Administrator
	};

	enum operatorPrivileges
	{
		opPrivPrint = AF_OP_PRINT,		//The print operator privilege is enabled. 
		opPrivComm = AF_OP_COMM,		//The communications operator privilege is enabled. 
		opPrivServer = AF_OP_SERVER,	//The server operator privilege is enabled. 
		opPrivAccounts = AF_OP_ACCOUNTS	//The accounts operator privilege is enabled. 
	};
	
	CNetUsers(CNetDomain pDomain)
	{
		m_nextUsrIndex = 0;
		m_pDomain = pDomain;
		m_strUserName = _T("");
		m_strError = _T("");
	}

	CNetUsers()
	{
		m_nextUsrIndex = 0;
		m_strUserName = _T("");
		m_strError = _T("");
	}
	
	~CNetUsers()
	{
		//m_nextUsrIndex = 0;
		m_strUserName = _T("");
		m_strError = _T("");
		//m_lstUsers.RemoveAll();
	}	

	CNetUsers& operator=(CNetUsers&)  // assignment operator
        {return *this;}

	DWORD Add(CString strUser, CString strPassword);
	DWORD Delete(CString strUserName);
	void SetUser(CString strUserName)
		{m_strUserName = strUserName;};
	CString GetUser()
		{return m_strUserName;};
	DWORD SetPassword(CString strUserName, CString strPassword);
	DWORD SetPassword(CString strPassword);
	DWORD ChangePassword(CString strUserName, CString strOldPassword, CString strNewPassword);
	DWORD ChangePassword(CString strOldPassword, CString strNewPassword);
	DWORD IsForcedChangePassword(BOOL& bChangePass);
	DWORD IsForcedChangePassword(CString strUserName, BOOL& bChangePass);
	DWORD ForceChangePassword(CString strUserName, BOOL bChangePass = TRUE);
	DWORD ForceChangePassword(BOOL bChangePass = TRUE);
	DWORD DisableUser(CString strUserName, BOOL bDisable = TRUE);
	DWORD DisableUser(BOOL bDisable = TRUE);
	DWORD GetUserInfo(CString strUserName, CNetUserInfo* pUserInfo);
	DWORD GetUserInfo(CNetUserInfo* pUserInfo);
	DWORD GetFullName(CString& strFullName);
	DWORD GetFullName(CString strUserName, CString& strFullName);
	DWORD SetFullName(CString strUserName, CString strFullName);
	DWORD SetFullName(CString strFullName);
	DWORD SetComment(CString strComment);
	DWORD SetComment(CString strUserName, CString strComment);
	DWORD GetHomeDirectory(CString& strHomeDirectory);
	DWORD GetHomeDirectory(CString strUserName, CString& strHomeDirectory);
	DWORD SetHomeDirectory(CString strHomeDirectory);
	DWORD SetHomeDirectory(CString strUserName, CString strHomeDirectory);
	DWORD GetUserPrivilege(int& nPrivilege);
	DWORD GetUserPrivilege(CString strUserName, int& nPrivilege);
	DWORD SetUserPrivilege(int nPrivilege);
	DWORD SetUserPrivilege(CString strUserName, int nPrivilege);
	DWORD SetOperatorPrivilege(int nPrivileges);
	DWORD SetOperatorPrivilege(CString strUserName, int nPrivileges);
	DWORD GetNotChangePass(BOOL& bNotPermit);
	DWORD GetNotChangePass(CString strUserName, BOOL& bNotPermit);
	DWORD SetNotChangePass(BOOL bPermit = FALSE);
	DWORD SetNotChangePass(CString strUserName, BOOL bPermit = FALSE);
	DWORD SetWorkStations(CString strWorkStations);
	DWORD SetWorkStations(CString strUserName, CString strWorkStations);
	DWORD GetWorkStations(CString& strWorkStations);
	DWORD GetWorkStations(CString strUserName, CString& strWorkStations);
	DWORD GetComment(CString& strComment);
	DWORD GetComment(CString strUserName, CString& strComment);
	DWORD GetBadPasswordCount(long& nCount);
	DWORD GetBadPasswordCount(CString strUserName, long& nCount);
	DWORD GetLogonCount(long& nCount);
	DWORD GetLogonCount(CString strUserName, long& nCount);
	DWORD GetPasswordAge(DWORD& nSeconds);
	DWORD GetPasswordAge(CString strUserName, DWORD& nSeconds);
	DWORD IsUserDisable(BOOL& bDisable);
	DWORD IsUserDisable(CString strUserName, BOOL& bDisable);
	DWORD StartQueryNextUser();
	DWORD QueryNextUser(CString& strUserName, BOOL& bMoreData, BOOL& bDisabledAccount);
	DWORD FindFirstUser(BOOL& bMoreData);
	DWORD FindFirstUser(CString& strUserName, BOOL& bMoreData);
	DWORD FindNextUser(BOOL& bMoreData);
	DWORD FindNextUser(CString& strUserName, BOOL& bMoreData);

// Addition
	DWORD SetLogonScript(CString sLoginScriptPath);
	DWORD SetLogonScript(CString strUserName, CString sLoginScriptPath);
	DWORD GetLogonScript(CString& sLoginScriptPath);
	DWORD GetLogonScript(CString strUserName, CString& sLoginScriptPath);
	
protected:
	void SetLastErrorString(DWORD nError, DWORD parm_err = 0)
		{m_strError = GetLastErrorString(nError, parm_err);};

public:
	CString m_strUserName;
	CString m_strError;
	
protected:
	CNetDomain m_pDomain;
	DWORD m_nextUsrIndex;
	CList<CString, CString> m_lstUsers;
	POSITION m_position;
	
};


class CNetGroups
{
public:
	CNetGroups(CNetDomain pDomain)
	{
		m_pDomain = pDomain;
		m_strGroupName = _T("");
		m_strError = _T("");
	}

	CNetGroups()
	{
		m_strGroupName = _T("");
		m_strError = _T("");
	}
	
	~CNetGroups()
	{
		m_strGroupName = _T("");
		m_strError = _T("");
		m_lstGroups.RemoveAll();
	}

	CNetGroups& operator=(CNetGroups&)  // assignment operator
        {return *this;}

	DWORD Add(CString strGroupName, CString strComments = _T(""));
	DWORD AddUser(CString strGroupName, CString strUserName);
	DWORD AddUser(CString strUserName);
	DWORD DeleteUser(CString strGroupName, CString strUserName);
	DWORD DeleteUser(CString strUserName);
	DWORD Delete();
	DWORD Delete(CString strGroupName);
	DWORD FindFirstGroup(BOOL& bMoreData);
	DWORD FindFirstGroup(CString& strGroupName, BOOL& bMoreData);
	DWORD FindNextGroup(BOOL& bMoreData);
	DWORD FindNextGroup(CString& strGroupName, BOOL& bMoreData);
	CString GetGroup()
		{return m_strGroupName;};

protected:
	void SetLastErrorString(DWORD nError, DWORD parm_err = 0)
		{m_strError = GetLastErrorString(nError, parm_err);};
	
public:
	CString m_strGroupName;
	CString m_strError;
	
protected:
	CNetDomain m_pDomain;
	CList<CString, CString> m_lstGroups;
	POSITION m_position;
};

class CNetLocalGroups
{
public:
	CNetLocalGroups(CNetDomain pDomain)
	{
		m_pDomain = pDomain;
		m_strLocalGroupName = _T("");
		m_strError = _T("");
	}

	CNetLocalGroups()
	{
		m_strLocalGroupName = _T("");
		m_strError = _T("");
	}
	
	~CNetLocalGroups()
	{
		m_strLocalGroupName = _T("");
		m_strError = _T("");
		m_lstLocalGroups.RemoveAll();
	}

	CNetLocalGroups& operator=(CNetLocalGroups&)  // assignment operator
        {return *this;}

	DWORD Add(CString strLocalGroupName, CString strComments = _T(""));
	DWORD AddUser(CString strLocalGroupName, CString strUserName);
	DWORD AddUser(CString strUserName);
	DWORD DeleteUser(CString strLocalGroupName, CString strUserName);
	DWORD DeleteUser(CString strUserName);
	DWORD Delete();
	DWORD Delete(CString strLocalGroupName);
	DWORD FindFirstGroup(BOOL& bMoreData);
	DWORD FindFirstGroup(CString& strLocalGroupName, BOOL& bMoreData);
	DWORD FindNextGroup(BOOL& bMoreData);
	DWORD FindNextGroup(CString& strLocalGroupName, BOOL& bMoreData);
	CString GetGroup()
		{return m_strLocalGroupName;};

protected:
	void SetLastErrorString(DWORD nError, DWORD parm_err = 0)
		{m_strError = GetLastErrorString(nError, parm_err);};
	
public:
	CString m_strLocalGroupName;
	CString m_strError;
	
protected:
	CNetDomain m_pDomain;
	CList<CString, CString> m_lstLocalGroups;
	POSITION m_position;
};

class CNetShare
{
public:
	CString m_sServerName;

	CNetShare(){
	}
	
	CNetShare(LPCTSTR pszServerName) {
		m_sServerName = pszServerName;
	}

	void SetServerName(LPCTSTR pszServerName) {
		m_sServerName = pszServerName;
	}

	DWORD Add(CString sShareName, CString sLocalPath, CString sComment = "", CString sUserName = "Everyone", DWORD dwPermissions = GENERIC_ALL);
	DWORD Delete(CString sShareName);
	DWORD GetLocalPath(CString sShareName, CString& sLocalPath);
	DWORD NetAddConnection(LPCTSTR pszShareName, LPCTSTR pszUser, LPCTSTR pszPassword);
};