#ifndef __KERNEL_H
#define __KERNEL_H
#include "..\\typedef.h"
#include "..\\..\\Common.Fil\\ScanApi\scanobj.h"

extern char scan_goes; // продолжать ли процесс сканирования
                       // 1 - продолжать 0 - не продолжать
extern char scan_end;  // показывает как завершился процесс сканирования
                       // 0 - без прерывания пользователем  1 - прерван пользователем
//#define ERR_MEM           1        // Not enough memory
//#define ERR_OPN           2        // File not found
//#define ERR_DSK           3        // Reading failed

// Type_Of_Object, used to define the type of object that is scanned
// ScanObject.type=Type_Of_Object, see "struct ScanObject"
// when scanning a file, set "ScanObject.type=_FILE;"
// when scanning a boot sector (HDD or floppy),
// set "ScanObject.type=_BOOT;"
// when scanning hard drive MBR, set "ScanObject.type=_MBR;"
// to scan the system memory, set "ScanObject.type=_MEMORY;"
//#define _FILE             0x00
//#define _MBR              0x11
//#define _BOOT             0x12
//#define _MEMORY           0x20

// Scan_Mode (in which mode search for the files to scan them)
// Put that value to "ScanObject.mode" before _PROCESSOBJECT call.
//#define SM_SMART          0x00   // Smart mask

// SHELL search for all files and passes them to KERNEL, KERNEL then
// scan the file if it has "executable" extension (COM, EXE, OVL, SYS,
// BAT..., Note: there are no DOC/DOT/XLS) or executable file format:
// MZ Exe stamp, FFFFFFFFh driver header, OLE2 stamp, or file looks as
// COM file (JMP or CALL at the file header, and so on).

//#define SM_PROGRAMS       0x01 // Scan only files with executable
                               // extensions

// the same as SM_SMART, but KERNEL chekcs file extension only (Note:
// Word documents and Excel tables will not be scanned)

//#define SM_ALL_FILES      0x02 // Scan all files
//#define SM_USER_MASK      0x03 // Scan files by user mask(s)

// SHELL search for all files and passes them to KERNEL, KERNEL then
// compares filename extension with the list of masks, and scans this file
// if its filename extension exists in the list. Wildcards are allowed.

// Note: if SHELL is run in SM_USER_MASK mode, it has to fill
// the list of extensions beforehand by _REGISTEREXT Call. To clean
// that list SHELL has to call _REGISTEREXT with DS:SI -> NULL (0h)

// Calls SHELL -> KERNEL
// Note: when KERNEL is processing SHELL's request, it may call
// PUT_STRING, MB_OK, MB_YN and LOAD_PROGRESS CallBack functions to
// display error messages, database filenames and Load_Progress
// information (percents).

//#define _STARTUP          0     // Init KERNEL, see source for more details
                                // Stack: unsigned short 'DO',
                                //        unsigned long address of CallBack handler
                                // Return: AX - KERNEL version (300h)

//#define _SETFLAGS         2     // Pass Global_Flags to KERNEL
                                // Stack: unsigned short Global_Flags
                                // Return: nothing

//#define _LOADBASE         3     // Load all databases from SET file
                                // DS:SI far ptr SET file full name
                                // Return: 0-ok,1-load not complete

//#define _UNLOADBASE       4     // Unload databases from the system memory
                                // Return: 0-ok,1-error

//#define _PROCESSOBJECT    5     // Scan object for viruses/[**check CRC]
                                // DS:SI far ptr to struct ScanObject
                                // Return: nothing

// Note: before return KERNEL fills "ScanObject.r_flags" field

// While processing this call KERNEL calls CallBacks:
// all except LOAD_PROGRESS.

// Example: if virus detected, KERNEL calls CallBack ASK_CURE with
// string No. 66 ("infected by"). SHELL display the message, and user
// replies "disinfect all". SHELL returns R_CURE. Then KERNEL detects a
// virus in next file and again calls SHELL by using ASK_CURE. SHELL does
// not display message box (user has stroke "disinfect all") and
// immediately returns R_CURE

//#define _REGISTEREXT      6     // Register user defined mask (not extension)
                                // to search files. Summary length of
                                // all masks < 80 bytes
                                // DS:SI ptr to Mask (ASCII with 00h)
                                // if DS:SI points to 00h, KERNEL cleans
                                // all masks
                                // Returns: 0-ok,1-error
