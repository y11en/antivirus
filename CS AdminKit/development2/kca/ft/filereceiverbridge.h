/*!
* (C) 2005 "Kaspersky Lab"
*
* \file filereceiverbridge.h
* \author Шияфетдинов Дамир
* \date 2005
* \brief Класс для локального межпроцесного взаимодействия с FileReceiver'ом.
*
*/

#ifndef FT_FILERECEIVERBRIDGE_H
#define FT_FILERECEIVERBRIDGE_H

#include <map>

#include <std/base/klbase.h>
#include <std/err/klerrors.h>
#include <std/thr/sync.h>

#include <kca/contie/connectiontiedobjectshelper.h>
#include <kca/ft/filetransferbridgebase.h>

#include "filereceiver.h"

namespace KLFT {	
	
	/*!
	* \brief Класс для локального межпроцесного взаимодействия с FileReceiver'ом.
	*/	

	class FileReceiverBridge : public KLSTD::KLBaseImpl<FileReceiverBridgeBase> 
	{	
	public:
		
		

		class FileReceiverBridgeDisconnectSink : public KLSTD::KLBaseImpl<CONTIE::DisconnectSink>
		{
		public:
			FileReceiverBridgeDisconnectSink() {}

			void Init( FileReceiverBridge *pBridge )
			{
				m_pBridge = pBridge;
			}

			virtual void OnDisconnectObjectID(
                            const std::wstring& wstrObject)
			{
				m_pBridge->OnDisconnectObjectID( wstrObject );
			}

		private:
			FileReceiverBridge *m_pBridge;
		};

		friend class FileReceiverBridgeDisconnectSink;

		
		FileReceiverBridge();
		~FileReceiverBridge();

		void Init();

		void CreateReceiver( std::wstring& receiverId,
			const std::wstring& localConnectionName,
			const std::wstring& remoteConnectionName );

		void CreateReceiver( std::wstring& receiverId );

		ConnectResult ConnectToServer( const std::wstring& receiverId );
		
		ConnectResult ConnectToMasterServer( const std::wstring& receiverId );

		FileOpeartionResult GetFileChunk( const std::wstring& receiverId,
			const std::wstring& fileName, unsigned long startFilePos, 			 
			void *chunkBuff, int chunkBuffSize,  int &receivedChunkSize );

		FileOpeartionResult GetFileInfo( const std::wstring& receiverId, const std::wstring &fileName, 
			FileInfo &r );

		void ReleaseReceiver( const std::wstring& receiverId );
		

	private:

		/* Class data members */
		struct ReceiverBridgeUnit
		{
			ReceiverBridgeUnit()
			{
				m_bConnected = false;
				m_bUseUAFolderSync = false;
			}

			ReceiverBridgeUnit( const ReceiverBridgeUnit &unit )
			{
				m_receiverId = unit.m_receiverId;
				m_bConnected = unit.m_bConnected;
				m_serverConnName = unit.m_serverConnName;
				m_localName = unit.m_localName;
				m_bUseUAFolderSync = unit.m_bUseUAFolderSync;
				m_folderSyncName = unit.m_folderSyncName;
				m_pFolderSync = unit.m_pFolderSync;
				m_wstrHostIdForReserveUpdates = unit.m_wstrHostIdForReserveUpdates;
			}
			
			std::wstring						m_receiverId;
			bool								m_bConnected;			
			std::wstring						m_serverConnName;
			std::wstring						m_localName;
			bool								m_bUseUAFolderSync;
			std::wstring						m_folderSyncName;
			KLSTD::CAutoPtr<KLFT::FolderSync>	m_pFolderSync;
			std::wstring						m_wstrHostIdForReserveUpdates;
		};

		typedef std::map<std::wstring,ReceiverBridgeUnit>	ReceiverBridgeUnitMap;

		ReceiverBridgeUnitMap									m_ReceiversUnits;		

		KLSTD::CAutoPtr<KLSTD::CriticalSection>					m_pCricSec;

		KLSTD::CAutoPtr<CONTIE::ConnectionTiedObjectsHelper>	m_pContie;		
		FileReceiverBridgeDisconnectSink						m_DisconnectSink;
		std::wstring											m_simpleSocketListenerLoc;
		std::wstring											m_socketListenerName;


	protected:
		/* Helpers function */
		void OnDisconnectObjectID( const std::wstring& wstrObject );

		bool CheckFolderSyncUpdateAgent( const std::wstring &folderSyncName );

		ConnectResult ConnectToFolderSyncUpdateAgent( 
			const std::wstring &folderSyncName,
			KLSTD::CAutoPtr<KLFT::FolderSync> &pFolderSync,
			std::wstring &localName, std::wstring &remoteName );

		void UploadSyncFolderFile( const std::wstring &localName, 
			const std::wstring &remoteName, KLFT::FolderSync *pFolderSync,
			const std::wstring &fileName, const std::wstring &receiverId );
	};

} // end namespace KLFT

/*\brief Возвращает указатель на глобальный объект класса FileTransfer */
KLFT::FileReceiverBridgeBase *KLFT_GetFileReceiverBridge();

#endif // FT_FILERECEIVERBRIDGE_H
