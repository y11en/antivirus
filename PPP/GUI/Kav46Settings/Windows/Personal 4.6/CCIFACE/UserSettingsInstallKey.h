#ifndef __USERSETTINGS_INSTALLKEY_H__
#define __USERSETTINGS_INSTALLKEY_H__

#include "DEFS.H"
#include "BaseMarshal.h"

//******************************************************************************
//
//******************************************************************************
namespace Private
{
class CCIFACEEXPORT CUserSettingsInstallKey_v1 : public CCBaseMarshal<PID_INSTALL_LIC_KEY_TASK_SETTINGS>
{
public:
	CUserSettingsInstallKey_v1() :
		m_bIsCurrentKey(true)
	{
	}
	
	virtual PoliType &MarshalImpl (PoliType &pt)
	{
		return BaseMarshal::MarshalImpl(pt)
					<< m_bIsCurrentKey
					<< m_KeyData
					<< m_sKeyFileName
					;
	}

	bool operator==(const CUserSettingsInstallKey_v1 &that) const
	{
		return m_bIsCurrentKey == that.m_bIsCurrentKey
				&& m_KeyData == that.m_KeyData
				&& m_sKeyFileName == that.m_sKeyFileName
				;
	}

	bool m_bIsCurrentKey;
	std::vector<BYTE> m_KeyData;
	std::wstring m_sKeyFileName;
};

}

typedef DeclareExternalName<Private::CUserSettingsInstallKey_v1> CUserSettingsInstallKey;

#endif
