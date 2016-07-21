/*!
* (C) 2005 "Kaspersky Lab"
*
* \file updateagentinfo.h
* \author Шияфетдинов Дамир
* \date 2005
* \brief Интерфейс 
*
*/

#ifndef KLFT_UPDATEAGENTINFO_H
#define KLFT_UPDATEAGENTINFO_H

#include <std/base/klbase.h>
#include <std/err/klerrors.h>

namespace KLFT {

	struct UpdateAgentInfo
	{		
		UpdateAgentInfo()
		{
			nAgentHostId = (-1);
		}

		UpdateAgentInfo( const UpdateAgentInfo &uai )
		{
			nAgentHostId = uai.nAgentHostId;
			strAgentHostId = uai.strAgentHostId;
			strAgentHostDisplayName = uai.strAgentHostDisplayName;
			nGroupId = uai.nGroupId;
			strConnName = uai.strConnName;
			strAddress = uai.strAddress;
			strNonSSLAddress = uai.strNonSSLAddress;
			strMulticastAddr = uai.strMulticastAddr;
			pCertificate = uai.pCertificate;
			strCertPswd = uai.strCertPswd;
		}


		int nAgentHostId;	// Идентификатор хоста, который будет использоватся в качестве агента обновлений ( UA )
		std::wstring strAgentHostId;	// Строковой идентификатор хоста
		std::wstring strAgentHostDisplayName;// Строка для отображения названия хоста

		int nGroupId;		// Идентификатор группы, хосты из который данный UA будет обслуживать 
							// ( если в функцию регистрации UA передан nGroupId < 0 то будет испльзованна группу который пердлежит UA )
		std::wstring strConnName;	// Транспротное имя
		std::wstring strAddress;	// TCP/IP адресс UA ( если в функцию регистрации UA передана пустая строка, 
					// то будет использован адрес указнный в описания данного хоста + порт 13000 )
		std::wstring strNonSSLAddress;	// TCP/IP адресс UA ( если в функцию регистрации UA передана пустая строка, 
					// то будет использован адрес указнный в описания данного хоста + порт 13000 )
		std::wstring strMulticastAddr; // Multicast адрес рассылки ( если передана пустая строка,
					// то адресс будет подобран автоматически )
		KLSTD::CAutoPtr<KLSTD::MemoryChunk> pCertificate;		// [in] Сертификат, который будет использовать для подписи 
					// пакетов рассылки и при создании подключений 
					// ( если в функцию регистрации UA передан NULL то сертификат будет сгенерирован внутри функции регистрации UA )
		std::wstring strCertPswd;	// Пароль сертификата
	};

	struct UpdateAgentLocation
	{
		std::wstring strConnectionName;
		std::wstring strLocation;
		std::wstring strSSLLocation;
		std::wstring strMulticastAddr;
	};

	typedef std::vector<UpdateAgentLocation> UpdateAgentLocationList;

	typedef std::vector<UpdateAgentInfo> UpdateAgentInfoList;
	

	struct UpdateAgentStatistics
	{
		UpdateAgentStatistics()
		{			
			time(&tCreateTime);

			nNumberOfFiles = 0; 
			nSizeOfFiles = 0;
		
			nSuccededUploads = 0;
			nSuccededUploadsSize = 0;

			nTCPUploadedSize = nMulticastUploadedSize = 0;

			nMulticastSentSize = 0;

			tFolderSyncLastServerSyncTime = 0;
			nFolderSyncServerSyncCount = 0;

			nFolderSyncMulticastSentCount = 0;
			nFolderSyncMulticastSentSize = 0;

			nFolderSyncTCPUploadedSize = 0;
			nFolderSyncTCPUploadedCount = 0;
		}
		
		time_t			tCreateTime;					// время создания агента обновления

		//////////////// Удаленная инсталяция ////////////////
		long			nNumberOfFiles;					// Обшее число файлов, поступивших для распространения через агента обновлений
		AVP_longlong	nSizeOfFiles;					// Общий размер файлов
		
		long			nSuccededUploads;				// Общее число успешный закачек с данного агента обновлений
		AVP_longlong	nSuccededUploadsSize;			// сумарный размер успешных закачек

		AVP_longlong	nTCPUploadedSize;				// размер закаченной информации по TCP/IP
		AVP_longlong	nMulticastUploadedSize;			// размер полученной информации по протоколу Multicast

		AVP_longlong	nMulticastSentSize;				// размер отосланной по multicast информации

		long			nPercentUsageMulticast;			// доля информации полученная клиентом по протоколу multicast
		//////////////// 

		//////////////// Антивирусные базы //////////////// 
		time_t			tFolderSyncLastServerSyncTime;	// время последней синхронизации с сервером
		long			nFolderSyncServerSyncCount;		// кол-во синхронизаций папки с сервером

		long			nFolderSyncMulticastSentCount;	// общее кол-во multicast рассылок
		AVP_longlong	nFolderSyncMulticastSentSize;	// размер multicast рассылки для синхронизации папок ( обновлений )		

		AVP_longlong	nFolderSyncTCPUploadedSize;		// размер TCP закачки для синхронизации папок ( обновлений )
		long			nFolderSyncTCPUploadedCount;	// кол-во синхронизаций с клиентом 
		long			nFolderSyncPercentUsageMulticast;	// доля информации полученная клиентом по протоколу multicast при синхронизации антивирусных баз
		//////////////// 
		
	};

} // end namespace KLFT

#endif // KLFT_UPDATEAGENTINFO_H
