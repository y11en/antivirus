#include "StdAfx.h"
#include "except.h"
#include <stdlib.h>

using namespace std;
using namespace KLUTIL;

string CExcept::GetSrcInfoString() const throw()
{
	string strRes;
	const CExSrcInfo& srcInfo = GetSrcInfo();
	strRes  = srcInfo.file;
	if (srcInfo.line > 0)
	{
		char buf[128];
		_itoa(srcInfo.line, buf, 10);
		strRes += string("(") + buf + ')';
	}

	if (!srcInfo.func.empty())
	{
		strRes += string("[") + srcInfo.func + ']';
	}
	return strRes;
}

