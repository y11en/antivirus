#ifndef __KLADHOSTSEARCH_H__
#define __KLADHOSTSEARCH_H__

#include<std/wnf/windowsnetinfo.h>

namespace KLWNF
{
    class KLSTD_NOVTABLE AdHostSearch
            :   public KLSTD::KLBase
    {
    public:
        /*!
          \brief	Finds host in AD by DNS name

          \param	dnsname     host DNS name (dNSHostName)
          \param	info        host info
        */
        virtual void FindHostAD(const std::wstring& dnsname, ADObjectInfo& info) = 0;
    };

    typedef KLSTD::CAutoPtr<KLWNF::AdHostSearch> AdHostSearchPtr;
};

void KLWNF_CreateAdHostSearch(KLWNF::AdHostSearch** pp);

#endif //__KLADHOSTSEARCH_H__
