#ifndef __KLSPLEX_H__
#define __KLSPLEX_H__

#include <std/base/klbase.h>
#include <std/thrstore/threadstore.h>
#include <transport/wat/common.h>
#include <transport/spl/common.h>

#ifndef KLCS_MODULENAME
    #define KLCS_MODULENAME L"KLSPLEX"
    #define KLSPLEX_UNDEF_MOD
#endif


namespace KLSPLEX
{
    typedef long splid_t;
	typedef std::wstring splmaphash_t;

    const splid_t c_nInvalidId = -1;

    typedef KLSTD::CAutoPtr<KLWAT::ClientContext> CtxPtr;

    /*!
      \brief	acquires session id from current thread

      \param	bThrow  if throw exception in case of error or return c_nInvalidId
      \return	session id
    */
    splid_t AcquireSessionId(bool bThrow = true);
	
	/*!
      \brief	acquires access map hash from current thread

      \param	bThrow  if throw exception in case of error or return empty string
      \return	hash
    */
	splmaphash_t AcquireMapHash(bool bThrow = true);

    /*!
      \brief	Acquires client context

      \return	client context, may be NULL if bThrowIfNo is false
    */
    CtxPtr AcquireContext(bool bThrowIfNo = true);

    /*!
      \brief	Stores client context for thread
      \param	pContext client context, may be NULL
    */
    void StoreContext(CtxPtr pContext);

    //! \brief stores session id for thread 
    class AutoStoreContext
    {
    public:
        /*!
          \brief	Sets client context for thread with later rolling back

          \param	pContext client context, may be NULL
        */
        AutoStoreContext(CtxPtr pContext)
            :   m_pOldClientContext(AcquireContext(false))
        {
            StoreContext(pContext);
        };
        
        ~AutoStoreContext()
        {
            KLERR_IGNORE( StoreContext(m_pOldClientContext); );
        };
    protected:
        CtxPtr m_pOldClientContext;
    };
    

    //! helpers


    /*!
      \brief	Adds join to the view v_acm_accessmap. 
                Useful for runtime-formed queries. 

        Example:

            KLSPLEX::AddInnerJoin_AccessMap_Read(os, L"v_hosts.nGroup", m_pSrvData->m_pLiterals);

      \param	os wide-char output stream
      \param	szwColumn2Join name of column
      \param    pDbLiterals if not NULL function processes szwColumn2Join with MakeIdName(szwColumn2Join, KLDB::kldbidt_column)
    */
    void AddInnerJoin_AccessMap_Read(
                        std::wostream&      os,
                        const wchar_t*      szwColumn2Join,
                        KLDB::DbLiterals*   pDbLiterals = NULL);

    void AddInnerJoin_AccessMap_Write(
                        std::wostream&      os, 
                        const wchar_t* szwColumn2Join,
                        KLDB::DbLiterals*   pDbLiterals = NULL);
};

#ifdef KLSPLEX_UNDEF_MOD
    #undef KLCS_MODULENAME
#endif


#endif //__KLSPLEX_H__
