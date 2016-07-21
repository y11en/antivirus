/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file EventPropertiesIterator.h
 * \author Михаил Кармазин
 * \date 11:26 28.11.2002
 * \brief серверная часть для поддержки инерфейсов EventProperties, 
 */

#ifndef __EVENTPROPERTIESITERATOR_H__D6A4A8F1_6D04_43b0_A233_949C19798C19__
#define __EVENTPROPERTIESITERATOR_H__D6A4A8F1_6D04_43b0_A233_949C19798C19__

#include <srvp/csp/klcsp.h>
#include <srvp/evp/eventproperties.h>
#include <kca/prci/componentid.h>

namespace KLEVP {

	class KLSTD_NOVTABLE EventPropertiesIterator :  public KLSTD::KLBase
	{
	public:
		virtual void ResetIterator (
            EventPropertiesType eventPropertiesType,
            KLPAR::Params* pParamsIteratorDescr,
			const std::vector<std::wstring>& vect_fields,
			const std::vector<KLCSP::field_order_t>& vect_fields_to_order,
			long  lifetime,
			std::wstring& wstrIteratorId ) = 0;

        virtual long GetRecordCount( const std::wstring& wstrIteratorId )  = 0;

        virtual void GetRecordRange(
            const std::wstring& wstrIteratorId,
            long nStart,
            long nEnd,
            KLPAR::Params** pParamsEvents ) = 0;
        
        virtual void ReleaseIterator( const std::wstring& wstrIteratorId ) = 0;

		virtual void DeleteProperties( const std::vector<long>& vectIDs  ) = 0;
        
        virtual void InitiateDelete( KLSTD::CAutoPtr<KLPAR::Params> pSettings ) = 0;

        virtual void CancelDelete( KLSTD::CAutoPtr<KLPAR::Params> pSettings ) = 0;

        virtual void BulkDelete( KLSTD::CAutoPtr<KLPAR::Params> pSettings ) = 0;
	};

	class KLSTD_NOVTABLE EventPropertiesIterator2 :  public EventPropertiesIterator
	{
	public:
		virtual void ResetIteratorEx (
            EventPropertiesType eventPropertiesType,
            KLPAR::Params* pParamsIteratorDescr,
			const std::vector<std::wstring>& vect_fields,
			const std::vector<KLCSP::field_order_t>& vect_fields_to_order,
			long  lifetime,
			const std::wstring& wstrLocalConnectionName,
			const std::wstring& wstrRemoteConnectionName,
			std::wstring& wstrIteratorId ) = 0;
		
		virtual void AccessCheck( 			
			const std::wstring &wstrIteratorId,
			int action ) = 0;
	};
}

#endif // __EVENTPROPERTIESITERATOR_H__D6A4A8F1_6D04_43b0_A233_949C19798C19__

// Local Variables:
// mode: C++
// End:

