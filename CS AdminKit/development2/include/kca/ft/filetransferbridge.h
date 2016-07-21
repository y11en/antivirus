/*!
* (C) 2005 "Kaspersky Lab"
*
* \file filetransferbridge.h
* \author Шияфетдинов Дамир
* \date 2005
* \brief Интерфейс класса для взаимодействия процессов с FileTransfer'ом сетевого агента
		Данный интерфейс позволяет закачывать файлы обновлений с сервера администрирования.

		Схема работы: FileTransferBridge соединяется с сетевым агентом, который пересылает все 
		запросы к серверу администирования
*/

#ifndef KLFT_FILETRANSFERBRIDGE_H
#define KLFT_FILETRANSFERBRIDGE_H

#include <wchar.h>

namespace KLFT {

	/*!
		\brief Результат работы функции Connect
	*/
	enum ConnectResult
	{
		CR_Ok,						//!< Соединение установелено успешно
		CR_WrongReceiverId,			//!< Неверный идентификатор ( либо удален, либо для данного receiver'а уже был вызван connect )
		CR_ServerBusy,				//!< Сервер на данный момент занят и не может обрабатывать запросы от клиента
		CR_ConnectionError,			//!< Ошибка физического подключений к серверу
		CR_ConnectionNagentError,	//!< Ошибка физического подключений к сетевому агенту
		CR_ConnectionServerError	//!< Ошибка физического подключений к главному серверу
	};
	
	/*!
		\brief Результат функций работы с файлами
	*/
	enum FileOpeartionResult
	{
		FR_Ok,					//!< Порция файла успешно закачена
		FR_WrongReceiverId,		//!< Неверный идентификатор ( либо удален, либо для данного receiver'а уже был вызван connect )
		FR_OkDownloaded,		//!< Порция файла успешно закачена и это была последняя порция данного файла
		FR_WrongArguments,		//!< Неверные парамеры функции
		FR_FileNotFound,		//!< Файл с данным именем не найден
		FR_Timeout,				//!< timeout
		FR_ErrorInOperation		//!< Ошибка при закачки очередной порции файла			
	};
	
	class FileTransferBridge
	{
	public:

		/*!
			\brief Структура описания файла
		*/
		struct FileInfo
		{			
			bool			m_isDirectory;
			bool			m_readPermissionOk;
			time_t			m_createTime;
			long			m_fileSize;
		};

		/*!
			\brief Функция производит подключение к серверу администрирования
		*/
		virtual ConnectResult Connect() = 0;

		/*!
			\brief Функция производит подключение от подчиненного сервера к главному
				серверу администрирования
		*/
		virtual ConnectResult ConnectToMasterServer() = 0;

		/*!
			\brief Возвращает информацию о файле

			\return false - файл с данным именем не найден
		*/
		virtual FileOpeartionResult GetFileInfo( const wchar_t *fileName,
			FileInfo &fileInfo ) = 0;

		/*!
			\brief Закачивает с сервера порцию файла

			\param fileName [in] Имя файла
			\param startPos [in] Позиция файла с которой необходимо прочитать порцию			
			\param chunkBuff [in,out] Буфер для порции файла 
			\param chunkBuffSize [in] Pазмер выделенной памяти
			\param receivedChunkSize [out] Размер закаченной порции

			\return FileChunkResult - результат работы функции
		*/
		virtual FileOpeartionResult GetFileChunk( const wchar_t *fileName,
			long startPos, void *chunkBuff, int chunkBuffSize,  int &receivedChunkSize ) = 0;

		/*!
			\brief Разрывает соединение с сервером
		*/
		virtual void Disconnect() = 0;

	};

} // end namespace KLFT


/*!
	\brief Функция создает экземпляр класса FileTransferBridge, который после использования
	необходимо освободить функцией KLFT_ReleaseFileTransferBridge
*/
void KLFT_CreateFileTransferBridge( KLFT::FileTransferBridge **ppBridge );

/*!
	\brief Функция освобождает экземпляр класса FileTransferBridge
*/
void KLFT_ReleaseFileTransferBridge( KLFT::FileTransferBridge **ppBridge );

#endif // KLFT_FILETRANSFERBRIDGE_H
