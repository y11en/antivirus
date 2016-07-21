#ifndef __WASCMESS_H
#define __WASCMESS_H

#include <windows.h>


///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
struct CTTMessageInfo {
	UINT   m_nWindowID;					// Идентификатор окна
	int		 m_nItemIndex;				// Индекс узла
	BOOL   m_bRepeate;				  // Повторить команду
	union {
		TCHAR  *m_pItemText;			// Текст после редактирования
	  VOID   *m_pStringList;		// Указатель на экземпляр CStringList, подлежащий заполнению
		UINT    m_nScrollGroup;		// Индексы в группе скроллирования 
															// LOWORD - начальный индекс в группе, HIWORD - конечный индекс в группе     
		HMENU   m_hMenu;					// Handle Context-меню
		DWORD   m_ToolBarStuff[2];// Context-меню toolbar ID[0]/Handle[1]
		DWORD   m_iCommandId;			// Идентификатор команды Context-меню
		BOOL    m_bSetRemove;			// Флаг - "Установить/Снять"
		DWORD   m_aUpDownStuff[3];// Параметры UpDown [0]-MinValue; [1]-MaxValue; [2]-Step
		TCHAR  *m_pDateFormat;		// Формат вывода даты
		TCHAR  *m_pTimeFormat;		// Формат вывода времени
		TCHAR  *m_pDateTimeFormat;// Формат вывода даты-времени
		WORD    m_aDateValue[4];	// Значение даты - первые 4 слова SYSTEMTIME
		WORD    m_aTimeValue[4];	// Значение времени - вторые 4 слова SYSTEMTIME
		WORD    m_aDateTimeValue[8];	// Значение даты-времени - SYSTEMTIME
		BYTE    m_aIPValue[4];	  // Значение IP адреса
		WORD    m_aPIDValue[4];	  // Значение PID
	} m_Value;
};

// Сообщение об намерении установить Checked
// WPARAM		- LPBOOL - указатель на результат - TRUE - если замена Checked разрешена
//            По-умолчанию - TRUE
// LPARAM		- указатель на экземпляр структуры CTTMessageInfo
//						m_Value.m_bSetRemove - TRUE - установить, FALSE - снять
// LRESULT	- не используется
UINT const CTTM_CHECKEDCHANGING     = (0xC000 - 1);

// Сообщение об изменении состояния
// WPARAM		- LPBOOL - указатель на результат - не используется
// LPARAM		- указатель на экземпляр структуры CTTMessageInfo
// LRESULT	- не используется
UINT const CTTM_STATECHANGED        = (0xC000 - 2);

// Запрос к Parent на выполнение внешнего редактирования текста элемента
// WPARAM		- LPBOOL - указатель на результат - TRUE - если редактирование выполнено
//            По-умолчанию - FALSE
// LPARAM		- указатель на экземпляр CTTMessageInfo 
//            m_Value.m_pItemText - копия текста узла
//						m_bRepeate					- флаг повторения команды, по-умолчанию	= FALSE;
// LRESULT	- не используется
UINT const CTTM_DOEXTERNALEDITING   = (0xC000 - 3);

// Запрос на скроллированиие
// WPARAM		- LPBOOL - указатель на результат - TRUE - если "скроллирование" выполнено
//            По-умолчанию - FALSE
// LPARAM		- указатель на экземпляр CTTMessageInfo 
//            m_Value.m_ScrollGroup - группа скроллирования
// LRESULT	- не используется
UINT const CTTM_VSCROLLUP			  	  = (0xC000 - 4);
UINT const CTTM_VSCROLLDOWN			    = (0xC000 - 5);

// Сообщение о начале редактирования текста узла
// WPARAM		- LPBOOL - указатель на результат - TRUE - если редактирование разрешено
//            По-умолчанию - TRUE
// LPARAM		- указатель на экземпляр CTTMessageInfo 
//            m_Value.m_pItemText - копия текста узла
// LRESULT	- не используется
UINT const CTTM_BEGINLABELEDIT			= (0xC000 - 6);

// Сообщение о завершении редактирования текста узла
// WPARAM		- LPBOOL - указатель на результат - TRUE - если замена текста разрешена
//            По-умолчанию - TRUE
// LPARAM		- указатель на экземпляр CTTMessageInfo 
//            m_Value.m_pItemText - копия отредактированного текста
// LRESULT	- не используется
UINT const CTTM_ENDLABELEDIT			  = (0xC000 - 7);

// Сообщение об отказе от редактирования текста узла
// WPARAM		- LPBOOL - указатель на результат - TRUE - если замена текста разрешена
//            По-умолчанию - FALSE
// LPARAM		- указатель на экземпляр CTTMessageInfo 
//            m_Value.m_pItemText - NULL
// LRESULT	- не используется
UINT const CTTM_CANCELLABELEDIT			= (0xC000 - 8);

// Сообщение о возможности выполнения PostEdit операций
// WPARAM		- LPBOOL - указатель на результат - TRUE - если обработка выполнена
//            По-умолчанию - FALSE
// LPARAM		- указатель на экземпляр CTTMessageInfo 
//            m_Value.m_pItemText - копия текста узла
// LRESULT	- не используется
UINT const CTTM_POSTLABELEDIT				= (0xC000 - 9);

// Запрос на заполнение состава ComboBox 
// WPARAM		- LPBOOL - указатель на результат - TRUE - если загрузка выполнена
//            По-умолчанию - FALSE
// LPARAM		- указатель на экземпляр CTTMessageInfo
//            m_Value.m_StringList - указатель на экземпляр CStringList, подлежащий заполнению
// LRESULT	- не используется
UINT const CTTM_LOADCOMBOCONTENTS		= (0xC000 - 10);

// Запрос на установку параметров UpDown Edit 
// WPARAM		- LPBOOL - указатель на результат - TRUE - если установка выполнена
//            По-умолчанию - FALSE
// LPARAM		- указатель на экземпляр CTTMessageInfo
// 		        m_Value.m_UpDownStuff - параметры UpDown [0]-MinValue; [1]-MaxValue; [2]-Step
// LRESULT	- не используется
UINT const CTTM_LOADUPDOWNCONTEXT		= (0xC000 - 11);

// Запрос на удаление элемента 
// WPARAM		- LPBOOL - указатель на результат - TRUE - если удаление выполнено
//            По-умолчанию - FALSE
// LPARAM		- указатель на экземпляр CTTMessageInfo 
//            m_Value.m_pItemText - копия текста узла
// LRESULT	- не используется
UINT const CTTM_REMOVEITEM          = (0xC000 - 12);

// Запрос на обработку меню
// WPARAM		- LPBOOL - указатель на результат - TRUE - если обработка выполнена
//            По-умолчанию - TRUE
// LPARAM		- указатель на экземпляр CTTMessageInfo 
//            m_Value.m_hMenu - Handle Context-меню
// LRESULT	- не используется
UINT const CTTM_PREPROCESSCONTEXTMENU = (0xC000 - 13);

// Запрос на выполнение команды меню
// WPARAM		- LPBOOL - указатель на результат - TRUE - если команда выполнена
//            По-умолчанию - TRUE
// LPARAM		- указатель на экземпляр CTTMessageInfo 
//            m_Value.m_Command - идентификатор команды меню
// LRESULT	- не используется
UINT const CTTM_DOCONTEXTMENUCOMMAND = (0xC000 - 14);

// Сообщение о смене селектирования объекта
// WPARAM		- не используется ( =0 )
// LPARAM		- указатель на экземпляр структуры CTTMessageInfo
//						m_Value.m_bSetRemove - TRUE - установка, FALSE - снятие
// LRESULT	- не используется
UINT const CTTM_ITEMSELCHANGING      = (0xC000 - 15);

// Сообщение об уничтожении окна
// WPARAM		- 0
// LPARAM		- указатель на экземпляр CTTMessageInfo 
// LRESULT	- не используется
UINT const CTTM_WINDOWSHUTDOWN       = (0xC000 - 16);

// Сообщение о запросе формата вывода даты при редактировании
// WPARAM		- LPBOOL - указатель на результат - TRUE - если фомат дан
//            По-умолчанию - FALSE
// LPARAM		- указатель на экземпляр структуры CTTMessageInfo
//						m_Value.m_pDateFormat - формат вывода даты
UINT const CTTM_QUERYDATEFORMAT      = (0xC000 - 17);

// Сообщение о запросе двоичного представления даты при редактировании
// WPARAM		- LPBOOL - указатель на результат - TRUE - если дата дана
//            По-умолчанию - FALSE
// LPARAM		- указатель на экземпляр структуры CTTMessageInfo
//						m_Value.m_aDateValue[4] - значение даты - первые 4 слова SYSTEMTIME
UINT const CTTM_QUERYDATE						 = (0xC000 - 18);

// Сообщение о завершении редактирования текста узла типа "Date"
// WPARAM		- LPBOOL - указатель на результат - TRUE - если замена текста разрешена
//            По-умолчанию - TRUE
// LPARAM		- указатель на экземпляр CTTMessageInfo 
//						m_Value.m_aDateValue[4] - значение даты - первые 4 слова SYSTEMTIME
// LRESULT	- не используется
UINT const CTTM_ENDDATELABELEDIT		 = (0xC000 - 19);

// Сообщение о раскрытии узла
// WPARAM		- не используется ( =0 )
// LPARAM		- указатель на экземпляр структуры CTTMessageInfo
//						m_Value.m_bSetRemove - TRUE - расрытие, FALSE - закрытие
// LRESULT	- не используется
UINT const CTTM_ITEMEXPANDING        = (0xC000 - 20);

