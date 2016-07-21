#ifndef __KL_DBINNERCONNECTION_H__
#define __KL_DBINNERCONNECTION_H__

#include <server/db/dbconnection.h>
#include <server/db/dbtransaction.h>

namespace KLDB
{
    //! in case of install mode
    const wchar_t c_szwIsInstallMode[] = L"KLDB_INSTALL_MODE";  //BOOL_T

    const long      c_nDefaultQuantum = 10000;

    typedef enum
    {
        dbic_rs_no,
        dbic_rs_single,
        dbic_rs_multiple
    }dbic_rs_type_t;

    /*!
      \brief	DbInnerConnection is dbprovider-specific abstraction for 
                the db conection.
    */
    class DbInnerConnection : public KLSTD::KLBaseQI
    {
    public:
        /*!
          \brief	Begins explicit transaction.
        */
        virtual void TransBegin() = 0;

        /*!
          \brief	Commits or rolls back explicit transaction.
        */
        virtual void TransEnd(bool bCommit) = 0;

        /*!
          \brief	Executes SQL statement which doesn't return any recordset.

          \param	szwCommand IN SQL statement
          \param	nRsType IN whether rs is required
          ]return   resulting user-side recordset
        */
        virtual DbRecordsetPtr ExecuteQuery(
                    const std::wstring& wstrCommand, 
                    dbic_rs_type_t nRsType) = 0;

        /*!
          \brief	Creates prepared statement and associates it with 
                    connection.

          \param	stmt    IN prepared statement desription
          \return	prepared statement for connection
        */
        virtual DbPreparedStmtPtr PutPrepStmt(
                                        const wchar_t* szwId,
                                        const prepstmt_props_t& stmt) = 0;

        /*!
          \brief	GetPrepStmt

          \param	szwId IN id
          \return	prepared statement for connection
        */
        virtual DbPreparedStmtPtr GetPrepStmt(const wchar_t* szwId) = 0;

        /*!
          \brief	Returns connection status. Returns false if connection is broken.
          \param	bForce  IN Forces connection check

          \return	false if if connection is broken
        */
        virtual bool CheckConnection(bool bForce) = 0;

        /*!
          \brief	It is required to recreate connection for some db servers due to bugs

          \return	true if required
        */
        virtual bool IfTimeToRecreate() = 0;
    };

    typedef KLSTD::CAutoPtr<DbInnerConnection> DbInnerConnectionPtr;

    struct conn_storage_par_t
    {
        conn_storage_par_t()
            :   m_nVersion(1)
            ,   m_nMaxGuiConnections(5)
            ,   m_nMaxLongWorkers(10)
            ,   m_nMaxOrdinalConnections(4)
            ,   m_nMinOrdinalConnections(3)
            ,   m_lConIdleQuantum(KLDB::c_nDefaultQuantum)
        {;};

        size_t  m_nVersion;
        size_t  m_nMaxGuiConnections;
        size_t  m_nMaxLongWorkers;
        size_t  m_nMaxOrdinalConnections;
        size_t  m_nMinOrdinalConnections;
        long    m_lConIdleQuantum;
    };

    //! class Db-specific connection factory
    class KLSTD_NOVTABLE DbInnerConnectionFactory
        :   public KLSTD::KLBaseQI
    {
    public:
        //! creates new real db connection
        virtual DbInnerConnectionPtr Allocate() = 0;

        //! fills conn_storage_par_t structire with best-matched values
        virtual void FillLimits(conn_storage_par_t& oLimits) = 0;

        //! returns connection string (for demonstration purposes only!!! without password)
        virtual std::wstring GetConnectionString() = 0;
        
        //! returns db server version
        virtual void GetDbServerVersion(
                        AVP_dword&  dwVersion, 
                        AVP_dword&  dwBuildNumber, 
                        AVP_dword&  dwFlags) = 0;
    };

    typedef KLSTD::CAutoPtr<DbInnerConnectionFactory> DbInnerConnectionFactoryPtr;

    //!DbConnector exported function name
    #define KLDB_CreateFactoryName          "KLDB_CreateFactory"

    //!DbConnector exported function pointer

    /*!
      \brief	pCreateInnerConnectionFactory_t

      \param	pParams     IN  connection parameters
      \param	ppFactory   OUT 
    */
    typedef void (*pCreateInnerConnectionFactory_t)(
                    KLPAR::Params*              pParams,
                    DbInnerConnectionFactory**  ppFactory);

    /*!
        Automatically begins and rolls back (by default) or commits 
        transaction.
        Sample:
        ...
        {
            InnerAutoTransaction at(pConn);
            ...
            at.m_bCommit = true;
        };
        ...
    */
    class InnerAutoTransaction
    {
    public:
        InnerAutoTransaction(DbInnerConnection* pDbConnection)
            :   m_pCon(pDbConnection)
            ,   m_bCommit(false)
        {
            if(m_pCon)
                m_pCon->TransBegin();
        };
        ~InnerAutoTransaction()
        {
            if(m_pCon)
            {
                try
                {
                    m_pCon->TransEnd(m_bCommit);
				}
				catch(KLERR::Error *pError){
					if(pError)pError->Release();
				}catch(std::exception&){
					KLSTD_ASSERT(false);
				};
            };
        };
    protected:
        DbInnerConnectionPtr m_pCon;
    public:
        bool            m_bCommit;
    };

    class DbInnerConnStorage;

    class DbSharedConnection : public DbConnection
    {
    public:
        virtual void Create(
                    dbconn_type_t       nTypeAndPriority,
                    long                lTimeout,
                    DbInnerConnStorage* pStorage) = 0;
        KLSTD_NOTHROW virtual void Destroy() throw() = 0;
    };

    typedef KLSTD::CAutoPtr<DbSharedConnection> DbSharedConnectionPtr;

    class DbPreparedStmt;
    
    class DbPreparedStmtWrapper : public DbPreparedStmt
    {
    public:
        virtual void Create(
                    KLDB::DbConnectionPtr   pCon,
                    DbPreparedStmtPtr       pStmt) = 0;

        virtual KLSTD_NOTHROW void Destroy() throw() = 0;
    };

    typedef KLSTD::CAutoPtr<DbPreparedStmtWrapper> DbPreparedStmtWrapperPtr;


    class DbInnerConnStorage : public KLSTD::KLBaseQI
    {
    public:
        virtual void AllocateConnection(
                        dbconn_type_t       nTypeAndPriority,
                        long                lTimeout,
                        DbInnerConnection** ppConnection) = 0;

        virtual void FreeConnection(DbInnerConnection* pConnection) = 0;
        
        virtual DbPreparedStmtWrapperPtr AllocatePrepStmtWrapper() = 0;

        virtual KLDB::DbValueFactoryPtr GetDbValueFactory() = 0;

        virtual KLDB::DbLiteralsPtr GetDbLiterals() = 0;

        virtual std::wstring GetConnectionString() = 0;
    };

    typedef KLSTD::CAutoPtr<DbInnerConnStorage> DbInnerConnStoragePtr;

};

KLCSSRV_DECL void KLDB_CreateDbConnStorageSingle(
            KLPAR::Params*          pParams,
            KLDB::DbConnStorage**   ppStorage);

#endif //__KL_DBINNERCONNECTION_H__
