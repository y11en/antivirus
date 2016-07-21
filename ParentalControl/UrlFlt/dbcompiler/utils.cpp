#include <windows.h>
#include "utils.h"
#include "inilib.h"

#include <atlenc.h>

// #######################################################################################

BOOL	_SetFilePointer(HANDLE hFile, LONGLONG llDistanceToMove, QWORD * pNewFilePointer, DWORD dwMoveMethod)
{
	LONG dwHigh = (LONG)(llDistanceToMove >> 32);
	DWORD dwResPos = ::SetFilePointer(hFile, (LONG)llDistanceToMove, &dwHigh, dwMoveMethod);
	if( dwResPos == INVALID_SET_FILE_POINTER )
		return FALSE;

	if( pNewFilePointer )
		*pNewFilePointer = (QWORD)dwResPos|((QWORD)dwHigh)<<32;
	return TRUE;
}

// #######################################################################################
// UNI Conversion

void	_A2W(LPCSTR src, wstring& dst, UINT cp)
{
	_A2W(src, strlen(src), dst, cp);
}

void	_A2W(LPCSTR src, DWORD nSrcLen, wstring& dst, UINT cp)
{
	dst.resize(nSrcLen*2);
	int nChars = ::MultiByteToWideChar(cp, 0, src, nSrcLen, &dst[0], dst.size());
	dst.resize(nChars);
}

void	_W2A(LPCWSTR src, string& dst, UINT cp)
{
	int nSrcLen = wcslen(src) + 1;
	dst.resize(nSrcLen*5);
	int nChars = ::WideCharToMultiByte(cp, 0, src, nSrcLen, &dst[0], dst.size() + 1, NULL, NULL);
	dst.resize(nChars);
}

// #######################################################################################
// CIni

CIni::CIni(LPCWSTR pFileName, bool fWrite) : m_hFile(INVALID_HANDLE_VALUE), m_fWrite(fWrite), m_fDirty(0)
{
	pIniSection=NULL;
	pPreComment=NULL;

	if( pFileName )
		Open(pFileName, fWrite);
}

CIni::~CIni()
{
	if( m_hFile == INVALID_HANDLE_VALUE )
		return;
	
	if( m_fWrite && m_fDirty )
	{
		tBin _buf; CStreamBuf _stm(_buf);
		if( SUCCEEDED(IniSave(&_stm, pIniSection, pPreComment)) )
		{
			::SetFilePointer(m_hFile, 0, NULL, FILE_BEGIN);
			if( _buf.size() )
			{
				DWORD dwWritten;
				::WriteFile(m_hFile, &_buf[0], _buf.size(), &dwWritten, NULL);
			}
			::SetEndOfFile(m_hFile);
		}
	}
	
	if( pPreComment ) FreeComment(pPreComment);
	if( pIniSection ) IniFree(pIniSection);

	::CloseHandle(m_hFile);
}

HRESULT	CIni::Open(LPCWSTR pFileName, bool fWrite)
{
	m_hFile = ::CreateFileW(pFileName, GENERIC_READ|(m_fWrite ? GENERIC_WRITE : 0), FILE_SHARE_READ, NULL, m_fWrite ? OPEN_ALWAYS : OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS|FILE_ATTRIBUTE_NORMAL, NULL);
	if( m_hFile == INVALID_HANDLE_VALUE )
		return HRESULT_FROM_WIN32(::GetLastError());

	tBin _buf; _buf.resize(::GetFileSize(m_hFile, NULL));
	if( _buf.size() )
	{
		DWORD dwReaded;
		if( !::ReadFile(m_hFile, &_buf[0], _buf.size(), &dwReaded, NULL) )
			return FALSE;
	}
	
	CStreamBuf _stm(_buf);
	return IniLoad(&_stm, &pIniSection, &pPreComment);
}

sIniSection * CIni::GetSection(LPCSTR pSectName)
{
	return IniGetSection(pIniSection, pSectName);
}

