#ifndef __DBCONWRAPPER_H__
#define __DBCONWRAPPER_H__

#include <server/db/dbconnection.h>
#include <server/db/dbtransaction.h>


// The same as KLDB_TRANSACTION_RERUN_BEGIN/KLDB_TRANSACTION_RERUN_END but without tracing

#define KLDB_DEADLOCK_RERUN_BEGIN(_pTmpCon)  \
            {   \
                KLDB::AutoUnlock _tmp_aul(_pTmpCon); \
                KLDB::trans_rerun_t _TmpRerunData;    \
                KLDB::TransRerunTry(_pTmpCon, _TmpRerunData);   \
                bool _bTmpRerunTrans;   \
                do{ \
                    _bTmpRerunTrans = false;    \
                KLERR_TRY

#define KLDB_DEADLOCK_RERUN_END()    \
		        KLERR_CATCH(pError) \
                    if(!KLDB::TransRerunCatch(pError, _TmpRerunData, _bTmpRerunTrans))    \
                    {   \
                        KLERR_RETHROW();    \
                    };  \
		        KLERR_ENDTRY    \
                }while(_bTmpRerunTrans);    \
                KLDB::TransRerunSuccess(_TmpRerunData);   \
            };

namespace KLDB
{
    typedef enum
    {
        dbcwf_no_dl_handling = 1    //! No internal deadlock handling
    }
    dbcw_flags_t;

    /*!
      \brief	DbConnectionWrapper is dbprovider-specific abstraction for 
                the db conection.
    */
    class DbConnectionWrapper : public KLSTD::KLBaseQI
    {
    public:
        /*!
          \brief	Returns value of specified bool flag
          \param	nId IN see dbcw_flags_t

          \return	value
        */
        virtual bool GetBoolFlag(int nId) = 0;

        /*!
          \brief	Sets value of specified bool flag
          \param	nId IN see dbcw_flags_t
          \param	bVal IN value

          \return	value
        */
        virtual void SetBoolFlag(int nId, bool bVal) = 0;
    };

    typedef KLSTD::CAutoPtr<DbConnectionWrapper> DbConnectionWrapperPtr;

    class AutoBoolFlag
    {
    public:
        AutoBoolFlag(KLSTD::KLBaseQI* pConn, int nId, bool bVal)
            :   m_nId(nId)
            ,   m_bOldValue(false)
        {
            if(pConn && m_nId)
            {
                DbConnectionWrapperPtr pCon;
                pConn->QueryInterface(KLSTD_IIDOF(KLDB::DbConnectionWrapper), (void**)&m_pConn);
                if(m_pConn)
                {
                    m_bOldValue = m_pConn->GetBoolFlag(m_nId);
                    m_pConn->SetBoolFlag(m_nId, bVal);
                };
            };
        };
        ~AutoBoolFlag()
        {
            if(m_pConn && m_nId)
            {
                m_pConn->SetBoolFlag(m_nId, m_bOldValue);
            };
        };
    protected:
        DbConnectionWrapperPtr  m_pConn;
        const int               m_nId;
        bool                    m_bOldValue;
    };
};

#endif //__DBCONWRAPPER_H__
