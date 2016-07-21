/*!
* (C) 2005 "Kaspersky Lab"
*
* \file filetransferupdateagentsproxy.h
* \author Шияфетдинов Дамир
* \date 2005
* \brief Интерфейс для управления агентами обновлений из консоли администрирования
*
*/

#ifndef KLFT_FILETRANSFERUPDATEAGENTSPROXY_H
#define KLFT_FILETRANSFERUPDATEAGENTSPROXY_H

#include <std/base/klbase.h>
#include <std/err/klerrors.h>

#include <kca/ft/filetransferconst.h>

namespace KLFT {
	
		

	class KLSTD_NOVTABLE FileTransferUpdateAgentsProxy : public KLSTD::KLBaseQI
	{
	public:

		struct UpdateAgentInfo
		{
			UpdateAgentInfo()
			{
				nAgentHostId = (-1);
				nGroupId = (-1);
				
				nPort = nSSLPort = 0;
			}
			int				nAgentHostId;
			std::wstring	strAgentHostId;
			std::wstring	strAgentHostDisplayName;
			int				nGroupId;
	
			int				nPort;
			int				nSSLPort;
			std::wstring	strMulticastAddr;
		};

		typedef std::vector<UpdateAgentInfo> UpdateAgentInfoList;

		virtual void GetDefaultUpdateAgentRegistrationInfo( int nGroupId,
			UpdateAgentInfo &defaultUpdateAgentInfo ) = 0;

		virtual void RegisterUpdateAgent( const UpdateAgentInfo &updateAgentInfo,
			bool checkExistingFlag = true ) = 0;
		
		virtual void ModifyUpdateAgent( const UpdateAgentInfo &updateAgentInfo ) = 0;

		virtual void UnregisterUpdateAgent( const std::wstring &strAgentHostId ) = 0;

		virtual void GetUpdateAgentsList( UpdateAgentInfoList &updateAgentsList ) = 0;

		virtual void GetUpdateAgentsListForGroup( int nGroupId,
			UpdateAgentInfoList &updateAgentsList ) = 0;
		
		virtual void GetUpdateAgentInfo( const std::wstring& strAgentHostId,
			UpdateAgentInfo &updateAgentInfo ) = 0;
		
		/*
			\brief Работа с признаком распространения папки через агенты обновлений

			Имена предопеределенных списков:

			 KLFT::SYNC_FOLDER_NAME_UPDATE - папка Updater'а
		*/
		virtual bool GetUseSyncFolderFlag( const std::wstring& strAgentHostId,
			const std::wstring& strSyncFolderName ) = 0;

		virtual void SetUseSyncFolderFlag( const std::wstring& strAgentHostId,
			const std::wstring& strSyncFolderName, bool bUse ) = 0;

	};

} // end namespace KLFT

#endif // KLFT_FILETRANSFERUPDATEAGENTSPROXY_H