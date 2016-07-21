/*!
 * (C) 2004 "Kaspersky Lab"
 *
 * \file FileDescription.h
 * \author Дамир Шияфетдинов
 * \date 2004
 * \brief Описание рабочего файла
 * 
 */

#ifndef KLFT_FILEDESCRIPTION_H
#define KLFT_FILEDESCRIPTION_H

#include <std/base/klbase.h>
#include <kca/prci/componentid.h>
#include <transport/tr/transport.h>

namespace KLFT {

	class FileDescription
	{
	public:		

		FileDescription()
		{
			m_fileName.clear();// = L"";
			m_fileId.clear();// = L"";
			m_description.clear();// = L"";
			m_currentUploadPos = 0;
			m_operationTime = 0;
			m_fileSize = 0;
			m_isDirectory = false;
			m_maxNumberUploadingClients = 0;
		}

		FileDescription( const FileDescription &fd )
		{
			m_status			= fd.m_status;
			m_fileName			= fd.m_fileName;
			m_fileId			= fd.m_fileId;
			m_description		= fd.m_description;		
			m_operationTime		= fd.m_operationTime;		
			m_fileSize			= fd.m_fileSize;		
			m_currentUploadPos	= fd.m_currentUploadPos;		
			m_sentLocsList		= fd.m_sentLocsList;
			m_notSendLocsList	= fd.m_notSendLocsList;
			m_uploadLocsList	= fd.m_uploadLocsList;
			m_isDirectory		= fd.m_isDirectory;
			m_maxNumberUploadingClients = fd.m_maxNumberUploadingClients;
		}

		
		enum FileStatus{ Uploaded, Uploading, Sent, Sending, Delivered };
		
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
		
		KLTRAP::Transport::LocationExList	m_sentLocsList;			//! Список клиентов, которым были отосланны сигналы на загрузку данного файла
		KLTRAP::Transport::LocationExList	m_notSendLocsList;		//! Список клиентов, для которых еще не посланны сигналы на загрузку
		
		KLTRAP::Transport::LocationExList	m_uploadLocsList;		//! Список клиентов, которые уже скачали данный файл
	};

} // end namespace KLFT

#endif // !defined(KLFT_FILEDESCRIPTION_H)

