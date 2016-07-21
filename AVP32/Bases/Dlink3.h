////////////////////////////////////////////////////////////////////
//
//  DLINK3.H
//  Engine Dynamic Linkage API
//  AVP 3.0 Engine
//  Alexey de Mont de Rique [graf@avp.ru], Kaspersky Labs. 1996
//  Copyright (c) Kaspersky Labs.
//
////////////////////////////////////////////////////////////////////

#ifndef __DLINK3_H
#define __DLINK3_H

#include "retcode3.h"
#include "sizes.h"
#include "types.h"
#include "fakename.h"

#ifdef	_WIN32
	#define FAR
	#define NEAR
	#define	asm	_asm
#else
	#define FAR		far
	#define NEAR	near
#endif

typedef unsigned char	UCHAR;
typedef unsigned int	UINT;
typedef unsigned long	ULONG;

typedef unsigned char	BYTE;
typedef unsigned short	WORD;
typedef unsigned long	DWORD;

typedef WORD FAR	WF;		// just for lazy Eugene

/* Tech notes       ------------------------------------

 Seek					returns long FileOffset, 0 if ERR
 Read/Write				returns number of bytes, 0 if ERR or EOF
 Seek_Read/Seek_Write	returns same as Read/Write in case of no errors on Seek
						returns 0 if ERR on Seek

That's done to let LINKs do not warry about errors.
In case of error FileAccess library sets internal ErrorFlag (see RETCODE.H).

*/

// Internal defines ------------------------------------

#define	kernel	extern			// item is imported from Kernel16/32
#define	kern16	extern			// item is imported from Kernel16 only

// Internal Data Prototypes

#ifdef	_WIN32
extern void*			Area_Owner;	// for multitasking AVPplications
#endif

kernel DWORD NEAR		RFlags;
extern WORD  NEAR		Entry_Count;

						// emulator breakpoints table
extern BYTE NEAR		BRP;
#define	pBRP			( (BYTE NEAR*) (&BRP) )
#define	Num_BP			( pBRP[0] )
#define	BPoints			( (WORD NEAR*)(pBRP+1) )
#define	pBP4000			( ((WORD NEAR*)(pBRP+0x21))[0] )
#define	pBP6000			( ((WORD NEAR*)(pBRP+0x23))[0] )
#define	pBPStop			( ((WORD NEAR*)(pBRP+0x25))[0] )

extern BYTE NEAR		Em_IntTbl[];
extern BYTE NEAR		Em_Stack[];
extern WORD NEAR		Em_SP;

// Internal Function Prototypes

#ifdef	_WIN32
#define AO	void*Area_Owner,
#else
#define AO
#endif

	// Note: all I/O routines set RFlags|=RF_IO_ERROR in case of I/O error

#ifdef	_WIN32
				// convertors definition
extern DWORD	_DoJump(void* AreaOwner, BYTE* page); 	// precesses JMP Records and calls DoStdJunp
														// returns EP/EP_Next or 0L if entry not found
kernel long 	_Seek(AO long lOff);					// returns long FileOffset, 0 if ERR
kernel int		_Read(AO void* lpBuf, UINT nCount );	// returns number of bytes, 0 if ERR or EOF
kernel int		_Write(AO const void* lpBuf, UINT nCount );			// the same as _Read
//kernel int	_Seek_Read(AO long lOff, void* lpBuf, UINT nCount);	// the same as _Read
//kernel int	_Seek_Write(AO long lOff, void* lpBuf, UINT nCount);// the same as _Read
kernel int		_Ch_Size(AO long NewLen);				// return 0 if error

#else

kernel int FAR	_Ch_Size(long NewLen);					// see above

#endif

	// all disk direct functions return 0 if ERROR, 1 if OK.

kernel int  FAR _Read_13 (AO BYTE Disk,WORD Sector,BYTE Head,WORD NumSectors,BYTE NEAR*Buffer);
kernel int  FAR _Write_13(AO BYTE Disk,WORD Sector,BYTE Head,WORD NumSectors,BYTE NEAR*Buffer);
kernel int  FAR _Read_25 (AO BYTE Drive,long Sector,WORD NumSectors,BYTE NEAR*Buffer);
kernel int  FAR _Write_26(AO BYTE Drive,long Sector,WORD NumSectors,BYTE NEAR*Buffer);

#ifdef	_WIN32
kernel int	FAR		_Trace(UCHAR* msg , ...);
kernel WORD FAR		_Rotate(AO char flag);		//rotate, see below
#define WINAPI __stdcall
#else
#define WINAPI
#define	_Trace	Printf
#endif

#undef	AO			// ---- AO undefined -------

// Memory definitions

kernel DWORD FAR	Peek(WORD Segm,WORD Offs);	// returns DWORD from DOS memory
kernel WORD	 FAR	Poke(WORD Segm,WORD Offs, DWORD Value, BYTE Size);
					// writes to DOS memory Size bytes from Value to Segm:Offs
					// returns 0 if failed

kern16 WORD	 FAR	Rotate(char flag);		// rotate symbol in bottom line,
											// return 1 to cancel processing
kernel WORD  FAR	Get_Time(void);			// returns WORD seconds

kernel int   FAR	Printf(UCHAR* msg , ...);
kernel DWORD FAR	Check_Sum(BYTE NEAR* ptr,WORD Len);

kernel DWORD FAR	DoJump(BYTE* page); // precesses JMP Records

extern UINT FAR	IsProgramm(BYTE FAR*Header, BYTE FAR*Ext);// bit 0 set - extension ok, 1 set - format ok
extern UINT FAR	GetFirstEntry();		// 0 - No enties 1-OK
extern UINT FAR	GetNextEntry();			// 0 - No More enties 1-OK
extern UINT FAR	GetMacro(int count);	// R_CLEAN - No enties R_PREDETECT-macro in Page_A
extern UINT FAR	Get_Java(int count);	// R_CLEAN - No enties R_PREDETECT-class in Page_A
extern UINT FAR	Mike0(void); 			// R_CLEAN - No enties R_PREDETECT-class in Page_A
extern DWORD FAR	GetArchOffs();		// FFFFFFFFh - no archive in file, else - file offset of archive

extern DWORD FAR NE_Addr_to_FOffs(WORD Segm, WORD Offs);// converts CS:IP to FOffs, 0 if error
extern DWORD FAR PE_Addr_to_FOffs(DWORD Address);	// converts RVA to FOffs, 0 if error

extern DWORD FAR	DoStdJump(BYTE NEAR*Page, BYTE Method, WORD Data1, WORD Data2, WORD Data3); //Return Entry Point offset.
extern WORD FAR		CureStdFile(BYTE Method, WORD Data1,WORD Data2,WORD Data3,
							WORD Data4,WORD Data5);		// returns R_CLEAN, R_FAIL, R_DELETE
extern WORD FAR		CureStdSector(BYTE Method, WORD Data1,WORD Data2,BYTE Data3);
extern BYTE FAR* FAR	Get_Sector_Image(BYTE Sub_Type);// returns far pointer, 0 - fail


// Public defines -------------------------------------

// Standard Linked Function Prototypes

extern WORD  FAR	decode();
extern WORD  FAR	cure();
extern DWORD FAR	jmp();

// Public Data Prototypes

kernel WORD  NEAR	Object_Type; //OT_FILE,OT_SECTOR,OT_MEMORY
kernel BYTE  NEAR	Sub_Type;    //ST_COM,ST_EXE,ST_SYS,ST_OLE2 ;ST_ABOOT,ST_HDBOOT...

kernel BYTE  NEAR	Disk;        //0-0x1F Floppy; 0x80-0x83 HD; 0x20 Remote; 0x40 RAMdrv; 0x60 CDROM
kernel BYTE  NEAR	Drive;       //0-a: 1-b: ...
extern WORD  NEAR	First_Data_Sector;	// First Data Sector on disk
kernel WORD  NEAR	Disk_Max_CX;	// AH=8, INT 13h, CX, 0 if carry or floppy
kernel BYTE  NEAR	Disk_Max_DH;	// AH=8, INT 13h, DH, 0 if carry or floppy

