/*!
 * (C) 2002 Kaspersky Lab 
 * 
 * \file	prcp_const.h
 * \author	Andrew Kazachkov
 * \date	01.10.2002 12:34:51
 * \brief	 онстанты модул€ Product Component Proxy (PRCP).
 * 
 */

#ifndef KLPRCP_PRCP_CONST_H
#define KLPRCP_PRCP_CONST_H

#include <time.h>
#include <string>
#include <vector>

#include "std/err/error.h"
#include "std/par/params.h"

namespace KLPRCP {	

	const wchar_t c_szwProxyName[]=L"klprcp-Proxy";
    const wchar_t c_szwTransparentProxyName[]=L"klprcp-TransparentProxy";


    /*!
		\brief »мена оповещений, св€занных с компонентами.
	*/

	const wchar_t c_EventComponentState[]=L"ComponentState";



	/*!
		¬ тела всех событий, публикуемых с помощью метода Publish 
		добавл€ютс€ следующие атрибуты:
	*/
	
	/*!  оличество миллисекунд (дополнительно к времени возникновени€ событи€
		от 0 до 999
	*/
	const wchar_t c_szwEventMilliseconds[] = L"klprci-milliseconds"; // INT_T

	/*! Ќомер событи€ (в рамках текущей сессии).
	*/   
    const wchar_t c_szwEventNumber[] = L"klprci-evt-number"; // INT_T

	/*!
		“ип событи€ о разрыве/установлени€ соединени€ прокси с компонентой.

		c_EventConnectionName - им€ удаленной компоненты с соединение с который было 
			разорванное/установленно
	*/
	const wchar_t c_EventRemoteConnectionName[]	= L"KLPRCP_EVENT_CONNECTION_NAME";

	const wchar_t c_EventConnectionBroken[] = L"KLPRCP_CONNECTION_BROKEN";
	const wchar_t c_EventConnectionEstablished[] = L"KLPRCP_CONNECTION_ESTABLISHED";
}

#endif // KLPRCI_PRCP_CONST_H
