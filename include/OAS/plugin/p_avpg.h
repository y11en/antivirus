// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Tuesday,  26 October 2004,  17:10 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2004.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Плагин для обработки событий из avpg.sys(vxd)
// Author      -- Sobko
// File Name   -- plugin_avpg.cpp
// Additional info
//    Реализация для Windows 9x/NT
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __public_plugin_avpg )
#define __public_plugin_avpg
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <OAS/iface/i_avpg.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
#define AVPG_ID  ((tPID)(61003))
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
#define VID_SOBKO  ((tVID)(67))
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin public definitions, AVPG )
// AVPG interface implementation
// Short comments -- Описание интерфейса для взаимодействия с драйвером-перехватчиком

// constants

// -
#define cAVPG_WORKING_MODE_ON_ACCESS   ((tDWORD)(0)) //  --
#define cAVPG_WORKING_MODE_ON_MODIFY   ((tDWORD)(1)) //  --
#define cAVPG_WORKING_MODE_ON_EXECUTE  ((tDWORD)(2)) //  --
#define cAVPG_WORKING_MODE_ON_OPEN     ((tDWORD)(3)) //  --
#define cAVPG_WORKING_MODE_ON_SMART    ((tDWORD)(4)) //  --	open+ex || open+r, open+w - check delayed list

// --- internal messages
#define pm_AVPG_NOTIFY_STARTED         ((tDWORD)(251)) // когда объект avpg создан и полностью проинициализирован
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // __public_plugin_avpg
// AVP Prague stamp end



