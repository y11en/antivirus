/* zutil.c -- target dependent utility functions for the compression library
 * Copyright (C) 1995-2002 Jean-loup Gailly.
 * For conditions of distribution and use, see copyright notice in zlib.h 
 */

#include "zutil.h"

#include <memory.h>

//struct internal_state      {int dummy;}; /* for buggy compilers */

/* Defined but never used. Commented out. */
/* static int CheckPointer(z_streamp z, tBYTE* lpData); */
static int AddToList(z_streamp z, tBYTE* lpData, tWORD wSize);
void DeleteList(z_streamp z);
int CreateList(z_streamp z);
static int DeleteFromList(tBYTE* lpData);
static int CompactList(z_streamp z);


#define OUT_DEBUG(a) ///{printf(" %d ", __LINE__);printf(a);printf("\n");}
#define OUT_DEBUGS(a)
#define Printf(a) __asm nop//{printf(" %d ", __LINE__);printf(a);printf("\n");}
#define OUT_HEX(a,b) //{printf(" %d ", __LINE__);printf(a,b);printf("\n");}


/*
const char * ZEXPORT zlibVersion()
{
    return ZLIB_VERSION;
}
*/

#ifdef DEBUG

#  ifndef verbose
#    define verbose 0
#  endif
int z_verbose = verbose;

void z_error (m)
    char *m;
{
    fprintf(stderr, "%s\n", m);
    exit(1);
}
#endif

/* exported to allow conversion of error code to string for compress() and
 * uncompress()
 */
/*
const char * ZEXPORT zError(err)
    int err;
{
    return ERR_MSG(err);
}
*/

#ifndef HAVE_MEMCPY

void zmemcpy(dest, source, len)
    tBYTE* dest;
    const tBYTE* source;
    tINT  len;
{
    if (len == 0) return;
    do {
        *dest++ = *source++; /* ??? to be unrolled */
    } while (--len != 0);
}

int zmemcmp(s1, s2, len)
    const tBYTE* s1;
    const tBYTE* s2;
    tINT  len;
{
    tINT j;

    for (j = 0; j < len; j++) {
        if (s1[j] != s2[j]) return 2*(s1[j] > s2[j])-1;
    }
    return 0;
}

void zmemzero(dest, len)
    tBYTE* dest;
    tINT  len;
{
    if (len == 0) return;
    do {
        *dest++ = 0;  /* ??? to be unrolled */
    } while (--len != 0);
}
#endif


/* Turbo C malloc() does not allow dynamic allocation of 64K bytes
 * and farmalloc(64K) returns a pointer with an offset of 8, so we
 * must fix the pointer. Warning: the pointer must be put back to its
 * original form in order to free it, use zcfree().
 */

#define MAX_PTR 10
/* 10*64K = 640K */

local int next_ptr = 0;

typedef struct ptr_table_s {
    tVOID * org_ptr;
    tVOID * new_ptr;
} ptr_table;

local ptr_table table[MAX_PTR];
/* This table is used to remember the original form of pointers
 * to large buffers (64K). Such pointers are normalized with a zero offset.
 * Since MSDOS is not a preemptive multitasking OS, this table is not
 * protected from concurrent access. This hack doesn't work anyway on
 * a protected system like OS/2. Use Microsoft C instead.
 */

#ifdef _DEBUG
tPTR zcallocDebug (z_streamp z, tPTR opaque,tUINT items,tUINT size,tINT Line)
{
	tPTR ptr;
	
	ptr=(tPTR)DebugNew(z->data, items* size,Line);
	if(ptr)
		memset((tBYTE *)ptr,0,size);
	else
	{
		OUT_DEBUG("Error allocate mem!");
		z->data->error=errNOT_ENOUGH_MEMORY;
	}
	AddToList(z, ptr, (tWORD)size);
	return ptr;
}


tVOID  zcfreeDebug (z_streamp z, tPTR opaque,tPTR ptr, tINT Line)
{
	DebugFree(z->data, ptr,Line);
	DeleteFromList(ptr);
}
#else
tPTR zcalloc (z_streamp z, tPTR opaque,tUINT items,tUINT size)
{
	tPTR ptr;
	
	ptr=(tPTR)_New(z->data,items*size);
	if(ptr)
	{
		memset((tBYTE *)ptr,0,items*size);
		AddToList(z, (tBYTE*)ptr, (tWORD)(items*size));
	}
	else
	{
		OUT_DEBUG("Error allocate mem!");
		z->data->error=errNOT_ENOUGH_MEMORY;
	}
	return ptr;
}


tVOID  zcfree (z_streamp z, tPTR opaque,tPTR ptr)
{
	if(ptr)
	{
		_Delete(z->data, ptr);
		DeleteFromList(ptr);
	}
}
#endif




#ifndef _WIN32_
#pragma pack(1)
#endif
typedef struct _tag_ListData
{
	tBYTE* lpData;
	tWORD       wSize;
}ListData;
#ifndef _WIN32_
#pragma pack()
#endif

