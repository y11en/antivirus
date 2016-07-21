#ifndef __CKAHCOMM_DEFS_H__
#define __CKAHCOMM_DEFS_H__

#if defined (CKAHCOMM_EXPORTS)
	#define CKAHCOMMEXPORT __declspec (dllexport)
#else
	#define CKAHCOMMEXPORT __declspec (dllimport)
#endif

#endif