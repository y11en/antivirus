#include <windows.h>
#include "util.h"
#include "defines.h"
#include <assert.h>

bool FastUni2Ansi(const wchar_t* sUniSrc, char* sAnsiDst, DWORD dwNumOfChars, DWORD dwDstBufCharSize)
{
	cPrStrW sUniSrcCopy;
	if ((LPVOID)sUniSrc == (LPVOID)sAnsiDst)
	{
		// make a copy of source string and change source pointer to it
		sUniSrcCopy = sUniSrc;
		sUniSrc = sUniSrcCopy;
	}
	if (dwNumOfChars == -1)
		dwNumOfChars = wcslen(sUniSrc) + 1;
	assert(dwDstBufCharSize == 0 || dwNumOfChars <= dwDstBufCharSize);
	return (WideCharToMultiByte(CP_ACP, 0, sUniSrc, -1, sAnsiDst, dwNumOfChars, NULL, NULL) != 0);
}


bool FastAnsi2Uni(const char* sAnsiSrc, wchar_t* sUniDst, DWORD dwNumOfChars, DWORD dwDstBufCharSize)
{
	cPrStrA sAnsiSrcCopy;
	if ((LPVOID)sAnsiSrc == (LPVOID)sUniDst)
	{
		// make a copy of source string and change source pointer to it
		sAnsiSrcCopy = (char*)sAnsiSrc;
		sAnsiSrc = sAnsiSrcCopy;
	}
	if (dwNumOfChars == -1)
		dwNumOfChars = strlen(sAnsiSrc) + 1;
	assert(dwDstBufCharSize == 0 || dwNumOfChars <= dwDstBufCharSize);
	return (MultiByteToWideChar(CP_ACP, 0, sAnsiSrc, -1, sUniDst, dwNumOfChars) != 0);
}


