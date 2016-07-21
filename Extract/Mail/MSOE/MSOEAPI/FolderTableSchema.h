tagCOLUMN_ g_rgFolderTblColumns[] = {

    0,	UnknownA,	0x08,  4,
    1,  Dword,		0x0C,  4,
    2,  String,		0x10,  4,
    3,  String,		0x14,  4,
    4,  String,		0x18,  4,
    5,  String,		0x1C,  4,
    6,  Unknown9,	0x24,  4,
    7,  Dword,		0x28,  4,
    8,  Dword,		0x2C,  4,
    9,  Bool,		0x40,  1,
 0x0A,  Bool,		0x41,  1,
 0x0B,  Bool,		0x42,  1,
 0x0C,  Dword,		0x44,  4,
 0x0D,  Dword,		0x48,  4,
 0x0E,  Dword,		0x4C,  4,
 0x0F,  Dword,		0x50,  4,
 0x10,  Dword,		0x54,  4,
 0x11,  Dword,		0x58,  4,
 0x12,  Dword,		0x64,  4,
 0x13,  DateTime,	0x68,  8,
 0x14,  String,		0x20,  4,
 0x15,  DateTime,	0x70,  8,
 0x16,  Dword,		0x34,  4,
 0x17,  Dword,		0x38,  4,
 0x18,  Dword,		0x5C,  4,
 0x19,  Dword,		0x60,  4,
 0x1A,  Dword,		0x78,  4,
 0x1B,  Dword,		0x3C,  4,
 0x1C,  Dword,		0x30,  4,
};

tagTABLEINDEX g_FolderTblPrimaryIndex = {
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

DWORD g_FolderSymbolTable[] = {	2
, 2, (DWORD)(DWORD*)"FOLDER_SUBSCRIBED", 1
, 1, (DWORD)(DWORD*)"FLDCOL_FLAGS", 6
, 0, 0, 0
};

tagTABLESCHEMA g_FoldersTableSchema = { 
	&CLSID_FolderDatabase, 
	0x0B8, 0, 4, 5, 7, 0x618, 0x0C, 0x27,
	(tagCOLUMN_*)&g_rgFolderTblColumns,
	&g_FolderTblPrimaryIndex,
	&g_FolderSymbolTable
};


