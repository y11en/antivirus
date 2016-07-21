#ifndef __KL_DBPREPSTMT_H__
#define __KL_DBPREPSTMT_H__

#include <server/db/dbvalue.h>
#include <server/db/dbrecordset.h>

namespace KLDB
{
    typedef enum{
        pspfInput = 1,
        pspfOutput = 2
    }prepstmt_par_flags_t;

    typedef enum{
        psptText = 1,
        psptStoredProc = 2
    }prepstmt_type_t;

    struct prepstmt_par_t
    {
        prepstmt_par_t(
                    dbvalue_type_t          nType,
                    size_t                  nLength = 0,
                    prepstmt_par_flags_t    nFlags = pspfInput)
            :   m_nType(nType)
            ,   m_nLength(nLength)
            ,   m_nFlags(nFlags)
        {;};
        dbvalue_type_t          m_nType;
        size_t                  m_nLength;
        prepstmt_par_flags_t    m_nFlags;
    };
    
    typedef std::vector<prepstmt_par_t> vec_prepstmt_par_t;

    struct prepstmt_props_t;

    void Fill(
                        prepstmt_type_t         nType,
                        const std::wstring&     wstrStatement,
                        const prepstmt_par_t*   pParameters,
                        size_t                  nParameters,
                        prepstmt_props_t&       result);

    struct prepstmt_props_t
    {
        prepstmt_props_t(   prepstmt_type_t             nType,
                            const std::wstring&         wstrStatement,
                            const vec_prepstmt_par_t&   vecParameters)
            :   m_nType(nType)
            ,   m_wstrStatement(wstrStatement)
            ,   m_vecParameters(vecParameters)
        {;};
        prepstmt_props_t(   prepstmt_type_t         nType,
                            const std::wstring&     wstrStatement,
                            const prepstmt_par_t*   pParameters,
                            size_t                  nParameters)
        {
            Fill(nType, wstrStatement, pParameters, nParameters, *this);
        };
        std::wstring                m_wstrStatement;
        prepstmt_type_t             m_nType;
        std::vector<prepstmt_par_t> m_vecParameters;
    };

    typedef DbFieldsPtr DbParametersPtr;

    class DbPreparedStmt : public KLSTD::KLBaseQI
    {
    public:
        /*!
          \brief    Retrieves prepared statement parameters.

          \return	Set of parameters  
        */
        virtual DbParametersPtr GetParameters() = 0;

        /*!
          \brief Executes prepared statement and returns recordset

          \param	bMultiRs [in] multiple recordset are expected
          \return	resulting recordset
        */
        virtual DbRecordsetPtr Execute(bool bMultiRs = false) = 0;

        /*!
          \brief Executes prepared statement without returning recordset

        */
        virtual void ExecuteNoRs() = 0;
    };

    typedef KLSTD::CAutoPtr<DbPreparedStmt> DbPreparedStmtPtr;
};

#endif //__KL_DBPREPSTMT_H__
