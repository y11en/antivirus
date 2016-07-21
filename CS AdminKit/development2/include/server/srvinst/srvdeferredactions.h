/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	srvdeferredactions.h
 * \author	Andrew Kazachkov
 * \date	12.12.2006 15:06:30
 * \brief	
 * 
 */


#ifndef __SRVDEFERREDACTIONS_H__
#define __SRVDEFERREDACTIONS_H__

namespace KLSRV
{
    class KLSTD_NOVTABLE SrvDeferredActionHandler
        :   public KLSTD::KLBaseQI
    {
    public:
        /*!
          \brief	Callas deferred handler

          \param	pCon        IN  db connection, non-NULL, not in transaction
          \param	nExecOrder  IN  execution oreder
          \param	nModule     IN  module id
          \param	nActionCode IN  action code (module-specific)
          \param	nPar1       IN  integer parameter 1
          \param	nPar2       IN  integer parameter 2
          \param	wstrPar1    IN  string parameter 1
          \param	wstrPar2    IN  string parameter 2
          \return	true if success. in this case action record 
                    will be deleted and action wouldn't be called any more
        */
        virtual bool OnDeferredAction(
                            KLDB::DbConnectionPtr   pCon,
                            int                     nExecOrder,
                            int                     nModule, 
                            int                     nActionCode,
                            int                     nPar1,
                            int                     nPar2,
                            const wchar_t*          wstrPar1,
                            const wchar_t*          wstrPar2) = 0;
    };


    class KLSTD_NOVTABLE SrvDeferredActions
        :   public KLSTD::KLBaseQI
    {
    public:
        /*!
          \brief	Not implementedd yet !!!

          \param	nModule  IN 
          \param	pHandler IN 
        */
        virtual void SetDeferredActionHandler(
                            int                 nModule,
                            SrvDeferredActionHandler*  pHandler) = 0;

        /*!
          \brief	Calls registered deferred actions. Not implemented yet !!!

          \param	pCon    IN connection pointer
          \param	nExecOrder IN execution order, cannot be 0
          \exception throws exception if pCon is invalid (or in transaction) or if in case of some fatal error
        */
        virtual void CallRegisteredDeferredActions(
                            KLDB::DbConnectionPtr   pCon,
                            int                     nExecOrder) = 0;

        /*!
          \brief	Calls deferred actions immediately using specified interface

          \param	pCon
          \param	nExecOrder  IN execution order, 0 means "for all levels"
          \param	nModule     IN module id (cannot be 0)
          \param	pHandler IN pointer to SrvDeferredActions interface, if NULL registered interface is used
          \exception throws exception in case of error
        */
        virtual void CallDeferredActions(
                            KLDB::DbConnectionPtr       pCon,
                            int                         nExecOrder,
                            int                         nModule,
                            SrvDeferredActionHandler*   pHandler) = 0;
        
        /*!
          \brief	Adds deferred action

          \param	pCon        IN  db connection
          \param	nExecOrder  IN  execution order
          \param	nModule     IN  module code
          \param	nActionCode IN  action code
          \param	nPar1       IN  
          \param	nPar2       IN  
          \param	wstrPar1    IN  
          \param	wstrPar2    IN  
          \return	id of acrion
        */
        virtual long AddDeferredAction(
                            KLDB::DbConnectionPtr   pCon,
                            int                     nExecOrder,
                            int                     nModule,
                            int                     nActionCode,
                            int                     nPar1,
                            int                     nPar2,
                            const wchar_t*          wstrPar1,
                            const wchar_t*          wstrPar2) = 0;

        /*!
          \brief	RemoveDeferredAction

          \param	pCon    IN  db connection
          \param	lId     IN  action id returned by AddDeferredAction
        */
        virtual void RemoveDeferredAction(
                            KLDB::DbConnectionPtr   pCon,
                            long                    lId) = 0;

    };

    class CAutoDeferredAction
    {
    public:
        CAutoDeferredAction(
                    KLSRV::SrvDeferredActions*  p,
                    KLDB::DbConnectionPtr       pCon,
                    int                         nExecOrder,
                    int                         nModule,
                    int                         nActionCode,
                    int                         nPar1 = 0,
                    int                         nPar2 = 0,
                    const wchar_t*              wstrPar1 = L"",
                    const wchar_t*              wstrPar2 = L"");

        CAutoDeferredAction(
                    KLSTD::KLBaseQI* p,
                    KLDB::DbConnectionPtr       pCon,
                    int                         nExecOrder,
                    int                         nModule,
                    int                         nActionCode,
                    int                         nPar1 = 0,
                    int                         nPar2 = 0,
                    const wchar_t*              wstrPar1 = L"",
                    const wchar_t*              wstrPar2 = L"");

        ~CAutoDeferredAction();

        void Success();
    protected:
        void Init(  int                         nExecOrder,
                    int                         nModule,
                    int                         nActionCode,
                    int                         nPar1,
                    int                         nPar2,
                    const wchar_t*              wstrPar1,
                    const wchar_t*              wstrPar2);
    protected:
        KLSTD::CAutoPtr<KLSRV::SrvDeferredActions>  m_pSrvDeferredActions;
        KLDB::DbConnectionPtr                       m_pCon;
        bool                                        m_bSucess;
        long                                        m_lId;
    };

};

#endif //__SRVDEFERREDACTIONS_H__
