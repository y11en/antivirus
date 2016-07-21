#include "SpcAlloc.h"

void* SpcAlloc(unsigned size, unsigned type)
{
	switch(type)
	{
	case SPCALLOC_GLOBAL:
	default:
		return new char[size];
	}
}

void  SpcFree(void* ptr, unsigned type)
{
	switch(type)
	{
	case SPCALLOC_GLOBAL:
	default:
		delete[] ptr;
	}
}
