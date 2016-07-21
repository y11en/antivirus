//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

#include "stdafx.h"
#include "AppCommon.h"
#include "time.h"
#include "io.h"
#include "malloc.h"

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

#ifdef _DEBUG 
#ifndef DISABLE_DEBUG_NEW
#define new DEBUG_NEW				    
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

#ifdef _USE_CSTRING

static CString s_dividers(_T("|,:;"));

inline int DecodeIntVal(CString str)
{
	TCHAR *ScanSt = str.GetBuffer(str.GetLength()), *ScanFi;

#ifdef _UNICODE
	return wcstol(ScanSt, &ScanFi, 10);
#else
	return strtol(ScanSt, &ScanFi, 10);
#endif
}

CString& operator >> (CString& str, float& val)
{
	double tmp;
	str = operator >> (str, tmp);
	val = float(tmp);
	return str;
}

CString& operator >> (CString& str, double& val)
{
	if(!str.IsEmpty())	
	{
		CString strVal;
		int i = str.FindOneOf(s_dividers);
		if(i==-1)	strVal = str;
		else			strVal = str.Left(i);

		double tmp;
		TCHAR *ScanSt = strVal.GetBuffer(strVal.GetLength()), *ScanFi;
#ifdef _UNICODE
		tmp = wcstod(ScanSt, &ScanFi);
#else
		tmp = strtod(ScanSt, &ScanFi);
#endif
		val = tmp;

		str = i==-1 ? _T("") : str.Right(str.GetLength()-i-1);
	}
	return str;
}


CString& operator >> (CString& str, int& val)
{
	if(!str.IsEmpty())	
	{
		int i = str.FindOneOf(s_dividers);
		CString strVal = (i==-1) ?	str :	str.Left(i);
		val = int(DecodeIntVal(strVal));
		str = i==-1 ? _T("") : str.Right(str.GetLength()-i-1);
	}
	return str;
}

CString& operator >> (CString& str, BYTE& val)
{
	if(!str.IsEmpty())	
	{
		int i = str.FindOneOf(s_dividers);
		CString strVal = (i==-1) ?	str :	str.Left(i);
		val = BYTE(DecodeIntVal(strVal));
		str = i==-1 ? _T("") : str.Right(str.GetLength()-i-1);
	}
	return str;
}

CString& operator >> (CString& str, CString& val)
{
	if(!str.IsEmpty())	
	{
		int i = str.FindOneOf(s_dividers);
		val = (i==-1) ?	str :	str.Left(i);
		str = i==-1 ? _T("") : str.Right(str.GetLength()-i-1);
	}
	return str;
}

CString& operator >> (CString& str, WORD& val)
{
	if(!str.IsEmpty())	
	{
		int i = str.FindOneOf(s_dividers);
		CString strVal = (i==-1) ?	str :	str.Left(i);
		val = WORD(DecodeIntVal(strVal));
		str = i==-1 ? _T("") : str.Right(str.GetLength()-i-1);
	}
	return str;
}

CString& operator >> (CString& str, short& val)
{
	if(!str.IsEmpty())	
	{
		int i = str.FindOneOf(s_dividers);
		CString strVal = (i==-1) ?	str :	str.Left(i);
		val = short(DecodeIntVal(strVal));
		str = i==-1 ? _T("") : str.Right(str.GetLength()-i-1);
	}
	return str;
}

CString& operator >> (CString& str, LONG& val)
{
	if(!str.IsEmpty())	
	{
		int i = str.FindOneOf(s_dividers);
		CString strVal = (i==-1) ?	str :	str.Left(i);
		val = LONG(DecodeIntVal(strVal));
		str = i==-1 ? _T("") : str.Right(str.GetLength()-i-1);
	}
	return str;
}

CString& operator >> (CString& str, DWORD& val)
{
	if(!str.IsEmpty())	
	{
		int i = str.FindOneOf(s_dividers);
		CString strVal = (i==-1) ?	str :	str.Left(i);
		val = DWORD(DecodeIntVal(strVal));
		str = i==-1 ? _T("") : str.Right(str.GetLength()-i-1);
	}
	return str;
}


#ifdef _AFX
CString& operator >> (CString& str, CTime& val)
{												 
	int year, month, day, hour, minute, second;
	str >> year >> month >> day >> hour >> minute >> second;
	val = CTime(year, month, day, hour, minute, second);
	return str;
}
#endif

inline CString CodeIntVal(int val)
{
	CString ret;
	ret.Format(_T("%d"), val);
	return ret;
}

inline void AddDivider(CString& str, TCHAR ch=',')
{
	if(!str.IsEmpty() && s_dividers.Find(str[str.GetLength()-1]) == -1)
		str += ch;
}

CString& operator << (CString& str, double val)
{
	AddDivider(str);
	CString strVal;
	strVal.Format(_T("%f"), val);
	str += strVal;
	return str;
}

CString& operator << (CString& str, float val)
{
	AddDivider(str);
	CString strVal;
	strVal.Format(_T("%f"), val);
	str += strVal;
	return str;
}

CString& operator << (CString& str, int val)
{
	AddDivider(str);
	str += CodeIntVal(val);
	return str;
}

CString& operator << (CString& str, BYTE val)
{
	AddDivider(str);
	str += CodeIntVal(val);
	return str;
}

CString& operator << (CString& str, CString val)
{
	// разделитель ?
	if( val.GetLength()==1 && s_dividers.Find(val[0]) != -1 )
	{
		// уже есть разделитель ?
		if(!str.IsEmpty() && s_dividers.Find(str[str.GetLength()-1]) != -1)
			str.GetBufferSetLength(str.GetLength()-1);
	}
	else
		AddDivider(str);

	str += val;
	return str;
}