kernel WORD  NEAR	DOS_Mem;		// last segm of DOS mem before VideoMem

kernel BYTE  NEAR*	Name;
kernel BYTE  NEAR*	Ext;
kernel BYTE  NEAR	Full_Name[ NAME_SIZE ];

kernel BYTE  NEAR	Header[ HEADER_SIZE ];
kernel BYTE  NEAR	Page_A[ PAGE_A_SIZE ];
kernel BYTE  NEAR	Page_B[ PAGE_B_SIZE ];
kernel BYTE  NEAR	Page_E[ PAGE_E_SIZE ];
kernel BYTE  NEAR	Page_C[ PAGE_C_SIZE ];

kernel DWORD NEAR	File_Length;

extern DWORD NEAR	EP;
extern DWORD NEAR	EP_Next;
extern DWORD NEAR	Tail;

extern DWORD NEAR	IP32_at_Entry;
extern DWORD NEAR	IP32_at_Next;
#define CS_at_Entry     (((WORD NEAR*)(&IP32_at_Entry))[1])
#define IP_at_Entry     (((WORD NEAR*)(&IP32_at_Entry))[0])
#define OLE_Type        (((BYTE NEAR*)(&IP32_at_Entry))[0])
#define OLE_Doc_No      (((BYTE NEAR*)(&IP32_at_Entry))[1])
#define OLE_MacrosInDoc (((WORD NEAR*)(&IP32_at_Entry))[1])

extern DWORD NEAR	Exe_Header_Len;
extern WORD  NEAR	Exe_CS;
extern WORD  NEAR	Exe_IP;

extern DWORD NEAR	Header_Offset; // NewEXE/PE/LX or multi-SYS file header file offset

extern WORD  NEAR	NExe_CS;
extern DWORD NEAR	NExe_IP;
extern WORD  NEAR	NExe_Format; //NE,PE,LE,LX

extern DWORD NEAR	PE_Sections_Offset;		// offset of PE Sections in file
extern WORD  NEAR	PE_Num_Of_Sections;		// total section in PE file
extern WORD  NEAR	PE_This_Section;		// # of section that contains specified addr, FFFFh otherwise
											// is valid only after  PE_Addr_to_FOffs(DWORD Address)
typedef DWORD (WINAPI t_AvpExecSpecial)(
		BYTE NEAR* FuncName, 
		DWORD wParam, 
		DWORD lParam
		);

extern t_AvpExecSpecial FAR* pExecSpecial;			// used to call external DLL routines

// Public Functions Prototypes

kern16 long  FAR	Seek(long Offset);				// rets FOffset, 0 if Error
kern16 int   FAR	Read(void NEAR*p,UINT Len);		// rets NumRead, 0 if Error
kern16 int   FAR	Write(void NEAR*p,UINT Len);	// rets NumWrt, 0 if Error
extern int   FAR	Seek_Read(long Offset,void NEAR*p,UINT Len); // same as above
extern int   FAR	Seek_Write(long Offset,void NEAR*p,UINT Len); // same as above
kern16 int   FAR	Ch_Size(long NewLen);			// ret 0/1 ERR/OK
extern int   FAR	Ch_Size_Lehigh(long NewLen);	// ret 0/1 ERR/OK

extern WORD  FAR	Cure_COM_Imm(BYTE NEAR* Ptr, WORD Bytes, DWORD NewLen); // returns R_codes only,
extern WORD  FAR	Cure_COM(DWORD Offset, WORD Bytes, DWORD NewLen);       // see RETCODE.H
extern WORD  FAR	Cure_EXE_Imm(BYTE NEAR* Exe_CS, BYTE NEAR* Exe_IP,      //
					BYTE NEAR* Exe_SS, BYTE NEAR* Exe_SP, DWORD NewLen);

