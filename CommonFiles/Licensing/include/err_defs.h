#ifndef __ERR_DEFS_H__
#define __ERR_DEFS_H__

#include "platform_compat.h"

#if defined (_MSC_VER)
  #pragma once
#endif

#if defined (_MSC_VER)
  #pragma warning (push, 4)
#endif

//-----------------------------------------------------------------------------

#if !defined (_WIN32)

//-----------------------------------------------------------------------------

#ifndef _HRESULT_DEFINED
#define _HRESULT_DEFINED
typedef long HRESULT;
#endif // !_HRESULT_DEFINED

#define _HRESULT_TYPEDEF_(_sc) ((HRESULT)_sc)


#define SUCCEEDED(Status) ((HRESULT)(Status) >= 0)
#define FAILED(Status) ((HRESULT)(Status)<0)
#define IS_ERROR(Status) ((unsigned long)(Status) >> 31 == SEVERITY_ERROR)

#define HRESULT_CODE(hr)    ((hr) & 0xFFFF)
#define SCODE_CODE(sc)      ((sc) & 0xFFFF)

#define HRESULT_FACILITY(hr)  (((hr) >> 16) & 0x1fff)
#define SCODE_FACILITY(sc)    (((sc) >> 16) & 0x1fff)

#define HRESULT_SEVERITY(hr)  (((hr) >> 31) & 0x1)
#define SCODE_SEVERITY(sc)    (((sc) >> 31) & 0x1)


#define MAKE_HRESULT(sev,fac,code)\
    ((HRESULT) (((unsigned long) (sev)<<31) | ((unsigned long) (fac)<<16) | ((unsigned long)(code))))

#define MAKE_SCODE(sev,fac,code)\
    ((SCODE) (((unsigned long)(sev)<<31) | ((unsigned long)(fac)<<16) | ((unsigned long)(code))) )


#define S_OK                                   ((HRESULT)0x00000000L)
#define S_FALSE                                ((HRESULT)0x00000001L)


#define E_UNEXPECTED                     _HRESULT_TYPEDEF_(0x8000FFFFL)
#define E_NOTIMPL                        _HRESULT_TYPEDEF_(0x80004001L)
#define E_OUTOFMEMORY                    _HRESULT_TYPEDEF_(0x8007000EL)
#define E_INVALIDARG                     _HRESULT_TYPEDEF_(0x80070057L)
#define E_NOINTERFACE                    _HRESULT_TYPEDEF_(0x80004002L)
#define E_POINTER                        _HRESULT_TYPEDEF_(0x80004003L)
#define E_HANDLE                         _HRESULT_TYPEDEF_(0x80070006L)
#define E_ABORT                          _HRESULT_TYPEDEF_(0x80004004L)
#define E_FAIL                           _HRESULT_TYPEDEF_(0x80004005L)
#define E_ACCESSDENIED                   _HRESULT_TYPEDEF_(0x80070005L)

//-----------------------------------------------------------------------------

#else // _WIN32 defined

  #include <windows.h>

#endif // #if !defined (WIN32) && !defined (_WIN32)

//-----------------------------------------------------------------------------

// Licensing specific errors

#define  _FACILITY_LICENSING_           0x10A

enum
{
	KEY_ALREADY_IN_USE = 1
};