// Сообщение о смене первого видимого элемента
// WPARAM		- не используется ( =0 )
// LPARAM		- указатель на экземпляр структуры CTTMessageInfo
//						m_Value.m_bSetRemove - TRUE - установка, FALSE - снятие
// LRESULT	- не используется
UINT const CTTM_FIRSTVISIBLECHANGING  = (0xC000 - 21);

// Запрос на загрузку IP адреса
// WPARAM		- LPBOOL - указатель на результат - TRUE - если загрузка выполнена
//            По-умолчанию - FALSE
// LPARAM		- указатель на экземпляр CTTMessageInfo
// 		        m_Value.m_aIPValue[4] - значения полей IP адреса
// LRESULT	- не используется
UINT const CTTM_LOADIPADDRESS					= (0xC000 - 22);

// Сообщение о завершении редактирования текста узла типа "IP Address"
// WPARAM		- LPBOOL - указатель на результат - TRUE - если замена текста разрешена
//            По-умолчанию - TRUE
// LPARAM		- указатель на экземпляр CTTMessageInfo 
//						m_Value.m_aIPValue[4] - значения полей IP адреса
// LRESULT	- не используется
UINT const CTTM_ENDADDRESSLABELEDIT		= (0xC000 - 23);

// Сообщение о запросе формата вывода времени при редактировании
// WPARAM		- LPBOOL - указатель на результат - TRUE - если фомат дан
//            По-умолчанию - FALSE
// LPARAM		- указатель на экземпляр структуры CTTMessageInfo
//						m_Value.m_pTimeFormat - формат вывода даты
UINT const CTTM_QUERYTIMEFORMAT      = (0xC000 - 24);

// Сообщение о запросе двоичного представления времени при редактировании
// WPARAM		- LPBOOL - указатель на результат - TRUE - если дата дана
//            По-умолчанию - FALSE
// LPARAM		- указатель на экземпляр структуры CTTMessageInfo
//						m_Value.m_aTimeValue[4] - значение даты - вторые 4 слова SYSTEMTIME
UINT const CTTM_QUERYTIME						 = (0xC000 - 25);

// Сообщение о завершении редактирования текста узла типа "Time"
// WPARAM		- LPBOOL - указатель на результат - TRUE - если замена текста разрешена
//            По-умолчанию - TRUE
// LPARAM		- указатель на экземпляр CTTMessageInfo 
//						m_Value.m_aTimeValue[4] - значение даты - вторые 4 слова SYSTEMTIME
// LRESULT	- не используется
UINT const CTTM_ENDTIMELABELEDIT		 = (0xC000 - 26);

// Сообщение о запросе toolbar ID для меню
// WPARAM		- LPBOOL - указатель на результат - TRUE - если ID выдан
//            По-умолчанию - FALSE
// LPARAM		- указатель на экземпляр CTTMessageInfo 
//            m_Value.m_ToolBarStuff	- значение toolbar ID[0] and Handle[1]
// LRESULT	- не используется
UINT const CTTM_GETMENUTOOLBAR		   = (0xC000 - 27);

// Сообщение о получении/потере фокуса ввода
// WPARAM		- HWND приемника/терятеля фокуса
// LPARAM		- указатель на экземпляр структуры CTTMessageInfo
//						m_Value.m_bSetRemove - TRUE - установка, FALSE - снятие
// LRESULT	- не используется
UINT const CTTM_FOCUSCHANGING        = (0xC000 - 28);

// Сообщение о запросе формата вывода даты-времени при редактировании
// WPARAM		- LPBOOL - указатель на результат - TRUE - если фомат дан
//            По-умолчанию - FALSE
// LPARAM		- указатель на экземпляр структуры CTTMessageInfo
//						m_Value.m_pDateTimeFormat - формат вывода даты-времени
UINT const CTTM_QUERYDATETIMEFORMAT      = (0xC000 - 29);

// Сообщение о запросе двоичного представления даты-времени при редактировании
// WPARAM		- LPBOOL - указатель на результат - TRUE - если дата дана
//            По-умолчанию - FALSE
// LPARAM		- указатель на экземпляр структуры CTTMessageInfo
//						m_Value.m_aDateTimeValue[8] - значение даты-времени - SYSTEMTIME
UINT const CTTM_QUERYDATETIME						 = (0xC000 - 30);

// Сообщение о завершении редактирования текста узла типа "DateTime"
// WPARAM		- LPBOOL - указатель на результат - TRUE - если замена текста разрешена
//            По-умолчанию - TRUE
// LPARAM		- указатель на экземпляр CTTMessageInfo 
//						m_Value.m_aDateTimeValue[4] - значение даты-времени - SYSTEMTIME
// LRESULT	- не используется
UINT const CTTM_ENDDATETIMELABELEDIT		 = (0xC000 - 31);

