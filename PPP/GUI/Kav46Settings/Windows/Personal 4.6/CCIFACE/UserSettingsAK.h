#ifndef __USERSETTINGSAK_H__
#define __USERSETTINGSAK_H__

#include "DEFS.h"
#include "BaseMarshal.h"

//============================================================================================
namespace Private
{
	struct CCIFACEEXPORT CPolicySection_v1 : CCBaseMarshal<PID_POLICY_SECTION>
	{
		CPolicySection_v1() : trait(policy) {}

		enum Traits
		{
			policy,
			taskpolicy,
			grouptask
		};

		std::wstring name;
		Traits trait;
		std::vector<std::wstring> valuenames;

		bool operator==(const CPolicySection_v1 &rhs) const
		{
			return name == rhs.name && valuenames == rhs.valuenames && trait == rhs.trait;
		}

		PoliType &MarshalImpl(PoliType &pt)
		{
			return BaseMarshal::MarshalImpl(pt) << name << SIMPLE_DATA_SER(trait) << valuenames;
		}
	};
}

typedef DeclareExternalName<Private::CPolicySection_v1> CPolicySection;

namespace Private
{
	struct CCIFACEEXPORT CAKStuffStorage_v1 : CCBaseMarshal<PID_AK_STUFF_STORAGE>
	{
		std::wstring m_swOASTaskName;
		std::wstring m_swMCTaskName;
		std::wstring m_swSCTaskName;
		std::wstring m_swOGTaskName;
		std::wstring m_swCKAHTaskName;
		std::wstring m_swUpdaterTaskName; // не используется

		std::vector<CPolicySection> m_PolicySections;

		std::vector<BYTE> m_OASNotificationDescriptionBlob;
		std::vector<BYTE> m_MCNotificationDescriptionBlob;
		std::vector<BYTE> m_SCNotificationDescriptionBlob;
		std::vector<BYTE> m_OGNotificationDescriptionBlob;
		std::vector<BYTE> m_CKAHNotificationDescriptionBlob;
		
		PoliType &MarshalImpl(PoliType &pt)
		{
			return BaseMarshal::MarshalImpl(pt)
					<< m_swOASTaskName
					<< m_swMCTaskName
					<< m_swSCTaskName
					<< m_swOGTaskName
					<< m_swCKAHTaskName
					<< m_swUpdaterTaskName
					<< m_PolicySections
					<< m_OASNotificationDescriptionBlob
					<< m_MCNotificationDescriptionBlob
					<< m_SCNotificationDescriptionBlob
					<< m_OGNotificationDescriptionBlob
					<< m_CKAHNotificationDescriptionBlob
					;
		}
	};
}

typedef DeclareExternalName<Private::CAKStuffStorage_v1> CAKStuffStorage;

#endif // __USERSETTINGSAK_H__