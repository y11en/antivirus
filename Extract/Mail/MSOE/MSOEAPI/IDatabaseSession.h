GUID CLSID_FolderDatabase   = {0x6F74FDC6, 0xE366, 0x11D1, 0x9A, 0x4E, 0x00, 0x0C0, 0x4F, 0x0A3, 0x09, 0x0D4};
GUID CLSID_MessageDatabase	= {0x6F74FDC5, 0xE366, 0x11D1, 0x9A, 0x4E, 0x00, 0x0C0, 0x4F, 0x0A3, 0x09, 0x0D4};
GUID CLSID_DatabaseSession	= {0x4A16043F, 0x676D, 0x11D2, 0x99, 0x4E, 0x00, 0x0C0, 0x4F, 0x0A3, 0x09, 0x0D4};
GUID IID_IDatabaseSession	= {0x4A160440, 0x676D, 0x11D2, 0x99, 0x4E, 0x00, 0x0C0, 0x4F, 0x0A3, 0x09, 0x0D4};

struct HROWSET__ {
	void* h;
};

enum enumCOLUMNDATATYPE_ {
	Unknown0 = 0,
	Unknown1 = 1,
	String	 = 2,
	Bool	 = 3,
	Dword	 = 4,
	Word	 = 5,
	Stream	 = 6,
	DateTime = 7,
	Unknown8 = 8,
	Unknown9 = 9,
	UnknownA = 10
};

struct tagCOLUMN_ {
	DWORD dwID;
	enumCOLUMNDATATYPE_ dwType;
	DWORD dwInfoOffset;
	DWORD dwDataSize;

};

struct tagTABLEINDEX {
	DWORD dwUnk[14];
};

struct tagTABLESCHEMA {
	GUID*	clsid;
	DWORD	dwInfoSize;
	DWORD	dwUnk2;
	DWORD	dwUnk3;
	DWORD	dwUnk4;
	DWORD	dwUnk5;
	DWORD	dwUnk6;
	DWORD	dwUnk7;
	DWORD	dwColumnsCount;
	tagCOLUMN_* pColumns;
	tagTABLEINDEX* pPrimaryIndex;
	VOID*	pSymbolTable;
};


struct IDatabaseSession {
	public: virtual long __stdcall QueryInterface(struct _GUID const &,void * *);
	public: virtual unsigned long __stdcall AddRef(void);
	public: virtual unsigned long __stdcall Release(void);
	public: virtual long __stdcall OpenDatabase(const char* FileName, DWORD dwCreationDisposition,struct tagTABLESCHEMA const *,struct IDatabaseExtension* pIDatabaseExtension,struct IDatabase** ppIDatabase);
	public: virtual long __stdcall OpenDatabaseW(unsigned short const *,unsigned long,struct tagTABLESCHEMA const *,struct IDatabaseExtension *,struct IDatabase * *);
	public: virtual long __stdcall OpenQuery(struct IDatabase *,char const *,struct IDatabaseQuery * *);
};

