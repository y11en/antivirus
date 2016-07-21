#if !defined(COMDEFS_H_INCLUDED_EE7D6BCE_3B25_4c1b_8C40_4211117BC479)
#define COMDEFS_H_INCLUDED_EE7D6BCE_3B25_4c1b_8C40_4211117BC479

#if _MSC_VER > 1000
    #pragma once
#endif

// system headers
#ifdef WIN32
    #include <winsock2.h>
    #include <windows.h>
#endif  // WIN32

// 4786 'identifier' : identifier was truncated to 'number' characters in the debug information
#pragma warning( disable : 4786 )


#ifndef WIN32
#include <unix/compatutils/compatutils.h>
#endif

// Prague headers
#include <Prague/prague.h>
#include <Prague/pr_cpp.h>
#include <Prague/pr_vector.h>
#include <Prague/pr_oid.h>
#include <Prague/iface/e_loader.h>
#include <Prague/iface/i_io.h>
#include <Prague/iface/e_ktime.h>
#include <Prague/pr_time.h>
#include <Prague/iface/i_mutex.h>
#include <AV/iface/i_avp3info.h>
#include <Prague/plugin/p_win32_reg.h>
#include <Prague/plugin/p_win32loader.h>
#include <Prague/plugin/p_win32_reg.h>
#include <AV/structs/s_avp3info.h>

// Product headers
#include <AV/iface/i_avs.h>
#include <ProductCore/structs/s_profiles.h>
#include <PPP/structs/s_bl.h>

// Updater core headers
#include "../helper/stdinc.h"
#include "../core/updater.h"

// Updater Prague Wrapper headers
#include "../../../SharedCode/PragueHelpers/PragueLog.h"
#include "../../../SharedCode/PragueHelpers/PragueHelpers.h"
#include "PragueDownloadProgress.h"
#include "PragueCallbacks.h"
#include "PragueJournal.h"
#include "UpdaterConfigurator/i_updaterconfigurator.h"
#include "UpdaterConfigurator/p_updaterconfiguratorimplementation.h"

#endif  // COMDEFS_H_INCLUDED_EE7D6BCE_3B25_4c1b_8C40_4211117BC479
