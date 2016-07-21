#ifndef __CKAH_NOTIFY_H__
#define __CKAH_NOTIFY_H__

#include "../../../../CommonFiles/KLUtil/inl/PoliStorage.h"

namespace KLCKAH
{

class CCKAHNotify
{
public:
    CCKAHNotify ()
    {
        m_BanTime = 0;
    }

    enum CKAHNotifyCmd
	{
		cmdAttackNotify,
		cmdRebootRequest,
		cmdRebootRequestAfterInstallUninstall
	};

	CKAHNotifyCmd	m_Command;

	std::string		m_AttackDescription;
	std::string		m_ProtoDescription;
	std::string		m_AttackerIP;
	USHORT			m_LocalPort;
	time_t			m_time;
    unsigned int    m_BanTime; // in minutes
	
    PoliType &MarshalImpl (PoliType &pt);
};

IMPLEMENT_MARSHAL_ENUM(CCKAHNotify::CKAHNotifyCmd)

inline
PoliType &CCKAHNotify::MarshalImpl (PoliType &pt)
{
	return pt << m_Command << m_AttackDescription << m_ProtoDescription << m_AttackerIP << m_LocalPort;
}

IMPLEMENT_MARSHAL(CCKAHNotify)

} // namespace KLCKAH

#endif