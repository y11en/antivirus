/*!
 * (C) 2005 "Kaspersky Lab"
 *
 * \file listitemiterator.h
 * \author Дамир Шияфетдинов
 * \date 2005
 * \brief Интерфейс класса итератора для элементов списка
 */


#if !defined(KLNLST_LISTITEMITERATOR)
#define KLNLST_LISTITEMITERATOR

#include <string>
#include <list>

#include <std/base/kldefs.h>
#include <std/base/klbase.h>
#include <std/tmstg/timeoutstore.h>

#include <kca/prci/componentid.h>

#include <srvp/csp/klcsp.h>

#include <server/db/dbconnection.h>
#include <server/srvinst/srvinstdata.h>

#include <srvp/nlst/networklistproxy.h>

namespace KLNLST {

	//!\brief Поля для выборок при помоши ListItemIterator ( VIEW v_full_nl_items  )
	const wchar_t FILED_ITEM_ID[]			= L"strId";
	const wchar_t FILED_HOST_NAME[]			= L"strHostDisplayName";			// может использоваться для сортировки
	const wchar_t FILED_CREATION_TIME[]		= L"tmCreation";				// может использоваться для сортировки
	const wchar_t FILED_PARAMS[]			= L"params";

	class ListItemIterator : public KLSTD::KLBase
	{
	public:
		virtual void ResetIterator (
            const std::wstring &listName,
            KLPAR::Params* pParamsIteratorDescr,
			const std::vector<std::wstring>& vect_fields,
			const std::vector<KLCSP::field_order_t>& vect_fields_to_order,
			long  lifetime,
			const std::wstring& wstrLocalConnectionName,
			const std::wstring& wstrRemoteConnectionName,
			std::wstring& wstrIteratorId ) = 0;

        virtual long GetRecordCount( const std::wstring& wstrIteratorId )  = 0;

        virtual void GetRecordRange(
            const std::wstring& wstrIteratorId,
            long nStart,
            long nEnd,
            KLPAR::Params** pParamsListItems ) = 0;
        
        virtual void ReleaseIterator( const std::wstring& wstrIteratorId ) = 0;
	};

} // end namespace KLNLST

#endif // !defined(KLNLST_LISTITEMITERATOR)