/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	evp/smtplist.h
 * \author	Mikhail Karmazine
 * \date	19.03.2003 18:16:47
 * \brief	обертка для KLMAILER
 * 
 */

#include <std/base/klbase.h>
#include <std/trc/trace.h>
#include <std/err/error.h>
//#pragma message ("Andrey, please move mailer from nagent to somewere else!")
#include <common/mailer/mailer.h>
#include <map>

#ifndef __SMTPLIST_H__
#define __SMTPLIST_H__

#ifdef __unix__
 #define _wcsicmp wcscasecmp 
 #include <wctype.h>
#endif

namespace KLSRV
{
    /* ! brief - держит список проинициализированных объектов 
        типа KLMAIL::Mailer, по одному для каждой пары сервер/порт
        Класс НЕ thread-safe !!!!!
    */

    class SMTPServers
    {
    public:
        ~SMTPServers();

        void SendMail(
            const std::wstring& wstrSMTPServer,
            long nSMTPPort,
            const std::wstring& wstrEmail,
            const std::wstring& wstrFrom,
            const std::wstring& wstrSubject,
            const std::wstring& wstrDetailedInfo,
			const std::wstring& wstrAttachment = L"",
			const std::wstring& wstrESMTPUserName = L"",
			const std::wstring& wstrESMTPUserPassword = L"",
			bool bSendMailAsinc = true,
			bool bThrow = false);

    private:
        struct SMTPServerDescr
        {
            std::wstring wstrSMTPServer;
            long nSMTPPort;
        };

        struct SMTPServerDescr_Less : public std::binary_function <SMTPServerDescr, SMTPServerDescr, bool> 
        {
            bool operator()( const SMTPServerDescr& _Left, const SMTPServerDescr& _Right ) const
            {
                int nCmp = _wcsicmp( _Left.wstrSMTPServer.c_str(), _Right.wstrSMTPServer.c_str() );
                if( nCmp == 0 )
                {
                    nCmp = _Left.nSMTPPort - _Right.nSMTPPort;
                }

                return nCmp < 0;
            }
        };

        typedef std::map<SMTPServerDescr, KLSTD::CAutoPtr<KLMAIL::Mailer>, SMTPServerDescr_Less> SMTPServerMap;

        SMTPServerMap m_smtpServerMap;
    };
}

#endif //__SMTPLIST_H__

// Local Variables:
// mode: C++
// End:

