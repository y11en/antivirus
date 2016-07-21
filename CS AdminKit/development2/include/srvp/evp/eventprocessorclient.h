/*!
 * (C) 2002 Kaspersky Lab 
 * 
 * \file	EventProcessorClient.h
 * \author	Mikhail Karmazine
 * \date	20:38 29.11.2002
 * \brief	заголовочный файл, включаемый клиентами EventProperties
 * 
 */

#ifndef __EVENTPROCESSORCLIENT_H_47057FD5_2927_4ca4_B217_40BB0757FCC8__
#define __EVENTPROCESSORCLIENT_H_47057FD5_2927_4ca4_B217_40BB0757FCC8__

#include <std/base/kldefs.h>
#include <kca/prcp/proxybase.h>
#include <srvp/evp/eventsprocessor.h>

/*!
  \brief —оздает proxy-объект класса KLEVP::EventsProcessor

    \param wstrURL      [in] адрес soap-сервера
    \param ppEventProperties   [out] ”казатель на объект класса KLEVP::ppEventsProcessor
*/
KLCSNAGT_DECL void KLEVP_CreateEventsProcessorProxy(
	const std::wstring&	wstrLocalComponentName,
	const std::wstring&	wstrConnName,
    KLEVP::EventsProcessor** ppEventsProcessor );

KLCSNAGT_DECL void KLEVP_CreateEventsProcessorProxy(
	const std::wstring&         wstrAddress,
    KLEVP::EventsProcessor**    ppEventsProcessor,
    KLPAR::BinaryValue*         pCert,
    KLPAR::Params*              pCredentials,
    bool                        bUseSsl,    
	const int*                  pPorts = NULL,
    KLPRCP::proxy_auth_t*       pProxyHost = NULL,
    bool                        bCompressData = false);


#endif //__EVENTPROCESSORCLIENT_H_47057FD5_2927_4ca4_B217_40BB0757FCC8__

// Local Variables:
// mode: C++
// End:

