/*!
 * (C) 2005 "Kaspersky Lab"
 *
 * \file errors.h
 * \author Дамир Шияфетдинов
 * \date 2005
 * \brief Описание кодов ошибок для модуля Network.
 *
 */


#ifndef KLNET_ERRORS_H
#define KLNET_ERRORS_H

#include <std/err/errintervals.h>
#include <std/err/error.h>

namespace KLNET {

	const wchar_t KLNET_ModuleName[] = L"NET";

	#define KLNET_THROW_ERROR( _code ) \
			KLERR_throwError(KLNET_ModuleName, _code, __FILE__, __LINE__, NULL, NULL)

	#define KLNET_THROW_ERROR1( _code, _param1 ) \
			KLERR_throwError(KLNET_ModuleName, _code, __FILE__, __LINE__, NULL, _param1)

	#define KLNET_THROW_ERROR2( _code, _param1, _param2 ) \
			KLERR_throwError(KLNET_ModuleName, _code, __FILE__, __LINE__, NULL, _param1, _param2)

    //! Перечисление кодов ошибок модуля TR. 
    enum Errors {
        NETERR_NONE = KLNETSTART+0, 
		NETERR_INVALID_PARAMETER,			/*!< Неверные параметры функции */
		NETERR_WRONG_ADDR,					/*!< Неверный сетевой адрес */
		NETERR_ERR_START_RDP_APPS			/*!< Ошибка запуска приложения создания RDP соединения */
    }; 

	//! Описание ошибок модуля TR	
	const KLERR::ErrorDescription c_errorDescriptions[]={
		
		{NETERR_NONE,						L"No error"},
		{NETERR_INVALID_PARAMETER,			L"Invalid function's parameter"},	
		{NETERR_WRONG_ADDR,					L"Wrong transport address. Address - '%ls'."},
		{NETERR_ERR_START_RDP_APPS,			L"Error in process of starting RDP application."},
        {0,0}
	};


} // namespace KLNET

#endif // KLNET_ERRORS_H

// Local Variables:
// mode: C++
// End:

