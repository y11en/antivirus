/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	std/err/errlocmodule.h
 * \author	Andrew Kazachkov
 * \date	25.05.2006 14:21:06
 * \brief	Erroro localization module
 * 
 */

#ifndef __KL_ERRLOCMODULE_H__
#define __KL_ERRLOCMODULE_H__

namespace KLERR
{
    KLSTD_DECLARE_HANDLE(HLOCMODULE); // KLERR::HLOCMODULE

    /*!
      \brief	Loads localization module

      \param	szwFileName IN module file name
      \return	module handle
    */
    KLSTD_NOTHROW KLCSC_DECL HLOCMODULE LoadLocModule(
                            const wchar_t* szwFileName) throw();
    
    /*!
      \brief	Unloads localization module previousely loaded by 
                LoadLocModule

      \param	hModule IN module handle
    */
    KLSTD_NOTHROW KLCSC_DECL void UnloadLocModule(
                            HLOCMODULE hModule) throw();
    

    /*!
      \brief	Loads localization string from specified module. if failed 
                uses default szwDefault and returns false.

      \param	hModule IN module handle
      \param	nID IN id of string
      \param	szwDefault IN default value, NULL is equivalent to L""
      \return	false if default used
    */
    KLSTD_NOTHROW KLCSC_DECL bool GetLocString(
                            HLOCMODULE      hModule,
                            unsigned        nID,
                            const wchar_t*  szwDefault,
                            KLSTD::AKWSTR&  wstrResult) throw();

    class AutoLocModuleLoad
    {
    public:
        explicit AutoLocModuleLoad(const wchar_t* szwFileName)
            :   m_hModule(NULL)
        {
            m_hModule = LoadLocModule(szwFileName);
        };
        ~AutoLocModuleLoad()
        {
            if(m_hModule)
                UnloadLocModule(m_hModule);
        };
        operator HLOCMODULE()
        {
            return m_hModule;
        };
        HLOCMODULE Detach()
        {
            HLOCMODULE res = m_hModule;
            m_hModule = NULL;
            return res;
        };
    protected:
        HLOCMODULE      m_hModule;
    };
};

#endif //__KL_ERRLOCMODULE_H__
