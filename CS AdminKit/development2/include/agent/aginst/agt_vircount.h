/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	agt_vircount.h
 * \author	Andrew Kazachkov
 * \date	17.02.2004 15:16:31
 * \brief	
 * 
 */

#ifndef __KL_AGT_VIRCOUNT_H__
#define __KL_AGT_VIRCOUNT_H__

#include <kca/prci/componentid.h>


namespace KLAGT
{
    const wchar_t c_szwVirusCountSubscription[]=L"KLAGT_VFOUND_COUNT";
    const wchar_t c_szwLastFullScanSubscription[]=L"KLAGT_LAST_FSCAN";

    void OnStart_VirusCount(const KLPRCI::ComponentId& idAgent);
    void OnStart_LastFullScan(const KLPRCI::ComponentId& idAgent);
}

#endif //__KL_AGT_VIRCOUNT_H__
