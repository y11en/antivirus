/*!
 * (C) 2006 "Kaspersky Lab"
 *
 * \file filesender.h
 * \author Дамир Шияфетдинов
 * \date 2006
 * \brief Интерфейс объекта для передачи файлов при помощи File Transfer
 * 
 */

#ifndef KLFT_FILESENDER_H
#define KLFT_FILESENDER_H

#include <std/base/klbase.h>
#include <std/thr/sync.h>

namespace KLFT {

	const int c_DefaultSendFileTimeout = 3600; // secs ( = 1 hour )
	
	/*!
    * \brief Интерфейс объекта для передачи файлов при помощи File Transfer
    *  
    */
	class KLSTD_NOVTABLE FileSender : public KLSTD::KLBase 
	{	
	public:
		/*!
			\brief Статус отправляемого файла
		*/
		struct SendFileStatus
		{			
			SendFileStatus()
			{
				Init();
			}

			void Init()
			{
				sentSize = 0;
				errorCode = 0;
				sentPercent = 0;
				sendPos = 0;
				fullFileSize = 0;
			}

			int				errorCode;

			unsigned long	fullFileSize;

			unsigned long	sendPos;
			unsigned long	sentSize;
			
			int				sentPercent;
		};

		/*!
			\brief Описание дополнительного файла
		*/
		struct FileInfo
		{
			FileInfo() 
			{
			}
			
			FileInfo( const std::wstring &sourcePath, const std::wstring &archivePath )
			{
				sourceFilePath = sourcePath;
				archiveFilePath = archivePath;
			}

			FileInfo( const std::wstring &sourcePath, const std::wstring &archivePath,
				const std::wstring &archiveNewName )
			{
				sourceFilePath = sourcePath;
				archiveFilePath = archivePath;
				archiveNewFileName = archiveNewName;
			}
			
			FileInfo( const FileInfo &fi )
			{
				sourceFilePath = fi.sourceFilePath;
				archiveFilePath = fi.archiveFilePath;
				archiveNewFileName = fi.archiveNewFileName;
			}

			~FileInfo()
			{
			}

			std::wstring	sourceFilePath;			// путь к дополнительному файлу ( возможно добавить только файл, директории неподдерживаются )
			std::wstring	archiveFilePath;		// относительный путь в архиве
			std::wstring	archiveNewFileName;		// новое имя файла в архиве ( если данная строка пустая, имя файла не изменяется ) ( актуально только в случае если sourceFilePath файл )
		};

		typedef std::vector<FileInfo> FileInfoList;
		
	public:

		/*!
			\brief Функция назначает соединения для использования

			\exception ERR_CONNECTION_BROKEN, ERR_BAD_FUNCTION_PARAMETERS
		*/
		virtual void SetConnection( const std::wstring &localConnName,
			const std::wstring &remoteConnName ) = 0;

		/*!
			\brief Функция отправки файла на сервер FileTransfer

			\exception ERR_CONNECTION_BROKEN, ERR_FILE_NOT_FOUND, ERR_BAD_FUNCTION_PARAMETERS.
					ERR_FILE_IS_ALREADY_UPLOADING

		*/
		virtual void SendFile( std::wstring &fileId,
			FileInfo *filesArray = NULL, int filesArraySize = 0,
			int lifeTime = c_DefaultSendFileTimeout ) = 0;

		/*!
			\brief Функция прерывает отправку файла 

			\exception ERR_WRONG_FILE_ID
		*/
		virtual void CancelSendFile( const std::wstring &fileId ) = 0;

		/*!
			\brief Функция ожидания окончания операции отправки файла на сервер

			\exception ERR_WRONG_FILE_ID, ERR_ERROR_OF_UPLOAD 
		*/
		virtual bool WaitFileSent( const std::wstring &fileId, 
			int waitTimeout = KLSTD_INFINITE ) = 0;

		/*!
			\brief Получение текущего статуса отправляемого файла

			\exception ERR_WRONG_FILE_ID
		*/
		virtual void GetFileStatus( const std::wstring &fileId, 
			SendFileStatus &fileStatus ) = 0;

		
	};	

} // end namespace KLFT

/*\brief Создает объект класса FileSender */
KLCSKCA_DECL void KLFT_CreateFileSender( KLFT::FileSender **ppFileSender );

#endif // !defined(KLFT_FILESENDER_H)
