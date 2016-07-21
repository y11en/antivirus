#ifdef _WIN32
// version.cpp (avsscan)
//
//

#include <string.h>
#include "avsscan.h"

#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include <io.h>
#include <string>
#include <vector>
#include <list>
#include <wincrypt.h>
#include <wintrust.h>
#include <softpub.h>
#include <iface/i_hash.h>
#include <plugin/p_win32_nfio.h>
#include <../../Prague/Hash/MD5/plugin_hash_md5.h>

#pragma comment(lib, "version.lib")
#pragma comment(lib, "crypt32.lib")
#pragma comment(lib, "wintrust.lib")

#define ENCODING (X509_ASN_ENCODING | PKCS_7_ASN_ENCODING)

struct CIOData
{
	cIO*         io;
	CIOData*     pParent;
	std::wstring sHash;
	bool         bParsed;
};

struct CIOInfo
{
	CIOInfo(){ memset(this, 0, sizeof(CIOInfo)); }

	std::wstring sFileName;
	__int64      qwFileSize;

	std::wstring qwHashMD5;
	std::wstring qwHashSha1;
	std::wstring qwParentHashMD5;

	std::wstring sProductName;
	std::wstring sProductVersion;
	std::wstring sFileVersion;
	std::wstring sCompanyName;
	std::wstring sDescription;
	std::wstring sCopyright;

	std::wstring sSignOk;
	std::wstring sIssuer;
	std::wstring sSignerNumber;
	std::wstring sSignerName;
	std::wstring sSignerOrganization;
	std::wstring sCertNotBefore;
	std::wstring sCertNotAfter;

	std::wstring sSignerProductName;
	std::wstring sPublisherLink;
	std::wstring sMoreInfoLink;
	std::wstring tmDateOfSign;
};

////////////////////////////////////////////////////////////////

void binary_to_string(PBYTE buff, int size, std::wstring& ret, bool from_end=false)
{
	ret.erase();
	for(int n = 0; n < size; n++)
	{
		WCHAR sBuff[4];
		_snwprintf(sBuff, sizeof(sBuff), L"%02x", buff[from_end ? size - (n + 1) : n]);
		ret += sBuff;
	}
}

////////////////////////////////////////////////////////////////

bool version_query_value(PVOID pVersion, LPCSTR pCP, LPCSTR pInfoName, std::wstring& ret)
{
	if( !pVersion || !pInfoName )
		return false;

	WCHAR VerPath[0x200];
	_snwprintf(VerPath, countof(VerPath), L"\\StringFileInfo\\%S\\%S", pCP, pInfoName);
	
	UINT nLen = 0;
	LPCWSTR pVerValue = NULL;
	if( !::VerQueryValueW(pVersion, VerPath,  (LPVOID *)&pVerValue, &nLen) )
		return false;

	ret.append(pVerValue, nLen);
	return true;
}

bool version_query_info(LPCWSTR sFileName, CIOInfo& info)
{
	DWORD dwHandle, dwVersionSize;
	PVOID pVersionInfo = NULL;
	UINT nLen = 0;
	LPDWORD pnLangCp;
	WCHAR Buff[0x200];

	dwVersionSize = ::GetFileVersionInfoSizeW(sFileName, &dwHandle);
	GetLastError();
	if( dwVersionSize )
		pVersionInfo = new char[dwVersionSize];

	if( pVersionInfo && ::GetFileVersionInfoW(sFileName, NULL, dwVersionSize, pVersionInfo) )
	{
		VS_FIXEDFILEINFO* pVS = NULL;
		::VerQueryValueW(pVersionInfo, L"\\",  (LPVOID*)&pVS, &nLen);

		_snwprintf(Buff, countof(Buff), L"[%d.%d.%d.%d]",
				pVS->dwFileVersionMS >> 16, pVS->dwFileVersionMS & 0xFFFF,
				pVS->dwFileVersionLS >> 16, pVS->dwFileVersionLS & 0xFFFF);

		info.sFileVersion += Buff;

		_snwprintf(Buff, countof(Buff), L"[%d.%d.%d.%d]",
				pVS->dwProductVersionMS >> 16, pVS->dwProductVersionMS & 0xFFFF,
				pVS->dwProductVersionLS >> 16, pVS->dwProductVersionLS & 0xFFFF);

		info.sProductVersion += Buff;

		if( ::VerQueryValueW(pVersionInfo, L"\\VarFileInfo\\Translation", (LPVOID*)&pnLangCp, &nLen) )
		{
			CHAR sLangCp[20];
			sprintf(sLangCp, "%08X", (*pnLangCp<<16)|(*pnLangCp>>16));

			version_query_value(pVersionInfo, sLangCp, "ProductName", info.sProductName);
			version_query_value(pVersionInfo, sLangCp, "ProductVersion", info.sProductVersion);
			version_query_value(pVersionInfo, sLangCp, "FileVersion", info.sFileVersion);
			version_query_value(pVersionInfo, sLangCp, "CompanyName", info.sCompanyName);
			version_query_value(pVersionInfo, sLangCp, "FileDescription", info.sDescription);
			version_query_value(pVersionInfo, sLangCp, "LegalCopyright", info.sCopyright);
		}
	}

	if( pVersionInfo )
		delete[] pVersionInfo;

	return true;
}

