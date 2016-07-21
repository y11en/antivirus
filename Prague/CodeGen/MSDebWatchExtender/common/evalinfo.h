#ifndef __evalinfo_h
#define __evalinfo_h

#ifndef IDA
#include <windows.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

enum eTYPE {
	eTYPE_NoType            = 0x00,
	eTYPE_Absolute          = 0x01,
	eTYPE__segment          = 0x02,
	eTYPE_void              = 0x03,
	eTYPE_char              = 0x10,
	eTYPE_short             = 0x11,
	eTYPE_long              = 0x12,
	eTYPE___int64           = 0x13,
	eTYPE_unsigned_char     = 0x20,
	eTYPE_unsigned_short    = 0x21,
	eTYPE_unsigned_long     = 0x22,
	eTYPE_unsigned___int64  = 0x23,
	eTYPE_bool              = 0x30,
	eTYPE_float             = 0x40,
	eTYPE_double            = 0x41,
	eTYPE_long_double       = 0x42,
	eTYPE_char_             = 0x70,
	eTYPE_wchar_t           = 0x71,
	eTYPE_int               = 0x72,
	eTYPE_unsigned_int      = 0x73,
	eTYPE_int_              = 0x74,
	eTYPE_unsigned_int_     = 0x75,
	eTYPE___int64_          = 0x76,
	eTYPE_unsigned___int64_ = 0x77,
};

typedef struct tag_TypeName {
	DWORD id;
	char*	name;
} tTypeName;

enum ePTR {
	ePTR_NoPtr         = 0x00,
		ePTR_NearPtr16     = 0x01,
		ePTR_FarPtr16      = 0x02,
		ePTR_HugePtr16     = 0x03,
		ePTR_NearPtr32     = 0x04,
		ePTR_FarPtr32      = 0x05,
		ePTR_HugePtr32     = 0x06,
};

typedef struct tag_PtrName {
	DWORD id;
	char*	name;
} tPtrName;

typedef struct tag_SymType {
	enum eTYPE type : 8;
	enum ePTR  ptr  : 7;
	DWORD dwUnknown : 17;
} tSymType;

typedef struct tag_REGISTER {
	BYTE  bNameLen;
	CHAR  sName[7];
	DWORD dwIdx;
	WORD  wUnknown;
	WORD  wType;
} REGISTER, *PREGISTER;

typedef struct tag_MOD_INFO
{
	DWORD dwUnknown0;
	DWORD dwUnknown1;
	DWORD dwUnknown2;
	DWORD dwUnknown3;
	DWORD dwUnknown4;
	DWORD dwUnknown5;
	DWORD dwUnknown6;
	DWORD dwModImageBase;
} MOD_INFO, *PMOD_INFO;

typedef struct tag_SYM_INFO {
	WORD  wSize;
	WORD  wFlags;
} SYM_INFO;

#define SYM_NAME_VALID(pSymInfo) (pSymInfo && pSymInfo->bNameLen>0 && pSymInfo->bNameLen!=0xFF && pSymInfo->sName[pSymInfo->bNameLen-1]!=0)

typedef struct tag_SYM_INFO_VAR {
	WORD  wSize;
	WORD  wFlags;
	DWORD dwAddress;
	enum eTYPE Type; 
	BYTE  bNameLen;
	BYTE  sName[1];
} SYM_INFO_VAR;

typedef struct tag_SYM_INFO_FUNC {
	WORD  wSize;
	WORD  wFlags;
	DWORD dwAddress;
	DWORD dwUnknown1; 
	DWORD dwUnknown2;
	DWORD dwCodeSize;
	DWORD dwUnknown4;
	DWORD dwUnknown5;
	DWORD dwUnknown6;
	DWORD dwUnknown7;
	WORD  wUnknown8;
	BYTE  bUnknown9;
	BYTE  bNameLen;
	BYTE  sName[1];
} SYM_INFO_FUNC;

typedef struct tag_OBJ_SRC_COUNTS {
	WORD wObjCount;
	WORD wSrcCount;
} OBJ_SRC_COUNTS;

