#ifndef __KLDBSRVER_H__
#define __KLDBSRVER_H__

#include <server/db/dbconnection.h>
#include <server/db/dbtransaction.h>

namespace KLDB
{
    typedef enum
    {
        KLDBF_SQLMSDE = 1
    };

    typedef enum
    {
        KLDB_SQL_2000 = 8,
        KLDB_SQL_2005 = 9
    };

    class DbServerVersion
        :   public KLSTD::KLBaseQI
    {
    public:

        /*!
          \brief	Returns db server version

          \param	dwVerMajor      major version number
          \param	dwVerMinor      minor version number
          \param	dwFlags         flags (KLDBF_SQLMSDE)
        */
        virtual void GetDbServerVersion(
                        AVP_dword&  dwVerMajor, 
                        AVP_dword&  dwVerMinor, 
                        AVP_dword&  dwFlags) = 0;
    };

    typedef KLSTD::CAutoPtr<DbServerVersion> DbServerVersionPtr;
};

#endif //__KLDBSRVER_H__
