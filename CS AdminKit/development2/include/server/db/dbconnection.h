#ifndef __KL_DBCONNECTION_H__
#define __KL_DBCONNECTION_H__

#include <std/base/klstd.h>
#include <std/par/params.h>
#include <server/db/dbvalue.h>
#include <server/db/dbrecordset.h>
#include <server/db/dbprepstmt.h>
#include <server/db/errors.h>
#include <server/db/dbliterals.h>

namespace KLDB
{
    //! DbConnection category and priority
    typedef enum
    {
        // connection priorities
        DBCT_PRIORITY_MASK      = 0x0000FFFF,        
        DBCT_PRIORITY_DEFAULT   = 0x00000000,
        DBCT_PRIORITY_LOW       = 0x00000001,
        DBCT_PRIORITY_NORMAL    = 0x00000009,
        DBCT_PRIORITY_HIGH      = 0x00000011,
        // connection categories
        DBCT_CATEGORY_MASK      = 0xFFFF0000,
        DBCT_CATEGORY_AUTH      = 0x00010000, //! transport authentication
        DBCT_CATEGORY_NAGENT    = 0x00020000, //! data from network agent is ready
        DBCT_CATEGORY_GUI       = 0x00030000, //! gui call
        DBCT_CATEGORY_WORKER    = 0x00040000, //! some periodical worker process
        DBCT_CATEGORY_LONGWORKER= 0x00050000, //! some periodical worker process
        DBCT_CATEGORY_SRVHRCH   = 0x00060000  //! server hierarchy data is ready
    };

    typedef unsigned long dbconn_type_t;

    /*!
      \brief	DbConnection is an abstraction for the db conection for 
                multithread usage. After instance of DbConnection had been 
                locked it can be used only by owner thread until method 
                DbConnection::Unlock is called. Until DbConnection isn't 
                locked no real db connection is associated with it. 
    */
    class DbConnection : public DbValueFactory
    {
    public:
       /*!
          \brief	Locks the connection. After calling this method real db
                    connection is being associated with DbConnection object, 
                    calling thread becomes owner of the connection and has 
                    exclusive access to it.

          \exception    DB_ERR_GENERAL          some database error
          \exception    DB_ERR_INVALID_OWNER    current thread isn't owner of connection
        */
        virtual void Lock() = 0;

        /*!
          \brief	Unlocks the connection. This method tears off relation 
                    to the real db connection.

          \exception    DB_ERR_GENERAL          some database error
          \exception    DB_ERR_INVALID_OWNER    current thread isn't owner of connection
          \exception    DB_ERR_UNLOCKED         there is no corresponding Unlock call
        */
        KLSTD_NOTHROW virtual void Unlock() throw() = 0;

        /*!
          \brief	Returns thread id of owning thread.

          \return	unsigned long 
        */
        virtual unsigned long  GetOwnerThreadId() = 0;

        /*!
          \brief	Returns type and priority specified during DbConnection 
                    instance acquiring.

          \return	dbconn_type_t 
          \exception    DB_ERR_INVALID_OWNER    current thread isn't owner of connection
        */
        virtual dbconn_type_t GetTypeAndPriority() = 0;

        /*!
          \brief	Begins explicit transaction. This method interanlly 
                    calls Lock method.

          \exception    DB_ERR_GENERAL          some database error
          \exception    DB_ERR_INVALID_OWNER    current thread isn't owner of connection
        */
        virtual void TransBegin() = 0;

        /*!
          \brief	Commits explicit transaction. This method interanlly 
                    calls Unlock method.

          \exception    DB_ERR_GENERAL          some database error
          \exception    DB_ERR_INVALID_OWNER    current thread isn't owner of connection
          \exception    DB_ERR_NO_TRANSACTION   there is no corresponding TransBegin call
        */
        virtual void TransCommit() = 0;

        /*!
          \brief	Rolls back explicit transaction. This method interanlly 
                    calls Unlock method.

          \exception    DB_ERR_GENERAL          some database error
          \exception    DB_ERR_INVALID_OWNER    current thread isn't owner of connection
          \exception    DB_ERR_NO_TRANSACTION   there is no corresponding TransBegin call
        */
        virtual void TransRollback() = 0;

        /*!         Determines whether the connection is in the transaction 
                    mode
          \return   non-zero if the connection is in the 
                    transaction mode. If connection isn't locked the method
                    returns zero.

          \exception    DB_ERR_INVALID_OWNER    current thread isn't owner of connection
        */
        virtual long InTransaction() = 0;

        /*!
          \brief	Executes SQL statement which doesn't return any recordset.

          \param	szwCommand IN SQL statement

          \exception    DB_ERR_GENERAL          some database error
          \exception    DB_ERR_INVALID_OWNER    current thread isn't owner of connection
        */
        virtual void ExecuteCommand(const std::wstring& wstrCommand) = 0;