typedef struct tag_PDB_MOD_INFO {
	struct tag_PDB_MOD_INFO* pNextPdbModInfo;
	DWORD dwUnknown004; // 0
	DWORD dwUnknown008; // 1
	DWORD dwUnknown00C; // 0
	DWORD dwUnknown010; // 0
	DWORD dwUnknown014; // 0
	DWORD dwUnknown018; // 0
	DWORD dwUnknown01C; // 0
	DWORD dwUnknown020; // 0
	DWORD dwUnknown024; // 0
	CHAR* pImageFullPathName; // "C:\WORK\PROJECTS\MIKE\LINKPTRCLASS\DEBUG\LINKPTRCLASS.EXE"
	CHAR* pImageName; // "LinkPtrClass"
	CHAR* pPdbFullPathName; // "C:\WORK\PROJECTS\MIKE\LINKPTRCLASS\DEBUG\LinkPtrClass.pdb"
	DWORD dwUnknown034; // 0
	DWORD dwUnknown038; // 0
	DWORD dwUnknown03C; // 0
	DWORD dwUnknown040; // offset word_20B1BD0
	DWORD dwUnknown044; // 0
	DWORD dwUnknown048; // offset off_22B06B8
	DWORD dwUnknown04C; // offset off_22B0AD0
	DWORD dwUnknown050; // offset off_228F798
	DWORD dwUnknown054; // offset off_22B0B98
	DWORD dwUnknown058; // offset off_22B0B80
	DWORD dwUnknown05C; // 0
	DWORD dwUnknown060; // 0
	DWORD dwUnknown064; // 0
	DWORD dwUnknown068; // 0
	DWORD dwUnknown06C; // 0
	DWORD dwUnknown070; // 0
	DWORD dwUnknown074; // 0
	DWORD dwUnknown078; // 0
	DWORD dwUnknown07C; // 0
	DWORD dwUnknown080; // 0
	DWORD dwUnknown084; // 0
	DWORD dwUnknown088; // 0
	DWORD dwUnknown08C; // 0
	DWORD dwUnknown090; // 0
	DWORD dwUnknown094; // 0
	DWORD dwUnknown098; // 0
	DWORD dwUnknown09C; // 0
	DWORD dwUnknown0A0; // 0
	DWORD dwUnknown0A4; // 0
	DWORD dwUnknown0A8; // 0
	DWORD dwUnknown0AC; // 0
	DWORD dwUnknown0B0; // 0
	DWORD dwUnknown0B4; // 0
	DWORD dwUnknown0B8; // 0
	DWORD dwUnknown0BC; // 0
	DWORD dwUnknown0C0; // 0
	DWORD dwUnknown0C4; // 0
	DWORD dwUnknown0C8; // 0
	DWORD dwUnknown0CC; // 0
	DWORD dwUnknown0D0; // 0
	DWORD dwUnknown0D4; // 0
	DWORD dwUnknown0D8; // 0
	DWORD dwUnknown0DC; // 0
	DWORD dwUnknown0E0; // 0
	DWORD dwUnknown0E4; // 0
	DWORD dwUnknown0E8; // 0
	DWORD dwUnknown0EC; // 0
	DWORD dwUnknown0F0; // 0
	DWORD dwUnknown0F4; // 0
	DWORD dwUnknown0F8; // 0
	DWORD dwUnknown0FC; // 0
	DWORD dwUnknown100; // 0
	DWORD dwUnknown104; // offset unk_20B0A08
	DWORD dwUnknown108; // 0
	DWORD dwUnknown10C; // 0
	DWORD dwUnknown110; // 0
	DWORD dwObjCount; // 3
	OBJ_SRC_COUNTS* pObjSrcCounts; // offset dword_20B1C30
	WORD* pwObj2SrcMaps; // offset dword_20B1C34
	WORD* pwObjHasCode; // offset word_20B1C3A
	DWORD* pdwSourceFilesOffsets; // offset off_20B1C40
	DWORD dwSourceFilesTotalSize; // 33h
	CHAR* pSourceFiles; // db 51,'C:\Work\Projects\Mike\LinkPtrClass\LinkPtrClass.cpp'
	DWORD dwUnknown130; // 0
	DWORD dwUnknown134; // 0
	DWORD dwUnknown138; // 0
	DWORD dwUnknown13C; // 0
	DWORD dwUnknown140; // 0
	DWORD dwUnknown144; // 0
	DWORD dwUnknown148; // 0
	DWORD dwUnknown14C; // 0
	DWORD dwUnknown150; // 1000h
	DWORD dwUnknown154; // 0
	DWORD dwUnknown158; // offset unk_20B1A28
	DWORD dwImageBase;  // 22340000h
	DWORD dwImageTimestamp; // 3EE6F13Fh
	DWORD dwUnknown164; // 0
	DWORD dwUnknown168; // 1000h
	DWORD dwUnknown16C; // 0
} PDB_MOD_INFO;

