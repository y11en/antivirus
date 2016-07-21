/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	errors.h
 * \author	Andrew Kazachkov
 * \date	19.08.2003 13:37:45
 * \brief	
 * 
 */

#ifndef __NAGINST_ERRORS_H__
#define __NAGINST_ERRORS_H__

namespace KLNAG
{
    enum ErrorCodes
    {
        KLNAG_ERR_APP_NOT_RUNNING=KLNAGINSTERRSTART,
        KLNAG_ERR_CMP_NOT_RUNNING,
        KLNAG_ERR_APP_NOTINSTALLED,
        KLNAG_ERR_APP_NOT_RUNNABLE,
        KLNAG_ERR_APP_ALREADY_RUNNING,
        KLNAG_ERR_APP_STOPPED_UNEXPECTEDLY,
        KLNAG_ERR_APP_NOT_STOPPABLE,
        KLNAG_ERR_APP_CANNOT_BE_STOPPED_NOW
    };
        
    const struct KLERR::ErrorDescription c_errorDescriptions [] =
    {
        {KLNAG_ERR_APP_NOT_RUNNING, L"Application '%ls' v %ls is not running"}, 
        {KLNAG_ERR_CMP_NOT_RUNNING, L"Component '%ls' ('%ls') of application '%ls' v. %ls is not running"},
        {KLNAG_ERR_APP_NOTINSTALLED, L"Application '%ls' v %ls is not installed on specified host"},
        {KLNAG_ERR_APP_NOT_RUNNABLE, L"Application '%ls' v %ls does not support manual starting"},
        {KLNAG_ERR_APP_ALREADY_RUNNING, L"Application '%ls' v %ls already running"},
        {KLNAG_ERR_APP_STOPPED_UNEXPECTEDLY, L"Application '%ls' v %ls stopped unexpectedly"},
        {KLNAG_ERR_APP_NOT_STOPPABLE,   L"Application '%ls' v %ls does not support manual stopping"},
        {KLNAG_ERR_APP_CANNOT_BE_STOPPED_NOW, L"Application '%ls' v %ls cannot be stopped now"}
    };
};

#define KLNAG_THROW_CMP_NOT_STARTED(idComponent)    \
                KLERR_MYTHROW4( \
                    KLNAG::KLNAG_ERR_CMP_NOT_RUNNING,\
                    (idComponent).componentName.c_str(),\
                    (idComponent).instanceId.c_str(),\
                    (idComponent).productName.c_str(),\
                    (idComponent).version.c_str())

#define KLNAG_THROW_APP_NOT_RUNNING(idComponent)    \
                KLERR_MYTHROW2( \
                    KLNAG::KLNAG_ERR_APP_NOT_RUNNING,\
                    (idComponent).productName.c_str(),\
                    (idComponent).version.c_str())

#define KLNAG_THROW_APP_NOT_RUNNING2(_product, _version)    \
                KLERR_MYTHROW2( \
                    KLNAG::KLNAG_ERR_APP_NOT_RUNNING,\
                    (const wchar_t*)_product,\
                    (const wchar_t*)_version)

#define KLNAG_THROW_APP_NOTINSTALLED(_product, _version)    \
                KLERR_MYTHROW2( \
                    KLNAG::KLNAG_ERR_APP_NOTINSTALLED,\
                    (const wchar_t*)_product,\
                    (const wchar_t*)_version)

#define KLNAG_THROW_APP_NOT_RUNNABLE(_product, _version)    \
                KLERR_MYTHROW2( \
                    KLNAG::KLNAG_ERR_APP_NOT_RUNNABLE,\
                    (const wchar_t*)_product,\
                    (const wchar_t*)_version)

#define KLNAG_THROW_APP_ALREADY_RUNNING(_product, _version)    \
                KLERR_MYTHROW2( \
                    KLNAG::KLNAG_ERR_APP_ALREADY_RUNNING,\
                    (const wchar_t*)_product,\
                    (const wchar_t*)_version)

#define KLNAG_THROW_APP_STOPPED_UNEXPECTEDLY(_product, _version)    \
                KLERR_MYTHROW2( \
                    KLNAG::KLNAG_ERR_APP_STOPPED_UNEXPECTEDLY,\
                    (const wchar_t*)_product,\
                    (const wchar_t*)_version)

#define KLNAG_THROW_APP_NOT_STOPPABLE(_product, _version)    \
                KLERR_MYTHROW2( \
                    KLNAG::KLNAG_ERR_APP_NOT_STOPPABLE,\
                    (const wchar_t*)_product,\
                    (const wchar_t*)_version)

#define KLNAG_THROW_APP_CANNOT_BE_STOPPED_NOW(_product, _version)    \
                KLERR_MYTHROW2( \
                    KLNAG::KLNAG_ERR_APP_CANNOT_BE_STOPPED_NOW,\
                    (const wchar_t*)_product,\
                    (const wchar_t*)_version)

#endif //__NAGINST_ERRORS_H__
