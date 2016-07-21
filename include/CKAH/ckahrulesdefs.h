#ifndef __CKAHRULES_DEFS_H__
#define __CKAHRULES_DEFS_H__

#if defined (CKAHRULES_EXPORTS)
	#define CKAHRULESEXPORT __declspec (dllexport)
#else
	#define CKAHRULESEXPORT __declspec (dllimport)
#endif

#endif