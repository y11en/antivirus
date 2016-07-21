// global definitions
//

#pragma once

#include "PragueLoader.h"

class cGuiStub;

extern DWORD                            g_nTlsIndex;
extern HINSTANCE                        g_hInst;
extern CPragueLoader                    g_Prague;
extern cSerObj<CProfileAdm>             g_pProduct;
extern cGuiStub *                       g_hGui;

#define g_pRoot                         ((CRootItem *)TlsGetValue(g_nTlsIndex))

// gui definitions

enum eProductType
{
	eProductTypeUnknown,
	eProductTypeFs,
	eProductTypeWks,
	eProductTypeSos,
};

eProductType GetProductType();
wchar_t *    GetProductName();
char *       GetProductRegistryPath();

#define STRID_PROFILEDESCRIPTION        "ProfileDescription"

#define AVP_SOURCE_GUI                  "Gui"

#define KLCS_MODULENAME	                L"AdminGUI"

#define KAVFS_PROXY_MODULE_NAME         L"ap_fs.dll"
#define KAVWKS_PROXY_MODULE_NAME        L"ap_wks.dll"
#define KAVSOS_PROXY_MODULE_NAME        L"ap_sos.dll"

#define KAVFS_MODULE_NAME				L"ap_fsi.dll"
#define KAVWKS_MODULE_NAME				L"ap_wksi.dll"
#define KAVSOS_MODULE_NAME				L"ap_sosi.dll"

#define KAVFS_PRODUCT_NAME              L"KAVFS7"
#define KAVWKS_PRODUCT_NAME             L"KAVWKS7"
#define KAVSOS_PRODUCT_NAME             L"KAVSOS7"

#define KAVFS_WKS_PRODUCT_VERSION       L"7.0.0.0"

#define KAV_PLUGIN_MUTEX                L"wks_fs_ap_plugin"

#define REGPATH_SETTINGS_FS             "HKEY_LOCAL_MACHINE\\SOFTWARE\\KasperskyLab\\Components\\34\\Products\\" VER_PRODUCT_REGISTRY_AK_FS "\\" VER_PRODUCT_REGISTRY_NAME
#define REGPATH_SETTINGS_WKS            "HKEY_LOCAL_MACHINE\\SOFTWARE\\KasperskyLab\\Components\\34\\Products\\" VER_PRODUCT_REGISTRY_AK_WKS "\\" VER_PRODUCT_REGISTRY_NAME
#define REGPATH_SETTINGS_SOS            "HKEY_LOCAL_MACHINE\\SOFTWARE\\KasperskyLab\\Components\\34\\Products\\" VER_PRODUCT_REGISTRY_AK_SOS "\\" VER_PRODUCT_REGISTRY_NAME

// Название сообщения - уведомления окна-родителя страницы о ее изменении пользователем
#define RWM_KLAKCON_PPN_PAGE_MODIFIED	"RWM_KLAKCON_PPN_PAGE_MODIFIED"

// Interfaces
#define KAV_INTERFACE_PROPERTY_SHEET    L"IPropertySheet"
#define KAV_INTERFACE_POLICY_SETTINGS   L"POLICY_SETTINGS"
#define KAV_INTERFACE_LOCAL_SETTINGS    L"LOCAL_SETTINGS"
#define KAV_INTERFACE_TASK_PARAMS       L"TASK_PARAMS"
#define KAV_INTERFACE_AGENT_PROXY       L"AGENT_PROXY"

// Paramers name for GetEvents()
#define EVENT_ARRAY_NAME				L"Events"
#define EVENT_COMPONENT_NAME			L"Component"
#define EVENT_ID_NAME					L"EventID"
#define EVENT_ID_SEVERITY				L"Severity"
#define EVENT_ID_LOCALIZED_NAME			L"LocalizedEventIDName"

void AntispamShowTrainingWizard (CItemBase* pParent = NULL);

#include "globals.h"