typedef struct tag_PDB_OBJ_INFO {
	WORD wUnknown00;
	WORD wUnknown02;
	WORD wUnknown04;
	WORD wUnknown06;
	WORD wUnknown08;
	WORD wUnknown0A;
	BYTE bObjNameLen;
	CHAR sObjName[1];
	// ...
} PDB_OBJ_INFO;

typedef struct tag_PDB_SRC_INFO {
	WORD wUnknown00;
	WORD wUnknown02;
	DWORD dwUnknown04;
	DWORD dwUnknown08;
	DWORD dwUnknown0C;
	DWORD dwUnknown10;
	DWORD dwUnknown14;
	DWORD dwUnknown18;
	DWORD dwUnknown1C;
	DWORD dwUnknown20;
	BYTE  bSrcNameLen;
	CHAR  sSrcName[1];
} PDB_SRC_INFO;

typedef struct tag_PDB_INFO
{
	PDB_MOD_INFO* pPdbModInfo;
	DWORD dwUnknown04;
	DWORD dwUnknown08;
	PDB_OBJ_INFO* pPdbObjInfo; 
	PDB_SRC_INFO* pPdbSrcInfo;
	CHAR* pObjRelativeName;
	DWORD dwUnknown18;
	DWORD dwSrcInfoSize;

} PDB_INFO;

typedef struct tag_EVAL_INFO
{
	DWORD dwUnknown0;
	tSymType SymType;
	DWORD dwUnknown2;
	DWORD dwAddress;
	DWORD dwUnknown4;
	DWORD dwUnknown5;
	DWORD dwUnknown6;
	PDB_INFO* pPdbInfo;
	DWORD dwUnknown8;
	DWORD dwUnknown9;
	MOD_INFO* pModInfo;
	DWORD dwUnknown11;
	DWORD dwUnknown12;
	DWORD dwUnknown13;
	SYM_INFO_FUNC* pSymInfoFunc;
	DWORD dwUnknown15;
	SYM_INFO_VAR* pSymInfoVar;
	DWORD dwUnknown17;
	DWORD dwFlags;
	DWORD dwUnknown19;
	DWORD dwUnknown20;
	DWORD dwUnknown21;
	DWORD dwStructSize;
	DWORD dwUnknown23;
	DWORD dwUnknown24;
	DWORD dwUnknown25;
	DWORD dwUnknown26;
	DWORD dwUnknown27;
	DWORD dwUnknown28;
	DWORD dwUnknown29;
	DWORD dwUnknown30;
	DWORD dwUnknown31;
	DWORD dwUnknown32;
	DWORD dwUnknown33;
	DWORD dwRegisterIdx;
	DWORD dwUnknown35;
	DWORD dwUnknown36;
	DWORD dwUnknown37;
	DWORD dwValue[4];
} EVAL_INFO, *PEVAL_INFO;

