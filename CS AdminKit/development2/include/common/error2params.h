/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	error2params.h
 * \author	Andrew Kazachkov
 * \date	31.01.2003 12:06:19
 * \brief	
 * 
 */


#ifndef __UTILS_H__I8YTkjZ0r_afhgOHjQsyy3
#define __UTILS_H__I8YTkjZ0r_afhgOHjQsyy3

#include <kca/bl/agentbusinesslogic.h>

namespace KAVP
{
	//Сообщения об ошибках
	const wchar_t c_epErrorInfo[] = KLBLAG_ERROR_INFO;				//PARAMS_T
		const wchar_t c_epErrorModule[] = KLBLAG_ERROR_MODULE;		//STRING_T
		const wchar_t c_epErrorCode[] = KLBLAG_ERROR_CODE;			//INT_T
		const wchar_t c_epErrorMsg[] = KLBLAG_ERROR_MSG;			//STRING_T
		const wchar_t c_epErrorFileName[] = KLBLAG_ERROR_FNAME;		//STRING_T
		const wchar_t c_epErrorLineNumber[] = KLBLAG_ERROR_LNUMBER;	//INT_T
        
        const wchar_t c_epErrorLocData[] = KLBLAG_ERROR_LOCDATA;	    //PARAMS_T
        const wchar_t c_epErrorFormatId[] = KLBLAG_ERROR_FORMATID;	    //INT_T
        const wchar_t c_epErrorFormat[] = KLBLAG_ERROR_FORMAT;	        //STRING_T
        const wchar_t c_epErrorFormatArgs[] = KLBLAG_ERROR_FORMATARGS;	//ARRAY_T|STRING_T


	/*!
	  \brief	Error2Params

	  \param	pError [in]
	  \param	ppParams [in/out]
	*/
	KLCSKCA_DECL void Error2Params(KLERR::Error* pError, KLPAR::Params** ppParams);
	

	/*!
	  \brief	MakeErrorTaskResult

	  \param	pError [in]
	  \param	pResult [in/out]
	*/
	KLCSKCA_DECL void MakeErrorTaskResult(KLERR::Error* pError, KLSTD::CAutoPtr<KLPAR::Params>& pResult);

	KLCSKCA_DECL void Params2Error(KLPAR::Params* pParams, KLERR::Error** ppError);

};

#endif //__UTILS_H__I8YTkjZ0r_afhgOHjQsyy3
