/*!
*		
*		
*		(C) 2002 Kaspersky Lab 
*		
*		\file	_avpff.h
*		\brief	
*		
*		\author Victor Matioshenkov
*		\date	6/15/2005 11:23:44 AM
*		
*		Example:	
*		
*		
*		
*/		

#ifndef __AVPFF_H__
#define __AVPFF_H__

#include "_avpio.h"

#ifdef AVPIOSTATIC
#   undef  F_INIT
#   define F_INIT(x) = NULL
#else
#   define F_INIT(x) 
#endif


typedef HANDLE (WINAPI t_AvpFindFirstFile)(
										 LPCTSTR lpFileFolder,         // file folder
										 LPTSTR  lpFoundFileName,			 // found file
										 DWORD   dwFileNameSize        // found file name size
									 );

typedef BOOL (WINAPI t_AvpFindNextFile)(
									HANDLE	hFindFile,              // search handle 
									LPTSTR  lpFoundFileName,			  // found file
									DWORD   dwFileNameSize          // found file name size
								);


typedef BOOL (WINAPI t_AvpFindClose)(
									HANDLE	hFindFile              // search handle 
								); 

declare( FindFirstFile )
declare( FindNextFile )
declare( FindClose )

EXTERN BOOL LoadFF(void);


#endif //__AVPFF_H__