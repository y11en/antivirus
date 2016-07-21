/*!
 * (C) 2002 Kaspersky Lab 
 * 
 * \file	EventPropertiesProxy.h
 * \author	Mikhail Karmazine
 * \date	20:41 29.11.2002
 * \brief	Заголовок клиентской реализации интерфейса EventProperties
 * 
 */


#ifndef __EVENTPROPERTIESPROXY_H_33B4A959_547C_427b_B386_DA9BCD94763D__
#define __EVENTPROPERTIESPROXY_H_33B4A959_547C_427b_B386_DA9BCD94763D__

#include <kca/prcp/proxybase.h>
#include <srvp/evp/eventproperties.h>
#include <srvp/csp/klcsp.h>

class EventPropertiesProxy :
    public KLSTD::KLBaseImpl<KLEVP::EventProperties>
{
public:
    EventPropertiesProxy(
                    KLPRCP::CProxyBase* pOwnersProxy,
                    KLSTD::KLBase*      pOwner);

	virtual ~EventPropertiesProxy();

    void ResetIterator (
        KLEVP::EventPropertiesType eventPropertiesType,
        const KLPAR::Params* pParamsIteratorDescr,
	    const std::vector<std::wstring>& vect_fields,
	    const std::vector<KLCSP::field_order_t>& vect_fields_to_order,
	    long  lifetime);

    long GetRecordCount();

    void GetRecordRange(
        long nStart,
        long nEnd,
        KLPAR::Params** pParamsEvents,
        KLEVP::RangeCalcProgressCallback fnProgress = NULL,
        void * pCallbackParam = NULL );

    long FindRecordByFirstChars( const std::wstring & wstrFieldToSearch, 
                                 const std::wstring & wstrFirstChars );
    
	void DeleteProperties( const std::vector<long>& vectIDs  );

    std::wstring InitiateDelete( KLSTD::CAutoPtr<KLPAR::Params> pSettings );

    void CancelDelete( KLSTD::CAutoPtr<KLPAR::Params> pSettings );

    std::wstring GetIteratorId( KLSTD::CAutoPtr<KLPAR::Params> pSettings );
private:
	void ReleaseIterator();
	std::wstring m_wstrIteratorId;
    KLPRCP::CProxyBase*             m_pOwnersProxy;
    KLSTD::CAutoPtr<KLSTD::KLBase>  m_pOwner;//we should keep owner to guaranree pointer m_pOwnersProxy to be valid

//	KLSTD::CAutoPtr<KLSTD::CriticalSection>	m_pCS;
public://instantiators
    static void CreateEventPropertiesProxy(
        KLPRCP::CProxyBase*                         pOwnersProxy,
        KLSTD::KLBase*                              pOwner,
        const std::vector<std::wstring>&            vect_fields,
	    const std::vector<KLCSP::field_order_t>&    vect_fields_to_order,
	    long                                        lifetime,
	    KLEVP::EventProperties**                    ppEventProperties);

    static void CreateEventPropertiesProxy(
        KLPRCP::CProxyBase*                         pOwnersProxy,
        KLSTD::KLBase*                              pOwner,
	    const KLPAR::Params*                        pParamsIteratorDescr, //! Параметры фильтрации событий
        const std::vector<std::wstring>&            vect_fields,
	    const std::vector<KLCSP::field_order_t>&    vect_fields_to_order,
	    long                                        lifetime,
	    KLEVP::EventProperties**                    ppEventProperties);

    static void CreateEventPropertiesProxyForHost(
        KLPRCP::CProxyBase*                         pOwnersProxy,
        KLSTD::KLBase*                              pOwner,
        const std::wstring&                         wstrDomainName,
        const std::wstring&                         wstrHostName,
        const std::wstring&                         wstrProduct,
        const std::wstring&                         wstrVersion,
        const std::vector<std::wstring>&            vect_fields,
	    const std::vector<KLCSP::field_order_t>&    vect_fields_to_order,
	    long                                        lifetime,
	    KLEVP::EventProperties**                    ppEventProperties);

    static void CreateEventPropertiesProxyForHost(
        KLPRCP::CProxyBase*                         pOwnersProxy,
        KLSTD::KLBase*                              pOwner,
        const std::wstring&                         wstrDomainName,
        const std::wstring&                         wstrHostName,
        const std::wstring&                         wstrProduct,
        const std::wstring&                         wstrVersion,
	    const KLPAR::Params*                        pParamsIteratorDescr, //! Параметры фильтрации событий
        const std::vector<std::wstring>&            vect_fields,
	    const std::vector<KLCSP::field_order_t>&    vect_fields_to_order,
	    long                                        lifetime,
	    KLEVP::EventProperties**                    ppEventProperties);
};

#endif //__EVENTPROPERTIESPROXY_H_33B4A959_547C_427b_B386_DA9BCD94763D__

// Local Variables:
// mode: C++
// End:

