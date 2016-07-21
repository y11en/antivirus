
#define CACHE_FILE_FLUSH_STATE_NOT_MAPPED        0
#define CACHE_FILE_FLUSH_STATE_MAPPED            1
#define CACHE_FILE_FLUSH_STATE_MAPPED_MODIFIED   2
	
typedef struct TFileData_
{
	DWORD                   Magic;
    HANDLE                  Handle;  //handle of physical file
    ULONG                   CurrPos; //virtual position
    ULONG                   CurrLen; //virtual length
    ULONG                   RealPos; //real position physical file
    ULONG                   InitPos; //initial position physical file
	DWORD					FlushState; // bit 1-in memory, bit 2-mofified in memory
	BOOL					WriteChecked;
	BOOL					WriteBackFailed;
	BOOL                    NoClose;
	BOOL					SetEOF;

#ifdef FPIAPI_VER_MAJOR
	ULONG                   HandleFS;
#endif
}TFileData;

//fHandleFS defines
#define FS_PRAGUE  0
#define FS_WIN32   1
#define FS_FPI     2

