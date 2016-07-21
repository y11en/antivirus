/*!
* (C) 2005 "Kaspersky Lab"
*
* \file filereceiverbridgebase.h
* \author Шияфетдинов Дамир
* \date 2007
* \brief Класс для закачки файлов с сервера администрирования
*
*/

#ifndef FT_FILEBRIDGEBASE_H
#define FT_FILEBRIDGEBASE_H

#include <map>

#include <std/base/klbase.h>
#include <std/err/klerrors.h>
#include <std/thr/sync.h>

#include <kca/ft/filetransferbridge.h>

namespace KLFT {	
	
	class FileReceiverBridgeBase : public KLSTD::KLBase 
	{	
	public:

		virtual void CreateReceiver( std::wstring& receiverId,
			const std::wstring& localConnectionName,
			const std::wstring& remoteConnectionName ) = 0;

		virtual void CreateReceiver( std::wstring& receiverId ) = 0;

		virtual ConnectResult ConnectToServer( const std::wstring& receiverId ) = 0;
		
		virtual ConnectResult ConnectToMasterServer( const std::wstring& receiverId ) = 0;

		virtual FileOpeartionResult GetFileChunk( const std::wstring& receiverId,
			const std::wstring& fileName, unsigned long startFilePos, 			 
			void *chunkBuff, int chunkBuffSize,  int &receivedChunkSize ) = 0;

		virtual FileOpeartionResult GetFileInfo( const std::wstring& receiverId, const std::wstring &fileName, 
			FileInfo &r ) = 0;

		virtual void ReleaseReceiver( const std::wstring& receiverId ) = 0;
		
	};

} // end namespace KLFT

/*\brief Возвращает указатель на глобальный объект класса FileTransferBridge */
KLCSKCA_DECL KLFT::FileReceiverBridgeBase *KLFT_GetFileReceiverBridge();

#endif // FT_FILEBRIDGEBASE_H
