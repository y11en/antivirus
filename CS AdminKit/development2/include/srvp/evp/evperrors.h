/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file EVPErrors.h
 * \author Михаил Кармазин
 * \date 2002
 * \brief Описание кодов ошибок для event processing
 *
 */

#ifndef __EVPSERRORS_H_BF3A8DBE_B012_44ea_98DB_2B83C714D93A__
#define __EVPSERRORS_H_BF3A8DBE_B012_44ea_98DB_2B83C714D93A__

#include "std/err/errintervals.h"
#include "std/err/error.h"

namespace KLEVP {

    enum ErrorCodes {
        ERR_BYREF_VALUE_IN_ADO_FIELD1 = KLEVPERRSTART + 1,
		ERR_ARRAY_VALUE_IN_ADO_FIELD1,
		ERR_UNSUPPORTED_TYPE_OF_VALUE_IN_ADO_FIELD2,
		ERR_CANNOT_CONVERT_DATE_VALUE2,
		ERR_SERVER_NOT_AVAILABLE0,
		ERR_PARAMS_TO_BIG_TO_SQL_FIELD1,
		ERR_NET_SEND3,
        ERR_EVENT_TYPE_EMPTY2,
        ERR_DELETION_FOR_ITERATOR_IN_PROGRESS1,
        ERR_SELECTION_IS_OBSOLETE1,
        ERR_DELETION_ABORT,
        ERR_NO_SUCH_DELETION1
    };

    //! Перечисление кодов ошибок модуля 
    const struct KLERR::ErrorDescription c_errorDescriptions [] = {
        {ERR_BYREF_VALUE_IN_ADO_FIELD1,		L"Field \"%s\" contains \"VT_BYREF\" value, which is not supported."},
        {ERR_ARRAY_VALUE_IN_ADO_FIELD1,		L"Field \"%s\" is variant array, which is not supported."},
		{ERR_UNSUPPORTED_TYPE_OF_VALUE_IN_ADO_FIELD2, L"Unsupported type of value (%d) in ADO field \"%s\"."},
		{ERR_CANNOT_CONVERT_DATE_VALUE2, L"Field \"%s\" contains unsupportable value %s."},
		{ERR_SERVER_NOT_AVAILABLE0, L"Server not available."},
		{ERR_PARAMS_TO_BIG_TO_SQL_FIELD1, L"Params cannot be stored in SQL field because serialized string is too long (%d)"},
		{ERR_NET_SEND3, L"Cannot execute \"net send\" - error %d. Tryed to send to \"%s\" with message \"%s\"."},
        {ERR_EVENT_TYPE_EMPTY2, L"Event type cnnot be empty in notification rule (product name \"%s\", product version \"%s\")."},
        {ERR_DELETION_FOR_ITERATOR_IN_PROGRESS1, L"Deletion for iterator %ls is in progress." },
        {ERR_SELECTION_IS_OBSOLETE1, L"Cannot perform bulk delete for iterator %ls because it's obsolete"},
        {ERR_DELETION_ABORT, L"Deletion aborted."},
        {ERR_NO_SUCH_DELETION1, L"Iterator id %ls not found - nothing to cancel" },
		{0,0}
    };
}



#endif // __EVPSERRORS_H_BF3A8DBE_B012_44ea_98DB_2B83C714D93A__

// Local Variables:
// mode: C++
// End:
