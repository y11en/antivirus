/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	prci_auth.cpp
 * \author	Andrew Kazachkov
 * \date	19.03.2003 12:31:17
 * \brief	
 * 
 */

#include "build/general.h"
#include "std/base/klbase.h"
#include "std/trc/trace.h"
#include "std/err/klerrors.h"
#include "std/err/error.h"
#include "kca/prci/componentinstance.h"
#include "kca/prcp/agentproxy.h"
#include <common/measurer.h>
#include <common/trace_username.h>

#include "transport/wat/authserver.h"

#include "prci_auth.h"

#define KLCS_MODULENAME L"KLPRCI"

using namespace KLSTD;
using namespace KLERR;
using namespace KLPRCI;

#ifdef _WIN32

#include <std/win32/klos_win32v40.h>

#endif //_WIN32

#ifdef KLSTD_WINNT
    static std::wstring GetTextualSid(PSID pSid)    
    {
        PSID_IDENTIFIER_AUTHORITY psia;
        DWORD dwSubAuthorities = 0;
        DWORD dwCounter = 0;
        DWORD cchSidCopy = 0;

        //
        // test if Sid passed in is valid
        //
	    KLSTD_CHK(pSid, IsValidSid(pSid)!=FALSE);

        // obtain SidIdentifierAuthority
        psia = GetSidIdentifierAuthority(pSid);

        // obtain sidsubauthority count
        dwSubAuthorities = *GetSidSubAuthorityCount(pSid);

        //
        // compute approximate buffer length
        // S-SID_REVISION- + identifierauthority- + subauthorities- + NULL
        //
        cchSidCopy = (15 + 12 + (12 * dwSubAuthorities) + 1) * sizeof(wchar_t);

	    wchar_t* TextualSid=(wchar_t*)alloca(cchSidCopy);

        //
        // prepare S-SID_REVISION-
        //
        cchSidCopy = wsprintfW(TextualSid, L"S-%lu-", SID_REVISION );

        //
        // prepare SidIdentifierAuthority
        //
        if ( (psia->Value[0] != 0) || (psia->Value[1] != 0) )
            cchSidCopy += wsprintfW(
					    TextualSid + cchSidCopy,
                        L"0x%02hx%02hx%02hx%02hx%02hx%02hx",
                        (USHORT)psia->Value[0],
                        (USHORT)psia->Value[1],
                        (USHORT)psia->Value[2],
                        (USHORT)psia->Value[3],
                        (USHORT)psia->Value[4],
                        (USHORT)psia->Value[5]);
	    else
            cchSidCopy += wsprintfW(
					    TextualSid + cchSidCopy,
                        L"%lu",
                        (ULONG)(psia->Value[5]      )   +
                        (ULONG)(psia->Value[4] <<  8)   +
                        (ULONG)(psia->Value[3] << 16)   +
                        (ULONG)(psia->Value[2] << 24)   );

        //
        // loop through SidSubAuthorities
        //
        for(dwCounter = 0 ; dwCounter < dwSubAuthorities ; dwCounter++)
	    {
            cchSidCopy += wsprintfW(
							    TextualSid + cchSidCopy,
							    L"-%lu",
							    *GetSidSubAuthority(pSid, dwCounter));
	    };
	    return TextualSid;
    }

    static void GetCurrentToken(HANDLE& hToken)
    {
        hToken=NULL;
        if(!::OpenThreadToken( ::GetCurrentThread(), TOKEN_QUERY, TRUE, & hToken ))
        {
            DWORD dwErrCode = ::GetLastError();
            if( dwErrCode != ERROR_NO_TOKEN )
            {
                KLSTD_TRACE0(1, L"Cannot open thread token:\n" );
                KLSTD_THROW_LASTERROR_CODE( dwErrCode );
            };
            if(!::OpenProcessToken( ::GetCurrentProcess(), TOKEN_QUERY, & hToken ))
            {
                KLSTD_TRACE0(1, L"Cannot open process token:\n" );
                KLSTD_THROW_LASTERROR_CODE( ::GetLastError() );
            };
        };
    };

#endif

    KLCSKCA_DECL std::wstring KLPRCI_GetCurrentSID()
    {
#ifdef KLSTD_WINNT
        std::wstring wstrResult;
        HANDLE hToken=NULL;
        KLERR_TRY
            GetCurrentToken(hToken);
		    DWORD dwLen=0;
		    if(!GetTokenInformation(hToken, TokenUser, NULL, 0, &dwLen) && !dwLen)
			    KLSTD_THROW_LASTERROR();
		    TOKEN_USER* pUserInfo=(TOKEN_USER*)alloca(dwLen);
		    if(!GetTokenInformation(hToken, TokenUser, pUserInfo, dwLen, &dwLen))
			    KLSTD_THROW_LASTERROR();
            wstrResult=GetTextualSid(pUserInfo->User.Sid);
        KLERR_CATCH(pError)
            KLERR_SAY_FAILURE(1, pError);
        KLERR_FINALLY
            if(hToken)
                CloseHandle(hToken);
            KLERR_RETHROW();
        KLERR_ENDTRY
        return wstrResult;
#else
        return L"";
#endif
    };

namespace KLPRCI
{
   
