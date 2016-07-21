#ifndef __TRAFMONUTIL_H__
#define __TRAFMONUTIL_H__

#include <string>

std::string GetBinBufferTraceStringShort(const BYTE *pDataBuffer, DWORD dwDataBufferSize);
std::string GetBinBufferTraceString(const BYTE *pDataBuffer, DWORD dwDataBufferSize);

#endif