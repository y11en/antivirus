tagCOLUMN_ g_rgMsgTblColumns[] = {

      0,     UnknownA,	  0x0C,     4,
      1,     Unknown9,	  0x10,     4,
      2,     Unknown0,	  0x14,     8,
      3,     Dword,		  0x1C,     4,
      4,     Stream,	  0x20,     4,
      5,     String,	  0x28,     4,
      6,     Unknown0,	  0x2C,     8,
      7,     String,	  0x34,     4,
      8,     String,	  0x38,     4,
      9,     String,	  0x3C,     4,
   0x0A,     String,	  0x40,     4,
   0x0B,     String,	  0x44,     4,
   0x0C,     String,	  0x48,     4,
   0x0D,     String,	  0x4C,     4,
   0x0E,     String,	  0x50,     4,
   0x0F,     Word,		  0x54,     2,
   0x10,     Word,		  0x56,     2,
   0x11,     Dword,		  0x58,     4,
   0x12,     Unknown0,	  0x5C,     8,
   0x13,     String,	  0x64,     4,
   0x14,     String,	  0x68,     4,
   0x15,     Dword,		  0x6C,     4,
   0x16,     String,	  0x70,     4,
   0x17,     String,	  0x74,     4,
   0x18,     String,	  0x78,     4,
   0x19,     String,	  0x7C,     4,
   0x1A,     String,	  0x80,     4,
   0x1B,     String,	  0x84,     4,
   0x1C,     DateTime,	  0x8C,     8,
   0x1D,     Word,		  0x94,     2,
   0x1E,     String,	  0x98,     4,
   0x1F,     Dword,		  0x9C,     4,
   0x20,     Dword,		 0x0A0,     4,
   0x21,     Dword,		 0x0A4,     4,
   0x22,     DateTime,	 0x0A8,     8,
   0x23,     String,	  0x88,     4,
   0x24,     Dword,		  0x24,     4,
   0x25,     Bool,		 0x0B0,     1,
   0x26,     String,	 0x0B4,     4,
};

tagTABLEINDEX g_MsgTblPrimaryIndex = {
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

DWORD g_MsgSymbolTable[98] = {	21
, 2, (DWORD)(DWORD*)"ARF_READ", 0x80
, 2, (DWORD)(DWORD*)"ARF_HASBODY", 1
, 2, (DWORD)(DWORD*)"ARF_ENDANGERED", 0x40
, 1, (DWORD)(DWORD*)"MSGCOL_FLAGS", 1
, 1, (DWORD)(DWORD*)"MSGCOL_EMAILFROM", 0x0E
, 2, (DWORD)(DWORD*)"ARF_WATCH", 0x400000
, 2, (DWORD)(DWORD*)"ARF_IGNORE", 0x800000
, 2, (DWORD)(DWORD*)"ARF_HASATTACH", 0x4000
, 2, (DWORD)(DWORD*)"ARF_SIGNED", 0x1000
, 2, (DWORD)(DWORD*)"ARF_ENCRYPTED", 0x2000
, 1, (DWORD)(DWORD*)"MSGCOL_PRIORITY", 0x10
, 1, (DWORD)(DWORD*)"MSGCOL_SUBJECT", 8
, 1, (DWORD)(DWORD*)"MSGCOL_FROMHEADER", 9
, 1, (DWORD)(DWORD*)"MSGCOL_ACCOUNTID", 0x1B
, 1, (DWORD)(DWORD*)"MSGCOL_LINECOUNT", 3
, 2, (DWORD)(DWORD*)"IMSG_PRI_HIGH", 1
, 2, (DWORD)(DWORD*)"IMSG_PRI_LOW", 5
, 1, (DWORD)(DWORD*)"MSGCOL_DISPLAYFROM", 0x0D
, 2, (DWORD)(DWORD*)"ARF_FLAGGED", 0x20
, 3, (DWORD)(DWORD*)"MessageAgeInDays", 0x65
, 2, (DWORD)(DWORD*)"ARF_DELETED_OFFLINE", 0x2000000
, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

tagTABLESCHEMA g_MessageTableSchema = { 
	&CLSID_MessageDatabase, 
	0x0B8, 0, 4, 5, 7, 0x618, 0x0C, 0x27,
	(tagCOLUMN_*)&g_rgMsgTblColumns,
	&g_MsgTblPrimaryIndex,
	&g_MsgSymbolTable
};
