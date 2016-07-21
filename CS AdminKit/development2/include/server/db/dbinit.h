/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	dbinit.h
 * \author	Andrew Kazachkov
 * \date	22.06.2005 11:17:19
 * \brief	
 * 
 */

#ifndef __KL_DBINIT_H__
#define __KL_DBINIT_H__

#include <std/thr/locks.h>

DECLARE_MODULE_INIT_DEINIT2(KLCSSRV_DECL, KLDB)

namespace KLDB
{
    extern KLSTD::LockCount    g_lckModule;
}

#endif //__KL_DBINIT_H__
