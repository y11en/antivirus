////////////////////////////////////////////////////////////////////
//
//  AVP_MSG.H
//  AVP 3.0 Callback message definitions
//  AVP project
//  Alexey de Mont de Rique [graf@avp.ru], Kaspersky Labs. 1997
//  Copyright (c) Kaspersky Labs.
//
////////////////////////////////////////////////////////////////////

#ifndef __AVP_MSG_H
#define __AVP_MSG_H

#define AVP_MSGSTRING "AVP32 Message System"

//         wParam  value                          description of lParam           description of return value

#define AVP_CALLBACK                     0x0100
#define AVP_CALLBACK_PUT_STRING          0x0101 // const char* string
#define AVP_CALLBACK_ROTATE              0x0102 // -                               0-continue processing of object; 1-stop processing
#define AVP_CALLBACK_ROTATE_OFF          0x0103 // -
#define AVP_CALLBACK_MB_OK               0x0104 // const char* message             
#define AVP_CALLBACK_MB_OKCANCEL         0x0105 // const char* message             IDOK,IDCANCEL
#define AVP_CALLBACK_MB_YESNO            0x0106 // const char* message             IDYES,IDNO,IDCANCEL
#define AVP_CALLBACK_ASK_CURE            0x0107 // ScanObject*                     0-skip; 1-try to disinfect; 2-delete; 
#define AVP_CALLBACK_LOAD_PROGRESS       0x0108 // LOAD_PROGRESS_STRUCT*
#define AVP_CALLBACK_LINK_NAME           0x0109 // const char* sourcename DEBUG!
#define AVP_CALLBACK_ASK_DELETE          0x010A // ScanObject* (For DELETE method) 2-delete; other-skip
#define AVP_CALLBACK_GET_SECTOR_IMAGE    0x010B // -  (For overwriting sectors)    BYTE* SectorImage
#define AVP_CALLBACK_LOAD_RECORD_NAME    0x0110 // const char* string
#define AVP_CALLBACK_LOAD_BASE           0x0111 // LOAD_PROGRESS_STRUCT*  0-load, 1-skip this base.
#define AVP_CALLBACK_LOAD_BASE_DONE      0x0112 // LOAD_PROGRESS_STRUCT* 
#define AVP_CALLBACK_ASK_CURE_MEMORY     0x0113 // ScanObject*           !!!!!!!!! 0-disinfect; 1-disinfect&clear detection 2-skip; 

#define AVP_CALLBACK_OBJ_NAME            0x010C // char* name (!!!! if it returns 17 - engine will skip file)

#define AVP_CALLBACK_OBJECT_DONE         0x0200 // ScanObject*
#define AVP_CALLBACK_OBJECT_MESS         0x0201 // ScanObject* (ScanObject->VirName is *message)
#define AVP_CALLBACK_OBJECT_DETECT       0x0202 // ScanObject*
#define AVP_CALLBACK_OBJECT_CURE         0x0203 // ScanObject*
#define AVP_CALLBACK_OBJECT_WARNING      0x0204 // ScanObject*
#define AVP_CALLBACK_OBJECT_SUSP         0x0205 // ScanObject*
#define AVP_CALLBACK_OBJECT_PACKED       0x0206 // ScanObject*
#define AVP_CALLBACK_OBJECT_ARCHIVE      0x0207 // ScanObject*
#define AVP_CALLBACK_OBJECT_DELETE       0x0208 // ScanObject*
#define AVP_CALLBACK_OBJECT_UNKNOWN      0x0209 // ScanObject*
#define AVP_CALLBACK_OBJECT_CURE_FAIL    0x020A // ScanObject*
#define AVP_CALLBACK_OBJECT_BEGIN		 0x020B // ScanObject* 
#define AVP_CALLBACK_OBJECT_ARCHIVE_DONE 0x020C // ScanObject*
#define AVP_CALLBACK_OBJECT_ARCHIVE_NAME 0x020D // ScanObject* (ScanObject->VirName is *name)(if it returns nonzero - engine will skip file)
#define AVP_CALLBACK_OBJECT_PACKED_DONE  0x0240 // ScanObject*
#define AVP_CALLBACK_OBJECT_SKIP_REQEST  0x0241 // ScanObject* return:bitfield 1 - skip AV, 2- skip extract.
#define AVP_CALLBACK_OBJECT_ZERO_HEADER  0x0242 // ScanObject* 
#define AVP_CALLBACK_OBJECT_GET_EMUL_FLAGS  0x0243 // ScanObject* return:dword flags
#define AVP_CALLBACK_OBJECT_FALSE_ALARM  0x0244 // ScanObject*

