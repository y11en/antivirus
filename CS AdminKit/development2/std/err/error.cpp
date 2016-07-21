#include <std/base/klbase.h>
#include <std/base/klstdutils.h>
#include <std/fmt/stdfmtstr.h>
#include <std/err/error.h>
#include <std/thr/sync.h>
#include <std/err/klerrors.h>
#include <std/err/errloc_intervals.h>
#include <std/trc/trace.h>

#ifdef N_PLAT_NLM
#include "wcharcrt.h"
#endif

#include <cstdlib>
#include <cstdio>
#include <map>
#include <string>
#include <vector>
#include <algorithm>

#include "./errorimp.h"

#define KLCS_MODULENAME L"KLERR"

using namespace KLSTD;

#define KLERR_TRACE_UNEXPECTED()    \
            KLSTD_ASSERT(!"Unexpected exception");  \
            KLSTD_TRACE3(   \
                    1,  \
                    L"\nUnexpected exception %hs, file %hs, line %d\n\n",   \
                    err.what(), \
                    __FILE__,   \
                    __LINE__)


namespace KLERR
{
	const size_t nMaxFormatBufferSize = 512u;
    const size_t c_nMaxArgs = 1024u;
    const size_t c_nMaxUsualArgs = 9u;
    const size_t c_nModulesHashSize = 31;

    KLSTD::CPointer<modules_t>          g_pModules, g_pLocModules;
    KLSTD::CPointer<map_locmodules_t>   g_pLocDlls;
	CAutoPtr<CriticalSection>	        g_pModulesCS;
    CAutoPtr<CriticalSection>	        g_pErrorCS;
	CAutoPtr<Error>				        g_pNoMemoryError;


    //! must be called under critical section g_pModulesCS !!!
    inline modules_t* GetModulesCS(bool bLoc)
    {
        return bLoc ? g_pLocModules : g_pModules;
    };

	void InitModuleDescriptions(
			const wchar_t*				module,
			const ErrorDescription*		descr,
			int							arraySize,
            bool                        bLoc)
	{
        module = KLSTD::FixNullString(module);

		if(!descr || !module || !arraySize)
        {
			KLSTD_ASSERT(!"Bad params");
            return;
		};
        KLERR::strings_t* pStrings = NULL;        
        try
        {
            AutoCriticalSection acs(KLERR::g_pModulesCS);
            modules_t* pModules = GetModulesCS(bLoc);
            KLSTD_ASSERT(pModules);
            if(!pModules)
                return;
            ;
		    modules_t::iterator it = pModules->find(module);
		    if(it != pModules->end())
			    pStrings=it->second;
		    else
			    pStrings = new KLERR::strings_t;
		    if(pStrings){
			    for(int i=0; i < arraySize; ++i)
				    (*pStrings)[descr[i].id]=descr[i].message;
			    (*pModules)[module]=pStrings;
		    };
        }
        catch(std::exception& err)
        {
            KLERR_TRACE_UNEXPECTED();
        };
	};

	void DeinitModuleDescriptions( 
                const wchar_t * module,
                bool            bLoc)
	{
        module = KLSTD::FixNullString(module);
        try
        {
            AutoCriticalSection acs(KLERR::g_pModulesCS);
            modules_t* pModules = GetModulesCS(bLoc);
            KLSTD_ASSERT(pModules);
            if(!pModules)
                return;
            ;
		    modules_t::iterator it=pModules->find(module);
		    if(it != pModules->end())
            {
			    delete it->second;
			    pModules->erase(it);
		    };
        }
        catch(std::exception& err)
        {
            KLERR_TRACE_UNEXPECTED();
        };
	};

	bool FindString(
                    int       nCode, 
                    AKWSTR&   wstrModule, 
                    AKWSTR&   wstrString,
                    bool      bLoc)
	{
        KLSTD::klwstr_t wstrResModule, wstrResString;
        bool bFound = false;
        try
        {
            AutoCriticalSection acs(KLERR::g_pModulesCS);
            modules_t* pModules = GetModulesCS(bLoc);
            KLSTD_ASSERT(pModules);
            if(!pModules)
                return false;            
		    for(    modules_t::iterator it=pModules->begin(); 
                    !bFound && it!=pModules->end(); ++it)
		    {
			    KLERR::strings_t::iterator it2=it->second->find(nCode);
			    if(it2==it->second->end())
                {
				    wstrResModule = it->first.c_str();
				    wstrResString = it2->second;
				    bFound = true;
			    };
		    };
            if(bFound)
            {
                wstrModule = wstrResModule.detach();
                wstrString = wstrResString.detach();
            };
        }
        catch(std::exception& err)
        {
            KLERR_TRACE_UNEXPECTED();
        };
		return bFound;
	};

	bool FindModuleString(
                    int             nCode, 
                    const wchar_t*  szwModule, 
                    AKWSTR&         wstrString,
                    bool            bLoc)
	{        
        szwModule = KLSTD::FixNullString(szwModule);
        AutoCriticalSection acs(KLERR::g_pModulesCS);
        try
        {
            modules_t* pModules = GetModulesCS(bLoc);
            KLSTD_ASSERT(pModules);
            if(!pModules)
                return false;
            modules_t::iterator it = pModules->find(szwModule);
            if( it != pModules->end())
            {
		        KLERR::strings_t::iterator it2=it->second->find(nCode);
		        if(it2!=it->second->end())
                {
                    wstrString = KLSTD::klwstr_t(it2->second).detach();
			        return true;
		        };
            };
        }
        catch(std::exception& err)
        {
            KLERR_TRACE_UNEXPECTED();
        };
		return false;
	};

    CError::CError()
        :   m_lCustomCode(0)
        ,   m_nId(0)
        ,   m_nLine(0)
    {
        ;
    };

    CError::CError(const CError& err)
        :   m_nId(err.m_nId)
        ,   m_lCustomCode(err.m_lCustomCode)
        ,   m_wsMsgTemplate(err.m_wsMsgTemplate)
        ,   m_wsMessage(err.m_wsMessage)
        ,   m_wsModule(err.m_wsModule)
        ,   m_asFile(err.m_asFile)
        ,   m_nLine(err.m_nLine)
    {
        if(err.m_pParentError)
        {
            KLERR::Error2Ptr pTemp;
            err.m_pParentError->GetError2()->Clone(&pTemp);
            if(pTemp)
                m_pParentError = pTemp->GetError();
        };
        if(err.m_pLocInfo)
            m_pLocInfo = err.m_pLocInfo->Clone();
    };

    CError::~CError()
    {
        ;
    };

	void CError::Initialize(
		long			lCustomCode,
		const wchar_t*	szwModule,			
		long			nId,
		const char*		szaFile, 
		int				nLine,
		const wchar_t*	szwMessage,
        bool            bSilent)
    {
        if(!bSilent)
        {
            KLSTD::Trace(
                        5, 
                        szwModule, 
                        L"Error %u/0x%X occured in file %hs on line %d\n", 
                        nId, 
                        lCustomCode, 
                        szaFile, 
                        nLine);
        };
		m_lCustomCode = lCustomCode;
        m_wsModule = KLSTD::FixNullString(szwModule);
		m_nId = nId;
		m_asFile = KLSTD::FixNullString(szaFile);
		m_nLine = nLine;
		if(szwMessage && szwMessage[0])
        {
            m_wsMsgTemplate = m_wsMessage = szwMessage;
        }
		else
        {
            KLSTD::klwstr_t wstrMsg;
            FindModuleString(nId, m_wsModule.c_str(), wstrMsg.outref(), false);
            m_wsMsgTemplate = (const wchar_t*)wstrMsg;
            m_wsMessage = m_wsMsgTemplate;
            /*
            AutoCriticalSection acs(KLERR::g_pModulesCS);
            modules_t* pModules = GetModulesCS(false);
            KLSTD_ASSERT(pModules);
            if(pModules)
            {
			    it_modules_t it = pModules->find(m_wsModule);
			    if(it != pModules->end())
                {
				    KLERR::strings_t* pStrings=it->second;
				    KLERR::strings_t::iterator it2=pStrings->find(nId);
				    if(it2!=pStrings->end())
                    {
					    m_wsMsgTemplate = m_wsMessage = it2->second;
				    };
			    };
            };
            */
		};
    };

//KLERR::Error, KLERR::Error2
    KLSTD_NOTHROW const wchar_t * CError::GetModuleName() const throw()
    {
        return m_wsModule.c_str();
    };

    KLSTD_NOTHROW int CError::GetId() const throw()
    {
        return m_nId;
    };

	KLSTD_NOTHROW long CError::GetErrorSubcode() const throw()
    {
        return m_lCustomCode;
    };

    KLSTD_NOTHROW const char * CError::GetFileName() const throw()
    {
        return m_asFile.c_str();
    };

    KLSTD_NOTHROW int CError::GetLine() const throw()
    {
        return m_nLine;
    };

    KLSTD_NOTHROW const wchar_t * CError::GetMsg() const throw()
    {
        return m_wsMessage.c_str();
    };

    KLSTD_NOTHROW void CError::SetMessageParams(va_list arg) throw()
    {
        KLSTD::AutoCriticalSection acs(g_pErrorCS);
        SetMessageParamsI(arg);
    };

    KLSTD_NOTHROW void CError::SetMessageParamsI(va_list arg) throw()
    {
		if(!arg)return;

		if(!m_wsMsgTemplate.empty())
        {
            try{
                std::vector<wchar_t> vecBuffer;
                vecBuffer.resize(nMaxFormatBufferSize);
			    KLSTD_VSWPRINTF(
                                &vecBuffer[0],
                                vecBuffer.size()-1,
                                m_wsMsgTemplate.c_str(),
                                arg);
                vecBuffer[vecBuffer.size()-1] = 0;
			    m_wsMessage = &vecBuffer[0];
            }
            catch(std::exception& err)
            {
                KLERR_TRACE_UNEXPECTED();
            };
		};
    };

	KLSTD_NOTHROW void CError::SetErrorSubcode(long lSubCode) throw()
    {
        m_lCustomCode=lSubCode;
    };

    KLSTD_NOTHROW KLERR::Error2* CError::GetError2() throw()
    {
        return static_cast<KLERR::Error2*>(this);
    };

    KLSTD_NOTHROW KLERR::Error* CError::GetError() throw()
    {
        return static_cast<KLERR::Error*>(this);
    };

    KLSTD_NOTHROW void CError::SetPreviousError(KLERR::Error* pError) throw()
    {
        KLSTD::AutoCriticalSection acs(g_pErrorCS);
        m_pParentError = pError;
    };

    KLSTD_NOTHROW void CError::SetPreviousError2(KLERR::Error2* pError) throw()
    {
        KLSTD::AutoCriticalSection acs(g_pErrorCS);
        m_pParentError = (ErrAdapt)pError;
    };

    KLSTD_NOTHROW void CError::GetPreviousError(KLERR::Error** ppError) throw()
    {
        KLSTD::AutoCriticalSection acs(g_pErrorCS);
        if(ppError && !(*ppError) && m_pParentError)
            m_pParentError.CopyTo(ppError);
    };

    KLSTD_NOTHROW void CError::GetPreviousError2(KLERR::Error2** ppError) throw()
    {
        KLSTD::AutoCriticalSection acs(g_pErrorCS);
        if(ppError && !(*ppError) && m_pParentError)
        {
            KLSTD::CAutoPtr<KLERR::Error2> pRes;
			pRes = ErrAdapt(m_pParentError);
            pRes.CopyTo(ppError);
        };
    };

    KLSTD_NOTHROW bool CError::Clone(KLERR::Error2** ppError) throw()
    {
        KLSTD_ASSERT(ppError && !(*ppError));
        KLSTD::AutoCriticalSection acs(g_pErrorCS);
        KLERR::Error2Ptr pResult;
        KLERR::CError* pNew = new CError(*this);
        pResult.Attach( static_cast<KLERR::Error2*>(pNew) );
        pResult.CopyTo(ppError);
        return !!pResult;
    };

//internal
    void CError::AddLocInfoI(CError::LocInfoPtr pLocInfo)
    {
        try{
            KLSTD::AutoCriticalSection acs(g_pErrorCS);
            if(m_pLocInfo)
            {
                pLocInfo->m_pPrevious = m_pLocInfo;
                m_pLocInfo = NULL;
            };            
            m_pLocInfo = pLocInfo;
        }
        catch(std::exception& err)
        {
            KLERR_TRACE_UNEXPECTED();
        };
    };

//KLERR::ErrorLocalization
    KLSTD_NOTHROW long CError::LocGetFormatId() throw()
    {
        KLSTD::AutoCriticalSection acs(g_pErrorCS);
        return m_pLocInfo ? m_pLocInfo->m_nFormat : 0;
    };

    KLSTD_NOTHROW const wchar_t* CError::LocGetFormatString()throw()
    {
        KLSTD::AutoCriticalSection acs(g_pErrorCS);
        return m_pLocInfo ? m_pLocInfo->m_wstrFormat.c_str() : L"";
    };

    KLSTD_NOTHROW const wchar_t* CError::LocGetPar(size_t nIndex)throw()
    {
        KLSTD::AutoCriticalSection acs(g_pErrorCS);
        return (    nIndex < 1 || 
                    nIndex > c_nMaxArgs || 
                    !m_pLocInfo || 
                    m_pLocInfo->m_vecArgs.size() < nIndex
                )
                    ?   L""
                    :   m_pLocInfo->m_vecArgs[nIndex-1].c_str();
    };

    KLSTD_NOTHROW size_t CError::LocGetParCount() throw()
    {
        KLSTD::AutoCriticalSection acs(g_pErrorCS);
        return m_pLocInfo
                    ?   m_pLocInfo->m_vecArgs.size()
                    :   0u;
    };

    KLSTD_NOTHROW bool CError::IsLocalized() throw()
    {
        KLSTD::AutoCriticalSection acs(g_pErrorCS);
        return (    m_pLocInfo && 
                    (   m_pLocInfo->m_nFormat || 
                        !m_pLocInfo->m_wstrFormat.empty() 
                    )
                )
                    ?   true
                    :   false;
    };
    
    KLSTD_NOTHROW const wchar_t* CError::GetLocModuleName() throw()
    {
        KLSTD::AutoCriticalSection acs(g_pErrorCS);
        return (    !m_pLocInfo )
                    ?   GetModuleName()
                    :   m_pLocInfo->m_wstrLocModule.c_str();
    };

    KLSTD_NOTHROW void CError::LocSetInfo(
                        const ErrLocAdapt&  locinfo) throw()
    {
        this->LocSetInfo2(locinfo, L"");
    };
    
