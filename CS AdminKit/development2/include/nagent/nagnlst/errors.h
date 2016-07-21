/*!
 * (C) 2005 "Kaspersky Lab"
 *
 * \file errors.h
 * \author Дамир Шияфетдинов
 * \date 2005
 * \brief Описание кодов ошибок для модуля KLNAGNLST.
 *
 */


#ifndef KLNAGNLST_ERRORS_H
#define KLNAGNLST_ERRORS_H

#include <std/err/errintervals.h>
#include <std/err/error.h>

namespace KLNAGNLST {

	const wchar_t KLNAGNLST_ModuleName[] = L"NAGNLST";

	#define KLNAGNLST_THROW_ERROR( _code ) \
			KLERR_throwError(KLNAGNLST::KLNAGNLST_ModuleName, _code, __FILE__, __LINE__, NULL, NULL)

	#define KLNAGNLST_THROW_ERROR1( _code, _param1 ) \
		KLERR_throwError(KLNAGNLST::KLNAGNLST_ModuleName, _code, __FILE__, __LINE__, NULL, _param1)

	#define KLNAGNLST_THROW_ERROR2( _code, _param1, _param2 ) \
			KLERR_throwError(KLNAGNLST::KLNAGNLST_ModuleName, _code, __FILE__, __LINE__, NULL, _param1, _param2)

	#define KLNAGNLST_THROW_ERROR3( _code, _param1, _param2, _param3 ) \
			KLERR_throwError(KLNAGNLST::KLNAGNLST_ModuleName, _code, __FILE__, __LINE__, NULL, _param1, _param2, _param3 )

    //! Перечисление кодов ошибок модуля SPL. 
    enum Errors {
        NAGNLSTERR_NONE = KLNAGNLSTSTART+0,
		NAGNLSTERR_WRONG_ARGS,
		NAGNLSTERR_WRONG_LIST_NAME,
		NAGNLSTERR_SERIALIZE_ERROR,
		NAGNLSTERR_START_TASK_ERROR,
		NAGNLSTERR_CANT_CONNECT_TO_COMPONENT
    }; 

	//! Описание ошибок модуля TR	
	const KLERR::ErrorDescription c_errorDescriptions[]={		
		{NAGNLSTERR_NONE,				L"No error"},
		{NAGNLSTERR_WRONG_ARGS,			L"Wrong arguments of function.'"},
		{NAGNLSTERR_WRONG_LIST_NAME,	L"Wrong name of the list - '%ls.'"},
		{NAGNLSTERR_SERIALIZE_ERROR,	L"Error of process of serializetion/desirializetion.'"},
		{NAGNLSTERR_START_TASK_ERROR,	L"Can't start network list task: list name - '%ls' taks name - '%ls'."},
		{NAGNLSTERR_CANT_CONNECT_TO_COMPONENT,	L"Can't connect to component: component name - '%ls'."}
		
	};


} // namespace KLNLST

#endif // KLNAGNLST_ERRORS_H


