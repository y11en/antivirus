#ifndef __securlib_h
#define __securlib_h

#ifdef __cplusplus
extern "C" {
#endif

#ifndef SSECURITYDATA_DEFINED
#define SSECURITYDATA_DEFINED

  typedef struct _SSecurityLib {
    unsigned char reserved[16];
  } SSecurityLib;

  typedef struct _SSecuritySession { 
    unsigned char reserved[40];
  } SSecuritySession;

#endif


#define SECLIB_E_BASE                               -3000

#define SECLIB_E_OK                                 (0)
#define SECLIB_E_INVALID_PARAMETER                  (SECLIB_E_BASE - 0)
#define SECLIB_E_FAILED                             (SECLIB_E_BASE - 1)
#define SECLIB_E_INTERNAL_ERROR                     (SECLIB_E_BASE - 2)
#define SECLIB_E_LOAD_SUPPORT_LIB                   (SECLIB_E_BASE - 3)
#define SECLIB_E_GET_SUPPORT_EXPORT_TABLE           (SECLIB_E_BASE - 4)
#define SECLIB_E_INIT_SUPPORT                       (SECLIB_E_BASE - 5)
#define SECLIB_E_QUERY_SUPPORT_INFO                 (SECLIB_E_BASE - 6)
#define SECLIB_E_NO_PACKAGES_INSTALLED              (SECLIB_E_BASE - 7)
#define SECLIB_E_PACKAGE_NOT_FOUND                  (SECLIB_E_BASE - 8)
#define SECLIB_E_ENUM_PACKAGES                      (SECLIB_E_BASE - 9)
#define SECLIB_E_UNKNOWN_CREDENTIALS                (SECLIB_E_BASE - 10)
#define SECLIB_E_NO_CREDENTIALS                     (SECLIB_E_BASE - 11)
#define SECLIB_E_NOT_OWNER                          (SECLIB_E_BASE - 12)
#define SECLIB_E_INSUFFICIENT_MEMORY                (SECLIB_E_BASE - 13)
#define SECLIB_E_SECPKG_NOT_FOUND                   (SECLIB_E_BASE - 14)
#define SECLIB_E_INVALID_TOKEN                      (SECLIB_E_BASE - 15)
#define SECLIB_E_INVALID_HANDLE                     (SECLIB_E_BASE - 16)
#define SECLIB_E_INVALID_SESSION                    (SECLIB_E_BASE - 17)
#define SECLIB_E_LOGON_DENIED                       (SECLIB_E_BASE - 18)
#define SECLIB_E_NO_AUTHENTICATING_AUTHORITY        (SECLIB_E_BASE - 19)
#define SECLIB_E_COMPLETE_NOT_SUPPORTED             (SECLIB_E_BASE - 20)
#define SECLIB_E_TARGET_UNKNOWN                     (SECLIB_E_BASE - 21)
#define SECLIB_E_EXCEPTION                          (SECLIB_E_BASE - 22)
#define SECLIB_E_GET_FUNC_FAILED                    (SECLIB_E_BASE - 23)
#define SECLIB_E_SET_FUNC_FAILED                    (SECLIB_E_BASE - 24)
#define SECLIB_E_IMPERSONATE_FAILED                 (SECLIB_E_BASE - 25)
#define SECLIB_E_LOOKUP_NAME_FAILED                 (SECLIB_E_BASE - 26)
#define SECLIB_E_OPEN_TOKEN                         (SECLIB_E_BASE - 27)
#define SECLIB_E_GET_TOKEN_INFO                     (SECLIB_E_BASE - 28)
                                                     
#define SECLIB_DEFAULT_PROVIDER  ((char*)-1)

int sec_InitLib( SSecurityLib* sec_lib, void*(allocate)(unsigned), void (*free)(void*) );
int sec_ProviderList( SSecurityLib* sec_lib, char** buffer, int* len, int* count );
int sec_TermLib( SSecurityLib* sec_lib );

int sec_InitSession( SSecuritySession* session, SSecurityLib* sec_lib, char* provider_name );
int sec_SelectProvider( SSecuritySession* session, char* provider_name );
int sec_GetContextLen( SSecuritySession* session );
int sec_CloseSession( SSecuritySession* session );

typedef int (*sec_GetFunc)( void* params, void* buffer, int len );
typedef int (*sec_SetFunc)( void* params, void* buffer, int len );

int sec_GenServerContext( SSecuritySession* sess, char *pIn, int cbIn, char *pOut, int *pcbOut );
int sec_GenClientContext( SSecuritySession* sess, char *pIn, int cbIn, char *pOut, int *pcbOut );

int sec_LogonClient( SSecuritySession* sess, sec_GetFunc gf, void* gp, sec_SetFunc sf, void* sp, const char* group );
int sec_LogonToServer( SSecuritySession* sess, sec_GetFunc gf, void* gp, sec_SetFunc sf, void* sp );
int sec_GetUserName( SSecuritySession* sess, char* user, int* cb_User );
int sec_CheckMembership( SSecuritySession* sess, const char* group_name );

int sec_Impersonate( SSecuritySession* sess );
int sec_Revert( SSecuritySession* sess );

int sec_IsDone( SSecuritySession* sess );
int sec_IsLogOn( SSecuritySession* sess );

#ifdef __cplusplus
}
#endif


#endif //__securlib_h