static ListData * ListOfList = NULL;
static ListData * LastCheckedItemOfList = NULL;
static tINT dwNumberOfItems;
static tINT iCurFrePos;
#define SIZE_OF_BUNCH 2048


int CreateList(z_streamp z)
{
	
	ListOfList = (ListData *)_New(z->data, sizeof(ListData)*SIZE_OF_BUNCH);
	memset((tBYTE*)ListOfList, 0 , sizeof(ListData)*SIZE_OF_BUNCH);
	if(ListOfList == NULL)
	{
		OUT_DEBUG("Error Create List Of List");
		return 0;
	}
	ListOfList[0].lpData = (void *)0xffffffff;
	ListOfList[0].wSize  = SIZE_OF_BUNCH;
	
	LastCheckedItemOfList = NULL;
	dwNumberOfItems = 1;
	iCurFrePos = 1;
	return 1;
}

static int FoundHole(int iEndPos, int iStartPos)
{
	int i;
	
	for(i=iStartPos; i<iEndPos;i++)
	{
		if(ListOfList[i].lpData == (tBYTE*)0)
			return i;
	}
	return 0;
}

/* Defined but never used. Commented out.*/
/* static int CheckPointer(z_streamp z, tBYTE* lpData) */
/* { */
/* int i; */

/* 	return 0xffff;	 */
/* 	//	dPrintf((" --- CheckPointer dwNumberOfItems %d, lpData %08lX",dwNumberOfItems, lpData)); */
/* 	//	if(Need) */
/* 	//	{ */
/* 	//		Printf (" CheckPointer ListOfList %08lX",ListOfList ); */
/* 	//		for(i=1;i<dwNumberOfItems;i++) */
/* 	//			Printf (" CheckPointer Enter i = %d lpData %08lX, size = %d",i, ListOfList[i].lpData,ListOfList[i].wSize); */
/* 	//	} */
	
/* 	if(LastCheckedItemOfList) */
/* 	{ */
/* 		if((tDWORD)lpData >= (tDWORD)LastCheckedItemOfList->lpData  &&  */
/* 			(tDWORD)lpData < (tDWORD)LastCheckedItemOfList->wSize+(tDWORD)LastCheckedItemOfList->lpData ) */
/* 		{ */
/* 			//		dPrintf((" --- CheckPointer OK (%d)",i)); */
/* 			return 0xffff; */
/* 		} */
/* 	} */
/* 	if((tDWORD)lpData >= (tDWORD)z->z_fixed_mem && (tDWORD)lpData < ((tDWORD)z->z_fixed_mem)+sizeof(inflate_huft)*FIXEDH) */
/* 	{ */
/* 		//		dPrintf((" --- CheckPointer OK (1)")); */
/* 		return 0xffffffff; */
/* 	} */
/* 	if(ListOfList) */
/* 	{ */
/* 		for(i=1; i<dwNumberOfItems;i++) */
/* 		{ */
/* 			if(ListOfList[i].lpData != 0) */
/* 			{ */
/* 				if((tDWORD)lpData >= (tDWORD)ListOfList[i].lpData  && (tDWORD)lpData <  */
/* 					ListOfList[i].wSize+(tDWORD)ListOfList[i].lpData ) */
/* 				{ */
/* 					//					dPrintf((" --- CheckPointer OK (%d)",i)); */
/* 					LastCheckedItemOfList = &ListOfList[i]; */
/* 					return i; */
/* 				} */
/* 			} */
/* 		} */
/* 	} */
/* //	dPrintf((" ++++++++++ Error Check Pointer %08lX %08lX %08lX ++++++++++",lpData, (DWORD)fixed_mem, (DWORD)fixed_mem + sizeof(inflate_huft)*FIXEDH)); */
/* 	return 0; */
/* } */


void DeleteList(z_streamp z)
{

//	dPrintf((" --- DeleteList dwNumberOfItems %d",dwNumberOfItems));

	if(ListOfList)
	{
		_Delete (z->data, (tBYTE*)ListOfList);
	}

	ListOfList = NULL;
	LastCheckedItemOfList = NULL;
	dwNumberOfItems = 0;
	iCurFrePos = 0;
}

