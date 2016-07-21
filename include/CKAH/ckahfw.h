#ifndef __CKAHFW_H__
#define __CKAHFW_H__

#include "ckahrules.h"

// Firewall
namespace CKAHFW
{
	//	Firewall rules
	
	CKAHUMMETHOD GetPacketRules (OUT HPACKETRULES *phRules);
	CKAHUMMETHOD SetPacketRules (IN HPACKETRULES hRules);

	CKAHUMMETHOD GetApplicationRules (OUT HAPPRULES *phRules);
	CKAHUMMETHOD SetApplicationRules (IN HAPPRULES hRules);

	CKAHUMMETHOD GetApplicationChecksum (IN LPCWSTR szwApplicationPath, OUT UCHAR Checksum[FW_HASH_SIZE]);
    CKAHUMMETHOD GetApplicationChecksumVersion (OUT PULONG pVersion);

	// параметр callback-функции уведомления о срабатывании пакетного правила
	struct PacketRuleNotify
	{
		LPCWSTR     RuleName;			// имя правила
		bool	    IsBlocking;			// пакет заблокирован / пропущен
		bool	    IsIncoming;			// входящий / исходящий пакет
        bool        IsStream;           // 
        bool        IsStreamIncoming;   // входящий / исходящий поток
		UCHAR	    Proto;				// протокол
		CKAHUM::IP	RemoteAddress;		// удаленный IP-адрес
		CKAHUM::IP	LocalAddress;		// локальный IP-адрес
		USHORT	    TCPUDPRemotePort;	// удаленный порт (только для протоколов TCP и UDP)
		USHORT	    TCPUDPLocalPort;	// локальный порт (только для протоколов TCP и UDP)
		UCHAR	    ICMPType;			// ICMP тип (только для протокола ICMP)
		UCHAR	    ICMPCode;			// ICMP код (только для протокола ICMP)
		bool	    Log;				// занести в журнал
		bool	    Notify;				// уведомить пользователя (хотя бы один из Log, Notify == true)
		__int64     Time;				// время события (UTC FILETIME)
		UCHAR*      pUserData;			// пользовательские данные правила (NULL если данных нет)
		ULONG	    UserDataSize;		// размер пользовательских данных в байтах
	};

	// параметр callback-функции уведомления о срабатывании правила для приложений (Popup == false)
	// или обнаружении сетевой активности приложения при отсутствии правила (Popup == true)
	// (*) - не используется при обнаружении сетевой активности приложения при отсутствии правила 
	struct ApplicationRuleNotify
	{
		LPCWSTR     RuleName;			// имя правила (*)
		LPCWSTR     ApplicationName;	// имя приложения
        LPCWSTR     CommandLine;        // командная строка приложения
		ULONG	    PID;				// PID приложения
		bool	    IsBlocking;			// заблокировано / пропущено (*)
		bool	    IsIncoming;			// направление
        bool        IsStream;           // 
        bool        IsStreamIncoming;   // входящий / исходящий поток
		UCHAR	    Proto;				// протокол (TCP или UDP)
		CKAHUM::IP	RemoteAddress;		// удаленный IP-адрес
		CKAHUM::IP	LocalAddress;		// локальный IP-адрес
		USHORT	    RemotePort;			// удаленный порт
		USHORT	    LocalPort;			// локальный порт
		bool	    Popup;				// запрос пользователя (правила нет) (если Popup == true, то Log и Notify == false)
		bool	    Log;				// занести в журнал
		bool	    Notify;				// уведомить пользователя (хотя бы один из Log, Notify == true, если Popup == false)
		__int64     Time;				// время события (UTC FILETIME)
		UCHAR*      pUserData;			// пользовательские данные правила (NULL если данных нет) (*)
		ULONG	    UserDataSize;		// размер пользовательских данных в байтах (*)
	};

	// параметр callback-функции уведомления о несоответствии контрольной суммы файла
	struct ChecksumChangedNotify
	{
		LPCWSTR ApplicationName;	// имя приложения
		ULONG	PID;				// PID приложения
		__int64 Time;				// время события (UTC FILETIME)
		UCHAR	ApplicationOldChecksum[FW_HASH_SIZE]; // сохраненная контрольная сумма приложения
		UCHAR	ApplicationNewChecksum[FW_HASH_SIZE]; // новая контрольная сумма приложения
	};

	enum RuleCallbackResult
	{
		crBlock,
		crPass,
		crPassModifyChecksum // специальный код возврата для CHECKSUMNOTIFYCALLBACK
	};		

	// callback-функция уведомления о срабатывании пакетного правила
	typedef void (CALLBACK *PACKETRULENOTIFYCALLBACK) (LPVOID lpContext, const PacketRuleNotify *pNotify);

	// callback-функция уведомления о срабатывании правила для приложений
	typedef RuleCallbackResult (CALLBACK *APPLICATIONRULENOTIFYCALLBACK) (LPVOID lpContext, const ApplicationRuleNotify *pNotify);

	// callback-функция уведомления о несоответствии контрольной суммы файла
	typedef RuleCallbackResult (CALLBACK *CHECKSUMNOTIFYCALLBACK) (LPVOID lpContext, const ChecksumChangedNotify *pNotify);

	// запускает Firewall
	CKAHUMMETHOD Start (PACKETRULENOTIFYCALLBACK PacketRuleNotifyCallback, LPVOID lpPacketCallbackContext,
						APPLICATIONRULENOTIFYCALLBACK ApplicationRuleNotifyCallback, LPVOID lpApplicationCallbackContext,
						CHECKSUMNOTIFYCALLBACK ChecksumCallback, LPVOID lpChecksumCallbackContext);

	// останавливает Firewall
	CKAHUMMETHOD Stop ();

	// приостанавливает Firewall (при запуске плагин приостановлен)
	CKAHUMMETHOD Pause ();

	// возобновляет работу Firewall (при запуске плагин приостановлен)
	CKAHUMMETHOD Resume ();

	CKAHUMMETHOD FixPid ();

	
	enum WorkingMode
	{
		wmBlockAll,
		wmRejectNotFiltered,
		wmAskUser,
		wmAllowNotFiltered,
		wmAllowAll
	};

	// устанавливает режим работы
	CKAHUMMETHOD SetWorkingMode (WorkingMode mode);

	// получает режим работы
	CKAHUMMETHOD GetWorkingMode (WorkingMode *mode);

    enum FilteringMode
    {
        fmMaxCompatibility,
        fmMaxSpeed
    };
    
    CKAHUMMETHOD SetFilteringMode( FilteringMode mode );

    CKAHUMMETHOD GetFilteringMode( FilteringMode mode );
};

#include "ckahports.h"
#include "ckahnetwork.h"

#pragma warning(default : 4200)

#endif