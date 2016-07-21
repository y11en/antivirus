#ifndef __NETCFG_DEFS_H__
#define __NETCFG_DEFS_H__

#if defined (NETCFG_EXPORTS)
	#define NETCFGEXPORT __declspec (dllexport)
#else
	#define NETCFGEXPORT __declspec (dllimport)
#endif

#endif