// Запрос на загрузку PID
// WPARAM		- LPBOOL - указатель на результат - TRUE - если загрузка выполнена
//            По-умолчанию - FALSE
// LPARAM		- указатель на экземпляр CTTMessageInfo
// 		        m_Value.m_aPIDValue[4] - значения полей PID
// LRESULT	- не используется
UINT const CTTM_LOADPIDVALUE					= (0xC000 - 32);

// Сообщение о завершении редактирования текста узла типа "PID Value"
// WPARAM		- LPBOOL - указатель на результат - TRUE - если замена текста разрешена
//            По-умолчанию - TRUE
// LPARAM		- указатель на экземпляр CTTMessageInfo 
//						m_Value.m_aPIDValue[4] - значения полей PID
// LRESULT	- не используется
UINT const CTTM_ENDPIDLABELEDIT		= (0xC000 - 33);

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
struct NSTMessageInfo {
	int	m_nItemIndex;				// Индекс узла
	union {
		HMENU   m_hMenu;							// Handle ContextMenu
		DWORD   m_ToolBarStuff[2];	// Context-меню toolbar ID[0]/Handle[1]
		DWORD   m_iCommandId;				// Идентификатор команды Context-меню
		BOOL    m_bSetRemove;				// Флаг - "Установить/Снять"
	} m_Value;
};

// Запрос на заполнение поддерева 
// WPARAM		- LPBOOL - указатель на результат - TRUE - если заполнение выполнено
//            По-умолчанию - TRUE
// LPARAM		- указатель на экземпляр структуры NSTMessageInfo
// LRESULT	- не используется
UINT const NSTM_LOADSUBTREE      		= (0xC000 - 40);

// Запрос на обработку меню
// WPARAM		- LPBOOL - указатель на результат - TRUE - если вывод меню разрешен
//            По-умолчанию - TRUE
// LPARAM		- указатель на экземпляр структуры NSTMessageInfo
//						m_Value.m_hMenu - Handle меню (HMENU)
// LRESULT	- не используется
UINT const NSTM_PREPROCESSCONTEXTMENU = (0xC000 - 41);

// Запрос на выполнение команды меню
// WPARAM		- LPBOOL - указатель на результат - TRUE - если команда выполнена
//            По-умолчанию - TRUE
// LPARAM		- указатель на экземпляр структуры NSTMessageInfo
//						m_Value.m_iCommandId - идентификатор команды меню
// LRESULT	- не используется
UINT const NSTM_DOCONTEXTMENUCOMMAND = (0xC000 - 42);

// Сообщение о ручной установке Checked
// WPARAM		- LPBOOL - указатель на результат - FALSE - запретить автоматическое выполнение
//            По-умолчанию - TRUE
// LPARAM		- указатель на экземпляр структуры NSTMessageInfo
//						m_Value.m_bSetRemove - TRUE - установить, FALSE - снять
// LRESULT	- не используется
UINT const NSTM_MANUALLYSETREMOVECHECKED   = (0xC000 - 43);

// Сообщение об автоматической установке Checked
// WPARAM		- LPBOOL - указатель на результат - FALSE - запретить автоматическое выполнение
//            По-умолчанию - TRUE
// LPARAM		- указатель на экземпляр структуры NSTMessageInfo
//						m_Value.m_bSetRemove - TRUE - установить, FALSE - снять
// LRESULT	- не используется
UINT const NSTM_SETREMOVECHECKED		= (0xC000 - 44);

// Сообщение о ручной установке Strict
// WPARAM		- LPBOOL - указатель на результат - FALSE - запретить автоматическое выполнение
//            По-умолчанию - TRUE
// LPARAM		- указатель на экземпляр структуры NSTMessageInfo
//						m_Value.m_bSetRemove - TRUE - установить, FALSE - снять
// LRESULT	- не используется
UINT const NSTM_MANUALLYSETREMOVESTRICT = (0xC000 - 45);

// Сообщение о смене селектирования объекта
// WPARAM		- не используется ( =0 )
// LPARAM		- указатель на экземпляр структуры NSTMessageInfo
//						m_Value.m_bSetRemove - TRUE - установка, FALSE - снятие
// LRESULT	- не используется
UINT const NSTM_ITEMSELCHANGING      = (0xC000 - 46);

// Сообщение об уничтожении окна
// WPARAM		- 0
// LPARAM		- 0
// LRESULT	- не используется
UINT const NSTM_WINDOWSHUTDOWN       = (0xC000 - 47);

// Сообщение о получении/потере фокуса ввода
// WPARAM		- HWND приемника/терятеля фокуса
// LPARAM		- указатель на экземпляр структуры NSTMessageInfo
//						m_Value.m_bSetRemove - TRUE - установка, FALSE - снятие
// LRESULT	- не используется
UINT const NSTM_FOCUSCHANGING        = (0xC000 - 48);

// Сообщение об изменении состояния
// WPARAM		- не используется
// LPARAM		- указатель на экземпляр структуры NSTMessageInfo
// LRESULT	- не используется
UINT const NSTM_STATECHANGED					= (0xC000 - 49);

