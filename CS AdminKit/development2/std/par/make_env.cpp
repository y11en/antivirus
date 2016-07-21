/*!
 * (C) 2002 Kaspersky Lab 
 * 
 * \file	make_env.cpp
 * \author	Andrew Kazachkov
 * \date	17.12.2002 13:47:58
 * \brief	Работа с окружением
 * 
 */

#include <std/base/klstd.h>
#include <std/err/error.h>

#if defined(__unix) || defined(N_PLAT_NLM)
#include "_env.h"
#endif

#include <std/par/params.h>
#include <std/par/make_env.h>

#include <build/general.h>

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#ifdef _WIN32

#include <atlbase.h>

using namespace KLSTD;
using namespace KLERR;

#include <list>

#define KLCS_MODULENAME L"KLPAR"

namespace KLPAR{

	static const void* MyGetEnvironmentStrings()
	{
#ifdef _UNICODE
		return (const void*)GetEnvironmentStringsW();
#else
		LPCSTR pEnvironment=GetEnvironmentStringsA();
		int nChars=0;
		for(CHAR* p=(CHAR*)pEnvironment;;)
		{
			int len=strlen(p);
			if(!len)
				break;
			nChars+=len+1;
			p+=len+1;
		};
		++nChars;
		int nWChars=MultiByteToWideChar(CP_ACP,0, pEnvironment, nChars, NULL, 0);
		void* pResult= malloc(nWChars * sizeof(WCHAR));		
        KLSTD_CHKMEM(pResult);
		MultiByteToWideChar(CP_ACP,0, pEnvironment, nChars, (WCHAR*)pResult, nWChars);
		return pResult;
#endif
	};

	static void MyFreeEnvironmentStrings(const void* pEnvironment)
	{
#ifdef _UNICODE
		FreeEnvironmentStrings((LPWSTR)pEnvironment);
#else
		free((void*)pEnvironment);
#endif
	};

	KLCSC_DECL void SplitEnvironmentW_(const void* pEnvironment, KLPAR::Params** ppEnv)
	{
		KLSTD_CHKOUTPTR(ppEnv);
		bool bFree=false;
		CAutoPtr<Params>  pParams;
		KLERR_TRY
			if(!pEnvironment)
			{
				pEnvironment=(const void*)MyGetEnvironmentStrings();
				bFree=true;
				if(!pEnvironment)
					KLSTD_THROW(STDE_GENERAL);
			};
			KLPAR_CreateParams(&pParams);
			for(wchar_t* p=(wchar_t*)pEnvironment;;)
			{
				int len=wcslen(p);
				if(!len)
					break;
				wchar_t* pEqualSign=wcschr(p, L'=');
				KLSTD_ASSERT(pEqualSign);
				if(pEqualSign)
				{
					int namelen=pEqualSign - p;
					wchar_t* pValue=++pEqualSign;					
					std::wstring wstrName(p, namelen);
					if(wstrName.empty())
						wstrName=c_szwEnvName_Empty;
					else
						_wcsupr((wchar_t*)wstrName.c_str());						
					CAutoPtr<StringValue> pStringValue;
					CreateValue(pValue, &pStringValue);
					if(!pParams->DoesExist(wstrName))
						pParams->AddValue(wstrName, pStringValue);
				}
				p+=len+1;
			};
			pParams.CopyTo(ppEnv);
		KLERR_CATCH(pError)
			;
		KLERR_FINALLY
			if(bFree && pEnvironment)
				MyFreeEnvironmentStrings((LPTSTR)pEnvironment);
			KLERR_RETHROW();
		KLERR_ENDTRY	
	};

	KLCSC_DECL void SplitEnvironmentA_(const void* pEnvironment, KLPAR::Params** ppEnv)
	{
		KLSTD_CHKOUTPTR(ppEnv);
		if(pEnvironment)
		{
			int nChars=0;
			for(CHAR* p=(CHAR*)pEnvironment;;)
			{
				int len=strlen(p);
				if(!len)
					break;
				nChars+=len+1;
				p+=len+1;
			};
			++nChars;
			int nWChars=MultiByteToWideChar(CP_ACP, 0, (CHAR*)pEnvironment, nChars, NULL, 0);
			CAutoPtr<MemoryChunk> pChunk;
			KLSTD_AllocMemoryChunk(nChars*sizeof(wchar_t), &pChunk);
			MultiByteToWideChar(
									CP_ACP,
									0,
									(CHAR*)pEnvironment,
									nChars,
									(WCHAR*)pChunk->GetDataPtr(),
									pChunk->GetDataSize()/sizeof(WCHAR));
			SplitEnvironmentW_((WCHAR*)pChunk->GetDataPtr(), ppEnv);
		}
		else
			SplitEnvironmentW_(pEnvironment, ppEnv);
	};
	
	KLCSC_DECL void MakeEnvironmentW_(KLPAR::Params* pEnvironment, KLSTD::MemoryChunk** ppChunk)
	{
		KLSTD_CHKINPTR(pEnvironment);
		
		ParamsNames names;
		pEnvironment->GetNames(names);
		std::list<std::wstring> lstEnv;
		int nFullSize=0;
		for(unsigned int i=0; i < names.size(); ++i)
		{
			bool bEmpty= (wcscmp(c_szwEnvName_Empty, names[i].c_str()) == 0);
			CAutoPtr<StringValue> p_strValue;
			GetValue(pEnvironment, names[i], &p_strValue);
			const wchar_t* p=p_strValue->GetValue();
			std::wstring wstr;			
			wstr+=bEmpty? L"": names[i];
			wstr+=L"=";
			wstr+=(p ? p : L"");
			lstEnv.push_back(wstr);
			nFullSize+=wstr.size()+1;
		};
		++nFullSize;
		CAutoPtr<KLSTD::MemoryChunk> pChunk;
		KLSTD_AllocMemoryChunk(nFullSize*sizeof(wchar_t), &pChunk);
		;
		wchar_t*	pData=(wchar_t*)pChunk->GetDataPtr();
		for(std::list<std::wstring>::iterator it=lstEnv.begin(); it != lstEnv.end(); ++it)
		{
			std::wstring& wstr=*it;
			int delta=wstr.size()+1;
			memcpy(pData, wstr.c_str(), delta*sizeof(wchar_t));
			pData+=delta;
		};
		*pData=0;
		KLSTD_ASSERT(pData-(wchar_t*)pChunk->GetDataPtr() == nFullSize-1);
		pChunk.CopyTo(ppChunk);
	};

	KLCSC_DECL void MakeEnvironmentA_(KLPAR::Params* pEnvironment, KLSTD::MemoryChunk** ppChunk)
	{
		KLSTD_CHKINPTR(pEnvironment);
		CAutoPtr<KLSTD::MemoryChunk> pTmpChunk, pChunk;
		MakeEnvironmentW_(pEnvironment, &pTmpChunk);		
		int nChars=WideCharToMultiByte(
							CP_ACP,
							0,
							(WCHAR*)pTmpChunk->GetDataPtr(),
							pTmpChunk->GetDataSize()/sizeof(wchar_t),
							NULL,
							0,
							NULL,
							NULL);
		KLSTD_AllocMemoryChunk(nChars*sizeof(CHAR), &pChunk);
		WideCharToMultiByte(
							CP_ACP,
							0,
							(WCHAR*)pTmpChunk->GetDataPtr(),
							pTmpChunk->GetDataSize()/sizeof(wchar_t),
							(CHAR*)pChunk->GetDataPtr(),
							pChunk->GetDataSize()/sizeof(char),
							NULL,
							NULL);
		pChunk.CopyTo(ppChunk);
	};
};

#endif // WIN32

#if defined(__unix) || defined(N_PLAT_NLM)

using namespace KLSTD;
using namespace KLERR;


// #include <list>

#define KLCS_MODULENAME L"KLPAR"

namespace KLPAR {

KLCSC_DECL void SplitEnvironmentA_( const void *pEnvironment, KLPAR::Params **ppEnv)
{
  KLSTD_CHKOUTPTR(ppEnv);

  CAutoPtr<Params> pParams;

  KLERR_TRY

    KLPAR_CreateParams(&pParams);
    if ( pEnvironment != 0 ) {
      for ( const wchar_t *e = reinterpret_cast<const wchar_t *>(pEnvironment);
            static_cast<wint_t>(*e) != wint_t(0); e += wcslen( e ) + 1 ) {
        std::wstring s( e );
        std::wstring::size_type p = s.find( L'=' );
        CAutoPtr<StringValue> pStringValue;
        std::wstring v;
        if ( p != std::wstring::npos && (p + 1) < s.size() ) {
          v = s.substr( p + 1 );
        }
        if ( v.size() == 0 ) {
          CreateValue( L"", &pStringValue );
        } else {
          CreateValue( v.c_str(), &pStringValue );
        }
        if ( !pParams->DoesExist( s.substr( 0, p ).c_str() ) ) {
          pParams->AddValue( s.substr( 0, p ).c_str(), pStringValue );
        }
      }
    } else {
      for ( Environment_wide::const_iterator i = Environment_wide::get().begin();
            i != Environment_wide::get().end();
            ++i ) {
        CAutoPtr<StringValue> pStringValue;
        CreateValue( (*i).second.c_str(), &pStringValue );
        if ( !pParams->DoesExist( (*i).first.c_str() ) ) {
          pParams->AddValue( (*i).first.c_str(), pStringValue );
        }
      }
    }
    pParams.CopyTo(ppEnv);

  KLERR_CATCH(pError)
  KLERR_FINALLY
  KLERR_RETHROW();
  KLERR_ENDTRY
}

KLCSC_DECL void MakeEnvironmentA_( KLPAR::Params *pEnvironment, KLSTD::MemoryChunk **ppChunk )
{
  // bogus function: all made twice...

  KLSTD_CHKINPTR(pEnvironment);

  ParamsNames names;
  int nFullSize = 0;
  pEnvironment->GetNames(names);
  
  int j = 0;
  for ( Environment_wide::const_iterator i = Environment_wide::get().begin();
        i != Environment_wide::get().end();
        ++i, ++j ) {
    
    CAutoPtr<StringValue> p_strValue;
    GetValue( pEnvironment, names[j], &p_strValue );
    const wchar_t *p = p_strValue->GetValue();
    std::wstring wstr( names[j] );
    wstr += L"=";
    wstr += p ? p : L"";
    nFullSize += wstr.size() + 1;
  }

  ++nFullSize;

  CAutoPtr<KLSTD::MemoryChunk> pChunk;
  KLSTD_AllocMemoryChunk( nFullSize*sizeof(wchar_t), &pChunk );

  wchar_t *pData = (wchar_t*)pChunk->GetDataPtr();
  int nData = (int)pChunk->GetDataSize();

  j = 0;
  for ( Environment_wide::const_iterator i = Environment_wide::get().begin();
        i != Environment_wide::get().end();
        ++i, ++j ) {
    
    CAutoPtr<StringValue> p_strValue;
    GetValue( pEnvironment, names[j], &p_strValue );
    const wchar_t *p = p_strValue->GetValue();
    std::wstring wstr( names[j] );
    wstr += L"=";
    wstr += p ? p : L"";
    memcpy( pData, wstr.c_str(), (wstr.size() + 1) * sizeof(wchar_t) );
    pData += wstr.size() + 1;
  }

  *pData=0;
  KLSTD_ASSERT(pData-(wchar_t*)pChunk->GetDataPtr() == nFullSize-1);
  pChunk.CopyTo(ppChunk);
}

}

#endif // __unix

