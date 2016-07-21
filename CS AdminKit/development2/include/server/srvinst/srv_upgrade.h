#ifndef __KLSRV_UPGRADE_H__
#define __KLSRV_UPGRADE_H__

#include <common/reporter.h>

namespace KLSRVUPGR
{
    struct upgrade_data_t
    {
        //std::wstring            m_wstrConnectionString;
        std::wstring            m_wstrStoresRoot;
        std::wstring            m_wstrDatabaseName;
        KLDB::DbConnStoragePtr  m_pDbConnStorage;
    };

    void CheckUpgradeFirstStart(upgrade_data_t& ud);
}

#endif //__KLSRV_UPGRADE_H__