// Сообщение о запросе toolbar ID для меню
// WPARAM		- LPBOOL - указатель на результат - TRUE - если ID выдан
//            По-умолчанию - FALSE
// LPARAM		- указатель на экземпляр NSTMessageInfo 
//            m_Value.m_ToolBarStuff[2]	- значение toolbar ID[0] and Handle[1]
// LRESULT	- не используется
UINT const NSTM_GETMENUTOOLBAR		   = (0xC000 - 50);

// Сообщение о раскрытии узла
// WPARAM		- не используется ( =0 )
// LPARAM		- указатель на экземпляр структуры NSTMessageInfo
//						m_Value.m_bSetRemove - TRUE - расрытие, FALSE - закрытие
// LRESULT	- не используется
UINT const NSTM_ITEMEXPANDING        = (0xC000 - 51);

// Сообщение о начале/окончании процесса полного расрытия узла
// WPARAM		- не используется ( =0 )
// LPARAM		- указатель на экземпляр структуры NSTMessageInfo
//						m_Value.m_bSetRemove - TRUE - начало процесса, FALSE - окончание процеса
// LRESULT	- не используется
UINT const NSTM_FULLEXPANDING       = (0xC000 - 52);

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
struct NSLMessageInfo {
	int	m_nItemIndex;				// Индекс узла
	union {
		HMENU  m_hMenu;				    // Handle ContextMenu
		DWORD  m_ToolBarStuff[2];	// Context-меню toolbar ID[0]/Handle[1]
		DWORD  m_iCommandId;			// Идентификатор команды Context-меню
		BOOL   m_bSetRemove;      // Флаг - "Установить/Снять"
	} m_Value;
};

// Запрос на обработку меню
// WPARAM		- LPBOOL - указатель на результат - TRUE - если вывод меню разрешен
//            По-умолчанию - TRUE
// LPARAM		- указатель на экземпляр структуры NSLMessageInfo
//						m_Value.m_nItemIndex	- может быть -1, если нет селектированного элемента
//						m_Value.m_hMenu - Handle меню (HMENU)
// LRESULT	- не используется
UINT const NSLM_PREPROCESSCONTEXTMENU = (0xC000 - 60);

// Запрос на выполнение команды меню
// WPARAM		- LPBOOL - указатель на результат - TRUE - если команда выполнена
//            По-умолчанию - TRUE
// LPARAM		- указатель на экземпляр структуры NSLMessageInfo
//						m_Value.m_iCommandId - идентификатор команды меню
// LRESULT	- не используется
UINT const NSLM_DOCONTEXTMENUCOMMAND  = (0xC000 - 61);

// Запрос на установку Checked
// WPARAM		- LPBOOL - указатель на результат - FALSE - запретить автоматическое выполнение
//            По-умолчанию - TRUE
// LPARAM		- указатель на экземпляр структуры NSLMessageInfo
//						m_Value.m_bSetRemove - TRUE - установить, FALSE - снять
// LRESULT	- не используется
UINT const NSLM_MANUALLYSETREMOVECHECKED = (0xC000 - 62);

// Запрос на установку Strict
// WPARAM		- LPBOOL - указатель на результат - FALSE - запретить автоматическое выполнение
//            По-умолчанию - TRUE
// LPARAM		- указатель на экземпляр структуры NSLMessageInfo
//						m_Value.m_bSetRemove - TRUE - установить, FALSE - снять
// LRESULT	- не используется
UINT const NSLM_MANUALLYSETREMOVESTRICT  = (0xC000 - 63);

// Сообщение о смене селектирования объекта
// WPARAM		- не используется ( =0 )
// LPARAM		- указатель на экземпляр структуры NSLMessageInfo
//						m_Value.m_bSetRemove - TRUE - установка, FALSE - снятие
// LRESULT	- не используется
UINT const NSLM_ITEMSELCHANGING				= (0xC000 - 64);

// Сообщение об уничтожении окна
// WPARAM		- 0
// LPARAM		- 0
// LRESULT	- не используется
UINT const NSLM_WINDOWSHUTDOWN				= (0xC000 - 65);

// Сообщение о получении/потере фокуса ввода
// WPARAM		- HWND приемника/терятеля фокуса
// LPARAM		- указатель на экземпляр структуры NSLMessageInfo
//						m_Value.m_bSetRemove - TRUE - установка, FALSE - снятие
// LRESULT	- не используется
UINT const NSLM_FOCUSCHANGING         = (0xC000 - 66);

// Сообщение об изменении состояния
// WPARAM		- не используется
// LPARAM		- указатель на экземпляр структуры NSLMessageInfo
// LRESULT	- не используется
UINT const NSLM_STATECHANGED					= (0xC000 - 67);

