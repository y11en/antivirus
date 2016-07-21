#include "commhdr.h"
#pragma hdrstop

#ifndef STRICT
#define STRICT 1// for Windows.H
#endif

#include <windows.h>

#include "apversion.h"


extern "C" 
extern const GUID IID_IXMLDOMDocument = 
  { 0x2933BF81, 0x7B36, 0x11D2, { 0xB2, 0x0E, 0x00, 0xC0, 0x4F, 0x98, 0x3E, 0x60 } };

extern "C" 
extern const GUID IID_IXMLDOMNode = 
  { 0x2933BF80, 0x7B36, 0x11D2, { 0xB2, 0x0E, 0x00, 0xC0, 0x4F, 0x98, 0x3E, 0x60 } };

extern "C" 
extern const GUID CLSID_DOMDocument = 
  { 0xF6D90F11, 0x9C73, 0x11D3, { 0xB3, 0x2E, 0x00, 0xC0, 0x4F, 0x99, 0x0B, 0xB4 } };


#pragma warning(disable:4786)  // identifier was truncated to '255' characters in the debug information

#pragma warning (push, 3)
#include "licensing_policy.h"
#include "err_defs.h"
#pragma warning (pop)


/*
//#import <msxml3.dll> raw_interfaces_only

struct __declspec(uuid("f6d90f11-9c73-11d3-b32e-00c04f990bb4"))
DOMDocument;
    // [ default ] interface IXMLDOMDocument2
    // [ default, source ] dispinterface XMLDOMDocumentEvents

struct __declspec(uuid("f5078f1b-c551-11d3-89b9-0000f81fe221"))
DOMDocument26;
    // [ default ] interface IXMLDOMDocument2
    // [ default, source ] dispinterface XMLDOMDocumentEvents

struct __declspec(uuid("f5078f32-c551-11d3-89b9-0000f81fe221"))
DOMDocument30;
    // [ default ] interface IXMLDOMDocument2
    // [ default, source ] dispinterface XMLDOMDocumentEvents
*/

#ifdef _DEBUG
#pragma comment(lib, "..\\LICENSING\\lib\\Debug2k_MBCS\\licpolicy.lib")
#pragma comment(lib, "..\\LICENSING\\lib\\Debug2k_MBCS\\property.lib")
#pragma comment(lib, "..\\LICENSING\\lib\\Debug2k_MBCS\\KLDTSer.lib")
#pragma comment(lib, "..\\LICENSING\\lib\\Debug2k_MBCS\\Win32Utils.lib")
#pragma comment(lib, "..\\LICENSING\\lib\\Debug2k_MBCS\\WinAvpIO.lib")
#pragma comment(lib, "..\\LICENSING\\lib\\Debug2k_MBCS\\Sign.lib")
#pragma comment(lib, "..\\LICENSING\\lib\\Debug2k_MBCS\\SWM.lib")
#else
#pragma comment(lib, "..\\LICENSING\\lib\\Release2k_MBCS\\licpolicy.lib")
#pragma comment(lib, "..\\LICENSING\\lib\\Release2k_MBCS\\property.lib")
#pragma comment(lib, "..\\LICENSING\\lib\\Release2k_MBCS\\KLDTSer.lib")
#pragma comment(lib, "..\\LICENSING\\lib\\Release2k_MBCS\\Win32Utils.lib")
#pragma comment(lib, "..\\LICENSING\\lib\\Release2k_MBCS\\WinAvpIO.lib")
#pragma comment(lib, "..\\LICENSING\\lib\\Release2k_MBCS\\Sign.lib")
#pragma comment(lib, "..\\LICENSING\\lib\\Release2k_MBCS\\SWM.lib")
#endif

#ifdef _USRDLL
#pragma comment(lib, "OLEAUT32.LIB") //?? SysFreeString, VariantInit, SysAllocString, VariantClear
#endif


using namespace LicensingPolicy;
 
CLicensingPolicy licPolicy;

static TCHAR LicStorageFile[_MAX_PATH]="";

int writeLicData (const char *s, unsigned int size, void *)
{
    if (!s || !size) {
        return 0;
    }

    FILE *file = _tfopen (LicStorageFile, _T ("wb"));

    if (!file) {
        return 0;
    }

    int r = fwrite (s, size, 1, file);

    fclose (file);

    return r;
}


int readLicData  (char **s, unsigned int *size, void *)
{
    if (!s || !size) {
        return 0;
    }


    FILE *file = _tfopen (LicStorageFile, _T ("rb"));

    if (!file) {
        return 0;
    }

    fseek (file, 0, SEEK_END);

    fpos_t f_size;
  
    fgetpos (file, &f_size);

    if (!f_size) {
        fclose (file);
        return 0;
    }

    *s = reinterpret_cast<char*> (malloc (static_cast<unsigned int>(f_size)));

    if (!*s) {
      return 0;
    }

    *size = static_cast<unsigned int> (f_size);

    fseek (file, 0, SEEK_SET);

    int r = fread (*s, *size, 1, file);

    fclose (file);

    return r ? *size : 0;
}


ILicenseStorage licenseStorage = {readLicData, writeLicData};


#include <appinfo_struct.h>
static _product_info_t PRODUCTS_LIST[] =
{
	{
		LIC_PROD_NUM, 
		LIC_PROD_STRING, 
		LIC_PROD_MAJORVERSION_STRING, 
		0 					 // publick key id
	}
};

_app_info_t AppInfo = 
{
	LIC_APP_DESCRIPTION_STRING,       // Description
	1,                                           // App info structure version
	LIC_APP_CREATE_DATE,                          // Date create
	aitApplicationInfo,                          // application info type (app or component)
	1116,                                        // Application Id
	"Kaspersky Personal Anti-Spam",          // Application Name
	"",                                          // Application version
	IS_RELEASE(),                                // 0 - beta, not 0 - release (app only)
	{  0, 0 },                                   // Components list (app only)
	{  0, 0 },                                   // Applications list (component only)
	{  1, PRODUCTS_LIST },                       // Products list
	{  0, 0 },                                   // Public key list
	{  0, 0 }                                    // Legacy applications compatibility info
};


#include "lic.h"

#include "LicErrors.rh"

static TCHAR *GetErrorString(HRESULT hr, HINSTANCE hinstLang)
{
    static TCHAR szBuff[200];
    /*if (_FACILITY_LICENSING_ != HRESULT_FACILITY (hr)) {
       if(0==FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_MAX_WIDTH_MASK, NULL, g_dwError, 0, buff, 1024, NULL)) {
       }
    }*/
    if(hinstLang) {
        int id=LIC_STR_ERROR+HRESULT_CODE(hr);
        if(!FAILED(hr))
            id=LIC_STR_WARNING+HRESULT_CODE(hr);
        if(LoadString(hinstLang, id, szBuff,200))
            return szBuff;
    }
    return getLicErrMsg(hr);
}



HRESULT initLicensing (const _app_info_t&                appInfo,
                         const string_list_t&              basesPaths,
                         CLicensingPolicy::checking_mode_t checkMode
                        )
{
  return licPolicy.init (&licenseStorage,
                          appInfo,
                          0,
                          basesPaths,
                          (basesPaths.size () ? checkMode : CLicensingPolicy::cmKeyOnly)
                         );
}




BOOL viewInstalledKeys (Item item, LPARAM lParam)
{
    initLic(NULL);

    CLicensingPolicy::check_info_list_t checkList;
    HRESULT hr=licPolicy.getInstalledKeysInfo (&checkList);

    if (SUCCEEDED (hr)) {
        if (!checkList.size ()) {
            return true;
        }

        for(CLicensingPolicy::check_info_list_t::iterator i(checkList.begin()); i!=checkList.end (); i++) {
            CLicensingPolicy::check_info_t *checkInfo = &(i->second);

            TCHAR szKey[100];
            wsprintf(szKey,TEXT("%04x-%06x-%08x"),
                checkInfo->keyInfo.keySerNum.number.parts.memberId,
                checkInfo->keyInfo.keySerNum.number.parts.appId,
                checkInfo->keyInfo.keySerNum.number.parts.keySerNum
                );


            string_t &lpszlicense=checkInfo->keyInfo.licenseInfo;
            string_t &lpszComment=checkInfo->keyInfo.productName;

            TCHAR szExpired[100];
            wsprintf(szExpired,TEXT("%s%d.%02d.%02d"),
                checkInfo->invalidReason==ILicensingPolicy::kirExpired? "key expired: ": "",
                checkInfo->keyExpirationDate.m_day,checkInfo->keyExpirationDate.m_month,checkInfo->keyExpirationDate.m_year);

            item(szKey, lpszlicense.begin(), lpszComment.begin(), szExpired, lParam);
        }

        return true;
    }
    return false;
}


LPCSTR regKey(const TCHAR *keyFileName, HINSTANCE hinstLang)
{
    key_info_t keyInfo;
    HRESULT hr=licPolicy.getKeyFileInfo (keyFileName, &keyInfo);

    if (FAILED (hr)) {
        return GetErrorString(hr, hinstLang);
    }

    hr = licPolicy.addKey (keyFileName);

    if(hr) {
        return GetErrorString(hr, hinstLang);
    }

    return NULL;
}

LPCTSTR deleteKey(LPCTSTR lpszKey, HINSTANCE hinstLang)
{
    key_serial_number_t serNum;
    sscanf(lpszKey,TEXT("%04x-%06x-%08x"),
            &serNum.number.parts.memberId,
            &serNum.number.parts.appId,
            &serNum.number.parts.keySerNum
            );

    HRESULT hr = licPolicy.revokeKey(serNum);

    if(FAILED(hr)) {
        return GetErrorString(hr, hinstLang);
    }

    return NULL;
}

#include "apversion.h"

LPCTSTR initLic(HINSTANCE hinstLang)
{
    if(!*LicStorageFile) {
        TCHAR szDllName[_MAX_PATH];
        { // Look for path in the registry
            static const TCHAR szGlobalOptionsKey[]=TEXT("Software\\")REG_KEY;
            static const TCHAR szValueName[]=TEXT("CatNames.dll");
            szDllName[0]=0;
            HKEY hKey;
            long lResult=RegOpenKeyEx(HKEY_LOCAL_MACHINE, szGlobalOptionsKey, 0, KEY_ALL_ACCESS, &hKey);
            if(ERROR_SUCCESS==lResult) {
                DWORD dwSize=sizeof(szDllName);
                DWORD dwType;
                lResult=RegQueryValueEx(hKey,szValueName,NULL,&dwType,(LPBYTE)szDllName,&dwSize);
                if(ERROR_SUCCESS!=lResult) {
                    szDllName[0]=0;
                }
                RegCloseKey(hKey);
            }
        }
        extern HINSTANCE g_hinst;
        HINSTANCE hinst=g_hinst;
        if(szDllName[0]==0 && hinst) { // Look for the path in directory base on the module instance
            GetModuleFileName(hinst,szDllName,_MAX_PATH);
        }
        TCHAR *cp=strrchr(szDllName,'\\');
        if(cp) lstrcpy(cp+1,TEXT("LicStorage.dat"));
        lstrcpy(LicStorageFile, szDllName);
    }
/*
    static BOOL bInit=FALSE;
    if(bInit) return;
    bInit=TRUE;
*/
    const string_list_t basesPaths;
    HRESULT hr=initLicensing(AppInfo, basesPaths, CLicensingPolicy::cmFullCheck);
    if (FAILED(hr))  {
        return GetErrorString(hr, hinstLang);
    }
    /*
    funcLevel fl;
    LPCSTR error=checkActiveKey(&fl, days, hinstLang);
    if(fl==::flFullFunctionality || fl==::flNoUpdates) {
        key_type_t keyType;
        enum key_type_t
        {
          ktUnknown = 0,
          ktBeta,
          ktTrial,
          ktTest,
          ktOEM,
          ktCommercial
        };
    }
    */
    return NULL;
} 
 

LPCTSTR checkActiveKey (funcLevel *level, int *days, key_type *type, HINSTANCE hinstLang)
{
    if(type) *type=::ktUnknown; 
    if(days) *days=-1;
    CLicensingPolicy::check_info_t checkInfo;
    HRESULT hr=licPolicy.checkActiveKey (&checkInfo);
    if (FAILED (hr))  {
        *level=flError;
        return GetErrorString(hr, hinstLang);
    }

    if(type) *type=(key_type)checkInfo.keyInfo.keyType;
    if(days) *days=checkInfo.daysTillExpiration;
    if(hr==S_FALSE) *level=::flNoFeatures;
    else *level=(funcLevel)checkInfo.funcLevel;
    return NULL;
}



////////////////
HRESULT checkKeyFile(LPCTSTR lpszFile, funcLevel *level) // For setup dll
{
    *level=(funcLevel)0;
    CLicensingPolicy::check_info_t checkInfo;
    HRESULT hr=licPolicy.checkKeyFile (lpszFile, &checkInfo);
    if (FAILED (hr) || (S_FALSE == hr))  {
        return hr;
    }

    *level=(funcLevel)checkInfo.funcLevel;
    return hr;
}
HRESULT addKeyFile(LPCTSTR lpszFile) // For setup dll
{
    HRESULT hr=licPolicy.addKey (lpszFile);
    return hr;
}
HRESULT deleteActiveKey() // For setup dll
{
    HRESULT hr=licPolicy.revokeActiveKey();
    return hr;
}
