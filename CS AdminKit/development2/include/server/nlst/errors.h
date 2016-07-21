/*!
 * (C) 2005 "Kaspersky Lab"
 *
 * \file errors.h
 * \author Дамир Шияфетдинов
 * \date 2005
 * \brief Описание кодов ошибок для модуля KLNLST.
 *
 */


#ifndef KLNLST_ERRORS_H
#define KLNLST_ERRORS_H

#include <std/err/errintervals.h>
#include <std/err/error.h>

namespace KLNLST {

	const wchar_t KLNLST_ModuleName[] = L"NLST";

	#define KLNLST_THROW_ERROR( _code ) \
			KLERR_throwError(KLNLST_ModuleName, _code, __FILE__, __LINE__, NULL, NULL)

	#define KLNLST_THROW_ERROR1( _code, _param1 ) \
			KLERR_throwError(KLNLST_ModuleName, _code, __FILE__, __LINE__, NULL, _param1)

	#define KLNLST_THROW_ERROR2( _code, _param1, _param2 ) \
			KLERR_throwError(KLNLST_ModuleName, _code, __FILE__, __LINE__, NULL, _param1, _param2)

	#define KLNLST_THROW_ERROR3( _code, _param1, _param2, _param3 ) \
			KLERR_throwError(KLNLST_ModuleName, _code, __FILE__, __LINE__, NULL, _param1, _param2, _param3 )

    //! Перечисление кодов ошибок модуля SPL. 
    enum Errors {
        NLSTERR_NONE = KLNLSTSTART+0,
		NLSTERR_WRONG_ARGS,
		NLSTERR_WRONG_LIST_NAME,
		NLSTERR_WRONG_NAGENT_ID,
		NLSTERR_WRONG_ITEM_ID
    }; 

	//! Описание ошибок модуля TR	
	const KLERR::ErrorDescription c_errorDescriptions[]={		
		{NLSTERR_NONE,				L"No error"},
		{NLSTERR_WRONG_ARGS,		L"Wrong arguments of function.'"},
		{NLSTERR_WRONG_LIST_NAME,	L"Wrong name of the list - '%ls.'"},
		{NLSTERR_WRONG_NAGENT_ID,	L"Wrong identificator of the nagent - '%ls.'"},
		{NLSTERR_WRONG_ITEM_ID,		L"Wrong identificator of the list's item.'"}
		
	};


} // namespace KLNLST

#endif // KLNLST_ERRORS_H


