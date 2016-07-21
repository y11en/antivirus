// objstack_test.cpp : Defines the entry point for the console application.
//

#include "StdAfx.h"
#include <memory>
#include <klava/klav_objstack.h>
#include <klava/klav_utils.h>
#include "enforcement.h"

KLAV_Malloc_Alloc g_malloc;

void exist_first_block()
{
	KLAV_ObjStack_Impl s (& g_malloc);
	void* ptr = s.last_block();
	ENFORCE(ptr);
	ENFORCE(s.block_size (ptr)==0);
	void* ptr2 = s.alloc(20);
	ENFORCE(ptr2 != ptr); 
	s.clear(0);
	ENFORCE(ptr == s.last_block ());
	ENFORCE(s.block_size (ptr)==0);
}

void constr_destr()
{
	{
		KLAV_ObjStack_Impl s (& g_malloc);
	}
}

void alloc0()
{
	KLAV_ObjStack_Impl s (& g_malloc);
	void* ptr0 = s.last_block ();
	void* ptr1 = s.alloc (0);
	void* ptr2 = s.alloc (0);
	ENFORCE(ptr0!=ptr1);
	ENFORCE(ptr1!=ptr2);
}

void alignment()
{
	KLAV_ObjStack_Impl s (& g_malloc);
	void* ptr[20];
	ptr[0] = s.last_block ();
	
	for (int i = 1; i<20; ++i){
		ptr[i] = s.alloc(i);
	}
	
	for (int j = 0; j<20; ++j){
		size_t addr = (char*)ptr[j] - (char*)0;
		ENFORCE(addr%(sizeof(void *)*2) == 0);
	}
	
}

void newchunk()
{
	KLAV_ObjStack_Impl s (& g_malloc, 100);
	char* ptr1 = (char*) s.alloc(50);
	char* ptr2 = (char*) s.alloc(50);
	ENFORCE(abs(ptr2-ptr1)>54);
}



int main(int argc, char* argv[])
{
	try{
		exist_first_block();
		constr_destr();
		alloc0();
		alignment();
		newchunk();
		printf("test ok!\n");
	}
	catch (const std::exception& e)
	{
		printf(e.what());
	}
	return 0;
}

