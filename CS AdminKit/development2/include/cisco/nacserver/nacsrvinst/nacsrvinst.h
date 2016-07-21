/*!
 * (C) 2006 Kaspersky Lab
 * 
 * \file	nacsrvinst.h
 * \author	Eugene Rogozhnikov
 * \date	15.11.2006 14:37:07
 * \brief	
 * 
 */

#ifndef __NACSRVINST_H__
#define __NACSRVINST_H__

#include <common/reporter.h>
#include "common/locevents.h"

#ifdef KLCSNACSRV_EXPORTS
# define KLCSNACSRV_DECL KLSTD_DLLEXPORT
#else
# define KLCSNACSRV_DECL KLSTD_DLLIMPORT
#endif

namespace KLNAC
{
	/*class KLSTD_NOVTABLE NacsrvCallback
	{
	public:
		virtual void MarkForRestart() = 0;
		virtual bool IsReiniting() = 0;
	};*/

	class KLSTD_NOVTABLE NACSrvInst
        :   public KLSTD::KLBaseQI
	{
	public:
		virtual void Initialize() = 0;
                    /*KLSTD::Reporter*        pReporter,
                    KLNAC::NacsrvCallback*  pNacsrvCallback) = 0;*/
		virtual KLSTD_NOTHROW void Deinitialize() throw() = 0;
		KLSTD_NOTHROW virtual void OnShuttingDown() throw() = 0;

		virtual bool GetStopFlag() const = 0;
	};

}; // namespace KLNAC

DECLARE_MODULE_INIT_DEINIT2(KLCSNACSRV_DECL, KLNACSRV);


KLCSNACSRV_DECL void KLNACSERVER_CreateServerInstance();
                        /*KLSTD::Reporter * pReporter,
                        KLNAC::NacsrvCallback* pNacsrvCallback = NULL);*/
KLCSNACSRV_DECL KLNAC::NACSrvInst* KLNACSERVER_GetServerInstance();

KLCSNACSRV_DECL void KLNACSERVER_DestroyServerInstance(bool bShuttingDown);

#endif //__NACSRVINST_H__