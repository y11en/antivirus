#ifndef __specfpr__
#define __specfpr__

#include "../osspec.h"

void SpecialFileProcessing();
NTSTATUS SpecFileRequest(PSPECFILEFUNC_REQUEST pSpecRequest, ULONG ReqSize, PVOID OutputBuffer, ULONG OutputBufferLength, ULONG* pOutSize);

#endif // __specfpr__