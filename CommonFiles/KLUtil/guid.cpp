#include "StdAfx.h"
#include "guid.h"

#include <objbase.h>

std::string KLUTIL::MakeGuidString()
{
	GUID guid;
	if (CoCreateGuid (&guid) != S_OK)
		return "";
	char buffer[39];
	sprintf(buffer, "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
		guid.Data1, guid.Data2, guid.Data3, 
		guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
		guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
    return buffer;
}