    KLSTD_NOTHROW void CError::LocSetInfo2(
                        const ErrLocAdapt&  locinfo,
                        const wchar_t*      szwDefFormat) throw()
    {
        try
        {
            CError::LocInfoPtr pLocInfo;
            pLocInfo.Attach( new CError::LocInfo );
            if(!pLocInfo)
                return;
            if( !locinfo.m_nFormat )
            {//set empty 
                ;//do nothing
            }
            else
            {
                pLocInfo->m_vecArgs.clear();
                pLocInfo->m_vecArgs.reserve(9);
                const wchar_t* pArgs[] = 
                {
                    locinfo.m_szwPar1,
                    locinfo.m_szwPar2,
                    locinfo.m_szwPar3,
                    locinfo.m_szwPar4,
                    locinfo.m_szwPar5,
                    locinfo.m_szwPar6,
                    locinfo.m_szwPar7,
                    locinfo.m_szwPar8,
                    locinfo.m_szwPar9,
                    NULL
                };
                for(size_t i = 0; pArgs[i]; ++i)
                {
                    pLocInfo->m_vecArgs.
                        push_back(std::wstring(KLSTD::FixNullString(pArgs[i])));
                };
            };
            ;
            pLocInfo->m_wstrLocModule = 
                        (locinfo.m_szwLocModuleName && 
                            locinfo.m_szwLocModuleName[0])
                                ?   locinfo.m_szwLocModuleName
                                :   this->GetModuleName();
            if(locinfo.m_nFormat)
            {
                KLSTD::klwstr_t wstrFormat;
                KLERR_FindLocString(
                    pLocInfo->m_wstrLocModule.c_str(),
                    locinfo.m_nFormat,
                    wstrFormat.outref(),
                    KLSTD::FixNullString(szwDefFormat));
                pLocInfo->m_wstrFormat = (const wchar_t*)wstrFormat;
                pLocInfo->m_nFormat = locinfo.m_nFormat;
            };
            AddLocInfoI(pLocInfo);
        }
        catch(std::exception& err)
        {
            KLERR_TRACE_UNEXPECTED();
        };
    };

	void InitModule()
	{
        g_pErrorCS = NULL;
        g_pModulesCS = NULL;        
        g_pModules = NULL;
        g_pLocModules = NULL;
        g_pLocDlls = NULL;
        g_pNoMemoryError = NULL;
        ;
		::KLSTD_CreateCriticalSection(&g_pModulesCS);
        g_pErrorCS = g_pModulesCS;
        ;
        g_pModules = new modules_t(c_nModulesHashSize);
        g_pLocModules = new modules_t(c_nModulesHashSize);
        g_pLocDlls = new map_locmodules_t(c_nModulesHashSize);
        ;
        {
            KLSTD::CAutoPtr<Error> pError;
            CError* p = NULL;
            pError.Attach(p = new RcClassImpl<CError>);
            if(p)
            {
                p->Initialize(
                            0, 
                            L"KLSTD", 
                            STDE_NOMEMORY, 
                            NULL, 
                            0, 
                            L"Out Of memory", 
                            true);
		        g_pNoMemoryError = pError;
            };
        };
	};

	void DeinitModule()
	{
        g_pErrorCS = NULL;
        g_pModulesCS = NULL;
        g_pModules = NULL;
        g_pLocModules = NULL;
        g_pLocDlls = NULL;
        g_pNoMemoryError = NULL;
	};


	KLSTD_NOTHROW KLCSC_DECL void SayFailure(
		int level,
		Error* perror,
		const wchar_t* module,
		const char* file,
		int line) throw()
	{
        int nTraceLevel = 0;
        if(KLSTD::IsTraceStarted(&nTraceLevel) && nTraceLevel >= level)
        {
		    try
            {
			    if(!perror)
                {
                    Trace(
					    level,
					    KLCS_MODULENAME,
					    L"Unknown error was caught in module \"%ls\" in file \"%hs\" on line %d\n",
					    module,
					    file,
					    line);
                }
			    else
                {
                    ErrorPtr pError = perror;
                    for( size_t i = 0; pError; ++i )
                    {
                        KLSTD::klwstr_t wstrLoc;
                        KLERR_LocFormatErrorString(pError, wstrLoc.outref());
                        Trace(
					        level,
					        KLCS_MODULENAME,
					        L"#%u, Error was caught in module \"%ls\" in file \"%hs\" on line %d."
					        L" Error params: (%u/0x%X (\"%ls\"), \"%ls\", \"%hs\", %d)\n\tError loc: '%ls'.\n",
                            (i+1),
					        module,
					        file,
					        line,
					        pError->GetId(),
					        pError->GetErrorSubcode(),
					        pError->GetMsg(),
					        pError->GetModuleName(),
					        pError->GetFileName(),
					        pError->GetLine(),
                            wstrLoc.c_str());
                        ;
                        KLERR::ErrorPtr pOldErr = pError; 
                        pError = NULL;
                        pOldErr->GetError2()->GetPreviousError(&pError);
                    };
                };
            }
            catch(std::exception& err)
            {
                KLERR_TRACE_UNEXPECTED();
            };
        };
	};
};

using namespace KLERR;

