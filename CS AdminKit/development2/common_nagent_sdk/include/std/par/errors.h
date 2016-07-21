/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file Errors.h
 * \author Андрей Казачков
 * \date 2002
 * \brief Описание кодов ошибок, возвращаемых модулем Params.
 *
 */


#ifndef KLPRM_ERRORS_H
#define KLPRM_ERRORS_H

#include "../err/errintervals.h"
#include "../err/error.h"

namespace KLPAR {

    enum ErrorCodes {
        WRONG_PARAM_NAME =  KLPARERRSTART+ 1,
        WRONG_VALUE_TYPE,
        ALREADY_EXIST,
        NOT_EXIST,
        WRONG_VALUE,
        WRONG_BUFFER,
        WRONG_DATE_VALUE,
        WRONG_FREE_BUFFER_CALLBACK,
        WRONG_PARAMS_VALUE,
        WRONG_ARRAY_VALUE
    };


    //! Перечисление кодов ошибок модуля Params. 
    //  Любой из методов модуля может вернуть исключение с этим кодом ошибки.
    const struct KLERR::ErrorDescription c_errorDescriptions [] = {
        {WRONG_PARAM_NAME, L"Wrong name of parameter \"%ls\""}, 
        {WRONG_VALUE_TYPE, L"Wrong type of parameter \"%ls\" value"},
        {ALREADY_EXIST, L"Parameter with name \"%ls\" already exists"},
        {NOT_EXIST, L"Parameter with name \"%ls\" does not exist"},
        {WRONG_VALUE, L"Wrong value"},
        {WRONG_BUFFER, L"Wrong buffer for BinaryValue"},
        {WRONG_DATE_VALUE, L"Wrong date value \"%ls\""},
        {WRONG_FREE_BUFFER_CALLBACK, L"Wrong callback to free buffer"},
        {WRONG_PARAMS_VALUE, L"Wrong params value"},
        {WRONG_ARRAY_VALUE, L"Wrong array value"},
        {0,0}
    };

}

#endif // KL_PRM_ERRORS_H
