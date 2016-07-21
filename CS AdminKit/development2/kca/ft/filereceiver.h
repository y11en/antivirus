/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file FileReceiver.h
 * \author Дамир Шияфетдинов
 * \date 2004
 * \brief Главный интерфейс для закачки файлов
 * 
 */

#ifndef KLFT_FILERECEIVER_H
#define KLFT_FILERECEIVER_H

#include <time.h>

#include <std/base/klbase.h>
#include <kca/ft/filetransferconst.h>

namespace KLFT {

	/*!
		\brief Максимальный размер порции для закачки
	*/
	const int c_MaxChunkSize = 32768;	// 32 Kb

	class KLSTD_NOVTABLE FileReceiver : public KLSTD::KLBase 
	{	
	public:
		
		/*!
			\brief Результат работы функции Connect
		*/
		enum ConnectResult
		{
			CR_Ok,					//!< Соединение установелено успешно
			CR_ServerBusy,			//!< Сервер на данный момент занят и не может обрабатывать запросы от клиента
			CR_ConnectionError		//!< Ошибка физического подключений к серверу
		};

		/*!
			\brief Результат работы функции GetFileChunk
		*/
		enum FileChunkResult
		{
			FC_Ok,					//!< Порция файла успешно закачена
			FC_OkDownloaded,		//!< Порция файла успешно закачена и это была последняя порция данного файла
			FC_WrongArguments,		//!< Неверные парамеры функции
			FC_FileNotFound,		//!< Файл с данным именем не найден
			FC_Timeout,				//!< timeout
			FC_ErrorInOperation		//!< Ошибка при закачки очередной порции файла			
		};

	public:

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
			\brief Функция назначает соединения для использования
		*/
		virtual void SetConnectParam( const std::wstring &localConnName,
			const std::wstring &remoteConnName ) = 0;

		/*!
			\brief Возвращает информацию о файле

			\return false - файл с данным именем не найден
		*/
		virtual bool GetFileInfo( const std::wstring &fileName,
			FileInfo &fileInfo ) = 0;

		/*!
			\brief Закачивает с сервера порцию файла

			\param fileName [in] Имя файла
			\param startPos [in] Позиция файла с которой необходимо прочитать порцию			
			\param chunkBuff [in,out] Буфер для порции файла 
			\param chunkBuffSize [in] Pазмер выделенной памяти
			\param receivedChunkSize [out] Размер закаченной порции

			\return FileChunkResult - результат работы функции
				FC_WrongArguments - возвращается в следующих случаях:
						chunkBuff == NULL
						chunkBuffSize = 0
						startPos < 0 или startPos выходит за пределы файла
		*/
		virtual FileChunkResult GetFileChunk( const std::wstring &fileName,
			long startPos, void *chunkBuff, int chunkBuffSize,  int &receivedChunkSize ) = 0;

		/*!
			\brief Стартует операции по закачке опеределенного файла
		*/
		virtual bool StartFileReceiving( const std::wstring &fileName ) = 0;

		/*!
			\brief Закачивает с сервера порцию текущего файла ( текущий файл устанавливается 
				функцией StartFileReceiving )
			\params см. функцию GetFileChunk
		*/
		virtual FileChunkResult GetCurrentFileChunk( void *chunkBuff, int chunkBuffSize,  
			int &receivedChunkSize ) = 0;

		/*!
			\brief Разрывает соединение с сервером
		*/
		virtual void Disconnect() = 0;
	};	

} // end namespace KLFT

/*\brief Создает объект класса FileReceiver */
KLCSNAGT_DECL void KLFT_CreateFileReceiver( KLFT::FileReceiver **ppFileReceiver );

#endif // !defined(KLFT_FILERECEIVER_H)