////////////////////////////////////////////////////////////////

bool signer_get_program_info(PCMSG_SIGNER_INFO pSignerInfo, CIOInfo& info)
{
	PCRYPT_ATTRIBUTE pOPUS = NULL;
	for(DWORD n = 0; n < pSignerInfo->AuthAttrs.cAttr; n++)
	{       
		CRYPT_ATTRIBUTE& p = pSignerInfo->AuthAttrs.rgAttr[n];
		if( lstrcmpA(SPC_SP_OPUS_INFO_OBJID, p.pszObjId) == 0 )
		{
			pOPUS = &p;
			break;
		}
	}

	if( !pOPUS )
		return false;

    DWORD dwData = 0;
	CryptDecodeObject(ENCODING, SPC_SP_OPUS_INFO_OBJID,
		pOPUS->rgValue[0].pbData, pOPUS->rgValue[0].cbData, 0, NULL, &dwData);

	if( !dwData )
		return false;

	PSPC_SP_OPUS_INFO OpusInfo = (PSPC_SP_OPUS_INFO)new char[dwData];

	CryptDecodeObject(ENCODING, SPC_SP_OPUS_INFO_OBJID,
			pOPUS->rgValue[0].pbData, pOPUS->rgValue[0].cbData, 0, OpusInfo, &dwData);

	if( OpusInfo->pwszProgramName )
		info.sSignerProductName = OpusInfo->pwszProgramName;

	if( OpusInfo->pPublisherInfo )
		switch( OpusInfo->pPublisherInfo->dwLinkChoice )
		{
			case SPC_URL_LINK_CHOICE:
				info.sPublisherLink = OpusInfo->pPublisherInfo->pwszUrl;
				break;

			case SPC_FILE_LINK_CHOICE:
				info.sPublisherLink = OpusInfo->pPublisherInfo->pwszFile;
				break;
		}

	if( OpusInfo->pMoreInfo )
		switch (OpusInfo->pMoreInfo->dwLinkChoice)
		{
			case SPC_URL_LINK_CHOICE:
				info.sMoreInfoLink = OpusInfo->pMoreInfo->pwszUrl;
				break;

			case SPC_FILE_LINK_CHOICE:
				info.sMoreInfoLink = OpusInfo->pMoreInfo->pwszFile;
				break;
		}

	delete[] OpusInfo;
    return true;
}

bool signer_get_counter_info(PCMSG_SIGNER_INFO pSignerInfo, CIOInfo& info)
{   
	PCRYPT_ATTRIBUTE pAttr = NULL;
    for(DWORD n = 0; n < pSignerInfo->UnauthAttrs.cAttr; n++)
        if( lstrcmpA(pSignerInfo->UnauthAttrs.rgAttr[n].pszObjId, szOID_RSA_counterSign) == 0 )
		{
			pAttr = &pSignerInfo->UnauthAttrs.rgAttr[n];
			break;
		}

	if( !pAttr )
		return false;

    DWORD dwSize = 0;
    CryptDecodeObject(ENCODING, PKCS7_SIGNER_INFO,
		pAttr->rgValue[0].pbData, pAttr->rgValue[0].cbData, 0, NULL, &dwSize);

	if( !dwSize )
		return false;

	PCMSG_SIGNER_INFO pCounterInfo = (PCMSG_SIGNER_INFO)new char[dwSize];

    CryptDecodeObject(ENCODING, PKCS7_SIGNER_INFO,
		pAttr->rgValue[0].pbData, pAttr->rgValue[0].cbData, 0, pCounterInfo, &dwSize);

	for(DWORD n = 0; n < pCounterInfo->AuthAttrs.cAttr; n++)
	{       
		CRYPT_ATTRIBUTE& p = pCounterInfo->AuthAttrs.rgAttr[n];
        if( lstrcmpA(szOID_RSA_signingTime, p.pszObjId) == 0 )
        {               
			FILETIME ft;
            DWORD dwSize = sizeof(FILETIME);
            CryptDecodeObject(ENCODING, szOID_RSA_signingTime,
				p.rgValue[0].pbData, p.rgValue[0].cbData, 0, (PVOID)&ft, &dwSize);

			binary_to_string((PBYTE)&ft, sizeof(FILETIME), info.tmDateOfSign);
			break;
        }
	}

	delete[] pCounterInfo;
    return true;
}

bool signer_get_cert_info(PCCERT_CONTEXT pCertContext, DWORD dwType, DWORD dwFlags, void* pvTypePara, std::wstring& info)
{
    DWORD dwSize = CertGetNameStringW(pCertContext, dwType, dwFlags, pvTypePara, NULL, 0);
	if( !dwSize )
		return false;

	LPWSTR szName = (LPWSTR)new WCHAR[dwSize];
	CertGetNameStringW(pCertContext, dwType, dwFlags, pvTypePara, szName, dwSize);

	if( !info.empty() )
		info += L';';

	info += szName;
	return true;
}

bool signer_verify(LPCWSTR pwszSourceFile)
{
	WINTRUST_FILE_INFO FileData;
	memset(&FileData, 0, sizeof(FileData));
	FileData.cbStruct = sizeof(WINTRUST_FILE_INFO);
	FileData.pcwszFilePath = pwszSourceFile;

	GUID WVTPolicyGUID = WINTRUST_ACTION_GENERIC_VERIFY_V2;
	WINTRUST_DATA WinTrustData;
	memset(&WinTrustData, 0, sizeof(WinTrustData));
	WinTrustData.cbStruct = sizeof(WinTrustData);
	WinTrustData.dwUIChoice = WTD_UI_NONE;
	WinTrustData.fdwRevocationChecks = WTD_REVOKE_NONE; 
	WinTrustData.dwUnionChoice = WTD_CHOICE_FILE;
	WinTrustData.dwProvFlags = WTD_SAFER_FLAG;
	WinTrustData.pFile = &FileData;

	if( WinVerifyTrust(NULL, &WVTPolicyGUID, &WinTrustData) == ERROR_SUCCESS )
		return true;

	return false;
}

bool signer_get_info(LPCWSTR sFileName, CIOInfo& info)
{
	HCRYPTMSG pMsg = NULL;
    HCERTSTORE hStore = NULL;
	DWORD dwCount = 0;
	DWORD dwSize = sizeof(DWORD);
	PCMSG_SIGNER_INFO pSignerInfo = NULL;
	PCCERT_CONTEXT pCertContext = NULL;

	BOOL bRes = CryptQueryObject(CERT_QUERY_OBJECT_FILE, sFileName, CERT_QUERY_CONTENT_FLAG_PKCS7_SIGNED_EMBED, 
		CERT_QUERY_FORMAT_FLAG_BINARY, 0, NULL, NULL, NULL, &hStore, &pMsg, NULL);

	if( !bRes )
		return false;

	if( signer_verify(sFileName) )
		info.sSignOk = L"ok";
	else
		info.sSignOk = L"failed";

	CryptMsgGetParam(pMsg, CMSG_SIGNER_COUNT_PARAM, 0, &dwCount, &dwSize);
						
	dwSize = 0;
	CryptMsgGetParam(pMsg, CMSG_SIGNER_INFO_PARAM, 0, NULL, &dwSize);

	if( dwSize )
		pSignerInfo = (PCMSG_SIGNER_INFO)new char[dwSize];

	if( pSignerInfo )
	{
		CryptMsgGetParam(pMsg, CMSG_SIGNER_INFO_PARAM, 0, pSignerInfo, &dwSize);
		signer_get_program_info(pSignerInfo, info);
		signer_get_counter_info(pSignerInfo, info);

		CERT_INFO CertInfo;     
		CertInfo.Issuer = pSignerInfo->Issuer;
		CertInfo.SerialNumber = pSignerInfo->SerialNumber;

		pCertContext = CertFindCertificateInStore(hStore, ENCODING, 0,
			CERT_FIND_SUBJECT_CERT, (PVOID)&CertInfo, NULL);

		delete[] pSignerInfo;
	}

	if( pCertContext )
	{
		binary_to_string(pCertContext->pCertInfo->SerialNumber.pbData,
			pCertContext->pCertInfo->SerialNumber.cbData, info.sSignerNumber, true);

		binary_to_string((PBYTE)&pCertContext->pCertInfo->NotBefore, sizeof(FILETIME), info.sCertNotBefore);
		binary_to_string((PBYTE)&pCertContext->pCertInfo->NotAfter, sizeof(FILETIME), info.sCertNotAfter);

		signer_get_cert_info(pCertContext, CERT_NAME_ATTR_TYPE, 0,
			szOID_ORGANIZATION_NAME, info.sSignerOrganization);

		signer_get_cert_info(pCertContext, CERT_NAME_ATTR_TYPE, 0,
			szOID_COMMON_NAME, info.sSignerName);

		signer_get_cert_info(pCertContext, CERT_NAME_SIMPLE_DISPLAY_TYPE,
			CERT_NAME_ISSUER_FLAG, 0, info.sIssuer);

		CertFreeCertificateContext(pCertContext);
	}

    if( hStore )
		CertCloseStore(hStore, 0);

    if( pMsg )
		CryptMsgClose(pMsg);

	return true;
}