KLSTD_NOTHROW void KLERR_InitModuleDescriptions( 
			const wchar_t * szwModule, 
			const KLERR::ErrorDescription * descr,
			int arraySize ) throw()
{
	try
    {
		AutoCriticalSection acs(KLERR::g_pModulesCS);
		KLERR::InitModuleDescriptions(szwModule, descr, arraySize, false);
    }
    catch(std::exception& err)
    {
        KLERR_TRACE_UNEXPECTED();
    };
};

KLSTD_NOTHROW void KLERR_DeinitModuleDescriptions( const wchar_t * szwModule) throw()
{
	try
    {
		AutoCriticalSection acs(KLERR::g_pModulesCS);
		KLERR::DeinitModuleDescriptions(szwModule, false);
    }
    catch(std::exception& err)
    {
        KLERR_TRACE_UNEXPECTED();
    };
};

KLSTD_NOTHROW KLCSC_DECL void KLERR_InitModuleLocalizationDefaults( 
    const wchar_t*					szwModule, 
    const KLERR::ErrorDescription*	descr,
    int								arraySize) throw()
{
	try
    {
		AutoCriticalSection acs(KLERR::g_pModulesCS);
		KLERR::InitModuleDescriptions(szwModule, descr, arraySize, true);
    }
    catch(std::exception& err)
    {
        KLERR_TRACE_UNEXPECTED();
    };
};

KLSTD_NOTHROW KLCSC_DECL void KLERR_DeinitModuleLocalizationDefaults(
                                    const wchar_t* szwModule) throw()
{
	try
    {
		AutoCriticalSection acs(KLERR::g_pModulesCS);
		KLERR::DeinitModuleDescriptions(szwModule, true);
    }
    catch(std::exception& err)
    {
        KLERR_TRACE_UNEXPECTED();
    };
};

namespace KLERR
{
    KLSTD_NOTHROW KLERR::ErrorPtr CreateError(
            const ErrLocAdapt*  pLocInfo,
		    const wchar_t*	    szwModule,
		    int				    nId,
		    const char*		    szaFile, 
		    int				    nLine,
		    const wchar_t*	    szwMessage) throw()
    {
        KLSTD::CAutoPtr<KLERR::Error> pResult;
	    try
        {
		    CError* p = NULL;        
            pResult.Attach(p = new CError);
            if(p)
            {
                p->Initialize(0, szwModule, nId, szaFile, nLine, szwMessage);
                if(pLocInfo)
                    p->LocSetInfo(*pLocInfo);
            };
        }
        catch(std::exception& err)
        {
            KLERR_TRACE_UNEXPECTED();
        };
	    if(!pResult)
		    pResult = KLERR::g_pNoMemoryError;
	    if(!pResult)
		    abort();
        return pResult;
    };
};

using namespace KLSTD;
using namespace KLERR;

KLSTD_NOTHROW void KLERR_CreateError(
		KLERR::Error** ppError,
		const wchar_t*	szwModule,
		int				nId,
		const char*		szaFile, 
		int				nLine,
		const wchar_t*	szwMessage) throw()
{
	KLSTD_ASSERT(ppError && !(*ppError));
    KLSTD::CAutoPtr<KLERR::Error> pResult = 
                CreateError(
                            NULL, 
                            szwModule,
                            nId,
                            szaFile,
                            nLine,
                            szwMessage);
    pResult.CopyTo(ppError);
};


KLSTD_NOTHROW KLCSC_DECL void KLERR_CreateError2(
	KLERR::Error** ppError,
	const wchar_t*	szwModule,
	int				nId,
	const char*		szaFile, 
	int				nLine,
	const wchar_t*	szwMessage, ...) throw()
{
	KLSTD_ASSERT(ppError && !(*ppError));
    KLSTD::CAutoPtr<KLERR::Error> pResult = 
                CreateError(
                            NULL, 
                            szwModule,
                            nId,
                            szaFile,
                            nLine,
                            szwMessage);
	try
    {
	    va_list marker;
	    va_start(marker, szwMessage);
	    pResult->SetMessageParams(marker);
	    va_end(marker);
    }
    catch(std::exception& err)
    {
        KLERR_TRACE_UNEXPECTED();
    };
    pResult.CopyTo(ppError);
};

KLSTD_NOTHROW KLCSC_DECL void KLERR_CreateLocError(
    const ErrLocAdapt&  locinfo,
	KLERR::Error**      ppError,
	const wchar_t*	    szwModule,
	int				    nId,
	const char*		    szaFile, 
	int				    nLine,
	const wchar_t*	    szwMessage, ...) throw()
{
	KLSTD_ASSERT(ppError && !(*ppError));
    KLSTD::CAutoPtr<KLERR::Error> pResult = 
                CreateError(
                            &locinfo, 
                            szwModule,
                            nId,
                            szaFile,
                            nLine,
                            szwMessage);
	try
    {
	    va_list marker;
	    va_start(marker, szwMessage);
	    pResult->SetMessageParams(marker);
	    va_end(marker);
    }
    catch(std::exception& err)
    {
        KLERR_TRACE_UNEXPECTED();
    };
    pResult.CopyTo(ppError);
};

KLCSC_DECL KLSTD_NORETURN void KLERR_throwError(
	const wchar_t*	szwModule,
	int				nId,
	const char*		szaFile, 
	int				nLine,
	const wchar_t*	szwMessage...)
{
    ErrorPtr pError = CreateError(
                    NULL, 
                    szwModule, 
                    nId, 
                    szaFile, 
                    nLine, 
                    szwMessage);
    try
    {
	    va_list lst;
	    va_start(lst, szwMessage);
	    pError->SetMessageParams(lst);
        va_end(lst);
    }
    catch(std::exception& err)
    {
        KLERR_TRACE_UNEXPECTED();
    };
	throw pError.Detach();
};

KLCSC_DECL KLSTD_NORETURN void KLERR_throwLocError(
    const ErrLocAdapt&  locinfo,
	const wchar_t*	    szwModule,
	int				    nId,
	const char*		    szaFile, 
	int				    nLine,
	const wchar_t*	    szwMessage ...)
{
    ErrorPtr pError = CreateError(
                    &locinfo, 
                    szwModule, 
                    nId, 
                    szaFile, 
                    nLine, 
                    szwMessage);
    try
    {
	    va_list lst;
	    va_start(lst, szwMessage);
	    pError->SetMessageParams(lst);
        va_end(lst);
    }
    catch(std::exception& err)
    {
        KLERR_TRACE_UNEXPECTED();
    };
	throw pError.Detach();
};


KLSTD_NOTHROW KLCSC_DECL bool KLERR_FindString(
								int				nCode,
								KLSTD::AKWSTR&	wstrModule,
								KLSTD::AKWSTR&	wstrString) throw()
{
	KLERR_BEGIN
		return KLERR::FindString(nCode, wstrModule, wstrString, false);
	KLERR_END
	return false;
};

KLSTD_NOTHROW KLCSC_DECL bool KLERR_FindModuleString(
							int				nCode,
							const wchar_t*  szwModule,
							KLSTD::AKWSTR&	wstrString) throw()
{
	KLERR_BEGIN
		return KLERR::FindModuleString(
                            nCode, 
                            szwModule, 
                            wstrString, 
                            false);
	KLERR_END
	return false;
};

KLSTD_NOTHROW KLCSC_DECL bool KLERR_IfIgnore(long lCode, ...)throw()
{
    va_list marker;
    va_start( marker, lCode);
    long id=0;
    for(;;)
    {
        id=va_arg( marker, long);
        if(id == 0 || lCode == id)
            break;
    };
    va_end( marker );
    return id != 0;
};

KLSTD_NOTHROW void KLERR_SetErrorLocalization(
                KLERR::Error*   pError,
                long            nFormatStringId,
                const wchar_t*  szwLocModuleName,                
                const wchar_t*  szwPar1,
                const wchar_t*  szwPar2,
                const wchar_t*  szwPar3,
                const wchar_t*  szwPar4,
                const wchar_t*  szwPar5,
                const wchar_t*  szwPar6,
                const wchar_t*  szwPar7,
                const wchar_t*  szwPar8,
                const wchar_t*  szwPar9)
{
    if(!pError)
        return;
    if(!nFormatStringId)
        return;
    ;
    KLSTD::CAutoPtr<KLERR::ErrorLocalization> pErrorLocalization;
    pError->GetError2()->QueryInterface(
            KLSTD_IIDOF(KLERR::ErrorLocalization),
            (void**)&pErrorLocalization);
    KLSTD_ASSERT(pErrorLocalization);
    if(!pErrorLocalization)
        return;
    pErrorLocalization->LocSetInfo(
                    ErrLocAdapt(
                        nFormatStringId,
                        szwLocModuleName,
                        szwPar1,
                        szwPar2,
                        szwPar3,
                        szwPar4,
                        szwPar5,
                        szwPar6,
                        szwPar7,
                        szwPar8,
                        szwPar9));
};

KLSTD_NOTHROW KLCSC_DECL bool KLERR_IsErrorLocalized(KLERR::Error* pError)
{
    if(!pError)
        return false;
    ;
    KLSTD::CAutoPtr<KLERR::ErrorLocalization> pErrorLocalization;
    pError->GetError2()->QueryInterface(
            KLSTD_IIDOF(KLERR::ErrorLocalization),
            (void**)&pErrorLocalization);
    KLSTD_ASSERT(pErrorLocalization);
    if(!pErrorLocalization)
        return false;
    return pErrorLocalization->IsLocalized();
};

