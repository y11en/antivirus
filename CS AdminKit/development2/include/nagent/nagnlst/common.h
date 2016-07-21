/*!
 * (C) 2005 "Kaspersky Lab"
 *
 * \file nagent/nlst/common.h
 * \author Дамир Шияфетдинов
 * \date 2005
 * \brief Общие опеределения модуля NLST
 */

#if !defined(NAGENT_COMMON)
#define NAGENT_COMMON

#include <string>

#include <std/time/kltime.h>
#include <std/par/params.h>

namespace KLNLST {
	
	class NetworkListItem
	{
	public:
		NetworkListItem() 
		{
			bDeletedFlag = false;
		}

		NetworkListItem( const NetworkListItem &li )
		{
			wstrItemId = li.wstrItemId;		
			tTime = li.tTime;
			pItemParams = li.pItemParams;
			bDeletedFlag = li.bDeletedFlag;
		}

		std::wstring					wstrItemId;		
		
		KLSTD::precise_time_t			tTime;
		KLSTD::CAutoPtr<KLPAR::Params>	pItemParams;

		bool							bDeletedFlag;
	};

	typedef std::vector<NetworkListItem> NetworkListItemsList;

	/*
		!\brief Идентификатор подписки
	*/
	const wchar_t LIST_CHANGED_SUBS_ID[]			=	L"SUBS";
	
	/*
		!\brief Событие об изменении списка

		Данное события генерируются на конечном хосте, и в случае необходимости, доставляются на сервер
		при помоши EVP
	*/
	const wchar_t LIST_CHANGED_EVENT[]			=	L"list_changed";


	//!\brief Стандартные поля тела событий

	/* \!brief Тип произведенной операции
	*/
	const wchar_t LIST_ITEM_CHANGED_TYPE[]		=	L"li_change_type"; 

	const wchar_t LIST_ITEM_CHANGED_ADDED[]		=	L"item_added";
	const wchar_t LIST_ITEM_CHANGED_DELETED[]	=	L"item_deleted";
	const wchar_t LIST_ITEM_CHANGED_MODIFIED[]	=	L"item_modified";

	/* \!\brief Идентификатор элемента списка. 
		
		Используя данный идентификатор производяться все операции над данным элементом списка на 
		конечном хосте.
	 */
	const wchar_t LIST_ITEM_ID[]					=	L"list_item_id"; 
	
	/* \!\brief Список идентификатор элементов списка. 
		
	 */
	const wchar_t LIST_ITEM_IDS[]					=	L"list_item_ids"; 

	/* \!\brief Имя зарегестрированного на сервере списка.		
	 */
	const wchar_t LIST_NAME[]					=	L"list_name";


	/* \!\brief Имя компоненты списка		
	 */
	const wchar_t LIST_COMPONENT_NAME[]			=	L"list_comp_name";

	/* \!\brief Параметры элемента списка. 
		
		Данные параметры имеею свободный формат и используются только на уровне польз. интерфейса
		и при выполнении операции над элементом списка на конечном хосте
	 */
	const wchar_t LIST_ITEM_PARAMS[]				=	L"list_item_params";
	
	
} // namespace KLNLST

#endif // !defined(NAGENT_COMMON)
