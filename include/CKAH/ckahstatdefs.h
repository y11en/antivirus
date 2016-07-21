#ifndef __CKAHSTAT_DEFS_H__
#define __CKAHSTAT_DEFS_H__

#if defined (CKAHSTAT_EXPORTS)
	#define CKAHSTATEXPORT __declspec (dllexport)
#else
	#define CKAHSTATEXPORT __declspec (dllimport)
#endif

#endif