KLSTD_NOTHROW bool KLERR_LocFormatErrorString(
                KLERR::Error*   pError,
                KLSTD::AKWSTR&  wstrResult)
{
    if(!pError)
        return false;
    ;
    KLSTD::CAutoPtr<KLERR::ErrorLocalization> pErrorLocalization;
    pError->GetError2()->QueryInterface(
            KLSTD_IIDOF(KLERR::ErrorLocalization),
            (void**)&pErrorLocalization);
    KLSTD_ASSERT(pErrorLocalization);
    if(!pErrorLocalization)
        return false;
    ;
    long nFormat = pErrorLocalization->LocGetFormatId();
    const wchar_t* szwFormat = pErrorLocalization->LocGetFormatString();
    if(!nFormat && !szwFormat[0])
        return false;
    ;
    KLSTD::klwstr_t wstrFormat;
    KLERR_FindLocString(
            pErrorLocalization->GetLocModuleName(),
            nFormat,
            wstrFormat.outref(),
            szwFormat);
    if(!wstrFormat.empty())
    {
        std::vector<const wchar_t*> vecArgs;
        const size_t nCount = pErrorLocalization->LocGetParCount();
        vecArgs.resize(pErrorLocalization->LocGetParCount()+1);
        for(size_t i = 0; i < nCount; ++i)
        {
            vecArgs[i] = pErrorLocalization->LocGetPar(i+1);
        };
        KLSTD_FormatMessage(
                        wstrFormat.c_str(), 
                        &vecArgs[0], 
                        vecArgs.size()-1, 
                        wstrResult);
    };
    return wstrResult && wstrResult[0];
};

KLSTD_NOTHROW bool KLERR_LocFormatModuleString(
                const wchar_t*  szwModule,
                int             nCode,
                KLSTD::AKWSTR&  wstrResult,
                const wchar_t*  szwPar1,
                const wchar_t*  szwPar2,
                const wchar_t*  szwPar3,
                const wchar_t*  szwPar4,
                const wchar_t*  szwPar5,
                const wchar_t*  szwPar6,
                const wchar_t*  szwPar7,
                const wchar_t*  szwPar8,
                const wchar_t*  szwPar9) throw()
{
    KLSTD::klwstr_t wstrFormat;
    KLSTD_ASSERT(szwModule && szwModule[0] && nCode > 0);
    if(szwModule && szwModule[0] && nCode > 0)
    {
    KLERR_BEGIN
        KLERR_FindLocString(szwModule, nCode, wstrFormat.outref());
        if(!wstrFormat.empty())
        {
            const wchar_t* pArgs[] = 
            {
                szwPar1,
                szwPar2,
                szwPar3,
                szwPar4,
                szwPar5,
                szwPar6,
                szwPar7,
                szwPar8,
                szwPar9
            };
            std::vector<const wchar_t*> vecArgs;
            vecArgs.reserve(KLSTD_COUNTOF(pArgs)+1);
            for(size_t i = 0; i < KLSTD_COUNTOF(pArgs) && pArgs[i]; ++i)
            {
                vecArgs.push_back(pArgs[i]);
            };
            vecArgs.push_back(NULL);
            KLSTD_FormatMessage(
                            wstrFormat.c_str(), 
                            &vecArgs[0], 
                            vecArgs.size()-1, 
                            wstrResult);
        };
    KLERR_ENDT(1);
    };
    return wstrResult && wstrResult[0];
};

KLSTD_NOTHROW KLCSC_DECL void KLERR_GetErrorLocDesc(
                KLERR::Error*   pError, 
                KLSTD::AKWSTR&  wstrMessage) throw()
{
    if( !KLERR_IsErrorLocalized(pError) ||
        !KLERR_LocFormatErrorString(pError, wstrMessage) )
    {
        KLSTD::klwstr_t wstrMsg = pError->GetMsg();
        wstrMessage = wstrMsg.detach();
    };
};


/*
#ifdef _DEBUG

	void KLERR_Trace(const char* szaModule, const char* szaFormat, ...)
	{
		va_list x;
		va_start(x, szaFormat);
		#ifdef _WIN32
			char* pBuffer=NULL;
			if(szaModule){
				int len=strlen(szaModule);
				int nBuffer=KLERR::nMaxFormatBufferSize*2+len+1;
				pBuffer=(char*)alloca(nBuffer);
				strcpy(pBuffer, szaModule);
				strcat(pBuffer, ":");
				_vsnprintf(&pBuffer[len+1], nBuffer-1, szaFormat, x);
			}
			else{
				int nBuffer=KLERR::nMaxFormatBufferSize*2+1;
				pBuffer=(char*)alloca(nBuffer);
				_vsnprintf(pBuffer, nBuffer-1, szaFormat, x);
			};
			OutputDebugStringA(pBuffer);
		#else
			if(szaModule)
				fprintf(stderr, "%s:", szaModule);
			vfprintf(stderr, szaFormat, x);
		#endif // _WIN32
		va_end(x);
	};

#endif
*/

KLSTD_NOTHROW KLCSC_DECL const wchar_t* KLERR_FindString2(
						int				                nCode,
                        const KLERR::ErrorDescription*  pData,
                        size_t                          nData) throw()
{
    for(size_t i =0; i < nData; ++i)
    {
        if(pData[i].id == nCode)
            return KLSTD::FixNullString(pData[i].message);
    };
    return L"";
};

