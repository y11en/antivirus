#ifndef __CKAHUM_DEFS_H__
#define __CKAHUM_DEFS_H__

#if defined (CKAHUM_EXPORTS)
	#define CKAHUMEXPORT __declspec (dllexport)
#else
	#define CKAHUMEXPORT __declspec (dllimport)
#endif

#endif