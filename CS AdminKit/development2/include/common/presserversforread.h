/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	evp/presserversforread.h
 * \author	Mikhail Karmazine
 * \date	25.06.2003 11:22:06
 * \brief	Класс, содержащий PRES-сервера, созданные
            функцией KLPRES_CreateEventsStorageServer2 с
            параметром bSubscribeToEventsToStore = false.
 */


#ifndef __KL_AGENTPRESSERVERS_H__
#define __KL_AGENTPRESSERVERS_H__

#pragma warning( disable : 4786 )

#include <string>
#include <map>
#include <kca/prss/settingsstorage.h>
#include <kca/prci/componentid.h>

namespace KLEVP
{
    class PRESServersForRead
    {
    public:
        void Initialize(  const KLPRCI::ComponentId & cidMaster );
        void Deinitialize();

        std::wstring GetPRESServerId( const KLPRSS::product_version_t & productVersion );
    private:
        typedef std::map<
            KLPRSS::product_version_t,
            std::wstring> ProductToPRESMap;
        ProductToPRESMap m_productToPRESMap;

        void GetPRESPaths(
            const KLPRSS::product_version_t & productVersion,
            std::wstring & wstrPRESFile,
            std::wstring & wstrPRESFolder );

        KLSTD::CAutoPtr<KLSTD::CriticalSection> m_pCS;
    
        KLPRCI::ComponentId m_cidMaster;
    };
}

#endif //__KL_AGENTPRESSERVERS_H__

// Local Variables:
// mode: C++
// End:

