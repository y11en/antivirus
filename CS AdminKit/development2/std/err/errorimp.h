/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	errorimp.h
 * \author	Andrew Kazachkov
 * \date	25.05.2006 14:29:31
 * \brief	
 * 
 */

#ifndef __ERRORIMP_H__
#define __ERRORIMP_H__

#include <std/base/klbaseqi_imp.h>
#include <std/err/errlocmodule.h>
#include <std/err/error_localized2.h>

#include <hash_map>
#include <vector>

namespace KLERR
{
	typedef std::hash_map<int, const wchar_t*>  strings_t;
    typedef strings_t::iterator                 it_strings_t;
    typedef strings_t::value_type               val_strings_t;

	typedef std::hash_map<std::wstring, strings_t*>  modules_t;
    typedef modules_t::iterator                 it_modules_t;
    typedef modules_t::value_type               val_modules_t;

    class CLocModuleInfo
        :   public KLSTD::KLBaseImpl<KLSTD::KLBase>
    {
    public:
        CLocModuleInfo(
                KLERR::HLOCMODULE hModule, 
                int nStart, 
                const wchar_t* szwFile
        )
            :   m_hModule(hModule)
            ,   m_nStart(nStart)
            ,   m_wstrFileName(KLSTD::FixNullString(szwFile))
        {;};
        const std::wstring  m_wstrFileName;
        KLERR::HLOCMODULE   m_hModule;// may be NULL
        int                 m_nStart;
    };

    typedef KLSTD::CAutoPtr<CLocModuleInfo> CLocModuleInfoPtr;

    typedef std::hash_map<
                    std::wstring, 
                    KLSTD::KLAdapt<CLocModuleInfoPtr>
                    >                                   map_locmodules_t;
    typedef map_locmodules_t::iterator                  it_map_locmodules_t;
    typedef map_locmodules_t::value_type                val_map_locmodules_t;

    class CErrorBase
        :   public KLERR::Error
        ,   public KLERR::Error2
        ,   public KLERR::ErrorLocalization2
    {
        ;
    };
        
    class CError
        :   public KLSTD::KLBaseImpl<CErrorBase>
    {
    public:
        CError();
        CError(const CError& err);
        virtual ~CError();
		void Initialize(
			long			lCustomCode,
			const wchar_t*	szwModule,			
			long			nId,
			const char*		szaFile, 
			int				nLine,
			const wchar_t*	szwMessage,
            bool bSilent = false);

        KLSTD_INTERAFCE_MAP_BEGIN(KLERR::Error)
            KLSTD_INTERAFCE_MAP_ENTRY(KLERR::Error2)
            KLSTD_INTERAFCE_MAP_ENTRY(KLERR::ErrorLocalization)
            KLSTD_INTERAFCE_MAP_ENTRY(KLERR::ErrorLocalization2)
        KLSTD_INTERAFCE_MAP_END()

    //KLERR::Error, KLERR::Error2
        KLSTD_NOTHROW virtual const wchar_t * GetModuleName() const throw();
        KLSTD_NOTHROW virtual int GetId() const throw();
		KLSTD_NOTHROW virtual long GetErrorSubcode() const throw();
        KLSTD_NOTHROW virtual const char * GetFileName() const throw();
        KLSTD_NOTHROW virtual int GetLine() const throw();
        KLSTD_NOTHROW virtual const wchar_t * GetMsg() const throw();
        KLSTD_NOTHROW virtual void SetMessageParams(va_list arg) throw();
		KLSTD_NOTHROW virtual void SetErrorSubcode(long lSubCode) throw();
        KLSTD_NOTHROW virtual KLERR::Error2* GetError2()throw();
        KLSTD_NOTHROW virtual KLERR::Error* GetError() throw();
        KLSTD_NOTHROW virtual void SetPreviousError(KLERR::Error* pError) throw();
        KLSTD_NOTHROW virtual void SetPreviousError2(KLERR::Error2* pError) throw();
        KLSTD_NOTHROW virtual void GetPreviousError(KLERR::Error** ppError) throw();
        KLSTD_NOTHROW virtual void GetPreviousError2(KLERR::Error2** ppError) throw();
        KLSTD_NOTHROW virtual bool Clone(KLERR::Error2** ppError) throw();
    //KLERR::ErrorLocalization
        KLSTD_NOTHROW virtual void LocSetInfo(
                            const ErrLocAdapt&  locinfo) throw();
        KLSTD_NOTHROW virtual long LocGetFormatId()throw();
        KLSTD_NOTHROW virtual const wchar_t* LocGetFormatString()throw();
        KLSTD_NOTHROW virtual const wchar_t* LocGetPar(size_t nIndex)throw();
        KLSTD_NOTHROW virtual size_t LocGetParCount() throw();
        KLSTD_NOTHROW virtual bool IsLocalized() throw();
        KLSTD_NOTHROW virtual const wchar_t* GetLocModuleName() throw();
    //KLERR::ErrorLocalization2
        KLSTD_NOTHROW virtual void LocSetInfo2(
                            const ErrLocAdapt&  locinfo,
                            const wchar_t*      szwDefFormat) throw();
    public:
        KLSTD_NOTHROW void SetMessageParamsI(va_list arg) throw();
    protected:
        class LocInfo;
        typedef KLSTD::CAutoPtr<LocInfo> LocInfoPtr;
        ;
        class LocInfo
            :   public KLSTD::KLBaseImpl<KLSTD::KLBase>
        {
        public:
            LocInfo()
                :   m_nFormat(0)
                ,   m_vecArgs()
            {;};
            LocInfoPtr Clone()
            {
                LocInfoPtr pNew;
                pNew.Attach(new LocInfo);
                if(pNew)
                {
                    pNew->m_nFormat = m_nFormat;
                    pNew->m_wstrFormat = m_wstrFormat;
                    pNew->m_vecArgs = m_vecArgs;
                    pNew->m_wstrLocModule = m_wstrLocModule;
                    if(m_pPrevious)
                        pNew->m_pPrevious = m_pPrevious->Clone();
                };
                return pNew;
            };
            long                        m_nFormat;
            std::wstring                m_wstrFormat;
            std::wstring                m_wstrLocModule;
            std::vector<std::wstring>   m_vecArgs;
            LocInfoPtr                  m_pPrevious;
        };        
    protected:
        void AddLocInfoI(LocInfoPtr pLocInfo);
    protected:
		long			m_nId, m_lCustomCode;
		std::wstring	m_wsMsgTemplate, m_wsMessage;
		std::wstring	m_wsModule;		
		std::string		m_asFile;
		int				m_nLine;
        ;
        KLSTD::CAutoPtr<KLERR::Error>   m_pParentError;
        KLSTD::CAutoPtr<LocInfo>        m_pLocInfo;
    };
};

#endif //__ERRORIMP_H__
