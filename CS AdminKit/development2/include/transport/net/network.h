/*!
 * (C) 2005 "Kaspersky Lab"
 *
 * \file network.h
 * \author Дамир Шияфетдинов
 * \date 2005
 * \brief Главный интерфейс модуля работы с сетью
 * 
 */


#ifndef KLNET_NETWORK_H
#define KLNET_NETWORK_H

#include <string>
#include <list>

#include <std/par/params.h>

#include "errors.h"

namespace KLNET {	

	class Network
	{
	public:
		/*!
			\brief Функция посылает Wake On Lan сигнал на удаленный хост
		
			\param pMacAddrs [in] список MAC адресов компьютера
				каждый элемент строка hex вида
		   
			 \exception NETERR_INVALID_PARAMETER
			 
			 \return true - сигнал был успешно отправлен, false - ошибка отправки
		*/
		virtual bool SendWakeOnLanSignal( KLPAR::ArrayValue *pMacAddr ) = 0;

		/*!
			\brief Функция определяет возможность создания RDP соединения
		 
			 \return true - возможно создание RDP соединения
		*/
		virtual bool IsRDPConnectionEnabled() = 0;

		/*!
			\brief Функция запускает приложения для создания RDP соединения

			\param hostAddr [in] адрес хоста,  с которым необходимо создать соединение
		 
			\exception NETERR_INVALID_PARAMETER, NETERR_ERR_START_RDP_APPS
		*/
		virtual void CallRDPConnetionApps( const std::wstring &hostAddr ) = 0;
		
		/*!
			\brief Функция производит выключение cистемы ( shutdown )
		*/
		virtual void ShutdownLocalSystem() = 0;
		
		/*!
			\brief Функция асинхронно посылает Wake On Lan сигнал на удаленный хост
		*/
		virtual void SendWakeOnLanSignalAsync( KLSTD::CAutoPtr<KLPAR::ArrayValue> pMacAddr ) = 0;
		
		/*!
			\brief Функция производит выключение cистемы ( shutdown ) c дополнительными флагами
		*/
		virtual void ShutdownLocalSystemEx( bool bForce, bool bRestart ) = 0;

	};

} // end namespace KLNET

DECLARE_MODULE_INIT_DEINIT2( KLCSTR_DECL, KLNET );

/*\brief Возвращает указатель на глобальный объект класса Network */
KLCSTR_DECL KLNET::Network *KLNET_GetNetwork();

#endif // KLNET_NETWORK_H
