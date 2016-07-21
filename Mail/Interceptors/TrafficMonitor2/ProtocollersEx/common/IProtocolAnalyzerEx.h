/*
	\brief Интерфейс, аналогичный IProtocolAnalyzer, но построенный на основе C++ & STL
*/

#if !defined(__KL_IPROTOCOLANALYZEREX_H)
#define __KL_IPROTOCOLANALYZEREX_H

#include "../interfaces/IProtocolAnalyzer.h"
#include <memory>

namespace TrafficProtocoller
{

//////////////////////////////////////////////////////////////////////////
class IProtocolAnalyzerEx
{
public:
	virtual ~IProtocolAnalyzerEx()  {}
	virtual detect_code_t Detect(data_source_t src, const char* data, size_t data_size) = 0;
	virtual void EnqueueData(data_source_t src, const char* data, size_t data_size) = 0;
	virtual bool HasContent() const = 0;
	virtual std::auto_ptr<IContentItem> ExtractContent() = 0;
	virtual void AssembleContent(IContentItem& content) const = 0;
};

}  // namespace TrafficProtocoller

#endif  // !defined(__KL_IPROTOCOLANALYZEREX_H)
