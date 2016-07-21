#if defined(__unix__)
#include "StdAfx.h"

LPCSTR GetDateTimeStr(CRootItem* pRoot, cDateTime& prDt, tString& str, DTT_TYPE eType, bool bSeconds, bool bLocal, LPCSTR strEstDaysFmt)
{
	return 0;
}

void ConvertPath(PATHOP_TYPE eType, LPCSTR strText, tString& strRes)
{
	strRes = strText;
}

tCHAR * CompressPath(const tCHAR *pStr, int len, bool bExclusive, tCHAR *pDst, DWORD dwDstLen)
{
	if( dwDstLen >= 3 )
		strcpy(pDst, ":)");
	else if( dwDstLen )
		pDst[0] = 0;
	return pDst;
}

#endif //__unix__