void	CIni::ReadStr(LPCSTR pSectName, LPCSTR pKeyName, string& out, LPCSTR pDefault)
{
	CHAR * val = IniGetValue(pIniSection, pSectName, pKeyName);
	if( !val )
	{
		if( !pDefault )
			out = "";
		else
			out = pDefault;
	}
	else
		out = val;
}

void	CIni::WriteStr(LPCSTR pSectName, LPCSTR pKeyName, LPCSTR pData)
{
	m_fDirty = 1;

	IniSetValue(pIniSection, pSectName, pKeyName, pData);
}

void	CIni::ReadStrW(LPCSTR pSectName, LPCSTR pKeyName, wstring& out, LPCWSTR pDefault)
{
	CHAR * val = IniGetValue(pIniSection, pSectName, pKeyName);
	if( !val )
	{
		if( !pDefault )
			out = L"";
		else
			out = pDefault;
	}
	else
		_A2W(val, out, CP_UTF8);
}

void	CIni::WriteStrW(LPCSTR pSectName, LPCSTR pKeyName, LPCWSTR pData)
{
	m_fDirty = 1;

	string _str; _W2A(pData, _str, CP_UTF8);
	IniSetValue(pIniSection, pSectName, pKeyName, _str.c_str());
}

DWORD	CIni::ReadInt(LPCSTR pSectName, LPCSTR pKeyName, DWORD nDefault)
{
	CHAR * val = IniGetValue(pIniSection, pSectName, pKeyName);
	if( !val )
		return nDefault;

	return atol(val);
}

void	CIni::WriteInt(LPCSTR pSectName, LPCSTR pKeyName, DWORD nData)
{
	m_fDirty = 1;

	CHAR strNum[20]; _ltoa(nData, strNum, 10);
	IniSetValue(pIniSection, pSectName, pKeyName, strNum);
}

void	CIni::ReadBin(LPCSTR pSectName, LPCSTR pKeyName, tBin& out)
{
	CHAR * strBase64 = IniGetValue(pIniSection, pSectName, pKeyName);
	if( !strBase64 )
		return;

	int nValLen = strlen(strBase64);
	int nLen = Base64DecodeGetRequiredLength(nValLen);
	if( !nLen )
		return;

	out.resize(nLen);
	if( out.size() != nLen )
	{
		out.resize(0);
		return;
	}

	if( !Base64Decode(strBase64, nValLen, &out[0], &nLen) )
		nLen = 0;

	out.resize(nLen);

	if( out.size() < sizeof(DWORD) )
	{
		out.resize(0);
		return;
	}
	
	DWORD nCRC32 = *(DWORD *)&out[0];
	if( nCRC32 )
	{
	}

	out.erase(out.begin(), out.begin() + sizeof(DWORD));
}

void	CIni::WriteBin(LPCSTR pSectName, LPCSTR pKeyName, PBYTE pData, DWORD nDataSize)
{
	m_fDirty = 1;

	tBin buf; buf.resize(nDataSize + sizeof(DWORD));
	if( buf.size() != nDataSize + sizeof(DWORD) )
		return;

	DWORD nCRC32 = 0;
	memcpy(&buf[0], &nCRC32, sizeof(DWORD));
	memcpy(&buf[sizeof(DWORD)], pData, nDataSize);
	
	int nLen = Base64EncodeGetRequiredLength(buf.size(), ATL_BASE64_FLAG_NOCRLF);
	string strBase64; strBase64.resize(nLen);
	if( strBase64.size() != nLen )
		return;

	Base64Encode(&buf[0], buf.size(), &strBase64[0], &nLen, ATL_BASE64_FLAG_NOCRLF);
	strBase64.resize(nLen);

	IniSetValue(pIniSection, pSectName, pKeyName, strBase64.c_str());
}

// #######################################################################################
// #######################################################################################
