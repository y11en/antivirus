////////////////////////////////////////////////////////////////////
//
//  AVPIOCTL.H
//  AVP Monitor: control codes for FSAVP.SYS and AVP95.VXD driver
//  AVP project
//  Alexey de Mont de Rique [graf@avp.ru], Kaspersky Labs. 1997
//  Copyright (c) Kaspersky Labs.
//
////////////////////////////////////////////////////////////////////

#ifndef __AVPIOCTL_H
#define __AVPIOCTL_H

#define AVP_IOCTL_Version 0x00030002

#define IOCTL_AVP_GET_VERSION     CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
//OutBuf: DWORD Version; OutBufSize 4;
#define IOCTL_AVP_GET_MFLAGS      CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)
//OutBuf: DWORD Enable; OutBufSize 4;
#define IOCTL_AVP_SET_MFLAGS      CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, METHOD_BUFFERED, FILE_ANY_ACCESS)
//InBuf: DWORD MFlags; InBufSize 4;
#define IOCTL_AVP_GET_STAT        CTL_CODE(FILE_DEVICE_UNKNOWN, 0x803, METHOD_BUFFERED, FILE_ANY_ACCESS)
//OutBuf: AVPStat stat; OutBufSize sizeof(AVPStat);
#define IOCTL_AVP_STATIC_MFLAGS   CTL_CODE(FILE_DEVICE_UNKNOWN, 0x804, METHOD_BUFFERED, FILE_ANY_ACCESS)
//InBuf: DWORD StaticMFlags; InBufSize 4; (BOOL)
#define IOCTL_AVP_LOAD_BASE       CTL_CODE(FILE_DEVICE_UNKNOWN, 0x805, METHOD_BUFFERED, FILE_ANY_ACCESS)
//InBuf: char *SetName; InBufSize strlen(SetName);
#define IOCTL_AVP_PROCESS_OBJECT  CTL_CODE(FILE_DEVICE_UNKNOWN, 0x806, METHOD_BUFFERED, FILE_ANY_ACCESS)
//InBuf:  AVPScanObject* so; InBufSize sizeof(AVPScanObject);
//OutBuf:  AVPScanObject* so; OutBufSize sizeof(AVPScanObject);
#define IOCTL_AVP_SET_ALERT_TEXT  CTL_CODE(FILE_DEVICE_UNKNOWN, 0x808, METHOD_BUFFERED, FILE_ANY_ACCESS)
//Obsolete!!!! Use IOCTL_AVP_SET_MESSAGE_TEXT
#define IOCTL_AVP_GET_BASE_INFO   CTL_CODE(FILE_DEVICE_UNKNOWN, 0x809, METHOD_BUFFERED, FILE_ANY_ACCESS)
//OutBuf: AVPBaseInfo baseinfo; OutBufSize sizeof(AVPBaseInfo);
#define IOCTL_AVP_ENABLE          CTL_CODE(FILE_DEVICE_UNKNOWN, 0x80A, METHOD_BUFFERED, FILE_ANY_ACCESS)
//InBuf: DWORD Enable; InBufSize 4; (BOOL)
#define IOCTL_AVP_ADD_MASK        CTL_CODE(FILE_DEVICE_UNKNOWN, 0x80B, METHOD_BUFFERED, FILE_ANY_ACCESS)
//InBuf: char* Mask; InBufSize strlen(Mask);
#define IOCTL_AVP_SET_MESSAGE_TEXT  CTL_CODE(FILE_DEVICE_UNKNOWN, 0x80C, METHOD_BUFFERED, FILE_ANY_ACCESS)
//InBuf: {DWORD NoOfMEss,LPSZ OEMstring}; InBufSize 4;
//No 0:Infection:  Should have two %s!!!! Like this:
//"AVP Monitor\n\n\n\n\n\nObject    %s\n Infected by virus:  %s\n\nTry to disinfect?"
//No 1:Suspicion:  Should have two %s!!!! Like this:
//"AVP Monitor\n\n\n\n\n\nObject    %s\n Suspicted for virus:  %s\n\nDeny Access?"
//No 2:Watning:  Should have two %s!!!! Like this:
//"AVP Monitor\n\n\n\n\n\nObject    %s\n Infected by new variant of virus:  %s\n\nDeny Access?"
#define IOCTL_AVP_RELOAD_BASE       CTL_CODE(FILE_DEVICE_UNKNOWN, 0x80D, METHOD_BUFFERED, FILE_ANY_ACCESS)
//NULL---------
//#define IOCTL_AVP_SET_REPORT_FILE   CTL_CODE(FILE_DEVICE_UNKNOWN, 0x80E, METHOD_BUFFERED, FILE_ANY_ACCESS)
//InBuf:  AVPReport rep; InBufSize sizeof(AVPReport);

