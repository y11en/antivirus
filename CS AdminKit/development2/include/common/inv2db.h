/*!
 * (C) 2007 Kaspersky Lab
 * 
 * \file	inv2db.h
 * \author	Eugene Rogozhnikov
 * \date	23.07.2007 17:03:29
 * \brief	Здесь описаны настройки, используемые для синхронизации inventory
				информации на хосте с информацией в базе данных на сервере
 * 
 */

#ifndef __INV2DB_H__
#define __INV2DB_H__

#include <avp\klids.h>

namespace INV2DB
{
	//!\brief Имя секции политики сетевого агента 
	const wchar_t KLINV_NAGENT_SECTION[] = KLCS_COMPONENT_NAGENT;

	//!\brief Имя поля в секции KLNLST_SEND_QB_LIST 
	const wchar_t KLINV_NAGENT_ENABLE_INVENTORY_FLAG[] = L"ENABLE_INVENTORY_FLAG"; // BOOL_T

	//////////////////////////////////////////////////////////////////////////
	// !\brief Имя сетевого списка для регистрации
	const wchar_t szInventoryNetworkListName[] = L"InventoryNetworkList";

	// Префиксы к строковому идентификатору для элементов, передаваемых по сети
	const wchar_t PRODUCT_ELEMENT_ID_PREFIX[] = L"prod";
	const wchar_t PATCH_ELEMENT_ID_PREFIX[] = L"patch";

	////////////////////////////////////////////////////////////////////////
	//	Имена параметров для элементов, передаваемых по сети

	// для продукта
	const wchar_t PROD_PRODUCT_ID[] = L"ProductID";
	const wchar_t PROD_IS_MSI[] = L"bIsMsi";
	const wchar_t PROD_PACKAGE_CODE[] = L"PackageCode";
	const wchar_t PROD_DISPLAY_NAME[] = L"DisplayName";
	const wchar_t PROD_DISPLAY_VERSION[] = L"DisplayVersion";
	const wchar_t PROD_PIBLISHER[] = L"Publisher";
	const wchar_t PROD_LANG_ID[] = L"LangId";
	const wchar_t PROD_INSTALL_DATE[] = L"InstallDate";
	const wchar_t PROD_INSTALL_DIR[] = L"InstallDir";
	const wchar_t PROD_COMMENTS[] = L"Comments";
	const wchar_t PROD_HELP_LINK[] = L"HelpLink";
	const wchar_t PROD_HELP_TELEPHONE[] = L"HelpTelephone";
	const wchar_t PROD_ARP_REGKEY[] = L"ARPRegKey";
	const wchar_t PROD_CLEANER_PRODUCT_NAME[] = L"CleanerProductName";

	// для патча
	const wchar_t PATCH_PATCH_ID[] = L"PatchID";
	const wchar_t PATCH_IS_MSI[] = L"bIsMsi";
	const wchar_t PATCH_PARENT_ID[] = L"ParentID";
	const wchar_t PATCH_DISPLAY_NAME[] = L"DisplayName";
	const wchar_t PATCH_DISPLAY_VERSION[] = L"DisplayVersion";
	const wchar_t PATCH_PIBLISHER[] = L"Publisher";
	const wchar_t PATCH_INSTALL_DATE[] = L"InstallDate";
	const wchar_t PATCH_COMMENTS[] = L"Comments";
	const wchar_t PATCH_HELP_LINK[] = L"HelpLink";
	const wchar_t PATCH_HELP_TELEPHONE[] = L"HelpTelephone";
	const wchar_t PATCH_CLASSIFICATION[] = L"Classification";
	const wchar_t PATCH_MORE_INFO_URL[] = L"MoreInfoURL";

}; // namespace INV2DB

#endif //__INV2DB_H__