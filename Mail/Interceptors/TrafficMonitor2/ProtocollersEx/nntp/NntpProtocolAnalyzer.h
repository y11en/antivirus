/*
*/

#if !defined(__KL_NNTPPROTOCOLANALYZER_H)
#define __KL_NNTPPROTOCOLANALYZER_H

#include "../common/ProtocolAnalyzerImplAdapter.h"
#include "NntpProtocolAnalyzerImpl.h"

namespace TrafficProtocoller
{

typedef Util::ProtocolAnalyzerImplAdapter<NntpProtocolAnalyzerImpl> NntpProtocolAnalyzer;

}  // namespace TrafficProtocoller

#endif   // !defined(__KL_NNTPPROTOCOLANALYZER_H)
