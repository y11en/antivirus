/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	nag_autostart.h
 * \author	Andrew Kazachkov
 * \date	12.07.2004 14:03:22
 * \brief	
 * 
 */

#ifndef __KLNAG_AUTOSTART_H__
#define __KLNAG_AUTOSTART_H__

#include <common/reporter.h>

KLSTD_NOTHROW void KLNAG_DoReplicateAutoStart(
                            const std::wstring& wstrProduct,
                            const std::wstring& wstrVersion,
                            KLSTD::Reporter*    pReporter,
                            long                lSsTimeout) throw();

#endif //__KLNAG_AUTOSTART_H__