////////////////////////////////////////////////////////////////

struct SHA1Context
{
    unsigned Message_Digest[5]; /* Message Digest (output)          */

    unsigned Length_Low;        /* Message length in bits           */
    unsigned Length_High;       /* Message length in bits           */

    unsigned char Message_Block[64]; /* 512-bit message blocks      */
    int Message_Block_Index;    /* Index into message block array   */

    int Computed;               /* Is the digest computed?          */
    int Corrupted;              /* Is the message digest corruped?  */
};

#define SHA1CircularShift(bits,word) \
                ((((word) << (bits)) & 0xFFFFFFFF) | \
                ((word) >> (32-(bits))))


void SHA1ProcessMessageBlock(SHA1Context *context)
{
    const unsigned K[] =            /* Constants defined in SHA-1   */      
    {
        0x5A827999,
        0x6ED9EBA1,
        0x8F1BBCDC,
        0xCA62C1D6
    };
    int         t;                  /* Loop counter                 */
    unsigned    temp;               /* Temporary word value         */
    unsigned    W[80];              /* Word sequence                */
    unsigned    A, B, C, D, E;      /* Word buffers                 */

    /*
     *  Initialize the first 16 words in the array W
     */
    for(t = 0; t < 16; t++)
    {
        W[t] = ((unsigned) context->Message_Block[t * 4]) << 24;
        W[t] |= ((unsigned) context->Message_Block[t * 4 + 1]) << 16;
        W[t] |= ((unsigned) context->Message_Block[t * 4 + 2]) << 8;
        W[t] |= ((unsigned) context->Message_Block[t * 4 + 3]);
    }

    for(t = 16; t < 80; t++)
    {
       W[t] = SHA1CircularShift(1,W[t-3] ^ W[t-8] ^ W[t-14] ^ W[t-16]);
    }

    A = context->Message_Digest[0];
    B = context->Message_Digest[1];
    C = context->Message_Digest[2];
    D = context->Message_Digest[3];
    E = context->Message_Digest[4];

    for(t = 0; t < 20; t++)
    {
        temp =  SHA1CircularShift(5,A) +
                ((B & C) | ((~B) & D)) + E + W[t] + K[0];
        temp &= 0xFFFFFFFF;
        E = D;
        D = C;
        C = SHA1CircularShift(30,B);
        B = A;
        A = temp;
    }

    for(t = 20; t < 40; t++)
    {
        temp = SHA1CircularShift(5,A) + (B ^ C ^ D) + E + W[t] + K[1];
        temp &= 0xFFFFFFFF;
        E = D;
        D = C;
        C = SHA1CircularShift(30,B);
        B = A;
        A = temp;
    }

    for(t = 40; t < 60; t++)
    {
        temp = SHA1CircularShift(5,A) +
               ((B & C) | (B & D) | (C & D)) + E + W[t] + K[2];
        temp &= 0xFFFFFFFF;
        E = D;
        D = C;
        C = SHA1CircularShift(30,B);
        B = A;
        A = temp;
    }

    for(t = 60; t < 80; t++)
    {
        temp = SHA1CircularShift(5,A) + (B ^ C ^ D) + E + W[t] + K[3];
        temp &= 0xFFFFFFFF;
        E = D;
        D = C;
        C = SHA1CircularShift(30,B);
        B = A;
        A = temp;
    }

    context->Message_Digest[0] =
                        (context->Message_Digest[0] + A) & 0xFFFFFFFF;
    context->Message_Digest[1] =
                        (context->Message_Digest[1] + B) & 0xFFFFFFFF;
    context->Message_Digest[2] =
                        (context->Message_Digest[2] + C) & 0xFFFFFFFF;
    context->Message_Digest[3] =
                        (context->Message_Digest[3] + D) & 0xFFFFFFFF;
    context->Message_Digest[4] =
                        (context->Message_Digest[4] + E) & 0xFFFFFFFF;

    context->Message_Block_Index = 0;
}

