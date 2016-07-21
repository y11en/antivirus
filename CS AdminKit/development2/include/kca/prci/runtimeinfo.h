#ifndef __RUNTIMEINFO_H__vpw2R_ndKKaaRO716B0QO3
#define __RUNTIMEINFO_H__vpw2R_ndKKaaRO716B0QO3

#include <std/par/params.h>
#include <std/thr/sync.h>
#include <kca/prci/componentid.h>

/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	runtimeinfo.h
 * \author	Andrew Kazachkov
 * \date	03.02.2003 18:26:07
 * \brief	
 * 
 */


KLCSKCA_DECL void KLPRCI_GetRuntimeInfo(
				const std::wstring&			wstrLocation,
				const KLPRCI::ComponentId&	id,
				KLPAR::Params**				ppInfo,
				long						lTimeout = KLSTD_INFINITE);

#endif //__RUNTIMEINFO_H__vpw2R_ndKKaaRO716B0QO3

// Local Variables:
// mode: C++
// End:
