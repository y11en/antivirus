#ifndef _KDPRINT_CLI_H
#define _KDPRINT_CLI_H
//------------------------------------------------------------------------------------------------------------------------
typedef
VOID (WINAPI *KDPRINT_CALLBACK)(PCHAR Str2Output);

BOOL Client_InitKDPrint(KDPRINT_CALLBACK pCallbacks);
void Client_UnInitKDPrint();
BOOL Client_InitKDPrintDevice(PCHAR ControlDeviceName);
//------------------------------------------------------------------------------------------------------------------------
#endif