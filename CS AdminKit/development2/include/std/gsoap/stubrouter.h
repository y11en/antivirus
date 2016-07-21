 /*!
 * (C) 2004 "Kaspersky Lab"
 *
 * \file stubrouter.h
 * \author Дамир Шияфетдинов
 * \date 2004
 * \brief Определения функция для работы с stub функциями gSOAP
 *
 */

#ifndef KLSTD_STUBROUTER_H
#define KLSTD_STUBROUTER_H

#include <string>
#include <map>

#include "std/base/kldefs.h"

//#include "soaph.h"

namespace KLSTRT{

	//!\brief Описание функции
	struct SOAPStubFunc
	{
		const wchar_t	*funcName;	//<- Имя функции
		void			*funcPtr;	//<- Указатель на функцию
	};

	//!< Функции де/инициализации подсистемы
	void InitStubRouter();
	void DeinitStubRouter();
		
	//!\brief Регестрирует gSOAP функции модуля
	KLCSC_DECL void RegisterGSOAPStubFunctions( SOAPStubFunc *array, int array_size );

	//!\brief Удаляет функции из списка зарегестрированных gSOAP функции модуля
	KLCSC_DECL void UnregisterGSOAPStubFunctions( SOAPStubFunc *array, int array_size );

	//!\brief Возвращает указатель зарегестрированной функции
	KLCSC_DECL void *GetFunctionPtrByName( const wchar_t *funcName );

};

KLCSC_DECL int klstd_soap_serve(struct soap *);

#endif // KLSTD_STUBROUTER_H
