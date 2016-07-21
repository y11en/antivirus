// testing.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"

#include "database.h"


#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <stdio.h>

#include "istreamstub.h"

#ifdef USE_LOCAL_MEMORY_MANAGER
    #include "../../client-filter/librules/memmngr.h"
    CLocalMemoryManager g_LocalMemoryManager;
#endif//USE_LOCAL_MEMORY_MANAGER


#ifdef USE_CUSTOM_HEAP
    #include <win32-filter/include/mem_heap.h>
#endif

int main(int argc, char* argv[])
{
//MessageBox(0, "0", 0, 0);
#ifdef USE_LOCAL_MEMORY_MANAGER
    LocalMemoryManager_Init(&g_LocalMemoryManager, 4*1024, 100*1024, 1, 2*1024);
//    LocalMemoryManager_Init(&g_LocalMemoryManager, 1000*1024, 1000*1024, 13, 20000);
#endif//USE_LOCAL_MEMORY_MANAGER
    
#ifdef USE_CUSTOM_HEAP
    InitHeapIndex   ();
    CreateHeap();
#endif

    IStreamStub isGSG, isPDB;
    
    if ( 
        -1 == isGSG.open("1.gsg", O_RDONLY|O_BINARY) ||
        -1 == isPDB.open("1.pdb", O_RDONLY|O_BINARY ) )
    {
        printf ("can't open file");
        return 1;
    }
    CDataBase   db;
//MessageBox(0, "1", 0, 0);
    db.Init(&isGSG, &isPDB);
//MessageBox(0, "2", 0, 0);

	int n = 0, n2 = 0, n20, n3 = 0;
	for (int i = 0; i < g_LocalMemoryManager.m_nBlocksCount; i ++)
	{
		n20 = g_LocalMemoryManager.m_pMemBlocks[i].m_nReserved - g_LocalMemoryManager.m_pMemBlocks[i].m_nSize ;
		if (n20 >= 0)
			n2 += n20;
		else
			n20 = 0;

		n += g_LocalMemoryManager.m_pMemBlocks[i].m_nSize;
		n3 += g_LocalMemoryManager.m_pMemBlocks[i].m_nReserved;
	}
    
    printf ("finish");
    return 0;
}

