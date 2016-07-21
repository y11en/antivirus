/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	srv_common.h
 * \author	Andrew Kazachkov
 * \date	07.07.2005 14:04:07
 * \brief	Common include for administration server
 * 
 */

#ifndef __KL_SRV_COMMON_H__
#define __KL_SRV_COMMON_H__

#include <std/base/klstd.h>
#include <std/base/klbaseqi_imp.h>
#include <std/thr/locks.h>
#include <std/err/klerrors.h>
#include <std/err/error.h>
#include <std/err/error2string.h>
#include <std/err/error_localized.h>
#include <std/tp/threadspool.h>
#include <std/par/params.h>
#include <std/par/par_conv.h>
#include <std/tmstg/timeoutstore.h>
#include <std/fmt/stdfmtstr.h>

#include <common/measurer.h>
#include <common/reporter.h>
#include <common/error2params.h>

#include <kca/prss/settingsstorage.h>
#include <kca/prss/prssconst.h>
#include <kca/prci/componentinstance.h>
#include <kca/prci/productstatistics2.h>
#include <kca/prcp/agentproxy.h>
#include <kca/prts/tasksstorage.h>
#include <kca/prts2/taskstorage2.h>
#include <kca/ss_srv/ss_srv.h>

#include <kca/contie/connectiontiedobjectshelper.h>

#include <server/db/dbconnection.h>
#include <server/db/dbtransaction.h>
#include <server/db/schema.h>
#include <server/db/dbinit.h>

#include <server/srvinst/server_constants.h>
#include <server/dbutils/dbutils.h>
#include <server/srvinst/srvinst_utils.h>
#include <server/srvinst/srvinstdata.h>
#include <server/klsplex/klsplex.h>

#include <vector>
#include <list>
#include <set>
#include <map>
#include <string>
#include <sstream>
#include <algorithm>

#endif //__KL_SRV_COMMON_H__
