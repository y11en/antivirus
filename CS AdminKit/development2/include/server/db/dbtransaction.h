#ifndef __KL_DBTRANSACTION_H__
#define __KL_DBTRANSACTION_H__

#include <server/db/dbconnection.h>

/*!
    Defines KLDB_TRANSACTION_RERUN_BEGIN and 
    KLDB_TRANSACTION_RERUN_END must be used to rerun transaction which
    became a deadlock victim.
    
    Example:
        void SomeMethod(DbConnectionPtr pCon)
        {
		    KLERR_TRY
                KLDB_TRANSACTION_RERUN_BEGIN(pCon)
                    KLDB::AutoTransaction at(pCon);
                    pCon->ExecuteCommand( c_szwSt_1 );
                    pCon->ExecuteCommand( c_szwSt_2 );
                    pCon->ExecuteCommand( c_szwSt_3 );
                    at.Commit();
                KLDB_TRANSACTION_RERUN_END();
		    KLERR_CATCH(pError)
			    KLERR_SAY_FAILURE(1, pError);
                //@todo Do some error processing here
                //...
                KLERR_RETHROW();
		    KLERR_ENDTRY
        }
*/

#define KLDB_TRANSACTION_RERUN_BEGIN(_pTmpCon)  \
            KLSTD_TRACE2(3, L"KLDB_TRANSACTION_RERUN_BEGIN, file:%hs, line:%u\n", __FILE__, __LINE__); \
            {   \
                KLDB::AutoUnlock _tmp_aul(_pTmpCon); \
                KLDB::trans_rerun_t _TmpRerunData;    \
                KLDB::TransRerunTry(_pTmpCon, _TmpRerunData);   \
                bool _bTmpRerunTrans;   \
                do{ \
                    _bTmpRerunTrans = false;    \
                KLERR_TRY

#define KLDB_TRANSACTION_RERUN_END()    \
                KLSTD_TRACE2(3, L"KLDB_TRANSACTION_RERUN_END, file:%hs, line:%u\n", __FILE__, __LINE__); \
		        KLERR_CATCH(pError) \
                    KLSTD_TRACE2(3, L"KLDB_TRANSACTION_RERUN_END - CATCH, file:%hs, line:%u\n", __FILE__, __LINE__); \
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
    struct trans_rerun_t;

    //! internal function
    KLCSSRV_DECL void TransRerunTry(
            KLDB::DbConnectionPtr pCon, 
            trans_rerun_t& data);

    //! internal function
    KLCSSRV_DECL bool TransRerunCatch(
            KLERR::Error*   pError, 
            trans_rerun_t&  data,
            bool&           bRerun);

    //! internal function
    KLCSSRV_DECL void TransRerunSuccess(
            trans_rerun_t&  data);
    
    //! internal function
    KLCSSRV_DECL void TransRerunFinally(
            trans_rerun_t&  data);

    struct trans_errors_info_t;

    //! internal data structure
    struct trans_rerun_t
    {
        trans_rerun_t()
            :   m_nVersion(2)
            ,   m_nCount(0)
            ,   m_bMayRerun(false)
            ,   m_lTransId(0)
            ,   m_pErrorsInfo(NULL)
        {;};
        ~trans_rerun_t()
        {
            TransRerunFinally(*this);
        };
        size_t                  m_nVersion;
        size_t                  m_nCount;
        long                    m_lTransId;
        bool                    m_bMayRerun;
        trans_errors_info_t*    m_pErrorsInfo;
    };
};

#endif //__KL_DBTRANSACTION_H__
