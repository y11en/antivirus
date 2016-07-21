#ifndef __KLPRCP_ERRLOCIDS_H__
#define __KLPRCP_ERRLOCIDS_H__

namespace KLPRCP
{
	enum
    {
		// general errors        
        PRCPEL_ERR_CANT_CONNECT = 1,
        PRCPEL_ERR_UNEXPECTED_STOP,
        
        PRCPEL_TR_GENERAL,
        PRCPEL_TR_WRONG_ADDR,
        PRCPEL_TR_REMOTE_CONN_ALREADY_EXISTS,
        PRCPEL_TR_MAX_CONNECTIONS,
        PRCPEL_TR_AUTHENTICATION_FAILED,
		PRCPEL_TR_SSL_CONNECT_ERR,
        PRCPEL_TR_SSL_SERVER_AUTH_ERR,
        PRCPEL_TR_HOST_NOT_FOUND,
        PRCPEL_TR_SOCKET_NOBUFS        
	};

	const KLERR::ErrorDescription c_LocErrDescriptions[] = 
    {
        {PRCPEL_ERR_CANT_CONNECT, L"Transport level error while connecting to %1: remote host doesn't respond, possibly invalid address or port"},
        {PRCPEL_ERR_UNEXPECTED_STOP, L"Connection was closed unexpectedly"},        
        //these errors must be localized in transport        
        {PRCPEL_TR_GENERAL,   L"Transport level error while connecting to %1: general error %2"},
        {PRCPEL_TR_WRONG_ADDR, L"Transport level error while connecting to %1: ill-formed address"},
        {PRCPEL_TR_REMOTE_CONN_ALREADY_EXISTS, L"Transport level error while connecting to %1: remote host wasn't notified about broken connection. Check proxy/firewall settings. "},
        {PRCPEL_TR_MAX_CONNECTIONS, L"Transport level error while connecting to %1: maximum number of connections was exceeded"},
        {PRCPEL_TR_AUTHENTICATION_FAILED, L"Transport level error while connecting to %1: authentication failure"},
		{PRCPEL_TR_SSL_CONNECT_ERR, L"Transport level error while connecting to %1: SSL connection error, possibly a non-SSL port was used"},
        {PRCPEL_TR_SSL_SERVER_AUTH_ERR, L"Transport level error while connecting to %1: SSL authentication failure, certificate is invalid or out-of-date"},
        {PRCPEL_TR_HOST_NOT_FOUND, L"Transport level error while connecting to %1: failed to resolve address"},
        {PRCPEL_TR_SOCKET_NOBUFS, L"Transport level error while connecting to %1: not enough system memory"}
	};
};

#endif //__KLPRCP_ERRLOCIDS_H__
