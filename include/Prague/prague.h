/*-----------------02.05.99 09:31-------------------
 * Project Prague                                 *
 * Subproject Common Prague header                *
 * Author Andrew Andy Petrovitch Graf             *
 * Copyright (c) Kaspersky Lab                    *
 * Purpose Prague API                             *
 *                                                *
--------------------------------------------------*/

#ifndef __prague_h
#define __prague_h

#ifndef PRAGUE
	#define PRAGUE
#endif

/* common headers */
#include <Prague/pr_compl.h>  // compiler dependent macro definitions
#include <Prague/pr_types.h>
#include <Prague/pr_obj.h>
#include <Prague/pr_int.h>

/* constant definition headers */
#include <Prague/pr_iid.h>
#include <Prague/pr_pid.h>
#include <Prague/pr_vid.h>

/* errors definition header */
#include <Prague/pr_err.h>

#include <Prague/pr_prop.h>
#if !defined( __RPC__ )
#include <Prague/pr_sys.h>
#include <Prague/pr_reg.h>
#include <Prague/pr_vtbl.h>
#endif

#include <Prague/pr_dbg.h>

/* message definition header */
#include <Prague/pr_msg.h>

/* plugin registering headers */
//#include "pr_plug.h"


#endif
