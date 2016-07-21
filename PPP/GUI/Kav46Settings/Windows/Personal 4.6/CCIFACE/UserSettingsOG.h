#ifndef __USERSETTINGS_OG_H__
#define __USERSETTINGS_OG_H__

#include "DEFS.H"
#include "BaseMarshal.h"
#include <algorithm>

#pragma warning(disable : 4251) // 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
#pragma warning(disable : 4275) // non - DLL-interface classkey 'identifier' used as base for DLL-interface classkey 'identifier'
#pragma warning(disable : 4786) // 'identifier' : identifier was truncated to 'number' characters in the debug information


//******************************************************************************
// Настроки Office Guard
//******************************************************************************

//! События OfficeGuard.
enum EOffGuardEvents
{
	ogWORD_OPTIONS_SAVENORMALPROMPT,	//!< Отключение запроса на сохранение шаблона Normal
	ogWORD_OPTIONS_VIRUSPROTECTION,	//!< Отключение защиты от вирусов
	ogWORD_MACROCOPY,	//!< Команда MacroCopy
	ogWORD_ORGANIZER_COPY,	//!< Копирование модулей (OrganizerCopy)
	ogWORD_ORGANIZER_DELETE,	//!< Удаление модулей (OrganizerDelete)
	ogWORD_ORGANIZER_RENAME,	//!< Переименование модулей (OrganizerRename)
	ogVBCA,	//!< Добавить модуль
	ogVBCR,	//!< Удаление модуля
	ogVBCE,	//!< Экспорт модулей
	ogVBCI,	//!< Импорт модулей
	ogADDFROMFILE,	//!< Вставка строк макроса в модуль из файла
	ogADDFROMSTRING,	//!< Добавление строк в макрос
	ogINSERTLINE,	//!< Вставка строк в макрос
	ogREPLACELINE,	//!< Замена строк в макросе
	ogDELETELINE,	//!< Удаление строк макроса
	ogCREATEPROCEDURE,	//!< Создание процедуры
	ogOPENFORWRITE,	//!< Открытие файла в режиме записи
	ogCHANGEFILEATTR,	//!< Смена аттрибутов файла
	ogKILLFILE,	//!< Удаление файлов
	ogMAKEDIR,	//!< Создание папок
	ogREMOVEDIR,	//!< Удаление папок
	ogSHELLCOMMAND,	//!< Команды Shell
	ogCREATEOBJECT,	//!< Создание объектов ActiveX
	ogCREATEOBJECT2,	//!< Создание объекта ActiveX на удалённом компьютере
	ogGETOBJECT,	//!< Доступ к ActiveX объекту
	ogSENDKEYS,	//!< Эмуляций нажатия клавиш
	ogEXCEL_COPY,	//!< Копирование листов Excel
	ogAPIFUNCTION,	//!< Вызов API-функции
	ogCLOSEFILE_notused_bug16913,	//!< Закрытие файла
	ogOTHER,	//!< Прочие команды
	ogVBCS,	//!< Модули
	ogMACRO,	//!< Макросы
	ogFILEOPERATIONS,	//!< Файловые операции

	ogLast
};

//! Действия на события OfficeGuard.
enum EUserAnswer
{
	uaAskUser = 0,	//!< Спросить пользователя.
	uaAllow,		//!< Разрешить выполнение.
	uaBlockMacro,	//!< Блокировать выполнение макроса.
	uaBlockScript,	//!< Блокировать выполнение скрипта.
	
	uaDefaultForUnknown = uaAskUser,	//!< Действие по умолчанию для неизвестных событий (могут возникнуть в случае изменения перехватчика OfficeGuard).
	uaDefault = uaBlockMacro	//!< Действие по умолчанию (если по каким-то причинам нет возможности спросить пользователя).
};

IMPLEMENT_MARSHAL_ENUM(EOffGuardEvents);
IMPLEMENT_MARSHAL_ENUM(EUserAnswer);

namespace Private
{
	struct CEventAnswer
	{
		CEventAnswer(EOffGuardEvents ev_ = ogVBCA): ev(ev_), answ(uaDefaultForUnknown) {}
		EOffGuardEvents ev;
		EUserAnswer answ;
		bool operator<(const CEventAnswer& rhs) const { return ev < rhs.ev; }
		bool operator==(const CEventAnswer& rhs) const { return ev == rhs.ev  && answ == rhs.answ; }
		PoliType & MarshalImpl(PoliType& pt)
		{
			return pt << ev << answ;
		}
	};
	
	IMPLEMENT_MARSHAL(Private::CEventAnswer);
	
	class CCIFACEEXPORT COfficeGuardSettings_v1 : public CCBaseMarshal<PID_USER_SETTINGS_OFFICE_GUARD>
	{
	public:
		typedef std::vector< CEventAnswer > CIdToAction;
		
		virtual PoliType & MarshalImpl(PoliType& pt)
		{
			return BaseMarshal::MarshalImpl (pt) << m_IdToAction;
		}
		
	protected:
		mutable CIdToAction m_IdToAction;
	};
} // namespace Private

struct CCIFACEEXPORT COfficeGuardSettings: public DeclareExternalName<Private::COfficeGuardSettings_v1>
{
	COfficeGuardSettings() { PPro_SetRecommended(); }

	EUserAnswer GetAnswer(EOffGuardEvents ev) const { return Get(ev).answ; }
	void SetAnswer(EOffGuardEvents ev, EUserAnswer answ) { Get(ev).answ = answ; }
	const CIdToAction& GetActionList() const { return m_IdToAction; }
	
	void PPro_SetMaximalProtection();
	void PPro_SetRecommended();
	void PPro_SetMaximalSpeed();

	bool PPro_IsMaximalProtection() const;
	bool PPro_IsRecommended() const;
	bool PPro_IsMaximalSpeed() const;

	bool operator == (const COfficeGuardSettings & rhs) const
	{
		return m_IdToAction == rhs.m_IdToAction;
	}
	
private:
	Private::CEventAnswer& Get(EOffGuardEvents ev) const
	{
		CIdToAction::iterator it = std::lower_bound(
			m_IdToAction.begin(), m_IdToAction.end(), Private::CEventAnswer(ev));
		if (it != m_IdToAction.end() && it->ev == ev)
			return *it;
		return *m_IdToAction.insert(it, Private::CEventAnswer(ev));
	}
};

#endif // __USERSETTINGS_OG_H__