CString& operator << (CString& str, short val)
{
	AddDivider(str);
	str += CodeIntVal(val);
	return str;
}

CString& operator << (CString& str, WORD val)
{
	AddDivider(str);
	str += CodeIntVal(val);
	return str;
}

CString& operator << (CString& str, DWORD val)
{
	AddDivider(str);
	str += CodeIntVal(val);
	return str;
}

void CutOffFileName(CString& io_strPathname, CString& o_strName)
{
	SplitPathname(io_strPathname, io_strPathname, o_strName);
}

void SplitPathname(LPCTSTR i_strPathname, CString& o_strPath, CString& o_strName)
{
	TCHAR drive	[_MAX_DRIVE];
	TCHAR dir	[_MAX_DIR];
	TCHAR fname	[_MAX_FNAME];
	TCHAR ext	[_MAX_EXT];
	
#ifdef _UNICODE
	_wsplitpath( i_strPathname, drive, dir, fname, ext );
#else
	_splitpath( i_strPathname, drive, dir, fname, ext );
#endif
	
	TCHAR szTmp[1024];
	wsprintf(szTmp, _T("%s%s"), drive, dir);

	o_strPath = szTmp;

	wsprintf(szTmp, _T("%s%s"), fname, ext);
	o_strName = szTmp;
}

#ifdef _AFX
CString& operator << (CString& str, CTime val)
{
	AddDivider(str, '|');
	str << val.GetYear() << val.GetMonth() << val.GetDay() << val.GetHour() << val.GetMinute();
	str << val.GetSecond();
	AddDivider(str, '|');
	return str;
}

void  operator >> (CString& str, CStringList& list)
{
  list.RemoveAll();

	while(!str.IsEmpty())	
	{
		int i = str.FindOneOf(s_dividers);
		CString strVal = (i==-1) ?	str :	str.Left(i);
    list.AddTail(strVal);
		str = i==-1 ? _T("") : str.Right(str.GetLength()-i-1);
	}
}

void  operator << (CString& str, CStringList& list)
{
  str.Empty();
  POSITION pos = list.GetHeadPosition();
  while(pos)
  {
    if(!str.IsEmpty()) str += _T("|");
    str += list.GetNext(pos);
  }
}

// remove unsupported symbols from source
void  NormalizeFileObjectName(CString& o_strName, LPCTSTR i_strName)
{
	STokenReplace oTokens[] = 
	{
		{_T(":"),  _T("_")	},
		{_T("\""),  _T("_")	},
		{_T("/"),  _T("_")	},
		{_T("|"),  _T("_")	},
		{_T("*"),  _T("_")	},
		{_T("?"),  _T("_")	},
		{_T("\\"),  _T("_")	},
		{_T("\r"),  _T("_")	},
		{_T("\n"),  _T("_")	},
		{_T("\t"),  _T("_")	},
		{_T(" "),  _T("_")	}
	};

	ReplaceTokens(i_strName, o_strName, &oTokens[0], sizeof(oTokens) / sizeof(oTokens[0]));
}

void  operator << (CFile& file, CStringList& list)
{
  file.SeekToBegin();

  POSITION pos = list.GetHeadPosition();
  while(pos)
  {
    CString str = list.GetNext(pos);
    str += _T("\r\n");
    file.Write(str.GetBuffer(str.GetLength()), str.GetLength());
  }
}

CView*	AppFindViewWith(CDocument* pDoc, void* pData, BOOL pfunc(CView*, void*))
{
	POSITION pos = pDoc->GetFirstViewPosition();
	CView* pRetView = 0;
	while(pos)
	{
		pRetView = pDoc->GetNextView(pos);
		if(pfunc(pRetView, pData))
			break;
		pRetView = 0;
	}
	return pRetView;
}

