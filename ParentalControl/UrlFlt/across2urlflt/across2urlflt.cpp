// across2urlflt.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

// #######################################################################################
// #######################################################################################

void	_W2A(LPCWSTR src, DWORD nSrcLen, string& dst, UINT cp = CP_ACP)
{
	dst.resize(nSrcLen*5);
	DWORD nChars = ::WideCharToMultiByte(cp, 0, src, nSrcLen, &dst[0], (int)dst.size(), NULL, NULL);
	dst.resize(nChars);
}

inline bool _MatchOkayA(char ch) { return !(ch && ch!='*'); }

bool _MatchWithPattern(const char * Pattern, const char * Name, bool bPatternWithAsterics = false)
{
	char ch;
	char pc;

	if (!Pattern || !Name)
		return false;

	if(*Pattern=='*')
	{
		Pattern++;
		while(*Name && *Pattern)
		{
			pc = *Pattern;
			ch = *Name;
			if ((ch == '*') || (pc == ch) || (pc == '?'))
			{
				if(_MatchWithPattern(Pattern+1,Name+1, bPatternWithAsterics)) 
					return true;
			}

			Name++;
		}
		return _MatchOkayA(*Pattern);
	} 

	while(*Name && *Pattern != '*')
	{
		pc = *Pattern;
		ch = *Name;
		if(pc == ch || pc == '?')
		{
			Pattern++;
			Name++;
		} 
		else
		{
			if (pc == 0 && bPatternWithAsterics)
			{
				if (ch == '\\' || ch == '/')
					return true;
			}
			return false;
		}

	}
	if(*Name)
		return _MatchWithPattern(Pattern,Name,bPatternWithAsterics);

	return _MatchOkayA(*Pattern);
}

// #######################################################################################

typedef vector<string>	tExcl;
typedef vector<BYTE>	tBin;
typedef set<string>		tUrls;

struct _across_item
{
	LPSTR strUrl;
	LPSTR strVerdict;
	LPSTR strTime;
	LPSTR strProduct;
};

HRESULT _ReadSrcString(LPSTR str, tExcl& aExcl, tUrls& _urls)
{
	_across_item _item;
	_item.strUrl = "";
	_item.strVerdict = "";
	_item.strTime = "";
	_item.strProduct = "";
	
	LPSTR _sep = str;
	
	if( str && (_sep = strchr(str, ',')) )
	{
		_item.strUrl = str;
		*_sep = 0; str = _sep + 1;
	}

	if( str && (_sep = strchr(str, ',')) )
	{
		_item.strVerdict = str;
		*_sep = 0; str = _sep + 1;
	}

	if( str && (_sep = strchr(str, ',')) )
	{
		_item.strTime = str;
		*_sep = 0; str = _sep + 1;
	}
	
	if( str )
		_item.strProduct = str;

	_sep = strchr(_item.strUrl, '?');
	if( !_sep )
		_sep = strrchr(_item.strUrl, '/');

	for(DWORD i = 0; i < aExcl.size(); i++)
	{
		if( _MatchWithPattern(aExcl[i].c_str(), _item.strVerdict) )
			return S_FALSE;
	}
	
	if( !_sep )
		_urls.insert(_item.strUrl);
	else
	{
		_sep++;
		*_sep = 0;
		string strUrl(_item.strUrl);
		strUrl += "*";
		_urls.insert(strUrl.c_str());
	}
	
	return S_OK;
}