        /*!
          \brief	Executes SQL query and returns resulting user-side 
                    recordset. Returned user-side recordset is not attached 
                    to any db connection so it can be safely used after the 
                    connection was freed. Calls Lock/Lock are not required
                    to call this method as they are called internally.

          \param	wstrCommand IN  query
          \param	bMultiRs IN  multiple recordset are expected
          \return   resulting user-side recordset

          \exception    DB_ERR_GENERAL          some database error
          \exception    DB_ERR_INVALID_OWNER    current thread isn't owner of connection
        */
        virtual DbRecordsetPtr ExecuteQuery(
                    const std::wstring& wstrCommand, 
                    bool bMultiRs = false) = 0;

        /*!
          \brief	Creates prepared statement and associates it with 
                    connection.

          \param	id      IN prepared statement id
          \param	stmt    IN prepared statement desription
          \return	prepared staement

          \exception    DB_ERR_GENERAL          some database error
          \exception    DB_ERR_INVALID_OWNER    current thread isn't owner of connection
          \exception    DB_ERR_UNLOCKED         connection must be locked 
        */
        virtual DbPreparedStmtPtr AddPrepStmt(
                            const wchar_t* id, 
                            const prepstmt_props_t& stmt) = 0;

        /*!
          \brief	Returns prepared statement

          \param	id IN prepared staement id
          \return	prepared staement

          \exception    DB_ERR_GENERAL          some database error
          \exception    DB_ERR_INVALID_OWNER    current thread isn't owner of connection
          \exception    DB_ERR_UNLOCKED         connection must be locked 
        */
        virtual DbPreparedStmtPtr GetPrepStmt(const wchar_t*      id) = 0;
    };

    typedef KLSTD::CAutoPtr<DbConnection> DbConnectionPtr;

    class AutoUnlock
    {
    public:
        AutoUnlock(DbConnection* pDbConnection)
            :   m_pCon(pDbConnection)
        {
            if(m_pCon)
                m_pCon->Lock();
        };
        ~AutoUnlock()
        {
            if(m_pCon)
                m_pCon->Unlock();
        };
        DbConnectionPtr m_pCon;
    };

    /*!
        Automatically begins and rolls back transaction.
        Sample:
        ...
        {
            AutoTransaction at(pConn);
            ...
            at.Commit();
        };
        ...
    */
    class AutoTransaction
    {
    public:

        /*!
          \brief	AutoTransaction

          \param	pCon
          \param	bBeginAlways    'false' means that AutoTransaction
                                    begins transaction only if the 
                                    connection not in the transaction 
                                    mode yet
          \return	
        */
        AutoTransaction(DbConnection* pCon, bool bBeginAlways = true)
        {
            if(bBeginAlways || (pCon && !pCon->InTransaction()))
                m_pCon = pCon;
            if(m_pCon)
                m_pCon->TransBegin();
        };

        void Commit()
        {
            if(m_pCon)
            {
                m_pCon->TransCommit();
                m_pCon = NULL;
            };
        };

        ~AutoTransaction()
        {
            if(m_pCon)
            {
                try
                {
                    m_pCon->TransRollback();
				}
				catch(KLERR::Error *pError){
					if(pError)pError->Release();
				}catch(std::exception&){
					KLSTD_ASSERT(false);
				};
            };
        };
        DbConnectionPtr m_pCon;
    };

    class KLSTD_NOVTABLE DbConnStorage : public DbValueFactory
    {
    public:
        /*!
          \brief	Waits for all conection being unlocked and deinitializes.
                    All new requests for connection (AcquireDbConnection) will 
                    fail.
        */
        virtual void Destroy() = 0;

        /*!
          \brief	Acquires DbConnection instance. 

          \param	nTypeAndPriority    IN  connection usage category and priority
          \param	ppConnection        OUT connection
          \param	lTimeout            IN  timeout to wait for locking
        */
        virtual void AcquireDbConnection(  
                                        dbconn_type_t   nTypeAndPriority,
                                        DbConnection**  ppConnection,
                                        long            lTimeout = KLSTD_INFINITE) = 0;
        
        /*!
          \brief	WatchdogCheck

          \return	returns false if at least one connection seems to hang
        */
        virtual bool WatchdogCheck()  = 0;
    };

    typedef KLSTD::CAutoPtr<DbConnStorage>  DbConnStoragePtr;
};

KLCSSRV_DECL void KLDB_CreateDbConnStorage(
            KLPAR::Params*          pParams,
            KLDB::DbConnStorage**   ppStorage);

#endif //__KL_DBCONNECTION_H__