enum
{
	KEY_NOT_FOUND                                      = 1,
	NOT_SIGNED                                         = 2,
	SECURE_DATA_CORRUPT                                = 3,
	KEY_CORRUPT                                        = 4,
	KEY_INST_EXP                                       = 5,
	NO_ACTIVE_KEY                                      = 6,
	KEY_IS_INVALID                                     = 7,
	WRITE_SECURE_DATA                                  = 8,
	READ_SECURE_DATA                                   = 9,
	IO_ERROR                                           = 10,
	CANNOT_FIND_BASES                                  = 11,
	CANT_REG_MORE_KEYS                                 = 12,
	NOT_INITIALIZED                                    = 13,
	INVALID_BASES                                      = 14,
	DATA_CORRUPTED                                     = 15,
	CANT_REG_EXPIRED_RESERVE_KEY                       = 16,
	SYS_ERROR                                          = 17,
	BLACKLIST_CORRUPTED                                = 18,
	SIGN_DOESNT_CONFIRM_SIGNATURE                      = 19,
	CANT_REG_BETA_KEY_FOR_RELEASE                      = 20,
	CANT_REG_NON_BETA_KEY_FOR_BETA                     = 21,
	KEY_WRONG_FOR_PRODUCT                              = 22,
	KEY_IS_BLACKLISTED                                 = 23,
	TRIAL_ALREADY_USED                                 = 24,
	KEY_IS_CORRUPTED                                   = 26,
	DIGITAL_SIGN_CORRUPTED                             = 27,
	CANNOT_REG_EXPIRED_NON_COMMERCIAL_KEY              = 28,
	KEY_DOESNT_PROVIDE_SUPPORT                         = 29,
	KEY_CREATION_DATE_INVALID                          = 30,
	XML_PARSE_ERROR                                    = 31,
	CANNOT_INSTALL_SECOND_TRIAL                        = 32,
	BLACKLIST_CORRUPTED_OR_NOT_FOUND                   = 33,
	UPDATE_DESC_CORRUPTED_OR_NOT_FOUND                 = 34,
	LICENSE_INFO_WRONG_FOR_PRODUCT                     = 35,
	CANNOT_REG_NONVALID_RESERVE_KEY                    = 36,
	KEY_INSTALL_DATE_INVALID                           = 37,
	TRIAL_PERIOD_OVER                                  = 38,
	KEY_CONFLICTS_WITH_COMMERCIAL                      = 39,
};

#define LIC_S_KEY_ALREADY_IN_USE   MAKE_HRESULT (0, _FACILITY_LICENSING_, KEY_ALREADY_IN_USE)

#define LIC_E_KEY_NOT_FOUND        MAKE_HRESULT (1, _FACILITY_LICENSING_, KEY_NOT_FOUND)
#define LIC_E_NOT_SIGNED           MAKE_HRESULT (1, _FACILITY_LICENSING_, NOT_SIGNED)
#define LIC_E_SECURE_DATA_CORRUPT  MAKE_HRESULT (1, _FACILITY_LICENSING_, SECURE_DATA_CORRUPT)
#define LIC_E_KEY_CORRUPT          MAKE_HRESULT (1, _FACILITY_LICENSING_, KEY_CORRUPT)
#define LIC_E_KEY_INST_EXP         MAKE_HRESULT (1, _FACILITY_LICENSING_, KEY_INST_EXP)
#define LIC_E_NO_ACTIVE_KEY        MAKE_HRESULT (1, _FACILITY_LICENSING_, NO_ACTIVE_KEY)
#define LIC_E_KEY_IS_INVALID       MAKE_HRESULT (1, _FACILITY_LICENSING_, KEY_IS_INVALID)
#define LIC_E_WRITE_SECURE_DATA    MAKE_HRESULT (1, _FACILITY_LICENSING_, WRITE_SECURE_DATA)
#define LIC_E_READ_SECURE_DATA     MAKE_HRESULT (1, _FACILITY_LICENSING_, READ_SECURE_DATA)
#define LIC_E_IO_ERROR             MAKE_HRESULT (1, _FACILITY_LICENSING_, IO_ERROR)
#define LIC_E_CANNOT_FIND_BASES    MAKE_HRESULT (1, _FACILITY_LICENSING_, CANNOT_FIND_BASES)
#define LIC_E_CANT_REG_MORE_KEYS   MAKE_HRESULT (1, _FACILITY_LICENSING_, CANT_REG_MORE_KEYS)
#define LIC_E_NOT_INITIALIZED      MAKE_HRESULT (1, _FACILITY_LICENSING_, NOT_INITIALIZED)
#define LIC_E_INVALID_BASES        MAKE_HRESULT (1, _FACILITY_LICENSING_, INVALID_BASES)
#define LIC_E_DATA_CORRUPTED       MAKE_HRESULT (1, _FACILITY_LICENSING_, DATA_CORRUPTED)
#define LIC_E_CANT_REG_EXPIRED_RESERVE_KEY MAKE_HRESULT (1, _FACILITY_LICENSING_, CANT_REG_EXPIRED_RESERVE_KEY)
#define LIC_E_SYS_ERROR            MAKE_HRESULT (1, _FACILITY_LICENSING_, SYS_ERROR)
#define LIC_E_BLACKLIST_CORRUPTED  MAKE_HRESULT (1, _FACILITY_LICENSING_, BLACKLIST_CORRUPTED)
#define LIC_E_SIGN_DOESNT_CONFIRM_SIGNATURE  MAKE_HRESULT (1, _FACILITY_LICENSING_, SIGN_DOESNT_CONFIRM_SIGNATURE)

#define  LIC_E_CANT_REG_BETA_KEY_FOR_RELEASE MAKE_HRESULT (1, _FACILITY_LICENSING_, CANT_REG_BETA_KEY_FOR_RELEASE)
#define  LIC_E_CANT_REG_NON_BETA_KEY_FOR_BETA MAKE_HRESULT (1, _FACILITY_LICENSING_, CANT_REG_NON_BETA_KEY_FOR_BETA)
#define  LIC_E_KEY_WRONG_FOR_PRODUCT          MAKE_HRESULT (1, _FACILITY_LICENSING_, KEY_WRONG_FOR_PRODUCT)
#define  LIC_E_KEY_IS_BLACKLISTED             MAKE_HRESULT (1, _FACILITY_LICENSING_, KEY_IS_BLACKLISTED)
#define  LIC_E_TRIAL_ALREADY_USED             MAKE_HRESULT (1, _FACILITY_LICENSING_, TRIAL_ALREADY_USED)
#define  LIC_E_KEY_IS_CORRUPTED               MAKE_HRESULT (1, _FACILITY_LICENSING_, KEY_IS_CORRUPTED)
#define  LIC_E_DIGITAL_SIGN_CORRUPTED         MAKE_HRESULT (1, _FACILITY_LICENSING_, DIGITAL_SIGN_CORRUPTED)
#define  LIC_E_CANNOT_REG_EXPIRED_NON_COMMERCIAL_KEY MAKE_HRESULT (1, _FACILITY_LICENSING_, CANNOT_REG_EXPIRED_NON_COMMERCIAL_KEY)
#define  LIC_W_KEY_DOESNT_PROVIDE_SUPPORT MAKE_HRESULT (0, _FACILITY_LICENSING_, KEY_DOESNT_PROVIDE_SUPPORT)
#define  LIC_E_KEY_CREATION_DATE_INVALID     MAKE_HRESULT (1, _FACILITY_LICENSING_, KEY_CREATION_DATE_INVALID)
#define  LIC_E_XML_PARSE_ERROR     MAKE_HRESULT (1, _FACILITY_LICENSING_, XML_PARSE_ERROR)
#define  LIC_E_CANNOT_INSTALL_SECOND_TRIAL     MAKE_HRESULT (1, _FACILITY_LICENSING_, CANNOT_INSTALL_SECOND_TRIAL)
#define  LIC_E_BLACKLIST_CORRUPTED_OR_NOT_FOUND     MAKE_HRESULT (1, _FACILITY_LICENSING_, BLACKLIST_CORRUPTED_OR_NOT_FOUND)
#define  LIC_E_UPDATE_DESC_CORRUPTED_OR_NOT_FOUND     MAKE_HRESULT (1, _FACILITY_LICENSING_, UPDATE_DESC_CORRUPTED_OR_NOT_FOUND)
#define  LIC_E_LICENSE_INFO_WRONG_FOR_PRODUCT   MAKE_HRESULT (1, _FACILITY_LICENSING_, LICENSE_INFO_WRONG_FOR_PRODUCT)
#define  LIC_E_CANNOT_REG_NONVALID_RESERVE_KEY  MAKE_HRESULT (1, _FACILITY_LICENSING_, CANNOT_REG_NONVALID_RESERVE_KEY)
#define  LIC_E_KEY_INSTALL_DATE_INVALID			MAKE_HRESULT (1, _FACILITY_LICENSING_, KEY_INSTALL_DATE_INVALID)
#define  LIC_E_TRIAL_PERIOD_OVER				MAKE_HRESULT (1, _FACILITY_LICENSING_, TRIAL_PERIOD_OVER)
#define  LIC_E_KEY_CONFLICTS_WITH_COMMERCIAL	MAKE_HRESULT (1, _FACILITY_LICENSING_, KEY_CONFLICTS_WITH_COMMERCIAL)

//-----------------------------------------------------------------------------

#if defined (__cplusplus)
extern "C"
{
#endif

_TCHAR* getLicErrMsg (HRESULT hr);

#if defined (__cplusplus)
}
#endif


//-----------------------------------------------------------------------------

#if defined (_MSC_VER)
  #pragma warning (pop)
#endif

#endif
