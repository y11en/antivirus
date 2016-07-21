/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	wat/authserver.h
 * \author	Mikhail Karmazine
 * \date	06.02.2003 11:41:59
 * \brief	Интерфейс KLWAT::AuthClient
 * 
 */

#ifndef __KLWAT_AUTHCLIENT_H__
#define __KLWAT_AUTHCLIENT_H__

#include <string>
#include <std/base/kldefs.h>
#include "common.h"
#include <std/par/params.h>

namespace KLWAT
{
   /*!
		\brief Класс, работающий на клиентской стороне аутентификации.
        Пример:

            std::string strInData, strOutData;

            // server id may be any string which is constant during connection 
            // to server - server IP address, for example.
            pAuthClient->InitializeHandshake( L"my server", strOutData ); 

            bool bDone = false;
            std::wstring wstrClientIdAtServer;
            while( ! bDone )
            {
                if( ! strInData.empty() )  // at first loop strInData is empty
                {
                    bDone =
                        pAuthClient->HandshakeWithDataFromServer( L"my server", strInData, wstrClientIdAtServer, strOutData );
                }

                // pServerProxy->ProcessMessage() - placeholder for function which 
                // transmits strOutData to server, and receives back strInData.
                // wstrClientIdAtServer parameter is both input and output - at first call it is empty,
                // and ProcessMessage() assigns client id to it, and then assigned value shall be used in
                // consequent calls.
                pServerProxy->ProcessMessage( wstrClientIdAtServer, strOutData, strInData );
            }

        После такого цикла сервер узнает клиента по строке wstrClientIdAtServer, и
        может провести имперсонацию, получить AccessToken, и т.п.
    */
    class KLSTD_NOVTABLE AuthClient : public KLSTD::KLBase
    {
    public:
        virtual ~AuthClient () {};

        /*!
          \brief	ServerShallKnowClient
                        Проверяет, был ли осущесвлен успешный handshake с данным сервером.
                        Если да, то можно вызывать функцию GetClientIdAtServer()

          \param	wstrServerIdentity - идентификатор сервера.
          \return	bool - true если handshake был осуществлен, false otherwise.
        */
        virtual bool ServerShallKnowClient( const std::wstring & wstrServerIdentity ) = 0;

        virtual void InitializeHandshake( const std::wstring & wstrServerIdentity,
                                          std::string & strOutData ) = 0;

        virtual void InitializeBAHandshake( const std::wstring & wstrServerIdentity,
                                            const KLPAR::Params * pCustomCredentials,
                                            std::string & strOutData ) = 0;
        
        /*!
          \brief	HandshakeWithDataFromServer

          \param	wstrServerIdentity
          \param	strInData
          \param	strOutData  - даже если функция вернула true, содержимое все равно надо
                послать на сервер, если длина strOutData>0
          \return	bool - возвращает true, когда клиент получил все необходимые данные для 
                аутентификации от сервера, т.е. вызывать для этого сервера эту функцию больше
                    не надо. Но если длина strOutData>0, то все равно надо отсылать эту
                    строку на сервер!!!!
        */
        virtual bool HandshakeWithDataFromServer( const std::wstring & wstrServerIdentity,
                                                  const std::string & strInData,
                                                  const std::wstring & wstrClientId,
                                                        std::string & strOutData ) = 0;

        /*!
          \brief	ForgetServer - удаляет сервер из списка известных серверов.

          \param	wstrServerIdentity - идентификатор сервера.
        */
        virtual void ForgetServer( const std::wstring & wstrServerIdentity ) = 0;


        /*!
          \brief	GetClientIdAtServer - возвращает ID, который надо испльзовать
                при общении с сервером, чтобы идентифицировать себя.
                Процесс handshake'а должен быть завершен, иначе будет выброшено исключение.

          \param	const std::wstring & wstrServerIdentity - идентификатор сервера.
          \param	std::wstring & wstrClientId [out] - ID клиента.
        */
        virtual void GetClientIdAtServer( const std::wstring & wstrServerIdentity,
                                          std::wstring & wstrClientId ) = 0;
    };
	const wchar_t c_szwClientCertFieldName[]= L"KLWAT_CLNT_CERT";
}

KLCSTR_DECL void KLWAT_CreateAuthClient( KLWAT::AuthClient **ppAuthClient, bool bMultithreaded = true );

KLCSTR_DECL void KLWAT_GetGlobalAuthClient( KLWAT::AuthClient **ppAuthClient );

#endif //__KLWAT_AUTHCLIENT_H__

// Local Variables:
// mode: C++
// End:
