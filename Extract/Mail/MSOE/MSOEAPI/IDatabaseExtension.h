struct IDatabaseExtension {
public: virtual long __stdcall QueryInterface (struct _GUID const &,void * *){ return 0;}
	public: virtual unsigned long __stdcall AddRef (void){ return 100;}
	public: virtual unsigned long __stdcall Release (void){ return 100;}
	public: virtual long __stdcall Initialize(struct IDatabase *){ return 0;}
	public: virtual long __stdcall OnLock(void){ return 0;}
	public: virtual long __stdcall OnUnlock(void){ return 0;}
	public: virtual long __stdcall OnRecordInsert(enum  tagOPERATIONSTATE,struct tagORDINALLIST *,void *){ return 0;}
	public: virtual long __stdcall OnRecordUpdate(enum  tagOPERATIONSTATE,struct tagORDINALLIST *,void *,void *){ return 0;}
	public: virtual long __stdcall OnRecordDelete(enum  tagOPERATIONSTATE,struct tagORDINALLIST *,void *){ return 0;}
	public: virtual long __stdcall OnExecuteMethod(unsigned long,void *,unsigned long *){ return 0;}
};

struct IDatabaseProgress {
public: virtual long __stdcall QueryInterface (struct _GUID const &,void * *){ return 0;}
	public: virtual unsigned long __stdcall AddRef (void){ return 0;}
	public: virtual unsigned long __stdcall Release (void){ return 0;}
	public: virtual long __stdcall Update(unsigned long){ return 0;}
};
