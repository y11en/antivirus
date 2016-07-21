#ifndef __CKAHUM_H__
#define __CKAHUM_H__

#include "ckahdefs.h"
#include "ckahumdefs.h"

#pragma warning(disable : 4200) // non-standard MS extension

#define CKAHUMMETHOD_ CKAHUMEXPORT
#define CKAHUMMETHOD CKAHUMMETHOD_ CKAHUM::OpResult 

// общие функции работы с CKAH
namespace CKAHUM
{
	// прототип функции-трассировщика
	typedef void (CALLBACK *EXTERNALLOGGERPROC) (LogLevel Level, LPCSTR szString);

	// устанавливает persistent хранилище в CCKAHRegStorageImpl по defalt пути,
	// а также внешнюю функцию-трассировщик
	CKAHUMMETHOD Initialize (EXTERNALLOGGERPROC logger = NULL);

	// устанавливает persistent хранилище в CCKAHRegStorageImpl с указанным путем в реестре),
	// а также внешнюю функцию-трассировщик
	CKAHUMMETHOD Initialize (HKEY hKey, LPCWSTR szSubKey, EXTERNALLOGGERPROC logger = NULL);

    // разрегистрирует внешнюю функцию-трассировщик
	CKAHUMMETHOD Deinitialize (EXTERNALLOGGERPROC logger = NULL);

	// обновляет persistent хранилище и перезапускает запущенные плагины
	CKAHUMMETHOD Reload (LPCWSTR szManifestFilePath);

    // разрегистрирует и удаляет проинсталлированные драйвера (для вызова из деинсталлятора)
    CKAHUMMETHOD Uninstall ();
	
	// включает фильтрацию базовыми драйверами (при старте фильтрация включена)
	// можно вызывать и до старта плагинов
	CKAHUMMETHOD Pause ();

	// выключает фильтрацию базовыми драйверами (при старте фильтрация включена)
	// можно вызывать и до старта плагинов
	CKAHUMMETHOD Resume ();
	
	// разрегистрирует базовые драйверы
	CKAHUMMETHOD UnregisterBaseDrivers ();

	// запрещает / разрешает базовые драйверы
	CKAHUMMETHOD EnableBaseDrivers (bool bEnable);
	
	// определяет, разрешены ли бызовые драйверы
	CKAHUMMETHOD IsBaseDriversEnabled (bool *pbEnabled);

	// определяет, работают ли базовые драйверы
	CKAHUMMETHOD IsBaseDriversRunning (bool *pbRunning);

	// останавливает все запущенные плагины
	CKAHUMMETHOD StopAll ();

}; //namespace CKAHUM

// C stubs
extern "C" CKAHUMMETHOD CKAHUM_Reload (LPCWSTR szManifestFilePath);
extern "C" CKAHUMMETHOD CKAHUM_Uninstall ();
extern "C" CKAHUMMETHOD CKAHUM_Initialize (HKEY hKey, LPCWSTR szSubKey, CKAHUM::EXTERNALLOGGERPROC logger = NULL);
extern "C" CKAHUMMETHOD CKAHUM_Deinitialize (CKAHUM::EXTERNALLOGGERPROC logger = NULL);

#include "ckahids.h"
#include "ckahstm.h"
#include "ckahfw.h"
#include "ckahcr.h"

#pragma warning(default : 4200)

#include "ckahrules.h"

#endif