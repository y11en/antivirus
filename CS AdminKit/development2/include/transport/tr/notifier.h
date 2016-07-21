/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file notifier.h
 * \author Дамир Шияфетдинов
 * \date 2004
 * \brief Интерфейс для регистрации обработчиков событий транспрта
 * 
 */


#ifndef KLTR_NOTIFIER_H
#define KLTR_NOTIFIER_H

#include <string>
#include <list>

#include <std/par/params.h>

#include "common.h"
#include "errors.h"

namespace KLTR {	

	const int  NumberOfNotification = 11;

	enum NotificationType
	{
		ComponentConnected,			//!< Произошло соединения с удаленной компонентой

		ComponentDisconnected,		//!< Соединение с удаленной компонентой разорванно

		ConnectionEstablished,		//!< Соединение было успешно установленно 
									// ( если одна из зарегестрированных функций вернет 
									// false соединение будет принудительно закрыто )

		ConnectionOnClose,			//!< Соединение после вызова данной нотификации будет закрыто

		ConnectionBroken,			//!< Соединение разорвано 
									// ( вызывается сразу после зарытия соединения )

		UnicastPacketArrived,		//!< Пришел unicast пакет для создания соединений

		PingTimeoutElapsed,			//!< Истекло время Ping'а

		UnusedConnectionCheck,
									//!< В данной нотификации производиться проверка возможности
									// закрыть данное соединени как давно неиспользуемое и не 
									// критичное для базовой функциональности. true - данное 
									// соединение может быть закрыто. Траснпортная подсистема 
									// закрывает неиспользованные соединения только если достигнут 
									// лимит количества соедиенений в данном процессе
		ParamsUnicastPacketArrived, //!< Пришел unicast пакет с параметрами

		MulticastPacketArrived,		//!< Пришел multicast пакет
		
		ConnectByLocation = (NumberOfNotification - 1)
									//!< Оповещения об успешном соединение на нижнем уровне ( используется 
									//		для кеширования параметров созданных соединений ) см. NotificationConnectByLocation
	};

	class NotificationReceiverBase : public KLSTD::KLBaseQI
	{
	};

	class NotificationReceiver : public NotificationReceiverBase
	{
	public:
		
		virtual bool TransportNotification( NotificationType actionType, 
				const std::wstring &localName,
				const std::wstring &remoteName,
				bool remoteConnectionFlag ) = 0;
	};

	class NotificationUnicastPacket : public NotificationReceiverBase
	{
	public:
		
		virtual void UnicastPacketArrived( const KLPAR::Params *pParams, 
			const KLPAR::BinaryValue *pTransportPacket ) = 0;
	};

	class NotificationMulticastPacket : public NotificationReceiverBase
	{
	public:
		
		virtual void MulticastPacketArrived( const std::wstring &mcGroupName,
			const KLPAR::BinaryValue *pTransportPacket ) = 0;
	};
	
	class NotificationConnectByLocation : public NotificationReceiverBase
	{
	public:

		enum ConnectByLocationStatus
		{
			Connected,			// успешное подключение
								// remoteLocation [in] - имя удаленного хоста переданного в функцию AddConnection
								// remoteIpAddr [in] - ip адрес удаленного хоста в формате xxx.xxx.xxx.xxx
								// return - true адрес успешно сохранен в хранилище

			ResolvedError,		// ошибка опеределения ip адреса по имени хоста
								// remoteLocation [in] - имя удаленного хоста переданного в функцию AddConnection
								// remoteIpAddr [out] - ip адрес по которому необходимо произвести повторную попытку подключения
								//						в формате xxx.xxx.xxx.xxx
								// return - true адрес записан в переменной remoteIpAddr

			ConnectionError		// ошибка подключения по указанному имени хоста
								// remoteLocation [in] - имя удаленного хоста переданного в функцию AddConnection
								// remoteIpAddr [out] - ip адрес по которому необходимо произвести повторную попытку подключения
								//						в формате xxx.xxx.xxx.xxx
								// return - true адрес записан в переменной remoteIpAddr
		};
		
		virtual bool ConnectByLocationNotification( ConnectByLocationStatus status, 
			const std::wstring &remoteLocation,
			std::wstring &remoteIpAddr ) = 0;
	};


	class TransportNotifier
	{
	public:
		/*!
			\brief Добавляет получателя нотификации транспорта

			\param receiver [in] получатель
			\param needType [in] необходимый получателю тип нотификаций
		*/
		virtual void AddNotificationReceiver( NotificationReceiverBase *receiver, 
			NotificationType needType ) = 0;

		/*!
			\brief Добавляет простого получателя нотификации транспорта
			( будет вызываться для след. нотификаций:
				ComponentConnected,
				ComponentDisconnected,
				UnicastPacketArrived,
				PingTimeoutElapsed. )

			\param receiver [in] получатель			
		*/		
		virtual void AddSimpleNotificationReceiver( NotificationReceiverBase *receiver ) = 0;

		/*!
			\brief Удаляет получателя из списка 

			Данный получатель перестанет получать все нотификации от транспорта
		*/
		virtual void DeleteNotificationReceiver( NotificationReceiverBase *receiver ) = 0;
	};


} // end namespace KLTR

#endif // KLTR_NOTIFIER_H
