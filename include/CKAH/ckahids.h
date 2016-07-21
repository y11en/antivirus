#ifndef __CKAHIDS_H__
#define __CKAHIDS_H__

#include "ckahip.h"
#include "../../windows/Hook/Plugins/Include/attacknotify_api.h"

// IDS
namespace CKAHIDS
{
	// параметр callback-функции уведомления об атаке
	struct AttackNotify
	{
		ULONG	    AttackID;			// идентификатор атаки
		LPCWSTR	    AttackDescription;	// название атаки (нуль-терминированная юникод-строка)
		UCHAR	    Proto;				// протокол
		CKAHUM::IP	AttackerIP;			// IP-адрес атакующего
		USHORT	    LocalPort;			// порт
		ULONG	    ResponseFlags;		// флаги ответа (см. $/windows/Hook/Plugins/Include/attacknotify_api.h)
		__int64     Time;				// время атаки (UTC FILETIME)
	};

	// callback-функция уведомления об атаке
	typedef void (CALLBACK *ATTACKNOTIFYPROC) (LPVOID lpContext, const AttackNotify *pAttackNotify);

	// запускает IDS
	CKAHUMMETHOD Start (ATTACKNOTIFYPROC AttackNotifyCallback,	// callback-функция уведомления об атаке
										LPVOID lpCallbackContext);			// параметр lpContext callback-функции

	// останавливает IDS
	CKAHUMMETHOD Stop ();

	// приостанавливает IDS (при запуске плагин приостановлен)
	CKAHUMMETHOD Pause ();

	// возобновляет работу IDS (при запуске плагин приостановлен)
	CKAHUMMETHOD Resume ();

	// управление параметрами IDS

	// устанавливает для атаки AttackID параметр ParmNum в значение ParmValue
	CKAHUMMETHOD SetIDSParam (ULONG AttackID, ULONG ParmNum, ULONG ParmValue);
	// получает для атаки AttackID параметр ParmNum в pParmValue
	CKAHUMMETHOD GetIDSParam (ULONG AttackID, ULONG ParmNum, PULONG pParmValue);
	// получает для атаки AttackID количество параметров
	CKAHUMMETHOD GetIDSParamCount (ULONG AttackID, PULONG pParamCount);

	// устанавливает параметры уведомления об атаках
	// AttackID - идентификатор атаки, 0 - для всех по умолчанию
	// ResponseFlags - флаги из $/windows/Hook/Plugins/Include/attacknotify_api.h
	// BanTime - время блокирования атакующего (в секундах)
	CKAHUMMETHOD SetIDSAttackNotifyParam (ULONG AttackID, ULONG ResponseFlags, ULONG BanTime);

    // устанавливает период нотификации для продолжающейся атаки (нотификация об одной
    // атаке не будет выдаваться чаще, чем указанный период)
    CKAHUMMETHOD SetIDSAttackNotifyPeriod (ULONG NotifyPeriod);

	// Banomet

    struct BannedHost
    {
        CKAHUM::IP  IP;                 // IP-адрес
        __int64     BanTime;            // Время, когда был забанен (UTC FILETIME)
    };

    typedef LPVOID HBANNEDLIST;

    // получить список забаненных хостов
    CKAHUMMETHOD GetBannedList ( OUT HBANNEDLIST * phList );

    // получить размер списка забаненных хостов
    CKAHUMMETHOD BannedList_GetSize ( IN HBANNEDLIST hList, OUT int * psize );

    // получить элемент списка забаненных хостов 
    CKAHUMMETHOD BannedList_GetItem ( IN HBANNEDLIST hList, IN int n, OUT BannedHost * pBannedHost );

    // удалить список забаненных хостов
    CKAHUMMETHOD BannedList_Delete ( IN HBANNEDLIST hList );

	// разблокирует хосты с ip-адресом ip по маске mask
	CKAHUMMETHOD UnbanHost (IN CKAHUM::IP *pIp, IN CKAHUM::IP *pMask);

};

#endif