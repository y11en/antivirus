#include <Prague/prague.h>
#include <Prague/iface/i_root.h>

#include "static.h"

#include <BaseWork/SpcAlloc.h>

void* SpcAlloc(unsigned size, unsigned type)
{
	void* ptr=NULL;
	if(size)
	{
		switch(type)
		{
		case SPCALLOC_GLOBAL:
			if(g_root)
				g_root->heapAlloc(&ptr,size);
			break;
		default:
			ptr=new char[size];
			break;
		}
	}
	return ptr;
}

void  SpcFree(void* ptr, unsigned type)
{
	if(ptr)
	{
		switch(type)
		{
		case SPCALLOC_GLOBAL:
			if(g_root)
				g_root->heapFree(ptr);
			break;
		default:
			delete[] ptr;
			break;
		}
	}
}
