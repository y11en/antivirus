#include <windows.h>
#include <assert.h>

#include "../al_file_mem.h"
#include "../al_heap_win.h"

void test_cpp(alHeap* pHeap, alFile* pFile)
{
	void* pMem = pHeap->Realloc(0, 10);
	assert(pMem);
	assert(pHeap->Free(pMem));
	assert(pFile->Write("test", 5, 0));
}

extern "C" void test_c(alHeap* pHeap, alFile* pFile);

void main()
{
	alAutoRelease<alHeapWin32*> pHeap = new_alHeapWin32();
	alAutoRelease<alFileMem*> pFile = new_alFileMem(pHeap, false);
	//assert(pFile->CreateFileA("al_test.dat", GENERIC_WRITE | GENERIC_READ, 0, 0, CREATE_ALWAYS, FILE_FLAG_DELETE_ON_CLOSE, 0));
	pFile->
	test_cpp(pHeap, pFile);
	test_c(pHeap, pFile);
}
