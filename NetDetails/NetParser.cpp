// -------------------------------------------
#include <ProductCore/basedefs.h>
#include <variant.h>

#include "NetParser.h"

CNetParser::CNetParser(void)
{
}

CNetParser::~CNetParser(void)
{
}

tERROR CNetParser::GetSnifferFrameSummary(const cSnifferData* pData, cSnifferFrameReport* pFrameReport)
//-----------------------------------------------------------------------------------------------------
{
	// параной€!!! - убедимс€, что pFrameReport создана на вызывающей стороне
	if (pFrameReport == NULL || pData == NULL)
		return errPARAMETER_INVALID;

	tERROR error = errOK;

	ParserEngine pe;
	ParserEngine::tSummary summary;
	tBYTE* data = (tBYTE*)pData->data();

	pe.init(data, pData->size());
	if (!pe.surface_parse(data, &summary))
		error = errUNEXPECTED;
	else
	{
		pFrameReport->m_IPVersion = summary.is_ipv4 ? cSnifferFrameSummary::eIPVer4 : cSnifferFrameSummary::eIPVer6;
		pFrameReport->m_IPSrc = summary.ip_source_address; 
		pFrameReport->m_IPTgt = summary.ip_destination_address;
		pFrameReport->m_Proto = summary.protocol;
		pFrameReport->m_HighProto = summary.hi_protocol;
	}
	// заполн€етс€ всегда
	pFrameReport->m_EType = summary.ether_type;
	
	return error;
}

tERROR CNetParser::GetSnifferFrameDetails(const cSnifferData* pData, cSnifferFrameDetailsItem* pFrameDetails)
//-----------------------------------------------------------------------------------------------------------
{
	// параной€!!!
	if (pFrameDetails == NULL || pData == NULL)
		return errPARAMETER_INVALID;

	tERROR error = errOK;

	ParserEngine pe;
	pe.init((tBYTE*)pData->data(), pData->size());
	tBYTE* data = (tBYTE*)pData->data();
	
	if (!pe.parse_tree(data, pFrameDetails))
		error = errUNEXPECTED;

	return error;
}