extern const __declspec(selectany) tTypeName TypeNames[] = {
	{ eTYPE_NoType            , "NoType"            },
	{ eTYPE_Absolute          , "Absolute"          },	
	{ eTYPE__segment          , "_segment"          },
	{ eTYPE_void              , "void"              },
	{ eTYPE_char              , "char"              },
	{ eTYPE_short             , "short"             },
	{ eTYPE_long              , "long"              },
	{ eTYPE___int64           , "__int64"           },
	{ eTYPE_unsigned_char     , "unsigned char"     },
	{ eTYPE_unsigned_short    , "unsigned short"    },
	{ eTYPE_unsigned_long     , "unsigned long"     },
	{ eTYPE_unsigned___int64  , "unsigned __int64"  },
	{ eTYPE_bool              , "bool"              },
	{ eTYPE_float             , "float"             },
	{ eTYPE_double            , "double"            },
	{ eTYPE_long_double       , "long double"       },
	{ eTYPE_char_             , "char"              },
	{ eTYPE_wchar_t           , "wchar_t"           },
	{ eTYPE_int               , "int"               },
	{ eTYPE_unsigned_int      , "unsigned int"      },
	{ eTYPE_int_              , "int"               },
	{ eTYPE_unsigned_int_     , "unsigned int"      },
	{ eTYPE___int64_          , "__int64"           },		
	{ eTYPE_unsigned___int64_ , "unsigned __int64"  },		
};

extern const __declspec(selectany) tPtrName PtrNames[] = {
	{ ePTR_NoPtr           , ""                  },
	{ ePTR_NearPtr16       , "*"                 },	
	{ ePTR_FarPtr16        , "far *"             },
	{ ePTR_HugePtr16       , "huge *"            },
	{ ePTR_NearPtr32       , "*"                 },
	{ ePTR_FarPtr32        , "far *"             },
	{ ePTR_HugePtr32       , "*"                 },
};

extern const __declspec(selectany) REGISTER Registers[] = {
	{	2,	"AX",		0x09,	0x03,	0x21 },
	{	2,	"BX",		0x0C,	0x03,	0x21 },
	{	2,	"CX",		0x0A,	0x03,	0x21 },
	{	2,	"DX",		0x0B,	0x03,	0x21 },
	{	2,	"SP",		0x0D,	0x03,	0x21 },
	{	2,	"BP",		0x0E,	0x03,	0x21 },
	{	2,	"SI",		0x0F,	0x03,	0x21 },
	{	2,	"DI",		0x10,	0x03,	0x21 },
	{	2,	"DS",		0x1C,	0x03,	0x21 },
	{	2,	"ES",		0x19,	0x03,	0x21 },
	{	2,	"SS",		0x1B,	0x03,	0x21 },
	{	2,	"CS",		0x1A,	0x03,	0x21 },
	{	2,	"IP",		0x1F,	0x03,	0x21 },
	{	2,	"FL",		0x20,	0x03,	0x21 },
	{	3,	"EFL",		0x22,	0x02,	0x22 },
	{	3,	"EAX",		0x11,	0x02,	0x22 },
	{	3,	"EBX",		0x14,	0x02,	0x22 },
	{	3,	"ECX",		0x12,	0x02,	0x22 },
	{	3,	"EDX",		0x13,	0x02,	0x22 },
	{	3,	"ESP",		0x15,	0x02,	0x22 },
	{	3,	"EBP",		0x16,	0x02,	0x22 },
	{	3,	"ESI",		0x17,	0x02,	0x22 },
	{	3,	"EDI",		0x18,	0x02,	0x22 },
	{	3,	"EIP",		0x21,	0x02,	0x22 },
	{	2,	"FS",		0x1D,	0x02,	0x21 },
	{	2,	"GS",		0x1E,	0x02,	0x21 },
	{	2,	"AH",		0x05,	0x07,	0x20 },
	{	2,	"BH",		0x08,	0x07,	0x20 },
	{	2,	"CH",		0x06,	0x07,	0x20 },
	{	2,	"DH",		0x07,	0x07,	0x20 },
	{	2,	"AL",		0x01,	0x0B,	0x20 },
	{	2,	"BL",		0x04,	0x0B,	0x20 },
	{	2,	"CL",		0x02,	0x0B,	0x20 },
	{	2,	"DL",		0x03,	0x0B,	0x20 },
	{	2,	"st",		0x80,	0x13,	0x42 },
	{	3,	"st0",		0x80,	0x13,	0x42 },
	{	3,	"st1",		0x81,	0x13,	0x42 },
	{	3,	"st2",		0x82,	0x13,	0x42 },
	{	3,	"st3",		0x83,	0x13,	0x42 },
	{	3,	"st4",		0x84,	0x13,	0x42 },
	{	3,	"st5",		0x85,	0x13,	0x42 },
	{	3,	"st6",		0x86,	0x13,	0x42 },
	{	3,	"st7",		0x87,	0x13,	0x42 },
	{	3,	"MM0",		0x92,	0x01,	0x23 },
	{	3,	"MM1",		0x93,	0x01,	0x23 },
	{	3,	"MM2",		0x94,	0x01,	0x23 },
	{	3,	"MM3",		0x95,	0x01,	0x23 },
	{	3,	"MM4",		0x96,	0x01,	0x23 },
	{	3,	"MM5",		0x97,	0x01,	0x23 },
	{	3,	"MM6",		0x98,	0x01,	0x23 },
	{	3,	"MM7",		0x99,	0x01,	0x23 },
	{	6,	"XMM0IL",	0x0DC,	0x01,	0x23 },
	{	6,	"XMM1IL",	0x0DD,	0x01,	0x23 },
	{	6,	"XMM2IL",	0x0DE,	0x01,	0x23 },
	{	6,	"XMM3IL",	0x0DF,	0x01,	0x23 },
	{	6,	"XMM4IL",	0x0E0,	0x01,	0x23 },
	{	6,	"XMM5IL",	0x0E1,	0x01,	0x23 },
	{	6,	"XMM6IL",	0x0E2,	0x01,	0x23 },
	{	6,	"XMM7IL",	0x0E3,	0x01,	0x23 },
	{	6,	"XMM0IH",	0x0E4,	0x01,	0x23 },
	{	6,	"XMM1IH",	0x0E5,	0x01,	0x23 },
	{	6,	"XMM2IH",	0x0E6,	0x01,	0x23 },
	{	6,	"XMM3IH",	0x0E7,	0x01,	0x23 },
	{	6,	"XMM4IH",	0x0E8,	0x01,	0x23 },
	{	6,	"XMM5IH",	0x0E9,	0x01,	0x23 },
	{	6,	"XMM6IH",	0x0EA,	0x01,	0x23 },
	{	6,	"XMM7IH",	0x0EB,	0x01,	0x23 },
	{	6,	"XMM0DL",	0x0C2,	0x01,	0x41 },
	{	6,	"XMM1DL",	0x0C3,	0x01,	0x41 },
	{	6,	"XMM2DL",	0x0C4,	0x01,	0x41 },
	{	6,	"XMM3DL",	0x0C5,	0x01,	0x41 },
	{	6,	"XMM4DL",	0x0C6,	0x01,	0x41 },
	{	6,	"XMM5DL",	0x0C7,	0x01,	0x41 },
	{	6,	"XMM6DL",	0x0C8,	0x01,	0x41 },
	{	6,	"XMM7DL",	0x0C9,	0x01,	0x41 },
	{	6,	"XMM0DH",	0x0CA,	0x01,	0x41 },
	{	6,	"XMM1DH",	0x0CB,	0x01,	0x41 },
	{	6,	"XMM2DH",	0x0CC,	0x01,	0x41 },
	{	6,	"XMM3DH",	0x0CD,	0x01,	0x41 },
	{	6,	"XMM4DH",	0x0CE,	0x01,	0x41 },
	{	6,	"XMM5DH",	0x0CF,	0x01,	0x41 },
	{	6,	"XMM6DH",	0x0D0,	0x01,	0x41 },
	{	6,	"XMM7DH",	0x0D1,	0x01,	0x41 },
	{	5,	"MXCSR",	0x0D3,	0x01,	0x22 },
	{	4,	"XMM0",		0x9A,	0x01,	0x24 },
	{	4,	"XMM1",		0x9B,	0x01,	0x24 },
	{	4,	"XMM2",		0x9C,	0x01,	0x24 },
	{	4,	"XMM3",		0x9D,	0x01,	0x24 },
	{	4,	"XMM4",		0x9E,	0x01,	0x24 },
	{	4,	"XMM5",		0x9F,	0x01,	0x24 },
	{	4,	"XMM6",		0x0A0,	0x01,	0x24 },
	{	4,	"XMM7",		0x0A1,	0x01,	0x24 },
	{	5,	"XMM00",	0x0A2,	0x01,	0x40 },
	{	5,	"XMM01",	0x0A3,	0x01,	0x40 },
	{	5,	"XMM02",	0x0A4,	0x01,	0x40 },
	{	5,	"XMM03",	0x0A5,	0x01,	0x40 },
	{	5,	"XMM10",	0x0A6,	0x01,	0x40 },
	{	5,	"XMM11",	0x0A7,	0x01,	0x40 },
	{	5,	"XMM12",	0x0A8,	0x01,	0x40 },
	{	5,	"XMM13",	0x0A9,	0x01,	0x40 },
	{	5,	"XMM20",	0x0AA,	0x01,	0x40 },
	{	5,	"XMM21",	0x0AB,	0x01,	0x40 },
	{	5,	"XMM22",	0x0AC,	0x01,	0x40 },
	{	5,	"XMM23",	0x0AD,	0x01,	0x40 },
	{	5,	"XMM30",	0x0AE,	0x01,	0x40 },
	{	5,	"XMM31",	0x0AF,	0x01,	0x40 },
	{	5,	"XMM32",	0x0B0,	0x01,	0x40 },
	{	5,	"XMM33",	0x0B1,	0x01,	0x40 },
	{	5,	"XMM40",	0x0B2,	0x01,	0x40 },
	{	5,	"XMM41",	0x0B3,	0x01,	0x40 },
	{	5,	"XMM42",	0x0B4,	0x01,	0x40 },
	{	5,	"XMM43",	0x0B5,	0x01,	0x40 },
	{	5,	"XMM50",	0x0B6,	0x01,	0x40 },
	{	5,	"XMM51",	0x0B7,	0x01,	0x40 },
	{	5,	"XMM52",	0x0B8,	0x01,	0x40 },
	{	5,	"XMM53",	0x0B9,	0x01,	0x40 },
	{	5,	"XMM60",	0x0BA,	0x01,	0x40 },
	{	5,	"XMM61",	0x0BB,	0x01,	0x40 },
	{	5,	"XMM62",	0x0BC,	0x01,	0x40 },
	{	5,	"XMM63",	0x0BD,	0x01,	0x40 },
	{	5,	"XMM70",	0x0BE,	0x01,	0x40 },
	{	5,	"XMM71",	0x0BF,	0x01,	0x40 },
	{	5,	"XMM72",	0x0C0,	0x01,	0x40 },
	{	5,	"XMM73",	0x0C1,	0x01,	0x40 },
	{	4,	"MM00",		0x0EC,	0x01,	0x40 },
	{	4,	"MM01",		0x0ED,	0x01,	0x40 },
	{	4,	"MM10",		0x0EE,	0x01,	0x40 },
	{	4,	"MM11",		0x0EF,	0x01,	0x40 },
	{	4,	"MM20",		0x0F0,	0x01,	0x40 },
	{	4,	"MM21",		0x0F1,	0x01,	0x40 },
	{	4,	"MM30",		0x0F2,	0x01,	0x40 },
	{	4,	"MM31",		0x0F3,	0x01,	0x40 },
	{	4,	"MM40",		0x0F4,	0x01,	0x40 },
	{	4,	"MM41",		0x0F5,	0x01,	0x40 },
	{	4,	"MM50",		0x0F6,	0x01,	0x40 },
	{	4,	"MM51",		0x0F7,	0x01,	0x40 },
	{	4,	"MM60",		0x0F8,	0x01,	0x40 },
	{	4,	"MM61",		0x0F9,	0x01,	0x40 },
	{	4,	"MM70",		0x0FA,	0x01,	0x40 },
	{	4,	"MM71",		0x0FB,	0x01,	0x40 },
	{	3,	"TIB",		0x0FD,	0x01,	0x22 },
	{	3,	"ERR",		0x0FC,	0x01,	0x22 },
	{	3,	"CLK",		0x0FE,	0x01,	0x22 },
};

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __evalinfo_h