void SHA1PadMessage(SHA1Context *context)
{
    /*
     *  Check to see if the current message block is too small to hold
     *  the initial padding bits and length.  If so, we will pad the
     *  block, process it, and then continue padding into a second
     *  block.
     */
    if (context->Message_Block_Index > 55)
    {
        context->Message_Block[context->Message_Block_Index++] = 0x80;
        while(context->Message_Block_Index < 64)
        {
            context->Message_Block[context->Message_Block_Index++] = 0;
        }

        SHA1ProcessMessageBlock(context);

        while(context->Message_Block_Index < 56)
        {
            context->Message_Block[context->Message_Block_Index++] = 0;
        }
    }
    else
    {
        context->Message_Block[context->Message_Block_Index++] = 0x80;
        while(context->Message_Block_Index < 56)
        {
            context->Message_Block[context->Message_Block_Index++] = 0;
        }
    }

    /*
     *  Store the message length as the last 8 octets
     */
    context->Message_Block[56] = (context->Length_High >> 24) & 0xFF;
    context->Message_Block[57] = (context->Length_High >> 16) & 0xFF;
    context->Message_Block[58] = (context->Length_High >> 8) & 0xFF;
    context->Message_Block[59] = (context->Length_High) & 0xFF;
    context->Message_Block[60] = (context->Length_Low >> 24) & 0xFF;
    context->Message_Block[61] = (context->Length_Low >> 16) & 0xFF;
    context->Message_Block[62] = (context->Length_Low >> 8) & 0xFF;
    context->Message_Block[63] = (context->Length_Low) & 0xFF;

    SHA1ProcessMessageBlock(context);
}


void SHA1Reset(SHA1Context *context)
{
    context->Length_Low             = 0;
    context->Length_High            = 0;
    context->Message_Block_Index    = 0;

    context->Message_Digest[0]      = 0x67452301;
    context->Message_Digest[1]      = 0xEFCDAB89;
    context->Message_Digest[2]      = 0x98BADCFE;
    context->Message_Digest[3]      = 0x10325476;
    context->Message_Digest[4]      = 0xC3D2E1F0;

    context->Computed   = 0;
    context->Corrupted  = 0;
}

int SHA1Result(SHA1Context *context)
{

    if (context->Corrupted)
    {
        return 0;
    }

    if (!context->Computed)
    {
        SHA1PadMessage(context);
        context->Computed = 1;
    }

    return 1;
}

void SHA1Input(     SHA1Context         *context,
                    const unsigned char *message_array,
                    unsigned            length)
{
    if (!length)
    {
        return;
    }

    if (context->Computed || context->Corrupted)
    {
        context->Corrupted = 1;
        return;
    }

    while(length-- && !context->Corrupted)
    {
        context->Message_Block[context->Message_Block_Index++] =
                                                (*message_array & 0xFF);

        context->Length_Low += 8;
        /* Force it to 32 bits */
        context->Length_Low &= 0xFFFFFFFF;
        if (context->Length_Low == 0)
        {
            context->Length_High++;
            /* Force it to 32 bits */
            context->Length_High &= 0xFFFFFFFF;
            if (context->Length_High == 0)
            {
                /* Message is too long */
                context->Corrupted = 1;
            }
        }

        if (context->Message_Block_Index == 64)
        {
            SHA1ProcessMessageBlock(context);
        }

        message_array++;
    }
}