// Сообщение о запросе toolbar ID для меню
// WPARAM		- LPBOOL - указатель на результат - TRUE - если ID выдан
//            По-умолчанию - FALSE
// LPARAM		- указатель на экземпляр NSLMessageInfo 
//            m_Value.m_ToolBarStuff[2]	- значение toolbar ID[0] and Handle[1]
// LRESULT	- не используется
UINT const NSLM_GETMENUTOOLBAR		   = (0xC000 - 68);
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
struct RPVMessageInfo {
	int	m_nItemIndex;				// Индекс item'а
	int	m_nSubItemIndex;		// Индекс subitem'а
	union {
		HMENU    m_hMenu;					 // Handle ContextMenu
		DWORD    m_ToolBarStuff[2];// Context-меню toolbar ID[0]/Handle[1]
		DWORD    m_iCommandId;		 // Идентификатор команды Context-меню
		BOOL     m_bSetRemove;     // Флаг - "Установить/Снять"
		BOOL     m_bSortAscending; // Флаг - "Сортировка по возрастанию/убыванию"
		TCHAR   *m_pContext;			 // Контекст поиска
		DWORD    m_MenuItemStuff[2];// Context-меню item ID[0]/BitmapHandle[1]
	} m_Value;
};

// Запрос на заполнение элемента
// WPARAM		- LPBOOL - указатель на результат - TRUE - если элемент заполнен
//            По-умолчанию - FALSE
// LPARAM		- указатель на экземпляр структуры RPVMessageInfo
// LRESULT	- не используется
UINT const RPVM_DOFILLITEM						= (0xC000 - 70);

// Сообщение об уничтожении окна
// WPARAM		- 0
// LPARAM		- 0
// LRESULT	- не используется
UINT const RPVM_WINDOWSHUTDOWN				= (0xC000 - 71);

// Запрос на сортировку по заданному полю
// WPARAM		- LPBOOL - указатель на результат - TRUE - если сортировка выполнена
//            По-умолчанию - FALSE
// LPARAM		- указатель на экземпляр структуры RPVMessageInfo
//            m_nItemIndex - индекс элемента заголовка
// LRESULT	- не используется
UINT const RPVM_DOSORTITEMS					  = (0xC000 - 72);

// Запрос на установку Checked
// WPARAM		- LPBOOL - указатель на результат - FALSE - запретить автоматическое выполнение
//            По-умолчанию - TRUE
// LPARAM		- указатель на экземпляр структуры RPVMessageInfo
//						m_Value.m_bSetRemove - TRUE - установить, FALSE - снять
// LRESULT	- не используется
UINT const RPVM_MANUALLYSETREMOVECHECKED = (0xC000 - 73);

// Запрос на обработку меню
// WPARAM		- LPBOOL - указатель на результат - TRUE - если вывод меню разрешен
//            По-умолчанию - TRUE
// LPARAM		- указатель на экземпляр структуры RPVMessageInfo
//						m_Value.m_nItemIndex	- может быть -1, если нет селектированного элемента
//						m_Value.m_hMenu - Handle меню (HMENU)
// LRESULT	- не используется
UINT const RPVM_PREPROCESSCONTEXTMENU = (0xC000 - 74);

// Запрос на выполнение команды меню
// WPARAM		- LPBOOL - указатель на результат - TRUE - если команда выполнена
//            По-умолчанию - TRUE
// LPARAM		- указатель на экземпляр структуры RPVMessageInfo
//						m_Value.m_iCommandId - идентификатор команды меню
// LRESULT	- не используется
UINT const RPVM_DOCONTEXTMENUCOMMAND  = (0xC000 - 75);

// Запрос на контекст поиска
// WPARAM		- LPBOOL - указатель на результат - TRUE - если контекст выдан
//            По-умолчанию - FALSE
// LPARAM		- указатель на экземпляр структуры RPVMessageInfo
//						m_Value.m_pContext - указатель на контекст поиска
// LRESULT	- не используется
UINT const RPVM_GETFINDITEMCONTEXT    = (0xC000 - 76);

// Запрос на проверку элемента при поиске
// WPARAM		- LPBOOL - указатель на результат - TRUE - если элемент удовлетворяет условию поиска
//            По-умолчанию - FALSE
// LPARAM		- указатель на экземпляр структуры RPVMessageInfo
//						m_Value.m_pContext - указатель на контекст поиска
// LRESULT	- не используется
UINT const RPVM_CHECKITEMFORFIND      = (0xC000 - 77);

// Запрос на создание меню
// WPARAM		- LPBOOL - указатель на результат - TRUE - если если меню создано
//            По-умолчанию - FALSE
// LPARAM		- указатель на экземпляр структуры RPVMessageInfo
//						m_Value.m_nItemIndex	- может быть -1, если нет селектированного элемента
//						m_Value.m_hMenu - Handle меню (HMENU)
// LRESULT	- не используется
UINT const RPVM_CREATECONTEXTMENU			= (0xC000 - 78);

