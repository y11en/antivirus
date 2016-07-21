#include <windows.h>
#include <assert.h>

#include "../al_heap.h"
#include "../al_file.h"

void test_c(alHeap* pHeap, alFile* pFile)
{
	char buff[5];
	assert(pFile->vtbl->SeekRead(pFile, 0, buff, 5, 0));
	assert(0 == strcmp(buff, "test"));
}