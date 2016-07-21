#ifndef __KLPAR_GSOAP_H__
#define __KLPAR_GSOAP_H__

#include "std/par/params.h"

#ifndef soapH_H
#include "soapH.h"
#endif

namespace KLPAR
{
	KLCSC_DECL KLSTD_NOTHROW void ExceptionForSoap(
					struct soap*			soap,
					/*[in]*/KLERR::Error*	pError,
					/*[out]*/param_error&	error) throw();

	KLCSC_DECL KLSTD_NOTHROW bool ExceptionFromSoap(
					/*[in]*/const param_error&	error,
					/*[out]*/KLERR::Error**		ppError) throw();

	KLCSC_DECL KLSTD_NOTHROW char* soap_strdup(struct soap* soap, const char* str) throw();
	KLCSC_DECL KLSTD_NOTHROW wchar_t* soap_strdup(struct soap* soap, const wchar_t* str) throw();

    // throws exception in case of lack of memory
	KLCSC_DECL char* soap_strdup2(struct soap* soap, const char* str);
	KLCSC_DECL wchar_t* soap_strdup2(struct soap* soap, const wchar_t* str);

	KLCSC_DECL void ParamsForSoap(
				/*[in]*/struct soap*	soap,
				/*[in]*/const Params*	pParams,
				/*[out]*/param__params&	params,
                bool                    bIsTransportSoap = true);

	KLCSC_DECL void ParamsFromSoap(
				/*[in]*/param__params&	params,
				/*[out]*/Params**		ppParams);
                
    KLCSC_DECL void ValueForSoap(
				/*[in]*/struct soap*	soap,
				/*[in]*/const Value*	pValue,
				/*[out]*/param__value&	value);

	KLCSC_DECL void ValueFromSoap(
				/*[in]*/param__value&	value,
				/*[out]*/Value**		ppValue);
};

	// Прототипы функций для сериализации/десереализации Params.
SOAP_FMAC1 void SOAP_FMAC2 soap_mark_pparams_t(struct soap*, const struct params_t *);
SOAP_FMAC1 void SOAP_FMAC2 soap_default_pparams_t(struct soap*, struct params_t *);
SOAP_FMAC1 void SOAP_FMAC2 soap_out_pparams_t(struct soap*, const char*, int, const struct params_t *, const char*);
SOAP_FMAC1 struct params_t * SOAP_FMAC2 soap_in_pparams_t(struct soap*, const char*, struct params_t *, const char*);

#endif //__KLPAR_GSOAP_H__
