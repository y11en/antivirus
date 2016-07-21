/*
*/

#if !defined(__KL_IPROTOCOLANALYZER_H)
#define __KL_IPROTOCOLANALYZER_H

#include <Prague/prague.h>
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
	contentPartition,
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
	virtual hIO DetachDataIO() = 0;
	virtual void RecreateDataIO(const char* data = 0, size_t size = 0) = 0;
};

//////////////////////////////////////////////////////////////////////////
enum analysis_state_t
{
	analyzeDetect = 0,
	analyzeNormal,
	analyzeWaitHugeData,
	analyzeError
};

//////////////////////////////////////////////////////////////////////////
class INoopInfo
{
public:
	virtual ~INoopInfo()  {}
	virtual tBOOL Enabled() const = 0;		// возвращает true, если поддерживается Noop
	virtual tDWORD Timeout() const = 0;		// возвращает отрезок времени (в секундах?) через который следует посылать Noop-данные
	virtual tDWORD GetNoopData(tBYTE* pDest, tDWORD data_size) const = 0;	
				// позволяет получить Noop-данные
				// вовращает 0, если Noop не поддерживается
				// если pDest != NULL, возвращает количество записанных байт, либо необходимое количество байт, если не хватило места
				// если pDest == NULL, возвращает необходимое количество байт для данных
};

//////////////////////////////////////////////////////////////////////////
class IProtocolAnalyzer
{
public:
	virtual ~IProtocolAnalyzer()  {}
	virtual detect_code_t Detect(data_source_t src, const tBYTE* data, tDWORD data_size) = 0;
	virtual tERROR EnqueueData(data_source_t src, const tBYTE* data, tDWORD data_size) = 0;
	virtual tBOOL HasContent() const = 0;
	virtual tERROR ExtractContent(IContentItem** ppContent) = 0;
	virtual tERROR AssembleContent(IContentItem* pContent) const = 0;
	virtual tERROR ReleaseContent(IContentItem* pContent) const = 0;
//	virtual analysis_state_t State() const = 0;
//	virtual const INoopInfo& ClientNoopInfo() const = 0;
//	virtual const INoopInfo& ServerNoopInfo() const = 0;
};

}

#endif  // !defined(__KL_IPROTOCOLANALYZER_H)
