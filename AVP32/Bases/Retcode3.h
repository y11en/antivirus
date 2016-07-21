////////////////////////////////////////////////////////////////////
//
//  RETCODE.H
//  Engine Dynamic Linkage API, Public defines for decode,cure and jump proc
//  AVP 3.0 Engine
//  Alexey de Mont de Rique [graf@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs.
//
////////////////////////////////////////////////////////////////////

#ifndef __RETCODE_H
#define __RETCODE_H

// Public defines -------------------------------------

// the special procedures must return one of the values:

#define R_CLEAN					0		// file is not infected, stop detectio
#define R_DETECT				1		// file is infected
#define R_PREDETECT				2		// continue detection procedure
#define R_CURE					3		// file is cured
#define R_PRECURE				4		// processing before standard method
#define R_DELETE				5		// file is deleted
#define R_WARNING				6		// file is possible infected
#define R_ERROR					7		// i/o error
#define R_MEMORY				8		// no free memory
#define R_CANCEL				9		// testing/curing cancelled by user
#define R_SKIP					10		// testing/curing skipped by user
#define R_USER					11		// display the message from Message_St
#define R_CURE_FAIL				12		// cure procedure failed
#define R_FAIL					R_CURE_FAIL // just for lazy Eugene
#define R_NOMESS				13		// internal return code
#define	R_SUSPIC				14		// suspicious object

#endif//__RETCODE_H
