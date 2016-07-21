/*!
 * (C) 2005 Kaspersky Lab 
 * 
 * \file	srvhrch\srvhrch_errors.h
 * \author	Andrew Lashenkov
 * \date	12.01.2005 16:41:00
 * \brief	Ошибки модуля KLSRVH
 * 
 */


#ifndef __KLSRVH_SRVHRCH_ERRORS_H__
#define __KLSRVH_SRVHRCH_ERRORS_H__

#include <std/err/errintervals.h>
#include <std/err/error.h>

namespace KLSRVH
{
    enum ErrorCodes {
        ERR_ONESELF_AS_SLAVE = KLSRVHSTART + 1,
        ERR_MASTER_AS_SLAVE
    };

    //! Перечисление кодов ошибок модуля 
    const struct KLERR::ErrorDescription c_errorDescriptions [] =
    {
        {ERR_ONESELF_AS_SLAVE, L"Server cannot be registered as slave for itself."},
		{ERR_MASTER_AS_SLAVE,  L"Master server cannot be registered as slave."},
        {0,0}
    };
}

#endif //__KLSRVH_SRVHRCH_ERRORS_H__
