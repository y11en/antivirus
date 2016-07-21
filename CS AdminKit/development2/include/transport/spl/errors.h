/*!
 * (C) 2005 "Kaspersky Lab"
 *
 * \file errors.h
 * \author Дамир Шияфетдинов
 * \date 2005
 * \brief Описание кодов ошибок для модуля KLSPL.
 *
 */


#ifndef KLSPL_ERRORS_H
#define KLSPL_ERRORS_H

#include <std/err/errintervals.h>
#include <std/err/error.h>

namespace KLSPL {

	const wchar_t KLSPL_ModuleName[] = L"SPL";

	#define KLSPL_THROW_ERROR( _code ) \
			KLERR_throwError(KLSPL::KLSPL_ModuleName, KLSPL::_code, __FILE__, __LINE__, NULL, NULL)

	#define KLSPL_THROW_ERROR1( _code, _param1 ) \
			KLERR_throwError(KLSPL::KLSPL_ModuleName, KLSPL::_code, __FILE__, __LINE__, NULL, _param1)

	#define KLSPL_THROW_ERROR2( _code, _param1, _param2 ) \
			KLERR_throwError(KLSPL::KLSPL_ModuleName, KLSPL::_code, __FILE__, __LINE__, NULL, _param1, _param2)

	#define KLSPL_THROW_ERROR3( _code, _param1, _param2, _param3 ) \
			KLERR_throwError(KLSPL::KLSPL_ModuleName, KLSPL::_code, __FILE__, __LINE__, NULL, _param1, _param2, _param3 )

    //! Перечисление кодов ошибок модуля SPL. 
    enum Errors {
        SPLERR_NONE = KLSPLSTART+0,
		SPLERR_WRONG_PERMISSIONS_FORMAT,
		SPLERR_POLICY_STORAGE_ERROR,
		SPLERR_WRONG_PERMISSION_NAME,
		SPLERR_ERROR_CHECK_PERMISSION,
		SPLERR_ERROR_GROUP_CONTROL_NOT_INITIALIZED,
		SPLERR_ERROR_CANT_FOUND_NAGENT_GROUP,
		SPLERR_USER_WILL_LOSE_WRITE_RIGHTS
    }; 

	//! Описание ошибок модуля TR	
	const KLERR::ErrorDescription c_errorDescriptions[]={		
		{SPLERR_NONE,						L"No error"},
		{SPLERR_WRONG_PERMISSIONS_FORMAT,	L"Wrong permissions format for group - '%d'."},
		{SPLERR_POLICY_STORAGE_ERROR,		L"Error in policy storage. Error code - '%d'."},
		{SPLERR_WRONG_PERMISSION_NAME,		L"Wrong name of user name or user group ('%ls')."},
		{SPLERR_ERROR_CHECK_PERMISSION,		L"Error in check permissions."},
		{SPLERR_ERROR_GROUP_CONTROL_NOT_INITIALIZED,	L"Error group control not initialized."},
		{SPLERR_ERROR_CANT_FOUND_NAGENT_GROUP,	L"Nagent's group wasn't found. Nagent Id - '%ls'."},
		{SPLERR_USER_WILL_LOSE_WRITE_RIGHTS,	L"The current user will lose access rights for a specefied group."}		
		
	};


} // namespace KLSPL

#endif // KLSPL_ERRORS_H


