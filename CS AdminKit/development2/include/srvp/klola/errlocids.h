#ifndef __KLOLA_ERRLOCIDS_H__
#define __KLOLA_ERRLOCIDS_H__

namespace KLOLA
{
	enum
    {
		// general errors        
        KLOLAEL_ERR_HTTP_ERROR,
        KLOLAEL_ERR_UNKNOWN_SERVER_ERROR,
        KLOLAEL_ERR_INTERNAL_SERVER_ERROR,
        KLOLAEL_ERR_KEY_NOT_FOUND,
        KLOLAEL_ERR_KEY_OR_ACTIVATION_CODE_BLOCKED,
        KLOLAEL_ERR_MANDATORY_PARAMETERS_MISSING,
        KLOLAEL_ERR_WRONG_CUSTOMER_ID_OR_PASSWORD,
        KLOLAEL_ERR_WRONG_ACTIVATION_CODE,
        KLOLAEL_ERR_WRONG_APPLICATION_ID,
        KLOLAEL_ERR_ACTIVATION_CODE_REQUIRED,
        KLOLAEL_ERR_USE_MOBILE_ACTIVATION,
        KLOLAEL_ERR_ACTIVATION_PERIOD_EXPIRED,
        KLOLAEL_ERR_ACTIVATION_ATTEMPTS_EXHAUSTED,
        KLOLAEL_ERR_UNEXPECTED_ERROR,
        KLOLAEL_ERR_INVALID_PARAMETER,
        KLOLAEL_ERR_INVALID_ACTIVATION_CODE,
        KLOLAEL_ERR_INVALID_CUSTOMER_ID,
        KLOLAEL_ERR_INVALID_CUSTOMER_PASSWORD,
        KLOLAEL_ERR_UNKNOWN_SERVER_RESPONSE,
        KLOLAEL_ERR_OPERATION_ABORTED,
        KLOLAEL_ERR_UNZIP_ERROR,
        KLOLAEL_ERR_UNEXPECTED_ARCHIVE_CONTENTS
	};

	const KLERR::ErrorDescription c_LocErrDescriptions[] = 
    {
        {KLOLAEL_ERR_HTTP_ERROR,                        L"Online activation: http error %1 occured."},
        {KLOLAEL_ERR_UNKNOWN_SERVER_ERROR,              L"Online activation: unknown server error occured."},
        {KLOLAEL_ERR_INTERNAL_SERVER_ERROR,             L"Online activation: internal server error occurred."},
        {KLOLAEL_ERR_KEY_NOT_FOUND,                     L"Online activation: there is no key for supplied activation code"},
        {KLOLAEL_ERR_KEY_OR_ACTIVATION_CODE_BLOCKED,    L"Online activation: key or activation code is blocked"},
        {KLOLAEL_ERR_MANDATORY_PARAMETERS_MISSING,      L"Online activation: mandatory request parameters are missing"},
        {KLOLAEL_ERR_WRONG_CUSTOMER_ID_OR_PASSWORD,     L"Online activation: wrong customer id or password"},
        {KLOLAEL_ERR_WRONG_ACTIVATION_CODE,             L"Online activation: wrong activation code"},
        {KLOLAEL_ERR_WRONG_APPLICATION_ID,              L"Online activation: supplied application id is not listed on the server"},
        {KLOLAEL_ERR_ACTIVATION_CODE_REQUIRED,          L"Online activation: client must supply activation code"},
        {KLOLAEL_ERR_USE_MOBILE_ACTIVATION,             L"Online activation: client must use mobile activation protocol for supplied activation code"},
        {KLOLAEL_ERR_ACTIVATION_PERIOD_EXPIRED,         L"Online activation: activation period expired"},
        {KLOLAEL_ERR_ACTIVATION_ATTEMPTS_EXHAUSTED,     L"Online activation: quantity of activation attempts exhausted"},
        {KLOLAEL_ERR_UNEXPECTED_ERROR,                  L"Online activation: unexpected error"},
        {KLOLAEL_ERR_INVALID_PARAMETER,                 L"Online activation: invalid parameter supplied"},
        {KLOLAEL_ERR_INVALID_ACTIVATION_CODE,           L"Online activation: activation code is invalid"},
        {KLOLAEL_ERR_INVALID_CUSTOMER_ID,               L"Online activation: customer identifier is invalid"},
        {KLOLAEL_ERR_INVALID_CUSTOMER_PASSWORD,         L"Online activation: customer password is invalid"},
        {KLOLAEL_ERR_UNKNOWN_SERVER_RESPONSE,           L"Online activation: activation server returned unknown response"},
        {KLOLAEL_ERR_OPERATION_ABORTED,                 L"Online activation: activation request execution aborted"},
        {KLOLAEL_ERR_UNZIP_ERROR,                       L"Online activation: unzip error %1 while decompressing file '%2' into folder '%3'"},
        {KLOLAEL_ERR_UNEXPECTED_ARCHIVE_CONTENTS,       L"Online activation: contents of archive '%1' is unexpected"}
	};
};


#endif //__KLOLA_ERRLOCIDS_H__
