/*!
* (C) 2006 "Kaspersky Lab"
*
* \file multicastsender.h
* \author Шияфетдинов Дамир
* \date 2006
* \brief Интерфейс рассылки содержимого файла по протоколу multicast
*
*/

#ifndef KLFT_MULTICASTSENDER_H
#define KLFT_MULTICASTSENDER_H

#include <std/base/klbase.h>
#include <std/err/klerrors.h>

#include <std/tp/threadspool.h>

namespace KLFT {

	const int c_MulticastSendChunkSize		= 5120;	// bytes
	const int c_MulticastSendChunkTimeout	= 200;	// msec

	class MulticastSender : public KLTP::ThreadsPool::Worker
	{
	public:

		//!\brief callback класс для получения содержимого файла
		class FileProvider : public KLSTD::KLBase
		{
		public:
			virtual int GetFileInfo( const std::wstring &fileId, 
				const std::wstring &fileName,
				unsigned long &fullSize,
				std::wstring &folderName ) = 0;

			virtual int GetFileChunk( const std::wstring &fileId, 
				const std::wstring &fileName,
				unsigned long pos,
				void *buff, unsigned long buffSize, 
				bool &endOfFile ) = 0;

			virtual int OnFileSent( const std::wstring &fileId,
				const std::wstring &fileName, 
				const unsigned long sentSize ) = 0;
		};

		//!\brief Инициализация подсистемы
		virtual void Init( const std::wstring &strMulticastIP, 
			KLPAR::BinaryValue *pPrivateSSLKey,
			FileProvider *pFileProvider,
			int chunkSize = c_MulticastSendChunkSize,
			int chunkTimeout = c_MulticastSendChunkTimeout ) = 0;

		//!\brief Остановка работы отправки файлов
		virtual void Stop() = 0;

		//!\brief Иницирования отправки файла по multicast
		virtual void SendFile( const std::wstring &fileId, 
			unsigned long fullFileSize ) = 0;

		//!\brief Иницирования отправки файлов по multicast
		virtual void SendSyncFolderFiles( const std::wstring &syncFolderId, 
			const std::vector<std::wstring> &filesNames ) = 0;

		//!\brief Производит поиск указанного файла во внтуреннем списке рассылки
		virtual bool IsFileSending( const std::wstring &fileId ) = 0;

	};

} // end namespace KLFT

KLCSKCA_DECL void KLFT_CreateMulticastSender( KLFT::MulticastSender **ppMCSender );

#endif // KLFT_MULTICASTSENDER_H