/*
// Note: use this Call only in mode "search by user defined mask(s)".
// SHELL searches for all files in specified path(s) and passes all
// file names to KERNEL (by _PROCESSOBJECT call).

// Example: user input is "A:\*.COM C:\BIN\A???????.E?? C:\BIN\*.DLL"
// SHELL calls
//  - _REGISTEREXT with DS:SI -> 0
//  - _REGISTEREXT with DS:SI -> *.COM
// then scans for all files in A:\*.* and passes their names to KERNEL
//  - _REGISTEREXT with DS:SI -> 0
//  - _REGISTEREXT with DS:SI -> A???????.E??
//  - _REGISTEREXT with DS:SI -> *.DLL
// then scans for all files in C:\BIN\*.* and passes their names to KERNEL

#define _SHUTDOWN         8     // Free system memory reserved for KERNEL.
                                // KERNEL closes all TMP files, releases
                                // DOS and XMS memory and erases its own code

#define _REDUNDANT_ON     9     // Unable redundans mode
                                // return: 0 - OK, 1 - low memory

#define _REDUNDANT_OFF    10    // Disable redundans mode

// CallBack List
// this list contains the KERNEL -> SHELL commands
// KERNEL calls SHELL by using CallBack with register AX=Command. Other
// registers (BX,CX,DX,SI,DI,DS) may contain other info (data and pointers)

// CallBack may force SHELL to display a message. See messages list and
// their numbers in AVP.LNG

#define _GET_CRC          11

#define _OUT_NAME         0x130 // write FullFileName to scan window,
                                // is used while scanning files.

#define _PRINT_NAME       0x131 // write FullFileName to report file,
                                // is used while scanning infected files.

// Note: if SHELL has to write all scanned file names to log (infected and
// clean) is should write to LOG only the names of clean files. Infected
// names are written to log by _PRINT_NAME CallBack

#define _MAKE_NAME        0x132 // make name

#define _TYPE_OK          0x134 // display and write to log
                                // "FileName not infected",
                                // is used while scanning files.

//#define _TYPE_STR_WITH_OK 0x135 // display string No. BL (see AVP.LNG)
                                // followed by "OK" (object is not infected)
                                // used while scanning sectors.

#define _SEND_PARAM       0x136 // save DS:DX for use in _PUT_STRING,
                                // _PRINT_STRING, _MB_*, _ASK_*
                                // DS:DX points to a string:
                                // virus name, "XMS ver. xxx", "HDD#",
                                // "A:", path name and so on, see AVP.LNG

#define _OUT_PACK         0x137 // EQU print_name_,print_string_(74),out_name

#define _OUT_ARCHIVE      0x138 // EQU print_name_,print_string_(75),out_name

#define _PUT_STRING       0x101 // Print string #BL [with DS:DX]
                                // BL - str. no. (see AVP.LNG)

#define _ROTATE           0x102 // Rotate |/-\|/-\... (if needed)
                                // return: 0 - continue, 1 - abort

#define _ROTATE_OFF       0x103 // Disable rotating

#define _MB_OK            0x104 // Display MessageBox with [OK], BL - as 101h

#define _MB_YESNO         0x106 // - // - // - [YES] [NO], BL - as 101h
                                // return: 0 - YES, 1 - NO

#define _ASK_CURE         0x107 // - // - // - [CURE] [DELETE] [SKIP]
                                // request to disinfect the virus
                                // BX - as 101h
                                // Returns: R_CURE - cure, R_DELETE - delete,
                                // R_SKIP - skip

#define _ASK_DELETE       0x108 // - // - // - [DELETE] [SKIP]
                                // request to disinfect the virus
                                // BX - as 101h
                                // Returns: R_DELETE - delete, R_SKIP - skip
                                // Note: this CallBack is performed only if
                                // a virus is not disinfectable.

#define _LOAD_PROGRESS    0x109 // Database is being loaded,
                                // DS:SI - struct (see below)

#define _OPEN_TMP_FILE    0x120 // Open TMP file for read/write
                                // return: BX - handle of ok,
                                // Carry flag if error

#define _DELETE_TMP_FILE  0x121 // BX - handle

#define _PRINTF           0x116 // write ASCII str at DS:SI into window and log
// Note1: this calls exists only while 1) disinfecting complex viruses -
// decrypting hard drive infected by OneHalf (str="decrypting cylinder %d"),
// decrypting RDA-based viruses ("decoding... %d")
// 2) extra info such as ICARO test file: KERNEL displays: "this is NOT
// the virus, but test file"
// 3) debug information while debugging databases

// Note2: these calls go directly from AVP databases and cannot be
// translated to other languages

#define _INCREMENT        0x118 // see INC CallBack below
                                // DI = function

#define _GET_SECTOR_IMAGE 0x122 // BL = Sub type:
                                //  while scanning MBR:
                                //   10h - MBR sector
                                //   80h - active boot sector pointed by MBR
                                //   81h - nonactive boot sector pointed by MBR
                                //  while scanning boot sector:
                                //   20h - floppy boot
                                //   40h - HD boot
                                // return DX:AX - far ptr to disk image

// When AVP Kernel cannot disinfect the boot or MBR infector and has to
// overwrite infected sector with a standard one, AVP Kernel calls Shell
// for standard images to write them to disk sector.
// While overwriting a sector AVP Kernel does not change system data
// within this sector - AVP Kernel does not change Disk Partition Table (MBR)
// and BPB (Boot sector). AVP Kernel overwrites only standard system
// loader routine.

// All CallBacks that begins with PRINT write info into
// both LogFile and ScanWindow
// If SHELL is not going to write to log, it should just to ignore these
// calls

// While scanning not infected file KERNEL does:
//  calls _OUT_NAME, scan file, calls _TYPE_OK, returns.
// While scanning infected file KERNEL does:
//  calls _OUT_NAME, scan file and detects a virus, calls _ASK_CURE,
//  calls _ASK_DELETE, then calls _PRINT_NAME, _PRINT_STRING (virus name)
//  returns.
// While scanning "warning/susp." file KERNEL does:
//  calls _OUT_NAME, scan file detects warning/susp, then calls _PRINT_NAME,
//  _PRINT_STRING (virus name), returns.
*/
// INC CallBack
#define INCL_SECTORS       1 // number of scanned sector objects
#define INCL_FILES         2 // total number of scanned files
#define INCL_FOLDER        3 // number of scanned directories
//#define INCL_DISKS       4 // number of scanned disks
#define INCL_ARCHIVES      5 // number of archives
#define INCL_PACKED        6 // number of packed files
#define INCL_KNOWNVIR      7 // number of
#define INCL_VIRBODIES     8 // number of
#define INCL_DISINFECT     9 // number of disinfected files
#define INCL_DELETED       10// number of deleted files
#define INCL_WARNING       11// number of warnings
#define INCL_SUSPICION     12// number of suspicious files
#define INCL_CORRUPTED     13// number of
#define INCL_IOERRORS      14// number of input/output errors

