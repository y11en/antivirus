#ifndef __SendDIOC_h__
#define __SendDIOC_h__


#ifdef __cplusplus
extern "C"  {
#endif
	
NTSTATUS SendDIOC(PUNICODE_STRING uDeviceName, ULONG IoControlCode,PVOID InputBuffer,ULONG InputBufferLength,PVOID OutputBuffer,ULONG OutputBufferLength);

#ifdef __cplusplus
}
#endif

#endif