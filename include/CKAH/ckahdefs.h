#ifndef __CKAHDEFS_H__
#define __CKAHDEFS_H__

namespace CKAHUM
{
	// коды возврата функций 
	enum OpResult
	{
		srOK,								// все хорошо
		srNeedReload,						// необходимо вызвать Reload для обновления (возможно первый старт)
		srInvalidArg,						// неверный аргумент
		srManifestNotFound,					// файл манифеста по указанному пути не найден
		srWrongManifestFormat,				// произошла ошибка при разборе манифеста из-за неправильного формата
		srErrorReadingPersistentManifest,	// произошла ошибка при чтении манифеста из внешнего хранилища
		srNeedReboot,						// требуется перезагрузка
		srNotStarted,						// компонент не запущен
		srAlreadyStarted,					// компонент уже запущен
		srNoPersistentManifest,				// отсутствует persistent хранилище (функция Initialize не была вызвана)
        srInsufficientBuffer,               // 
		
		srOpFailed,							// операция прошла неуспешно
	};

	// уровень трассировки
	enum LogLevel
	{
		lError = 1,
		lWarning = 2,
		lInfo = 3
	};

}; //namespace CKAHUM


#endif