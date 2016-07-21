// KLEAPI.h
// KL Scan Engine API 
//
// Alexey de Mont de Rique [graf@kaspersky.com]
// 21.08.01

#ifndef __KLEAPI_h__
#define __KLEAPI_h__

#ifdef __cplusplus
extern "C"  {
#endif
#pragma pack(push, 1)
	
#define KLE_API_Version 0x00010006      //There is GetAPIVersion() function in API to get this value.
#define KLE_CALL __cdecl

typedef void                 tVOID;
typedef unsigned char        tBYTE;
typedef unsigned short       tWORD;
typedef unsigned long        tDWORD;
typedef int                  tBOOL;
typedef char                 tCHAR;
typedef unsigned short       tWCHAR;
typedef long                 tERROR;
	
//Device names for different OS to get API FunctionTable pointer.
#define KLE_DEVICE_NAME_NT_L L"\\Device\\KLE_SYSDevice0"
#define KLE_DEVICE_NAME_9X "KLE_VXD"
#define KLE_IOCTL_GET_FUNCTION_TABLE  CTL_CODE(FILE_DEVICE_UNKNOWN, 0x901, METHOD_BUFFERED, FILE_ANY_ACCESS)
//OutBuf: KLE_FUNCTION_TABLE* TablePtr; OutBufSize 4;

struct KLE_FUNCTION_TABLE; //KLE API Functin Table - all calls go through this table.
struct LOAD_PROGRESS;      //Structure to get information about loaging/loaded antiviral bases
struct KLE_SCAN_OBJECT;    //Non published Structure-handle for all scan requests.

//KLE Errors: All functions in KLE API return tERROR. All other returns go through parameters.
#define KLE_ERR_OK                             0x00000000
#define KLE_ERR                                0x80000000
#define KLE_ERR_UNKNOWN                        0x80000001
#define KLE_ERR_NOT_IMPLEMENTED                0x80000002
#define KLE_ERR_BAD_PARAMETER                  0x80000003
#define KLE_ERR_NO_FILE                        0x80000004
#define KLE_ERR_BASES_NOT_LOADED               0x80000005
#define KLE_ERR_MEM_ALLOC                      0x80000006
#define KLE_ERR_BAD_PROPID                     0x80000007
#define KLE_ERR_BUFFER_TOO_SMALL               0x80000008
#define KLE_ERR_PROP_NOT_ALLOCATED             0x80000009
#define KLE_ERR_PROP_READONLY                  0x8000000A
#define KLE_ERR_SYNCRONYSATION                 0x8000000B

#define KLE_SUCCESS(x)                 (!(KLE_ERR & (x)))
#define KLE_FAILED(x)                 (!!(KLE_ERR & (x)))

//DWORD  Properties of the SCAN_OBJECT. There are GetPropertyDword/SetPropertyDword funtions to work with.
#define KLE_PROP_CALLBACK_PTR_DWORD            0x00000001  //RW Set/get callback function KLE_F_CALLBACK
#define KLE_PROP_STATUS_DWORD                  0x00000002  //RO
#define KLE_PROP_OPTIONS_DWORD                 0x00000003  //RW
#define KLE_PROP_ARC_LAYER_DWORD               0x00000004  //RO
#define KLE_PROP_PAC_LAYER_DWORD               0x00000005  //RO
#define KLE_PROP_USER_DATA_DWORD               0x00000006  //RW

//STRING Properties of the SCAN_OBJECT. There are GetPropertyString/SetPropertyString funtions to work with.
#define KLE_PROP_TEMP_PATH_STRING              0x00001001
#define KLE_PROP_OBJECT_NAME_STRING            0x00001002
#define KLE_PROP_VIRUS_NAME_STRING             0x00001003
#define KLE_PROP_ARCHIVE_NAME_STRING           0x00001004

//Options: Bitfield in KLE_PROP_OPTIONS_DWORD
#define KLE_OPTION_CLEAN_VIRUS                 0x00002000
#define KLE_OPTION_ARCHIVE                     0x00020000
#define KLE_OPTION_SELF_EXRACT_ARCHIVE         0x00000400
#define KLE_OPTION_MAILPLAIN                   0x00400000
#define KLE_OPTION_MAILBASES                   0x00800000
#define KLE_OPTION_HEURISTICS                  0x00140000
#define KLE_OPTION_ARC_STOP_IF_DETECT          0x00000020

//Status: Bitfield in KLE_PROP_STATUS_DWORD
#define KLE_STAT_INFECTED                      0x00000001
#define KLE_STAT_DISINFECTED                   0x00000002
#define KLE_STAT_SUSPICIOUS                    0x00000004
#define KLE_STAT_SUSPIC_MODIF                  0x00000008
#define KLE_STAT_DELETE                        0x00000100
#define KLE_STAT_DELETED                       0x00000200
#define KLE_STAT_CURE_DELETE                   0x00800000
#define KLE_STAT_DISINFECT_FAILED              0x00000010
#define KLE_STAT_CORRUPTED                     0x00000020
#define KLE_STAT_ENCRYPTED                     0x00000400
#define KLE_STAT_PASSWORD_PROTECTED            0x00000800
#define KLE_STAT_NOT_A_PROGRAMM                0x00000080
#define KLE_STAT_INTERNAL_ERROR                0x00000040
#define KLE_STAT_DISK_WRITE_PROTECTED          0x00001000
#define KLE_STAT_DISK_NOT_READY                0x00002000
#define KLE_STAT_DISK_OUT_OF_SPACE             0x00004000
#define KLE_STAT_IO_ERROR                      0x00008000
#define KLE_STAT_NOT_OPEN                      0x00010000
#define KLE_STAT_OUT_OF_MEMORY                 0x00020000
#define KLE_STAT_KERNEL_FAULT                  0x00200000
#define KLE_STAT_PACKED                        0x02000000
#define KLE_STAT_ARCHIVE                       0x04000000
#define KLE_STAT_REPORT_DONE                   0x01000000
#define KLE_STAT_STOPPED                       0x10000000

//Callback Messages
#define KLE_CALLBACK_YIELD                     0x00000000 //engine call this one frequently during processing.
#define KLE_CALLBACK_OBJECT_BEGIN              0x00001000 
#define KLE_CALLBACK_OBJECT_DONE               0x00001001  
#define KLE_CALLBACK_OBJECT_INFECTED           0x00001002
#define KLE_CALLBACK_OBJECT_DISINFECTED        0x00001003
#define KLE_CALLBACK_OBJECT_DELETED            0x00001004
#define KLE_CALLBACK_ARCHIVE_BEGIN             0x00002000
#define KLE_CALLBACK_ARCHIVE_DONE              0x00002001
#define KLE_CALLBACK_ARCHIVE_NEXT_NAME         0x00002002
#define KLE_CALLBACK_BASE_LOAD_BEGIN           0x00004000 //param == LOAD_PROGRESS*
#define KLE_CALLBACK_BASE_LOAD_DONE            0x00004001 //
#define KLE_CALLBACK_BASE_LOAD_PROGRESS        0x00004002 //
#define KLE_CALLBACK_BASE_CORRUPTED            0x00004003 //
#define KLE_CALLBACK_BASE_ABSENT               0x00004004 //

// KLE API Function Prototypes
// Callback function is to be implementd by user. Engine will call this function during processing.
typedef tERROR (KLE_CALL KLE_F_CALLBACK)( KLE_SCAN_OBJECT* so, tDWORD message, tDWORD param, tCHAR* debugStr);  
// Definition of functions in Function Table.
typedef tERROR (KLE_CALL KLE_F_GET_API_VERSION)( tDWORD* dwVersion);
typedef tERROR (KLE_CALL KLE_F_LOAD_BASES)( tVOID* setname, tBOOL bUnicode, KLE_F_CALLBACK* fnCallback);
typedef tERROR (KLE_CALL KLE_F_GET_BASES_INFO)( LOAD_PROGRESS* pLP);
typedef tERROR (KLE_CALL KLE_F_INIT_SCAN_OBJECT)( KLE_SCAN_OBJECT** pso);
typedef tERROR (KLE_CALL KLE_F_CLEANUP_SCAN_OBJECT)( KLE_SCAN_OBJECT* so);
typedef tERROR (KLE_CALL KLE_F_SCAN_FILE) ( KLE_SCAN_OBJECT* so, tVOID* filename, tBOOL bUnicode);
typedef tERROR (KLE_CALL KLE_F_CLEAN_FILE) ( KLE_SCAN_OBJECT* so, tVOID* filename, tBOOL bUnicode);
typedef tERROR (KLE_CALL KLE_F_SCAN_BOOT) ( KLE_SCAN_OBJECT* so, tBYTE drive);
typedef tERROR (KLE_CALL KLE_F_CLEAN_BOOT) ( KLE_SCAN_OBJECT* so, tBYTE drive);
typedef tERROR (KLE_CALL KLE_F_SCAN_MBR) ( KLE_SCAN_OBJECT* so, tBYTE disk);
typedef tERROR (KLE_CALL KLE_F_CLEAN_MBR) ( KLE_SCAN_OBJECT* so, tBYTE disk);
typedef tERROR (KLE_CALL KLE_F_CLEAN_MEM) ( KLE_SCAN_OBJECT* so );
typedef tERROR (KLE_CALL KLE_F_STOP_SCAN) ( KLE_SCAN_OBJECT* so, tERROR reason);
typedef tERROR (KLE_CALL KLE_F_GET_PROP_DWORD)( KLE_SCAN_OBJECT* so, tDWORD propid, tDWORD* data);
typedef tERROR (KLE_CALL KLE_F_SET_PROP_DWORD)( KLE_SCAN_OBJECT* so, tDWORD propid, tDWORD data);
typedef tERROR (KLE_CALL KLE_F_GET_PROP_STRING)( KLE_SCAN_OBJECT* so, tDWORD propid, tVOID* buffer, tBOOL bUnicode, tDWORD* size);
typedef tERROR (KLE_CALL KLE_F_SET_PROP_STRING)( KLE_SCAN_OBJECT* so, tDWORD propid, tVOID* buffer, tBOOL bUnicode);

typedef struct KLE_FUNCTION_TABLE{
	tDWORD                        StructureSize;
	KLE_F_GET_API_VERSION       * GetAPIVersion;
	KLE_F_LOAD_BASES            * LoadBases;
	KLE_F_GET_BASES_INFO        * GetBasesInfo;
	KLE_F_INIT_SCAN_OBJECT      * InitScanObject;	
	KLE_F_CLEANUP_SCAN_OBJECT   * CleanupScanObject;
	KLE_F_SCAN_FILE             * ScanFile;
	KLE_F_CLEAN_FILE            * CleanFile;
	KLE_F_SCAN_BOOT             * ScanBoot;
	KLE_F_CLEAN_BOOT            * CleanBoot;
	KLE_F_SCAN_MBR              * ScanMBR;
	KLE_F_CLEAN_MBR             * CleanMBR;
	KLE_F_CLEAN_MEM             * CleanMem;
	KLE_F_STOP_SCAN             * StopScan;     //can be called in any time from any thread.
	KLE_F_GET_PROP_DWORD        * GetPropDword; //should be called from the scan thread. For ex. during callback.
	KLE_F_SET_PROP_DWORD        * SetPropDword; //should be called from the scan thread. For ex. during callback.
	KLE_F_GET_PROP_STRING       * GetPropString; //should be called from the scan thread. For ex. during callback.
	KLE_F_SET_PROP_STRING       * SetPropString; //should be called from the scan thread. For ex. during callback.
}KLE_FUNCTION_TABLE;

typedef struct LOAD_PROGRESS{
	tDWORD curLength;   // Length of cutrrent AV database file in BYTES
	tDWORD curPos;      // Position in cutrrent AV database file in BYTES
	tDWORD setLength;   // Total Length of all AV database in the set file in BYTES (From build 115)
	tDWORD setPos;      // Count of BYTES that was loaded at this moment (From build 115)
	tDWORD virCount;    // Count of Virus Names that was loaded at this moment
	tDWORD recCount;    // Count of records that was loaded at this moment
	tCHAR* curName;     // FullPathName of current AV database file.
	tCHAR* setName;     // FullPathName of SET file.
	tDWORD RecCountArray[8]; //  Count of records that was loaded at this moment by RecordTypes (From build 115)
	
	tDWORD NumberOfBases;  // (From build 117)
	tWORD  LastUpdateYear;
	tBYTE  LastUpdateMonth;
	tBYTE  LastUpdateDay;
	tDWORD LastUpdateTime;
	tWORD  CurBaseYear;    // (From build 3.0.132.2101)
	tBYTE  CurBaseMonth;
	tBYTE  CurBaseDay;
	tDWORD CurBaseTime;
}LOAD_PROGRESS;


#pragma pack(pop)
#ifdef __cplusplus
}
#endif
#endif




