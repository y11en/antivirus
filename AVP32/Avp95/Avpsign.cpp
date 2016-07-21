#include "stdafx.h"

#define _WINVXD
#include <Sign/c_mem.c>
#include <Sign/c_urand.c>

#include <_avpio.h>
#define SIGN_BUF_LEN 0x2000
#define  SignCloseHandle	AvpCloseHandle
#define  SignReadFile		AvpReadFile
#define  SignCreateFile		AvpCreateFile 
#include <Sign/a_chfile.c>

//extern "C" void CTN_NotInitVariables       (){}
//extern "C" void CTN_LANSNot                (){}