#define AVP_CALLBACK_WA_SUSP             0x0210 // WorkArea*
#define AVP_CALLBACK_WA_CORRUPT          0x0211 // WorkArea*
#define AVP_CALLBACK_WA_ARCHIVE_NAME     0x0212 // WorkArea*
#define AVP_CALLBACK_WA_PACKED_NAME      0x0213 // WorkArea*
#define AVP_CALLBACK_WA_PACKED           0x0214 // WorkArea*
#define AVP_CALLBACK_WA_ARCHIVE          0x0215 // WorkArea*

#define AVP_CALLBACK_WA_PROCESS_ARCHIVE  0x030E // WorkArea* (transfer to external extractors)(if it returns nonzero - engine will not call internal extractors)
#define AVP_CALLBACK_WA_PROCESS_OBJECT   0x030F // WorkArea* (transfer to external scaner)(if it returns nonzero - engine will not call internal processing)

#define AVP_CALLBACK_ERROR_FILE_OPEN     0x0400 // char* filename
#define AVP_CALLBACK_ERROR_SET           0x0401 // char* filename
#define AVP_CALLBACK_ERROR_BASE          0x0402 // char* filename
#define AVP_CALLBACK_ERROR_KERNEL_PROC   0x0403 // char* procname
#define AVP_CALLBACK_ERROR_LINK          0x0405 // LOAD_PROGRESS_STRUCT*
#define AVP_CALLBACK_ERROR_SYMBOL_DUP    0x0406 // char* symbolname
#define AVP_CALLBACK_ERROR_SYMBOL        0x0407 // char* symbolname
#define AVP_CALLBACK_ERROR_FIXUPP        0x0408 // int fixuppcode

#define AVP_CALLBACK_CHECK_FILE_INTEGRITY           0x0800 // char* filename (returns: 0 - OK, other - failed)
#define AVP_CALLBACK_FILE_INTEGRITY_FAILED          0x0801 // char* filename (returns: 0 - continue, other - stop operation)
#define AVP_CALLBACK_SETFILE_INTEGRITY_FAILED       0x0802 // char* filename (returns: 0 - continue, other - stop operation)

#define AVP_CALLBACK_FILE_OPEN_FAILED_TRY_AGAIN     0x0806// char* filename (returns: 0 - skip, other - try again)
#define AVP_CALLBACK_SETFILE_OPEN_FAILED_TRY_AGAIN  0x0807// char* filename (returns: 0 - skip, other - try again)

#define AVP_INTERNAL                     0x1000
#define AVP_INTERNAL_CHECK_DIR           0x1001
#define AVP_INTERNAL_MESSAGE             0x1002
#define AVP_INTERNAL_SHOW                0x1003
#define AVP_INTERNAL_NOTIFY              0x1004

#define AVP_CALLBACK_SAVE_PAGES          0x1011 // WorkArea*
#define AVP_CALLBACK_SAVE_PACKED         0x1012 // WorkArea*
#define AVP_CALLBACK_SAVE_ARCHIVED       0x1013 // WorkArea*

#define AVP_CALLBACK_SAVE_MARKS          0x1014 // SAVE_MARK_STRUCT*

#define AVP_CALLBACK_WA_RECORD           0x2000
#define AVP_CALLBACK_IS_PRAGUE_HERE      0x3000
#define AVP_CALLBACK_LINK_BEGIN          0x4000 //Void* function code
#define AVP_CALLBACK_LINK_DONE           0x4001 //Void* function code

#define AVP_CALLBACK_REGISTER_PASSWORD   0x5000 //char* password

typedef struct LOAD_PROGRESS_S{
        DWORD curLength;   // Length of cutrrent AV database file in BYTES
        DWORD curPos;      // Position in cutrrent AV database file in BYTES
        DWORD setLength;   // Total Length of all AV database in the set file in BYTES (From build 115)
        DWORD setPos;      // Count of BYTES that was loaded at this moment (From build 115)
        DWORD virCount;    // Count of Virus Names that was loaded at this moment
        DWORD recCount;    // Count of records that was loaded at this moment
        char* curName;     // FullPathName of current AV database file.
        char* setName;     // FullPathName of SET file.
        DWORD RecCountArray[8]; //  Count of records that was loaded at this moment by RecordTypes (From build 115)

        DWORD NumberOfBases;  // (From build 117)
		WORD  LastUpdateYear;
		BYTE  LastUpdateMonth;
		BYTE  LastUpdateDay;
		DWORD LastUpdateTime;
		WORD  CurBaseYear;    // (From build 3.0.132.2101)
		BYTE  CurBaseMonth;
		BYTE  CurBaseDay;
		DWORD CurBaseTime;
		BYTE  Verdicts;
}LOAD_PROGRESS_STRUCT;

typedef struct SAVE_MARK_STRUCT_S{
	char* Name;
	DWORD SumNumber; // 0 - first 1- second
	BOOL  LinkPresence;
	DWORD Offset;
	DWORD Length;
	BYTE* Mark;
}SAVE_MARK_STRUCT;

#endif//__AVP_MSG_H