// Сообщение о запросе toolbar ID для меню
// WPARAM		- LPBOOL - указатель на результат - TRUE - если ID выдан
//            По-умолчанию - FALSE
// LPARAM		- указатель на экземпляр RPVMessageInfo 
//            m_Value.m_ToolBarStuff[2]	- значение toolbar ID[0] and Handle[1]
// LRESULT	- не используется
UINT const RPVM_GETMENUTOOLBAR		   = (0xC000 - 79);

// Сообщение о получении/потере фокуса ввода
// WPARAM		- HWND приемника/терятеля фокуса
// LPARAM		- указатель на экземпляр структуры RPVMessageInfo
//						m_Value.m_bSetRemove - TRUE - установка, FALSE - снятие
// LRESULT	- не используется
UINT const RPVM_FOCUSCHANGING		   = (0xC000 - 80);

// Сообщение о запросе BITMAP для элемента меню
// WPARAM		- LPBOOL - указатель на результат - TRUE - если BITMAP выдан
//            По-умолчанию - FALSE
// LPARAM		- указатель на экземпляр RPVMessageInfo 
//            m_Value.m_hBitmap	- указатель на HBITMAP
// LRESULT	- не используется
UINT const RPVM_GETMENUITEMBITMAP   = (0xC000 - 81);

// Сообщение о смене селектирования объекта
// WPARAM		- не используется ( =0 )
// LPARAM		- указатель на экземпляр структуры RPVMessageInfo
//						m_Value.m_bSetRemove - TRUE - установка, FALSE - снятие
// LRESULT	- не используется
UINT const RPVM_ITEMSELCHANGING			= (0xC000 - 82);
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
struct STVMessageInfo {
	int	m_nItemIndex;				// Индекс узла
	union {
		HMENU   m_hMenu;					// Handle ContextMenu
		DWORD   m_ToolBarStuff[2];// Context-меню toolbar ID[0]/Handle[1]
		DWORD   m_iCommandId;			// Идентификатор команды Context-меню
		TCHAR  *m_pContext;			  // Контекст поиска
		BOOL    m_bSetRemove;     // Флаг - "Установить/Снять"
	} m_Value;
};

// Сообщение об уничтожении окна
// WPARAM		- 0
// LPARAM		- 0
// LRESULT	- не используется
UINT const STVM_WINDOWSHUTDOWN				= (0xC000 - 90);

// Запрос на обработку меню
// WPARAM		- LPBOOL - указатель на результат - TRUE - если вывод меню разрешен
//            По-умолчанию - TRUE
// LPARAM		- указатель на экземпляр структуры STVMessageInfo
//						m_Value.m_nItemIndex	- может быть -1, если нет селектированного элемента
//						m_Value.m_hMenu - Handle меню (HMENU)
// LRESULT	- не используется
UINT const STVM_PREPROCESSCONTEXTMENU = (0xC000 - 91);

// Запрос на выполнение команды меню
// WPARAM		- LPBOOL - указатель на результат - TRUE - если команда выполнена
//            По-умолчанию - TRUE
// LPARAM		- указатель на экземпляр структуры STVMessageInfo
//						m_Value.m_iCommandId - идентификатор команды меню
// LRESULT	- не используется
UINT const STVM_DOCONTEXTMENUCOMMAND  = (0xC000 - 92);

// Запрос на контекст поиска
// WPARAM		- LPBOOL - указатель на результат - TRUE - если контекст выдан
//            По-умолчанию - FALSE
// LPARAM		- указатель на экземпляр структуры STVMessageInfo
//						m_Value.m_pContext - указатель на контекст поиска
// LRESULT	- не используется
UINT const STVM_GETFINDITEMCONTEXT    = (0xC000 - 93);

// Запрос на проверку элемента при поиске
// WPARAM		- LPBOOL - указатель на результат - TRUE - если элемент удовлетворяет условию поиска
//            По-умолчанию - FALSE
// LPARAM		- указатель на экземпляр структуры STVMessageInfo
//						m_Value.m_pContext - указатель на контекст поиска
// LRESULT	- не используется
UINT const STVM_CHECKITEMFORFIND      = (0xC000 - 94);

// Запрос на создание меню
// WPARAM		- LPBOOL - указатель на результат - TRUE - если если меню создано
//            По-умолчанию - FALSE
// LPARAM		- указатель на экземпляр структуры STVMessageInfo
//						m_Value.m_nItemIndex	- может быть -1, если нет селектированного элемента
//						m_Value.m_hMenu - Handle меню (HMENU)
// LRESULT	- не используется
UINT const STVM_CREATECONTEXTMENU			= (0xC000 - 95);

// Сообщение о запросе toolbar ID для меню
// WPARAM		- LPBOOL - указатель на результат - TRUE - если ID выдан
//            По-умолчанию - FALSE
// LPARAM		- указатель на экземпляр STVMessageInfo 
//            m_Value.m_ToolBarStuff[2]	- значение toolbar ID[0] and Handle[1]
// LRESULT	- не используется
UINT const STVM_GETMENUTOOLBAR		   = (0xC000 - 96);

