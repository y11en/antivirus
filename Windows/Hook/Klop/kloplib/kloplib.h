#ifndef __KLOPLIB_H__
#define __KLOPLIB_H__

#include "winioctl.h"
#include "..\..\hook\klop_api.h"

#pragma warning(disable : 4200) // non-standard MS extension

#define KLOP_DRV_NAME	"Klop"

#if defined (KLOPUM_EXPORTS)
	#define KLOPUMEXPORT __declspec (dllexport)
#else
#if defined (KLOPUM_INPLACE)
	#define KLOPUMEXPORT 
#else 
	#define KLOPUMEXPORT __declspec (dllimport)
#endif
#endif

#define KLOPUMMETHOD_ KLOPUMEXPORT
#define KLOPUMMETHOD KLOPUMMETHOD_ KLOPUM::OpResult 

namespace KLOPUM
{
	enum OpResult 
	{
		srOK,
		srNotOk,
		srDriverNotLoaded,
		srInitializedAlready
	};

	// инициализация модуля лицензирования.
	// pKey		  - буфер, содержащий лицензионный ключ
	// KeySize	  - размер буфера	
	KLOPUMMETHOD	KlopInit( const char* pKey, unsigned long KeySize);

	// Активизация ключа.
	// TimeToWait - кол-во миллисекунд, в течение которых драйвер ждет ответа из сети,
	//				после чего заканчивает прием пакетов из сети.
	KLOPUMMETHOD	KlopActivate(unsigned long TimeToWait);


	// получает количество таких же лицензий, полученных к данному моменту
	KLOPUMMETHOD	KlopGetCount( ULONG* pKeyCount );

	// полная деинициализация модуля лицензирования. после вызова функции пакеты перестают отправляться и приниматься.
	KLOPUMMETHOD	KlopDone();
}

extern HANDLE	g_DrvHandle;

#endif