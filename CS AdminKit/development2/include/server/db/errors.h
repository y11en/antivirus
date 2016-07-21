#ifndef __KL_DB_ERRORS_H__
#define __KL_DB_ERRORS_H__

namespace KLDB
{
    enum ErrorCodes 
    {
        DB_ERR_GENERAL = KLDBSTART,
        DB_ERR_UNLOCKED,
        DB_ERR_NO_TRANSACTION,
        DB_ERR_INVALID_OWNER,
        DB_ERR_CLOSED_RECORDSET,
        DB_ERR_INVALID_TYPECAST,
        DB_ERR_INVALID_PARAMETER,
        DB_ERR_DEADLOCK,
        DB_ERR_DEADLOCK_FATAL,
        DB_ERR_UNSUPPORTED_DB_SERVER,
        DB_ERR_UNSUPPORTED_DB_CLIENT
    };

    const struct KLERR::ErrorDescription c_errorDescriptions [] = {
        {DB_ERR_GENERAL, L"Generic db error: \"%ls\""},
        {DB_ERR_UNLOCKED, L"Connection must be locked to perform the operation"},
        {DB_ERR_NO_TRANSACTION, L"Connection is not in transaction mode"},
        {DB_ERR_INVALID_OWNER, L"Calling thread is not connection owner"},
        {DB_ERR_CLOSED_RECORDSET, L"Unable to perform operation on a closed recordset"},
        {DB_ERR_INVALID_TYPECAST, L"Invalid typecast"},
        {DB_ERR_INVALID_PARAMETER, L"Invalid parameter"},
        {DB_ERR_DEADLOCK,   L"Transaction became the deadlock victim: '%ls'"},
        {DB_ERR_DEADLOCK_FATAL,   L"Failed to recover from a deadlock: '%ls'"},
        {DB_ERR_UNSUPPORTED_DB_SERVER,   L"Unsupported DB server version: %ls."},
        {DB_ERR_UNSUPPORTED_DB_CLIENT,   L"Unsupported DB client version: %ls."},
        {0,0}
    };

#define DB_THROW_ERR_GENERAL(_str)    \
                    KLERR_THROW1(   L"KLDB", \
                                    KLDB::DB_ERR_GENERAL, \
                                    (const wchar_t*)_str)

#define DB_THROW_ERR_UNLOCKED()     \
                    KLERR_THROW0(L"KLDB", KLDB::DB_ERR_UNLOCKED)

#define DB_THROW_ERR_NO_TRANSACTION() \
                    KLERR_THROW0(L"KLDB", KLDB::DB_ERR_NO_TRANSACTION)

#define DB_THROW_ERR_INVALID_OWNER()  \
                    KLERR_THROW0(L"KLDB", KLDB::DB_ERR_INVALID_OWNER)

#define DB_THROW_ERR_CLOSED_RECORDSET()  \
                    KLERR_THROW0(L"KLDB", KLDB::DB_ERR_CLOSED_RECORDSET)


#define DB_THROW_ERR_INVALID_TYPECAST()  \
                    KLERR_THROW0(L"KLDB", KLDB::DB_ERR_INVALID_TYPECAST)

#define DB_THROW_ERR_INVALID_PARAMETER()  \
                    KLERR_THROW0(L"KLDB", KLDB::DB_ERR_INVALID_PARAMETER)

#define DB_THROW_ERR_DEADLOCK(_desc)    \
                    KLERR_THROW1(L"KLDB", KLDB::DB_ERR_DEADLOCK, (const wchar_t*)_desc)

#define DB_THROW_ERR_UNSUPPORTED_DB_SERVER(_desc)    \
                    KLERR_THROW1(L"KLDB", KLDB::DB_ERR_UNSUPPORTED_DB_SERVER, (const wchar_t*)_desc)

#define DB_THROW_ERR_UNSUPPORTED_DB_CLIENT(_desc)    \
                    KLERR_THROW1(L"KLDB", KLDB::DB_ERR_UNSUPPORTED_DB_CLIENT, (const wchar_t*)_desc)

};

#endif //__KL_DB_ERRORS_H__
