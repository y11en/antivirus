#ifndef __MSCAT_H_
#define __MSCAT_H_

#include <windows.h>
//#include <wchar.h>

class cSigInfo
{
public:
	//virtual void SetSubjectName(LPCWSTR wcsSubjectOrganization) = 0;
	virtual LPCWSTR GetSubjectName() = 0;
	virtual LPWSTR GetCertName(DWORD dwType, DWORD dwFlags, void* pvTypePara) = 0;
	virtual void FreeCertName(LPWSTR wcsCertName) = 0;
//	virtual cSigInfo* GetRef() = 0;
	virtual void Release() = 0;
};

#define MSC_DONT_LOOKUP_CATALOGS      1
#define MSC_DONT_LOOKUP_EMBEDDED_SIG  2

typedef BOOL (__cdecl *tCallbackGetSHA1Norm)(void* pContext, BYTE sha1[20]);

class cMSCatCache {
public:
	virtual bool IsSigned(LPCWSTR wcsFileName, DWORD dwFlags, OPTIONAL HANDLE hFileHandle = INVALID_HANDLE_VALUE, OPTIONAL OUT cSigInfo** ppSigInfo = NULL, LONG* pWVTResult = NULL, tCallbackGetSHA1Norm pfCallbackGetSHA1Norm = NULL, void* pCallbackContext = NULL) = 0;
};

cMSCatCache* GetMSCatCache(LPCWSTR wcsCacheFile = NULL);
void ReleaseMSCatCache(cMSCatCache*);

#endif // __MSCAT_H_
