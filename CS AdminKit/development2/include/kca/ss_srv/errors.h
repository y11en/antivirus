/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	errors.h
 * \author	Andrew Kazachkov
 * \date	25.09.2003 12:21:45
 * \brief	Errors for ss server module
 * 
 */

#ifndef __KLSS_SRV_ERRORS_H__
#define __KLSS_SRV_ERRORS_H__

#include <std/err/errintervals.h>

namespace KLSSS
{    
    enum ErrorCodes {
        KLSSS_SERVER_NOT_FOUND = KLPRSSSRVSTART + 1,
        KLSSS_SERVER_EXISTS,
        KLSSS_SERVER_UNAVAILABLE
    };

    //! Перечисление кодов ошибок модуля 
    const struct KLERR::ErrorDescription c_errorDescriptions [] = {
        {KLSSS_SERVER_NOT_FOUND,	L"Settings storage server '%ls' not found."},
        {KLSSS_SERVER_EXISTS,       L"Settings storage server '%ls' already exists"},
        {KLSSS_SERVER_UNAVAILABLE,  L"Settings storage server '%ls' is unavailable ('%ls')"},
		{0,0}
    };

    #define KLSSS_THROW_SERVER_NOT_FOUND(_wstrName)     \
        KLERR_MYTHROW1(KLSSS::KLSSS_SERVER_NOT_FOUND, _wstrName.c_str());

    #define KLSSS_THROW_SERVER_EXISTS(_wstrName)     \
        KLERR_MYTHROW1(KLSSS::KLSSS_SERVER_EXISTS, _wstrName.c_str());

    #define KLSSS_THROW_SERVER_UNAVAILABLE(_wstrName, _szwReason)   \
        KLERR_MYTHROW2( \
                        KLSSS::KLSSS_SERVER_UNAVAILABLE,    \
                        _wstrName.c_str(),  \
                        ((_szwReason)?(_szwReason):L""));
};

#endif //__KLSS_SRV_ERRORS_H__
