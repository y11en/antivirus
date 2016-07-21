#include "servfuncs.h"
#include <windows.h>
#include "memory.h"

unsigned long GetSubstringPosBM(__int8* Str, unsigned long sourcse_strlen, __int8* SubStr, unsigned long SubStrLen)
{
	unsigned __int8 Table[0x100];
	int i;
	__int8 *PStr,*PEndStr,*PSubStr,*PEndSubStr;
	__int8 *PStrCur;
	if ((SubStrLen==0) || (sourcse_strlen<SubStrLen)) 
		return -1;
	FillMemory(Table, sizeof(Table), SubStrLen);
	for(i = SubStrLen - 2; i >= 0; i--)
	{
		if (Table[SubStr[i]] == SubStrLen)
			Table[SubStr[i]] = (__int8) (SubStrLen - i - 1);
	}
	PSubStr = PEndSubStr = SubStr+SubStrLen - 1;
	PStrCur = PStr=Str + SubStrLen - 1;
	PEndStr = Str + sourcse_strlen;
	do {
		if((*PStr) == (*PSubStr))
		{
			if (PSubStr == SubStr) 
				return PStr - Str;
			PStr--;
			PSubStr--;
		} 
		else 
		{
			PSubStr = PEndSubStr;
			PStr = PStrCur += Table[(*PStrCur)];
		}
	} while (PStr<PEndStr);
	
	return -1;
}
