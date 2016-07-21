typedef enum tagINSTALLSTATE
{
	INSTALLSTATE_NOTUSED      = -7,  // component disabled
	INSTALLSTATE_BADCONFIG    = -6,  // configuration data corrupt
	INSTALLSTATE_INCOMPLETE   = -5,  // installation suspended or in progress
	INSTALLSTATE_SOURCEABSENT = -4,  // run from source, source is unavailable
	INSTALLSTATE_MOREDATA     = -3,  // return buffer overflow
	INSTALLSTATE_INVALIDARG   = -2,  // invalid function argument
	INSTALLSTATE_UNKNOWN      = -1,  // unrecognized product or feature
	INSTALLSTATE_BROKEN       =  0,  // broken
	INSTALLSTATE_ADVERTISED   =  1,  // advertised feature
	INSTALLSTATE_REMOVED      =  1,  // component being removed (action state, not settable)
	INSTALLSTATE_ABSENT       =  2,  // uninstalled (or action state absent but clients remain)
	INSTALLSTATE_LOCAL        =  3,  // installed on local drive
	INSTALLSTATE_SOURCE       =  4,  // run from source, CD or net
	INSTALLSTATE_DEFAULT      =  5,  // use default, local or source
} INSTALLSTATE;

typedef enum _OS_TYPE{
	OS_9X,
	OS_NT,
	OS_ALL,
}OS_TYPE;

typedef enum _ACTION_TYPE{
	ROOT_ADD,
	PATH_SAVE_RET,
	INI_ADD,
	REG_ADD,
	STARTUP_ADD,
}ACTION_TYPE;

typedef struct _StartUpObjectList{
	OS_TYPE				OsType;
	ACTION_TYPE			ActionType;
//	pfnHKCRWriteType	pHKCRWriteType;
	char				chString[100];
	DWORD				dParam;
}StartUpObjectList,*pStartUpObjectList;

#define	PARAM_NOTHING					0x0
#define SUBITEM_NEEDED					0x4
#define HKEY_LOCAL_MACHINE_BASE			0x200
#define HKEY_CURRENT_USER_BASE			0x400
#define HKEY_USERS_BASE					0x800

#define LONG_PROCESS_OK					0
#define LONG_PROCESS_ERROR				1
#define LONG_PROCESS_UNKNOWN_API		2
#define LONG_PROCESS_STOPPED			3

DWORD	WriteStartUpInfo(HWND ProgressItemHWnd,pCOUNTSTRUCT CountStruct,HANDLE hEv);
void	_GetStartUpMenuNT(void);
void	_GetStartUpMenu9x(void);
DWORD	ReadAllKeyValues(DWORD cValues,HKEY AddrHkey,HWND ProgressItemHWnd,pCOUNTSTRUCT CountStruct,HANDLE hEv);
DWORD	ReadRegistryValues(HWND ProgressItemHWnd,pCOUNTSTRUCT CountStruct,const char SubKey[],HKEY BaseAddr,const bool NeedSubKeys,HANDLE hEv);
bool	ReadKeyValue(char KeyPath[],char NameValue[],HKEY BaseKeyEnter,DWORD DataSize,char* pData);
bool	iFindLongPath(LPCSTR szIn, LPSTR szOut);
void	ConvertToLongName(LPSTR szShortFileName,char** pLongFileName);
void	ReadSystemProfile(const char SectionName[], const char FileName[]);
void	ListFilesStartUp(LPCTSTR StartUpPath);
DWORD	Count(const char* KeyName, HKEY KeyBase, bool SubKeyCount);
char*	_GetIniPath(const char IniName[],char IniPath[MAX_PATH]);



