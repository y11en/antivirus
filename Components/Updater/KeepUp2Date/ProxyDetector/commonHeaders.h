#if !defined(AFX_STDAFX_H__F40BF3AB_99DF_4450_9633_7A1FCF381FF7__INCLUDED_)
#define AFX_STDAFX_H__F40BF3AB_99DF_4450_9633_7A1FCF381FF7__INCLUDED_

#if _MSC_VER > 1000
	#pragma once
#endif // _MSC_VER > 1000

// Exclude rarely-used stuff from Windows headers
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <windows.h>

// 4786 - identifier was truncated to '255' characters in the browser information
#pragma warning(disable:4786)

#include <string>
#include <vector>
#include <algorithm>
#include <memory>


// this WinInet.h file which is neither from SDK, no from Visual Studio headers,
//  but from some other unknown source
#include "winInet_NotFromSDK.h"


// remove all these headers. The headers are included to only provide Network functionanlity
//  which has bad design and can not be separated from Updater sources now

#include "../Client/include/update_source.h"
#include "../Client/include/cfg_updater.h"
#include "../Client/include/callback_iface.h"
#include "../Client/include/journal_iface.h"
#include "../Client/helper/updaterStaff.h"

#include "../Client/helper/local_file.h"

#include "../Client/net/netTransfer.h"
#include "../Client/core/HttpProtocolImplementation.h"

#endif // !defined(AFX_STDAFX_H__F40BF3AB_99DF_4450_9633_7A1FCF381FF7__INCLUDED_)
