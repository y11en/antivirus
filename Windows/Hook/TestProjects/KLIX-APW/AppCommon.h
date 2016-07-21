#ifndef _AppCommon_h_included_laskdfkljasdf
#define _AppCommon_h_included_laskdfkljasdf

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

#define DEFINE_USER_ERROR_CODE(name,val) const DWORD name=val|0x20000000; 		
#define IS_USER_DEFINED_ERROR_CODE(n)	(n & 0x20000000)
#define GET_USER_DEFINED_ERROR_CODE(n)	(n & ~(0x20000000))

#define CLEAR_POINTER(ptr) {if(ptr){ delete (ptr); (ptr)= NULL; }}
#define CLOSE_HANDLE(h) { CloseHandle(h); h = INVALID_HANDLE_VALUE; }
#define SIZE_OF(array) (sizeof(array))/sizeof((array)[0])
#define RELEASE_INTERFACE(ptr) {if(ptr){ (ptr)->Release(); (ptr)=NULL; }}
#define INIT_STRING(str) {if(str) str[0]=0;}
#define PURE_BOOL(var) (var)?TRUE:FALSE
#define ZERO_OBJECT(obj) ZeroMemory(&obj,sizeof(obj))

#define RC_MOVE_TO_PTR(rc, ptr) rc.OffsetRect(ptr.x-rc.left, ptr.y-rc.top)
#define RECT_EXPAND(rc) rc.left,rc.top,rc.Width(),rc.Height() 
															  
#define IS_CTRL_PRESSED() (GetKeyState(VK_CONTROL) & 0xF000)
//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// strSysError can be use within macros block 
#define SYS_ERROR_BEGIN {\
	LPTSTR strSysError;\
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,\
		NULL, GetLastError(), 0, (LPTSTR)&strSysError, 0,	NULL);

#define SYS_ERROR_END  LocalFree( strSysError ); }
#define OBJECT_EXISTS(strPathname) (0xFFFFFFFF!=GetFileAttributes(strPathname))
#define FILE_OBJECT_EXISTS(strPathname) (0==(FILE_ATTRIBUTE_DIRECTORY&GetFileAttributes(strPathname)))

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

#include "unknwn.h"

class CComAutoRelease
{									  
	IUnknown* m_pUnk;
public:
	CComAutoRelease(IUnknown* pUnk) :  m_pUnk(pUnk){}
	~CComAutoRelease(){	RELEASE_INTERFACE(m_pUnk); }
};

class CHandleAutoClose
{
	HANDLE& m_rHandle;
public:
	CHandleAutoClose(HANDLE& rHandle) : m_rHandle(rHandle){}
	~CHandleAutoClose(){ CLOSE_HANDLE(m_rHandle); }
};

class CCritSecAutoLock
{
	CRITICAL_SECTION* m_pCS;
public:
	CCritSecAutoLock(CRITICAL_SECTION* i_pCS) : m_pCS(i_pCS) { EnterCriticalSection(m_pCS); }
	~CCritSecAutoLock() { LeaveCriticalSection(m_pCS); }
};

class CFocusPreserver
{
	HWND m_hWnd;
public:
	CFocusPreserver(){ m_hWnd = ::GetFocus(); }
	~CFocusPreserver(){ if(m_hWnd) ::SetFocus(m_hWnd); }
};

class CAutoFlag
{
	BOOL& m_bFlag;
public:
	CAutoFlag(BOOL& i_bFlag) : m_bFlag(i_bFlag) { m_bFlag = TRUE; }
	~CAutoFlag() { m_bFlag = FALSE; }
};

#define COM_AUTO_RELEASE(cobj) CComAutoRelease ComAutoRelease##obj(cobj)
#define HANDLE_AUTO_CLOSE(hobj) CHandleAutoClose HandleAutoClose##obj(hobj)
#define ENTER_AUTO_LOCK(lockobj) CCritSecAutoLock AutoLock##obj(lockobj)
#define PRESERVE_FOCUS CFocusPreserver objFocusPreserver
#define AUTO_FLAG(flagobj) CAutoFlag objAutoFlag##flagobj(flagobj)

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
void AppGetTempName(TCHAR* strBuf, DWORD nChars, TCHAR* pExt = NULL);
void AppGetTempPath(TCHAR* strBuf, DWORD nChars, TCHAR* pExt = NULL);
void AppGetModuleName(TCHAR* strBuf, DWORD nChars, HMODULE i_hModule=NULL);
void AppGetModulePath(TCHAR* strBuf, DWORD nChars, HMODULE i_hModule=NULL);

void FormatRegionalDate(LPTSTR o_strBuf, int i_BufLen, 
		SYSTEMTIME& i_dt, BOOL i_bTime, BOOL i_bDate, BOOL i_bDateShort);

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
class CFuncTimer
{
	LPCTSTR m_strFuncName;
	DWORD	m_dwFuncEnterTicks;
public:
	CFuncTimer(LPCTSTR i_strFuncName);
	~CFuncTimer();
};

