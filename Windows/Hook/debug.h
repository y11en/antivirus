#ifndef __KLDRV_DEBUG_H_
#define __KLDRV_DEBUG_H_

/*!
*		
*		
*		(C) 2003 Kaspersky Lab 
*		
*		\file	debug.h
*		\brief	управление отладочной информацией
*		
*		\author Sergey Belov, Andrew Sobko
*		\date	12.09.2003 14:09:58
*		
*		Example:	
*		
*		
*		
*/		



#include "defs.h"

#ifdef _DEBUG
	#define __DBG__
#endif

extern CHAR NullStr[];
extern WCHAR NullStrU[];

extern CHAR UnknownStr[];
extern WCHAR UnknownStrW[];

#ifdef __DBG__

typedef enum _DEBUG_INFO_LEVEL
{
	DL_FATAL_ERROR,		//0
	DL_ERROR			,//1
	DL_WARNING			,//2
	DL_IMPORTANT		,//3
	DL_NOTIFY			,//4
	DL_INFO				,//5
	DL_SPAM				 //6
}DEBUG_INFO_LEVEL;

//+ ----------------------------------------------------------------------------------------

#define DCB_NOCAT		0	// +хч ърЄхуюЁшш
#define DCB_SYS			1 // System
#define DCB_FILE		2 // NT FileIO and 9x IFS 
#define DCB_DISK		3 // NT DiskIO and 9x IOS
#define DCB_REG			4	// Registry
#define DCB_R3			5 // Ring3
#define DCB_FWALL		6	// Firewall Lev1
#define DCB_I21			7	// 9x Int21
//
#define DCB_IOCTL		15	//Work with IOCTLs
#define DCB_CLIENT		16	//Work with Clients
#define DCB_EVENT		17	//Work with Events queues
#define DCB_FILTER		18	//Work with Filters queues
#define DCB_INVTH		19	//Work with Invisible Threads
#define DCB_LOG			20	//Logging
#define DCB_DOG			21	//Dogging
#define DCB_NAMEC		22	//Work with Name cache
#define DCB_TSP			23	//Thread subprocessing
#define DCB_OBJC		24	//Clients Obj_Cache
#define DCB_LLDISKIO	25	//Low level disk IO
#define DCB_PARAMS		26	//params

#define DC_NOCAT		(1L << DCB_NOCAT)	
#define DC_DRV			DC_NOCAT
#define DC_SYS			(1L << DCB_SYS)
#define DC_FILE			(1L << DCB_FILE) // NT FileIO and 9x IFS 
#define DC_DISK			(1L << DCB_DISK) 
#define DC_REG			(1L << DCB_REG)
#define DC_R3			(1L << DCB_R3)
#define DC_FWALL		(1L << DCB_FWALL)
#define DC_I21			(1L << DCB_I21)
#define DC_IOCTL		(1L << DCB_IOCTL)
#define DC_CLIENT		(1L << DCB_CLIENT)
#define DC_EVENT		(1L << DCB_EVENT)
#define DC_FILTER		(1L << DCB_FILTER)
#define DC_INVTH		(1L << DCB_INVTH)
#define DC_LOG			(1L << DCB_LOG)
#define DC_DOG			(1L << DCB_DOG)
#define DC_NAMEC		(1L << DCB_NAMEC)
#define DC_TSP			(1L << DCB_TSP)
#define DC_OBJC			(1L << DCB_OBJC)
#define DC_LLDISKIO		(1L << DCB_LLDISKIO)
#define DC_PARAMS		(1L << DCB_PARAMS)

//+ ----------------------------------------------------------------------------------------

	#define DbgBreakPoint() 	__asm int 3
	BOOLEAN __DbPrint(ULONG Category,DEBUG_INFO_LEVEL Level);
	#define DbPrint(Category,Level,arg) {if (__DbPrint(Category,Level)){DbgPrint arg;}}


	extern DEBUG_INFO_LEVEL DebLev;
	extern ULONG DebCat;

	PCHAR DbgGetDevTypeName(ULONG type);
#else
	#define DbgBreakPoint() {}
//	#define DbPrint(Category,Level,arg) {}
	#define DbPrint(Category,Level,arg)
#endif

#endif //__KLDRV_DEBUG_H_
