/*!
* (C) 2005 "Kaspersky Lab"
*
* \file wakeonlansubnet.h
* \author Шияфетдинов Дамир
* \date 2007
* \brief Интерфейс класса распеределения рассылки WakeOnLan сигнала на хосты находящиеся в подсетых отличной 
	от сервероной
*
*/

#ifndef KLTSK_WAKEONLANSUBNET_H
#define KLTSK_WAKEONLANSUBNET_H

#include <std/base/klbase.h>
#include <std/err/klerrors.h>

#include <server/srv_common.h>

namespace KLTSK {

	/*!
		\brief 

		Для указанной задачи производит следующий набор действий:

		1. Выделяет из хостов входящих в задачу, группы хостов, которые распологаются в разных с сервером подситях
		2. Выбирает несколько хостов из найденных подситей по признаку времени последнего подключения
		3. Отсылает на выбранные хосты из подситей сигнала о необходимости рассылки WakeOnLan сигнала		
	
	*/	

	class WakeOnLanSubnet : public KLSTD::KLBase {
	public:

		/*!
			\brief Инициализация подсистемы для работы в качестве сервера
		*/
		virtual void Init( KLSRV::SrvData* pSrvData ) = 0;

		/*!
			\brief Отсылает сигнал хостам из других подситей 
		*/
		virtual void SendSignalForTask( long nTaskId, long timeout ) = 0;

		/*!
			\brief Останавливает работу подсистемы
		*/
		virtual void Shutdown() = 0;		

	};

	

} // end namespace KLTSK

void KLTSK_CreateWakeOnLanSubnet( KLTSK::WakeOnLanSubnet **ppWolSubnet );

#endif // KLTSK_WAKEONLANSUBNET_H