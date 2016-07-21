// klaverr.cpp
//

#include <klava/klaverr.h>

#ifndef _WIN32_WCE
	#include <errno.h>
#else
	#include <altcecrt.h>
	#include <stddef.h>
#endif


#ifndef KL_PLATFORM_WINCE

#if defined(KL_PLATFORM_WINDOWS) 
#include <winsock2.h>

#define ETIME            ERROR_SEM_TIMEOUT
#define EWOULDBLOCK      WSAEWOULDBLOCK
#define EINPROGRESS      WSAEINPROGRESS
#define EALREADY         WSAEALREADY
#define ENOTSOCK         WSAENOTSOCK
#define EDESTADDRREQ     WSAEDESTADDRREQ
#define EMSGSIZE         WSAEMSGSIZE
#define EPROTOTYPE       WSAEPROTOTYPE
#define ENOPROTOOPT      WSAENOPROTOOPT
#define EPROTONOSUPPORT  WSAEPROTONOSUPPORT
#define ESOCKTNOSUPPORT  WSAESOCKTNOSUPPORT
#define EOPNOTSUPP       WSAEOPNOTSUPP
#define EPFNOSUPPORT     WSAEPFNOSUPPORT
#define EAFNOSUPPORT     WSAEAFNOSUPPORT
#define EADDRINUSE       WSAEADDRINUSE
#define EADDRNOTAVAIL    WSAEADDRNOTAVAIL
#define ENETDOWN         WSAENETDOWN
#define ENETUNREACH      WSAENETUNREACH
#define ENETRESET        WSAENETRESET
#define ECONNABORTED     WSAECONNABORTED
#define ECONNRESET       WSAECONNRESET
#define ENOBUFS          WSAENOBUFS
#define EISCONN          WSAEISCONN
#define ENOTCONN         WSAENOTCONN
#define ESHUTDOWN        WSAESHUTDOWN
#define ETOOMANYREFS     WSAETOOMANYREFS
#define ETIMEDOUT        WSAETIMEDOUT
#define ECONNREFUSED     WSAECONNREFUSED
#define ELOOP            WSAELOOP
#define EHOSTDOWN        WSAEHOSTDOWN
#define EHOSTUNREACH     WSAEHOSTUNREACH
#define EPROCLIM         WSAEPROCLIM
#define EUSERS           WSAEUSERS
#define EDQUOT           WSAEDQUOT
#define ESTALE           WSAESTALE
#define EREMOTE          WSAEREMOTE
#define EADDRINUSE       WSAEADDRINUSE
#define ECANCELED        ERROR_OPERATION_ABORTED
#endif // KL_PLATFORM_WINDOWS

KLAV_ERR klav_translate_errno (int e)
{
	switch (errno)
	{
	case 0:
		return KLAV_OK;
	case ENOMEM:
		return KLAV_ENOMEM;
	case E2BIG:
		return KLAV_ESMALLBUF;
	case EINVAL:
	case EBADF:
		return KLAV_EINVAL;
	case EALREADY:
		return KLAV_EALREADY;
	case ECANCELED:
		return KLAV_ECANCEL;
	case EEXIST:
		return KLAV_EDUPLICATE;
	case EACCES:
		return KLAV_EACCESS;
	case ETIMEDOUT:
		return KLAV_ETIMEOUT;
	case ECONNABORTED:
	case ECONNREFUSED:
	case ECONNRESET:
	case EHOSTUNREACH:
		return KLAV_ENOTCONN;
	default:
		//err = KLAV_MAKE_SYSTEM_ERROR (errno);
		return KLAV_EUNKNOWN;
	}
}

KLAV_ERR klav_return_err (KLAV_ERR err)
{
	if (KLAV_FAILED (err))
	{
		// place to set breakpoint
		return err;
	}
	return err;
}

#endif // KL_PLATFORM_WINCE


