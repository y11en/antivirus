#ifndef __USERSETTINGS_EXCLUDE_H__
#define __USERSETTINGS_EXCLUDE_H__

#include "DEFS.H"
#include "basetypes.h"
#include "BaseMarshal.h"
#include <algorithm>

#include "../cc/Common/Pid.h"

namespace KLUTIL { class CLogBase; }

#pragma warning(disable : 4251) // 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
#pragma warning(disable : 4275) // non - DLL-interface classkey 'identifier' used as base for DLL-interface classkey 'identifier'
#pragma warning(disable : 4786) // 'identifier' : identifier was truncated to 'number' characters in the debug information

//! Задачи, залоченные из AdminKit.
enum ELockedExcludes
{
	LockedExcludes_Nothing = 0,
	LockedExcludes_ODS	= 0x1,
	LockedExcludes_MC	= 0x2,
	LockedExcludes_OAS	= 0x4,
	LockedExcludes_ObjectScan	= 0x8,
};

//! Принадлежность исключения.
enum EExcludeOwner
{
	ExcludeType_User,		//!< Исключение задано пользователем.
	ExcludeType_ODS,		//!< Исключение ODS, задано AdminKit.
	ExcludeType_MC,			//!< Исключение MC, задано AdminKit.
	ExcludeType_OAS			//!< Исключение OAS, задано AdminKit.
};

namespace Private
{
	struct CCIFACEEXPORT CAdminExcludeSettings_v1: public CCBaseMarshal<PID_AMDIN_EXCLUDE_SETTINGS>
	{
		CAdminExcludeSettings_v1()
			: m_LockedExcludes(LockedExcludes_Nothing)
			, m_UseExcludes(LockedExcludes_Nothing)
			, m_LockedRiskware(LockedExcludes_Nothing)
			, m_UseRiskware(LockedExcludes_Nothing)
		{
		}
		
		//! Типы задач, для которых нельзя менять исключения (для которых стоят замочки в AdminKit).
		ELockedExcludes m_LockedExcludes;
		//! Типы задач, для которых надо использовать исключения (для которых стоят галочки в AdminKit).
		ELockedExcludes m_UseExcludes;
		//! Типы задач, для которых нельзя менять включённость/отключенность riskware (для которых стоят замочки в AdminKit).
		ELockedExcludes m_LockedRiskware;
		//! Типы задач, для которых надо использовать riskware (для которых стоят галочки в AdminKit).
		ELockedExcludes m_UseRiskware;

		bool operator==(const CAdminExcludeSettings_v1& rhs) const
		{
			return m_LockedExcludes == rhs.m_LockedExcludes
				&& m_UseExcludes == rhs.m_UseExcludes
				&& m_LockedRiskware == rhs.m_LockedRiskware
				&& m_UseRiskware == rhs.m_UseRiskware
				;
		}

		virtual PoliType& MarshalImpl(PoliType& pt)
		{
			return BaseMarshal::MarshalImpl(pt) << SIMPLE_DATA_SER(m_LockedExcludes)
				<< SIMPLE_DATA_SER(m_UseExcludes)
				<< SIMPLE_DATA_SER(m_LockedRiskware)
				<< SIMPLE_DATA_SER(m_UseRiskware)
				;
		}
	};
}

typedef DeclareExternalName<Private::CAdminExcludeSettings_v1> CAdminExcludeSettings;

namespace Private
{
	//! Исключение из сканирования.
	struct CCIFACEEXPORT CExclude_v1: public CCBaseMarshal<PID_USER_SETTINGS_EXCLUDE_ITEM>
	{
		bool m_bTrueByDefaultYouCanUseIt;	//!< Этот bool можно заиспользовать под что-нибудь, если его дефолт будет true.
		std::wstring m_sMask;	//!< Исключаемый файл или маска.
		std::wstring m_sVerdict;	//!< Вердикт, если любой вердикт, то пустая строка.s
		bool m_bAllTasks;		//!< Флаг, указывающий на то, что используются все задачи (m_Tasks - игнорируется).
		typedef std::vector<std::string> CTaskList;
		CTaskList m_Tasks;	//!< Имена задач, в которых исключение используется.

		virtual PoliType& MarshalImpl(PoliType& pt)
		{
			return BaseMarshal::MarshalImpl(pt) << m_bTrueByDefaultYouCanUseIt << m_sMask << m_sVerdict << m_bAllTasks << m_Tasks;
		}
	};

	struct CCIFACEEXPORT CExclude_v2: public DeclareNextVersion<CExclude_v1>
	{
		EExcludeOwner m_Owner; //!< Принадлежность исключения.
		
		virtual PoliType& MarshalImpl(PoliType& pt)
		{
			return BaseMarshal::MarshalImpl(pt) << SIMPLE_DATA_SER(m_Owner);
		}
	};
}

struct CCIFACEEXPORT CExclude: public DeclareExternalName<Private::CExclude_v2>
{
	CExclude() { m_bTrueByDefaultYouCanUseIt = true; m_bAllTasks = false; m_Owner = ExcludeType_User; }
	CExclude(const std::wstring& sMask)
	{
		m_bTrueByDefaultYouCanUseIt = true;
		m_bAllTasks = false;
		m_sMask = sMask;
		m_Owner = ExcludeType_User;
	}
	bool operator==(const CExclude &rhs) const
	{
		return m_bTrueByDefaultYouCanUseIt == rhs.m_bTrueByDefaultYouCanUseIt && m_sMask == rhs.m_sMask
			&& m_sVerdict == rhs.m_sVerdict && m_Tasks == rhs.m_Tasks
			&& m_Owner == rhs.m_Owner;
	}
	bool operator!=(const CExclude &rhs) const
	{
		return !operator==(rhs);
	}
	void Dump(KLUTIL::CLogBase& log, int index);
};

