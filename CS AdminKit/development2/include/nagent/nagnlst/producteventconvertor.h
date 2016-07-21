/*!
* (C) 2005 "Kaspersky Lab"
*
* \file producteventconvertor.h
* \author Шияфетдинов Дамир
* \date 2005
* \brief Интерфейс конвертора событий и подписок для сетевых списков
*
*/

#ifndef KLNLST_PRODUCTEVENTCONVERTOR_H
#define KLNLST_PRODUCTEVENTCONVERTOR_H

#include <std/base/klbase.h>
#include <std/err/klerrors.h>

#include <kca/prss/settingsstorage.h>
#include <kca/pres/eventsstorage.h>
#include <kca/prcp/componentproxy.h>
#include <kca/prcp/agentproxy.h>

#include <nagent/nagnlst/common.h>
#include <nagent/naginst/naginst.h>

namespace KLNAGNLST {

	class ProductEventConvertor : public KLSTD::KLBase {
	public:

		typedef std::vector<std::wstring> SubsIdsList;
		typedef std::vector<std::wstring> ItemsListIds;

		virtual void Initialize( const KLPRCI::ComponentId &nagentId,
			KLNAG::NagentHelpers* pNagHelpers,
			const std::wstring &workingFolder,
			CONTIE::ConnectionTiedObjectsHelper *pTieStorage ) = 0;

		virtual void CreateSubscriptions( const std::wstring &listName, 
			const KLPRSS::product_version_t &productVersion,
			const KLPRCI::ComponentId &subscriber,
			KLPRES::EventsStorage *pEventsStorage, SubsIdsList &subsIdList ) = 0;

		virtual void DeleteSubscriptions( const std::wstring &listName, 
			const KLPRSS::product_version_t &productVersion,
			KLPRES::EventsStorage *pEventsStorage ) = 0;

		virtual void GetListAllItems( const std::wstring &listName, 
			const KLPRSS::product_version_t &productVersion,
			const KLPRCI::ComponentId &listCompId,
			int startPos, int needCount, int &nextPosOffset,
			KLPRCP::ComponentProxy *pProxy,
			KLNLST::NetworkListItemsList &itemsList, 
			KLPAR::ArrayValue **ppItemsIds ) = 0;

		virtual void OnSentToServer( const std::wstring &listName, 
			const KLPRSS::product_version_t &productVersion ) = 0;

		virtual bool StartConvertionEvents( const std::wstring &listName,
			const KLPRSS::product_version_t &prodVer ) = 0;
		
		virtual bool ConvertEvent( const std::wstring &listName, 
			const KLPRSS::product_version_t &productVersion,
			const std::wstring				&eventTypeFrom,
			KLSTD::CAutoPtr<KLPAR::Params>	pEventBodyFrom,
			std::wstring &itemId,
			std::wstring					&eventTypeTo,
			KLPAR::Params					**pEventBodyTo ) = 0;

		virtual void FinishConvertionEvents( const std::wstring &listName,
			const KLPRSS::product_version_t &prodVer ) = 0;

		virtual void CallListItemTask( const std::wstring &listName, 
			const KLPRSS::product_version_t &productVersion,
			KLPRCP::ComponentProxy			*pProxy,
			KLPRCP::AgentProxy				*pAgentProxy,
			const std::wstring				&listItemId,
			const std::wstring				&taskName,
			KLPAR::Params					*pTaskParams ) = 0;
		
		virtual void GetItemFileInfo( 
			KLTRAP::Transport::ConnectionFullName &trFullName,
			const std::wstring				&listName,
			const KLPRSS::product_version_t &productVersion,
			const std::wstring				&listItemId,
			std::wstring					&fileName,
			unsigned long					&lTotalSize ) = 0;

		virtual KLCONN::NetworkListItemsFile::FileOperationResult GetItemFileChunk( 
			KLTRAP::Transport::ConnectionFullName &trFullName,
			const std::wstring				&listName,
			const KLPRSS::product_version_t &productVersion,
			const std::wstring				&listItemId,
			unsigned long					startPos,
			unsigned long					neededSize,
			char							*pBuff,
			int								&buffSize ) = 0;

		
	};

} // end namespace KLNAGNLST

#endif // KLNLST_PRODUCTEVENTCONVERTOR_H