#ifdef _DEBUG
#define FUNC_TIMER(strFuncName) \
	CFuncTimer CFuncTimerObj(strFuncName)
#else
#define FUNC_TIMER(strFuncName) 
#endif

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
#ifdef _TRACE_STACK_

class CStackTracer
{
	struct SThreadStack
	{
		LPTSTR* m_ppStackDescr;
		UINT	m_nThreadId;
		TCHAR   m_strThreadDescription[128];
		int		m_nCurUnrollingLevel;
	};

	CRITICAL_SECTION m_csStacks;
	SThreadStack*	m_pStacks;
	int				m_nStacks;
	const int		m_cnStackItemsMax;

	void Cleanup();
	SThreadStack* FindThreadStack(UINT i_nThreadId);

public:
	CStackTracer(int i_nStacks);
	~CStackTracer();

	void RegisterThread(LPCTSTR i_strThreadDesciption);
	BOOL Push(LPCTSTR i_strFuncName);
	void Pop();

	LPCTSTR InitStackUnrolling();
	LPCTSTR GetNextStackItem();
};

class CAutoStackTracer
{
	CStackTracer* m_pStackTracer;
public:
	CAutoStackTracer(CStackTracer* i_pStackTracer, LPCTSTR i_strStackItem)
	{ m_pStackTracer = i_pStackTracer; if(m_pStackTracer) m_pStackTracer->Push(i_strStackItem); }
	virtual ~CAutoStackTracer()
	{ if(m_pStackTracer) m_pStackTracer->Pop(); }
};

#define _TRACE_STACK(ptrStackTracer,strStackItem) \
	CAutoStackTracer CAutoStackTracerObj(ptrStackTracer,strStackItem)

#else
#define _TRACE_STACK(ptrStackTracer,strStackItem) 
#endif

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

namespace CMN
{
	void* LoadFromFile(LPCTSTR i_strPathname, DWORD& o_dwRead);
	BOOL LoadFromFile(LPCTSTR i_strPathname, void* i_pBuf, DWORD& io_dwRead);
	BOOL ReadFromHandle(HANDLE i_hFile, LONG i_lPosition, DWORD i_dwMoveMethod, void* i_pBuf, DWORD i_dwBufLen);
	BOOL SaveToFile(LPCTSTR i_strPathname, void* i_pBuf, DWORD i_dwSize);
	BOOL GetFileSize(LPCTSTR i_strPathname, DWORD& o_dwSize);
	BOOL RegisterComDll(LPCTSTR i_strPathname);
	BOOL CopyFile(HANDLE i_hFrom, HANDLE i_hTo);

	inline BOOL IsWhiteSpaceChar(TCHAR i_chTest)
	{
		return 
		i_chTest==_T(' ') ||
		i_chTest==_T('\r') ||
		i_chTest==_T('\n') ||
		i_chTest==_T('\t');
	}

}; // CMN

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// get hex string, decode to bin form and save to file
// '123e4d...' we must convert to 123e4d... in numeric form
BOOL WriteHexObject(HANDLE hFile, char* strHexObject);

// read file and store each byte in hex-two-symbol form string
BOOL ReadHexObject(char*& strHexObject, HANDLE hFile);

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// scan input string and replace &, <, >, ', " with &amp; , &lt; , &gt; , &apos; , &qout; respectively
BOOL XmlNormalizeString	(LPCTSTR i_strInput, LPTSTR o_strOutput, DWORD i_dwOutputSize);

struct STokenReplace
{
	LPCTSTR m_strFirst;
	LPTSTR	m_strSecond;
};

// calculate output size with replaced tokens
int  ReplaceTokensCalcSize(LPCTSTR i_strInput, STokenReplace* i_pTokens, int i_nTokensCount);
void ReplaceTokens(LPCTSTR i_strInput, LPTSTR o_strOutput, STokenReplace* i_pTokens, int i_nTokensCount);

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// mfc section

#ifdef _AFX
#define _USE_CSTRING
#endif

#ifdef __AFXSTR_H__
#ifndef _USE_CSTRING
#define _USE_CSTRING
#endif
#endif

#ifdef __ATLSTR_H__
#ifndef _USE_CSTRING
#define _USE_CSTRING
#endif
#endif

#ifdef _USE_CSTRING

void ReplaceTokens(LPCTSTR i_strInput, CString& o_strOutput, STokenReplace* i_pTokens, int i_nTokensCount);
void XmlNormalizeString	(LPCTSTR i_strInput, CString& o_strOutput);
void SqlNormalizeString	(LPCTSTR i_strInput, CString& o_strOutput);
void CutOffFileName(CString& io_strPathname, CString& o_strName);	
void SplitPathname(LPCTSTR i_strPathname, CString& o_strPath, CString& o_strName);

