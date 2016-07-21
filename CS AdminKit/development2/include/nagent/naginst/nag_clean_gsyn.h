/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	nag_clean_gsyn.h
 * \author	Andrew Kazachkov
 * \date	26.10.2005 15:00:33
 * \brief	
 * 
 */

#ifndef __NAG_CLEAN_GSYN_H__
#define __NAG_CLEAN_GSYN_H__

namespace KLNAGINST
{
    class CRemoveSs
    {
    public:
        virtual void DoRemoveSs(const std::wstring& wstrLocation) = 0;
    };

    void CleanGsyncs(
                std::vector<KLPRSS::product_version_t>	vecProducts,
                CRemoveSs*                              pRemoveSs);
};

#endif //__NAG_CLEAN_GSYN_H__
