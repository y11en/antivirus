/*!
 * (C) 2004 "Kaspersky Lab"
 *
 * \file ServerFileDescription.h
 * \author Дамир Шияфетдинов
 * \date 2004
 * \brief Описание рабочего файла
 * 
 */

#ifndef KLFT_SERVERFILEDESCRIPTION_H
#define KLFT_SERVERFILEDESCRIPTION_H

#include <std/base/klbase.h>
#include <kca/prci/componentid.h>
#include <transport/tr/transport.h>

namespace KLFTS {

	struct HostDesc
	{
		HostDesc()
		{
			nHostId = (-1);
		}
		
		std::wstring	strHostName;
		std::wstring	strHostConnectionName;
		std::wstring	strHostDisplayName;		
		int				nHostId;
		
		bool operator == (const std::wstring &hostName ) const
        {
			return (strHostName==hostName);
        };
		
		bool operator == (const int &hostId ) const
        {
			return (nHostId==hostId);
        };
	};

	typedef std::vector<HostDesc> HostDescList;

	typedef std::list<int>						HostsIdsList;

	class ServerFileDescription
	{
	public:
		

		ServerFileDescription()
		{
			m_fileName = L"";
			m_fileId = L"";
			m_description = L"";
			m_currentUploadPos = 0;
			m_operationTime = 0;
			m_fileSize = 0;
			m_isDirectory = false;
			m_maxNumberUploadingClients = 0;
		}

		ServerFileDescription( const ServerFileDescription &fd )
		{
			m_status			= fd.m_status;
			m_fileName			= fd.m_fileName;
			m_fileId			= fd.m_fileId;
			m_description		= fd.m_description;		
			m_operationTime		= fd.m_operationTime;		
			m_fileSize			= fd.m_fileSize;		
			m_currentUploadPos	= fd.m_currentUploadPos;		
			m_sentHostsList		= fd.m_sentHostsList;
			m_notSendHostsList	= fd.m_notSendHostsList;
			m_uploadHostsList	= fd.m_uploadHostsList;
			m_isDirectory		= fd.m_isDirectory;
			m_maxNumberUploadingClients = fd.m_maxNumberUploadingClients;
			m_notSendUAList		= fd.m_notSendUAList;
			m_pParContext		= fd.m_pParContext;
		}

		
		enum FileStatus{ Uploaded, Uploading, Sent, Sending, Delivered, NotStartUploading };
		
		/*!\brief Статус проходит следующие состояния:
		
			Для клиента:
			Uploading - клиент получил сигнал о  необходимости закачки файла и производит его загрузку
			Uploaded - клиент полностью скачал файл и послал серверу сигнал об окончании работы
		  
			Для сервера:
			Sending	- сервер находиться в процессе посылки сигналов клиентам ( еще не все клиенты получили сигналы )
			Sent - сервер отправил сигналы всем клиентам
			Delivered - данный файл был скачан всеми клиентами 
		*/
		FileStatus		m_status;			//! Текущий статус файла
		
		std::wstring	m_fileName;			//!	Имя файла
		std::wstring	m_fileId;			//! Идентификатор файла, сгенерированный сервером FT

		std::wstring	m_description;		//! Описание файла

		bool			m_isDirectory;		//! Признак дирректории
		
		time_t			m_operationTime;	//! Время создании записи об операции с данным файлом
		
		time_t			m_createFileTime;	//! Время создания файла
		
		unsigned long	m_fileSize;			//! Размер файла
		
		unsigned long	m_currentUploadPos;	//! Текущая позиция загрузки данного файла ( актуальна для статуса Uploading )
		
		int				m_maxNumberUploadingClients;	//! Максимальное кол-во клиентов для одновременной закачки
		
		HostDescList	m_sentHostsList;	//! Список хостов, которым были отосланны сигналы на загрузку данного файла
		HostDescList	m_notSendHostsList;	//! Список хостов, для которых еще не посланны сигналы на загрузку
		
		HostDescList	m_uploadHostsList;	//! Список хостов, которые уже скачали данный файл

		HostDescList	m_notSendUAList;	//! Список хостов UA, для которых еще не посланны сигналы на загрузку

		KLSTD::CAutoPtr<KLPAR::Params>	m_pParContext;	//! Пользовательский контекст файла
	};

} // end namespace KLFTS

#endif // !defined(KLFT_SERVERFILEDESCRIPTION_H)

