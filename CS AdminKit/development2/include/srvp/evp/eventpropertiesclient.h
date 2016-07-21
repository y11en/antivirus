/*!
 * (C) 2002 Kaspersky Lab 
 * 
 * \file	EventPropertiesClient.h
 * \author	Mikhail Karmazine
 * \date	20:38 29.11.2002
 * \brief	заголовочный файл, включаемый клиентами EventProperties
 * 
 */

#ifndef __EVENTPROPERTIESCLIENT_H_02468007_3A25_47a9_911C_302036B1C275__
#define __EVENTPROPERTIESCLIENT_H_02468007_3A25_47a9_911C_302036B1C275__

#include <std/base/kldefs.h>
#include <srvp/evp/eventproperties.h>
#include <srvp/evp/eventseverities.h>
#include <srvp/csp/klcsp.h>

namespace KLEVP
{
    class KLSTD_NOVTABLE EventPropertiesFactory : public KLSTD::KLBaseQI
    {
    public:
        virtual void CreateEventPropertiesProxy(
            const std::vector<std::wstring>&            vect_fields,
	        const std::vector<KLCSP::field_order_t>&    vect_fields_to_order,
	        long                                        lifetime,
	        KLEVP::EventProperties**                    ppEventProperties) = 0;

        virtual void CreateEventPropertiesProxy(
	        const KLPAR::Params*                        pParamsIteratorDescr, //! ѕараметры фильтрации событий
            const std::vector<std::wstring>&            vect_fields,
	        const std::vector<KLCSP::field_order_t>&    vect_fields_to_order,
	        long                                        lifetime,
	        KLEVP::EventProperties**                    ppEventProperties) = 0;

        virtual void CreateEventPropertiesProxyForHost(
            const std::wstring&                         wstrDomainName,
            const std::wstring&                         wstrHostName,
            const std::wstring&                         wstrProduct,
            const std::wstring&                         wstrVersion,
            const std::vector<std::wstring>&            vect_fields,
	        const std::vector<KLCSP::field_order_t>&    vect_fields_to_order,
	        long                                        lifetime,
	        KLEVP::EventProperties**                    ppEventProperties) = 0;

