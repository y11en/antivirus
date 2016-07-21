/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file Common.h
 * \author Дамир Шияфетдинов
 * \date 2002
 * \brief Описание общих определений.
 *
 */


#ifndef KLTR_COMMON_H
#define KLTR_COMMON_H

#include "std/base/klbase.h"
#include "std/err/error.h"

#include "std/tp/threadspool.h"

const int c_gSOAPTimeoutErrCode	= (-101);
const int c_gSOAPTrafficErrCode	= (-102);

#define KLTR_IS_LOCAL_CONNECTION	L"tr-is_local_connection"

namespace KLTRAP {

	const wchar_t	c_InetLocationLocalPrefic[]	= L"http://127.0.0.1";

	const int c_DefaultTransportTimeout = 30000;	// 30 secs

	const int c_Undefined = (-1);

	#define SOCKET_NULL	32000

	class Transport;

	KLCSTR_DECL void GetLocalIp( std::wstring &localIp );

	/*!\brief Функция разбора location'а
		\exception TRERR_WRONG_ADDR
	*/
	KLCSTR_DECL void ParseLocation( const wchar_t *location, int &port, std::wstring &addr,
		bool checkPort );

} // namespace KLTRAP

/*\brief функция инициализация транспортной библиотеки.
*   Данная функций заводит один объект класса Trasnport, который доступен черз функцию
*	KLTR_GetTransport. 
*
/*\brief Функция особождает глобальный объект класса Transport*/
DECLARE_MODULE_INIT_DEINIT2( KLCSTR_DECL, KLTR );

/*\brief функция инициализация транспортной библиотеки в тестовом режиме.*/
DECLARE_MODULE_INIT_DEINIT2( KLCSTR_DECL, KLTRTEST );

/*\brief Возвращает указатель на глобальный объект класса Transport */
KLCSTR_DECL KLTRAP::Transport *KLTR_GetTransport();

#endif // KLTR_COMMON_H

// Local Variables:
// mode: C++
// End:
