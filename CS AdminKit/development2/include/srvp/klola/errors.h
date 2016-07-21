#ifndef __KLOLA_ERRORS_H__
#define __KLOLA_ERRORS_H__

#include "std/err/errintervals.h"
#include "std/err/error.h"

namespace KLOLA
{
    enum ErrorCodes {
        KLOLA_ERR_HTTP = KLOLASTART,
        KLOLA_ERR_SERVER,
        KLOLA_ERR_LOGIC,
        KLOLA_ERR_PROXY_AUTH_REQUIRED,
        KLOLA_ERR_UNZIP_ERROR
    };


    const struct KLERR::ErrorDescription c_errorDescriptions [] = {
        {KLOLA_ERR_HTTP,	L"Online activation: http error %u"}, 
        {KLOLA_ERR_SERVER,	L"Online activation: activation server error %u"},
		{KLOLA_ERR_LOGIC,	L"Online activation: client error %u"},
        {KLOLA_ERR_PROXY_AUTH_REQUIRED, L"Online activation: proxy authentication is required"},
        {KLOLA_ERR_UNZIP_ERROR, L"Online activation: unzip error %u"},
        {0,0}
    };
};

#endif //__KLOLA_ERRORS_H__