	//Declaration
	class CSecContext : public KLSTD::KLBaseImpl<SecContext>
	{
	public:
		CSecContext(KLWAT::ClientContext* pContext);
		bool ThreadImpersonate(bool bThrowException);
		bool ThreadRevert(bool bThrowException);
		bool AccessCheck(
						KLAVT::AccessControlList*	pAclObj,
						SecContext*					pContext,
						AVP_dword					dwAccessMask,
						const std::wstring*			pwstrBasicAuthLogin,
						bool						bThrow);
	protected:
		KLSTD::CAutoPtr<KLWAT::ClientContext>	    m_pContext;
        KLSTD::CAutoPtr<KLWAT::AccessTokenHolder>   m_pTokenHolder;
	};


	//Implementation

	CSecContext::CSecContext(KLWAT::ClientContext* pContext)
		:	KLSTD::KLBaseImpl<SecContext>()
		,	m_pContext(pContext)
	{
        if(!m_pContext)
            m_pTokenHolder=KLWAT_GetCurrentToken();
    };

	bool CSecContext::ThreadImpersonate(bool bThrowException=true)
	{
		bool bResult=true;
        //KLSTD_ASSERT(m_pContext != NULL);
		KLERR_TRY
			if(m_pContext)
			{
            KL_TMEASURE_BEGIN(L"CSecContext::ThreadImpersonate", 4)
				CAutoPtr<KLWAT::AuthServer> pServer;
				KLWAT_GetGlobalAuthServer(&pServer);
				pServer->ImpersonateThread(m_pContext->GetClientIdentity());
                KLSTD::TraceImpersonated(4);
            KL_TMEASURE_END()
			};
		KLERR_CATCH(pError)
			KLERR_SAY_FAILURE(2, pError);
			if(bThrowException)
				KLERR_RETHROW();
			bResult=false;
		KLERR_ENDTRY
		return bResult;
	};

	bool CSecContext::ThreadRevert(bool bThrowException=false)
	{
		bool bResult=true;
        //KLSTD_ASSERT(m_pContext != NULL);
		KLERR_TRY
			if(m_pContext)
			{
				CAutoPtr<KLWAT::AuthServer> pServer;
				KLWAT_GetGlobalAuthServer(&pServer);
				pServer->RevertThread(m_pContext->GetClientIdentity());
			};
		KLERR_CATCH(pError)
			KLERR_SAY_FAILURE(2, pError);
			if(bThrowException)
				KLERR_RETHROW();
			bResult=false;
		KLERR_ENDTRY
		return bResult;
	};

	bool CSecContext::AccessCheck(
					KLAVT::AccessControlList*	pAclObj,
					SecContext*					pContext,
					AVP_dword					dwAccessMask,
					const std::wstring*			pwstrBasicAuthLogin,
					bool						bThrow)
	{
		CAutoPtr<KLWAT::AuthServer> pServer;
		KLWAT_GetGlobalAuthServer(&pServer);
        KLSTD::CAutoPtr<KLWAT::AccessTokenHolder> pToken;
        if(m_pContext)
            pToken=pServer->GetImpersonationToken(
										m_pContext->GetClientIdentity());
        else
            pToken=m_pTokenHolder;
        KLSTD_ASSERT(pToken);
		bool bResult = true;
		if( pwstrBasicAuthLogin )
		{
			if( pToken->GetType() == KLWAT::atCustom )
			{
				bResult = (pToken->GetCustomAccessToken().wstrLogin.compare(pwstrBasicAuthLogin->c_str()) == 0);
			}
			else
			{
				bResult = false;
			}
		}
		if( bResult )
		{
			bResult = pAclObj->AccessCheck(pToken, dwAccessMask);
		}
		if(bThrow && !bResult)
			KLSTD_THROW(STDE_NOACCESS);
		return bResult;
	};
};


KLCSKCA_DECL bool KLPRCI_AccessCheckAcl(
					KLPRCI::SecContext*						pContext,
					const KLAVT::ACE_DECLARATION*	pAcl,
					long							nAcl,
					AVP_dword						dwAccess,
					bool							bThrowException)
{
	return KLPRCI_AccessCheckEx(pContext, pAcl, nAcl, dwAccess, NULL, bThrowException);
};

KLCSKCA_DECL bool KLPRCI_AccessCheckAclObj(
					KLPRCI::SecContext*				pContext,
					KLAVT::AccessControlList*		pAclObj,
					AVP_dword						dwAccess,
					bool							bThrowException)
{
	KLSTD_ASSERT(pContext && pAclObj);
	return ((CSecContext*)pContext)->AccessCheck(pAclObj, pContext, dwAccess, NULL, bThrowException);
};

KLCSKCA_DECL bool KLPRCI_AccessCheckEx(
					KLPRCI::SecContext*				pContext,
					const KLAVT::ACE_DECLARATION*	pAcl,
					long							nAcl,
					AVP_dword						dwAccess,
					const std::wstring*				pwstrBasicAuthLogin,
					bool							bThrowException)
{
	KLSTD_ASSERT(pContext && pAcl);

	CAutoPtr<KLAVT::AccessControlList> pAclObj;
	KLAVT_CreateACL(pAcl, nAcl, /*L"",*/ &pAclObj);
	return ((KLPRCI::CSecContext*)pContext)->AccessCheck(pAclObj, pContext, dwAccess, pwstrBasicAuthLogin, bThrowException);
}

KLCSKCA_DECL void KLPRCI_CreateSecContext(
					KLWAT::ClientContext*	pContext,
					KLPRCI::SecContext**	ppSecContext)
{
	KLSTD_CHKOUTPTR(ppSecContext);
	*ppSecContext=new KLPRCI::CSecContext(pContext);
	KLSTD_CHKMEM(*ppSecContext);
};
