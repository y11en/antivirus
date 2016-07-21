/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	common/reporter.h
 * \author	Mikhail Karmazine, Andrew Kazachkov
 * \date	08.09.2003 17:32:47
 * \brief	Интерфейс Reporter - в Windows используется для записи в EventLog
 * 
 */

#ifndef __KLSTD_REPORTER_H__
#define __KLSTD_REPORTER_H__

#include <std/base/kldefs.h>

namespace KLSTD
{
    /*!
	\brief Интерфейс Reporter - в Windows используется для записи в EventLog
    */
	class KLSTD_NOVTABLE Reporter
	{
	public:
		KLSTD_NOTHROW virtual void ReportError(const wchar_t*	szwMessage) throw() =0;
		KLSTD_NOTHROW virtual void ReportError(const char*		szaMessage) throw() =0;

        KLSTD_NOTHROW virtual void ReportInfo(const wchar_t*    szwMessage) throw() =0;
        KLSTD_NOTHROW virtual void ReportInfo(const char*       szaMessage) throw() =0;

        KLSTD_NOTHROW virtual void ReportWarning(const wchar_t* szwMessage) throw() =0;
        KLSTD_NOTHROW virtual void ReportWarning(const char*    szaMessage) throw() =0;
	};
}


/*!
  \brief	Installs/deinstalls global report handler to use by 
            KLSTD_Report* functions 

  \param	pReporter reporter interface
*/
KLCSC_DECL KLSTD_NOTHROW void KLSTD_SetGlobalReporter(
                                    KLSTD::Reporter* pReporter) throw();


/*!
  \brief	Reports an error using global report handler installed by
            KLSTD_SetGlobalReporter.

  \param	szwMessage message to report
  \return	false if there's no global report handler installed
*/
KLCSC_DECL KLSTD_NOTHROW bool KLSTD_ReportError(
                                    const wchar_t* szwMessage) throw();

/*!
  \brief	Reports some info using global report handler installed by
            KLSTD_SetGlobalReporter.

  \param	szwMessage message to report
  \return	false if there's no global report handler installed
*/
KLCSC_DECL KLSTD_NOTHROW bool KLSTD_ReportInfo(
                                    const wchar_t* szwMessage) throw();

/*!
  \brief	Reports a warning using global report handler installed by
            KLSTD_SetGlobalReporter.

  \param	szwMessage message to report
  \return	false if there's no global report handler installed
*/
KLCSC_DECL KLSTD_NOTHROW bool KLSTD_ReportWarning(
                                    const wchar_t* szwMessage) throw();

#endif //__KLSTD_REPORTER_H__
