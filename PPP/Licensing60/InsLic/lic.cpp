
#include "licensing_policy.h"
#include "../1165b.h"

#define _LP(mbr)    LicensingPolicy::##mbr
#define _ILP(mbr)   LicensingPolicy::ILicensingPolicy::##mbr

struct CKeyInfo{
   _LP(string_t)  m_strSerNumber ;
   _LP(string_t)  m_strLicensing ;
   _LP(string_t)  m_strExpDate ;
   _LP(string_t)  m_strCustomerInfo ;
} s_key_info ;

static int writeLicData(const char* s, unsigned int size, void* context)
{
   return size ;
}

static int readLicData(char** s, unsigned int* size, void* context)
{
   return 0 ;
}

BOOL __stdcall CheckFileKey(LPTSTR   p_pszFilePath,
                            LPCTSTR* p_pszSerNumber,
                            LPCTSTR* p_pszLicensing,
                            LPCTSTR* p_pszExpDate,
                            LPCTSTR* p_pszCustomerInfo)
{
   ILicenseStorage licenseStorage ;
   licenseStorage.readSecureData  = readLicData ;
   licenseStorage.writeSecureData = writeLicData ;
   
   _LP(CLicensingPolicy) m_licPolicy ;
   _LP(string_list_t) aBasesPath ;

   HRESULT hResult = m_licPolicy.init(&licenseStorage, AppInfo0, NULL, aBasesPath, _ILP(cmKeyOnly)) ;
   if (FAILED(hResult))
   {
      return FALSE ;
   }

   _LP(string_t) key_file_name = p_pszFilePath ;
   _ILP(check_info_t) check_info ;
   hResult = m_licPolicy.checkKeyFile(key_file_name, &check_info, _ILP(kamAdd)) ;
   if (FAILED(hResult))
   {
      return FALSE ;
   }

   TCHAR pszBuff[1024] ;
   sprintf(pszBuff, "%04X-%06X-%08X", check_info.keyInfo.keySerNum.number.parts.memberId, 
                                      check_info.keyInfo.keySerNum.number.parts.appId,
                                      check_info.keyInfo.keySerNum.number.parts.keySerNum) ;
   s_key_info.m_strSerNumber = pszBuff ;
   *p_pszSerNumber = s_key_info.m_strSerNumber.c_str() ;

   s_key_info.m_strLicensing = check_info.keyInfo.productName ;
   *p_pszLicensing = s_key_info.m_strLicensing.c_str() ;

   sprintf(pszBuff, "%02d/%02d/%04d", check_info.appLicenseExpDate.m_day,
                                      check_info.appLicenseExpDate.m_month,
                                      check_info.appLicenseExpDate.m_year) ;
   s_key_info.m_strExpDate = pszBuff ;
   *p_pszExpDate = s_key_info.m_strExpDate.c_str() ;

   s_key_info.m_strCustomerInfo = check_info.keyInfo.customer_info ;
   *p_pszCustomerInfo = s_key_info.m_strCustomerInfo.c_str() ;

   return TRUE ;
}

BOOL __stdcall CheckFileKeyEx(LPTSTR p_pszFilePath,
                              LPTSTR p_pszSerNumber,
                              LPTSTR p_pszLicensing,
                              LPTSTR p_pszExpDate,
                              LPTSTR p_pszCustomerInfo)
{
   LPCTSTR pszSerNumber    = NULL ;
   LPCTSTR pszLicensing    = NULL ;
   LPCTSTR pszExpDate      = NULL ;
   LPCTSTR pszCustomerInfo = NULL ;

   if (!CheckFileKey(p_pszFilePath, &pszSerNumber, &pszLicensing, &pszExpDate, &pszCustomerInfo))
   {
      return FALSE ;
   }

   if (strlen(pszSerNumber) > 1023)
   {
      strncpy(p_pszSerNumber, pszSerNumber, 1023) ;
      p_pszSerNumber[1024] = '\0' ;
   }
   else
   {
      strcpy(p_pszSerNumber, pszSerNumber) ;
   }

   if (strlen(pszLicensing) > 1023)
   {
      strncpy(p_pszLicensing, pszLicensing, 1023) ;
      p_pszLicensing[1024] = '\0' ;
   }
   else
   {
      strcpy(p_pszLicensing, pszLicensing) ;
   }

   if (strlen(pszExpDate) > 1023)
   {
      strncpy(p_pszExpDate, pszExpDate, 1023) ;
      p_pszExpDate[1024] = '\0' ;
   }
   else
   {
      strcpy(p_pszExpDate, pszExpDate) ;
   }

   if (strlen(pszCustomerInfo) > 1023)
   {
      strncpy(p_pszCustomerInfo, pszCustomerInfo, 1023) ;
      p_pszSerNumber[1024] = '\0' ;
   }
   else
   {
      strcpy(p_pszCustomerInfo, pszCustomerInfo) ;
   }

   return TRUE ;
}