// Сообщение о получении/потере фокуса ввода
// WPARAM		- HWND приемника/терятеля фокуса
// LPARAM		- указатель на экземпляр структуры STVMessageInfo
//						m_Value.m_bSetRemove - TRUE - установка, FALSE - снятие
// LRESULT	- не используется
UINT const STVM_FOCUSCHANGING		   = (0xC000 - 97);

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
struct ABTMessageInfo {
	int	m_nItemIndex;				// Индекс узла
	union {
		HMENU   m_hMenu;					// Handle ContextMenu
		DWORD   m_ToolBarStuff[2];// Context-меню toolbar ID[0]/Handle[1]
		DWORD   m_iCommandId;			// Идентификатор команды Context-меню
		TCHAR  *m_pContext;			  // Контекст поиска
		BOOL   m_bSetRemove;      // Флаг - "Установить/Снять"
	} m_Value;
};

// Сообщение об уничтожении окна
// WPARAM		- 0
// LPARAM		- 0
// LRESULT	- не используется
UINT const ABTM_WINDOWSHUTDOWN				= (0xC000 - 100);

// Запрос на обработку меню
// WPARAM		- LPBOOL - указатель на результат - TRUE - если вывод меню разрешен
//            По-умолчанию - TRUE
// LPARAM		- указатель на экземпляр структуры ABTMessageInfo
//						m_Value.m_nItemIndex	- может быть -1, если нет селектированного элемента
//						m_Value.m_hMenu - Handle меню (HMENU)
// LRESULT	- не используется
UINT const ABTM_PREPROCESSCONTEXTMENU = (0xC000 - 101);

// Запрос на выполнение команды меню
// WPARAM		- LPBOOL - указатель на результат - TRUE - если команда выполнена
//            По-умолчанию - TRUE
// LPARAM		- указатель на экземпляр структуры ABTMessageInfo
//						m_Value.m_iCommandId - идентификатор команды меню
// LRESULT	- не используется
UINT const ABTM_DOCONTEXTMENUCOMMAND  = (0xC000 - 102);

// Запрос на контекст поиска
// WPARAM		- LPBOOL - указатель на результат - TRUE - если контекст выдан
//            По-умолчанию - FALSE
// LPARAM		- указатель на экземпляр структуры ABTMessageInfo
//						m_Value.m_pContext - указатель на контекст поиска
// LRESULT	- не используется
UINT const ABTM_GETFINDITEMCONTEXT    = (0xC000 - 103);

// Запрос на проверку элемента при поиске
// WPARAM		- LPBOOL - указатель на результат - TRUE - если элемент удовлетворяет условию поиска
//            По-умолчанию - FALSE
// LPARAM		- указатель на экземпляр структуры ABTMessageInfo
//						m_Value.m_pContext - указатель на контекст поиска
// LRESULT	- не используется
UINT const ABTM_CHECKITEMFORFIND      = (0xC000 - 104);

// Запрос на создание меню
// WPARAM		- LPBOOL - указатель на результат - TRUE - если если меню создано
//            По-умолчанию - FALSE
// LPARAM		- указатель на экземпляр структуры ABTMessageInfo
//						m_Value.m_nItemIndex	- может быть -1, если нет селектированного элемента
//						m_Value.m_hMenu - Handle меню (HMENU)
// LRESULT	- не используется
UINT const ABTM_CREATECONTEXTMENU			= (0xC000 - 105);

// Сообщение о запросе toolbar ID для меню
// WPARAM		- LPBOOL - указатель на результат - TRUE - если ID выдан
//            По-умолчанию - FALSE
// LPARAM		- указатель на экземпляр ABTMessageInfo 
//            m_Value.m_ToolBarStuff[2]	- значение toolbar ID[0] and Handle[1]
// LRESULT	- не используется
UINT const ABTM_GETMENUTOOLBAR		   = (0xC000 - 106);

// Сообщение о смене селектирования объекта
// WPARAM		- не используется ( =0 )
// LPARAM		- указатель на экземпляр структуры ABTMessageInfo
//						m_Value.m_bSetRemove - TRUE - установка, FALSE - снятие
// LRESULT	- не используется
UINT const ABTM_ITEMSELCHANGING			 = (0xC000 - 107);

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

// Запрос на выполнение команды меню или акселератора от CCheckList
// WPARAM		- идентификатор команды
// LPARAM		- не используется
// LRESULT	- не используется
UINT const CLWM_DOCOMMAND							= (0xC000 - 200);

// Запрос на выполнение команды меню или акселератора от CCheckTree
// WPARAM		- идентификатор команды
// LPARAM		- не используется
// LRESULT	- не используется
UINT const CTWM_DOCOMMAND							= (0xC000 - 201);

#endif