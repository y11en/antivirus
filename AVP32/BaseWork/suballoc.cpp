#include "StdAfx.h"
#include "Basework.h"
#include "_printf.h"

#include <new>

BYTE*   _New(long size)
{
#if !defined (_MSC_VER) || (_MSC_VER > 1200 )
  BYTE*   ptr=new (std::nothrow) BYTE[size+4];
#else
	BYTE*   ptr=new BYTE[size+4];
#endif	
#ifdef _DEBUG
	if(!ptr){
		Printf("ERROR: Memory not allocated (%d)",size);
		return 0;
	}
#endif
	
	if(!bdata->AllocRegistry)
		bdata->AllocRegistry=new CPtrArray;
	
	if(bdata->AllocRegistry && ptr){
		bdata->AllocRegistry->Add(ptr);
		*(DWORD*)ptr=size;
		return ptr+4;
	}
	
	if(ptr){ 
		delete ptr;
#ifdef _DEBUG
		Printf("ERROR: AllocRegistry not allocated");
#endif
	}
	return 0;
}

void    _Delete(BYTE* ptr)
{
	if(ptr && bdata->AllocRegistry)
	{
		int i=bdata->AllocRegistry->GetSize();
		ptr-=4;
		while(i--)
		{
			if(ptr==(bdata->AllocRegistry->GetAt(i)))
			{
				bdata->AllocRegistry->RemoveAt(i);
				delete ptr;
				return;
			}
		}
	}
#ifdef _DEBUG
	if(ptr)
		Printf("ERROR: delete wrong pointer (%08X)",ptr);
#endif
}
