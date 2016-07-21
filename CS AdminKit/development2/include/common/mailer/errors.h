/*!
 * (C) 2005 Kaspersky Lab 
 * 
 * \file	errors.h
 * \author	Andrey Bryksin
 * \date	7.06.2005 15:29:11
 * \brief	Ошибки модуля KLMAIL
 * 
 */

#ifndef __MAILER_ERRORS_H__
#define __MAILER_ERRORS_H__

#include "std/err/errintervals.h"
#include "std/err/error.h"

namespace KLMAIL {

    enum ErrorCodes {
        ERR_CREATE_SOCKET = KLMAILSTART + 1,
		ERR_SMTP_SERVER_ADDRESS_EMPTY,
		ERR_CONNECT_TO_SMTP_SERVER,
		ERR_SENDING_HELLO,
		ERR_AUTHENTICATION_FAILED,
		ERR_SENDING_FROM,
		ERR_SENDING_RCPT,
		ERR_SENDING_DATA,
		ERR_SENDING_BODY,
		ERR_UNEXPECTED_SMTP_SERVER_WELCOME_MESSAGE,
		ERR_INVALID_ATTACHMENT,
		ERR_INVALID_ARGUMENTS
    };

    //! Перечисление кодов ошибок модуля 
    const struct KLERR::ErrorDescription c_errorDescriptions [] = {
        {ERR_CREATE_SOCKET,					L"Failed to create client socket."},
		{ERR_SMTP_SERVER_ADDRESS_EMPTY,		L"Empty SMTP server address"},
        {ERR_CONNECT_TO_SMTP_SERVER,		L"Could not connect to the SMTP serve."},
        {ERR_SENDING_HELLO,					L"An unexpected error occurred while sending the HELO command."},
        {ERR_AUTHENTICATION_FAILED,			L"ESMTP Authentication failed."},
        {ERR_SENDING_FROM,					L"An unexpected error occurred while sending the FROM command."},
		{ERR_SENDING_RCPT,					L"Failed in call to send RCPT command"},
		{ERR_SENDING_DATA,					L"Failed in call to send DATA command"},
		{ERR_SENDING_BODY,					L"Failed in call to send the body."},
        {ERR_UNEXPECTED_SMTP_SERVER_WELCOME_MESSAGE, L"An unexpected SMTP login response was received."},
		{ERR_INVALID_ATTACHMENT,			L"Invalid message attachment."},
		{ERR_INVALID_ARGUMENTS,				L"Invalid message arguments."},
		{0,0}
    };
}


#endif //__MAILER_ERRORS_H__

// Local Variables:
// mode: C++
// End:
