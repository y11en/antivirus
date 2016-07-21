#ifndef __USERSETTINGS_CKAH_H__
#define __USERSETTINGS_CKAH_H__

#include "DEFS.H"
#include "BaseMarshal.h"
#include "../release_type.h"

#pragma warning(disable : 4251) // 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
#pragma warning(disable : 4275) // non - DLL-interface classkey 'identifier' used as base for DLL-interface classkey 'identifier'
#pragma warning(disable : 4786) // 'identifier' : identifier was truncated to 'number' characters in the debug information

namespace Private
{

struct CCIFACEEXPORT CUserSettingsCKAH_v1 : public CCBaseMarshal<PID_USER_SETTINGS_CKAH>
{
	CUserSettingsCKAH_v1()
		: m_bNotifyUser(true)
		, m_bBan(false)
		, m_dwBanTime(60 * 60)
		, m_bUseStealthMode(false)
#ifdef __WKS
		, m_ActionWhenRebootRequired(actSuspend)
#else
		, m_ActionWhenRebootRequired(actAskUser)
#endif		
	{
	}

	enum RebootAction
	{
		actAskUser,
		actSuspend,
		actRebootImmediately
	};
	
	bool	m_bNotifyUser;
	bool	m_bBan;
	DWORD	m_dwBanTime; // seconds, 0 - no ban
	bool	m_bUseStealthMode;
	RebootAction m_ActionWhenRebootRequired;

	bool operator==(const CUserSettingsCKAH_v1 &that) const
	{
		return m_bNotifyUser == that.m_bNotifyUser
				&& m_bBan == that.m_bBan
				&& m_dwBanTime == that.m_dwBanTime
				&& m_bUseStealthMode == that.m_bUseStealthMode
				&& m_ActionWhenRebootRequired == that.m_ActionWhenRebootRequired
				;
	}
	bool operator!=(const CUserSettingsCKAH_v1 &that) const
	{
		return !operator==(that);
	}

	PoliType &MarshalImpl(PoliType &pt)
    {
        return BaseMarshal::MarshalImpl(pt)
					<< m_bNotifyUser
					<< m_bBan
					<< m_dwBanTime
					<< m_bUseStealthMode
					<< SIMPLE_DATA_SER(m_ActionWhenRebootRequired)
					;
	}
};

} // namespace Private

typedef DeclareExternalName<Private::CUserSettingsCKAH_v1> CUserSettingsCKAH;

#endif // __USERSETTINGS_CKAH_H__