PBYTE AddSha1HashData(SHA1Context* sha_context, PBYTE pData, PBYTE pToData, DWORD cbSkip)
{
	size_t cbSize = pToData - pData;
	SHA1Input(sha_context, pData, (unsigned int)cbSize);
	return pData + cbSize + cbSkip;
}

BOOL CalcHashFromMappedFile(IN PVOID pFileData, DWORD dwFileSize, IN OUT DWORD* hash_size, OUT BYTE* hash)
{
/*	IMAGE_DOS_HEADER* pMZ;
	IMAGE_NT_HEADERS* pPE;
	IMAGE_SECTION_HEADER* pSec;
	DWORD dwSecurityData, dwSecuritySize;
*/
	SHA1Context sha;
	PBYTE pNextData;
	int i;

	if (hash_size)
	{
		if (*hash_size < 20)
		{
			*hash_size = 20;
			return FALSE;
		}
		*hash_size = 20;
	}

	if (!hash)
		return FALSE;

	SHA1Reset(&sha);
	pNextData = (PBYTE)pFileData;
/*	pMZ = (IMAGE_DOS_HEADER*)pFileData;
	if (pMZ->e_magic != IMAGE_DOS_SIGNATURE)
		return FALSE;
	if (dwFileSize < (ULONG)pMZ->e_lfanew + sizeof(IMAGE_NT_HEADERS))
		return FALSE;
	pPE = (IMAGE_NT_HEADERS*)((PBYTE)pFileData + (ULONG)pMZ->e_lfanew);
	if (pPE->Signature != IMAGE_NT_SIGNATURE)
		return FALSE;
	if (pPE->FileHeader.Machine != IMAGE_FILE_MACHINE_I386) // TODO: IMAGE_FILE_MACHINE_AMD64
		return FALSE;
	pNextData = AddSha1HashData(&sha, pNextData, (PBYTE)&pPE->OptionalHeader.CheckSum, sizeof(pPE->OptionalHeader.CheckSum));
	pNextData = AddSha1HashData(&sha, pNextData, (PBYTE)&pPE->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_SECURITY], sizeof(IMAGE_DATA_DIRECTORY));
	dwSecurityData = pPE->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_SECURITY].VirtualAddress;
	dwSecuritySize = pPE->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_SECURITY].Size;
	// validate security data
	if (!dwSecurityData && dwSecuritySize)
		return FALSE;
	if (dwSecurityData)
	{
		WORD i;
		WORD nSections;
		if (dwSecurityData + dwSecuritySize > dwFileSize)
			return FALSE;
		nSections = pPE->FileHeader.NumberOfSections;
		pSec = (IMAGE_SECTION_HEADER*)((PBYTE)pPE + sizeof(DWORD) + sizeof(IMAGE_FILE_HEADER) + pPE->FileHeader.SizeOfOptionalHeader);
		if ((PBYTE)pSec + nSections*sizeof(IMAGE_SECTION_HEADER) > (PBYTE)pFileData + dwFileSize)
			return FALSE;
		for (i=0; i<nSections; i++, pSec++)
		{
			if (0 == pSec->PointerToRawData)
				continue;
			if (pSec->PointerToRawData + pSec->SizeOfRawData > dwSecurityData)
				return FALSE;
		}
		pNextData = AddSha1HashData(&sha, pNextData, (PBYTE)pFileData + dwSecurityData, dwSecuritySize);
	}
*/	pNextData = AddSha1HashData(&sha, pNextData, (PBYTE)pFileData + dwFileSize, 0);
	if (dwFileSize & 7)
	{
		BYTE fill[8] = {0};
		AddSha1HashData(&sha, fill, fill + 8 - (dwFileSize & 7), 0);
	}

	if (!SHA1Result(&sha))
		return FALSE;
	for (i=0; i<5; i++)
	{
		PBYTE p = (PBYTE)&sha.Message_Digest[i];
		*hash++ = p[3];
		*hash++ = p[2];
		*hash++ = p[1];
		*hash++ = p[0];
	}
	return TRUE;
}

