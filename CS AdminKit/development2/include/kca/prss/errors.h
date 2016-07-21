/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file PRSS/Errors.h
 * \author Андрей Казачков
 * \date 2002
 * \brief Описание кодов ошибок, возвращаемых модулем Product Settings Storage (PRSS).
 *
 */


#ifndef KLPRSS_ERRORS_H
#define KLPRSS_ERRORS_H

#include "std/err/errintervals.h"
#include "std/err/error.h"

namespace KLPRSS {

    enum ErrorCodes {
        ERR_READ = KLPRSSERRSTART + 1,
        ERR_WRITE,
		ERR_UNEXPECTED_TYPE,
        ALREADY_EXIST,
        NOT_EXIST,
        CANT_OPEN_STORAGE,
		ERR_UNDEFINED,
		ERR_BADINSTALL,
		ERR_NOTREGISTERED,
		ERR_BADREGISTRATION
    };


    //! Перечисление кодов ошибок модуля Product Settings Storage. 
    //  Любой из методов модуля может вернуть исключение с этим кодом ошибки.
    const struct KLERR::ErrorDescription c_errorDescriptions [] = {
        {ERR_READ,				L"Error reading from the storage \"%ls\"."}, 
        {ERR_WRITE,				L"Error writing to storage \"%ls\"."},
		{ERR_UNEXPECTED_TYPE,	L"Value \"%ls\" has unexpected type."},
        {ALREADY_EXIST,			L"Parameter with name \"%ls\" already exists."},
        {NOT_EXIST,				L"Parameter with name \"%ls\" not exist."},
        {CANT_OPEN_STORAGE,		L"Can't open settings storage \"%ls\"."},
		{ERR_UNDEFINED,			L"The product was not installed correctly: component storage location is undefined."},
		{ERR_BADINSTALL,		L"The product was not installed correctly."},
		{ERR_NOTREGISTERED,		L"Component \"%ls\" is not registered."},
		{ERR_BADREGISTRATION,	L"Component \"%ls\" was not installed correctly."},
        {0,0}
    };

}

#define KLPRSS_MAYEXIST(_expr)						\
	KLERR_TRY										\
	_expr;											\
	KLERR_CATCH(pError)								\
	if(pError->GetId() != KLPRSS::ALREADY_EXIST)	\
					KLERR_RETHROW();				\
KLERR_ENDTRY


#define KLPRSS_MAYNOTEXIST(_expr)					    \
	KLERR_TRY                                           \
        _expr;                                          \
	KLERR_CATCH(pError)                                 \
		if(                                             \
				pError->GetId() != KLPRSS::NOT_EXIST &&   \
				pError->GetId() != KLSTD::STDE_NOENT &&   \
				pError->GetId() != KLSTD::STDE_NOTFOUND)  \
		{                                               \
			KLERR_RETHROW();                            \
		};                                              \
	KLERR_ENDTRY


#endif // KLPRSS_ERRORS_H
