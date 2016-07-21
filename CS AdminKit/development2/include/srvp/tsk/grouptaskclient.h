/*!
 * (C) 2002 Kaspersky Lab 
 * 
 * \file	TSK\GroupTaskClient.h
 * \author	Mikhail Karmazine
 * \date	16:29 25.12.2002
 * \brief	заголовочный файл, включаемый клиентами GroupTask'ов
 * 
 */

#ifndef __GROUPTASKCLIENT_H__
#define __GROUPTASKCLIENT_H__

#include <std/base/kldefs.h>
#include <srvp/tsk/grouptaskcontrol.h>

/*!
  \brief —оздает proxy-объект класса KLTSK::GroupTaskControl

    \param wstrURL      [in] адрес soap-сервера
    \param ppGroupTaskControlProxy   [out] ”казатель на объект класса KLTSK::GroupTaskControlProxy
*/

KLCSSRVP_DECL void KLTSK_CreateGroupTaskControlProxy(
                        const wchar_t*  szwAddress,
                        KLTSK::GroupTaskControl**           pp,
                        const unsigned short* pPorts=NULL,
                        bool            bUseSsl = true);

KLCSSRVP_DECL bool KLSRV_IsServerGroupTaskId( const std::wstring & wstrTSId );


#endif // __GROUPTASKCLIENT_H__