// Return codes
//#define R_CURE            3
//#define R_DELETE          5
//#define R_SKIP            10

extern _AVPScanObject ScanObject;

//extern unsigned char      MBRData[512];
//extern unsigned char      BootData[512];

extern unsigned char *make_name_ptr;          // pointer to _MAKE_NAME data
//extern unsigned char *send_param_ptr;         // pointer to _SEND_PARAM data
//extern unsigned long      kbytes;                 // INC_KBYTES data

//void           KeepDS();                           // Data (not function!) to keep DS
//void  far      CallBackHandler();                  // CallBack handler

// Calls SHELL -> KERNEL
//unsigned short StartUp();                          // Init KERNEL
//void           SetFlags(unsigned short m_flags);   // Pass Flags to KERNEL
//void scan_now_(void);
//void           ProcessObject();                    // Scan object (file/sector/memory)
//void           RegisterExt(unsigned char *mask);   // Pass file mask to KERNEL
//void           ShutDown();                         // Shut down KERNEL
//unsigned char  RedundantOn();                      // Unable redundant mode
//void           RedundantOff();                     // Disable redundant mode
//unsigned char  CheckCRC();                         // CRC check

//#define DRIVE_UNKNOWN     0
//#define DRIVE_NO_ROOT_DIR 1
//#define DRIVE_REMOVABLE   2
//#define DRIVE_FIXED       3
//#define DRIVE_REMOTE      4
//#define DRIVE_CDROM       5
//#define DRIVE_RAMDISK     6

#define IDOK            1
#define IDCANCEL        2
#define IDABORT         3
#define IDRETRY         4
#define IDIGNORE        5
#define IDYES           6
#define IDNO            7
#define IDCLOSE         8
#define IDHELP          9

typedef struct _DisinfectDlg
 {
  //enum { IDD = IDD_DISINFECT };
  BOOL    m_All;
  BOOL    m_CopyFolder;
  CHAR    m_sObject[0x200];
  CHAR    m_sVirus[0x200];
  int     m_Action;
 } DisinfectDlg;

typedef struct _MessageDlg
 {
  CHAR    *Text;
  CHAR    *Title;
  ULONG   Param;
 } MessageDlg;

typedef struct _surBase
 {
  USHORT LastUpdateYear;
  BYTE   LastUpdateMonth;
  BYTE   LastUpdateDay;
  ULONG  LastUpdateTime;
  ULONG  VirCount;
  ULONG  RecCount;
  ULONG  RecCountArray[8];
 } _SurBase;

extern _SurBase SurBase;
//extern  HEV hevSendWaitSem;
extern BOOL SelfScan;
MRESULT MySendMsg(ULONG ulMsgId,MPARAM mp1,MPARAM mp2);
void StatusLine(char* Line);
BOOL load_avpdll(void);
BOOL load_avpbase(char* name);                     // Load a-v database
unsigned short UnloadBase(void);                   // Unload a-v database
BOOL ScanFile(char *fullFileName,unsigned long fileSize,unsigned long mFlags);
void InitializeThread(void);
void TerminateThread(void);
ULONG GetFlags(void);
UINT ScanAllPaths();
LONG avp_mess(DWORD mp1,DWORD mp2);
//LONG avp_mess(MPARAM mp1,MPARAM mp2);

#endif