enum  tagACCESSTYPE {
	ACCESS_READ = 0x64,
	ACCESS_WRITE = 0xC8,
};

enum  tagSEEKROWSETTYPE {
	SEEK_BEGINNING = 0
};

struct IDatabase {
	public: virtual long __stdcall QueryInterface(struct _GUID const &,void * *);
	public: virtual unsigned long __stdcall AddRef(void);
	public: virtual unsigned long __stdcall Release(void);
	public: virtual long __stdcall Lock(struct HLOCK__ * *);
	public: virtual long __stdcall Unlock(struct HLOCK__ * *);
	public: virtual long __stdcall InsertRecord(void *);
	public: virtual long __stdcall UpdateRecord(void *);
	public: virtual long __stdcall DeleteRecord(void *);
	public: virtual long __stdcall FindRecord(unsigned long,unsigned long,void *,unsigned long *);
	public: virtual long __stdcall GetRowOrdinal(unsigned long,void *,unsigned long *);
	public: virtual long __stdcall FreeRecord(void *);
	public: virtual long __stdcall GetUserData(void *,unsigned long);
	public: virtual long __stdcall SetUserData(void *,unsigned long);
	public: virtual long __stdcall GetRecordCount(unsigned long,unsigned long *);
	public: virtual long __stdcall GetIndexInfo(unsigned long,char * *,struct tagTABLEINDEX *);
	public: virtual long __stdcall ModifyIndex(unsigned long,char const *,struct tagTABLEINDEX const *);
	public: virtual long __stdcall DeleteIndex(unsigned long);
	public: virtual long __stdcall CreateRowset(unsigned long,unsigned long,struct HROWSET__ * *);
	public: virtual long __stdcall SeekRowset(struct HROWSET__ *,enum  tagSEEKROWSETTYPE,long,unsigned long *);
	public: virtual long __stdcall QueryRowset(struct HROWSET__ *,long,void * *,unsigned long *);
	public: virtual long __stdcall CloseRowset(struct HROWSET__ * *);
	public: virtual long __stdcall CreateStream(unsigned long *);
	public: virtual long __stdcall DeleteStream(unsigned long);
	public: virtual long __stdcall CopyStream(struct IDatabase *,unsigned long,unsigned long *);
	public: virtual long __stdcall OpenStream(enum  tagACCESSTYPE,unsigned long,struct IStream * *);
	public: virtual long __stdcall ChangeStreamLock(struct IStream *,enum  tagACCESSTYPE);
	public: virtual long __stdcall RegisterNotify(unsigned long,unsigned long,unsigned long,struct IDatabaseNotify *);
	public: virtual long __stdcall DispatchNotify(struct IDatabaseNotify *);
	public: virtual long __stdcall SuspendNotify(struct IDatabaseNotify *);
	public: virtual long __stdcall ResumeNotify(struct IDatabaseNotify *);
	public: virtual long __stdcall UnregisterNotify(struct IDatabaseNotify *);
	public: virtual long __stdcall LockNotify(unsigned long,struct HLOCK__ * *);
	public: virtual long __stdcall UnlockNotify(struct HLOCK__ * *);
	public: virtual long __stdcall GetTransaction(struct HTRANSACTION__ * *,enum  tagTRANSACTIONTYPE *,void *,void *,unsigned long *,struct tagORDINALLIST *);
	public: virtual long __stdcall MoveFileA(unsigned short const *);
	public: virtual long __stdcall SetSize(unsigned long);
	public: virtual long __stdcall Repair(void);
	public: virtual long __stdcall Compact(struct IDatabaseProgress *,unsigned long);
	public: virtual long __stdcall HeapAllocate(unsigned long,unsigned long,void * *);
	public: virtual long __stdcall HeapFree(void *);
	public: virtual long __stdcall GenerateId(unsigned long *);
	public: virtual long __stdcall GetClientCount(unsigned long *);
	public: virtual long __stdcall GetFile(unsigned short * *);
	public: virtual long __stdcall GetSize(unsigned long *,unsigned long *,unsigned long *,unsigned long *);
};