namespace Private
{
	struct CCIFACEEXPORT CRiskware_v1 : public CCBaseMarshal<PID_USER_SETTINGS_RISKWARE_ITEM>
	{
		std::wstring m_sMask;	//!< Исключаемый файл или маска.
		std::wstring m_sVerdict;	//!< Вердикт, если любой вердикт, то пустая строка.s

		bool operator==(const CRiskware_v1 &rhs) const
		{
			return m_sMask == rhs.m_sMask && m_sVerdict == rhs.m_sVerdict;
		}

		virtual PoliType& MarshalImpl(PoliType& pt)
		{
			return BaseMarshal::MarshalImpl(pt) << m_sMask << m_sVerdict;
		}
	};
}

typedef DeclareExternalName<Private::CRiskware_v1> CRiskware;

namespace Private
{
	struct CCIFACEEXPORT CExcludeList_v1: public CCBaseMarshal<PID_USER_SETTINGS_EXCLUDE_LIST>
	{
		std::vector<CExclude> m_List;
		
		virtual PoliType& MarshalImpl(PoliType& pt)
		{
			return BaseMarshal::MarshalImpl(pt) << m_List;
		}
	};
	
	struct CCIFACEEXPORT CExcludeList_v2: public DeclareNextVersion<CExcludeList_v1>
	{
		CAdminExcludeSettings m_AdminExcludeSettings;

		std::vector<CRiskware> m_RiskwareList;

		virtual PoliType& MarshalImpl(PoliType& pt)
		{
			CExclude::CTaskList m_AdminTasksDeprecated;
			return BaseMarshal::MarshalImpl(pt) << m_AdminExcludeSettings
				<< m_RiskwaredTasks << m_RiskwareList << m_NotUseExcludes << m_AdminTasksDeprecated;
		}

	protected:
		CExclude::CTaskList m_NotUseExcludes;	//!< Имена задач, которые не используют заданные исключения. (Реализация старого поведения: список есть, но не включен)
		CExclude::CTaskList m_RiskwaredTasks;	//!< Имена задач, в которых включено использоваение Riskware.
	};
}

struct CCIFACEEXPORT CExcludeList: public DeclareExternalName<Private::CExcludeList_v2>
{
	CExcludeList();

	bool operator==(const CExcludeList& rhs) const
	{
		return m_List == rhs.m_List
			&& m_AdminExcludeSettings == rhs.m_AdminExcludeSettings
			&& m_RiskwareList == rhs.m_RiskwareList
			&& m_NotUseExcludes == rhs.m_NotUseExcludes
			&& m_RiskwaredTasks == rhs.m_RiskwaredTasks
			;
	}
	bool operator!=(const CExcludeList& rhs) const
	{
		return !operator==(rhs);
	}

	void Dump(KLUTIL::CLogBase& log, LPCSTR pszObjectName);

	//! Используется ли в данной задаче это исключение.
	bool IsTaskUseIt(
		const CExclude& exclude,		//!< [in] Проверяемое исключение.
		LPCSTR pszTaskName,				//!< [in] Имя задачи.
		bool bUseAdminSettings = true	//!< [in] Учитывать админские лочки.
		) const;

	//! Возвращает включено ли использование riskware для данной задачи.
	bool IsTaskUseRiskware(
		LPCSTR pszTaskName,		//!< [in] Имя задачи.
		bool bUseAdminSettings	//!< [in] Учитывать админские лочки.
		) const;

	//! Устанавливает включено ли использование riskware для данной задачи.
	void SetTaskUseRiskware(LPCSTR pszTaskName, bool bUse);

	//! Возвращает включено ли использование исключений для данной задачи.
	bool IsTaskUseExclude(
		LPCSTR pszTaskName,		//!< [in] Имя задачи.
		bool bUseAdminSettings	//!< [in] Учитывать админские лочки.
		) const;
	
	//! Устанавливает включено ли использование исключений для данной задачи.
	void SetTaskUseExclude(LPCSTR pszTaskName, bool bUse);

	//! Встраивает в список исключение для задачи. Если такое исключение есть, то добавляет к нему.
	void AddExclude(LPCWSTR pszMask, LPCWSTR pszVerdict, LPCSTR pszTaskName);

	//! Удаляет исключение для задачи.
	void RemoveExclude(
		LPCWSTR pszMask,
		LPCWSTR pszVerdict,
		LPCSTR pszTaskName,
		const std::vector<std::string>& taskList	//!< [in] Список всех задач (нужен если удаляем задачу из исключения для всех задач)
		);

	//! Добавляет задачу в список исключений. (При создании задачи)
	void AddTask(LPCSTR pszTaskName);
	
	//! Удаляет упоминание задачи из списка исключений. (При удалении задачи)
	void RemoveTask(LPCSTR pszTaskName);

	//! Проверяет, что лочки исключений распространяются на эту ODS задачу
	bool IsOdsTaskPolicyManaged(LPCSTR pszTaskName) const;

	static bool IsTaskNeverUseExcludes(LPCSTR pszTaskName);

private:
	//! Возвращает тип исключения по имени задачи с точки зрения политик AdminKit.
	ELockedExcludes GetExcludeType(LPCSTR pszTaskName) const;
};

#endif // __USERSETTINGS_EXCLUDE_H__
