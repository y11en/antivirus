////////////////////////////////////////////////////////////////////
//
//  CLINK.H
//  CLink Main class definitions
//  AVP 3.0 Engine
//  Alexey de Mont de Rique [graf@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs.
//
////////////////////////////////////////////////////////////////////

#ifndef __CLINK_H
#define __CLINK_H

#include <FileFormat/Coff.h>
#include "Workarea.h"


#if defined (BASE_EMULATOR)
	#include "emulator/symboltable.h"
	#include "emulator/wrappers.h"
	typedef struct _CWorkArea CWorkArea;
#endif /* defined BASE_EMULATOR */


#if !defined (BASE_EMULATOR)
class CGlobalNames
{
protected:
	CPtrArray NameArray[16];
public:
	void FreeAll();
	void Compact();

	CGlobalNames(){};
	~CGlobalNames(){FreeAll();};
	DWORD SetName(char* name,DWORD value,void* link, int alloc=0);
	DWORD GetName(char* name, BOOL remove=FALSE);
	void Remove(void* link);
};
#endif /* !defined BASE_EMULATOR */

class CBaseWork;

class CLink
{
protected:
#ifdef LINK_RECORD_SAVE				
	void* Image;
	DWORD ImageSize;
#endif
	void FreeAll();
public:
	DWORD Entry;
	DWORD EntryC;
	~CLink();
	CLink();

#if defined (BASE_EMULATOR)
	int ExecuteEntry(CWorkArea * wa, int Entry, const char * part, const char * name);
#else
	int ExecuteEntry(CWorkArea * wa, int Entry);
#endif /* defined BASE_EMULATOR */

	BOOL Link(BYTE* buf,CBaseWork* bw);
#ifdef LINK_RECORD_SAVE				
	BOOL SaveImage(void* buf, DWORD size);
	void* GetImage(){return Image;};
	DWORD GetImageSize(){return ImageSize;};
	BOOL DeleteImage();
#endif
};


#endif//__CLINK_H