#define APPEND_DIR_ENDING_MARK(str) \
	{if(str.GetLength() && _T('\\') != str[str.GetLength()-1]) str += _T("\\");}

#define REMOVE_DIR_ENDING_MARK(str) \
	{if(str.GetLength() && _T('\\') == str[str.GetLength()-1]) str.GetBufferSetLength(str.GetLength()-1);}

#define APPEND_WEB_DIR_ENDING_MARK(str) \
	{if(str.GetLength() && _T('/') != str[str.GetLength()-1]) str += _T("/");}

#define REMOVE_WEB_DIR_ENDING_MARK(str) \
	{if(str.GetLength() && _T('/') == str[str.GetLength()-1]) str.GetBufferSetLength(str.GetLength()-1);}


// запихивание \ выковыривание из строки
CString& operator >> (CString& str, float&);
CString& operator >> (CString& str, double&);
CString& operator >> (CString& str, CString&);
CString& operator >> (CString& str, WORD&);
CString& operator >> (CString& str, short&);
CString& operator >> (CString& str, DWORD&);
CString& operator >> (CString& str, LONG&);
CString& operator >> (CString& str, int&);
CString& operator >> (CString& str, BYTE&);

#ifdef _AFX
CString& operator >> (CString& str, CTime&);
#endif

CString& operator << (CString& str, float);
CString& operator << (CString& str, double);
CString& operator << (CString& str, CString);
CString& operator << (CString& str, WORD);
CString& operator << (CString& str, short);
CString& operator << (CString& str, DWORD);
CString& operator << (CString& str, int);
CString& operator << (CString& str, BYTE);

#ifdef _AFX
CString& operator << (CString& str, CTime);

void  operator >> (CString& str, CStringList& list);
void  operator << (CString& str, CStringList& list);
void  operator << (CFile& file, CStringList& list);
void  NormalizeFileObjectName(CString& o_strName, LPCTSTR i_strName);

CView*	AppFindViewWith(CDocument* pDoc, void* pData, BOOL pfunc(CView*, void*));

#endif

#endif // _USE_CSTRING

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
template<class T>	void Exchange(T& v1, T& v2)
{
	T v;
	v = v1;
	v1 = v2;
	v2 = v;
}


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// type of datetime : Thu, 25 Jul 2002 17:04:19 +0400
BOOL ConvertDatetime(SYSTEMTIME& o_oSysTime, char* i_strTime);

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

BOOL IsRunningOnHost(LPCSTR i_strHostName);

// make folder and check existance of each subdirectory
BOOL CreateLocalDirectoryDeep(LPCTSTR i_strDir);

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
class CAppRegistryExt
{
public:
	typedef BOOL (*pKeyCheckerFn)(LPCTSTR i_strKeyName, HKEY i_hKey, void* i_pData);
	static BOOL FindFirstKey(HKEY hKey, BOOL i_bRecursive, pKeyCheckerFn i_pKeyChecker, void* i_pData); 

	// supports opening sub sections divided by '\'
	// for instance 
	// section1\section2\...\sectionN
	static HKEY GetSectionKey(HKEY i_hKey, LPCTSTR i_lpszSection);

	static HKEY GetSystemEnumKey();
};

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

interface IFileStream : public IStream
{
	LONG  m_dwReffs;
	HANDLE m_hFile;
	IFileStream(LPCTSTR i_strPathname, BOOL i_bRead);
	IFileStream(HANDLE i_hFile);
	~IFileStream();

	virtual HRESULT __stdcall QueryInterface(struct _GUID const &,void **);
	virtual ULONG	__stdcall AddRef(void);
	virtual ULONG	__stdcall Release(void);
    virtual HRESULT __stdcall Read(void *pv, ULONG cb, ULONG *pcbRead);	
    virtual HRESULT __stdcall Write(const void *pv, ULONG cb, ULONG *pcbWritten);
    virtual HRESULT __stdcall Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER *plibNewPosition);
    virtual HRESULT __stdcall SetSize(ULARGE_INTEGER libNewSize); 
    virtual HRESULT __stdcall CopyTo(IStream __RPC_FAR *pstm, ULARGE_INTEGER cb, ULARGE_INTEGER *pcbRead, ULARGE_INTEGER *pcbWritten);
    virtual HRESULT __stdcall Commit(DWORD grfCommitFlags );
    virtual HRESULT __stdcall Revert();
    virtual HRESULT __stdcall LockRegion( ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType);
    virtual HRESULT __stdcall UnlockRegion( ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType);
    virtual HRESULT __stdcall Stat(STATSTG *pstatstg,  DWORD grfStatFlag);
    virtual HRESULT __stdcall Clone(IStream **ppstm);
};

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
#ifdef _AFX_TEMPLATES
#include "afxtempl.h"

class CIntList : public CList<int,int&> 
{
public:
	void operator = (CIntList& i_lstOther);
};

#endif // _DEFINE_STD_TEMPLATES

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

#endif
