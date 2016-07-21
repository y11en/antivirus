#ifndef __USERSETTINGS_LOGON_CREDENTIALS_H__
#define __USERSETTINGS_LOGON_CREDENTIALS_H__

#include "DEFS.H"
#include "BaseMarshal.h"

//******************************************************************************
//
//******************************************************************************
namespace Private
{
class CCIFACEEXPORT CLogonCredentials_v1 : public CCBaseMarshal<PID_LOGON_CREDENTIALS>
{
public:
	CLogonCredentials_v1 () :
		m_bUseCredentials (false)
    {
    }

	typedef std::vector<BYTE> EncodedString;

	virtual PoliType &MarshalImpl (PoliType &pt)
	{
		return BaseMarshal::MarshalImpl (pt) << m_bUseCredentials << m_Domain << m_UserName << m_EncryptedPassword;
	}

	bool m_bUseCredentials;
	std::string m_Domain;
	std::string m_UserName;

	EncodedString m_EncryptedPassword;
};

class CCIFACEEXPORT CLogonCredentials_v2 : public DeclareNextVersion<CLogonCredentials_v1>
{
public:
	CLogonCredentials_v2(): m_ePasswordChanged(ePasswordNotChanged) {}
	
	std::string m_sPassword;	//!< Передаётся только из GUI в сервис.
	enum 
	{
		ePasswordNotChanged,	//!< Ничего не поменялось.
		ePasswordChanged,		//!< В m_sPassword новый пароль, m_EncryptedByAKPassword не валидный.
		ePasswordAndAKBlobChanged,	//!< В m_sPassword новый пароль, m_EncryptedByAKPassword новый блок, соответствующий m_sPassword.
	} m_ePasswordChanged;	//!< Передаётся только из GUI в сервис. Если true, то в m_sPassword новый пароль.

	EncodedString m_EncryptedByAKPassword; //!< Зашифрованный AdminKit пароль.

	virtual PoliType &MarshalImpl (PoliType &pt)
	{
		return BaseMarshal::MarshalImpl (pt) << m_sPassword
			<< SIMPLE_DATA_SER(m_ePasswordChanged) << m_EncryptedByAKPassword;
	}
};

}

typedef DeclareExternalName<Private::CLogonCredentials_v2> CLogonCredentials;

#endif //__USERSETTINGS_LOGON_CREDENTIALS_H__