BOOL CalcHashFromFileHandle(IN HANDLE hFile, OUT BYTE* hash)
{
	HANDLE hMapping;
	LPVOID pData;
	BOOL bRes;
	DWORD dwFileSize = GetFileSize(hFile, NULL);
	if (dwFileSize == 0xFFFFFFFF)
		return FALSE;
	if (dwFileSize < sizeof(IMAGE_DOS_HEADER)+sizeof(IMAGE_NT_HEADERS))
		return FALSE;
	hMapping = CreateFileMapping(hFile, 0, PAGE_READONLY, 0, dwFileSize, NULL);
	if (hMapping == NULL)
		return FALSE;
	pData = MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, dwFileSize);
	CloseHandle(hMapping);
	if (!pData)
		return FALSE;

	DWORD hash_size;
	bRes = CalcHashFromMappedFile(pData, dwFileSize, &hash_size, hash);
	UnmapViewOfFile(pData);
	return bRes;
}

////////////////////////////////////////////////////////////////

int IsIOPE(cIO* io)
{
	char data[0x1000];
	io->SeekRead(NULL, 0, data, sizeof(data));

	tQWORD size;
	io->GetSize(&size, IO_SIZE_TYPE_EXPLICIT);

	const IMAGE_DOS_HEADER &dosHdr = *(const IMAGE_DOS_HEADER *)data;
	if( dosHdr.e_magic == IMAGE_DOS_SIGNATURE )
	{
		uint32_t newPeOffs = (uint32_t)dosHdr.e_lfanew;
		if( newPeOffs + sizeof(IMAGE_NT_HEADERS) > newPeOffs &&   // check for overflow
			newPeOffs + sizeof(IMAGE_NT_HEADERS) < size )      // does PE header fit in memory
		{
			IMAGE_NT_HEADERS pHeader;
			io->SeekRead(NULL, dosHdr.e_lfanew, &pHeader, sizeof(pHeader));

			if( pHeader.Signature == IMAGE_NT_SIGNATURE )
				return 1; // PE
		}
	}

	cStrObj sName;
	sName.assign(io, pgOBJECT_NAME);

	size_t pos = sName.find_last_of(".");
	if( pos == std::string::npos )
		return false;

	cStrObj ext;
	ext.assign(sName, (tDWORD)pos);

	WCHAR* sExt[] = {L".EXE", L".COM", L".BAT", L".CMD", L".VBS",
		L".VBE", L".JS", L".JSE", L".WSF", L".WSH", NULL};

	for(int i = 0; sExt[i]; i++)
		if( !ext.compare(sExt[i], fSTRING_COMPARE_CASE_INSENSITIVE) )
			return (i == 0) ? 1 : (i == 1) ? 3 : 2;

	return 0;
}

////////////////////////////////////////////////////////////////

const char* u2f(std::wstring& u, std::string& f)
{
	size_t i;
	for(i = 0; i < u.size(); i++)
	{
		if( u[i] == L'\t' )
			u[i] = L' ';

		if( (unsigned)u[i] < L' ' && u[i] )
			u[i] = '.';
	}

	f.resize(0x1000);
	WideCharToMultiByte(CP_UTF8, 0, u.c_str(), -1, (char*)f.c_str(), 0x1000, NULL, NULL);

	for(i = 0; f[i]; i++)
	{
		if( (unsigned char)f[i] < ' ' )
			f[i] = '.';
	}
	return f.c_str();
}

