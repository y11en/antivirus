#ifndef __KL_ADMSRV_SRV_H__
#define __KL_ADMSRV_SRV_H__

#include "./admsrv.h"

KLCSSRVP_DECL void KLADMSRV_CreateAdmServerForConnection(
                const wchar_t*              szTrLocalName,
                const wchar_t*              szTrRemoteName,
                KLADMSRV::AdmServer**       ppServer);


#endif //__KL_ADMSRV_SRV_H__