#endif // _AFX
#endif // _USE_CSTRING

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// type of datetime : Thu, 25 Jul 2002 17:04:19 +0400 or 2 Oct 2002 00:30:01 +0400
BOOL ConvertDatetime(SYSTEMTIME& o_oSysTime, char* i_strTime)
{
	ZeroMemory(&o_oSysTime, sizeof(o_oSysTime));

	if(NULL == i_strTime)
		return FALSE;

	char s_month[][4] = { "jan", "feb", "mar", "apr", "may", "jun", "jul", "aug", "sep", "oct", "nov", "dec" };
	char s_day[][4] =  { "sun", "mon", "tue", "wed", "thu", "fri", "sat" };

	// make lower characters

	char strTime[128];
	char strBuffer[128];
	lstrcpynA(strTime, i_strTime, 128);
	_strlwr(strTime);

	int nYear, nMonth, nDay, nDayOfWeek, nHour, nMinute, nSec;

	char* strRest = strTime;

	while(' ' == strRest[0]) ++strRest;
	if(3 > lstrlenA(strRest))
		return FALSE;

	lstrcpynA(strBuffer, strRest, 4);
	nDayOfWeek = -1;

	// day of the week
	for(int i=0; i<7; i++)
		if( 0 == lstrcmpiA(strBuffer, s_day[i]) )
		{
			nDayOfWeek = i;
			break;
		}

	// heading day of the week can be absent
	if(-1 != nDayOfWeek)		
		strRest += 4;

	while(' ' == strRest[0]) ++strRest;

	// day in month
	if(0 == lstrlenA(strRest))
		return FALSE;

	char* ScanFi;
	nDay = WORD(strtol(strRest, &ScanFi, 10));

	if( nDay < 1 || nDay > 31)
		return FALSE;

	strRest = ScanFi; 
	while(' ' == strRest[0]) ++strRest;

	// month
	if(3 > lstrlenA(strRest))
		return FALSE;

	strRest[3] = 0;

	nMonth = -1;

	for(i=0; i<12; i++)
		if( 0 == lstrcmpiA(strRest, s_month[i]) )
		{
			nMonth = i + 1;
			break;
		}

	if(-1 == nMonth)		
		return FALSE;

	strRest += 4;
	while(' ' == strRest[0]) ++strRest;

	// year
	if(0 == lstrlenA(strRest))
		return FALSE;

	nYear = WORD(strtol(strRest, &ScanFi, 10));
	strRest = ScanFi; 
	while(' ' == strRest[0]) ++strRest;

	// hour

	if(0 == lstrlenA(strRest))
		return FALSE;
	nHour = WORD(strtol(strRest, &ScanFi, 10));
	strRest = ScanFi; 
	while(':' == strRest[0]) ++strRest;

	// minute
	if(0 == lstrlenA(strRest))
		return FALSE;

	nMinute = WORD(strtol(strRest, &ScanFi, 10));
	strRest = ScanFi; 
	while(':' == strRest[0]) ++strRest;

	// second
	if(0 == lstrlenA(strRest))
		return FALSE;

	nSec = WORD(strtol(strRest, &ScanFi, 10));
	strRest = ScanFi; 
	while(' ' == strRest[0]) ++strRest;

	struct tm atm;
	atm.tm_sec = nSec;
	atm.tm_min = nMinute;
	atm.tm_hour = nHour;
	atm.tm_mday = nDay;
	atm.tm_mon = nMonth - 1;        // tm_mon is 0 based

/*	if(nYear >= 1900) 
		nYear -= 1900;
	else 
	if(nYear < 100)
		nYear += 100;
*/
	if(nYear >= 1980) 
		nYear -= 1980;
	else 
	if(nYear < 100)
		nYear += 20;


//	atm.tm_year = nYear;			// tm_year is 1900 based
	atm.tm_year = nYear;			// tm_year is 1980 based
	atm.tm_isdst = -1;
	time_t oTime = mktime(&atm);

	if(oTime == -1)
		return FALSE;

	// hour offset / this can be missed field
	if(lstrlenA(strRest))
	{
		long hOffset = 0;
		int nCurrentZone = 4;

		if(0 != lstrcmpA("gmt", strRest))
		{
			hOffset = strtol(strRest, &ScanFi, 10) / 100;

			// if we get some integers
			if(ScanFi == strRest)
				return FALSE;
		}

		TIME_ZONE_INFORMATION oTZ;
		GetTimeZoneInformation(&oTZ);

		nCurrentZone = - (oTZ.Bias + oTZ.DaylightBias) / 60;

		// adjust time zone - add hours offset
		if(hOffset != nCurrentZone)
			//oTime -= (3600 * (hOffset - nCurrentZone));
			oTime = oTime - (3600 * (hOffset - nCurrentZone));
	}

	struct tm* pTime = localtime(&oTime);
	//o_oSysTime.wYear = WORD(pTime->tm_year + 1900);
	o_oSysTime.wYear = WORD(pTime->tm_year + 1980);
	o_oSysTime.wMonth = WORD(pTime->tm_mon + 1);
	o_oSysTime.wDay = WORD(pTime->tm_mday);
	o_oSysTime.wHour = WORD(pTime->tm_hour);
	o_oSysTime.wMinute = WORD(pTime->tm_min);
	o_oSysTime.wSecond = WORD(pTime->tm_sec);
	o_oSysTime.wDayOfWeek = WORD(pTime->tm_wday);

	return TRUE;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
void AppGetTempName(TCHAR* strBuf, DWORD nChars, TCHAR* pExt)
{
	if(!strBuf || !nChars)	return;

	// fix quick calls from one thread
	Sleep(5);

	if(!pExt) pExt = _T("tmp");
	DWORD dwTicktime = (GetTickCount()/10) % 1000000;
	TCHAR strFilename[_MAX_FNAME];

	// fix multi thread query 
	wsprintf(strFilename, _T("tm%d%06ld.%s"), GetCurrentThreadId(), dwTicktime, pExt);
	lstrcpyn(strBuf, strFilename, int(nChars));
}

void AppGetTempPath(TCHAR* strBuf, DWORD nChars, TCHAR* pExt)
{
	GetTempPath(nChars, strBuf);
	int nStrLen = lstrlen(strBuf);
	if(int(nChars) > nStrLen)
		nChars -= nStrLen;
	if(nChars && strBuf[nStrLen-1] != _T('\\')){
		lstrcat(strBuf, _T("\\"));
		-- nChars;
		++ nStrLen;
	}
	
	AppGetTempName(strBuf + nStrLen, nChars, pExt);
}

void AppGetModuleName(TCHAR* strBuf, DWORD nChars, HMODULE i_hModule)
{
	INIT_STRING(strBuf);
	TCHAR strModuleName[4000] = {0};

	if(!GetModuleFileName(i_hModule, strModuleName, sizeof(strModuleName))) 
		return;
	
	for(int i=lstrlen(strModuleName);i>=0;i--) 
	{
		if(strModuleName[i]=='.')
			strModuleName[i]=0;
		if(strModuleName[i]=='\\') 
		{
			lstrcpyn(strBuf, strModuleName+i+1, nChars);
			return;
		}
	}
}

void AppGetModulePath(TCHAR* strBuf, DWORD nChars, HMODULE i_hModule)
{
	INIT_STRING(strBuf);
	TCHAR strModuleName[4000] = {0};

	if(!GetModuleFileName(i_hModule, strModuleName, sizeof(strModuleName)))
		return;
	for(int i=lstrlen(strModuleName);i>=0;i--) 
		if(strModuleName[i]=='\\') 
		{
			strModuleName[i] = 0;
			lstrcpyn(strBuf, strModuleName, nChars);
			return;
		}
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
void FormatRegionalDate(LPTSTR o_strBuf, int i_nBufLen, 
		SYSTEMTIME& i_dt, BOOL i_bTime, BOOL i_bDate, BOOL i_bDateShort)
{
	if(NULL == o_strBuf)
		return;

	DWORD dwFlags;

	if(i_bDateShort)
		dwFlags = DATE_SHORTDATE;
	else
		dwFlags = DATE_LONGDATE;

	if(i_bDate)	
	{
		int nWritten = 
			GetDateFormat(LOCALE_SYSTEM_DEFAULT, dwFlags, &i_dt, NULL, o_strBuf, i_nBufLen);

		// remove terminating null addition
		if(nWritten)
			--nWritten;

		o_strBuf += nWritten;
		i_nBufLen -= nWritten;

		// add space
		if(i_bTime)
		{
			if(i_nBufLen)
			{
				lstrcat(o_strBuf, _T(" "));
				++o_strBuf;
			}
			--i_nBufLen;
		}


		if(i_nBufLen <=0)
			return;
	}

	if(i_bTime)
		GetTimeFormat(LOCALE_SYSTEM_DEFAULT, TIME_NOSECONDS, &i_dt, NULL, o_strBuf, i_nBufLen);
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
void* CMN::LoadFromFile(LPCTSTR i_strPathname, DWORD& o_dwRead)
{
	o_dwRead = 0;

	HANDLE hFile = CreateFile(i_strPathname, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	if(INVALID_HANDLE_VALUE == hFile)
		return NULL;

	DWORD dwToRead = ::GetFileSize(hFile, NULL);
	BYTE* pBuf = new BYTE[dwToRead+2];

	if(!ReadFile(hFile, pBuf, dwToRead, &o_dwRead, NULL))
		CLEAR_POINTER(pBuf);

	CloseHandle(hFile);

	return pBuf;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
BOOL CMN::LoadFromFile(LPCTSTR i_strPathname, void* i_pBuf, DWORD& io_dwRead)
{
	HANDLE hFile = CreateFile(i_strPathname, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	if(INVALID_HANDLE_VALUE == hFile)
		return FALSE;

	DWORD dwToRead = min(::GetFileSize(hFile, NULL), io_dwRead);
	BOOL bRet = ReadFile(hFile, i_pBuf, dwToRead, &io_dwRead, NULL);
	CloseHandle(hFile);

	return bRet;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
BOOL CMN::SaveToFile(LPCTSTR i_strPathname, void* i_pBuf, DWORD i_dwSize)
{
	HANDLE hFile = CreateFile(i_strPathname, GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);
	if(INVALID_HANDLE_VALUE == hFile)
		return NULL;

	DWORD dwWritten;
	BOOL bRet = WriteFile(hFile, i_pBuf, i_dwSize, &dwWritten, NULL) && 
		dwWritten == i_dwSize;

	CloseHandle(hFile);

	return bRet;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
BOOL CMN::CopyFile(HANDLE i_hFrom, HANDLE i_hTo)
{
	BYTE arrBuf[4096];
	for(;;)
	{
		DWORD dwRead, dwWritten;

		if(	!ReadFile(i_hFrom, arrBuf, sizeof(arrBuf), &dwRead, NULL) )
			return FALSE;
			
		if(0 == dwRead)
			break;

		if(!WriteFile(i_hTo,  arrBuf, dwRead, &dwWritten, NULL) || dwRead != dwWritten )
			return FALSE;
	}

	return TRUE;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
BOOL CMN::GetFileSize(LPCTSTR i_strPathname, DWORD& o_dwSize)
{
	o_dwSize = DWORD(-1);

	HANDLE hFile = 
		CreateFile(i_strPathname, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0);

	if(INVALID_HANDLE_VALUE == hFile)
		return FALSE;

	o_dwSize = ::GetFileSize(hFile, NULL);

	CloseHandle(hFile);

	return 0xFFFFFFFF != o_dwSize;
}

BOOL CMN::ReadFromHandle(HANDLE i_hFile, LONG i_lPosition, DWORD i_dwMoveMethod, 
	void* i_pBuf, DWORD i_dwBufLen)
{
	if(0xFFFFFFFF == SetFilePointer(i_hFile, i_lPosition, NULL, i_dwMoveMethod))
		return FALSE;
	DWORD dwRead;
	if(!ReadFile(i_hFile, i_pBuf, i_dwBufLen, &dwRead, NULL))
		return FALSE;

	return dwRead == i_dwBufLen;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// get hex string, decode to bin form and save to file
// '123e4d...' we must convert to 123e4d... in numeric form
BOOL WriteHexObject(HANDLE hFile, char* strHexObject)
{
	BYTE  arrBuf	[512];
	WORD  nBufPtr = 0;
	DWORD dwWritten;
	int   nStrLen = lstrlenA(strHexObject);

	for(int i=0; i<nStrLen && strHexObject[i+1]; i+=2)
	{
		BYTE& rByte = *((BYTE*)strHexObject + i + 2);
		BYTE nSaved = rByte;
		rByte = 0;

		int nByteVal = 0;
#ifdef _UNICODE
		//swscanf(strHexObject + i, L"%lX", &nByteVal);
#else
		//sscanf(strHexObject + i, "%lX", &nByteVal);
#endif

		rByte = nSaved;

		arrBuf[nBufPtr++] = BYTE(nByteVal);

		if(nBufPtr < SIZE_OF(arrBuf))
			continue;

		if(!WriteFile(hFile, arrBuf, SIZE_OF(arrBuf), &dwWritten, 0))
			return FALSE;

		nBufPtr = 0;
	}

	if(nBufPtr && !WriteFile(hFile, arrBuf, nBufPtr, &dwWritten, 0))
			return FALSE;

	return TRUE;
}

// read file and store each byte in hex-two-symbol form string
BOOL ReadHexObject(char*& strHexObject, HANDLE hFile)
{
	DWORD  dwSize = GetFileSize(hFile, NULL), dwRead, dwHexIdx = 0;
	strHexObject  = (char*) new BYTE [2 * dwSize + 2];
	BYTE   arrBuf[512];
					    
	do
	{
		if(!ReadFile(hFile, arrBuf, 512, &dwRead, 0))
		{
			CLEAR_POINTER(strHexObject);
			return FALSE;
		}
	
		for(DWORD i=0; i<dwRead; i++, dwHexIdx += 2)
			wsprintfA(strHexObject+dwHexIdx, "%.2X", int(arrBuf[i]));				

	} while(dwRead);

	return TRUE;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// scan input string and replace &, <, >, ', " with &amp; , &lt; , &gt; , &apos; , &qout; respectively
BOOL XmlNormalizeString	(LPCTSTR i_strInput, LPTSTR o_strOutput, DWORD i_dwOutputSize)
{
	if(!i_dwOutputSize)
		return FALSE;

	STokenReplace oTokens[] = 
	{
		{_T("&"),  _T("&amp;")	},
		{_T("<"),  _T("&lt;")	},
		{_T(">"),  _T("&gt;")	},
		{_T("\'"), _T("&apos;")	},
		{_T("\""), _T("&quot;")	},	
	};

	int nOutputSize = ReplaceTokensCalcSize(i_strInput, &oTokens[0], 5);

	if(nOutputSize > int(i_dwOutputSize))
		return FALSE;

	ReplaceTokens(i_strInput, o_strOutput, &oTokens[0], 5);

	return TRUE;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

// must second fully match to first and first can exceed the second
inline BOOL IsEqualTokens(LPCTSTR i_strFirst, LPCTSTR i_strSecond)
{
	int nSize = lstrlen(i_strSecond);
	for(int i=0; i<nSize; i++)
		if(i_strFirst[i] != i_strSecond[i])
			return FALSE;
		
		return TRUE;
}

// calculate output size with replaced tokens
int ReplaceTokensCalcSize(LPCTSTR i_strInput, STokenReplace* i_pTokens, int i_nTokensCount)
{
	int nInputSize = lstrlen(i_strInput);
	int nOutputSize= 0;
	for(int i=0; i<nInputSize; i++)
	{
		for(int k=0; k<i_nTokensCount; k++)
			if(IsEqualTokens(i_strInput + i, i_pTokens[k].m_strFirst))
				break;
			
			// no token were found
			if(k==i_nTokensCount)
			{
				++nOutputSize;
				continue;
			}
			
			nOutputSize += lstrlen(i_pTokens[k].m_strSecond);
	}
	
	// plus null position
	return nOutputSize+1;
}	

// replace tokens
void ReplaceTokens(LPCTSTR i_strInput, LPTSTR o_strOutput, STokenReplace* i_pTokens, int i_nTokensCount)
{
	int nInputSize = lstrlen(i_strInput);
	for(int i=0, j=0; i<nInputSize; )
	{
		for(int k=0; k<i_nTokensCount; k++)
			if(IsEqualTokens(i_strInput + i, i_pTokens[k].m_strFirst))
				break;
			
			// no token were found
			if(k==i_nTokensCount)
			{
				// ending null included, so max number of copied characters is 2 = one character and one null
				lstrcpyn(o_strOutput + j, i_strInput + i, 2);
				++j;
				++i;
				continue;
			}
			
			lstrcpy(o_strOutput + j, i_pTokens[k].m_strSecond);
			
			i += lstrlen(i_pTokens[k].m_strFirst);
			j += lstrlen(i_pTokens[k].m_strSecond);
	}
}

#ifdef _USE_CSTRING

void ReplaceTokens(LPCTSTR i_strInput, CString& o_strOutput, STokenReplace* i_pTokens, int i_nTokensCount)
{
	o_strOutput.Empty();
	int nNewSize = ReplaceTokensCalcSize(i_strInput, i_pTokens, i_nTokensCount);
	ReplaceTokens(i_strInput, o_strOutput.GetBufferSetLength(nNewSize), i_pTokens, i_nTokensCount);
	o_strOutput.ReleaseBuffer();
}					 

void XmlNormalizeString	(LPCTSTR i_strInput, CString& o_strOutput)
{
	STokenReplace oTokens[] = 
	{
		{_T("&"),  _T("&amp;")	},
		{_T("<"),  _T("&lt;")	},
		{_T(">"),  _T("&gt;")	},
		{_T("\'"), _T("&apos;")	},
		{_T("\""), _T("&quot;")	},	
	};

	ReplaceTokens(i_strInput, o_strOutput, &oTokens[0], 5);
}

void SqlNormalizeString	(LPCTSTR i_strInput, CString& o_strOutput)
{
	STokenReplace oTokens[] = 
	{
		{_T("'"),  _T("''")	},
	};

	ReplaceTokens(i_strInput, o_strOutput, &oTokens[0], 1);
}
#endif // _USE_CSTRING


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

inline int _lstrcmp(LPCSTR szOne, LPCSTR szTwo)
{
	int nLen1 = lstrlenA(szOne);
	int nLen2 = lstrlenA(szTwo);
	
	if(nLen1 != nLen2)
		return 1;

	for(int i=0; i<nLen1; i++)
		if(szOne[i] != szTwo[i])
			return 1;

	return 0;
}

BOOL IsRunningOnHost(LPCSTR i_strHostName)
{
	char strHostName[128];
	DWORD dwSize = SIZE_OF(strHostName);
	GetComputerNameA(strHostName, &dwSize);

	return 0 == _lstrcmp(strHostName, i_strHostName);
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// make folder and check existance of each subdirectory
// !!! input string must be directory path without file name on it's end
BOOL CreateLocalDirectoryDeep(LPCTSTR i_strDir)
{
	int nDirNameLen = lstrlen(i_strDir);
	TCHAR strCurrentDirectory[4000];

	// skip drive letter : i=3
	for(int i=3; i<nDirNameLen; i++)
	{
		// check directory existance at the end of string or at the \ mark
		if(	i != (nDirNameLen - 1) && _T('\\') != i_strDir[i])
			continue;

		lstrcpyn(strCurrentDirectory, i_strDir, i + 1 + 1);

		if(
#ifdef _UNICODE
			0 != _waccess(strCurrentDirectory, 0) && 
#else
			0 != _access(strCurrentDirectory, 0) && 
#endif
			!CreateDirectory(strCurrentDirectory, NULL))
			return FALSE;
	}

	return TRUE;
}


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
#ifdef _TRACE_STACK_
CStackTracer::CStackTracer(int i_nStacks) :
	m_nStacks(i_nStacks),
	m_cnStackItemsMax(1028)
{
	InitializeCriticalSection(&m_csStacks);

	m_pStacks = new SThreadStack[m_nStacks];
	ZeroMemory(m_pStacks, sizeof(SThreadStack) * m_nStacks);
}

CStackTracer::~CStackTracer()
{
	Cleanup();
	DeleteCriticalSection(&m_csStacks);
}

void CStackTracer::Cleanup()
{
	ENTER_AUTO_LOCK(&m_csStacks);

	for(int i=0; i<m_nStacks; i++)
	{
		SThreadStack* pStack = m_pStacks + i;

		// release stack items descriptions
		if(pStack->m_ppStackDescr)
		for(int i=0; pStack->m_ppStackDescr[i] &&
			i<m_cnStackItemsMax; i++)
		{
			LPTSTR& strStackItem = pStack->m_ppStackDescr[i];
			CLEAR_POINTER(strStackItem);
		}

		// release stack itself
		CLEAR_POINTER(pStack->m_ppStackDescr);
		ZeroMemory(pStack, sizeof(SThreadStack));
	}

	CLEAR_POINTER(m_pStacks);
	m_nStacks = 0;	
}

CStackTracer::SThreadStack* CStackTracer::FindThreadStack(UINT i_nThreadId)
{
	for(int i=0; i<m_nStacks; i++)
	{
		SThreadStack* pStack = m_pStacks + i;
		if(i_nThreadId == pStack->m_nThreadId)
			return pStack;
	}

	return NULL;
}

void CStackTracer::RegisterThread(LPCTSTR i_strThreadDesciption)
{
	ENTER_AUTO_LOCK(&m_csStacks);

	UINT nThreadId = GetCurrentThreadId(); 

	SThreadStack* 
		pStack = FindThreadStack(nThreadId);

	if(!pStack)
		pStack = FindThreadStack(0);

	if(!pStack)
		return;

	pStack->m_nThreadId = nThreadId;
	lstrcpyn(pStack->m_strThreadDescription, i_strThreadDesciption, 
		sizeof(pStack->m_strThreadDescription));
}

BOOL CStackTracer::Push(LPCTSTR i_strFuncName)
{
	SThreadStack* pStack = FindThreadStack(GetCurrentThreadId());

	if(!pStack)
		return FALSE;

	if(NULL == pStack->m_ppStackDescr)
	{
		pStack->m_ppStackDescr = new LPTSTR[m_cnStackItemsMax];
		ZeroMemory(pStack->m_ppStackDescr, sizeof(LPTSTR) * m_cnStackItemsMax);
	}

	// find next empty stack item
	for(int i=0; i<m_cnStackItemsMax && pStack->m_ppStackDescr[i]; i++);

	if(i == m_cnStackItemsMax)
		return FALSE;

#ifdef _UNICODE
	pStack->m_ppStackDescr[i] = _wcsdup(i_strFuncName);
#else
	pStack->m_ppStackDescr[i] = strdup(i_strFuncName);
#endif

#ifdef _DEBUG
#ifdef _OUTPUT_STACK_DEBUG_
	TRACE1(_T("\nStack tracing -> %s"), i_strFuncName);
#endif
#endif

	return TRUE;
}

void CStackTracer::Pop()
{
	SThreadStack* pStack = FindThreadStack(GetCurrentThreadId());

	if(!pStack)
		return;

	// find next empty stack item
	for(int i=0; i<m_cnStackItemsMax && pStack->m_ppStackDescr[i]; i++);

	--i;

	if(i < 0)
		return;

#ifdef _DEBUG
#ifdef _OUTPUT_STACK_DEBUG_
	TRACE1(_T("\nStack tracing <- %s"), pStack->m_ppStackDescr[i]);
#endif
#endif

	if(pStack->m_ppStackDescr[i])
	{
		free(pStack->m_ppStackDescr[i]);
		pStack->m_ppStackDescr[i] = NULL;
	}
}

LPCTSTR CStackTracer::InitStackUnrolling()
{
	SThreadStack* pStack = FindThreadStack(GetCurrentThreadId());

	if(!pStack)
		return NULL;

	for(int i=0; i<m_cnStackItemsMax && pStack->m_ppStackDescr[i]; i++);

	pStack->m_nCurUnrollingLevel = i;
	return pStack->m_strThreadDescription;
}

LPCTSTR CStackTracer::GetNextStackItem()
{
	SThreadStack* pStack = FindThreadStack(GetCurrentThreadId());

	if(!pStack)
		return NULL;

	--pStack->m_nCurUnrollingLevel;
	
	if(	pStack->m_nCurUnrollingLevel >=0 &&
		pStack->m_nCurUnrollingLevel < m_cnStackItemsMax)
		return pStack->m_ppStackDescr[pStack->m_nCurUnrollingLevel];

	return NULL;
}

#endif

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
CFuncTimer::CFuncTimer(LPCTSTR i_strFuncName)
{
	m_dwFuncEnterTicks = GetTickCount();
	m_strFuncName = i_strFuncName;
}

CFuncTimer::~CFuncTimer()
{
	DWORD dwTotalTicks = GetTickCount() - m_dwFuncEnterTicks;

	if(0 == dwTotalTicks)
		return;

	DWORD dwMin, dwSec, dwMSec;

	dwMin = dwTotalTicks / (1000*60);
	dwTotalTicks -= dwMin * (1000*60);
	dwSec = dwTotalTicks / 1000;
	dwMSec = dwTotalTicks - dwSec*1000;

	TCHAR strMsg[1024];

	wsprintf(strMsg, _T("Func time (%s) is : "), m_strFuncName);

	if(dwMin)
		wsprintf(strMsg + lstrlen(strMsg), _T("%d min "), dwMin);

	if(dwSec)
		wsprintf(strMsg + lstrlen(strMsg), _T("%d sec "), dwSec);

	wsprintf(strMsg + lstrlen(strMsg), _T("%d msec "), dwMSec);
	lstrcat(strMsg, _T("\n"));

	TRACE(strMsg);
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
BOOL CMN::RegisterComDll(LPCTSTR i_strPathname)
{
	TCHAR strCommandLine[1000];
	wsprintf(strCommandLine, _T("regsvr32.exe %s -s"), i_strPathname);

	PROCESS_INFORMATION pi;
    STARTUPINFO si;

    // Set up the start up info struct.
    ZeroMemory(&si,sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);

    si.wShowWindow = SW_HIDE;
	si.dwFlags |= STARTF_USESHOWWINDOW;

	if (!CreateProcess(NULL,strCommandLine, 
		NULL, NULL, TRUE,
		CREATE_NEW_CONSOLE,NULL,NULL,&si,&pi))
		return FALSE;

	WaitForSingleObject(pi.hProcess, 5*60*1000);

	BOOL bRet = TRUE;
	DWORD dwExitCode;

	if(GetExitCodeThread(pi.hThread, &dwExitCode))
	{
		if(0 != dwExitCode)
			bRet = FALSE;	
	}

    CloseHandle(pi.hThread);

	return TRUE;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// supports opening sub sections divided by '\'
// for instance 
// section1\section2\...\sectionN
HKEY CAppRegistryExt::GetSectionKey(HKEY i_hKey, LPCTSTR lpszSection)
{
	LPTSTR strSection = (LPTSTR) _alloca((lstrlen(lpszSection)+1)*sizeof(TCHAR));
	lstrcpy(strSection, lpszSection);
	HKEY hParentSection = i_hKey;
	for(;;)
	{
		BOOL bTheLastSubSection = TRUE;
		// get next sub section	
		for(int i=0; i<lstrlen(strSection); i++)
			if(strSection[i] == _T('\\'))
			{
				bTheLastSubSection = FALSE;
				strSection[i] = _T('\0');
				break;
			}
		HKEY hNextSectionKey;
		RegOpenKeyEx(hParentSection, strSection, 0, KEY_READ, &hNextSectionKey);
		if(hParentSection != i_hKey)
			RegCloseKey(hParentSection);
		if(NULL == hNextSectionKey)
			return NULL;
		hParentSection = hNextSectionKey;

		if(bTheLastSubSection)
			break;
		strSection += lstrlen(strSection) + 1;
	}
	return hParentSection;
}

BOOL CAppRegistryExt::FindFirstKey(HKEY i_hKey, BOOL i_bRecursive, 
	pKeyCheckerFn i_pKeyChecker, void* i_pData) 
{
    TCHAR    astrKeyName[MAX_PATH]; 

    for (int i = 0;; i++) 
    { 
		ULONG nBufLen = MAX_PATH;

        if(ERROR_SUCCESS != RegEnumKeyEx(i_hKey, i, astrKeyName, &nBufLen, NULL, NULL, NULL, NULL))
			return FALSE;

		HKEY hSubKey;
		if(	ERROR_SUCCESS != RegOpenKeyEx(i_hKey, astrKeyName, 0, KEY_READ, &hSubKey))
			return FALSE;

		BOOL bFound = i_pKeyChecker(astrKeyName, hSubKey, i_pData);
		if(!bFound && i_bRecursive)
			bFound = FindFirstKey(hSubKey, TRUE, i_pKeyChecker, i_pData);

		RegCloseKey(hSubKey);

		if(bFound)
			return TRUE;
    } 

	return FALSE;
}

HKEY CAppRegistryExt::GetSystemEnumKey()
{
	return GetSectionKey(HKEY_LOCAL_MACHINE, _T("System\\CurrentControlSet\\Enum"));
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
IFileStream::IFileStream(LPCTSTR i_strPathname, BOOL i_bRead)
{
	m_dwReffs = 0;
	if(i_bRead)
		m_hFile = ::CreateFile( i_strPathname, 
			GENERIC_READ, FILE_SHARE_READ,NULL, OPEN_EXISTING, 0, NULL );
	else
		m_hFile = ::CreateFile( i_strPathname, 
			GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ,NULL, CREATE_ALWAYS, 0, NULL );
}

IFileStream::IFileStream(HANDLE i_hFile)
{
	m_dwReffs = 0;
	m_hFile = i_hFile;
}

IFileStream::~IFileStream()
{
	if(INVALID_HANDLE_VALUE != m_hFile)
		CloseHandle(m_hFile);
}

#pragma warning (disable:4100)
HRESULT IFileStream::QueryInterface(struct _GUID const &iid,void **ppvObj)
{
	if (iid == IID_IUnknown || iid == IID_IStream)
	{
		*ppvObj = this;
		return NOERROR;
	}
	return E_NOINTERFACE;
}

ULONG	IFileStream::AddRef(void)
{
	InterlockedIncrement(&m_dwReffs);
	return ULONG(m_dwReffs);
}

ULONG	IFileStream::Release(void)
{
	InterlockedDecrement(&m_dwReffs);
	DWORD dwRet = m_dwReffs;
	if(0 >= dwRet)
	{
		delete this;
		dwRet = 0;
	}

	return ULONG(dwRet);
}

HRESULT IFileStream::Read(void *pv, ULONG cb, ULONG *pcbRead)
{
	if(INVALID_HANDLE_VALUE == m_hFile)
		return E_UNEXPECTED;

	DWORD dwRead;
	if(!ReadFile(m_hFile, pv, cb, &dwRead, NULL))
		return E_UNEXPECTED;

	if(pcbRead)
		*pcbRead = dwRead;

	return cb == dwRead ? S_OK : S_FALSE;
}	

HRESULT IFileStream::Write(const void *pv, ULONG cb, ULONG *pcbWritten)
{
	if(INVALID_HANDLE_VALUE == m_hFile)
		return E_UNEXPECTED;
	DWORD dwWritten;
	BOOL bRet = WriteFile(m_hFile, pv, cb, &dwWritten, NULL);
	return bRet && dwWritten == cb ? S_OK : E_FAIL;
}	

HRESULT IFileStream::Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER *plibNewPosition)
{
	DWORD dwFileDisposition = FILE_BEGIN;
	switch(dwOrigin)
	{
	case STREAM_SEEK_SET:	dwFileDisposition = FILE_BEGIN; break;
	case STREAM_SEEK_CUR:	dwFileDisposition = FILE_CURRENT; break;
	case STREAM_SEEK_END:	dwFileDisposition = FILE_END; break;
	}

	DWORD dwNewPos = 
		SetFilePointer(m_hFile, dlibMove.LowPart, &dlibMove.HighPart, dwFileDisposition);
		
	if(dwNewPos == 0xFFFFFFFF && GetLastError() != NO_ERROR )
		return E_UNEXPECTED;

	if(plibNewPosition)
	{
		plibNewPosition->LowPart = dwNewPos;		
		plibNewPosition->HighPart = dlibMove.HighPart;		
	}

	return S_OK;
}	

HRESULT IFileStream::SetSize(ULARGE_INTEGER libNewSize)
{
	return E_NOTIMPL;
}	

HRESULT IFileStream::CopyTo(IStream __RPC_FAR *pstm, ULARGE_INTEGER cb, ULARGE_INTEGER *pcbRead, ULARGE_INTEGER *pcbWritten)
{
	return E_NOTIMPL;
}	

HRESULT IFileStream::Commit(DWORD grfCommitFlags )
{
	return S_OK;
}	

HRESULT IFileStream::Revert()
{
	return E_NOTIMPL;
}	

HRESULT IFileStream::LockRegion( ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType)
{
	return E_NOTIMPL;
}	

HRESULT IFileStream::UnlockRegion( ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType)
{
	return E_NOTIMPL;
}	

HRESULT IFileStream::Stat(STATSTG *pstatstg,  DWORD grfStatFlag)
{
	if(pstatstg)
	{
		ZeroMemory(pstatstg, sizeof(STATSTG));
		pstatstg->cbSize.LowPart = GetFileSize(m_hFile, NULL);
	}

	return S_OK;
}	

HRESULT IFileStream::Clone(IStream **ppstm)
{
	return E_NOTIMPL;
}	
#pragma warning (default:4100)
//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
#ifdef _AFX_TEMPLATES
void CIntList::operator = (CIntList& i_lstOther)
{
	RemoveAll();
	POSITION pos = i_lstOther.GetHeadPosition();
	while(pos)
		AddTail(i_lstOther.GetNext(pos));
}

#endif