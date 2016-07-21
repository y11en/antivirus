/*++

Copyright (c) 2002-2003  Kaspersky Lab

Module Name:

    MiniKavKU.h

Abstract:

    Header file which contains the structures, type definitions,
    constants, global variables and function prototypes that are
    shared between kernel and user mode.

Environment:

    Kernel & user mode

--*/

#ifndef _MINIKAV_API_
#define _MINIKAV_API_

#define MINIKAV_BUILDNUM 1

#define MINIKAV_NAME	L"MINIKAV"
#define MINIKAV_IOCTL_INDEX	0x880

#define MINIKAV_FILE_DEVICE  0x00009010

//----------------------------------------------------------------------------
#define MINIKAV_GET_VERSION		CTL_CODE(0x00009010, MINIKAV_IOCTL_INDEX + 0, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define MINIKAV_START_FILTER	CTL_CODE(0x00009010, MINIKAV_IOCTL_INDEX + 1, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define MINIKAV_STOP_FILTER		CTL_CODE(0x00009010, MINIKAV_IOCTL_INDEX + 2, METHOD_BUFFERED, FILE_ANY_ACCESS)
//-------------------------------------------------------------------------------

#define MJ_MINIKAV_ACCESS_FILE		0x0001

#endif //_MINIKAV_API_
