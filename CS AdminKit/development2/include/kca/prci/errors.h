/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file PRCI/Errors.h
 * \author Андрей Казачков
 * \date 2002
 * \brief Описание кодов ошибок, возвращаемых модулем Product Component Instance (PRCI).
 *
 */


#ifndef KLPRCI_ERRORS_H
#define KLPRCI_ERRORS_H

#include "std/err/errintervals.h"
#include "std/err/error.h"

namespace KLPRCI {

    enum ErrorCodes {
        ERR_WRONG_CALLBACK = KLPRCIERRSTART + 1,
        ERR_WRONG_PRODUCT_NAME,
        ERR_WRONG_PRODUCT_VERSION,
        ERR_WRONG_COMPONENT_NAME,
        ERR_WRONG_EVENT_TYPE,
        ERR_WRONG_EVENT_FILTER,
        ERR_WRONG_SECTION,
        ERR_WRONG_PARAMETER_NAME,
        ERR_WRONG_VALUE,
		ERR_INSTNAME_EXISTS,
		ERR_NOTREADY,
        ERR_TSTASK_ALREADY_RUNNING,
        ERR_TASK_NOT_FOUND,
        ERR_INSTANCE_UNAVAIL
    };


    //! Перечисление кодов ошибок модуля Product Component Instance. 
    //  Любой из методов модуля может вернуть исключение с этим кодом ошибки.
    const struct KLERR::ErrorDescription c_errorDescriptions [] = {
        {ERR_WRONG_CALLBACK,			L"Wrong callback function"}, 
        {ERR_WRONG_PRODUCT_NAME,		L"Wrong product name \"%ls\""},
        {ERR_WRONG_PRODUCT_VERSION,		L"Wrong product version \"%ls\""},
        {ERR_WRONG_COMPONENT_NAME,		L"Wrong component name \"%ls\""},
        {ERR_WRONG_EVENT_TYPE,			L"Wrong event type \"%ls\""},
        {ERR_WRONG_EVENT_FILTER,		L"Wrong event body filter"},
        {ERR_WRONG_SECTION,				L"Wrong product parameters section \"%ls\""},
        {ERR_WRONG_PARAMETER_NAME,		L"Wrong product parameter name \"%ls\""},
        {ERR_WRONG_VALUE,				L"Wrong parameter value"},
		{ERR_INSTNAME_EXISTS,			L"Component instance with name \"%ls\" already exists"},
		{ERR_NOTREADY,					L"Component instance '%ls'-'%ls'-'%ls'-'%ls' is not ready"},
        {ERR_TSTASK_ALREADY_RUNNING,    L"Task \"%ls\" from taskstorage is already running"},
        {ERR_TASK_NOT_FOUND,            L"Task with taskid %u is not found"},
        {ERR_INSTANCE_UNAVAIL,          L"Component instance '%ls' unavailable"},
        {0,0}
    };

}

#endif // KLPRCI_ERRORS_H