extern WORD  FAR	Move_File_Up(DWORD Len, WORD Cut);	// Shifts file Len bytes up
extern WORD  FAR	Move_File_Data(DWORD From, DWORD To, WORD Len);		// return R_codes only,
extern WORD  FAR	CutPast_File(DWORD DstOff, DWORD SrcOff, WORD Cut);
extern WORD  FAR	Fill_File(DWORD Offset, BYTE FillByte, WORD Len);	// see RETCODE.H
extern WORD  FAR	Xor_File(DWORD Offset, BYTE Key, DWORD Len);
extern WORD  FAR	Add_File(DWORD Offset, BYTE Key, DWORD Len);
extern WORD  FAR	Rol_File(DWORD Offset, BYTE Num, DWORD Len);
extern WORD  FAR	Ror_File(DWORD Offset, BYTE Num, DWORD Len);
extern WORD  FAR	Xor_File_Word(DWORD Offset,WORD Key, DWORD Len);
extern WORD	 FAR	Add_File_Word(DWORD Offset,WORD Key, DWORD Len);
extern WORD  FAR	Rol_File_Word(DWORD Offset,WORD Num, DWORD Len);
extern WORD	 FAR	Ror_File_Word(DWORD Offset,WORD Num, DWORD Len);

extern BYTE	 FAR	BP_Dword1_Cpy(DWORD DW1,WORD Len);
extern BYTE	 FAR	BP_Dword3_Cpy(DWORD DW1,DWORD DW2,DWORD DW3,WORD Len);

extern int   FAR	Read_13(WORD Sector,BYTE Head,BYTE NEAR*Buffer);	// return 0/1 ERR/OK
extern int   FAR	Write_13(WORD Sector,BYTE Head,BYTE NEAR*Buffer);
extern int   FAR	Read_25(long Sector,BYTE NEAR*Buffer);
extern int   FAR	Write_26(long Sector,BYTE NEAR*Buffer);
extern int   FAR	Read_Boot(BYTE NEAR*Buffer);
extern int   FAR	Read_MBR(BYTE NEAR*Buffer);
extern int   FAR	Write_Boot(BYTE NEAR*Buffer);
extern int   FAR	Write_MBR(BYTE NEAR*Buffer);


extern void  FAR	Fill_Data(BYTE NEAR*ptr, BYTE FillByte, WORD Len);
extern void  FAR	Copy_Data(BYTE NEAR*dest, BYTE NEAR*src, WORD Len);

extern void  FAR	Xor_Byte(BYTE NEAR*src,BYTE NEAR*dst,BYTE Byte,	WORD Len);
extern void  FAR	Add_Byte(BYTE NEAR*src,BYTE NEAR*dst,BYTE Byte,	WORD Len);
extern void  FAR	Rol_Byte(BYTE NEAR*src,BYTE NEAR*dst,BYTE Num,	WORD Len);
extern void  FAR	Ror_Byte(BYTE NEAR*src,BYTE NEAR*dst,BYTE Num,	WORD Len);
extern void  FAR	Neg_Byte(BYTE NEAR*src,BYTE NEAR*dst,			WORD Len);
extern void  FAR	Xor_Word(BYTE NEAR*src,BYTE NEAR*dst,WORD Word, WORD Len);
extern void  FAR	Add_Word(BYTE NEAR*src,BYTE NEAR*dst,WORD Word, WORD Len);
extern void  FAR	Rol_Word(BYTE NEAR*src,BYTE NEAR*dst,WORD Num,	WORD Len);	// WORD Num to use pRoutine in other funtions
extern void  FAR	Ror_Word(BYTE NEAR*src,BYTE NEAR*dst,WORD Num,	WORD Len);


// Debug Functions Prototypes - only for AVP Editor

#ifdef _WIN32

kernel int   FAR	D_Open(BYTE*pName);			// open debug log
kernel int   FAR	D_Close();					// close debug log
kernel int   FAR	D_Write(void*p,DWORD Len);	// write to debug log

#endif

// Internal Data Prototypes , part 2
											// to use while scanning/disinfecting
											// Boot Records in the MBR
#define	Original_MBR		(Header+0x200)
#define	Ptr_PTable_Entry	Header_Offset
#define	Boot_Entry_CX		Exe_CS
#define	Boot_Entry_DH		(BYTE)Exe_IP

#endif//__DLINK3_H