//! main loc description search function 
KLSTD_NOTHROW KLCSC_DECL void KLERR_FindLocString(
                                    const wchar_t*	szwModule, 
                                    int             nCode,
                                    KLSTD::AKWSTR&  wstrResult,
                                    const wchar_t*  szwDefault) throw()
{
    szwModule = KLSTD::FixNullString(szwModule);
    szwDefault = KLSTD::FixNullString(szwDefault);    
    ;
    //try to get string from dll
    CLocModuleInfoPtr pExistingInfo;
    {
        AutoCriticalSection acs(KLERR::g_pModulesCS);
        KLSTD_ASSERT(g_pLocDlls);
        if(g_pLocDlls)
        {
		    it_map_locmodules_t it = g_pLocDlls->find(szwModule);
		    if(it != g_pLocDlls->end())
            {
                pExistingInfo = it->second;
            };
        };
    };
    if(pExistingInfo)
    {
        GetLocString(
            pExistingInfo->m_hModule,
            nCode + pExistingInfo->m_nStart,
            szwDefault,
            wstrResult);
    };
    pExistingInfo = NULL;

    //if failed to get it from DLL, get from data set by KLERR_InitModuleLocalizationDefaults
    if(!wstrResult || !wstrResult[0])
    {
        FindModuleString(nCode, szwModule, wstrResult, true);
    };
    
    //if failed use szwDefault
    if(!wstrResult || !wstrResult[0])
    {
        wstrResult = KLSTD::klwstr_t(szwDefault).detach();
    };
};

KLSTD_NOTHROW KLCSC_DECL void KLERR_InitModuleLocalization( 
                                    const wchar_t*	szwModule, 
                                    const wchar_t*	szwFileName, 
                                    int             nStart) throw()
{
    szwFileName = KLSTD::FixNullString(szwFileName);
    szwModule = KLSTD::FixNullString(szwModule);
    ;
    KLSTD_TRACE3(
                1, 
                L"Loading localization for module %ls, filename %ls, base is %u\n",
                szwModule,
                szwFileName,
                nStart);
    bool bNeedChanged = true;
    CLocModuleInfoPtr pExistingInfo, pNewInfo;
    {
        AutoCriticalSection acs(KLERR::g_pModulesCS);
        KLSTD_ASSERT(g_pLocDlls);
        if(g_pLocDlls)
        {
		    it_map_locmodules_t it = g_pLocDlls->find(szwModule);
		    if(it != g_pLocDlls->end())
            {
                pExistingInfo = it->second;
                if(pExistingInfo->m_wstrFileName == szwFileName)
                {
                    if(pExistingInfo->m_nStart != nStart)
                    {
                        pExistingInfo->m_nStart = nStart;
                    };
                    bNeedChanged = false;
                };
            };
		};
    };
    pExistingInfo = NULL;
    if(bNeedChanged)
    {
        AutoLocModuleLoad alml(szwFileName);
        {
            AutoCriticalSection acs(KLERR::g_pModulesCS);
            KLSTD_ASSERT(g_pLocDlls);
            if(g_pLocDlls)
            {
		        it_map_locmodules_t it = g_pLocDlls->find(szwModule);
		        if(it != g_pLocDlls->end())
                {
                    pExistingInfo = it->second;
                    g_pLocDlls->erase(it);
                };
                pNewInfo.Attach(new CLocModuleInfo(
                            alml.Detach(),
                            nStart, 
                            szwFileName));
                g_pLocDlls->insert(
                            val_map_locmodules_t(szwModule,pNewInfo));
            };
        };
    };
    pExistingInfo = NULL;
    pNewInfo = NULL;
};

KLSTD_NOTHROW KLCSC_DECL void KLERR_DeinitModuleLocalization(
                                    const wchar_t* szwModule) throw()
{
    szwModule = KLSTD::FixNullString(szwModule);
    CLocModuleInfoPtr pExistingInfo;
    {
        AutoCriticalSection acs(KLERR::g_pModulesCS);
        KLSTD_ASSERT(g_pLocDlls);
        if(g_pLocDlls)
        {
		    it_map_locmodules_t it = g_pLocDlls->find(szwModule);
		    if(it != g_pLocDlls->end())
            {
                pExistingInfo = it->second;
                g_pLocDlls->erase(it);
            };
        };
    };
    pExistingInfo = NULL;
};

KLSTD_NOTHROW KLCSC_DECL void KLERR_LoadModuleLocalizations(
                                const wchar_t*              szwFileName,
                                const KLERR::ErrLocModule*  pData,
                                size_t                      nData) throw()
{
KLERR_BEGIN
    for(size_t i = 0; i < nData; ++i)
    {
    KLERR_BEGIN
        KLERR_InitModuleLocalization(
                                pData[i].szwModule,
                                szwFileName,
                                pData[i].nBaseId);
    KLERR_ENDT(1)
    };
KLERR_ENDT(1)
};

KLSTD_NOTHROW KLCSC_DECL void KLERR_UnloadModuleLocalizations(
                                const KLERR::ErrLocModule*  pData,
                                size_t                      nData) throw()
{
KLERR_BEGIN
    for(size_t i = 0; i < nData; ++i)
    {
    KLERR_BEGIN
        KLERR_DeinitModuleLocalization(pData[i].szwModule);
    KLERR_ENDT(1)
    };
KLERR_ENDT(1)
};