static int AddToList(z_streamp z, tBYTE* lpData, tWORD wSize)
{
//	dPrintf((" --- AddToList dwNumberOfItems %d, lpData %08lX",dwNumberOfItems, lpData));
//	dPrintf((" --- AddToList  wSize %X",wSize));

	if(ListOfList == NULL)
	{
		OUT_DEBUG("Error ListOfList not allocated");
		return 0;
	}

	ListOfList[dwNumberOfItems].lpData = lpData;
	ListOfList[dwNumberOfItems].wSize = wSize;

	dwNumberOfItems++;

//	dPrintf((" --- AddToList OK dwNumberOfItems %d",dwNumberOfItems));

	if(dwNumberOfItems == SIZE_OF_BUNCH - 1)
	{
		if(CompactList(z) == 0)
		{
//			dPrintf(("AddToList failed"));
			return 0;
		}
/*		LastItemOfList[0].lpData = New(sizeof(ListData)*SIZE_OF_BUNCH);
		memset((Byte FAR*)LastItemOfList[0].lpData , 0 , sizeof(ListData)*SIZE_OF_BUNCH);

		LastItemOfList[0].wSize  = SIZE_OF_BUNCH;

		dPrintf((" !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!SIZE_OF_BUNCH %d (dwBreakPoint %d) lpData %08lX", dwNumberOfItems, dwBreakPoint, LastItemOfList[0].lpData));

		LastItemOfList = (ListData FAR* ) LastItemOfList[0].lpData;
		dwNumberOfItems++;*/
	}

	return dwNumberOfItems;
}

static int DeleteFromList(tBYTE* lpData)
{
//ListData * ListOfListTmp;
int i;

	if(ListOfList)
	{
		for(i=1; i<dwNumberOfItems;i++ )
		{
/*			if(j == SIZE_OF_BUNCH)
			{
				ListOfListTmp = (ListData FAR*)ListOfListTmp[j-1].lpData;
				j=0;
			}*/
			if((tDWORD)ListOfList[i].lpData == (tDWORD)lpData)
			{
//				dPrintf ((" Delete from List i=%d, dwNumberOfItems = %d",i,dwNumberOfItems));
				if(LastCheckedItemOfList == (ListData *)ListOfList[i].lpData)
					LastCheckedItemOfList = (ListData *)0;
				ListOfList[i].lpData = (tBYTE *)0;
				ListOfList[i].wSize = 0;

//				Printf (" Delete from List lpData %08lX, dwNumberOfItems = %d",ListOfListTmp[i].lpData,dwNumberOfItems);

				if(i+1 == dwNumberOfItems)
				{
					dwNumberOfItems--;
				}
				return i;
			}
		}
	}
	return 0;
}


static int CompactList(z_streamp z)
{
int i;
int iHolePos;
int bRet;
int iTotalCompacted;
	
	bRet = 0;
	iHolePos = 1;
	iTotalCompacted = 0;
	for(i=dwNumberOfItems;i>1;i--)
	{
//		dPrintf ((" CompactList Loop lpData %08lX, i = %d dwNumberOfItems = %d",ListOfList[i].lpData,i, dwNumberOfItems));
		if(ListOfList[i].lpData != NULL)
		{
			iHolePos = FoundHole(i, iHolePos);
			if(iHolePos == 0)
			{
				break;
			}
			else
				bRet = 1;
			ListOfList[iHolePos].lpData = ListOfList[i].lpData;
			ListOfList[iHolePos].wSize  = ListOfList[i].wSize;
			
			ListOfList[i].lpData = (tBYTE*)0;
			ListOfList[i].wSize  = 0;
			iTotalCompacted ++;
		}
		else
		{
			iTotalCompacted++;
			bRet = 1;
		}
	}
	
	if(bRet == 1)
	{
		dwNumberOfItems -= iTotalCompacted;
		LastCheckedItemOfList = NULL;
	}
//	dPrintf(("Compacting Result... dwNumberOfItems %d %d",dwNumberOfItems, bRet));
	
	return bRet;
}


#ifdef _DEBUG
tBYTE* DebugNew(DeflateData* data, tDWORD dwBytes, tINT Line)
{
	tBYTE * lpData;
	
	//	TotalFree++;
	if(CALL_Heap_Alloc(data->hHeap, &lpData, dwBytes)!= errOK)
	{
		data->error=errNOT_ENOUGH_MEMORY;
	}
	//	Printf("Allocated %d (%X) bytes (Line %d) (%d)",dwBytes, lpData,Line,++TotalFree);
	return lpData;
}


tVOID DebugFree(DeflateData* data, tBYTE * lpdata, tINT Line)
{
	if(lpdata==NULL)
	{
		//		Printf("Free %X (Line %d) (---)",lpdata, Line);
		return ;
	}
	//	Printf("Free %X (Line %d) (%d)",lpdata, Line,--TotalFree);
	CALL_Heap_Free(data->hHeap, lpdata);
}
#else
tBYTE* _New(DeflateData* data, tDWORD dwBytes)
{
	tBYTE * lpData;
	
	CALL_Heap_Alloc(data->hHeap,(tPTR*)&lpData, dwBytes);
	if(lpData==NULL)
	{
		data->error=errNOT_ENOUGH_MEMORY;
	}
	return lpData;
}


tVOID _Delete(DeflateData* data, tBYTE * lpdata)
{
	if(lpdata==NULL)
	{
		return ;
	}
	CALL_Heap_Free(data->hHeap, lpdata);
}
#endif