bool query_io_info(CIOData* pData, int type)
{
	cStrObj sPath;
	FILE* f = NULL;
	HANDLE hFile = NULL;

	pData->bParsed = true;

	cIO* io = pData->io;
	if( io->propGetDWord(pgPLUGIN_ID) == PID_NATIVE_FIO )
	{
		sPath.assign(io, pgOBJECT_FULL_NAME);
		hFile = io->propGetPtr(plNATIVE_HANDLE);
	}
	else
	{
		char* tmpname = _tempnam("c:\\", "avs");
		sPath = tmpname;
		f = fopen(tmpname, "w+b");
		free(tmpname);

		if( !f )
			return false;

		hFile = (HANDLE)_get_osfhandle(f->_file);
	}

	CIOInfo info;
	cAutoObj<cHash> hHash;
	g_root->sysCreateObjectQuick(hHash, IID_HASH, PID_HASH_MD5);

	SHA1Context sha;
	SHA1Reset(&sha);

	tQWORD size, offset = 0;
	tDWORD count = 0;
	char buff[4096] = {0};

	io->GetSize(&size, IO_SIZE_TYPE_EXPLICIT);
	while( PR_SUCC(io->SeekRead(&count, offset, buff, sizeof(buff))) && count )
	{
		offset += count;

		if( f ) fwrite(buff, 1, count, f);

		if( !!hHash )
			hHash->Update((tBYTE*)buff, count);

		SHA1Input(&sha, (PBYTE)buff, count);
	}

	if(size & 7)
	{
		BYTE fill[8] = {0};
		SHA1Input(&sha, fill, (unsigned)(8 - (size & 7)));
	}

	BYTE pHash[20];
	if( SHA1Result(&sha) )
	{
		BYTE *h = pHash;
		for(int i=0; i<5; i++)
		{
			PBYTE p = (PBYTE)&sha.Message_Digest[i];
			*h++ = p[3];
			*h++ = p[2];
			*h++ = p[1];
			*h++ = p[0];
		}
		binary_to_string(pHash, 20, info.qwHashSha1);
	}

	if( !!hHash )
	{
		hHash->GetHash(pHash, 16);
		binary_to_string(pHash, 16, pData->sHash);
		info.qwHashMD5 = pData->sHash;
	}

	memset(pHash, 0, sizeof(pHash));
	if( pData->pParent )
		info.qwParentHashMD5 = pData->pParent->sHash;
	else
		binary_to_string(pHash, 16, info.qwParentHashMD5);

	cStrObj sName;
	static tPROPID propName = 0;
	if( !propName )
		g_root->RegisterCustomPropId(&propName, "npENGINE_VIRTUAL_OBJECT_NAME", pTYPE_STRING);
	sName.assign(io, propName);

	if( sName.empty() )
		sName.assign(io, pgOBJECT_FULL_NAME);

/*	size_t pos = sName.find_last_of("\\/");
	if( pos != cStrObj::npos )
		sName.erase(0, (tDWORD)pos+1);
*/
	info.sFileName = sName.data();
	info.qwFileSize = size;

	if( f ) fclose(f);

	version_query_info(sPath.data(), info);
	signer_get_info(sPath.data(), info);

	if( f ) _unlink(sPath.c_str(cCP_ANSI));

	std::string t[30];
	kl_ftprintf (stdout, KL_TEXT("%S\t%I64i\t%d\t%S\t%S\t%S\t%S\t%S\t%S\t%S\t%S\t%S\t%S\t%S\t%S\t%S\t%S\t%S\t%S\t%S\t%S\t%S\n"),
		u2f(info.sFileName,t[0]),
		info.qwFileSize,
		type,
		"1",
		u2f(info.qwHashMD5,t[1]),
		u2f(info.qwHashSha1,t[2]),
		u2f(info.qwParentHashMD5,t[3]),

		u2f(info.sProductName,t[4]),
		u2f(info.sProductVersion,t[5]),
		u2f(info.sFileVersion,t[6]),
		u2f(info.sCompanyName,t[7]),
		u2f(info.sDescription,t[19]),
		u2f(info.sCopyright,t[20]),

		u2f(info.sSignOk,t[8]),
		u2f(info.sSignerNumber,t[9]),
		u2f(info.sSignerName,t[10]),
		u2f(info.sSignerOrganization,t[11]),
		u2f(info.sIssuer,t[12]),
		u2f(info.sCertNotBefore,t[13]),
		u2f(info.sCertNotAfter,t[14]),

		u2f(info.tmDateOfSign,t[15]),
		u2f(info.sSignerProductName,t[16]),
		u2f(info.sPublisherLink,t[17]),
		u2f(info.sMoreInfoLink,t[18])
		);
	return true;
}

bool query_io_info(cIO* io)
{
	static tPROPID propId = 0;
	static std::list<CIOData> g_data;

	if( !propId )
		g_root->RegisterCustomPropId(&propId, "my_prop", pTYPE_PTR);

	CIOData* pData = (CIOData*)io->propGetPtr(propId);
	if( !pData )
	{
		cObj* pParent = io->sysGetParent(IID_IO);
		CIOData* pPData = pParent ? (CIOData*)pParent->propGetPtr(propId) : NULL;

		if( pPData )
			g_data.push_back(CIOData());
		else
			g_data.resize(1);

		pData = &g_data.back();
		pData->pParent = pPData;

		io->propSetPtr(propId, pData);
	}

	pData->io = io;
	pData->bParsed = false;
	pData->sHash.erase();

	int type = IsIOPE(io);
	if( !type )
		return false;

	if( pData->pParent && !pData->pParent->bParsed )
		query_io_info(pData->pParent, 0);

	query_io_info(pData, type);
	return true;
}

////////////////////////////////////////////////////////////////

#endif //_WIN32
