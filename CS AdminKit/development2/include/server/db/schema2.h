/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	schema2.h
 * \author	Andrew Kazachkov
 * \date	18.07.2005 14:30:30
 * \brief	Hand-made db methods
 * 
 */

#include <server/db/schema.h>

namespace KLDBSQL
{
    std::wstring GetGroupName(
                    KLDB::DbConnectionPtr   pCon,
                    long                    nId);

    std::wstring GetHostDisplayName(
                    KLDB::DbConnectionPtr   pCon,
                    long                    nId);

    std::wstring GetHostGroupName(
                    KLDB::DbConnectionPtr   pCon,
                    long                    nId);

    std::wstring GetGroupFullName(
                    KLDB::DbConnectionPtr   pCon,
                    long                    nId);

    std::wstring GetAdsOUFullName(
                    KLDB::DbConnectionPtr   pCon,
                    long                    nIdOU);
};