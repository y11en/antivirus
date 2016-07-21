#if !defined (_RPC_H_)
#define _RPC_H_

#include <rpc/praguerpc.h>

typedef PragueRPC::ConnectionID handle_t;
typedef PragueRPC::Interface RPC_IF_HANDLE;

typedef int RPC_STATUS;

#define ERROR_SUCCESS     0
#define RPC_E_UNEXPECTED -1

#define MAX_CLIENTS       5

#define __RPC_USER
#define __RPC_FAR
#endif //_RPC_H_