        virtual void CreateEventPropertiesProxyForHost(
            const std::wstring&                         wstrDomainName,
            const std::wstring&                         wstrHostName,
            const std::wstring&                         wstrProduct,
            const std::wstring&                         wstrVersion,
	        const KLPAR::Params*                        pParamsIteratorDescr, //! ѕараметры фильтрации событий
            const std::vector<std::wstring>&            vect_fields,
	        const std::vector<KLCSP::field_order_t>&    vect_fields_to_order,
	        long                                        lifetime,
	        KLEVP::EventProperties**                    ppEventProperties) = 0;
    };
};
/*!
  \brief —оздает proxy-объект класса KLEVP::EventProperties

    \param wstrURL      [in] адрес soap-сервера
    \param ppEventProperties   [out] ”казатель на объект класса KLEVP::EventProperties
*/
/*
KLCSC_DECL void KLEVP_CreateEventPropertiesProxy(
	const std::wstring&	wstrLocalComponentName,
	const std::wstring&	wstrConnName,
    const std::vector<std::wstring>& vect_fields,
	const std::vector<KLCSP::field_order_t>& vect_fields_to_order,
	long  lifetime,
	KLEVP::EventProperties** ppEventProperties );

KLCSC_DECL void KLEVP_CreateEventPropertiesProxy(
	const wchar_t*			szwAddress,
    const std::vector<std::wstring>& vect_fields,
	const std::vector<KLCSP::field_order_t>& vect_fields_to_order,
	long  lifetime,
	KLEVP::EventProperties** ppEventProperties,
    const unsigned short*	pPorts = NULL );

KLCSC_DECL void KLEVP_CreateEventPropertiesProxy(
	const wchar_t*			szwAddress,
	const KLPAR::Params* pParamsIteratorDescr, //! ѕараметры фильтрации событий
    const std::vector<std::wstring>& vect_fields,
	const std::vector<KLCSP::field_order_t>& vect_fields_to_order,
	long  lifetime,
	KLEVP::EventProperties** ppEventProperties,
    const unsigned short*	pPorts = NULL );

KLCSC_DECL void KLEVP_CreateEventPropertiesProxyForHost(
	const std::wstring&	wstrLocalComponentName,
	const std::wstring&	wstrConnName,
    const std::wstring&	wstrDomainName,
    const std::wstring&	wstrHostName,
    const std::wstring&	wstrProduct,
    const std::wstring&	wstrVersion,
    const std::vector<std::wstring>& vect_fields,
	const std::vector<KLCSP::field_order_t>& vect_fields_to_order,
	long  lifetime,
	KLEVP::EventProperties** ppEventProperties,
    KLSTD::CAutoPtr<KLSTD::KLBase> pObjectToKeep = NULL );

KLCSC_DECL void KLEVP_CreateEventPropertiesProxyForHost(
	const std::wstring&	wstrLocalComponentName,
	const std::wstring&	wstrConnName,
    const std::wstring&	wstrDomainName,
    const std::wstring&	wstrHostName,
    const std::wstring&	wstrProduct,
    const std::wstring&	wstrVersion,
	const KLPAR::Params* pParamsIteratorDescr, //! ѕараметры фильтрации событий
    const std::vector<std::wstring>& vect_fields,
	const std::vector<KLCSP::field_order_t>& vect_fields_to_order,
	long  lifetime,
	KLEVP::EventProperties** ppEventProperties,
    KLSTD::CAutoPtr<KLSTD::KLBase> pObjectToKeep = NULL );

KLCSC_DECL void KLEVP_CreateEventPropertiesProxyForHost(
	const wchar_t*			szwAddress,
    const std::wstring&	wstrDomainName,
    const std::wstring&	wstrHostName,
    const std::wstring&	wstrProduct,
    const std::wstring&	wstrVersion,
    const std::vector<std::wstring>& vect_fields,
	const std::vector<KLCSP::field_order_t>& vect_fields_to_order,
	long  lifetime,
	KLEVP::EventProperties** ppEventProperties,
    const unsigned short*	pPorts = NULL );

KLCSC_DECL void KLEVP_CreateEventPropertiesProxyForHost(
	const wchar_t*			szwAddress,
    const std::wstring&	wstrDomainName,
    const std::wstring&	wstrHostName,
    const std::wstring&	wstrProduct,
    const std::wstring&	wstrVersion,
	const KLPAR::Params* pParamsIteratorDescr, //! ѕараметры фильтрации событий
    const std::vector<std::wstring>& vect_fields,
	const std::vector<KLCSP::field_order_t>& vect_fields_to_order,
	long  lifetime,
	KLEVP::EventProperties** ppEventProperties,
    const unsigned short*	pPorts = NULL );
*/

/* дл€ локального доступа к хранилищу событий дл€ продукта на хосте(создаетс€ экземпл€р EventStorageServer)
	\param  vectSeverities - фильтр по важности событий (см. c_NFXXXXDescr)
*/
KLCSSRVP_DECL void KLEVP_CreateEventPropertiesProxyLocal(
    const std::wstring&	wstrProductName,
    const std::wstring&	wstrProductVersion,
    const std::vector<std::wstring>& vect_fields,
	const std::vector<KLCSP::field_order_t>& vect_fields_to_order,
	KLEVP::EventProperties** ppEventProperties,
	const std::vector<std::wstring> & vectSeverities );

/* дл€ локального доступа к хранилищу событий дл€ задачи на хосте(создаетс€ экземпл€р EventStorageServer)
	\param  vectSeverities - фильтр по важности событий (см. c_NFXXXXDescr)
*/
KLCSSRVP_DECL void KLEVP_CreateEventPropertiesProxyForTaskLocal(
	const std::wstring& wstrTaskID,
	const std::wstring& wstrProductName,
	const std::wstring& wstrProductVersion,
	const std::vector<std::wstring>& vect_fields,
	const std::vector<KLCSP::field_order_t>& vect_fields_to_order,
	KLEVP::EventProperties** ppEventProperties,
	const std::vector<std::wstring> & vectSeverities);

#endif //__EVENTPROPERTIESCLIENT_H_02468007_3A25_47a9_911C_302036B1C275__

// Local Variables:
// mode: C++
// End:

