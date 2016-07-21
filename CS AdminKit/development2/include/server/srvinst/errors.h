/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	errors.h
 * \author	Andrew Kazachkov
 * \date	20.08.2003 12:59:48
 * \brief	
 * 
 */

#ifndef __KLSRV_ERRORS_H__
#define __KLSRV_ERRORS_H__

namespace KLSRV
{
    enum ErrorCodes
    {
        KLSRV_ERR_NO_HOST_ADDRESS=KLSRVERRSTART,
        KLSRV_ERR_HOST_NOT_RESPONDING,
        KLSRV_ERR_NO_NAGENT_PACKAGE,
        KLSRV_ERR_SLAVE_NOT_CONNECTED,
		KLSRV_ERR_PACKAGE_EXISTS,
        KLSRV_ERR_GRP_RULE_DPNS_USED,
        KLSRV_ERR_GRP_RULE_OU_USED,
        KLSRV_ERR_GRP_RULE_UNASSIGNED_USED,
        KLSRV_ERR_AKLWNGT_UNEXPECTED_RESPONSE_CODE,
        KLSRV_ERR_AKLWNGT_INVALID_RESPONSE_DATA,
        KLSRV_ERR_AKLWNGT_UNEXPECTED_RESPONSE_DATA_TYPE
    };
        
    const struct KLERR::ErrorDescription c_errorDescriptions [] =
    {
        {KLSRV_ERR_NO_HOST_ADDRESS, L"Location of host '%ls' is unavailable"}, 
        {KLSRV_ERR_HOST_NOT_RESPONDING, L"Host '%ls' is not responding"},
        {KLSRV_ERR_NO_NAGENT_PACKAGE, L"Cannot find package of network agent"},
        {KLSRV_ERR_SLAVE_NOT_CONNECTED, L"Slave server '%ls' isn't connected"},
		{KLSRV_ERR_PACKAGE_EXISTS, L"Package with name '%ls' already exists"},
        {KLSRV_ERR_GRP_RULE_DPNS_USED,   L"Ip diapasone is already used as a rule for group"},
        {KLSRV_ERR_GRP_RULE_OU_USED,   L"Organisation unit is already used as a rule for group"},
        {KLSRV_ERR_GRP_RULE_UNASSIGNED_USED,   L"Group from unassigned is already used as a rule for group"},
        {KLSRV_ERR_AKLWNGT_UNEXPECTED_RESPONSE_CODE, L"Unexpected response code. Code %u expected."},
        {KLSRV_ERR_AKLWNGT_INVALID_RESPONSE_DATA, L"Invalid response data."},
        {KLSRV_ERR_AKLWNGT_UNEXPECTED_RESPONSE_DATA_TYPE, L"Unexpected response data type."},
    };
};

#define KLSRV_THROW_NO_HOST_ADDRESS(_host)    \
                KLERR_THROW1( \
                    L"KLSRV", \
                    KLSRV::KLSRV_ERR_NO_HOST_ADDRESS,\
                    ((const wchar_t*)(_host)))

#define KLSRV_THROW_HOST_NOT_RESPONDING(_addr)    \
                KLERR_THROW1( \
                    L"KLSRV", \
                    KLSRV::KLSRV_ERR_HOST_NOT_RESPONDING,\
                    ((const wchar_t*)(_addr)))

#define KLSRV_THROW_NO_NAGENT_PACKAGE()    \
                KLERR_THROW0(L"KLSRV", KLSRV_ERR_NO_NAGENT_PACKAGE)

#define KLSRV_THROW_SLAVE_NOT_CONNECTED(_inst)  \
                KLERR_THROW1( \
                    L"KLSRV", \
                    KLSRV::KLSRV_ERR_SLAVE_NOT_CONNECTED,\
                    ((const wchar_t*)(_inst)))

#define KLSRV_THROW_PACKAGE_EXISTS(_inst)  \
                KLERR_THROW1( \
                    L"KLSRV", \
                    KLSRV::KLSRV_ERR_PACKAGE_EXISTS,\
                    ((const wchar_t*)(_inst)))

#define KLSRV_THROW_ERR_GRP_RULE_DPNS_USED()    \
                KLERR_THROW0(L"KLSRV", KLSRV::KLSRV_ERR_GRP_RULE_DPNS_USED)

#define KLSRV_THROW_ERR_GRP_RULE_OU_USED()  \
                KLERR_THROW0(L"KLSRV", KLSRV::KLSRV_ERR_GRP_RULE_OU_USED)

#define KLSRV_THROW_ERR_GRP_RULE_UNASSIGNED_USED()  \
                KLERR_THROW0(L"KLSRV", KLSRV::KLSRV_ERR_GRP_RULE_UNASSIGNED_USED)

#define KLSRV_THROW_ERR_AKLWNGT_UNEXPECTED_RESPONSE_CODE(_expected) \
                KLERR_THROW1( \
                    L"KLSRV", \
                    KLSRV::KLSRV_ERR_AKLWNGT_UNEXPECTED_RESPONSE_CODE,\
                    ((unsigned)(_expected)))

#define KLSRV_THROW_ERR_AKLWNGT_INVALID_RESPONSE_DATA()  \
                KLERR_THROW0(L"KLSRV", KLSRV::KLSRV_ERR_AKLWNGT_INVALID_RESPONSE_DATA)

#define KLSRV_THROW_ERR_AKLWNGT_UNEXPECTED_RESPONSE_DATA_TYPE()  \
                KLERR_THROW0(L"KLSRV", KLSRV::KLSRV_ERR_AKLWNGT_UNEXPECTED_RESPONSE_DATA_TYPE)

#endif //__KLSRV_ERRORS_H__