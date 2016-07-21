/*!
 * (C) 2007 Kaspersky Lab 
 * 
 * \file	include\common\rpt\dsh_proc.h
 * \author	Andrew Lashchenkov
 * \date	17.12.2007 16:15
 * \brief	
 * 
 */

#ifndef __KL_DSH_PROC_H_
#define __KL_DSH_PROC_H_

#include <chartgen/klchartpicgen.h>
#include <srvp/rpt/admsrvstat.h>

bool KLDSH_ProcessDiagramDashData(
	KLPAR::ParamsPtr parEntry, 
	KLDSH::EDashboardType& eType,
	std::vector<KLCHARTPICGEN::TDiagramEntry>& data, 
	std::vector<KLCHARTPICGEN::TTimeInterval>& times,
	std::vector<std::wstring>& Series, 
	std::vector<COLORREF>& SeriesColors,
	AVP_byte& btType);

#endif //__KL_DSH_PROC_H_