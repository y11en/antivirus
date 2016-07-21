typedef struct tag_LOCALDATA {
#if !defined(_NO_PRAGUE_)
	hCRITICAL_SECTION hCriticalSection;
	hMUTEX  hMutex;
#else
#if defined (__unix__)
        int LockFile;
#else
        HANDLE LockFile; 
#endif
#endif
	tCHAR	szTempFile[MAX_PATH*2];
	HANDLE	hTempFile;
	HANDLE	hFileMapping;
	VOID*   pMappingView;
	SFDB_HDR* pDB;
	int ( pr_call *RecordCompareFunc ) ( const void *, const void *);
	tDWORD ( pr_call *GetIndex) (struct tag_LOCALDATA* ldata, tVOID* pRecordID);
	tDWORD  dwSerializeOptions;
	tDWORD  dwCryptKey;
	tBOOL	bWin9xSystem;
	tDWORD  dwCurrentDateTimeGetCounter;
	tDWORD  dwCurrentDate;
	tDWORD  dwUnusedRecordsLimitTime;
//	tDWORD  dwUnusedRecordsLimitPercent;
	tDWORD  dwDatabaseSizeLimit;
	tCHAR   szSFDBPathName[MAX_PATH*2];
	tCHAR   szSFDBSharedName[0x40];
	tDWORD  dwMapViewSize;
	LARGE_INTEGER liSyncOverhit;
	tDWORD  dwSyncCalls;
	HSA*    pHSA;
} LOCALDATA, *PLOCALDATA;