HRESULT _ReadSrc(LPCWSTR strFile, tExcl& aExcl, tUrls& _urls)
{
	HANDLE hFile = ::CreateFileW(strFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN|FILE_ATTRIBUTE_NORMAL, NULL);
	if( hFile == INVALID_HANDLE_VALUE )
		return HRESULT_FROM_WIN32(::GetLastError());

	DWORD nSize = ::GetFileSize(hFile, NULL);
	if( !nSize )
		return ::CloseHandle(hFile), S_FALSE;
	
	HRESULT hr = S_OK;
	tBin _buf; _buf.resize(nSize + 2);
	if( _buf.size() != nSize + 2 )
		return ::CloseHandle(hFile), E_OUTOFMEMORY;
	
	DWORD dwReaded = 0;
	if( !::ReadFile(hFile, &_buf[0], nSize, &dwReaded, NULL) )
		hr = HRESULT_FROM_WIN32(::GetLastError());

	::CloseHandle(hFile);

	if( !SUCCEEDED(hr) )
		return hr;

	_buf[nSize] = '\n';
	_buf[nSize + 1] = 0;
	
	LPSTR buf = (LPSTR)&_buf[0];
	LPSTR str = buf;
	while( *buf )
	{
		if( *buf != '\n' )
		{
			buf++;
			continue;
		}
		
		*buf = 0;
		
		size_t len = buf - str;
		if( len && str[len - 1] == '\r' )
		{
			str[len - 1] = 0;
			len--;
		}

		if( len && FAILED(hr = _ReadSrcString(str, aExcl, _urls)) )
			break;

		str = ++buf;
	}

	return hr;
}

HRESULT _WriteDst(tUrls& _urls, LPCWSTR strFile)
{
	if( !_urls.size() )
		return S_FALSE;
	
	HANDLE hFile = ::CreateFileW(strFile, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN|FILE_ATTRIBUTE_NORMAL, NULL);
	if( hFile == INVALID_HANDLE_VALUE )
		return HRESULT_FROM_WIN32(::GetLastError());

	DWORD dwWriten = 0;
	HRESULT hr = S_OK;
	string _str;
	
	#define _WRITESTR(_s)	\
		{ _str = _s; _str += "\r\n";	\
		if( !::WriteFile(hFile, _str.c_str(), (DWORD)_str.size(), &dwWriten, NULL) )	\
		hr = HRESULT_FROM_WIN32(::GetLastError()); }

	_WRITESTR("[Sites.black]");
	for(tUrls::iterator i = _urls.begin(); SUCCEEDED(hr) && i != _urls.end(); i++)
		_WRITESTR((*i).c_str());
	
	::CloseHandle(hFile);
	return hr;
}

// #######################################################################################
// #######################################################################################

int wmain(int argc, wchar_t *argv[])
{
	printf("ACROSS2URLFLT version 1.0 (C) 2007 Kaspersky labs.\n\n");
	if( argc < 3 )
	{
		fwprintf(stderr, L"Wrong number of arguments!\n");
		fwprintf(stderr, L"Usage: ACROSS2URLFLT <src_file> <dst_file> [-e=verdict_mask1[,verdict_mask2,...]]\n");
		return E_FAIL;
	}
	
	LPCWSTR strSrc = argv[1];
	LPCWSTR strDst = argv[2];

	tExcl aExcl;
	if( argc >= 4 )
	{
		LPCWSTR _str = argv[3];
		if( !wcsncmp(_str, L"-e=", sizeof("-e=")/2 - 1) )
		{
			_str += sizeof("-e=")/2 - 1;
			while( *_str )
			{
				LPCWSTR _str1 = wcschr(_str, L',');
				if( !_str1 )
					_str1 = _str + wcslen(_str);

				_W2A(_str, (DWORD)(_str1 - _str), *aExcl.insert(aExcl.end(), string()));
				if( !*_str1 )
					break;
				_str = _str1 + 1;
			}
		}
	}
	
	HRESULT hr;
	tUrls _urls;
	
	fwprintf(stdout, L"Reading source file \"%s\"...\n", strSrc);
	if( FAILED(hr = _ReadSrc(strSrc, aExcl, _urls)) )
		fwprintf(stderr, L"Error read file: 0x%08x\n", hr);
	
	fwprintf(stdout, L"Writing destination file \"%s\"...\n", strDst);
	if( SUCCEEDED(hr) && FAILED(hr = _WriteDst(_urls, strDst)) )
		fwprintf(stderr, L"Error write file: 0x%08x\n", hr);
	
	fwprintf(stdout, L"Completed%s\n", SUCCEEDED(hr) ? L" successfully." : L" with error!");
	return hr;
}

// #######################################################################################
// #######################################################################################
