#define IOCTL_AVP95_ENABLE          CTL_CODE(FILE_DEVICE_UNKNOWN, 0x40, METHOD_BUFFERED, FILE_ANY_ACCESS)
//InBuf: DWORD Enable; InBufSize 4; (BOOL)
#define IOCTL_AVP95_GET_MFLAGS      CTL_CODE(FILE_DEVICE_UNKNOWN, 0x41, METHOD_BUFFERED, FILE_ANY_ACCESS)
//OutBuf: DWORD Enable; OutBufSize 4;
#define IOCTL_AVP95_SET_MFLAGS      CTL_CODE(FILE_DEVICE_UNKNOWN, 0x42, METHOD_BUFFERED, FILE_ANY_ACCESS)
//InBuf: DWORD MFlags; InBufSize 4;
#define IOCTL_AVP95_GET_STAT        CTL_CODE(FILE_DEVICE_UNKNOWN, 0x43, METHOD_BUFFERED, FILE_ANY_ACCESS)
//OutBuf: AVP95Stat stat; OutBufSize sizeof(AVP95Stat);
#define IOCTL_AVP95_STATIC_MFLAGS   CTL_CODE(FILE_DEVICE_UNKNOWN, 0x44, METHOD_BUFFERED, FILE_ANY_ACCESS)
//InBuf: DWORD StaticMFlags; InBufSize 4; (BOOL)
#define IOCTL_AVP95_LOAD_BASE       CTL_CODE(FILE_DEVICE_UNKNOWN, 0x45, METHOD_BUFFERED, FILE_ANY_ACCESS)
//InBuf: char SetName[]; InBufSize strlen(SetName);
#define IOCTL_AVP95_PROCESS_OBJECT  CTL_CODE(FILE_DEVICE_UNKNOWN, 0x46, METHOD_BUFFERED, FILE_ANY_ACCESS)
//InBuf:  AVPScanObject so; InBufSize sizeof(AVPScanObject);
#define IOCTL_AVP95_REGISTER_CALLBACK  CTL_CODE(FILE_DEVICE_UNKNOWN, 0x47, METHOD_BUFFERED, FILE_ANY_ACCESS)
//Not Implemented Yet!
#define IOCTL_AVP95_SET_ALERT_TEXT  CTL_CODE(FILE_DEVICE_UNKNOWN, 0x48, METHOD_BUFFERED, FILE_ANY_ACCESS)
//InBuf: char* OEMstring; InBufSize strlen(OEMstring);
//Should have two %s!!!! should be OEM!!! Like this:
//"AVP for Windows95\n\n\n\n\n\nObject    %s\n Infected by virus:  %s\n\nTry to disinfect?"
#define IOCTL_AVP95_GET_BASE_INFO   CTL_CODE(FILE_DEVICE_UNKNOWN, 0x49, METHOD_BUFFERED, FILE_ANY_ACCESS)
//OutBuf: AVP95BaseInfo baseinfo; OutBufSize sizeof(AVP95BaseInfo);

typedef struct stat_{
        DWORD Enable;
        DWORD Objects;
        DWORD Infected;
        DWORD Disinfected;
        DWORD Deleted;
        DWORD Skipped;
        DWORD MFlags;
        DWORD StaticMFlags;
        char*  LastObjectName;
        char*  LastInfectedObjectName;
        char*  LastVirusName;
}AVP95Stat;

typedef struct baseinfo_{
        DWORD NumberOfVirNames;
        DWORD NumberOfRecords;
        DWORD NumberOfBases;
		WORD  LastUpdateYear;
		BYTE  LastUpdateMonth;
		BYTE  LastUpdateDay;
		DWORD LastUpdateTime;
        char*  SetName;
}AVP95BaseInfo;
