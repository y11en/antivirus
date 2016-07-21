/*
*/

#if !defined(__KL_PROTOCOLANALYZER_H)
#define __KL_PROTOCOLANALYZER_H

#include <Prague/iface/i_io.h>
#include "TrafficProtocollerTypes.h"

namespace TrafficProtocoller
{

//////////////////////////////////////////////////////////////////////////
enum content_t
{
	contentProtocolSpecific = 0,
	contentMime,
	contentHtml,
	contentBinary,
	// etc
	contentOther
};

//////////////////////////////////////////////////////////////////////////
class IContentItem
{
public:
	virtual ~IContentItem()  {}
	virtual data_source_t Source() const = 0;
	virtual content_t Type() const = 0;
	virtual hIO DataIO() const = 0;
};

//////////////////////////////////////////////////////////////////////////
class IProtocolAnalyzer
{
public:
	virtual ~IProtocolAnalyzer()  {}
	virtual detect_code_t Detect(data_source_t src, tBYTE* data, tDWORD data_size) = 0;
	virtual tERROR EnqueueData(data_source_t src, tBYTE* data, tDWORD data_size) = 0;
	virtual tBOOL  HasContent() const = 0;
	virtual tERROR ExtractContent(IContentItem** ppContent, tBOOL bForce = cFALSE) = 0;
	virtual tERROR AssembleContent(IContentItem* pContent) = 0;
	virtual tERROR ReleaseContent(IContentItem* pContent) = 0;
};

}

#endif  // !defined(__KL_PROTOCOLANALYZER_H)
