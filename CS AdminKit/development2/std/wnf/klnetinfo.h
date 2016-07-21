/*!
*		(C) 2002 Kaspersky Lab 
*		
*		\file	klnetinfo.h
*		\brief	
*		
*		\author Андрей Брыксин
*		\date	16.10.2002 13:58:16
*		
*		Example:	
*/		


#include "lm.h"
#include <string>
#include <list>

namespace KLWNF
{

struct CNetUserInfo
{
	std::wstring name;
	std::wstring password;
	DWORD password_age;
	DWORD priv;
	std::wstring home_dir;
	std::wstring comment;
	DWORD flags;
	std::wstring script_path;
	DWORD auth_flags;
	std::wstring full_name;
	std::wstring usr_comment;
	std::wstring parms;
	std::wstring workstations;
	DWORD last_logon;
	DWORD last_logoff;
	DWORD acct_expires;
	DWORD max_storage;
	DWORD units_per_week;
	PBYTE logon_hours;
	DWORD bad_pw_count;
	DWORD num_logons;
	std::wstring logon_server;
	DWORD country_code;
	DWORD code_page;
	DWORD user_id;
	DWORD primary_group_id;
	std::wstring profile;
	std::wstring home_dir_drive;
	BOOL password_expired;
};

////////////////////////////////////////////////////////////////////////////////
// CNetDomain

class CNetDomain
{
public:
	CNetDomain(const wchar_t* pszDomainName, bool bDontThrowAccessDeniedException = false){
		SetDomainName(pszDomainName);
	}
	
	CNetDomain(bool bDontThrowAccessDeniedException = false){
		SetDomainName(L"");
	}

	virtual ~CNetDomain(){}

	void SetDomainName(const wchar_t* pszDomainName) {
		m_sDomainName = pszDomainName;
	}

	std::wstring GetDomainName() const {
		return m_sDomainName;
	}

	std::wstring GetPDC();
	void SetPDC(const wchar_t* pszPDC)
	{
		KLSTD_CHKINPTR(pszPDC);

		m_sPDC = pszPDC;
	}
	std::wstring GetAnyDC();

	template<class T>
	void GetComputers(std::list<T>& arWorkstations)
	{
		NET_DISPLAY_MACHINE* pBuff, *p;
		DWORD dwRec, i = 0;

		DWORD nStatus;
		do
		{ 
			// Call the NetQueryDisplayInformation function; specify information level 2 (computer information).
			nStatus = NetQueryDisplayInformation(GetPDC().c_str(), 2, i, 1000, 0xFFFFFFFF, &dwRec, (void**)&pBuff);
			if ((nStatus == ERROR_SUCCESS) || (nStatus == ERROR_MORE_DATA))
			{
				p = pBuff;
				for(;dwRec>0;dwRec--)
				{
					// Skip disabled accounts
					if ((p->usri2_flags & UF_ACCOUNTDISABLE) || (p->usri2_flags & UF_LOCKOUT))
						continue;

					if (p->usri2_name)
					{
						p->usri2_name[wcslen(p->usri2_name) - 1] = 0;
						LPCWSTR szwHost=p->usri2_name;
						if (szwHost[0] == L'\\' && szwHost[1] == L'\\') szwHost=&szwHost[2];
						arWorkstations.push_back(T(szwHost));
					}
					i = p->usri2_next_index;	// If there is more data, set the index.
					p++;
				}
				NetApiBufferFree(pBuff);
			}
			else
				KLSTD_THROW_LASTERROR_CODE2(nStatus);
		} 
		while(nStatus == ERROR_MORE_DATA);	// Continue while there is more data.
	}

	bool IsDomain();
	
protected:
	std::wstring m_sPDC;
	std::wstring m_sDomainName;
};

////////////////////////////////////////////////////////////////////////////////
// CNetUsers

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
		opPrivPrint = AF_OP_PRINT,			//The print operator privilege is enabled. 
		opPrivComm = AF_OP_COMM,			//The communications operator privilege is enabled. 
		opPrivServer = AF_OP_SERVER,		//The server operator privilege is enabled. 
		opPrivAccounts = AF_OP_ACCOUNTS		//The accounts operator privilege is enabled. 
	};
	
	CNetUsers(CNetDomain Domain)
	{
		m_nextUsrIndex = 0;
		m_Domain = Domain;
	}

	CNetUsers() {
		m_nextUsrIndex = 0;
	}
	
	virtual ~CNetUsers(){}	

	CNetUsers& operator=(CNetUsers&) { // assignment operator
        return *this;
	}

	void Add(const wchar_t* pszUserName, const wchar_t* pszPassword);
	void Delete(const wchar_t* pszUserName);

	void SetPassword(const wchar_t* pszUserName, const wchar_t* pszPassword);
	void ChangePassword(const wchar_t* pszUserName, const wchar_t* pszOldPassword, const wchar_t* pszNewPassword);
	
	void GetUserInfo(const wchar_t* pszUserName, CNetUserInfo* pUserInfo);

	bool IsForcedChangePassword(const wchar_t* pszUserName);
	void ForceChangePassword(const wchar_t* pszUserName, bool bChangePass);

	bool IsUserDisable(const wchar_t* pszUserName);
	void DisableUser(const wchar_t* pszUserName, bool bDisable);

	std::wstring GetFullName(const wchar_t* pszUserName);
	void SetFullName(const wchar_t* pszUserName, const wchar_t* pszFullName);

	void SetComment(const wchar_t* pszUserName, const wchar_t* pszComment);
	std::wstring GetComment(const wchar_t* pszUserName);

	std::wstring GetHomeDirectory(const wchar_t* pszUserName);
	void SetHomeDirectory(const wchar_t* pszUserName, const wchar_t* pszHomeDirectory);

	int GetUserPrivilege(const wchar_t* pszUserName);
	void SetUserPrivilege(const wchar_t* pszUserName, int nPrivilege);

	void SetWorkStations(const wchar_t* pszUserName, const wchar_t* pszWorkStations);
	std::wstring GetWorkStations(const wchar_t* pszUserName);

	long GetBadPasswordCount(const wchar_t* pszUserName);

	long GetLogonCount(const wchar_t* pszUserName);
	DWORD GetLastLogonTime(const wchar_t* pszUserName);
	DWORD GetLastLogoffTime(const wchar_t* pszUserName);

	DWORD GetPasswordAge(const wchar_t* pszUserName);

	bool GetLogonScript(const wchar_t* pszUserName, std::wstring& sLogonScript);
	void SetLogonScript(const wchar_t* pszUserName, const wchar_t* pszLoginScriptPath);
    void RemoveLogonScript(const wchar_t* pszUserName);

	void StartQueryNextUser();
	bool QueryNextUser(std::wstring& sUserName, bool& bDisabledAccount);
	
	void GetUserGroups(const wchar_t* pszUserName, std::list<std::wstring>& groups);
protected:
	CNetDomain m_Domain;
	DWORD m_nextUsrIndex;
};

////////////////////////////////////////////////////////////////////////////////
// CNetShare

class CNetShare
{
public:
	CNetShare(){
	}
	
	CNetShare(const wchar_t* pszServerName) {
		SetServerName(pszServerName);
	}

	void SetServerName(const wchar_t* pszServerName) {
		m_sServerName = pszServerName?pszServerName:L"";
	}

	void Add(const wchar_t* pszShareName, const wchar_t* pszLocalPath, const wchar_t* pszComment = NULL, const wchar_t* pszUserName = L"Everyone", DWORD dwPermissions = GENERIC_ALL);
	void Add(const wchar_t*pszShareName, const wchar_t* pszLocalPath, const wchar_t* pszComment, PSECURITY_DESCRIPTOR psd);
	void Delete(const wchar_t* pszShareName);
	std::wstring GetLocalPath(const wchar_t* pszShareName);
	void NetAddConnection(const wchar_t* pszShareName, const wchar_t* pszUser, const wchar_t* pszPassword);

protected:
	std::wstring m_sServerName;
	
	PSID _CreateSID(DWORD dwAuthority);
};

}