//REMOVED!!!

#define IOCTL_AVP_LIMITCOMPOUNDSIZE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x80F, METHOD_BUFFERED, FILE_ANY_ACCESS)
//InBuf: DWORD MaxSize; InBufSize 4;
#define IOCTL_AVP_REGISTERTHREAD	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x810, METHOD_BUFFERED, FILE_ANY_ACCESS)
//InBuf: DWORD PETHREAD; InBufSize 4;
#define IOCTL_AVP_UNREGISTERTHREAD	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x811, METHOD_BUFFERED, FILE_ANY_ACCESS)
//InBuf: DWORD PETHREAD; InBufSize 4;
#define IOCTL_AVP_MAKE_FULL_NAME	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x812, METHOD_BUFFERED, FILE_ANY_ACCESS)
// Makes full name of a virus
//InBuf: CHAR* VirNamePtr; InBufSize 4;
//OutBuff: BYTE[]Buffer to receive full name ;OutBufSize - max size of buffer;
#define IOCTL_AVP_ADD_EXCLUDE_MASK        CTL_CODE(FILE_DEVICE_UNKNOWN, 0x821, METHOD_BUFFERED, FILE_ANY_ACCESS)
//InBuf: char* Mask; InBufSize strlen(Mask);


#define IOCTL_AVP_CALLBACK_CREATE       CTL_CODE(FILE_DEVICE_UNKNOWN, 0x840, METHOD_BUFFERED, FILE_ANY_ACCESS)
//InBuf: 95: DWORD APC_CallbackProcPtr; InBufSize 4
//InBuf: NT: DWORD EventHandle; InBufSize 4
//OutBuf: DWORD CallbackHandle
#define IOCTL_AVP_CALLBACK_SYNC_DATA    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x841, METHOD_BUFFERED, FILE_ANY_ACCESS)
//InBuf:  95 ONLY: DWORD DataPtr; InBufSize 4 //data to release(from APC argument)
//OutBuf: NT ONLY: AVPScanObject; OutBufSize =sizeof(AVPScanObject)
#define IOCTL_AVP_CALLBACK_DELETE       CTL_CODE(FILE_DEVICE_UNKNOWN, 0x842, METHOD_BUFFERED, FILE_ANY_ACCESS)
//InBuf:  DWORD CallBackHandle; InBufSize 4

#define IOCTL_AVP_CHECK_FILE_INTEGRITY  CTL_CODE(FILE_DEVICE_UNKNOWN, 0x850, METHOD_BUFFERED, FILE_ANY_ACCESS)
//InBuf: char *FileName; InBufSize strlen(SetName);
//OutBuf: DWORD HRESULT


#pragma pack(push, pack_ioctl)
#pragma pack(1)

typedef struct stat_{
        DWORD Enable;
        DWORD Objects;
        DWORD Infected;
        DWORD Disinfected;
        DWORD Deleted;
        DWORD Skipped;
        DWORD MFlags;
        DWORD StaticMFlags;
        DWORD Warnings;
        DWORD Suspicions;
        char  LastObjectName[0x200];
        char  LastInfectedObjectName[0x200];
        char  LastVirusName[0x200];
}AVPStat;

typedef struct baseinfo_{
        DWORD NumberOfVirNames;
        DWORD NumberOfRecords;
        DWORD NumberOfBases;
		WORD  LastUpdateYear;
		BYTE  LastUpdateMonth;
		BYTE  LastUpdateDay;
		DWORD LastUpdateTime;
        DWORD Status;
        char  SetName[0x200];
}AVPBaseInfo;

#define BI_ERROR   0x00000001
#define BI_LOADING 0x00000002
#define BI_LOADED  0x00000004

#define MESS_TEXT_MAX 10

#pragma pack()
#pragma pack(pop, pack_ioctl)

#endif//__AVPIOCTL_H
